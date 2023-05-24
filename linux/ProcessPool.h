#ifndef PROCESSPOOL_H_
#define PROCESSPOOL_H_

#include "../base.h"
#include <signal.h>
#include <list>
#include <unordered_map>

namespace Mango{

typedef struct _MG_PIPE{
    int read_pipe;
    int write_pipe;
}MG_PIPE, *PMG_PIPE;

typedef struct _ARGUMENTS{
    size_t count;
    void* ptr;
    MG_PIPE pipe_fd;
}ARGUMENTS, *PARGUMENTS;

typedef int (*PFUNC)(void*);

class ProcessPool{
public:
    explicit ProcessPool()
        : process_count(0){};

    ~ProcessPool();

    inline size_t GetProcessCount(){
        return this->process_count;
    }
    
    u_int16_t CreateProcess(PFUNC pFunc, PARGUMENTS args);
    u_int16_t CreateProcess(PFUNC pFunc, PARGUMENTS args, 
                            u_int32_t stack_size);
private:
    size_t process_count;
    std::list<pid_t> pids;
    std::unordered_map<u_int32_t, MG_PIPE> pipe_list;
    std::unordered_map<u_int32_t, u_char*> stack_list;
};
}

#endif