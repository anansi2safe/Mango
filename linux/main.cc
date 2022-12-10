#include "NetworkEpoll.h"
using namespace Mango;

void ss(void* buffer, size_t buffer_size, CLIENT_CONTEXT ctx){
    char s[] = "HTTP/1.0 200 OK\n"
                "Server: SimpleHTTP/0.6 Python/3.10.6\n"
                "Date: Fri, 09 Dec 2022 08:35:52 GMT\n"
                "Content-type: text/html\n"
                "Content-Length: 16\n"
                "Last-Modified: Fri, 09 Dec 2022 08:31:32 GMT\n\n"
                "<!DOCTYPE html>";
                
    int l = send(ctx.fd, s, sizeof(s), 0);
    close(ctx.fd);
}

int main(int argc, char* argv[]) {
    NetworkEpoll* net = new NetworkEpoll("0.0.0.0", 8000, 9000, true);
    net->Initialize();
    net->CreateServer();
    net->EpollLoop(ss);
    return 0;
}