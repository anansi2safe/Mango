#ifndef NETWORKEPOLLET_H_
#define NETWORKEPOLLET_H_
#endif

#include "NetworkBase.h"

namespace Mango{
typedef struct CLIENT_CONTEXT_ {
    std::string ip;
    unsigned short port;
    int fd;
}CLIENT_CONTEXT, *PCLIENT_CONTEXT;

// 回调函数，要注意的是尽量避免在该函数中处理过于复杂甚至是
// 会导致死循环的逻辑事件，因为该函数是阻塞执行
typedef void (*EPOLL_CALLBACK)(void* buffer, 
            size_t buffer_size, CLIENT_CONTEXT ctx);

class NetworkEpollET : public Mango::NetworkBase{
public:
    explicit NetworkEpollET()
        :   epoll_filepath(MAX_USER_WATCHES),
            max_user_watches(0),
            socket_fd(-1),
            epoll_fd(-1),
            server_(false),
            client_(false),
            max_event(1024),
            epoll_events(nullptr),
            buffer(nullptr),
            max_buffsize(MAX_BUFF_SIZE)
        {};
    explicit NetworkEpollET(std::string ip, 
        unsigned short port) 
        :   NetworkBase(ip, port),
            epoll_filepath(MAX_USER_WATCHES),
            max_user_watches(0),
            socket_fd(-1),
            epoll_fd(-1),
            server_(false),
            client_(false),
            max_event(1024),
            epoll_events(nullptr),
            buffer(nullptr),
            max_buffsize(MAX_BUFF_SIZE)
        {};
    explicit NetworkEpollET(std::string ip, 
        unsigned short port,
        size_t max_event) 
        :   NetworkBase(ip, port),
            epoll_filepath(MAX_USER_WATCHES),
            max_user_watches(0),
            socket_fd(-1),
            epoll_fd(-1),
            server_(false),
            client_(false),
            max_event(max_event),
            epoll_events(nullptr),
            buffer(nullptr),
            max_buffsize(MAX_BUFF_SIZE)
        {};
    ~NetworkEpollET();
    inline size_t GetMaxUserWatches() { 
        return this->max_user_watches;
    }

    inline bool IsServer(){
        return this->server_;
    }

    inline bool IsClient(){
        return this->client_;
    }

    inline size_t GetMaxEvent(){
        return this->max_event;
    }

    void SetBuffSize(size_t s){
        this->max_buffsize = s;
    }

    size_t GetBuffSize(){
        return this->max_buffsize;
    }

    void Initialize();
    void CreateServer();
    void EpollLoop(EPOLL_CALLBACK call);
    
private:
    void addfd(int fd, bool flg);
    void delfd(int fd);
    void epoll_accept();


    bool server_;
    bool client_;
    std::string epoll_filepath;
    size_t max_user_watches;
    size_t max_event;
    size_t max_buffsize;
    int socket_fd;
    int epoll_fd;
    MBYTE* buffer;
    epoll_event* epoll_events;
};
}