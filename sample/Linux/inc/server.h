#ifndef SERVER_H_  
#define SERVER_H_  
  
#ifdef __cplusplus  
extern "C" {  
#endif  

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>
#include <math.h>

#define cmd 0x01
#define dat 0x02
#define request 0x03
#define reply 0x04
#define xcontrol 0x05
#define zcontrol 0x06
#define stop  0x07

#define head_size sizeof(head)

char msg[1000];

sem_t s; 
sem_t ss; 

struct Param{
	int fc;	
	char ip[16];
};

//pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

typedef struct position{
	  float x; 
	  float y; 
	  float z;
	  int rotate;
	  double latitude;
	  double longitude;
}position;

int add_x,add_y,add_z,local_rotate;
char xcmd ,zcmd;

position flight_position;

typedef struct head{
	  char protocol;
	  int message_length;
	  char cmdtype;
}head;
head flight_head;

void* server( );  

void* func(void* p);

void c_function(void);
  
#ifdef __cplusplus  
}  
#endif  
  
#endif /* CLIENT_H_ */