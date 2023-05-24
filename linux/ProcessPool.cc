#include "ProcessPool.h"

u_int16_t Mango::ProcessPool::CreateProcess(PFUNC pFunc, 
    PARGUMENTS args, u_int32_t stack_size){
    u_char* stack_mem = (u_char*)calloc(1, stack_size);
    pid_t pid = clone(pFunc, stack_mem+stack_size, SIGCHLD, args, NULL);
}