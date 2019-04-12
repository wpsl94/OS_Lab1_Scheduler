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
#define SIZE 5		// process size

// struct of task
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// 그래프 그리기 위한 함수의 인자 전역변수 선언
int arnum = 0;
char arr[20] = { 0 };

// 프로세스 구조체 (pid, arvTime, sevTime, runned_time)
typedef struct Process
{
    char pid;
    int arvTime;
    int sevTime;
    int runned_time;
}Process;

// 체인노드 구조체 (data, link)
typedef struct ChainNode
{
    Process proc;
    struct ChainNode* link;
}ChainNode;

// 큐 구조체 (front, rear, count)
typedef struct Queue
{
    ChainNode *front;
    ChainNode *rear;
    
    int count;
}Queue;

Process InitProcess(char, int, int);
void InitQueue(Queue*);
int IsEmpty(Queue*);
void push(Queue* , Process);
void pop(Queue);
void Finish(Queue* , Queue* , Process);
void Run(Queue*, Process );
void clear_arr(char* );
void Q_graph(char* arr1);
void input_proc(Process, Queue *, int ;
void MLFQ(Queue*, Process, Process, Process, Process, Process);
void MLFQ_2(Queue*, Process, Process, Process, Process, Process , int , int , int );
int get_randomnum(int );
int get_total_sevT(Process, Process, Process, Process, Process);
int get_ticket(Process, int , int );
int add(int* , int , int );
void Lottery(Queue *, Process, Process, Process, Process, Process);



#endif /* LAB1_HEADER_H*/



