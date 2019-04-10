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
#include <stdbool.h>
#include <math.h>
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
    endl();
    for(;i<SIZE;i++){
        if(queue[i] == NULL) continue;
        printf("i:%d,name:%c,svc:%d,prt:%d\n",i,queue[i]->name,queue[i]->svc,queue[i]->prt);
    }
    printf("top:%d, last:%d\n",qt,ql);
    endl();
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
        endl();
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
        printf("[%c] ",task[i].name);
        printf("turnaround %2d  ",task[i].tat);
        printf("response %2d\n",task[i].rst);
    }
    avg_tat = (float)sum_tat/SIZE;
    avg_rst = (float)sum_rst/SIZE;
    printf("[turnaround] ");
    printf("sum %2d, avg %.2f\n",sum_tat,avg_tat);
    printf("[reponse]    ");
    printf("sum %2d, avg %.2f\n",sum_rst,avg_rst);
}

/*
 * you need to implement FCFS, RR, MLFQ, Lottery scheduler.
 */
void initQue(Queue *queue){
	queue->front = queue->rear = NULL;
	queue->count = 0;
}

/*
 * MLFQ에서 Queue의 timeSlice를 설정하는 함수
 */
void setQueueTimeSlice(Queue *queue, int timeSlice){
	queue -> timeSlice = timeSlice;
}

/*
 * 큐의 공백을 검사하는 함수
 */
bool isEmpty(Queue *queue){
	if(queue->count == 0){
		return true; //공백이면 true
	}else{
		return false; //아니면 false
	}
}

/*
 * MLFQ에서 모든 큐가 공백인지 검사하는 함수
 */
bool isAllEmpty(Queue *multiQueue, int queueLevel){
	for(int i = 0; i < queueLevel; i++){
		if(isEmpty(&multiQueue[i]) == false){
			return false;
		}
	}
	return true;
}

/*
 * 큐의 rear에 노드를 삽입하는 함수
 */
void enQueueRear(Queue *queue, Process *process){
	Node *newNode = (Node *)malloc(sizeof(Node));
	newNode->process = process;
	newNode->next = NULL;

	if(isEmpty(queue)){
		queue->front = newNode;
		queue->rear = newNode;
	}else{
		queue->rear->next = newNode;
	}
	queue->rear = newNode;
	queue->count++;
}

void enQueueInSJF(Queue *queue, Process *process){
	Node *newNode = (Node *)malloc(sizeof(Node));
	newNode->process = process;
	newNode->next = NULL;
	Node *pa = queue->front;//노드찾는 용도
	Node *ch = queue->front;
	

	if(ch == NULL){ //노드가 비었을 경우
		queue->front=newNode;
		queue->rear=newNode;
	}else{
		ch = pa->next;
		for(;ch != NULL;){ //노드가 하나 이상  있을시, 중간에 삽입할 경우
			if(pa->process->serviceTime <= process->serviceTime && ch->process->serviceTime <= process->serviceTime){
				newNode->next = ch;
				pa->next = newNode;
				break;
			}else{
				pa = pa->next;
				ch = ch->next;
			}
		}
			if(ch == NULL){ //맨 앞, 맨 뒤에 삽입하는 경우
				if(queue->rear->process->serviceTime < process->serviceTime){//맨 뒤 삽입
		                	queue->rear->next = newNode;
					queue->rear = newNode;
                  		}else{ //노드가 하나일 경우, 맨 앞 삽입
                          		newNode->next = queue->front;
					queue->front = newNode;
				}
			}
	}
	queue->count++;
}

/*
 * 큐의 head에 노드를 삽입하는 함수
 */
void enQueueFront(Queue *queue, Process *process){
	Node *newNode = (Node *)malloc(sizeof(Node));
	newNode->process = process;
	newNode->next = NULL;

	if(isEmpty(queue)){
		queue->front = newNode;
		queue->rear = newNode;
	}else{
		newNode->next = queue->front;
		queue->front = newNode;
	}
	queue->count++;
}

/*
 * 큐의 head 노드를 삭제하는 함수
 */
Process * deQueue(Queue *queue){
	Process *delProc;
	Node *curNode;

	if(isEmpty(queue)){
		return NULL;
	}else{
		curNode = queue->front;
		delProc = curNode->process;
		queue->front = curNode->next;
		free(curNode);
		queue->count--;
		return delProc;
	}
}

void * deQueueInLottery(Queue *queue, Node *pa, Node *ch){
	if(!pa){
		queue->front = ch->next;
		free(ch);
	}else if(!(ch->next)){
		queue->rear = pa;
		pa->next = NULL;
		free(ch);
	}else{
		pa->next = ch->next;
		free(ch);
	}
	queue->count--;
}

/*
 * FIFO
 * processSet : 스케쥴링의 대상이 되는 프로세스 집합
 * workLoad : 프로세스들의 수행결과를 저장할 워크로드
 * totalRunningTime : 대기중인 프로세스들의 총 수행시간
 */
void firstInFirstOut(Process *processSet, bool **workLoad, int totalRunningTime){
	Queue readyQue;
	int timer = 0;
	int processIndex = 0;
	Process *runningProcess = NULL;
	initQue(&readyQue);

	for(; timer < totalRunningTime; timer++){
		while(processSet[processIndex].arriveTime == timer){
			enQueueRear(&readyQue, &processSet[processIndex]);
			processIndex++;
		}
		if(runningProcess == NULL){
			runningProcess = deQueue(&readyQue);
		}
		runningProcess -> currentServiceTime++;
		workLoad[runningProcess -> processId][timer] = true;
		if(runningProcess -> serviceTime == runningProcess -> currentServiceTime){
			runningProcess = NULL;
		}
	}
}

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

// multi level feedback queue
void mlfq(){
    taskSet();
    char tn[] = "Multi Level Feedback Queue\0",
    in[20];
    int i = 0,
    kill_count = 0,
    svc_t = 0,
    next = 0;
    struct task_t *now = &task[next++];
    startLog(tn);
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
    endl();
    print_table(in);
    print_performance();
    endLog(tn);
}



/*
 * MLFQ
 * processSet : 스케쥴링의 대상이 되는 프로세스 집합
 * workLoad : 프로세스들의 수행결과를 저장할 워크로드
 * totalRunningTime : 대기중인 프로세스들의 총 수행시간
 * timeSlice : 작업 할당량
 * exp : 큐의 우선순위가 낮아질수록  timeSlice를 제곱함 
 */
void multilevelFeedbackQueue(Process *processSet, bool **workLoad, int totalRunningTime, int timeSlice, bool exp){
	Queue multiQueue[4];
	int timer = 0;
	int expedNumber = timeSlice;
	int processIndex = 0;
	int queueLevel = 4;
	Process *runningProcess = NULL;

	for(int i = 0; i < queueLevel; i++){
		initQue(&multiQueue[i]);
		multiQueue[i].priority = i;
		if(exp == true){
			timeSlice = pow(expedNumber, i);
		}
		multiQueue[i].timeSlice = timeSlice;
	}

	for(; timer < totalRunningTime;){
		
		while(processSet[processIndex].arriveTime == timer){
			enQueueFront(&multiQueue[0], &processSet[processIndex]);
			processIndex++;
		}
		if(runningProcess == NULL){ //작업중인 큐가 없으면 큐에 새로운 잡을 넣어줌
			for(int i = 0; i < queueLevel; i++){
				runningProcess = deQueue(&multiQueue[i]); // 우선순위가 높은 큐에 있는 잡부터 수행프로세스에 삽입
			
				if(runningProcess != NULL){ // 러닝프로세스에 들오면 더이상 큐에서 프로세스를 빼지 않음
					runningProcess -> priority = i;
					break;
				}
			}
		}
		runningProcess -> currentServiceTime++;
		runningProcess -> accumulatedTime++;
		workLoad[runningProcess -> processId][timer] = true;
		timer++; // 타이머 증가
		
		if(runningProcess -> currentServiceTime == runningProcess -> serviceTime){
			runningProcess = NULL; //프로세스 종료
		}else if(runningProcess -> accumulatedTime == multiQueue[runningProcess -> priority].timeSlice){ //아직 수행할 시간이 더 남았음
			if(isAllEmpty(multiQueue, queueLevel) == false || processSet[processIndex].arriveTime == timer){
				if( runningProcess -> priority != (queueLevel -1)){
					runningProcess -> priority++;
				}
			}
			enQueueRear(&multiQueue[runningProcess -> priority], runningProcess);
			
			runningProcess -> accumulatedTime = 0;
			runningProcess = NULL;
		}
	}
}


void rr(int max_sched_cnt){
    taskSet();  // task initialize
    char tn[30] = "Round Robin \0",    // task name
    max[1],    // for round robin scheduling conut number
    in[20];    // string for scheduling result print
    int kill_count = 0,    // task killed count
    svc_t = 0,    // service time
    next = 0,        // next task index
    sched_cnt = 0; // schedule count
    struct task_t *now = &task[next++]; // now task
    sprintf(max,"%d",max_sched_cnt);
    strcat(tn,max);
    printf("   ");
    while(kill_count < SIZE){
        printf("%c ",now->name);
        in[svc_t] = now->name;
        if(now && now->rst == -1) // response time check
            now->rst = svc_t - now->arv;
        svc_t++;
        
        while(next<SIZE && task[next].arv == svc_t){
            q_put(&task[next++]);        // put next task
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
    endl();
    print_table(in);    // print scheduling result table
}


void lottery(){
    char tn[] = "Lottery\0",
    in[20];
    int i = 0,
    kill_count = 0,
    arv_cnt=1,
    svc_t=0,
    max, stack_tk, win_number;
    taskSet();
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
    endl();
    print_table(in);
}

bool ** createWorkLoad(int totalRunningTime, bool **workLoad){
	workLoad = (bool**) malloc (sizeof(bool*) * PROCESS_COUNT);
    for(int i = 0; i < PROCESS_COUNT; i++){
         workLoad[i] = (bool*) malloc (sizeof(bool) * totalRunningTime);
         for(int j = 0; j < totalRunningTime; j++){
			 workLoad[i][j] = false;
		 }
    }
	return workLoad;
}

void showWorkLoad(int totalRunningTime, bool **workLoad){
	char processName[PROCESS_COUNT] = {'A', 'B', 'C', 'D', 'E'};

	for(int i = 0; i < PROCESS_COUNT; i++){
		printf("%c :", processName[i]);
		for(int j =0; j < totalRunningTime; j++){
			if(workLoad[i][j] == true){
				printf(" ■ ");
			}else{
				printf(" □ ");
			}
		}
		printf("\n");
	}
}

void initComponent(Process *processSet, bool **workLoad, int totalServiceTime){
	//Process 설정 초기화
	for(int i = 0; i < PROCESS_COUNT; i++){
		processSet[i].currentServiceTime = 0;
		processSet[i].accumulatedTime = 0;
	}

	//workLoad 초기화
	for(int i = 0; i < PROCESS_COUNT; i++){
		for(int j = 0; j < totalServiceTime; j++){
				workLoad[i][j] = false;
		}
	}
}

void showMenu(void){
	printf("-------------------------------------\n");
	printf("1. First come first serve\n");
	printf("2. Shortest job first\n");
	printf("3. Round robin\n");
	printf("4. Multilevel feedback queue(q=1)\n");
    printf("5. Multilevel feedback queue(q=2^i)\n");
	printf("6. Lottery\n");
	printf("7. QUIT\n");
	printf("-------------------------------------\n");
}
