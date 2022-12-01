#include <sys/stat.h>

#include <errno.h>
#include <optional>
#include <cstring>

#include "handle.h"
#include "define.h"
#include "util.h"

namespace webserver {
  bool Handler::ReadInternal(int conn_fd, std::shared_ptr<Connection> conn) {
      char buf[8196];
      int n = 0;

      std::string &read_storage = conn->read_;
      while ((n = read(conn_fd, buf, sizeof(buf))) > 0) {
          printf("[%d] received %d", conn_fd, n);
          read_storage.append(buf, n);

          if (conn->req_ == nullptr) {
              const int pos = read_storage.find("\r\n\r\n");
              if (pos != std::string::npos) {
                  conn->header_size_ = pos + 4;

                  ParseHeader(conn, conn_fd);
              }
          }

          if (conn->req_ != nullptr &&
              conn->header_size_ + conn->body_size_ == read_storage.size()) {
              ParseAll(conn, false, conn_fd);

              return Handler::WriteInternal(conn_fd, conn);
          }
      }

      if (n < 0) {
          printf("[%d] %d -> %s signal received", conn_fd, errno,
                   ::strerror(errno));
      }

      if (n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
          return false;
      }

      return true;
  }

  void Handler::Read(int conn_fd, std::shared_ptr<Connection> conn) {
      bool exec_result = Handler::ReadInternal(conn_fd, conn);

      if (exec_result) {
          close(conn_fd);

          printf("[%d] disposed\n", conn_fd);
      }
  }

  bool Handler::WriteInternal(int conn_fd, std::shared_ptr<Connection> conn) {

      if (conn->req_ == nullptr && conn->read_.empty()) {
          printf("1");
          return false;
      }
      if (conn->req_ == nullptr) {
          Handler::ParseAll(conn, true, conn_fd);
      }

      // modify to offset
      int n = 0, remain = conn->write_.size() - conn->write_ptr_;
      while ((n = write(conn_fd, conn->write_.c_str() + conn->write_ptr_, remain)) >
             0) {
          printf("[%d] written %d\n", conn_fd, n);
          conn->write_ptr_ += n;
          remain -= n;
      }
      if (conn->write_.size() == conn->write_ptr_) {
          printf("[%d] file sent completed\n", conn_fd);
          return true;
      }

      if (n < 0) {
          printf("[%d] remain %d resources, %d -> %s signal received\n", conn_fd,
                   remain, errno, ::strerror(errno));
      }
      if (n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
          return false;
      }
      return true;
  }

  void Handler::Write(int conn_fd, std::shared_ptr<Connection> conn) {
      bool exec_result = Handler::WriteInternal(conn_fd, conn);
      if (exec_result) {
          close(conn_fd);
          printf("[%d] disposed\n", conn_fd);
      }
  }


  void Handler::ParseAll(std::shared_ptr<Connection> conn, bool init, int conn_fd) {

      if (init) {
          conn->req_ = std::unique_ptr<Request>(new Request());
          ParseHeader(conn, conn_fd);
      }

      ParseBody(conn, conn_fd);


      conn->read_.clear();
      conn->read_ptr_ = 0;
      printf("[%d] Request parsed\n", conn_fd);

      const auto &http_request = conn->req_;
      Response http_response{};
      if (http_request->method != "GET") {
          http_response.code = "405";
          http_response.message = "Method not allowed";

          conn->write_ = http_response.build();
          return;
      }

      const std::string file_path = std::string(kWorkingDir) + http_request->path;
      printf("the file_path is %s\n", file_path.c_str());
      std::optional<std::string> file_extension =
              ReadFileExtension(http_request->path);
      struct stat file_stat {};
      FILE *fp = fopen(file_path.c_str(), "r");

      if (file_extension == std::nullopt ||
          !FileExist(file_path, file_stat) || fp == nullptr) {
          http_response.code = "404";
          http_response.message = "Not found";
          conn->write_ = http_response.build();
          return;
      }
      http_response.code = "200";
      http_response.message = "OK";
      http_response.header["Content-type"] = GetFileType(file_extension.value());
      http_response.header["Content-length"] = std::to_string(file_stat.st_size);

      conn->write_ = http_response.build();
      int file_block_size = 0;
      char buffer[8192];
      while ((file_block_size = fread(buffer, sizeof(char), 8192, fp)) > 0) {
          conn->write_.append(buffer, file_block_size);
      }
      fclose(fp);
      printf("File read completed");
  }
    void Handler::PrintClientInfo(sockaddr *client_addr, socklen_t client_addr_len) {
        char host_name[MAX_BUF], host_port[MAX_BUF];

        memset(host_name, 0, sizeof(host_name));
        memset(host_port, 0, sizeof(host_port));

        getnameinfo(client_addr, client_addr_len, host_name, sizeof(host_name),
                    host_port, sizeof(host_port), 0);

        printf("Received Request from: %s, port: %s\n", host_name, host_port);
    }
};