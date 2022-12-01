#ifndef WEBSERVER_HTTP_H
#define WEBSERVER_HTTP_H

#include <string>
#include <unordered_map>
namespace webserver {

    class Request {
    public:
        std::string method, path, version;
        std::unordered_map<std::string, std::string> header;
        std::string body;
    };

    class Response {
    public:
        std::string version, code, message;
        std::unordered_map<std::string, std::string> header;
        std::string build();
        Response() {
            this->version = "HTTP/1.0";
            this->header["Connection"] = "close";
            this->header["Server"] = "A-very-cool-webserver";
        }
    };
};

#endif //WEBSERVER_HTTP_H
