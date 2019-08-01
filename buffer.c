
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>


 typedef int buffer_item; 
 #define BUFFER_SIZE 5 


buffer_item buffer[BUFFER_SIZE];

pthread_mutex_t mutex;
sem_t full;
sem_t empty; 
int count=0, in=0, out=0;


int insert_item(buffer_item item);
int remove_item(buffer_item *item);
void *consumer(void *param);
void *producer(void *param);

int main(int argc, char **argv){
  
	if (argc != 4){
    printf("ERROR: Provide exactly three arguments.\n");
    exit(1);
  }

 
  const long int stime = strtol(argv[1], NULL, 0);
  const long int num_producer = strtol(argv[2], NULL, 0); 
  const long int num_consumer = strtol(argv[3], NULL, 0); 

 
  int i;
  srand(time(NULL));
  pthread_mutex_init(&mutex, NULL); 
  sem_init(&empty, 0, BUFFER_SIZE); 
  sem_init(&full, 0, 0);


  pthread_t producers[num_producer];
  pthread_t consumers[num_consumer];
  for(i = 0; i < num_producer; i++)
    pthread_create(&producers[i], NULL, producer, NULL);
  for(i = 0; i < num_consumer; i++)
    pthread_create(&consumers[i], NULL, consumer, NULL);

  
  sleep(stime);
  return 0;
}


int insert_item(buffer_item item){
  int r;
  sem_wait(&empty);
  pthread_mutex_lock(&mutex); 


  if( count != BUFFER_SIZE){ 
    buffer[in] = item;
    in = (in + 1) % BUFFER_SIZE;
    count++;
    r = 0;
  }
  else
    r = -1;

  pthread_mutex_unlock(&mutex); 
  sem_post(&full);
  
  return r; 
}


int remove_item(buffer_item *item){
  int r;
  
  sem_wait(&full);
  pthread_mutex_lock(&mutex);
  
 
  if( count != 0){
    *item = buffer[out];
    out = (out + 1) % BUFFER_SIZE;
    count--;
    r = 0;
  }
  else
    r = -1;

  pthread_mutex_unlock(&mutex);
  sem_post(&empty);
  
  return r;
}

void *producer(void *param){
  buffer_item item;
  while(1){
    sleep(rand() % 5 + 1); 
    
    item = rand();
    if(insert_item(item))
      printf("Error occured\n"); 
    else
      printf("Producer produced %d\n", item);
  }
}


void *consumer(void *param){
  buffer_item item;
  while(1){
    sleep(rand() % 5 + 1); 

    if(remove_item(&item))
      printf("Error occured\n");
    else
      printf("Consumer consumed %d\n", item);
  }
}