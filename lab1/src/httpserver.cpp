#include <arpa/inet.h>
#include <csignal>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "httpserver.h"
#include "util.h"
#include "handle.h"

namespace webserver {
    void HttpServer::run() {
        int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
        sockaddr_in server_addr{};
        server_addr.sin_port = htons(port_);
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = inet_addr(addr_);
        bind(listen_fd, (struct sockaddr *)(&server_addr),
             sizeof(server_addr));
        listen(listen_fd, 5);
        SetNonBlock(listen_fd);
        while(true) {
            sockaddr_in client_addr{};
            socklen_t client_addr_len = sizeof(client_addr);

            int conn_fd = accept(listen_fd, (struct sockaddr *)(&client_addr), &client_addr_len);
            if (conn_fd != -1) {
                printf("[%d] Connection Established\n", conn_fd);

                SetNonBlock(conn_fd);
                Handler::PrintClientInfo((struct sockaddr *)(&client_addr), client_addr_len);
                std::shared_ptr<Connection> conn_ptr(new Connection());
                Handler::Read(conn_fd, conn_ptr);
                // Handler::Write(conn_fd, conn_ptr);
                close(conn_fd);
            }
        }
    }
};