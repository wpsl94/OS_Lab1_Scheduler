/*
*	DKU Operating System Lab
*	    Lab1 (Scheduler Algorithm Simulator)
*	    Student id : 32141444
*	    Student name : 김희주
*
*   lab1_sched.c :
*       - Lab1 source file.
*       - Must contains scueduler algorithm function'definition.
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
 * you need to implement FCFS, RR, SPN, SRT, HRRN, MLFQ scheduler. 
 */


void sched(){
	fifo();
	rr(1);
	rr(4);
	mlfq();
    mlfq_2();
	lottery();
}




// queue pop
struct task_t *q_pop(){
	struct task_t *task_top = queue[qt];
	queue[qt] = NULL;
	if(++qt >= SIZE) qt = 0;
	return task_top;
}

// queue put
void q_put(struct task_t* task_one){
	queue[ql] = task_one;
	if(++ql >= SIZE) ql = 0;
}

// print queue
void print_queue(){
	int i=0;
    printf("\n");
	for(;i<SIZE;i++){
		if(queue[i] == NULL) continue;
		printf("i:%d,name:%c,svc:%d,prt:%d\n",i,queue[i]->name,queue[i]->svc,queue[i]->prt);
	}
	printf("top:%d, last:%d\n",qt,ql);
	printf("\n");
}

// task array setting
void taskSet(){
	char name[] = "abcde\0";
	int arv[] = {0,2,4,6,8},
		svc[] = {3,6,4,5,2},
		i;
	qt = ql = 0;
	for(i=0;i<SIZE;i++){
		queue[i] = NULL;
		task[i].name = name[i];
		task[i].arv = arv[i];
		task[i].svc = svc[i];
		task[i].prt = 0;
		task[i].tat = -1; 
		task[i].rst = -1;
		task[i].tk  = svc[i]*100;
	}

}

// task array print
void taskPrint(){
	int i;
	for(i=0;i<SIZE;i++){
		printf("name : %c, arrival Time : %d, service Time : %d, priority : %d\n",task[i].name,task[i].arv,task[i].svc,task[i].prt);
	}
}


// scheduling table print
void print_table(char arr[]){
	int i=0,j,asize=0;
	for(;i<SIZE;i++){
		printf("%c ",task[i].name);
		for(j=0;j<20;j++){
			if(task[i].name == arr[j]){
				printf("■ ");
			} else {
				printf("□ ");
			}
		}
		printf("\n");
	}
}

// print average value
void print_performance(){
	int sum_tat=0, sum_rst=0,
		i=0;
	float avg_tat, avg_rst;
	for(;i<SIZE;i++){
		sum_tat += task[i].tat;
		sum_rst += task[i].rst;
        printf("%c : ",task[i].name);
		printf("Turnaround Time = %2d  ",task[i].tat);
		printf("Response Time = %2d\n",task[i].rst);
	}
	avg_tat = (float)sum_tat/SIZE;
	avg_rst = (float)sum_rst/SIZE;
	printf("turnaround = ");
    printf("sum  %2d, avg %.2f\n",sum_tat,avg_tat);
	printf("reponse =    ");
	printf("sum %2d, avg %.2f\n",sum_rst,avg_rst);
}

void fifo(){
	taskSet();		// task variable initialize
	taskPrint();		// task status print
	char tn[] = "First in First out\0",	// task name
		 in[20];		// for scheduling print
	int killed_count = 0,		// killed task count
		svc_t = 0,			// for service time increase
		next = 0;			// next task index
	struct task_t *now= &task[next++];	 // now task
	printf("\nstart \t %s\n",tn);
	printf("   ");
	while(killed_count < SIZE){
		// arrival time same service time than next task put queue
		while(next<SIZE && task[next].arv == svc_t){
			q_put(&task[next]);
			next++;
		}
		printf("%c ",now->name);	// now process name print
		in[svc_t] = now->name;	// store present process name put for print table
		if(now && now->rst == -1)	// response time check
			now->rst = svc_t - now->arv;
		svc_t++;		// service time increament
		if(--now->svc <= 0){	// task kill and next task pop
			killed_count++;
			now->tat = svc_t - now->arv;
			now = q_pop();
		}
	}
	printf("\n");
	print_table(in);		// print task scheduling table
	print_performance();	// print scheduling performance ( turnaround time, response time )
	printf("end \t %s\n\n",tn);
}

void rr(int max_sched_cnt){
	taskSet();  // task initialize
	char tn[30] = "Round Robin \0",	// task name
		  max[1],	// for round robin scheduling conut number
		  in[20];	// string for scheduling result print 
	int kill_count = 0,	// task killed count
		svc_t = 0,	// service time
		next = 0,		// next task index
		sched_cnt = 0; // schedule count
	struct task_t *now = &task[next++]; // now task
	sprintf(max,"%d",max_sched_cnt);
	strcat(tn,max);
	printf("\nstart \t %s\n",tn);
	printf("   ");
	while(kill_count < SIZE){
		printf("%c ",now->name);
		in[svc_t] = now->name;
		if(now && now->rst == -1) // response time check
			now->rst = svc_t - now->arv;
		svc_t++;

		while(next<SIZE && task[next].arv == svc_t){
			q_put(&task[next++]);		// put next task
		}
		if(--now->svc <= 0){
			kill_count++; // kill task
			now->tat = svc_t - now->arv; // turnaround time check
			sched_cnt = 0; // initialize scheduling time
			now = q_pop();
		} else {
			if(++sched_cnt >= max_sched_cnt){
				sched_cnt = 0; // initialize scheduling time
				q_put(now);
				now = q_pop();
			}
		}
	}
	printf("\n");
	print_table(in);	// print scheduling result table
	print_performance(); // print scheduling performance
	printf("end \t %s\n\n",tn);
}

// sort by priority
void sortByPrt(){
	int now = ql-1,prev;
	struct task_t *temp;
	if(now < 0) now = SIZE-1;

	// order by priority 
	while(1){
		prev = now-1;
		if(prev < 0) now = SIZE-1;
		if(!queue[prev] || queue[prev]->prt == queue[now]->prt || prev == qt) break;
		if(queue[prev]->prt > queue[now]->prt){
			temp = queue[now];
			queue[now] = queue[prev];
			queue[prev] = temp;
		}
		now = prev;
	}

}

/* multi level feedback queue {
 */
void mlfq(){
	taskSet();
	char tn[] = "Multi Level Feedback Queue(q=1)\0",
		 in[20];
	int i = 0,
		kill_count = 0,
		svc_t = 0,
		next = 0;
	struct task_t *now = &task[next++];
	printf("\nstart \t %s\n",tn);
	printf("  ");
	q_put(now);
	while(kill_count < SIZE){
		if(next<SIZE && task[next].arv <= svc_t){
			now = &task[next++];
		} else {
			now = q_pop();
		}
		printf("%c ",now->name);
		if(now && now->rst == -1) now->rst = svc_t - now->arv;
		in[svc_t++] = now->name;
		now->prt++;
		if(--now->svc <= 0){
			kill_count++;
			now->tat = svc_t - now->arv;
		} else {
			q_put(now);
			sortByPrt();
		}
	}
	printf("\n");
	print_table(in);
	print_performance();
	printf("end \t %s\n\n",tn);
}


void mlfq_2(){
    taskSet();
    char tn[] = "Multi Level Feedback Queue(q=2^i)\0",
    in[20];
    int i = 0,
    kill_count = 0,
    svc_t = 0,
    next = 0;
    struct task_t *now = &task[next++];
    printf("\nstart \t %s\n",tn);
    printf("  ");
    q_put(now);
    while(kill_count < SIZE){
        if(next<SIZE && task[next].arv <= svc_t){
            now = &task[next++];
        } else {
            now = q_pop();
        }
        printf("%c ",now->name);
        if(now && now->rst == -1) now->rst = svc_t - now->arv;
        in[svc_t*2] = now->name;
        now->prt++;
        if(--now->svc <= 0){
            kill_count++;
            now->tat = svc_t - now->arv;
        } else {
            q_put(now);
            sortByPrt();
        }
    }
    printf("\n");
    print_table(in);
    print_performance();
    printf("end \t %s\n\n",tn);
}
/*
 MLFQ scheduling finished }*/

void lottery(){
	char tn[] = "Lottery\0",
		 in[20];
	int i = 0,
		kill_count = 0,
		arv_cnt=1,
		svc_t=0,
		max, stack_tk, win_number;
	taskSet();
	printf("\nstart \t %s\n",tn);
	srandom(time(NULL));
	max = task[i].tk;
	printf("  ");
	while(kill_count < SIZE){
		svc_t++;
		win_number = random() % max;
		for(i=stack_tk=0;i<arv_cnt;i++){
			if(task[i].svc>0) stack_tk += task[i].tk;
			if(win_number < stack_tk){
				if(task[i].svc == 0) continue;
				printf("%c ",task[i].name);
				if(task[i].rst == -1)
					task[i].rst = svc_t - task[i].arv -1;
				in[svc_t-1] = task[i].name;
				if(task[i].rst == -1)
					task[i].rst == svc_t - task[i].arv;
				if(--task[i].svc<=0){
					kill_count++;
					max -= task[i].tk;
					task[i].tat = svc_t - task[i].arv;
				}
				break;
			}
		}
		for(i=arv_cnt;i<SIZE;i++){
			if(task[i].arv <= svc_t){
				max += task[i].tk;
				arv_cnt++;
			}
		}

	}
	printf("\n");
	print_table(in);
	print_performance();
	printf("end \t %s\n\n",tn);
}


