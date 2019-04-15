/*
*	DKU Operating System Lab
*	    Lab1 (Scheduler Algorithm Simulator)
*	    Student id :    32141444     32144777   32140182
*	    Student name :  김희주         최영재      윤찬영
*
*   lab1_sched_types.h :
*       - lab1 header file.
*       - must contains scueduler algorithm function's declations.
*
*/

#ifndef _LAB1_HEADER_H
#define _LAB1_HEADER_H
#define SIZE 5		// process size for RR


/*
 *
 * RR
 *
 */
/////////Structure for Process/////////
struct task_info{
	char name;	
	int turnaround_t, response_t, arrive_t,	service_t, run_t, priority, ticket;		
};

/////////Structure for Queue/////////
struct task_info	*queue[SIZE],	
		task[SIZE];	
int queue_top,	
    queue_last;	


/*
 *
 * MLFQ
 *
 */
/////////for draw table/////////
int array_num = 0;
char arr[20] = { 0 };


/////////Structure for Process/////////
typedef struct Process
{
    char name;
    
    int arrive_t;
    int service_t;
    int runned_time;
    
}Process;

/////////Structure for ChainNode/////////
typedef struct ChainNode
{
    Process proc;
    struct ChainNode* link;
}ChainNode;

/////////Structure for Queue/////////
typedef struct Queue
{
    ChainNode *front;
    ChainNode *rear;
    
    int count;
}Queue;


/*
 *
 * FIFO, LOTTERY
 *
 */
/////////Structure for Process/////////
typedef struct y_task{
	char process;
	int arrive_t,
		service_t,
		response_t,
		turnarround_t,
		priority,
		ticket;
}y_task;
/////////for draw table/////////
y_task ytask[5];

char queue_y[5];
int seg;

/*    STRUCTURES & GLOBAL VAR    */

/* MLFQ FUNCTION */
Process Setting(char , int , int );
void InitQueue(struct Queue *);
int IsEmpty(struct Queue *);
void push(struct Queue *, struct Process);
void pop(struct Queue *);
void proc_end(struct Queue *, struct Queue *, struct Process);
void proc_start(struct Queue *, struct Process *);
void clear_job(char* );
void showTable(char* );
void put_in(struct Process , struct Queue *, int );
void MLFQ(struct Queue *, struct Process , struct Process , struct Process , struct Process, struct Process);
void MLFQ_2(struct Queue *, struct Process , struct Process , struct Process , struct Process , struct Process , int , int , int );

/* FIFO & LOTTERY FUNCTION */
void set_display(int img[5][20]);
void set_task();
void queue_clean(char , int );

/* RR FUNCTION */
struct task_info *queue_pop();
void queue_put(struct task_info *);		
void tasklist();				
void print_scheduling_table(char *);		
void performance_result();	



void fifo();	    // first in first out
void rr(int);	    // round robine
void mlfq_exe();    // mlfq
void lottery();	    // lottery
/* SCHEDULING FUCNTION */


#endif /* LAB1_HEADER_H*/



