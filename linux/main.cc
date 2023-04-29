#include "NetworkEpoll.h"
using namespace Mango;

int ss(void* buffer, size_t buffer_size, CLIENT_CONTEXT ctx){
    char s[] = "HTTP/1.0 200 OK\n"
                "Server: RedDragon/1.0.0 (Ubuntu)\n"
                "Date: Fri, 09 Dec 2022 08:35:52 GMT\n"
                "Content-type: text/html\n"
                "Content-Length: 16\n"
                "Last-Modified: Fri, 09 Dec 2022 08:31:32 GMT\n\n"
                "<!DOCTYPE html>\n"
                "<html>\n"
                "\t<head>\n"
                "\t\t<meta charset=\"utf-8\">\n"
                "\t\t<title>llllll</title>\n"
                "\t</head>\n"
                "\t<body>\n"
                "\t\t<p>111111111</p>\n"
                "\t</body>\n"
                "</html>\n";
                
    int l = send(ctx.fd, s, sizeof(s), 0);
    return 0;
}

void err(int s, CLIENT_CONTEXT ctx){

}

int main(int argc, char* argv[]) {
    NetworkEpoll* net = new NetworkEpoll("0.0.0.0", 8000, 9000, true);
    net->Initialize();
    net->SetKeepAlive(false);
    net->CreateEpollServer();
    net->EpollLoop(ss, err);
    return 0;
}