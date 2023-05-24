#include "NetworkEpoll.h"

Mango::NetworkEpoll::~NetworkEpoll() {
    close(this->epoll_fd);
    close(this->socket_fd);
    this->ac.clear();
    FREE_MEM(this->epoll_events);
    FREE_MEM(this->buffer);
}

void Mango::NetworkEpoll::Initialize() {
    size_t max_user_watches = 0;
    int file_fd = open(
        this->epoll_filepath.c_str(), O_RDONLY);
    DCHECK_EQ(file_fd, -1);
    char buff[255];
    size_t o =read(file_fd, buff, 255);
    this->max_user_watches = strtoul(buff, NULL, 10);
    close(file_fd);
    // create epoll
    this->epoll_fd = CreateEpoll();
    this->epoll_events = (epoll_event*)malloc(
        this->max_event * sizeof(epoll_event));
    DCHECK_EQ(this->epoll_fd, -1);
}

int Mango::NetworkEpoll::CreateSockServer() {
    int fd = 0;
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, 
            this->GetIp().c_str(),
            &addr.sin_addr);
    addr.sin_port = htons(this->GetPort());

    fd = socket(AF_INET, SOCK_STREAM, 0);
    DCHECK_EQ(fd, -1);
    // 设置非阻塞描述符
    int opt = 1;
    int r = fcntl(fd, F_SETFL, 
                fcntl(fd, 
                    F_GETFL, NULL) | O_NONBLOCK);
    DCHECK_EQ(r, -1);

    // 设置允许在端口释放后立即被再次使用
    if(setsockopt(fd, 
        SOL_SOCKET, 
        SO_REUSEADDR, 
        &opt, sizeof(opt))) {
            //异常中断
            EXCEPTION("setsockopt error");
            BREAK();
    } 
	// 设置允许完全的重复绑定
    if(setsockopt(fd, 
        SOL_SOCKET, 
        SO_REUSEPORT, 
        &opt, sizeof(opt))) {
            //异常中断
            EXCEPTION("setsockopt error");
            BREAK();
    }

    int bind_fd = bind(fd, 
                (struct sockaddr*)&addr,
                sizeof(addr));
    DCHECK_EQ(bind_fd, -1);
    bind_fd = listen(fd, 5);
    DCHECK_EQ(bind_fd, -1);
    return fd;
}

void Mango::NetworkEpoll::CreateEpollServer() {
    DCHECK_EQ(this->epoll_fd, -1);
    this->socket_fd = CreateSockServer();
    addfd(this->socket_fd);
}

void Mango::NetworkEpoll::addfd (int fd) {
    int op = EPOLL_CTL_ADD;
    // 判断是否是已在事件列表中
    epoll_event event;
    event.data.fd = fd;
    // epoll_flg为真是ET模式
    if(this->epoll_flg)
        event.events = EPOLLIN | EPOLLOUT | EPOLLET ;
    else
        event.events = EPOLLIN | EPOLLOUT ;

    if(this->ac.find(fd) != this->ac.end()){
        op = EPOLL_CTL_MOD;
    }else{
        this->ac[fd] = true;
    }
    // add fd
    int r = epoll_ctl(this->epoll_fd, 
                op, fd, &event);
    DCHECK_EQ(r, -1);
}

void Mango::NetworkEpoll::delfd (int fd) {
    if(this->ac.find(fd) != this->ac.end()){
        this->ac.erase(fd);
    }
     int r = epoll_ctl(this->epoll_fd, 
                EPOLL_CTL_DEL, fd, NULL);
    DCHECK_EQ(r, -1);
    close(fd);
}

void Mango::NetworkEpoll::epoll_accept() {
    struct sockaddr_in cl_addr;
    socklen_t addr_size = sizeof(cl_addr);
    while(true) {
        int client_fd = sock_accept(this->socket_fd, &cl_addr, &addr_size);
        // 如果client_fd为-1且errno为EAGAIN才代表连接接收完毕
        if((client_fd == -1) && (errno == EAGAIN))
            break;
        struct timeval time_o = {this->cli_timeo, 0};
            
        SETSOCKOPT(client_fd, time_o);
        addfd(client_fd);
    }
}

void Mango::NetworkEpoll::EpollLoop(
    Mango::EPOLL_CALLBACK call,
    Mango::EPOLL_CALLBACK_ERR call_err) {
        this->buffer = (MBYTE*)malloc(this->max_buffsize);
        while (true) {
            int ret = epoll_wait(
                this->epoll_fd, 
                this->epoll_events, 
                this->max_event, 
                -1);
                
            DCHECK_LE(ret, 0);
            for(size_t i = 0; i < ret; i++) {
                if(this->epoll_events[i].data.fd == this->socket_fd) {
                    // 侦听socket，接收新连接
                    // TODO: 此处可模仿nginx做一个负载均衡，是否有必要加入？
                    epoll_accept();
                    continue;
                } else if(this->epoll_events[i].events & EPOLLERR) {
                    // retry
                    continue;
                } else if(this->epoll_events[i].events & EPOLLIN) {
                    // 数据读事件
                    DCHECK(!buffer);
                    memset(buffer, '\0', this->max_buffsize);
                    size_t m = recv(
                        this->epoll_events[i].data.fd, 
                        buffer, 
                        this->max_buffsize, 0);
                    
                    if(m < 0) {
                        // 出错
                        delfd(this->epoll_events[i].data.fd);
                    } else if(m == 0){
                        delfd(this->epoll_events[i].data.fd);
                    }else {
                        // 数据接收成功
                        // 获取客户端ip与端口
                        struct sockaddr_in cli_addr;
                        socklen_t cli_len = sizeof(cli_addr);
                        getpeername(this->epoll_events[i].data.fd, 
                                    (struct sockaddr*)&cli_addr,
                                    &cli_len);
                        char ip_str[64];
                        inet_ntop(AF_INET, &cli_addr.sin_addr, ip_str, 64);
                        CLIENT_CONTEXT ctx;
                        ctx.port = htons(cli_addr.sin_port);
                        ctx.ip = ip_str;
                        ctx.fd = this->epoll_events[i].data.fd;
                        // 回调函数如果卡住将会卡住整个epoll循环体,导致服务端卡死
                        int status_code = call(buffer, this->max_buffsize, ctx);
                        if(status_code){
                            delfd(ctx.fd);
                            if(call_err)
                                call_err(status_code, ctx);
                        }
                        if(!this->keepalive)
                            delfd(ctx.fd);
                    }
                }
            }
        }
        FREE_MEM(this->buffer);
}