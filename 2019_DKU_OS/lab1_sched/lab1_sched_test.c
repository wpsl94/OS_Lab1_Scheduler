/*
*	DKU Operating System Lab
*	    Lab1 (Scheduler Algorithm Simulator)
*	    Student id : 32141444
*	    Student name : 김희주
*
*   lab1_sched.c :
*       - Lab1 source file.
*       - Must contains scueduler algorithm test code.
*
*/

#include <aio.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <pthread.h>
#include <asm/unistd.h>

#include "lab1_sched_types.h"

/*
 * you need to implement scheduler simlator test code.
 *
 */

int main()
{
    Queue myqueue;
    
    // 큐 초기화
    InitQueue(&myqueue);
    
    // 프로세스 초기값 설정
    Process a = InitProcess('A', 0, 3);
    Process b = InitProcess('B', 2, 6);
    Process c = InitProcess('C', 4, 4);
    Process d = InitProcess('D', 6, 5);
    Process e = InitProcess('E', 8, 2);
    
    // MLFQ
    MLFQ(&myqueue, a, b, c, d, e);
    
    // MLFQ - Ext
    MLFQ(&myqueue, a, b, c, d, e, 1, 2, 4);
    
    // 로터리
    Lottery(&myqueue, a, b, c, d, e);
    
    return 0;
}
