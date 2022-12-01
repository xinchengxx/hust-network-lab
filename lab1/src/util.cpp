#include "util.h"
#include "fcntl.h"

#include <unordered_set>
#include <sstream>
namespace webserver {
    std::string ReadlineFromConnection(std::shared_ptr<Connection> conn) {
        std::string result;
        char last_buf = '\0', buf = '\0';

        while (ReadOneCharFromConnection(buf, conn) > 0) {
            if (last_buf == '\r' && buf == '\n') {
                result.pop_back();
                break;
            }
            if (buf == '\0') {
                break;
            }
            result += buf;
            last_buf = buf;
        }
        return result;
    }
    void ParseHeader(std::shared_ptr<Connection> conn, int conn_fd) {
        conn->req_ = std::unique_ptr<Request>(new Request());
        ReadHttpFirstLine(conn, conn_fd);

        std::string buffer;
        while (!(buffer = ReadlineFromConnection(conn)).empty()) {
            const size_t line_length = buffer.size();
            std::string key, value;
            size_t ptr = 0;
            while (ptr < line_length) {
                if (buffer[ptr] == ':') {
                    break;
                }
                ptr++;
            }

            key = buffer.substr(0, ptr);
            value = buffer.substr(ptr + 2);

            conn->req_->header[key] = value;
        }

        if (conn->req_->header.count("Content-length")) {
            conn->body_size_ = stol(conn->req_->header["Content-length"]);
        } else if (conn->req_->header.count("content-length")) {
            conn->body_size_ = stol(conn->req_->header["content-length"]);
        } else if (conn->req_->header.count("Content-Length")) {
            conn->body_size_ = stol(conn->req_->header["Content-Length"]);
        } else {
            conn->body_size_ = 0;
        }

        printf("[%d] header parse invoked, body_size = %lu\n", conn_fd,
                 conn->body_size_);
    }
    void ParseBody(std::shared_ptr<Connection> conn, int conn_fd) {
        conn->req_->body = conn->read_.substr(conn->read_ptr_);
        printf("[%d] body parse invoked\n", conn_fd);
    }
    std::optional<std::string> ReadFileExtension(const std::string &url) {
        int index = url.find('.');
        if (index == std::string::npos) {
            return std::nullopt;
        }

        std::string result;
        const size_t url_size = url.size();
        for (int i = 0; i < url_size; i++) {
            if (url[url_size - i - 1] == '.')
                break;
            result.insert(result.begin(), url[url_size - i - 1]);
        }
        return result;
    }
    std::string GetFileType(const std::string &file_extensions) {
        const std::unordered_set<std::string> image_type_collection = {"jpg", "jpeg", "png",
                                                             "gif", "bmp"};
        if (image_type_collection.count(file_extensions)) {
            return "image/" + file_extensions;
        } else if (file_extensions == "html") {
            return "text/html";
        } else {
            return "text/plain";
        }
    }
    bool FileExist(const std::string &file_name, struct stat &file_stat) {
        return stat(file_name.c_str(), &file_stat) >= 0;
    }
    void SetNonBlock(const int fd) {
        int flags = fcntl(fd, F_GETFL, 0);
        if (flags < 0) {
            printf("[%d] set non-block io failed", fd);
            return;
        }
        fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    }
    void ReadHttpFirstLine(std::shared_ptr<Connection> conn, int conn_fd) {
        std::stringstream ss;

        ss << ReadlineFromConnection(conn);
        ss >> conn->req_->method >> conn->req_->path >> conn->req_->version;

        printf("[%d] %s %s %s\n", conn_fd, conn->req_->method.c_str(),
               conn->req_->path.c_str(), conn->req_->version.c_str());
    }
    int ReadOneCharFromConnection(char &buff, std::shared_ptr<Connection> conn) {
        if (conn->read_ptr_ == conn->read_.size()) {
            return 0;
        }
        buff = conn->read_[conn->read_ptr_];
        conn->read_ptr_++;
        return 1;
    }
};