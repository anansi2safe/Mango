#ifndef NETWORKBASE_H_
#define NETWORKBASE_H_
#endif

#include "../base.h"
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <map>

namespace Mango{
class NetworkBase {
public:
    explicit NetworkBase()
        :   ip("0.0.0.0"),
            port(0){}
    explicit NetworkBase(std::string ip, 
        unsigned short port) 
        :   ip(ip),
            port(port){}
        
    void SetIp(std::string ip) {
        this->ip = ip;
    }

    void SetPort(unsigned short port) {
        this->port = port;
    }

    inline std::string GetIp() {
        return this->ip;
    }

    inline unsigned short GetPort() {
        return this->port;
    }

private:
    std::string ip;
    unsigned short port;
};
}