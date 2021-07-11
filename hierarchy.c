#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<time.h>
#include<pthread.h>

//類似資料分級 先拿編號低的再拿高的 0號可用0跟1 
//放相反順序 
#define N 5

pthread_t philosopher[N];//設定哲學家 
pthread_mutex_t chopstick[N];//設定筷子 
pthread_mutexattr_t mutex_attr; //mutex的屬性 
int times = 10;//執行次 
time_t waiting_time = 0;
void *start(int *who) 
{
    int id = (int*)who;
    printf("Philosopher %d 坐下\n",id);
    sleep(2);
    while(times-->=0)
	{
		sleep( rand()%3+1 );//假設每個哲學家會思考1~3秒
		
		time_t begin= time(NULL);
		printf("Philosopher %d 餓了\n",id);
		take_fork(id);
		
    	waiting_time+=time(NULL)-begin;
    	printf ("Philosopher %d 開始吃\n",id);
    	sleep( rand()%3+1 );//假設每個哲學家會吃1~3秒 
    	put_fork(id);
	}
    printf("Philosopher %d 次數到了\n",id);
    put_fork(id);//因為限制次數 防止因為次數有thread結束時還拿著筷子的deadlock 
    return NULL;
}
void take_fork(int id)
{
	int first_take,second_take;
	if(id!=N-1)
	{
		first_take = id;
		second_take = (id+1)%5;
	}
	else
	{
		first_take = (id+1)%5;
		second_take = id;
	}
	//先拿編號低的 再拿高的 
	pthread_mutex_lock(&chopstick[first_take]);
    printf("Philosopher %d 拿起%d號筷子\n", id, first_take);
    pthread_mutex_lock(&chopstick[second_take]);
    printf("Philosopher %d 拿起%d號筷子\n", id, second_take);
    
} 
void put_fork(int id)
{
	int first_take,second_take;
	if(id!=N-1)
	{
		first_take = id;
		second_take = (id+1)%5;
	}
	else
	{
		first_take = (id+1)%5;
		second_take = id;
	}
	printf("Philosopher %d 放下%d號筷子\n", id, second_take);
    pthread_mutex_unlock(&chopstick[second_take]);
    printf("Philosopher %d 放下%d號筷子\n", id, first_take);
    pthread_mutex_unlock(&chopstick[first_take]);
} 
int main()
{
	int i = 0;
	srand(time(NULL));//讓亂數會改變 
	
	pthread_mutexattr_init(&mutex_attr);//初始化鎖mutex的類型 
    pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_ERRORCHECK_NP);//設為檢錯鎖，不能unlock別人的東西 
	
	for(i = 0;i<N;i++)
	{
		pthread_mutex_init(&chopstick[i],&mutex_attr);
	}
	
	for(i = 0;i<N;i++)
	{	
		pthread_create(&philosopher[i],NULL,start, i);
	}
	sleep(1);
	printf("\n-----------------------------\n");
	
	for(i = 0;i<N;i++)
    {
	    pthread_join(philosopher[i], NULL);//等 philosopher執行完 
	} 
	printf("total wating time = %ld",waiting_time);
    for(i = 0;i<N;i++)
	{
		pthread_mutex_destroy(&chopstick[i]);
	}
	
	return 0;
}
