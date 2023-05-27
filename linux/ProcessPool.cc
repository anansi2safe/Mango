#include "ProcessPool.h"

pid_t Mango::ProcessPool::AppendProcess(PFUNC pFunc, 
    PARGUMENTS args){
#if defined(__linux__) || defined(__APPLE__)
    u_char* stack_mem = (u_char*)ALLOC(1, stack_size);
    if(!stack_mem)
        EXCEPTION("memory allocate failed!");
    pid_t pid = CREATE_PROCESS(pFunc, 
        stack_mem+stack_size, 
        SIGCHLD|CLONE_NEWPID, 
        args, NULL);

    if(pid == -1)
        EXCEPTION("child process create failed!");
    
    if(pid > 0){
        MG_PIPE  pip;
        int pip_arr[2] = {0, 0};
        if(CREATE_PIPE(pip_arr) == -1){
            EXCEPTION("pipe create failed!");
        }else{
            pip.read_pipe = pip_arr[0];
            pip.write_pipe = pip_arr[1];
            pids.emplace_back(pid);
            pipe_list.emplace(pid, pip);
            stack_list.emplace(pid, stack_mem);
            process_count = pids.size();
        }
    }
    return pid;
#elif defined(_WIN32) || defined(_WIN64)
#endif
}

bool Mango::ProcessPool::DeleteProcess(pid_t pid){
    DCHECK_LE(0, pid);
    DCHECK_NEQ(stack_list.at(pid), nullptr);
#if defined(__linux__) || defined(__APPLE__)
    if(!CLOSE_PROCESS(pid, 0)){
#elif defined(_WIN32) || defined(_WIN64)
    HANDLE hProcess = OpenProcess(
        PROCESS_TERMINATE, FALSE, pid);
        
    if(!CLOSE_PROCESS(hProcess, 0)){
#endif
        EXCEPTION("close process failed!");
    }else{
        pids.remove(pid);
        close(pipe_list.at(pid).read_pipe);
        close(pipe_list.at(pid).write_pipe);
        FREE_MEM(stack_list.at(pid));
        process_count = pids.size();
    }
}