#include "server.h"

extern int time_break_flag;

void* func(void* p){
	
	int *q = p; 
	printf(" %d \n",*q);
	for(;;){
	       int n = read(4,msg,sizeof(msg));
		if(n<=0) break;
		memcpy(&flight_head,msg,sizeof(head));
		
		if( (flight_head.protocol == 0x01) && (flight_head.message_length == 1000) && (flight_head.cmdtype == cmd)){
			sem_post(&s); //��cmd ��ͬ���ź���
		}

		if( (flight_head.protocol == 0x01) && (flight_head.message_length == 1000) && (flight_head.cmdtype == xcontrol)){

			xcmd = msg[head_size];
			sem_post(&s); //��cmd ��ͬ���ź���
		}

		if( (flight_head.protocol == 0x01) && (flight_head.message_length == 1000) && (flight_head.cmdtype == zcontrol)){

			zcmd = msg[head_size];
			sem_post(&s); //��cmd ��ͬ���ź���
		}

		if( (flight_head.protocol == 0x01) && (flight_head.message_length == 1000) && (flight_head.cmdtype == stop)){
			//�յ�stop ֮֡����ִ��һ�β���������һ֡
			//zcmd = msg[head_size];
			time_break_flag = 1;
			printf(" changes changes \n");
			//sem_post(&s);
		}
		
		usleep(50);	//�ȴ�msg ��ֵ�������

		if( (flight_head.protocol == 0x01) && (flight_head.message_length == 1000) && (flight_head.cmdtype == dat) ){
			memcpy(&flight_position,msg+head_size,sizeof(position));
			sem_post(&s);
		}
		
		

		if (flight_head.cmdtype != zcontrol ){ //zcontrol ����Ҫ�ظ���Ϣ

			if( (msg[head_size] != 'm') && (msg[head_size] !='r') ){
				if(flight_head.cmdtype != stop)
					sem_wait(&ss);//��reply ��ͬ���ź���
			}
			
			flight_head.protocol = 0x01;
			flight_head.message_length = 1000;
			
			if((msg[head_size] == 'p')){
				flight_head.cmdtype = dat;
				memcpy(msg,&flight_head,sizeof(head));
				memcpy(msg+head_size,&flight_position,sizeof(position));
			}
			else{
				flight_head.cmdtype = reply;
				memcpy(msg,&flight_head,sizeof(head));
			}
			write(4,msg,sizeof(msg));
		}
		else{
			printf("no reply\n");
		}
		
		memset(msg, 0, sizeof(msg));
	}
	close(*q);	
	return q;
}


void c_function(void){
	int fd = socket(AF_INET,SOCK_STREAM,0);
	printf(" %d \n",fd);
	struct sockaddr_in c;
	c.sin_family = AF_INET;
	char ip[16];
	int f_ip = open("/home/nfs/NFS/Onboard-SDK-3.1.8/sample/Linux/ip.txt",O_RDONLY,16);
	if( f_ip!= -1){
		int k = read(f_ip, ip ,16);
	}
	else
		perror("open error\n");
	inet_pton(AF_INET,ip,&c.sin_addr);//�ѵ��ʮ���Ƹ�ʽip��ַת�����ڲ���ʽIP��ַ
	c.sin_port = htons(atoi("8888"));

	if(connect(fd,(struct sockaddr*)&c,sizeof(c))<0){
		perror("connect");
		exit(0);
	}

	pthread_t id;
	pthread_create(&id,NULL,func,&fd);
}





















