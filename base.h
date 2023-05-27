#ifndef BASE_H
#define BASE_H

#include <assert.h>
#include <iostream>

#define ALLOC calloc

#define FREE_MEM(p) do{ \
    if(p){              \
        free(p);        \
    }                   \
    p = nullptr;        \
}while(0);

#define OS_ABORT abort()
#define EXCEPTION(m) throw(m)
#if _DEBUG_
#define DCHECK_IMPL(b) assert(b)
// 当b为true时触发异常断点
#define DCHECK(b) DCHECK_IMPL(!(b))
#define DCHECK_EQ(a,b) DCHECK(a == b)
#define DCHECK_NEQ(a,b) DCHECK(a != b)
#define DCHECK_LE(a,b) DCHECK(a < b)
#define BREAK() assert(false);
#else
#define DCHECK(b)
#define DCHECK_EQ(a,b)
#define DCHECK_NEQ(a,b)
#define DCHECK_LE(a,b)
#define BREAK()
#endif

#define MBYTE unsigned char

#define MAX_USER_WATCHES "/proc/sys/fs/epoll/max_user_watches"
#define FILE_OPEN_FAILED 0xF
#define MAX_BUFF_SIZE 0xFFFF

#endif