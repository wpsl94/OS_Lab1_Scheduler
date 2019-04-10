/*
*	DKU Operating System Lab
*	    Lab1 (Scheduler Algorithm Simulator)
*	    Student id : 32141444
*	    Student name : 김희주
*
*   lab1_sched_types.h :
*       - lab1 header file.
*       - must contains scueduler algorithm function's declations.
*
*/

#ifndef _LAB1_HEADER_H
#define _LAB1_HEADER_H


/*
 * You need to Declare functions in  here
 */

#define SIZE 5        // process size
#define PROCESS_COUNT 5

// struct of task
struct task_t{
    char name;    // process name
    int tat,    // turnarround time
    rst,    // response time
    arv,    // Arrival time
    svc,    // Service time
    prt,    // Priority
    tk;        // ticket
};

/* global variable { */
struct task_t    *queue[SIZE],    // queue
task[SIZE];        // task
int qt,        // queue top index
ql;        // queue last index
/* } global variable */

/* default functions {
 will be implement in set.c */
struct task_t *q_pop();            // queue pop
void q_put(struct task_t *);        // queue put
void taskSet();                    // task array setting
void taskPrint();                // task array  print
void startLog(char *);            // start log print
void endLog(char *);                // end log print
void endl();                        // enter character
void print_queue();                // queue print
void print_table(char *);        // shceduling table print
void print_performance();        // print scheduling performance
/* } default functions */

/* scheduling functions { */
void fifo();        // implement at fifo.c    first in first out
void mlfq();
void rr(int);    // implement at rr.c        round robine
void lottery();    // implement at lottery.c
/* } scheduling functions */



typedef struct process{
    int arriveTime; // 프로세스 도착 시간
    int serviceTime; // 프로세스 요구 수행 시간
    int currentServiceTime; // 실제 프로세스 수행 시간
    int accumulatedTime; // MLFQ에 사용될 누적시간
    int processId; // processId
    int ticket;    //for rottery scheduler
    int priority; // MLFQ에서 사용될 우선순위
}Process;

typedef struct node{
	Process *process; // 프로세스 데이터
	struct node *next; // 다음 노드
}Node;

typedef struct queue{
	Node *front; // 맨 앞(프로세스 꺼낼 위치)
	Node *rear; // 맨 뒤(보관할 위치)
	int count; // 보관 개수
	int timeSlice; //MLFQ에서 큐의 타임슬라이스
	int priority; // MLFQ에서 큐의 우선순위
}Queue;

/* default functions {*/
void initQueue(Queue *queue); // 큐 초기화
void setQueueTimeSlice(Queue *queue, int timeSlice); // 큐 타임슬라이스 설정
bool isEmpty(Queue *queue); // 큐 공백 확인
void enQueueFront(Queue *queue, Process *process); // 큐 front에 프로세스 삽입
void enQueueRear(Queue *queue, Process *process); // 큐 rear에 프러세스 삽입
Process * deQueue(Queue *queue); // 큐에서 프로세스 삭제
void * deQueueInLottery(Queue *queue, Node *pa, Node *ch); //Lottery의 큐에서 프로세스 삭제
void showWorkLoad(int totalRunningTime, bool **workLoad); // workLoad 출력함수
bool ** createWorkLoad(int totalRunningTime, bool **workLoad); 
void showMenu(void);
void initComponent(Process *processSet, bool **workLoad, int totalServiceTime);
/* } default functions */

/* scheduling functions { */
void firstInFirstOut(Process *process, bool **workLoad, int totalServiceTime); // FIFO 스케쥴러 함수
void multilevelFeedbackQueue(Process *process, bool **workLoad, int totalServiceTime, int timeSlice, bool exp); //MLFQ 스케쥴러 함수
void RR(Process *processSet, int totalServiceTime, bool **workload, int timeSlice);
void Lottery(Process *processSet, int totalServiceTime, bool **workload);
/* } scheduling functions */


#endif /* LAB1_HEADER_H*/
