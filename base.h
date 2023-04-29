#ifndef BASE_H
#define BASE_H
#endif

#include <assert.h>

#define EXCEPTION(m) throw(m)
#define FREE_MEM(m) if(m){free(m);}
#if _DEBUG_
#define DCHECK_IMPL(b) assert(b)
// 当b为true时触发异常断点
#define DCHECK(b) DCHECK_IMPL(!(b))
#define DCHECK_EQ(a,b) DCHECK(a == b)
#define DCHECK_LE(a,b) DCHECK(a < b)
#define BREAK() assert(false);
#else
#define DCHECK(b)
#define DCHECK_EQ(a,b)
#define DCHECK_LE(a,b)
#define BREAK()
#endif

#define MBYTE unsigned char

#define MAX_USER_WATCHES "/proc/sys/fs/epoll/max_user_watches"
#define FILE_OPEN_FAILED 0xF
#define MAX_BUFF_SIZE 0xFFFF
