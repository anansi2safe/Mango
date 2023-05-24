#ifndef NetworkEpoll_H_
#define NetworkEpoll_H_

#include "NetworkBase.h"


namespace Mango{
typedef struct CLIENT_CONTEXT_ {
    std::string ip;
    unsigned short port;
    int fd;
}CLIENT_CONTEXT, *PCLIENT_CONTEXT;

#define SETSOCKOPT(client_fd, time_o) do {          \
    int r = fcntl(client_fd, F_SETFL,               \
                fcntl(client_fd,                    \
                    F_GETFL, NULL) | O_NONBLOCK);   \
    DCHECK_EQ(r, -1);                               \
    if(setsockopt(client_fd,                        \
        SOL_SOCKET,                                 \
        SO_RCVTIMEO,                                \
        &time_o, sizeof(time_o))) {                 \
            EXCEPTION("setsockopt error");          \
            BREAK();                                \
    }                                               \
    if(setsockopt(client_fd,                        \
        SOL_SOCKET,                                 \
        SO_SNDTIMEO,                                \
        &time_o, sizeof(time_o))) {                 \
            EXCEPTION("setsockopt error");          \
            BREAK();                                \
    }                                               \
}while(false)

// 回调函数，要注意的是尽量避免在该函数中处理过于复杂甚至是
// 会导致死循环的逻辑事件，因为该函数是阻塞执行，此回调函数
// 最终会返回一个整数，该整数可以用于自定义状态码，0为执行成功
typedef int (*EPOLL_CALLBACK)(void* buffer, 
            size_t buffer_size, CLIENT_CONTEXT ctx);
// 错误码处理回调函数
typedef void (*EPOLL_CALLBACK_ERR)(int status_code, 
                                    CLIENT_CONTEXT ctx);


class NetworkEpoll : public Mango::NetworkBase{
public:
    explicit NetworkEpoll()
        :   epoll_filepath(MAX_USER_WATCHES),
            max_user_watches(0),
            socket_fd(-1),
            epoll_fd(-1),
            max_event(1024),
            epoll_events(nullptr),
            buffer(nullptr),
            epoll_flg(false),
            cli_timeo(5),
            keepalive(true),
            max_buffsize(MAX_BUFF_SIZE)
        {};
    explicit NetworkEpoll(std::string ip, 
        unsigned short port) 
        :   NetworkBase(ip, port),
            epoll_filepath(MAX_USER_WATCHES),
            max_user_watches(0),
            socket_fd(-1),
            epoll_fd(-1),
            max_event(1024),
            epoll_events(nullptr),
            buffer(nullptr),
            epoll_flg(false),
            cli_timeo(5),
            keepalive(true),
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
            max_event(max_event),
            epoll_events(nullptr),
            buffer(nullptr),
            epoll_flg(false),
            cli_timeo(5),
            keepalive(true),
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
            max_event(max_event),
            epoll_events(nullptr),
            buffer(nullptr),
            cli_timeo(5),
            epoll_flg(flg),
            keepalive(true),
            max_buffsize(MAX_BUFF_SIZE)
        {};
        
    ~NetworkEpoll();
    
    inline size_t GetMaxUserWatches() { 
        return this->max_user_watches;
    }

    // 获取最大事件数
    inline size_t GetMaxEvent(){
        return this->max_event;
    }

    // 设置是否开启ET模式，默认LT模式
    inline void SetEpollFlg(bool flg){
        this->epoll_flg = flg;
    }

    // 判断是否是ET模式，否则LT模式
    inline bool IsET(){
        return this->epoll_flg;
    }

    // 客户端超时秒数
    inline void SetCliTimeout(int sec){
        this->cli_timeo = sec;
    }

    // 是否保持长连接
    inline void SetKeepAlive(bool k){
        this->keepalive = k;
    }

    // 获取epoll fd
    inline int GetEpollFd(){
        return this->epoll_fd;
    }

    // 獲取socket fd
    inline int GetSockFd(){
        return this->socket_fd;
    }

    // 接收缓存区大小
    inline void SetBuffSize(size_t s){
        this->max_buffsize = s;
    }

    // 获取接收缓存区大小
    inline size_t GetBuffSize(){
        return this->max_buffsize;
    }

    inline std::map<int, bool> GetActiveClient(){
        return this->ac;
    }

    inline int sock_accept(int sock_fd, 
                            sockaddr_in* cl_addr,
                            socklen_t* addr_size){
        return accept(
            sock_fd, 
            (struct sockaddr*)cl_addr, 
            addr_size);
    }

    inline int CreateEpoll(){
        return epoll_create1(0);
    }
   
    void Initialize();
    void addfd(int fd);
    void delfd(int fd);
    void epoll_accept();
    int  CreateSockServer();
    void CreateEpollServer();
    void EpollLoop(EPOLL_CALLBACK call, 
                    EPOLL_CALLBACK_ERR call_err = nullptr);
    
private:
    size_t max_user_watches;
    size_t max_event;
    size_t max_buffsize;
    bool epoll_flg;
    bool keepalive;
    std::string epoll_filepath;
    std::map<int, bool> ac;
    int socket_fd;
    int epoll_fd;
    int cli_timeo;
    MBYTE* buffer;
    epoll_event* epoll_events;
};
}

#endif