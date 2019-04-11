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



//start log, endlog, enter 삭제함 - 영재
//taskprint, print_table,print_performance 수정함- 영재
//print_queue 사용하지 않는 것 같아 삭제함- 영재

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
        printf("[%c] \n Arrival Time : %d, service Time : %d",task[i].name,task[i].arv,task[i].svc);
    }
}


// scheduling table print
void print_table(char arr[]){
    int i=0,j;
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
    }
    avg_tat = (float)sum_tat/SIZE;
    avg_rst = (float)sum_rst/SIZE;
    
    printf("Average Turnaround Time : %.2f\n",avg_tat);
    printf("Average Response Time : %.2f\n",avg_rst);
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
    char tn[30] = "Round Robin \0",
    in[20];    // 스케줄링의 결과를 출력할 때 사용할 문자열
    
    int kill_count = 0,    // task killed count
    svc_t = 0,    // 한 task의 수행해야 할 양을 마친 시간
    next = 0,        // 다음 task의 인덱스
    sched_cnt = 0; // schedule 실행 수
    
    struct task_t *now = &task[next++]; // now task
    printf("Round Robin \n");
    printf("q = %d\n",max_sched_cnt); // 타임퀀텀을 출력한다
    
    
    while(kill_count < SIZE){
        
        in[svc_t] = now->name; // 나중에 in[]의 값을 take.[i]와 대조해서 테이블에 ■,□출력하는데 사용한다.
        
        if(now && now->rst == -1) // response time을 확인하기 위한 if문!
            now->rst = svc_t - now->arv; //실행 시간에서 도착한 시간을 빼서, response time의 값을 구한다.
        svc_t++;
        
        while(next<SIZE && task[next].arv == svc_t){
            q_put(&task[next++]);        // 다음 task로 넘어간다. 만약 ql 인덱스가 SIZE보다 커지면 0으로 바꿔준다 (q_put)
        }
        if(--now->svc <= 0){
            kill_count++; // kill task
            now->tat = svc_t - now->arv; // turnaround time 을 구한다
            sched_cnt = 0; // task가 한차례에 얼마나 수행했는지 알려주는 실행 수를 초기화 시켜준다
            now = q_pop();
        } else {
            if(++sched_cnt >= max_sched_cnt){
                sched_cnt = 0; // 타임퀀텀 만큼 수행하면 초기화 시키고 순서를 넘긴다.
                q_put(now);
                now = q_pop();
            }
        }
    }
    printf("\n");
    print_table(in);    // print scheduling result table
    print_performance(); // print scheduling performance
    printf("end RR %d \n",max_sched_cnt);
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


