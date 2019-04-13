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
#include <math.h>
#include <asm/unistd.h>
#include "lab1_sched_types.h"

/*
 * you need to implement FCFS, RR, SPN, SRT, HRRN, MLFQ scheduler. 
 */


void sched(){
	fifo();
	rr(1);
	rr(4);
	mlfq_exe();
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


// 프로세스 초기화하는 함수
Process InitProcess(char pid, int at, int st)
{
	Process p;

	p.pid = pid;
	p.arvTime = at;
	p.sevTime = st;
	p.runned_time = 0;

	return p;
}

// 큐 초기화하는 함수
void InitQueue(Queue *myqueue)
{
	myqueue->front = NULL;
	myqueue->rear = NULL;
	myqueue->count = 0;
	srand((unsigned)time(NULL));
}

// 큐 비어있는지 검사하는 함수(비어있으면 1, 아니면 0)
int IsEmpty(Queue *myqueue)
{
	if (myqueue->front == NULL)
		return 1;
	else
		return 0;
}

// 삽입 함수
void push(Queue *myqueue, Process myprocess)
{
	// ChainNode 동적 할당 및 초기화
	ChainNode *newChain = (ChainNode *)malloc(sizeof(ChainNode));
	newChain->proc = myprocess;
	newChain->link = NULL;

	// ChainNode 붙이기
	if (IsEmpty(myqueue)) { // 큐가 비었을 경우
		myqueue->front = newChain;
	}
	else {
		myqueue->rear->link = newChain;
	}
	myqueue->rear = newChain;
	myqueue->count++;
}

// 삭제 함수
void pop(Queue *myqueue)
{
	int num = 0;
	ChainNode *newChain;

	if (IsEmpty(myqueue) == 1) // 비어있으면 삭제하지 않음
		myqueue->front = myqueue->rear = NULL;
	else {
		newChain = myqueue->front;
		myqueue->front = newChain->link;
		free(newChain);
	}
	myqueue->count--;
	return;
}

// 프로세스 종료시 호출되는 함수
void Finish(Queue *myqueue1, Queue *myqueue2, Process p)
{
	push(myqueue2, p);
	pop(myqueue1);
}

// 프로세스 실행시 호출되는 함수
void Run(Queue *myqueue, Process *p)
{
	p->runned_time++;
	arr[arnum] = p->pid;
	arnum++;
}

// 전역변수 초기화 함수
void clear_arr(char* arr)
{
	for (int i = 0; i < 20; i++)
	{
		arr[i] = 0;
	}
	arnum = 0;
}


// 그래프 그리는 함수
void Q_graph(char* arr1)
{
	int i;

	printf("A ");
	for (i = 0; i < 20; i++)
	{
		if (arr1[i] == 'A')
		{
			printf("■ ");
		}
		else
			printf("□ ");
	}

	printf("\n");
	printf("B ");
	for (i = 0; i < 20; i++)
	{
		if (arr1[i] == 'B')
		{
			printf("■ ");
		}
		else
			printf("□ ");
	}

	printf("\n");
	printf("C ");
	for (i = 0; i < 20; i++)
	{
		if (arr1[i] == 'C')
		{
			printf("■ ");
		}
		else
			printf("□ ");
	}

	printf("\n");
	printf("D ");
	for (i = 0; i < 20; i++)
	{
		if (arr1[i] == 'D')
		{
			printf("■ ");
		}
		else
			printf("□ ");
	}

	printf("\n");
	printf("E ");
	for (i = 0; i < 20; i++)
	{
		if (arr1[i] == 'E')
		{
			printf("■ ");
		}
		else
			printf("□ ");
	}
	printf("\n");
	clear_arr(arr);
}


// 프로세스 입력하는 함수
void input_proc(Process A, Queue *myqueue, int runtime)
{
	if (A.arvTime == runtime) {
		push(myqueue, A);
	}
}

// MLFQ 스케쥴링
void MLFQ(Queue *myqueue1, Process A, Process B, Process C, Process D, Process E) {

	// queue의 갯수가 3개, time quantum이 1인 MLFQ입니다.
	// Priority : myqueue1 > myqueue2 > myqueue3

	printf("\nMLFQ(q=1) 스케쥴링 입니다.\n\n");

	int runtime = 0;

	Queue temp_queue;
	Queue myqueue2;
	Queue myqueue3;

	InitQueue(&temp_queue);
	InitQueue(&myqueue2);
	InitQueue(&myqueue3);

	// 프로세스 삽입
	input_proc(A, myqueue1, runtime);
	input_proc(B, myqueue1, runtime);
	input_proc(C, myqueue1, runtime);
	input_proc(D, myqueue1, runtime);
	input_proc(E, myqueue1, runtime);

	while (1) {
		
		if (runtime > 19) { // while문 탈출조건
			break;
		}

		// 프로세스 입력 -> 실행 -> 첫 번째인 경우 원래 큐에 유지, 그 밖에는 밑의 큐로 내려감
		// -> 새로 입력되는 프로세스가 우선 큐에 삽입 -> 서비스타임이 다 되었을 경우 Finish
		// 상위 큐에 프로세스가 있을 경우 quantum이 다 되지 않아도 상위 큐 프로세스 실행
		// 상위 큐에 프로세스가 없을 경우 재 실행

		// 실행

		if (IsEmpty(myqueue1) == 0) { // 가장 상위 큐가 myqueue1이면

			Run(myqueue1, &myqueue1->front->proc); // 프로세스 실행

			if (myqueue1->front->proc.sevTime == myqueue1->front->proc.runned_time) {// 서비스타임 다 되면 없애고
				Finish(myqueue1, &temp_queue, myqueue1->front->proc);
			}
			else { // 서비스타임이 다 안되면 실행


				if (myqueue1->count == 1 && myqueue2.count == 0 && myqueue3.count == 0) { // 자신 말고 다른 프로세스가 없을 경우 현재 큐 유지
					
					// 실행된 프로세스가 큐에 삽입되기 전에 삽입 예정인 프로세스가 있는지 확인하여 큐에 추가
					input_proc(A, myqueue1, runtime + 1);
					input_proc(B, myqueue1, runtime + 1);
					input_proc(C, myqueue1, runtime + 1);
					input_proc(D, myqueue1, runtime + 1);
					input_proc(E, myqueue1, runtime + 1);

					Finish(myqueue1, myqueue1, myqueue1->front->proc);
					runtime++;
					continue;
				}

				else { // 자신 말고 다른 프로세스가 있을 경우 밑의 큐로 내려감
					Finish(myqueue1, &myqueue2, myqueue1->front->proc);
				}
			}

		}

		else if (IsEmpty(&myqueue2) == 0) { // 가장 상위 큐가 myqueue2이면

			if (myqueue2.front->proc.sevTime == myqueue2.front->proc.runned_time) { // 서비스타임 다 되면 없애고
				Finish(&myqueue2, &temp_queue, myqueue2.front->proc);
			}

			Run(&myqueue2, &myqueue2.front->proc); // 프로세스 실행
			Finish(&myqueue2, &myqueue3, myqueue2.front->proc); // 밑의 큐로 내려감
		}

		else if (IsEmpty(&myqueue3) == 0) { // 가장 상위 큐가 myqueue3이면

			if (myqueue3.front->proc.sevTime == myqueue3.front->proc.runned_time) { // 서비스타임 다 되면 없애고
				Finish(&myqueue3, &temp_queue, myqueue3.front->proc);
			}
			Run(&myqueue3, &myqueue3.front->proc); // 프로세스 실행
			Finish(&myqueue3, &myqueue3, myqueue3.front->proc); // 마지막 큐 이므로 RR
		}
		else if (IsEmpty(myqueue1) == 1 && IsEmpty(&myqueue2) == 1 && IsEmpty(&myqueue3) == 1) { // 세 개의 큐 내에 프로세스가 존재하지 않을 경우
			arr[arnum] = 'O'; // 프로세스가 없음을 뜻하고
			arnum++;		  // 다음 배열 인덱스로 이동해 입력 대기
		}
		runtime++;
		
		// 프로세스 삽입
		input_proc(A, myqueue1, runtime);
		input_proc(B, myqueue1, runtime);
		input_proc(C, myqueue1, runtime);
		input_proc(D, myqueue1, runtime);
		input_proc(E, myqueue1, runtime);
	}
	Q_graph(arr); // 그래프 그리기
}

// MLFQ-Ext 스케쥴링
void MLFQ_EXT(Queue *myqueue1, Process A, Process B, Process C, Process D, Process E, int quantum_1, int quantum_2, int quantum_3) {

	// queue의 갯수가 3개, 각 큐마다 time quantum 지정 가능한 MLFQ입니다.
	// Priority : myqueue1 > myqueue2 > myqueue3

	printf("\nMLFQ(q=2^i) 스케쥴링 입니다.\n\n");

	int runtime = 0;
	int i = 0;
	int qt1 = 0;
	int qt2 = 0;
	int qt3 = 0;

	Queue temp_queue;
	Queue myqueue2;
	Queue myqueue3;

	InitQueue(&temp_queue);
	InitQueue(&myqueue2);
	InitQueue(&myqueue3);

	// 프로세스 삽입
	input_proc(A, myqueue1, runtime);
	input_proc(B, myqueue1, runtime);
	input_proc(C, myqueue1, runtime);
	input_proc(D, myqueue1, runtime);
	input_proc(E, myqueue1, runtime);

	while (1) {

	P: // 돌아올 위치
		qt1 = 0; qt2 = 0; qt3 = 0;

		if (runtime > 19) { // while문 탈출조건
			break;
		}

		// 프로세스 입력 -> 실행 -> 첫 번째인 경우 원래 큐에 유지, 그 밖에는 밑의 큐로 내려감
		// -> 새로 입력되는 프로세스가 우선 큐에 삽입 -> 서비스타임이 다 되었을 경우 Finish
		// 상위 큐에 프로세스가 존재해도 이미 시작한 프로세스의 quantum을 채워야 함

		// 실행

		if (IsEmpty(myqueue1) == 0) { // 가장 상위 큐가 myqueue1이면

			while (qt1<quantum_1) { // 첫 번째 큐의 time quantum을 채울 동안 반복

				Run(myqueue1, &myqueue1->front->proc); // 프로세스 실행
				qt1++;

				if (myqueue1->front->proc.sevTime == myqueue1->front->proc.runned_time) {// 서비스타임 다 되면 없애고
					Finish(myqueue1, &temp_queue, myqueue1->front->proc);
					if (qt1 < quantum_1)
						goto P; // quantum이 남아있더라도 현재 프로세스의 서비스타임이 다되었으므로 다음 실행할 프로세스 확인
				}
				else { // 서비스타임이 다 안되면 실행

					if (myqueue1->count == 1 && myqueue2.count == 0 && myqueue3.count == 0) { // 자신 말고 다른 프로세스가 없을 경우 현재 큐 유지

						// 프로세스 삽입
						input_proc(A, myqueue1, runtime + 1);
						input_proc(B, myqueue1, runtime + 1);
						input_proc(C, myqueue1, runtime + 1);
						input_proc(D, myqueue1, runtime + 1);
						input_proc(E, myqueue1, runtime + 1);

						Finish(myqueue1, myqueue1, myqueue1->front->proc);
						runtime++;
						goto P;
					}
					else if (qt1 < quantum_1) { // time quantum이 남았을 경우

						// 프로세스 삽입
						input_proc(A, myqueue1, runtime + 1);
						input_proc(B, myqueue1, runtime + 1);
						input_proc(C, myqueue1, runtime + 1);
						input_proc(D, myqueue1, runtime + 1);
						input_proc(E, myqueue1, runtime + 1);

						runtime++;
						continue;
					}

					else { // 자신 말고 다른 프로세스가 있을 경우 밑의 큐로 내려감
						Finish(myqueue1, &myqueue2, myqueue1->front->proc);
					}
					qt1++;
				}
			}

		}

		else if (IsEmpty(&myqueue2) == 0) { // 가장 상위 큐가 myqueue2이면

			while (qt2 < quantum_2) {
				if (myqueue2.front->proc.sevTime == myqueue2.front->proc.runned_time) { // 서비스타임 다 되면 없애고
					Finish(&myqueue2, &temp_queue, myqueue2.front->proc);
					if (qt2 < quantum_2)
						goto P; // quantum이 남아있더라도 현재 프로세스의 서비스타임이 다되었으므로 다음 실행할 프로세스 확인
				}

				Run(&myqueue2, &myqueue2.front->proc); // 프로세스 실행
				qt2++;

				if (qt2 < quantum_2) { // time quantum이 남았을 경우
					input_proc(A, myqueue1, runtime + 1);
					input_proc(B, myqueue1, runtime + 1);
					input_proc(C, myqueue1, runtime + 1);
					input_proc(D, myqueue1, runtime + 1);
					input_proc(E, myqueue1, runtime + 1);

					runtime++;
					continue;
				}
				else {
					Finish(&myqueue2, &myqueue3, myqueue2.front->proc);
				}
				qt2++;
			}
		}

		else if (IsEmpty(&myqueue3) == 0) { // 가장 상위 큐가 myqueue3이면

			while (qt3 < quantum_3) {
				if (myqueue3.front->proc.sevTime == myqueue3.front->proc.runned_time) { // 서비스타임 다 되면 없애고
					Finish(&myqueue3, &temp_queue, myqueue3.front->proc);
					if (qt3 < quantum_3)
						goto P; // quantum이 남아있더라도 현재 프로세스의 서비스타임이 다되었으므로 다음 실행할 프로세스 확인
				}

				Run(&myqueue3, &myqueue3.front->proc); // 프로세스 실행
				qt3++;

				if (qt3 < quantum_3) { // time quantum이 남았을 경우
					input_proc(A, myqueue1, runtime + 1);
					input_proc(B, myqueue1, runtime + 1);
					input_proc(C, myqueue1, runtime + 1);
					input_proc(D, myqueue1, runtime + 1);
					input_proc(E, myqueue1, runtime + 1);

					runtime++;
					continue;
				}
				else {
					Finish(&myqueue3, &myqueue3, myqueue3.front->proc);
				}
				qt3++;
			}
		}
		else if (IsEmpty(myqueue1) == 1 && IsEmpty(&myqueue2) == 1 && IsEmpty(&myqueue3) == 1) { // 세 개의 큐 내에 프로세스가 존재하지 않을 경우
			arr[arnum] = 'O'; // 프로세스가 없음을 뜻하고
			arnum++;		  // 다음 배열 인덱스로 이동해 입력 대기
			runtime++;

			// 프로세스 삽입
			input_proc(A, myqueue1, runtime);
			input_proc(B, myqueue1, runtime);
			input_proc(C, myqueue1, runtime);
			input_proc(D, myqueue1, runtime);
			input_proc(E, myqueue1, runtime);

			goto P; // 실행할 프로세스가 있는지 재확인
		}
		runtime++;

		// 프로세스 삽입
		input_proc(A, myqueue1, runtime);
		input_proc(B, myqueue1, runtime);
		input_proc(C, myqueue1, runtime);
		input_proc(D, myqueue1, runtime);
		input_proc(E, myqueue1, runtime);
	}
	Q_graph(arr); // 그래프 그리기
}

void mlfq_exe(){
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
	MLFQ_EXT(&myqueue, a, b, c, d, e, 1, 2, 4);

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
	printf("\nstart \t %s\n",tn);
}


