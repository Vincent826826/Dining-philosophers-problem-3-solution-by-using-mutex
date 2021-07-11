#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include <time.h>
#include <windows.h>
#include<pthread.h>

//ż�_�l 0�����Ǯa�i�H��4��0  1����0��1 
//����_�l�٨S�Y lock�_�� ����Y���_�l��żunlock 
#define N 5
enum{
	THINKING,
	HUANGRY,
	EATING,
};//���Ǯa�����A 
enum{
	DIRTY,
	CLEAN
};//�_�l�����A 
 
pthread_t philosopher[N];//�]�w���Ǯa 
pthread_mutex_t chopstick[N];//�]�w�_�l 
int chopstick_status[2][N];//�Ĥ@���_�l�{�b�Q�֮� �ĤG���_�lż��ż
int  philosopher_status[N];//���Ǯa�����A 
int times = 10;//�`���榸�� 
time_t waiting_time = 0;

void *start(int *who) 
{
    int id = (int*)who;
    printf("Philosopher %d ���U\n",id);
    if(id!=N-1)//��l�� 
	{
		chopstick_status[0][id] = id;
		chopstick_status[1][id] = DIRTY;
		if(id==0)
		{
			chopstick_status[0][ N-1 ] = id;
			chopstick_status[1][ N-1 ] = DIRTY;//�s���̤p���H�h���s���̤j���_�l 
		}
	}
    sleep(3);//���Ҧ��H���i�� 
    while(times-->=0)//����j�a�Y�����`���� 
	{
		think(id);
		huangry(id);
	}
	printf("Philosopher %d ���ƨ�F\n",id);
    return NULL;
}
void think(int id)
{
	printf("Philosopher %d �}�l���\n",id);
	philosopher_status[id] =  THINKING;
	sleep( rand()%3+1 );//���]�C�ӭ��Ǯa�|���1~3��
}
void huangry(int id)
{
	philosopher_status[id] =  HUANGRY;
	printf("Philosopher %d �j�F\n",id);
	int first = (id+N-1)%5,second = id;//�����s���C�� �A������ 
	if(first > second)
	{
		first = id;
		second = (id+N-1)%5;
	}
	time_t begin= time(NULL);
	
	request(id,first);
	request(id,second);//��ݭn���_�l�o�erequest 
	
    waiting_time+=time(NULL)-begin;//�p�ⵥ�h�[�~�}�l�Y
	eating(id,first,second); 
}
void request(int id,int chopstick_id)
{
	if(chopstick_status[0][chopstick_id] != id)//�_�l���b�ڤ�W 
	{
		int chopstick_owner = chopstick_status[0][chopstick_id];//���o�Ӹ_�l���H
		if(pthread_mutex_lock(&chopstick[chopstick_id])==0)
		{
				chopstick_status[0][chopstick_id] = id;//�_�l���n���H 
				chopstick_status[1][chopstick_id] = CLEAN;//�����b	
				printf("Philosopher %d �q%d���䮳�F%d���_�l\n",id,chopstick_owner,chopstick_id);
		}
	}
	else
	{
		pthread_mutex_lock(&chopstick[chopstick_id]);
		printf("Philosopher %d �Φۤv��%d���_�l\n",id,chopstick_id);	
	} 
}
void eating(int id,int first,int second)
{
	//pthread_mutex_lock(&mutex);
	printf ("Philosopher %d �}�l�Y\n",id);
	philosopher_status[id] =  EATING;
	chopstick_status[1][first] = DIRTY;
	chopstick_status[1][second] = DIRTY;
	sleep( rand()%3+1 );//���]�C�ӭ��Ǯa�|�Y1~3��
	printf ("Philosopher %d �Y���F\n",id);
	pthread_mutex_unlock(&chopstick[first]);//�Y�������X�_�l���b�����H�i�� 
	pthread_mutex_unlock(&chopstick[second]);
	
	//pthread_mutex_unlock(&mutex);
}
int main()
{
	int i = 0;
	srand(time(NULL));//���üƷ|���� 
	
	for(i = 0;i<N;i++)
	{
		pthread_mutex_init(&chopstick[i],NULL);//�]���@�몺�� ����L�H�i�Hunlock�O�H���_�l 
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
