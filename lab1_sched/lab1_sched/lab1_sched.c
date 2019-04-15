/*
*	DKU Operating System Lab
*	    Lab1 (Scheduler Algorithm Simulator)
*	    Student id :    32141444    32144777   32140182
*           Student name :  김희주       최영재      윤찬영
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



/* * * RR * * */
// 큐 pop
struct task_info *queue_pop(){
	struct task_info *task_top = queue[queue_top];
	queue[queue_top] = NULL;
	if(++queue_top >= SIZE) queue_top = 0;
	return task_top;
}

// 큐 put
void queue_put(struct task_info* job){
	queue[queue_last] = job;
	if(++queue_last >= SIZE) queue_last = 0;
}


// 작업의 세부내용 설정 
void tasklist(){
	char name[] = "abcde\0";
	int arrive[] = {0,2,4,6,8},
		service[] = {3,6,4,5,2},
		i;
	queue_top = queue_last = 0;
	for(i=0;i<SIZE;i++){
		queue[i] = NULL;
		task[i].name = name[i];
		task[i].arrive_t = arrive[i];
		task[i].service_t = service[i];
		task[i].priority = 0;
		task[i].turnaround_t = -1; 
		task[i].response_t = -1;
		task[i].ticket  = service[i]*100;
	}

}


// 스케줄링 결과 출력
void print_scheduling_table(char arr[]){
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

// 성능 (turnaround time, response time의 평균)을 출력
void perfornamce_result(){
	int sum_turnaround_t=0, sum_response_t=0,
		i=0;
	float avg_turnaround_t, avg_response_t;
	for(;i<SIZE;i++){
		sum_turnaround_t += task[i].turnaround_t;
		sum_response_t += task[i].response_t;
      
	}
	avg_turnaround_t = (float)sum_turnaround_t/SIZE;
	avg_response_t = (float)sum_response_t/SIZE;
	
    printf("Turnaround Time 의 평균 : %.2f\n",avg_turnaround_t);
    printf("Response Time 의 평균 : %.2f\n",avg_response_t);
}

		

void rr(int max){
	tasklist();  
	char   s_result[20];	// 스케줄링의 결과를 출력할 때 사용할 문자
	int count = 0,	// SIZE를 넘는 수만큼 실행되면 안되기에 count를 세주기위한 변수
	service_end_t = 0,	//한 task가 수행해야 할 시간을 마친 시간을 체크하기 위한 변수
		next = 0,		// 다음 task 인덱스 
		schedule_count = 0; // schedule 실행 수 
	struct task_info *now = &task[next++]; // 지금 수행중인 task
	
	printf("Round Robin \t");
    	printf("q = %d\n",max); // 타임퀀텀을 출력한다
	while(count < SIZE){
		
		s_result[service_end_t] = now->name;
	// 나중에 result[]의 값을 take.[i]와 대조해서 테이블에 ■,□출력하는데 사용한다.
		if(now && now->response_t == -1) // response time 을 확인하기 위한 if문
			now->response_t = service_end_t - now->arrive_t;
		service_end_t++;

		while(next<SIZE && task[next].arrive_t == service_end_t){
			queue_put(&task[next++]);}
	// 다음 프로세스로 넘어간다. 만약 queue_last 인덱스가 SIZE보다 커지면 0으로 바꿔준다 (queue_put)
		if(--now->service_t <= 0){
			count++; // 다음 프로세스의 수행을 위해 count 값을 증가시킨다.
			now->turnaround_t = service_end_t - now->arrive_t;
		// turnaround time을 구한다
			schedule_count = 0; 
	// task가 한차례에 얼마나 수행했는지 알려주는 실행 수를 초기화 시켜준다
			now = queue_pop();
		} else {
			if(++schedule_count >= max){
				schedule_count = 0; 
		// 타임퀀텀 만큼 수행하면 초기화 시키고 순서를 넘긴다.
				queue_put(now);
				now = queue_pop();
			}
		}
	}
	printf("\n");
	print_scheduling_table(s_result);	// 스케쥴 결과를 출력한다.
	perfornamce_result(); // 스케줄링의 성능을 출력한다.
	printf("\n\n");
}

/* * * END RR * * */

/* * * MLFQ * * */


Process Setting(char name, int at, int st)  // Initializing Process Function
{
	Process p;

	p.name = name;
	p.arrive_t = at;
	p.service_t = st;
	p.runned_time = 0;

	return p;
}


void InitQueue(Queue *myQ)  // Initializing Queue Function
{
	myQ->front = NULL;
	myQ->rear = NULL;
	myQ->count = 0;
	srand((unsigned)time(NULL));
}


int IsEmpty(Queue *myQ)  //Check Queue is Empty (if NULL ->1 / !NULL ->0)
{
	if (myQ->front == NULL)
		return 1;
	else
		return 0;
}


void push(Queue *myQ, Process myprocess)  //Queue Push Funtion(Insert)
{
	// Using ChainNode
	ChainNode *newChain = (ChainNode *)malloc(sizeof(ChainNode));
	newChain->proc = myprocess;
	newChain->link = NULL;

	if (IsEmpty(myQ)) { // if Queue is Empty -> make new link at Front
		myQ->front = newChain;
	}
	else { // if Queue isn't Empty -> make new link at rear
		myQ->rear->link = newChain;
	}
	myQ->rear = newChain;
	myQ->count++;
}


void pop(Queue *myQ)  //Queue Pop Funtion(Delete)
{
	int num = 0;
	ChainNode *newChain;

	if (IsEmpty(myQ) == 1) // If Queue is NOT NULL, then not be removed
		myQ->front = myQ->rear = NULL;
	else {
		newChain = myQ->front;
		myQ->front = newChain->link;
		free(newChain);
	}
	myQ->count--;
	return;
}


void proc_end(Queue *myQ1, Queue *myQ2, Process p)  // Process end
{
	push(myQ2, p);
	pop(myQ1);
}


void proc_start(Queue *myQ, Process *p)  //Process Start
{
	p->runned_time++;
	arr[array_num] = p->name;
	array_num++;
}


void clear_job(char* arr) // clear array
{
	for (int i = 0; i < 20; i++)
	{
		arr[i] = 0;
	}
	array_num = 0;
}



void showTable(char* arr1) //Drawing Table for MLFQ
{
	int i;

	printf("a ");
	for (i = 0; i < 20; i++){
		if (arr1[i] == 'a') printf("■ ");
		else printf("□ ");
	}

	printf("\n");
	printf("b ");
	for (i = 0; i < 20; i++){
		if (arr1[i] == 'b') printf("■ ");
		else printf("□ ");
	}

	printf("\n");
	printf("c ");
	for (i = 0; i < 20; i++){
		if (arr1[i] == 'c') printf("■ ");
		else printf("□ ");
	}

	printf("\n");
	printf("d ");
	for (i = 0; i < 20; i++){
		if (arr1[i] == 'd') printf("■ ");
		else printf("□ ");
	}

	printf("\n");
	printf("e ");
	for (i = 0; i < 20; i++){	
		if (arr1[i] == 'e')printf("■ ");
		else printf("□ ");
	}
	printf("\n======================================================\n");
	clear_job(arr);
}



void put_in(Process A, Queue *myQ, int runtime) //Process insert Function
{
	if (A.arrive_t == runtime) {
		push(myQ, A);
	}
}

// MLFQ (q = 1)
void MLFQ(Queue *myQ1, Process a, Process b, Process c, Process d, Process e) {

    // 3 queue, time quantum = 1

	printf("\nMLFQ(q=1)\n\n");

	int runtime = 0;

	Queue temp_queue;
	Queue myQ2;
	Queue myQ3;

	InitQueue(&temp_queue);
	InitQueue(&myQ2);
	InitQueue(&myQ3);

	
	put_in(a, myQ1, runtime);
	put_in(b, myQ1, runtime);
	put_in(c, myQ1, runtime);
	put_in(d, myQ1, runtime);
	put_in(e, myQ1, runtime);

	while (1) {
		
		if (runtime > 19) { // while finishing condition
			break;
		}

        if (IsEmpty(myQ1) == 0) { // Highest Priority : myQ1
			proc_start(myQ1, &myQ1->front->proc); // Proc. execute

			if (myQ1->front->proc.service_t == myQ1->front->proc.runned_time) { //if process end -> proc_end
				proc_end(myQ1, &temp_queue, myQ1->front->proc);
			}
			else {
				if (myQ1->count == 1 && myQ2.count == 0 && myQ3.count == 0) { // Is there another process in Queue?
					
					put_in(a, myQ1, runtime + 1);
					put_in(b, myQ1, runtime + 1);
					put_in(c, myQ1, runtime + 1);
					put_in(d, myQ1, runtime + 1);
					put_in(e, myQ1, runtime + 1); // checking process that scheduled to load

					proc_end(myQ1, myQ1, myQ1->front->proc);
					runtime++;
					continue;
				}

				else {
					proc_end(myQ1, &myQ2, myQ1->front->proc);
				}
			}
		}
        /* Re-execute in the same way as myQ1 */
		else if (IsEmpty(&myQ2) == 0) {
			if (myQ2.front->proc.service_t == myQ2.front->proc.runned_time) {
				proc_end(&myQ2, &temp_queue, myQ2.front->proc);
			}

			proc_start(&myQ2, &myQ2.front->proc);
			proc_end(&myQ2, &myQ3, myQ2.front->proc);
		}
        /* Re-execute in the same way as myQ1, myQ2 */
		else if (IsEmpty(&myQ3) == 0) {
			if (myQ3.front->proc.service_t == myQ3.front->proc.runned_time) {
				proc_end(&myQ3, &temp_queue, myQ3.front->proc);
			}
			proc_start(&myQ3, &myQ3.front->proc);
			proc_end(&myQ3, &myQ3, myQ3.front->proc); // Last level Queue -> RR
		}
		else if (IsEmpty(myQ1) == 1 && IsEmpty(&myQ2) == 1 && IsEmpty(&myQ3) == 1) { //If All Queue is Empty
			arr[array_num] = 'O';
			array_num++;		  // next Index
		}
		runtime++;
        
		put_in(a, myQ1, runtime);
		put_in(b, myQ1, runtime);
		put_in(c, myQ1, runtime);
		put_in(d, myQ1, runtime);
		put_in(e, myQ1, runtime);  //process insert
	}
	showTable(arr); // Drawing Table
}

// MLFQ-2^i
void MLFQ_2(Queue *myQ1, Process a, Process b, Process c, Process d, Process e, int quantum_1, int quantum_2, int quantum_3) {

	 // 3 queue, time quantum = 2^i

	printf("\nMLFQ(q=2^i)\n\n");

	int runtime = 0;
	int i = 0;
	int qt1 = 0;
	int qt2 = 0;
	int qt3 = 0;

	Queue temp_queue;
	Queue myQ2;
	Queue myQ3;

	InitQueue(&temp_queue);
	InitQueue(&myQ2);
	InitQueue(&myQ3);
    
	put_in(a, myQ1, runtime);
	put_in(b, myQ1, runtime);
	put_in(c, myQ1, runtime);
	put_in(d, myQ1, runtime);
	put_in(e, myQ1, runtime);

	while (1) {

	P: // Back Point
		qt1 = 0; qt2 = 0; qt3 = 0;

		if (runtime > 19) {
			break;
		}
		if (IsEmpty(myQ1) == 0) { // Highest Priority : myQ1

			while (qt1<quantum_1) { //proc.execute until time quantum times
				proc_start(myQ1, &myQ1->front->proc);
				qt1++;

				if (myQ1->front->proc.service_t == myQ1->front->proc.runned_time) {
					proc_end(myQ1, &temp_queue, myQ1->front->proc);
					if (qt1 < quantum_1)
						goto P; // time quantum is left, but sevice time is over -> Check other Process.
				}
				else {
					if (myQ1->count == 1 && myQ2.count == 0 && myQ3.count == 0) {
						
						put_in(a, myQ1, runtime + 1);
						put_in(b, myQ1, runtime + 1);
						put_in(c, myQ1, runtime + 1);
						put_in(d, myQ1, runtime + 1);
						put_in(e, myQ1, runtime + 1);

						proc_end(myQ1, myQ1, myQ1->front->proc);
						runtime++;
						goto P;
					}
					else if (qt1 < quantum_1) { // time quantum is left

						put_in(a, myQ1, runtime + 1);
						put_in(b, myQ1, runtime + 1);
						put_in(c, myQ1, runtime + 1);
						put_in(d, myQ1, runtime + 1);
						put_in(e, myQ1, runtime + 1);

						runtime++;
						continue;
					}

					else {
						proc_end(myQ1, &myQ2, myQ1->front->proc);
					}
					qt1++;	}
			}

		}
        /* Re-execute in the same way as myQ1 */
		else if (IsEmpty(&myQ2) == 0) {

			while (qt2 < quantum_2) {
				if (myQ2.front->proc.service_t == myQ2.front->proc.runned_time) {
					proc_end(&myQ2, &temp_queue, myQ2.front->proc);
					if (qt2 < quantum_2)
						goto P;
				}

				proc_start(&myQ2, &myQ2.front->proc);
				qt2++;

				if (qt2 < quantum_2) {
					put_in(a, myQ1, runtime + 1);
					put_in(b, myQ1, runtime + 1);
					put_in(c, myQ1, runtime + 1);
					put_in(d, myQ1, runtime + 1);
					put_in(e, myQ1, runtime + 1);

					runtime++;
					continue;
				}
				else {
					proc_end(&myQ2, &myQ3, myQ2.front->proc);
				}
				qt2++;
			}
		}
         /* Re-execute in the same way as myQ1, myQ2 */
		else if (IsEmpty(&myQ3) == 0) {

			while (qt3 < quantum_3) {
				if (myQ3.front->proc.service_t == myQ3.front->proc.runned_time) {
					proc_end(&myQ3, &temp_queue, myQ3.front->proc);
					if (qt3 < quantum_3)
						goto P;
				}

				proc_start(&myQ3, &myQ3.front->proc);
				qt3++;

				if (qt3 < quantum_3) { 
					put_in(a, myQ1, runtime + 1);
					put_in(b, myQ1, runtime + 1);
					put_in(c, myQ1, runtime + 1);
					put_in(d, myQ1, runtime + 1);
					put_in(e, myQ1, runtime + 1);

					runtime++;
					continue;
				}
				else {
					proc_end(&myQ3, &myQ3, myQ3.front->proc);
				}
				qt3++;
			}
		}
		else if (IsEmpty(myQ1) == 1 && IsEmpty(&myQ2) == 1 && IsEmpty(&myQ3) == 1) {
			arr[array_num] = 'O';
			array_num++;
			runtime++;

			put_in(a, myQ1, runtime);
			put_in(b, myQ1, runtime);
			put_in(c, myQ1, runtime);
			put_in(d, myQ1, runtime);
			put_in(e, myQ1, runtime);

			goto P; // checking process to execute
		}
		runtime++;

		put_in(a, myQ1, runtime);
		put_in(b, myQ1, runtime);
		put_in(c, myQ1, runtime);
		put_in(d, myQ1, runtime);
		put_in(e, myQ1, runtime);
	}
	showTable(arr); // Drawing Table
}

void mlfq_exe(){
Queue myQ;

	// Initializing Queue
	InitQueue(&myQ);

	// Initializing Process
	Process a = Setting('a', 0, 3);
	Process b = Setting('b', 2, 6);
	Process c = Setting('c', 4, 4);
	Process d = Setting('d', 6, 5);
	Process e = Setting('e', 8, 2);

	// MLFQ
	MLFQ(&myQ, a, b, c, d, e);

	// MLFQ - Ext
	MLFQ_2(&myQ, a, b, c, d, e, 1, 2, 4);

}

/* * * END MLFQ * * */

/* * * FIFO & LOTTERY * * */
void set_display(int img[5][20]){
	
	char pro_name[5]="abcde";
	int i,j;
	for(j=0; j<5; j++){
		printf("%c ", pro_name[j]);
		for(i=0; i<20; i++){
			if(img[j][i]==0) {
				printf("□ ");
			} else printf("■ ");
		}
		printf("\n"); 
	}
	printf("======================================================\n");
	
	double sum_respo=0, sum_turna=0;
	for(i=0; i<5; i++){
		printf("%c : Turnaroundtime = %d  Response Time = %d\n", 
			ytask[i].process, ytask[i].turnarround_t, ytask[i].response_t);
		sum_respo = sum_respo+ytask[i].response_t;
		sum_turna = sum_turna+ytask[i].turnarround_t;
	}
	printf("Turnaroundtime  sum : %d  AVG : %.2lf\n", (int)sum_turna, sum_turna/5);
	printf("responsetime  sum : %d  AVG : %.2lf\n", (int)sum_respo, sum_respo/5);
	printf("\n\n");
}

void set_task(){
	int i;
	char name[5]="abcde";
	int arrive_t[5] = {1,2,3,4,5},
		service_t[5] = {3,6,4,5,2};
	
	for(i=0; i<5; i++){
		ytask[i].process = name[i];
		ytask[i].arrive_t = arrive_t[i];
		ytask[i].service_t = service_t[i];
		ytask[i].response_t = -1;
	}
}

void fifo(){
	printf("\nFIFO\n\n");
	set_task();
	int i,j;
	int time=-1;
	int img[5][20]={0};
	int prio=1;
	for(time=0; time<20; time++){
		
		for(i=0; i<5; i++){
			if((time+1)==ytask[i].arrive_t){
				queue_y[seg]=ytask[i].process;
				seg = seg+1;
			}
		}
	}
	for(i=0; i<5; i++){
		if(queue_y[i]==ytask[i].process){
			if(queue_y[i]=='a'){
				ytask[i].priority =prio;
				prio++;
			} else if(queue_y[i]=='b'){
				ytask[i].priority =prio;
				prio++;
			} else if(queue_y[i]=='c'){
				ytask[i].priority =prio;
				prio++;
			} else if(queue_y[i]=='d'){
				ytask[i].priority =prio;
				prio++;
			} else if(queue_y[i]=='e'){
				ytask[i].priority =prio;
				prio++;
			}
		}
	}
	
	int k=0;
	for(j=1; j<6; j++){
		for(i=0; i<5; i++)
		if(ytask[i].priority == j){
			ytask[i].response_t = (k+1)-ytask[i].arrive_t;
			for(; ytask[i].service_t!=0; ytask[i].service_t--)
			{
				img[i][k] = 1;
				k++;
			}
			ytask[i].turnarround_t = (k+1)-ytask[i].arrive_t;
		}
	}
	
	set_display(img);
}

void queue_y_clean(char name, int time){
	int i, j;
	for(i=0; i<5; i++){
		if(queue_y[i]==name){
			ytask[i].turnarround_t = time-ytask[i].service_t;
			for(j=i; j<seg; j++){
				queue_y[j] = queue_y[j+1];
			}
			seg--;
		}
	}
}

void lottery(){
	printf("\nLOTTERY\n\n");
	set_task();
	int img[5][20]={0};
	int t, i, j;
	seg=0;
	int number;
	int rannum_max=0;
	//서비스시간에 따른 비율로 서비스 배분
	int total_service_t=0;
	for(i=0; i<5; i++){
		total_service_t =total_service_t + ytask[i].service_t;
	} 
	for(i=0; i<5; i++){
		ytask[i].ticket = (int)(((double)ytask[i].service_t/(double)total_service_t)*100);
	}
	
	for(t=0; t<20; t++){
		int temp_num;
		for(i=0; i<5; i++){
			if(ytask[i].arrive_t==t+1){
				queue_y[seg] = ytask[i].process;
				seg++;
			}
		} 
		
		if(seg==1){
			for(i=0; i<5; i++){
				if(queue_y[seg-1] == ytask[i].process){
					img[i][t]=1;
					temp_num=i;
					ytask[i].service_t--;
					if(ytask[i].service_t==0){
						queue_y_clean(ytask[i].process, t);
					}
					break;
				}
			}
		}else if(seg == 2){
			int temp[2];
			int j=0;
			for(i=0; i<5; i++){
				if(queue_y[seg-2]==ytask[i].process){
					temp[j]=i;
					j++;
				}else if(queue_y[seg-1]==ytask[i].process){
					temp[j]=i;
					j++;
				}
			}
			rannum_max = ytask[temp[0]].ticket+ytask[temp[1]].ticket;
			srand(time(NULL));
			number=(rand() % rannum_max)+1;
			if(number<=ytask[temp[0]].ticket){
				temp_num=temp[0];
				img[temp[0]][t]=1;
				ytask[temp[0]].service_t--;
				if(ytask[temp[0]].service_t==0){
					queue_y_clean(ytask[temp[0]].process, t);
				}
			}else if(number>ytask[temp[0]].ticket){
				temp_num=temp[1];
				img[temp[1]][t]=1;
				ytask[temp[1]].service_t--;
				if(ytask[temp[1]].service_t==0){
					queue_y_clean(ytask[temp[1]].process, t);
				}
			}
		}else if(seg == 3){
			int temp[3];
			int j=0;
			for(i=0; i<5; i++){
				if(queue_y[seg-3]==ytask[i].process){
					temp[j]=i;
					j++;
				}else if(queue_y[seg-2]==ytask[i].process){
					temp[j]=i;
					j++;
				}else if(queue_y[seg-1]==ytask[i].process){
					temp[j]=i;
					j++;
				}
			}
			rannum_max = ytask[temp[0]].ticket+ytask[temp[1]].ticket+ytask[temp[2]].ticket;
			srand(time(NULL));
			number=(rand() % rannum_max)+1;
			if(number<=ytask[temp[0]].ticket){
				temp_num=temp[0];
				img[temp[0]][t]=1;
				ytask[temp[0]].service_t--;
				if(ytask[temp[0]].service_t==0){
					queue_y_clean(ytask[temp[0]].process, t);
				}
			}else if(number>ytask[temp[0]].ticket && number<=ytask[temp[1]].ticket){
				temp_num=temp[1];
				img[temp[1]][t]=1;
				ytask[temp[1]].service_t--;
				if(ytask[temp[1]].service_t==0){
					queue_y_clean(ytask[temp[1]].process, t);
				}
			}else if(number>ytask[temp[1]].ticket){
				temp_num=temp[2];
				img[temp[2]][t]=1;
				ytask[temp[2]].service_t--;
				if(ytask[temp[2]].service_t==0){
					queue_y_clean(ytask[temp[2]].process, t);
				}
			}
		}else if(seg == 4){
			int temp[4];
			int j=0;
			for(i=0; i<5; i++){
				if(queue_y[seg-4]==ytask[i].process){
					temp[j]=i;
					j++;
				}else if(queue_y[seg-3]==ytask[i].process){
					temp[j]=i;
					j++;
				}else if(queue_y[seg-2]==ytask[i].process){
					temp[j]=i;
					j++;
				}else if(queue_y[seg-1]==ytask[i].process){
					temp[j]=i;
					j++;
				}
			}
			rannum_max = ytask[temp[0]].ticket+ytask[temp[1]].ticket+ytask[temp[2]].ticket+ytask[temp[3]].ticket;
			srand(time(NULL));
			number=(rand() % rannum_max)+1;
			if(number<=ytask[temp[0]].ticket){
				temp_num=temp[0];
				img[temp[0]][t]=1;
				ytask[temp[0]].service_t--;
				if(ytask[temp[0]].service_t==0){
					queue_y_clean(ytask[temp[0]].process, t);
				}
			}else if(number>ytask[temp[0]].ticket && number<=ytask[temp[1]].ticket){
				temp_num=temp[1];
				img[temp[1]][t]=1;
				ytask[temp[1]].service_t--;
				if(ytask[temp[1]].service_t==0){
					queue_y_clean(ytask[temp[1]].process, t);
				}
			}else if(number>ytask[temp[1]].ticket && number<=ytask[temp[2]].ticket){
				temp_num=temp[2];
				img[temp[2]][t]=1;
				ytask[temp[2]].service_t--;
				if(ytask[temp[2]].service_t==0){
					queue_y_clean(ytask[temp[2]].process, t);
				}
			}else if(number>ytask[temp[2]].ticket){
				temp_num=temp[3];
				img[temp[3]][t]=1;
				ytask[temp[3]].service_t--;
				if(ytask[temp[3]].service_t==0){
					queue_y_clean(ytask[temp[3]].process, t);
				}
			}
		}else if(seg == 5){
			int temp[5];
			int j=0;
			for(i=0; i<5; i++){
				if(queue_y[seg-5]==ytask[i].process){
					temp[j]=i;
					j++;
				}else if(queue_y[seg-4]==ytask[i].process){
					temp[j]=i;
					j++;
				}else if(queue_y[seg-3]==ytask[i].process){
					temp[j]=i;
					j++;
				}else if(queue_y[seg-2]==ytask[i].process){
					temp[j]=i;
					j++;
				}else if(queue_y[seg-1]==ytask[i].process){
					temp[j]=i;
					j++;
				}
			}
			rannum_max = ytask[temp[0]].ticket+ytask[temp[1]].ticket+ytask[temp[2]].ticket+ytask[temp[3]].ticket+ytask[temp[4]].ticket;
			srand(time(NULL));
			number=(rand() % rannum_max)+1;
			if(number<=ytask[temp[0]].ticket){
				temp_num=temp[0];
				img[temp[0]][t]=1;
				ytask[temp[0]].service_t--;
				if(ytask[temp[0]].service_t==0){
					queue_y_clean(ytask[temp[0]].process, t);
				}
			}else if(number>ytask[temp[0]].ticket && number<=ytask[temp[1]].ticket){
				temp_num=temp[1];
				img[temp[1]][t]=1;
				ytask[temp[1]].service_t--;
				if(ytask[temp[1]].service_t==0){
					queue_y_clean(ytask[temp[1]].process, t);
				}
			}else if(number>ytask[temp[1]].ticket && number<=ytask[temp[2]].ticket){
				temp_num=temp[2];
				img[temp[2]][t]=1;
				ytask[temp[2]].service_t--;
				if(ytask[temp[2]].service_t==0){
					queue_y_clean(ytask[temp[2]].process, t);
				}
			}else if(number>ytask[temp[2]].ticket && number<=ytask[temp[3]].ticket){
				temp_num=temp[3];
				img[temp[3]][t]=1;
				ytask[temp[3]].service_t--;
				if(ytask[temp[3]].service_t==0){
					queue_y_clean(ytask[temp[3]].process, t);
				}
			}else if(number>ytask[temp[3]].ticket){
				temp_num=temp[4];
				img[temp[4]][t]=1;
				ytask[temp[4]].service_t--;
				if(ytask[temp[4]].service_t==0){
					queue_y_clean(ytask[temp[4]].process, t);
				}
			}
		}
		if(ytask[temp_num].response_t == -1){
			ytask[temp_num].response_t = t;
		}
	}
	set_display(img);
}

/* * * END FIFO & LOTTERY * * */
