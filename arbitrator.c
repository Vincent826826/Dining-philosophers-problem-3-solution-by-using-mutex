#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<time.h>
#include<windows.h>
#include<pthread.h>

//�����n���N��2�� ���_�l�ܦ�atomic�� ��_�l���O ��mutuex������ 
//���|�����j�����D�]�� �j�a�|�h�mmutex�ƶ� 
#define N 5//���Ǯa���ƶq 

pthread_t philosopher[N];//�]�w���Ǯa 
pthread_mutex_t chopstick[N];//�]�w�_�l 
pthread_mutex_t mutex;//�@���u���@�� philosopher�i�H���_�l 
pthread_mutexattr_t mutex_attr; //mutex���ݩ� 
int times = 10;//�ҭ��Ǯa��Y�����`���� 
time_t waiting_time = 0;//�Ҧ����Ǯa���h�[ 

void *start(int *who) 
{
    int id = (int*)who;
    printf("Philosopher %d ���U\n",id);
    sleep(3);
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
	pthread_mutex_lock(&mutex);
	bool left = pthread_mutex_trylock(&chopstick[id])==0;
	bool right = pthread_mutex_trylock(&chopstick[(id+1)%5])==0;
	if(left&&right)//��Ӹ_�l���i�H���~�� 
	{
    	printf("Philosopher %d ���_%d�� �� %d���_�l\n", id, id, (id+1)%5);
    	pthread_mutex_unlock(&mutex);
	}
    else//�p�G�u���@�ӥi�H���N��^�h 
	{
		if(left)
		{
			//printf("Philosopher %d ���_%d���_�l�S��U�FQQ\n", id, id);
			pthread_mutex_unlock(&chopstick[id]);
		}
		if(right)
		{
			//printf("Philosopher %d ���_%d���_�l�S��U�FQQ\n", id, (id+1)%5);
			pthread_mutex_unlock(&chopstick[(id+1)%5]);
		} 
		pthread_mutex_unlock(&mutex);
		Sleep(1);//�קK�L���j�� ����1�@�� 
		take_fork(id);
	}
    
}
/*void take_fork(int id) //�t�@�ذ��k�����Philosopher�j�F�N��������2�Ӹ_�l����ΡA�����j�a������A�� 
{//�n�B�̦h��3�� 
	
	pthread_mutex_lock(&mutex);
	if(pthread_mutex_lock(&chopstick[id])==0)printf("Philosopher %d ���_%d���_�l",id,id);
	if(pthread_mutex_lock(&chopstick[(id+1)%5])==0)printf("Philosopher %d ���_%d���_�l\n", id, (id+1)%5);
    pthread_mutex_unlock(&mutex);
}*/
void put_fork(int id)
{
	//pthread_mutex_lock(&mutex);//�[�F�n���ܤ[�|�j�a���b�mmutex 
	if(pthread_mutex_unlock(&chopstick[(id+1)%5])==0)
		printf("Philosopher %d ��U%d���_�l\n", id, (id+1)%5);
    if(pthread_mutex_unlock(&chopstick[id])==0)
    	printf("Philosopher %d ��U%d���_�l\n", id, id);
    
    //pthread_mutex_unlock(&mutex);
} 
int main()
{
	int i = 0;
	srand(time(NULL));//���üƷ|���� 
	
	pthread_mutexattr_init(&mutex_attr);//��l����mutex������ 
    pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_ERRORCHECK_NP);//�]���˿���A����unlock�O�H���F�� 
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
	    pthread_join(philosopher[i], NULL);//���Ҧ�philosopher���槹 
	} 
	printf("total wating time = %ld",waiting_time);
    for(i = 0;i<N;i++)
	{
		pthread_mutex_destroy(&chopstick[i]);
	}
	
	return 0;
}
