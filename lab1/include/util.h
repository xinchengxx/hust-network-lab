#ifndef WEBSERVER_UTIL_H
#define WEBSERVER_UTIL_H

#include <sys/stat.h>

#include <memory>
#include <optional>

#include "handle.h"
namespace webserver {
    std::string ReadlineFromConnection(std::shared_ptr<Connection> conn);
    void ParseHeader(std::shared_ptr<Connection> conn, int conn_fd);
    void ParseBody(std::shared_ptr<Connection> conn, int conn_fd);
    std::optional<std::string> ReadFileExtension(const std::string &url);
    std::string GetFileType(const std::string &file_extensions);
    bool FileExist(const std::string &file_name, struct stat &file_stat);
    void SetNonBlock(const int fd);
    void ReadHttpFirstLine(std::shared_ptr<Connection> conn, int conn_fd);
    int ReadOneCharFromConnection(char &buff, std::shared_ptr<Connection> conn);
};

#endif //WEBSERVER_UTIL_H
