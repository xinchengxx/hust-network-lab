#ifndef WEBSERVER_HANDLE_H
#define WEBSERVER_HANDLE_H

#include <memory>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>


#include "http.h"

namespace webserver {
    class Connection {
    public:
        std::string read_;
        std::string write_;
        size_t header_size_;
        size_t body_size_;
        size_t read_ptr_;
        size_t write_ptr_;
        std::unique_ptr<Request> req_;
        Connection(): header_size_(0), body_size_(0), read_ptr_(0), write_ptr_(0), req_(nullptr) {}
    };
    class Handler {
    public:

        static void PrintClientInfo(sockaddr *client_addr, socklen_t client_addr_len);
        static void Read(int conn_fd, std::shared_ptr<Connection> conn);
        static void Write(int conn_fd, std::shared_ptr<Connection> conn);

    private:
        static void ParseAll(std::shared_ptr<Connection> conn, bool init, int conn_fd);

        static bool ReadInternal(int conn_fd, std::shared_ptr<Connection> conn);
        static bool WriteInternal(int conn_fd, std::shared_ptr<Connection> conn);
    };
};

#endif //WEBSERVER_HANDLE_H
