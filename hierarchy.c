#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<time.h>
#include<pthread.h>

//������Ƥ��� �����s���C���A������ 0���i��0��1 
//��ۤ϶��� 
#define N 5

pthread_t philosopher[N];//�]�w���Ǯa 
pthread_mutex_t chopstick[N];//�]�w�_�l 
pthread_mutexattr_t mutex_attr; //mutex���ݩ� 
int times = 10;//���榸 
time_t waiting_time = 0;
void *start(int *who) 
{
    int id = (int*)who;
    printf("Philosopher %d ���U\n",id);
    sleep(2);
    while(times-->=0)
	{
		sleep( rand()%3+1 );//���]�C�ӭ��Ǯa�|���1~3��
		
		time_t begin= time(NULL);
		printf("Philosopher %d �j�F\n",id);
		take_fork(id);
		
    	waiting_time+=time(NULL)-begin;
    	printf ("Philosopher %d �}�l�Y\n",id);
    	sleep( rand()%3+1 );//���]�C�ӭ��Ǯa�|�Y1~3�� 
    	put_fork(id);
	}
    printf("Philosopher %d ���ƨ�F\n",id);
    put_fork(id);//�]������� ����]�����Ʀ�thread�������ٮ��۸_�l��deadlock 
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
	//�����s���C�� �A������ 
	pthread_mutex_lock(&chopstick[first_take]);
    printf("Philosopher %d ���_%d���_�l\n", id, first_take);
    pthread_mutex_lock(&chopstick[second_take]);
    printf("Philosopher %d ���_%d���_�l\n", id, second_take);
    
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
	printf("Philosopher %d ��U%d���_�l\n", id, second_take);
    pthread_mutex_unlock(&chopstick[second_take]);
    printf("Philosopher %d ��U%d���_�l\n", id, first_take);
    pthread_mutex_unlock(&chopstick[first_take]);
} 
int main()
{
	int i = 0;
	srand(time(NULL));//���üƷ|���� 
	
	pthread_mutexattr_init(&mutex_attr);//��l����mutex������ 
    pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_ERRORCHECK_NP);//�]���˿���A����unlock�O�H���F�� 
	
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
	    pthread_join(philosopher[i], NULL);//�� philosopher���槹 
	} 
	printf("total wating time = %ld",waiting_time);
    for(i = 0;i<N;i++)
	{
		pthread_mutex_destroy(&chopstick[i]);
	}
	
	return 0;
}
