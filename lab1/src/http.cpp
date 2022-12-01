#include "http.h"

namespace webserver {
    std::string Response::build() {
        std::string response_header =
                this->version + " " + this->code + " " + this->message + "\r\n";
        for (const std::pair<const std::string, std::string> &one_pair : this->header) {
            response_header += one_pair.first + ": " + one_pair.second + "\r\n";
        }
        response_header += "\r\n";
        return response_header;
    }

}