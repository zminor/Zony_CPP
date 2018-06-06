#ifndef DBG_H
#define DGB_H

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#ifdef NDEBUG
#define debug(M,...)
#else 
#define debug(M,...) fprintf(stderr,"DEBUG: %s:%d "M" \n",__FILE__,__LINE__,##__VA_ARGS__)
#endif

#define clean_errno() (errno ==0? "None":strerror(errno))

#define log_err(M,...) fprintf(stderr, "[ERROR](%s : %d: errno: %s) "M"\n  ",__FILE__,__LINE__,clean_errno(),##__VA_ARGS__)
#endif
