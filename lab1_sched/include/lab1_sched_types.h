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
struct task_t{
	char name;	// process name
	int tat,	// turnarround time
		rst,	// response time
		arv,	// Arrival time
		svc,	// Service time
		prt,	// Priority
		tk;		// ticket
};

/* global variable { */
struct task_t	*queue[SIZE],	// queue
		task[SIZE];		// task
int qt,		// queue top index
	ql;		// queue last index
/* } global variable */

/* default functions {
	 will be implement in set.c */
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
void mlfq();		// Multi level feedback queue(q=1)
void mlfq_2();        //Multi level feedback queue(q=2^i)
void lottery();	//lottery
/* } scheduling functions */


#endif /* LAB1_HEADER_H*/



