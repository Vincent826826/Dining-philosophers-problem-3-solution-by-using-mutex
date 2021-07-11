#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include <time.h>
#include <windows.h>
#include<pthread.h>

//髒筷子 0號哲學家可以用4跟0  1號用0跟1 
//拿到筷子還沒吃 lock起來 直到吃完筷子變髒unlock 
#define N 5
enum{
	THINKING,
	HUANGRY,
	EATING,
};//哲學家的狀態 
enum{
	DIRTY,
	CLEAN
};//筷子的狀態 
 
pthread_t philosopher[N];//設定哲學家 
pthread_mutex_t chopstick[N];//設定筷子 
int chopstick_status[2][N];//第一維筷子現在被誰拿 第二維筷子髒不髒
int  philosopher_status[N];//哲學家的狀態 
int times = 10;//總執行次數 
time_t waiting_time = 0;

void *start(int *who) 
{
    int id = (int*)who;
    printf("Philosopher %d 坐下\n",id);
    if(id!=N-1)//初始化 
	{
		chopstick_status[0][id] = id;
		chopstick_status[1][id] = DIRTY;
		if(id==0)
		{
			chopstick_status[0][ N-1 ] = id;
			chopstick_status[1][ N-1 ] = DIRTY;//編號最小的人多拿編號最大的筷子 
		}
	}
    sleep(3);//等所有人都進來 
    while(times-->=0)//限制大家吃飯的總次數 
	{
		think(id);
		huangry(id);
	}
	printf("Philosopher %d 次數到了\n",id);
    return NULL;
}
void think(int id)
{
	printf("Philosopher %d 開始思考\n",id);
	philosopher_status[id] =  THINKING;
	sleep( rand()%3+1 );//假設每個哲學家會思考1~3秒
}
void huangry(int id)
{
	philosopher_status[id] =  HUANGRY;
	printf("Philosopher %d 餓了\n",id);
	int first = (id+N-1)%5,second = id;//先拿編號低的 再拿高的 
	if(first > second)
	{
		first = id;
		second = (id+N-1)%5;
	}
	time_t begin= time(NULL);
	
	request(id,first);
	request(id,second);//對需要的筷子發送request 
	
    waiting_time+=time(NULL)-begin;//計算等多久才開始吃
	eating(id,first,second); 
}
void request(int id,int chopstick_id)
{
	if(chopstick_status[0][chopstick_id] != id)//筷子不在我手上 
	{
		int chopstick_owner = chopstick_status[0][chopstick_id];//有這個筷子的人
		if(pthread_mutex_lock(&chopstick[chopstick_id])==0)
		{
				chopstick_status[0][chopstick_id] = id;//筷子給要的人 
				chopstick_status[1][chopstick_id] = CLEAN;//擦乾淨	
				printf("Philosopher %d 從%d那邊拿了%d號筷子\n",id,chopstick_owner,chopstick_id);
		}
	}
	else
	{
		pthread_mutex_lock(&chopstick[chopstick_id]);
		printf("Philosopher %d 用自己的%d號筷子\n",id,chopstick_id);	
	} 
}
void eating(int id,int first,int second)
{
	//pthread_mutex_lock(&mutex);
	printf ("Philosopher %d 開始吃\n",id);
	philosopher_status[id] =  EATING;
	chopstick_status[1][first] = DIRTY;
	chopstick_status[1][second] = DIRTY;
	sleep( rand()%3+1 );//假設每個哲學家會吃1~3秒
	printf ("Philosopher %d 吃飽了\n",id);
	pthread_mutex_unlock(&chopstick[first]);//吃完之後放出筷子讓在等的人可拿 
	pthread_mutex_unlock(&chopstick[second]);
	
	//pthread_mutex_unlock(&mutex);
}
int main()
{
	int i = 0;
	srand(time(NULL));//讓亂數會改變 
	
	for(i = 0;i<N;i++)
	{
		pthread_mutex_init(&chopstick[i],NULL);//設為一般的鎖 讓其他人可以unlock別人的筷子 
	}
	
	for(i = 0;i<N;i++)
	{	
		pthread_create(&philosopher[i],NULL,start, (int*)i);
	}
	sleep(2);
	printf("\n-----------------------------\n");
	
	for(i = 0;i<N;i++)
    {
	    pthread_join(philosopher[i], NULL);//等所有philosopher執行完 
	} 
	printf("total wating time = %ld",waiting_time);
    for(i = 0;i<N;i++)
	{
		pthread_mutex_destroy(&chopstick[i]);
	}
	
	return 0;
}
