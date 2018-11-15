#ifndef _DEFINES_H
#define _DEFINES_H

#include <vector>
#include <string>

#define DASH '-'
#define EQUAL '='
#define WS ' '
#define DOT '.'
#define COLON ':'
#define WORKER_FILENAME "worker"
#define PRESENTER_FILENAME "presenter"
#define WORKER_HEADER "--W--" // to distinguish it's a fresh message from worker
#define PRESENTER_HEADER "--P--" // it's a message from presenter (not used)
#define LOADBALANCER_HEADER "--LB--" // it's a message from load balancer
#define NEWLINE '\n'
#define SLASH '/'
#define QUIT "quit"
#define PRC_CNT "prc_cnt"
#define DIRECTORY "dir"
#define ASCEND "ascend"
#define DESCEND "descend"
#define EMPTY_STR ""
#define FIFO_TEMP_PATH "/tmp/osca2"

typedef std::pair<std::string, std::string> Pair;

#endif