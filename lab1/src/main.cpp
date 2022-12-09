#include <cstdlib>
#include <string>
#include "define.h"
#include "httpserver.h"

int main(int argc, char* argv[]) {
    int port;
    const char* addr;
   if (argc == 1) {
       port = webserver::kDefaultPort;
       addr = webserver::kaddr;
   } else if (argc == 2) {
       port = webserver::kDefaultPort;
       addr = argv[1];
   } else if (argc == 3) {
       addr = argv[1];
       port = atoi(argv[2]);
   } else {
       printf("choose the format that \"./webserver [addr] [port]\n");
       return 0;
   }

   webserver::HttpServer server(port, addr);
   server.run();
   return 0;
}