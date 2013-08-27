#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <time.h>
#include <sys/stat.h>
#include <pthread.h>

pthread_cond_t condition_var = PTHREAD_COND_INITIALIZER;

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

char t_stamp[50];				//stores the timestamp of the file (global)

struct thr					//the threads would be stored as a linklist of structs
	{
	pthread_t* tid;	
	int con;
	struct thr* next;
	}*ll;

void error(const char *msg)			//function to prompt for an error
{
    perror(msg);
    exit(0);
}
						//global function declarations

void encode(short co,short ty,short st,char bu[],char* str);			//converts packet to a string
void retreive(char *string,short* co,short* ty,short* st,char* bu);		//decodes string to get the packet
void *get_timestamp(void*);							//derives the timestamp of the file in the global variable 't_stamp'

int main(int argc, char *argv[])						//takes port number as the argument						
{
		//packet structure	
	short connection_id = 0;
	short type = 0;
	short status = 0;
	char buffer[50];
		
		//defining socket
   int sock, length, n;
   socklen_t fromlen;
   struct sockaddr_in server;
   struct sockaddr_in from;
   char buf[50];

   if (argc < 2) {
      fprintf(stderr, "ERROR, no port provided\n");				//exception handling
      exit(0);
   }
   
   sock=socket(AF_INET, SOCK_DGRAM, 0);						//opening the socket
   if (sock < 0) error("Opening socket");
   length = sizeof(server);
   bzero(&server,length);
   server.sin_family=AF_INET;
   server.sin_addr.s_addr=INADDR_ANY;
   server.sin_port=htons(atoi(argv[1]));
   if (bind(sock,(struct sockaddr *)&server,length)<0) 
       error("binding");
   fromlen = sizeof(struct sockaddr_in);
	
	ll=NULL;								//initialising the head of the linklist that stores the thread
	pthread_t thread[4];   					
	int  iret1;
	int i=0;

	//for(i=0;i<4;i++)
	while(1) {
	n = recvfrom(sock,buf,50,0,(struct sockaddr *)&from,&fromlen);		//recieving the string from the client in 'buf'
       	if (n < 0) error("recvfrom");

	retreive(buf,&connection_id,&type,&status,buffer); 			//decoding the contents of 'buf' to get the packet contents      
	if((type==1)&&(connection_id==-1))
		{
		write(1,"Received REQUEST packet: ",25);
		write(1,buf,n);
		printf("\n");
		
		/*struct stat st;
		char *date;
		int time=stat(buffer,&st);					//syscall to get the creation timestamp
		date = asctime(localtime(&st.st_ctime));*/		
		
		iret1 = pthread_create( &thread[i], NULL, get_timestamp, (void*) buffer);	//creating threads for each request
		struct thr* p_to_thr=(struct thr*)malloc(sizeof(struct thr));			//creatng a new node
		p_to_thr->tid=&thread[i];
		p_to_thr->con=i;
		p_to_thr->next=NULL;		
		if(ll==NULL)									//inserting the node
			{		
			ll=p_to_thr;		
			}
		else
			{
			struct thr* q=ll;			
			while(q->next!=NULL)
				q=q->next;
			
			q->next=p_to_thr;
			}
		
		pthread_join( thread[i], NULL);					//wait for the thread[i] to get completed

		printf("Sending REQUEST_ACK\n");
		
		connection_id=4;
		type=2;		
		status=0;
		int i=0;		
		while(buffer[i]!='\0')
			{
			buffer[i]='0';
			i++;
			}		
		encode(connection_id,type,status,buffer,buf);			//encoding the packet into a string
		
		n = sendto(sock,buf,strlen(buf),
                  0,(struct sockaddr *)&from,fromlen);
       		if (n  < 0) error("sendto");
		
		//printf("%s",buffer);
		

		printf("Sending DONE packet\n");

		connection_id=9;
		type=3;		
		status=0;
		int j=0;
		while(t_stamp[j]!='\0')
			{
			buffer[j]=t_stamp[j];					//storing the timestamp in buffer wich will then be sent to the client
			j++;
			}
		buffer[j]='\0';
		encode(connection_id,type,status,buffer,buf);			
		

		n = sendto(sock,buf,strlen(buf),
                  0,(struct sockaddr *)&from,fromlen);
       		if (n  < 0) error("sendto");
						
		
		pthread_mutex_lock( &mutex1 );	
			n = recvfrom(sock,buf,50,0,(struct sockaddr *)&from, &length);
			if (n < 0) error("recvfrom");

			retreive(buf,&connection_id,&type,&status,buffer);
			if(type==4)
				{
				write(1,"Got DONE_ACK: ",14);
				write(1,buf,n);
				printf("\n");
				}
		pthread_mutex_unlock(&mutex1);				
		}
	}   
   	return 0;
}

void encode(short co,short ty,short st,char bu[],char* str)			//function to encode the packet into a string
	{
	//char str[50];	
	char c[10],t[10],s[10];
	sprintf(c,"%d",(int) co);	
	sprintf(t,"%d",(int) ty);
	sprintf(s,"%d",(int) st);
	strcpy(str,c);	
	strcat(str,"$");							//'$' is used to separate the different fields
	strcat(str,t);
	strcat(str,"$");
	strcat(str,s);
	strcat(str,"$");
	strcat(str,bu);	
	strcat(str,"#");							//'#' is used as a end delimiter
	//printf("%s\n",str);
	//return str;	
	}

void retreive(char *string,short* co,short* ty,short* st,char* bu)		//function to decode the string to get the fileds of the packet
	{
	char c[10],t[10],s[10],buf[256];		
	char *p,*q;
	int i=0,j=0,k=0,l=0,m=0;		
	p=string;
	q=string;


	while(*p!='$')				//getting the connection_id
		p++;
	while(q!=p)
		{
		c[i]=*q;
		i++;
		q++;
		}
	c[i]='\0';

	p++;q++;


	while(*p!='$')				//getting the type
		p++;
	while(q!=p)
		{
		t[j]=*q;
		j++;
		q++;
		}
	t[j]='\0';

	p++;q++;


	while(*p!='$')				//getting the status
		p++;
	while(q!=p)
		{
		s[k]=*q;
		k++;
		q++;
		}
	s[j]='\0';

	p++;q++;


	while(*p!='#')				//getting the buffer
		p++;
	while(q!=p)
		{
		buf[l]=*q;
		l++;
		q++;
		}
	buf[l]='\0';

	*co=atoi(c);
	*ty=atoi(t);
	*st=atoi(s);
	while(buf[m]!='\0')
		{
		bu[m]=buf[m];
		m++;
		}		
	bu[m]='\0';
	
	//printf("\n%s\n%s\n%s\n%s\n",c,t,s,bu);

	}

void *get_timestamp( void* bug )			//getting the timestamp
	{
	struct stat st;
	char* bu=(char*) bug;	
	int j=0;	
	char *date;
	int time=stat(bu,&st);
	date = asctime(localtime(&st.st_ctime));
	while(date[j]!='\0')
		{
		t_stamp[j]=date[j];
		j++;
		}     	
	t_stamp[j]='\0';	
	//printf("%s \n",t_stamp);
	}
