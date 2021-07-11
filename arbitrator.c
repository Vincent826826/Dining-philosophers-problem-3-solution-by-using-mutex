#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<time.h>
#include<windows.h>
#include<pthread.h>

//直接要拿就拿2個 拿筷子變成atomic的 放筷子不是 用mutuex的機制 
//不會有飢餓的問題因為 大家會去搶mutex排隊 
#define N 5//哲學家的數量 

pthread_t philosopher[N];//設定哲學家 
pthread_mutex_t chopstick[N];//設定筷子 
pthread_mutex_t mutex;//一次只有一個 philosopher可以拿筷子 
pthread_mutexattr_t mutex_attr; //mutex的屬性 
int times = 10;//所哲學家能吃飯的總次數 
time_t waiting_time = 0;//所有哲學家等多久 

void *start(int *who) 
{
    int id = (int*)who;
    printf("Philosopher %d 坐下\n",id);
    sleep(3);
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
	pthread_mutex_lock(&mutex);
	bool left = pthread_mutex_trylock(&chopstick[id])==0;
	bool right = pthread_mutex_trylock(&chopstick[(id+1)%5])==0;
	if(left&&right)//兩個筷子都可以拿才拿 
	{
    	printf("Philosopher %d 拿起%d號 跟 %d號筷子\n", id, id, (id+1)%5);
    	pthread_mutex_unlock(&mutex);
	}
    else//如果只有一個可以拿就放回去 
	{
		if(left)
		{
			//printf("Philosopher %d 拿起%d號筷子又放下了QQ\n", id, id);
			pthread_mutex_unlock(&chopstick[id]);
		}
		if(right)
		{
			//printf("Philosopher %d 拿起%d號筷子又放下了QQ\n", id, (id+1)%5);
			pthread_mutex_unlock(&chopstick[(id+1)%5]);
		} 
		pthread_mutex_unlock(&mutex);
		Sleep(1);//避免無限迴圈 延遲1毫秒 
		take_fork(id);
	}
    
}
/*void take_fork(int id) //另一種做法比較不Philosopher餓了就直接等到2個筷子都能用，期間大家都不能再拿 
{//好處最多等3秒 
	
	pthread_mutex_lock(&mutex);
	if(pthread_mutex_lock(&chopstick[id])==0)printf("Philosopher %d 拿起%d號筷子",id,id);
	if(pthread_mutex_lock(&chopstick[(id+1)%5])==0)printf("Philosopher %d 拿起%d號筷子\n", id, (id+1)%5);
    pthread_mutex_unlock(&mutex);
}*/
void put_fork(int id)
{
	//pthread_mutex_lock(&mutex);//加了要等很久會大家都在搶mutex 
	if(pthread_mutex_unlock(&chopstick[(id+1)%5])==0)
		printf("Philosopher %d 放下%d號筷子\n", id, (id+1)%5);
    if(pthread_mutex_unlock(&chopstick[id])==0)
    	printf("Philosopher %d 放下%d號筷子\n", id, id);
    
    //pthread_mutex_unlock(&mutex);
} 
int main()
{
	int i = 0;
	srand(time(NULL));//讓亂數會改變 
	
	pthread_mutexattr_init(&mutex_attr);//初始化鎖mutex的類型 
    pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_ERRORCHECK_NP);//設為檢錯鎖，不能unlock別人的東西 
	pthread_mutex_init(&mutex, &mutex_attr);
	
	for(i = 0;i<N;i++)
	{
		pthread_mutex_init(&chopstick[i],&mutex_attr);
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
