#include <stdio.h>
#include <stdlib.h>
#include <string.h>	   // memset
#include <pthread.h>   // pthread_t, pthread_create, pthread_join
#include <semaphore.h> // sem_init, sem_wait, sem_post
#include <time.h>
#include <unistd.h>

#define NUM_SEAT 3

#define SLEEP_MAX 5

//semaphores
sem_t sem_stu;
sem_t sem_ta;

//sem_t mutex;
pthread_mutex_t mutex;

int chair[3];
int count = 0; //number of waiting students
int next_seat = 0;
int next_teach = 0;

void rand_sleep(void);
void *stu_programming(void *stu_id);
void *ta_teaching();

void rand_sleep(void)
{
	int time = rand() % SLEEP_MAX + 1;
	sleep(time);
}

void rand_slp(void)
{
	int time = rand() % 2;
	sleep(time);
}

int main(int argc, char **argv)
{

	//thread(s)
	pthread_t *students;
	pthread_t *tas;

	int *student_ids;
	int student_num;
	int session_time;

	int ta_num;
	int *ta_ids;

	//index
	int i;

	//get number of students from user
	printf("\nHow many students? ");
	scanf("%d", &student_num);

	printf("\nhow many ta's? ");
	scanf("%d", &ta_num);
	printf("\nEnter session time(in seconds)? ");
	scanf("%d", &session_time);

	// for ending program after session time is expired
	time_t endwait;
	time_t start = time(NULL);
	time_t seconds = session_time; // end loop after this time has elapsed

	endwait = start + seconds;

	printf("\nsession started at time : %s\n", ctime(&start));

	//initialize
	students = (pthread_t *)malloc(sizeof(pthread_t) * student_num);
	student_ids = (int *)malloc(sizeof(int) * student_num);

	tas = (pthread_t *)malloc(sizeof(pthread_t) * ta_num);
	ta_ids = (int *)malloc(sizeof(int) * ta_num);

	memset(student_ids, 0, student_num);
	memset(ta_ids, 0, ta_num);

	sem_init(&sem_stu, 0, 0);
	sem_init(&sem_ta, 0, 1);

	//set random
	srand(time(NULL));

	//sem_init(&mutex,0,1);
	pthread_mutex_init(&mutex, NULL);

	//create thread
	//pthread_create(&tas,NULL,ta_teaching,NULL);

	//create ta threads
	for (i = 0; i < ta_num; i++)
	{
		ta_ids[i] = i + 1;
		pthread_create(&tas[i], NULL, ta_teaching, (void *)&ta_ids[i]);
	}

	//create student threads
	for (i = 0; i < student_num; i++)
	{
		student_ids[i] = i + 1;
		pthread_create(&students[i], NULL, stu_programming, (void *)&student_ids[i]);
	}

	while (start < endwait)
	{
		sleep(1);
		start = time(NULL);
	}

	for (i = 0; i < student_num; i++)
	{
		pthread_cancel(students[i]);
	}

	for (i = 0; i < ta_num; i++)
	{
		pthread_cancel(tas[i]);
	}

	printf("\nsession ended after %d seconds\n \nsession ended at time :  %s\n ", session_time, ctime(&endwait));

	return 0;
}

void *stu_programming(void *stu_id)
{
	int id = *(int *)stu_id;

	printf("[stu] student %d is programming\n", id);

	while (1)
	{
		rand_sleep();

		//sem_wait(&mutex);
		pthread_mutex_lock(&mutex);

		if (count < NUM_SEAT)
		{
			chair[next_seat] = id;
			count++;

			printf("\n	[stu] student %d is waiting\n", id);
			printf("\nwaiting students : [1] %d   [2] %d   [3] %d\n", chair[0], chair[1], chair[2]);
			next_seat = (next_seat + 1) % NUM_SEAT;

			//sem_post(&mutex);
			pthread_mutex_unlock(&mutex);

			//wakeup ta
			sem_post(&sem_stu);
			sem_wait(&sem_ta);
		}
		else //no more chairs
		{
			//sem_post(&mutex);
			pthread_mutex_unlock(&mutex);

			printf("\n[stu] no more chairs. student %d is programming\n", id);
		}
	}
}

void *ta_teaching(void *ta_id)
{
	int id = *(int *)ta_id;

	while (1)
	{
		rand_slp();
		sem_wait(&sem_stu);

		//sem_wait(&mutex);
		pthread_mutex_lock(&mutex);

		printf("\n		[ta] TA %d is teaching student %d\n", id, chair[next_teach]);
		chair[next_teach] = 0;
		count--;

		printf("\nwaiting students : [1] %d   [2] %d   [3] %d\n", chair[0], chair[1], chair[2]);
		next_teach = (next_teach + 1) % NUM_SEAT;

		pthread_mutex_unlock(&mutex);
		rand_sleep();

		printf("\n		[ta] TA %d teaching finish.\n", id);

		//sem_post(&mutex);
		// pthread_mutex_unlock(&mutex);

		sem_post(&sem_ta);
	}
}
