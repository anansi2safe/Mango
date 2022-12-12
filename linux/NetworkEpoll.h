#ifndef NetworkEpoll_H_
#define NetworkEpoll_H_
#endif

#include "NetworkBase.h"

namespace Mango{
typedef struct CLIENT_CONTEXT_ {
    std::string ip;
    unsigned short port;
    int fd;
}CLIENT_CONTEXT, *PCLIENT_CONTEXT;

typedef struct AC_CONTEXT_ {
    bool state;
    int mask;
}AC_CONTEXT, *PAC_CONTEXT;

// 回调函数，要注意的是尽量避免在该函数中处理过于复杂甚至是
// 会导致死循环的逻辑事件，因为该函数是阻塞执行
typedef void (*EPOLL_CALLBACK)(void* buffer, 
            size_t buffer_size, CLIENT_CONTEXT ctx);

class NetworkEpoll : public Mango::NetworkBase{
public:
    explicit NetworkEpoll()
        :   epoll_filepath(MAX_USER_WATCHES),
            max_user_watches(0),
            socket_fd(-1),
            epoll_fd(-1),
            server_(false),
            client_(false),
            max_event(1024),
            actives_(new AC_CONTEXT[1024]()),
            epoll_events(nullptr),
            buffer(nullptr),
            epoll_flg(false),
            max_buffsize(MAX_BUFF_SIZE)
        {};
    explicit NetworkEpoll(std::string ip, 
        unsigned short port) 
        :   NetworkBase(ip, port),
            epoll_filepath(MAX_USER_WATCHES),
            max_user_watches(0),
            socket_fd(-1),
            epoll_fd(-1),
            server_(false),
            client_(false),
            max_event(1024),
            actives_(new AC_CONTEXT[1024]()),
            epoll_events(nullptr),
            buffer(nullptr),
            epoll_flg(false),
            max_buffsize(MAX_BUFF_SIZE)
        {};
    explicit NetworkEpoll(std::string ip, 
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
            actives_(new AC_CONTEXT[max_event]()),
            epoll_events(nullptr),
            buffer(nullptr),
            epoll_flg(false),
            max_buffsize(MAX_BUFF_SIZE)
        {};
    explicit NetworkEpoll(std::string ip, 
        unsigned short port,
        size_t max_event,
        bool flg) 
        :   NetworkBase(ip, port),
            epoll_filepath(MAX_USER_WATCHES),
            max_user_watches(0),
            socket_fd(-1),
            epoll_fd(-1),
            server_(false),
            client_(false),
            max_event(max_event),
            actives_(new AC_CONTEXT[max_event]()),
            epoll_events(nullptr),
            buffer(nullptr),
            epoll_flg(flg),
            max_buffsize(MAX_BUFF_SIZE)
        {};
    ~NetworkEpoll();
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

    inline void SetEpollFlg(bool flg){
        this->epoll_flg = flg;
    }

    inline bool IsET(){
        return this->epoll_flg;
    }

    void SetBuffSize(size_t s){
        this->max_buffsize = s;
    }

    size_t GetBuffSize(){
        return this->max_buffsize;
    }

    static void CloseSocketFD(int &fd){
        if(fd != -1){
            close(fd);
            fd = -1;
        }
    }

    bool Is_Active(int fd){
        return this->actives_[fd].state;
    }

    void Initialize();
    void CreateServer();
    void EpollLoop(EPOLL_CALLBACK call);
    
private:
    void addfd(int fd, bool flg);
    void delfd(int fd);
    void epoll_accept();

    size_t max_user_watches;
    size_t max_event;
    size_t max_buffsize;
    AC_CONTEXT* actives_;
    bool epoll_flg;
    bool server_;
    bool client_;
    std::string epoll_filepath;
    int socket_fd;
    int epoll_fd;
    MBYTE* buffer;
    epoll_event* epoll_events;
};
}