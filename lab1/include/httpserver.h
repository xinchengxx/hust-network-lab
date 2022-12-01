#ifndef WEBSERVER_HTTP_SERVER_H
#define WEBSERVER_HTTP_SERVER_H
namespace webserver {
  class HttpServer {
  public:
      HttpServer(int port, const char* addr): port_(port), addr_(addr) {}

      [[noreturn]] void run();

  private:
      int port_;
      const char* addr_;
  };
};
#endif