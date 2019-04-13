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

struct task_t{
	char name;	// process name
	int tat,	// turnarround time
		rst,	// response time
		arv,	// Arrival time
		svc,	// Service time
		run,	// Runned time
		prt,	// Priority
		tk;		// ticket
};


struct task_t	*queue[SIZE],	// queue
		task[SIZE];	// task
int qt,	// queue top index
    ql;	// queue last index



Process InitProcess(char , int , int ); 
void InitQueue(struct Queue *);
int IsEmpty(struct Queue *);					// queue empty check
void push(struct Queue *, struct Process);
void pop(struct Queue *);
void Finish(struct Queue *, struct Queue *, struct Process);
void Run(struct Queue *, struct Process *);
void clear_arr(char* );
void Q_graph(char* );
void input_proc(struct Process , struct Queue *, int );
void MLFQ(struct Queue *, struct Process , struct Process , struct Process , struct Process, struct Process);
void MLFQ_EXT(struct Queue *, struct Process , struct Process , struct Process , struct Process , struct Process , int , int , int );
struct task_t *q_pop();			// queue pop
void q_put(struct task_t *);		// queue put
void taskSet();					// task array setting
void taskPrint();				// task array  print
void print_queue();				// queue print
void print_table(char *);		// shceduling table print
void print_performance();		// print scheduling performance
/* } default functions */

/* scheduling functions { */
void fifo();		//first in first out
void rr(int);	// round robine
void mlfq_exe();  //execute mlfq
void mlfq();		// Multi level feedback queue(q=1)
void mlfq_2();        //Multi level feedback queue(q=2^i)

/* } scheduling functions */


#endif /* LAB1_HEADER_H*/



