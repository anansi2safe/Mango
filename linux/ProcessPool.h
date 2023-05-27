#ifndef PROCESSPOOL_H_
#define PROCESSPOOL_H_

#include "../base.h"
#include <signal.h>
#include <list>
#include <unordered_map>

namespace Mango{

#if defined(__linux__) || defined(__APPLE__)
#define CREATE_PROCESS clone
#define CREATE_PIPE pipe
#define CLOSE_PROCESS kill

#elif defined(_WIN32) || defined(_WIN64)
#define CREATE_PROCESS CreateProcess
#define CREATE_PIPE CreatePipe
#define CLOSE_PROCESS TerminateProcess

#endif

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
        : process_count(0),
          stack_size(4096){};

    ~ProcessPool();

    pid_t AppendProcess(PFUNC pFunc, PARGUMENTS args);

    bool DeleteProcess(pid_t pid);

    inline size_t GetProcessCount(){
        return this->process_count;
    }

    inline void SetStackSize(size_t stack_size){
        this->stack_size = stack_size;
    }

    inline size_t GetStackSize(){
        return this->stack_size;
    }

    inline std::list<pid_t> 
    GetPids(){return this->pids;}

    inline std::unordered_map<pid_t, MG_PIPE> 
    GetPipeList(){return this->pipe_list;}
    
    inline std::unordered_map<pid_t, u_char*>
    GetStackList(){return this->stack_list;}
private:
    size_t stack_size;
    size_t process_count;
    std::list<pid_t> pids;
    std::unordered_map<pid_t, MG_PIPE> pipe_list;
    std::unordered_map<pid_t, u_char*> stack_list;
};
}

#endif