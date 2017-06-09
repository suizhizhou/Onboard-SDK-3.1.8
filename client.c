/*
 * client.c
 *
 *  Created on: Apr 27, 2017
 *      Author: root
 */
#include <sys/socket.h>
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
#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>

#define cmd 0x01
#define dat 0x02
#define request 0x03
#define reply 0x04
#define xcontrol 0x05
#define zcontrol 0x06
#define stop  0x07

#define DEV_PATH "/dev/input/event1"   //difference is possible

#define head_size sizeof(head)

sem_t s;

typedef struct position{
	  float x;
	  float y;
	  float z;
	  int rotate;
	  double latitude;
	  double longitude;
}position;

position flight_position;

typedef struct head{
	  char protocol;
	  int message_length;
	  char cmdtype;
}head;
head flight_head;

struct Param{
	int fc;
	char ip[16];
	int num; // the client numbers
};
#define MAX 50
struct Param cs[MAX];
sem_t sem[MAX];

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;


char msg[1000];

float add_z = 0;
int local_rotate;

void* func(void* p){
	int *q = p;
	for(;;){
		int n = read(*q,msg,sizeof(msg));
		if(n<=0) break;//检查是否出错
		memcpy(&flight_head,msg,sizeof(head));

		if( (flight_head.protocol == 0x01) && (flight_head.message_length == 1000) && (flight_head.cmdtype == reply) ){
			sem_post(&s);
		}

		if( (flight_head.protocol == 0x01) && (flight_head.message_length == 1000) && (flight_head.cmdtype == dat) ){
			memcpy(&flight_position,msg+head_size,sizeof(position));
			printf("x:%f  y:%f  z:%f\n",flight_position.x,flight_position.y,flight_position.z);
			printf("rotate:%d\n",flight_position.rotate);
			printf("latitude:%f  longitude:%f\n",flight_position.latitude*57.295740,flight_position.longitude*57.295740);
			add_z = flight_position.z;
			local_rotate = flight_position.rotate;
			sem_post(&s);
		}

		memset(msg, 0, sizeof(msg));
	}
	close(*q);
	return q;
}

void *server(void* p){
	flight_head.protocol = 0x01;
	flight_head.message_length = 1000;
	struct Param* r = p;

	pthread_mutex_lock(&m);
	  int i = r->num;
	pthread_mutex_unlock(&m);

	sem_init(&s,0,0);
	pthread_t id_server;
	pthread_create(&id_server,NULL,func,&(r->fc));

	if(i!=0){
		sem_wait(&sem[i]); //console is the first pthread
	}

	for(;;){
		memset(msg, 0, sizeof(msg));
		printf(" this is pthread %d\n",i);
		printf("|------------------DJI Onboard SDK Interactive Sample------------|\n");
		printf("|                                                                |\n");
		printf("|   The interactive mode allows you to execute a few commands    |\n");
		printf("| to help you get a feel of the DJI Onboard SDK. Try them out!   |\n");
		printf("|                                                                |\n");
		printf("| Standard DJI Onboard SDK Usage Flow:                           |\n");
		printf("| 1. Activate (The sample has already done this for you)         |\n");
		printf("| 2. Obtain Control (The sample has already done this for you)   |\n");
		printf("| 3. Takeoff                                                     |\n");
		printf("| 4. Execute Aircraft control (Movement control/Missions/Camera) |\n");
		printf("| 5. Return to Home/Land                                         |\n");
		printf("| 6. Release Control (The sample will do this for you on exit)   |\n");
		printf("|                                                                |\n");
		printf("| Available commands:                                            |\n");
		printf("| [a] Request Control                                            |\n");
		printf("| [b] Release Control                                            |\n");
		printf("| [c] Arm the Drone                                              |\n");
		printf("| [d] Disarm the Drone                                           |\n");
		printf("| [e] Takeoff                                                    |\n");
		printf("| [f] Waypoint Sample                                            |\n");
		printf("| [g] Position Control Sample: Draw Square                       |\n");
		printf("| [h] Landing                                                    |\n");
		printf("| [i] Go Home                                                    |\n");
		printf("| [m] Move                                                       |\n");
		printf("| [x] xcontral                                                   |\n");
		printf("| [z] zcontral                                                   |\n");
		printf("| [s] reset                                                      |\n");
		printf("| [p] Printf Location                                            |\n");
		printf("| [r] Rotate Control                                             |\n");
		printf("| [j] Exit this sample                                           |\n");
		printf("| [q] change the console                                         |\n");
		printf("|                                                                |\n");
		printf("| Type one of these letters and then press the enter key.        |\n");
		printf("|                                                                |\n");
		printf("| If you're new here, try following the usage flow shown above.  |\n");
		printf("|                                                                |\n");
		printf("| Visit developer.dji.com/onboard-sdk/documentation for more.    |\n");
		printf("|                                                                |\n");
		printf("|------------------DJI Onboard SDK Interactive Sample------------|\n");

		while(1){
		  if((scanf(" %[^\n]",msg+head_size))==1&&(msg[head_size]<='z'&&msg[head_size]>='a')) {
				if(msg[head_size]=='s')
					msg[head_size]='7';
				break;
			}
		  else{
			printf("input error\n");
			printf("please try again:\n");
		  }
		  getchar();     //接收输入的回车符
		}

		flight_head.cmdtype = cmd;
		memcpy(msg,&flight_head,sizeof(head));
		if(msg[head_size]!='q'){
			write(r->fc,msg,sizeof(msg));
			printf("send ok\n");
		}
		usleep(50);
		if(msg[head_size]=='q'){
			if(cs[i+1].num == 0 && (i+1)<=50 ){
				sem_post(&sem[0]);
			}
			else{
				sem_post(&sem[i+1]);
			}
			sem_wait(&sem[i]);
		}

		if(msg[head_size]=='j'){
				exit(0);
		}

		if(msg[head_size]=='e'){
			add_z = add_z + 1;
		}

		/******************** m **************************/
		if(msg[head_size]=='m'&&( add_z!=0 )){
			printf("x and y are offsets to  North and the East directions\n");
			printf("move x,y,z  please input:x,y,z (meter)\n");
			printf("x,y,z limit form 0 to 20\n");
			while(1){
				printf("x:\n");
				if(scanf("%f",&flight_position.x)==1&&(flight_position.x<=20&&flight_position.x>=-20))
					break;
				else{
				  printf("input error\n");
				  printf("please try again:\n");
					fflush(stdin);
			}
			getchar();
			}
			while(1){
				printf("y:\n");
				if(scanf("%f",&flight_position.y)==1&&(flight_position.y<=20&&flight_position.y>=-20))
					break;
				else{
				  printf("input error\n");
				  printf("please try again:\n");
					fflush(stdin);
			}
			getchar();
			}
			while(1){
				printf("z:\n");
				if(scanf("%f",&flight_position.z)==1&&(flight_position.z<=20&&flight_position.z>=-20))
					break;
				else{
				  printf("input error\n");
				  printf("please try again:\n");
				  fflush(stdin);
			}
			getchar();
			}
			printf("x,y,z:%f,%f,%f\n",flight_position.x,flight_position.y,flight_position.z);

			add_z = add_z + flight_position.z;
			//add_x = add_x + flight_position.x;
			//add_y = add_y + flight_position.y;

			if( add_z < 0){
				printf(" warning low altitude! you may want to land !\n");
				add_z = add_z - flight_position.z;
				flight_position.z = 0;
		  }

			flight_head.cmdtype = dat;
			memset(msg, 0, sizeof(msg));
			memcpy(msg,&flight_head,sizeof(head));
			memcpy(msg+head_size,&flight_position,sizeof(position));
			write(r->fc,msg,sizeof(msg));
			sleep(10);
		}

		if((msg[head_size]=='m')&&( add_z==0 )){
			printf("the aircraft needs to takeoff!\n");
		}
		/******************** m **************************/

		/******************** p **************************/
		/*if(msg[head_size]=='p'){
			flight_head.cmdtype = inquire;
			memset(msg, 0, sizeof(msg));
			memcpy(msg,&flight_head,sizeof(head)); //only head send
			write(fd,msg,sizeof(msg));
		}*/
		/******************** p **************************/

		/******************** r **************************/
		if(msg[head_size]=='r'){
			printf("Due north direction is 0 degrees\n");
			printf("r degrees clockwise\n");
			printf("r limit form -180 to 180\n");
			while(1){
				printf("r:\n");
				if(scanf("%u",&flight_position.rotate)==1&&(flight_position.rotate<=180&&flight_position.rotate>=-180))
					break;
				else{
				  printf("input error\n");
				  printf("please try again:\n");
				  fflush(stdin);
			}
			getchar();
			}
			flight_head.cmdtype = dat;
			memcpy(msg,&flight_head,sizeof(head));
			memcpy(msg+head_size,&flight_position,sizeof(position));
			write(r->fc,msg,sizeof(msg));
			//sleep(10);
		}
		/******************** r *****************************/

		/***************** wait for reply except for 'x' 'z' 'q' *******************/
		if(msg[head_size]!='x' && msg[head_size]!='z' && msg[head_size]!='q'){
			sem_wait(&s);
		}

		/******************** x *****************************/
		if(msg[head_size]=='x'){
			getchar();
			sem_wait(&s);
			char xcmd;
			while(1){
				printf("control the aircraft by 1.2.3.4.6.8.0.5.7\n");
				printf("2 : go ahead towards the head\n");
				printf("4 : go left towards the head\n");
				printf("6 : go right towards the head\n");
				printf("8 : go back towards the head\n");
				printf("5 : go up towards the head\n");
				printf("0 : go down towards the head\n");
				printf("1 : Turn clockwise by 10 degrees\n");
				printf("3 : Rotate 10 degrees counterclockwise\n");
				printf("7 : quit the mode\n");
				while(1){
					printf("input cmd:\n");
					//if(scanf("%c",&xcmd)==1 && (xcmd<='8'&&xcmd>='0'))
					if(scanf("%c",&xcmd)==1 && ((xcmd == '0') || (xcmd == '1')|| (xcmd == '2')|| (xcmd == '3')|| (xcmd == '4')|| (xcmd == '5')|| (xcmd == '6')|| (xcmd == '7')|| (xcmd == '8')))
						break;
					else{
					  printf("input error\n");
					  printf("please try again:\n");
					  fflush(stdin);
					}
					getchar();
				}
				getchar();

				flight_head.cmdtype = xcontrol;
				memcpy(msg,&flight_head,sizeof(head));
				msg[head_size] = xcmd;
				write(r->fc,msg,sizeof(msg));
				sem_wait(&s);
				if(xcmd=='7')
					break;
			}
		}
		/******************** x *****************************/

		/******************** z *****************************/

		if(msg[head_size]=='z'){
			getchar();
			sem_wait(&s);
			int keys_fd;
			struct input_event t;
			keys_fd=open(DEV_PATH, O_RDONLY);
			if(keys_fd <= 0)
			{
				printf("open /dev/input/event1 device error!\n");
				exit(0);
			}
			printf("control the aircraft by 1.2.3.4.6.8.0.5.7\n");
			printf("2 : go ahead towards the head\n");
			printf("4 : go left towards the head\n");
			printf("6 : go right towards the head\n");
			printf("8 : go back towards the head\n");
			printf("5 : go up towards the head\n");
			printf("0 : go down towards the head\n");
			printf("1 : Turn clockwise by 10 degrees\n");
			printf("3 : Rotate 10 degrees counterclockwise\n");
			printf("7 : quit the mode\n");
			while(1){
				if(read(keys_fd , &t, sizeof(t)) == sizeof(t))
				{
					/********* 1 ********/
					if(t.type == EV_KEY &&  t.code == 79){
						if(t.value == 1 ){// Pressed
								printf("Turn clockwise\n");
							}
							if(t.value == 0){
								printf("wait for reply\n");
								/*** stop ***/
								flight_head.cmdtype = stop;
								memcpy(msg,&flight_head,sizeof(head));
								msg[head_size] = '1';
								write(r->fc,msg,sizeof(msg));
								sem_wait(&s);
							}
							printf("send ok\n");
							close(keys_fd);

							flight_head.cmdtype = zcontrol;
							memcpy(msg,&flight_head,sizeof(head));
							msg[head_size] = '1';
							write(r->fc,msg,sizeof(msg));
							usleep(1500000);

							keys_fd=open(DEV_PATH, O_RDONLY);
					}

					/********* 3 ********/
					if(t.type == EV_KEY &&  t.code == 81){
						if(t.value == 1 ){// Pressed
								printf("Turn clockwise\n");
							}
							if(t.value == 0){
								printf("wait for reply\n");
								/*** stop ***/
								flight_head.cmdtype = stop;
								memcpy(msg,&flight_head,sizeof(head));
								msg[head_size] = '3';
								write(r->fc,msg,sizeof(msg));
								sem_wait(&s);
							}
							printf("send ok\n");
							close(keys_fd);

							flight_head.cmdtype = zcontrol;
							memcpy(msg,&flight_head,sizeof(head));
							msg[head_size] = '3';
							write(r->fc,msg,sizeof(msg));
							usleep(1500000);

							keys_fd=open(DEV_PATH, O_RDONLY);
					}
					/********* 2 ********/
					if(t.type == EV_KEY &&  t.code == 80){
						if(t.value == 1 ){// Pressed
								printf("Turn clockwise\n");
								flight_head.cmdtype = zcontrol;
								memcpy(msg,&flight_head,sizeof(head));
								msg[head_size] = '2';
								write(r->fc,msg,sizeof(msg));
							}
							if(t.value == 0){
								printf("wait for reply\n");
								/*** stop ***/
								flight_head.cmdtype = stop;
								memcpy(msg,&flight_head,sizeof(head));
								msg[head_size] = '2';
								write(r->fc,msg,sizeof(msg));
								sem_wait(&s);
							}
					}

					/********* 0 ********/
					if(t.type == EV_KEY &&  t.code == 82){
						if(t.value == 1 ){// Pressed
								printf("Turn clockwise\n");
							}
							if(t.value == 0){
								printf("wait for reply\n");
								/*** stop ***/
								flight_head.cmdtype = stop;
								memcpy(msg,&flight_head,sizeof(head));
								msg[head_size] = '0';
								write(r->fc,msg,sizeof(msg));
								sem_wait(&s);
							}
							printf("send ok\n");
							close(keys_fd);

							flight_head.cmdtype = zcontrol;
							memcpy(msg,&flight_head,sizeof(head));
							msg[head_size] = '0';
							write(r->fc,msg,sizeof(msg));
							usleep(1500000);

							keys_fd=open(DEV_PATH, O_RDONLY);
					}

					/********* 4 ********/
					if(t.type == EV_KEY &&  t.code == 75){
						if(t.value == 1 ){// Pressed
								printf("Turn clockwise\n");
								flight_head.cmdtype = zcontrol;
								memcpy(msg,&flight_head,sizeof(head));
								msg[head_size] = '4';
								write(r->fc,msg,sizeof(msg));
							}
							if(t.value == 0){
								printf("wait for reply\n");
								/*** stop ***/
								flight_head.cmdtype = stop;
								memcpy(msg,&flight_head,sizeof(head));
								msg[head_size] = '4';
								write(r->fc,msg,sizeof(msg));
								sem_wait(&s);
							}
					}
					/********* 5 ********/
					if(t.type == EV_KEY &&  t.code == 76){
						if(t.value == 1 ){// Pressed
							printf("Turn clockwise\n");
						}
						if(t.value == 0){
							printf("wait for reply\n");
							/*** stop ***/
							flight_head.cmdtype = stop;
							memcpy(msg,&flight_head,sizeof(head));
							msg[head_size] = '5';
							write(r->fc,msg,sizeof(msg));
							sem_wait(&s);
						}
						printf("send ok\n");
						close(keys_fd);

						flight_head.cmdtype = zcontrol;
						memcpy(msg,&flight_head,sizeof(head));
						msg[head_size] = '5';
						write(r->fc,msg,sizeof(msg));
						usleep(1500000);

						keys_fd=open(DEV_PATH, O_RDONLY);
					}

					/********* 6 ********/
					if(t.type == EV_KEY &&  t.code == 77){
						if(t.value == 1 ){// Pressed
								printf("Turn clockwise\n");
								flight_head.cmdtype = zcontrol;
								memcpy(msg,&flight_head,sizeof(head));
								msg[head_size] = '6';
								write(r->fc,msg,sizeof(msg));
							}
							if(t.value == 0){
								printf("wait for reply\n");
								/*** stop ***/
								flight_head.cmdtype = stop;
								memcpy(msg,&flight_head,sizeof(head));
								msg[head_size] = '6';
								write(r->fc,msg,sizeof(msg));
								sem_wait(&s);
							}

					}

					/********* 8 ********/
					if(t.type == EV_KEY &&  t.code == 72){
							if(t.value == 1 ){// Pressed
								printf("Turn clockwise\n");
								flight_head.cmdtype = zcontrol;
								memcpy(msg,&flight_head,sizeof(head));
								msg[head_size] = '8';
								write(r->fc,msg,sizeof(msg));
							}
							if(t.value == 0){
								printf("wait for reply\n");
								/*** stop ***/
								flight_head.cmdtype = stop;
								memcpy(msg,&flight_head,sizeof(head));
								//msg[head_size] = '9';
								write(r->fc,msg,sizeof(msg));
								sem_wait(&s);
							}
					}


					/********* 7 ********/
					if(t.type == EV_KEY &&t.code == KEY_ESC)
					{
						flight_head.cmdtype = zcontrol;
						memcpy(msg,&flight_head,sizeof(head));
						msg[head_size] = '7';
						write(r->fc,msg,sizeof(msg));
						fflush(stdin);
						break;
					}
				}
			}
		}
		sleep(2);
	}
	close(r->fc);
	return r;
}



int main(void)
{
	int j;
	for(j=0;j<MAX;j++){
		sem_init(&sem[j],0,0);
	}
	int fd = socket(AF_INET,SOCK_STREAM,0);
	//ipv4,AF_INET6---v6
	//SOCK_STREAM----tcp
	if(fd<0){
		perror("socket");
		exit(0);
	}
	struct sockaddr_in si;//sockaddr_in6
	si.sin_family = AF_INET;
	si.sin_addr.s_addr = htonl(INADDR_ANY);
	si.sin_port = htons(8888);
	//bind(int,sockaddr*,size_t);
	if(bind(fd,(struct sockaddr*)&si,sizeof(si))<0){
		perror("bind");
		exit(0);
	}

	listen(fd,10);
	for(;;){
		struct sockaddr_in c;
		socklen_t len = sizeof(c);
		int fc = accept(fd,(struct sockaddr*)&c,&len);
		if(fc<0) continue;
		int i;
		pthread_mutex_lock(&m);
		for(i=0;i<MAX&&cs[i].fc!=0;i++){};
		if(i==MAX){
			write(fc,"full\n",5);
			close(fc);
		}
		else{
			cs[i].fc = fc;
			inet_ntop(AF_INET,&c.sin_addr,cs[i].ip,16);
			cs[i].num = i;
			pthread_t id;
			pthread_create(&id,NULL,server,&cs[i]);
		}
		pthread_mutex_unlock(&m);
	}
}















