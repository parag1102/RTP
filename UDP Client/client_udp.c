#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void error(const char *);
void encode(short co,short ty,short st,char bu[],char* str);
void retreive(char *string,short* co,short* ty,short* st,char* bu);
int main(int argc, char *argv[])
{
   int sock, n;
   unsigned int length;
   struct sockaddr_in server, from;
   struct hostent *hp;
   char buf[50];
	short connection_id = -1;
	short type = 1;
	short status = 0;
	char buffer[20]; 
	printf("Enter the path\n");				//enter the remote file path
	scanf("%s",buffer);

	encode(connection_id,type,status,buffer,buf);
  	//printf("%s",buf);

   if (argc != 3) { printf("Usage: server port\n");
                    exit(1);
   }
   sock= socket(AF_INET, SOCK_DGRAM, 0);
   if (sock < 0) error("socket");

   server.sin_family = AF_INET;
   hp = gethostbyname(argv[1]);
   if (hp==0) error("Unknown host");

   bcopy((char *)hp->h_addr, 
        (char *)&server.sin_addr,
         hp->h_length);
   server.sin_port = htons(atoi(argv[2]));
   length=sizeof(struct sockaddr_in);
   /*printf("Please enter the message: ");
   bzero(buffer,256);
   fgets(buffer,255,stdin);*/
   n=sendto(sock,buf,
            strlen(buf),0,(const struct sockaddr *)&server,length);
   if (n < 0) error("Sendto");

   n = recvfrom(sock,buf,50,0,(struct sockaddr *)&from, &length);
   if (n < 0) error("recvfrom");
	
	retreive(buf,&connection_id,&type,&status,buffer);
   	if(type==2)
		{    
   		write(1,"Got REQUEST_ACK: ",17);
   		write(1,buf,n);
   		printf("\n");
	
		n = recvfrom(sock,buf,50,0,(struct sockaddr *)&from, &length);
		if (n < 0) error("recvfrom");

		retreive(buf,&connection_id,&type,&status,buffer);
		if(type==3)
			{
			write(1,"Got DONE: ",10);
			write(1,buf,n);
			printf("\n");
			
			printf("Sending DONE_ACK\n");

			connection_id=14;
			type=4;
			status=0;
			int j=0;		
			while(buffer[j]!='\0')
				{
				buffer[j]='0';
				j++;
				}		
			encode(connection_id,type,status,buffer,buf);
			n=sendto(sock,buf,
            		strlen(buf),0,(const struct sockaddr *)&server,length);
   			if (n < 0) error("Sendto");
       			}
		}
   
	close(sock);
  	return 0;
}

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

void encode(short co,short ty,short st,char bu[],char* str)
	{
	//char str[50];	
	char c[10],t[10],s[10];
	sprintf(c,"%d",(int) co);	
	sprintf(t,"%d",(int) ty);
	sprintf(s,"%d",(int) st);
	strcpy(str,c);	
	strcat(str,"$");
	strcat(str,t);
	strcat(str,"$");
	strcat(str,s);
	strcat(str,"$");
	strcat(str,bu);	
	strcat(str,"#");
	//printf("%s\n",str);
	//return str;	
	}

void retreive(char *string,short* co,short* ty,short* st,char* bu)
	{
	char c[10],t[10],s[10],buf[256];		
	char *p,*q;
	int i=0,j=0,k=0,l=0,m=0;		
	p=string;
	q=string;


	while(*p!='$')
		p++;
	while(q!=p)
		{
		c[i]=*q;
		i++;
		q++;
		}
	c[i]='\0';

	p++;q++;


	while(*p!='$')
		p++;
	while(q!=p)
		{
		t[j]=*q;
		j++;
		q++;
		}
	t[j]='\0';

	p++;q++;


	while(*p!='$')
		p++;
	while(q!=p)
		{
		s[k]=*q;
		k++;
		q++;
		}
	s[j]='\0';

	p++;q++;


	while(*p!='#')
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
