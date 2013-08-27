/* Creates a datagram server.  The port 
   number is passed as an argument.  This
   server runs forever */

#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

void encode(short co,short ty,short st,char bu[],char* str);
void retreive(char *string,short* co,short* ty,short* st,char* bu);
int main(int argc, char *argv[])
{
	short connection_id = 0;
	short type = 0;
	short status = 0;
	char buffer[20];

   int sock, length, n;
   socklen_t fromlen;
   struct sockaddr_in server;
   struct sockaddr_in from;
   char buf[50];

   if (argc < 2) {
      fprintf(stderr, "ERROR, no port provided\n");
      exit(0);
   }
   
   sock=socket(AF_INET, SOCK_DGRAM, 0);
   if (sock < 0) error("Opening socket");
   length = sizeof(server);
   bzero(&server,length);
   server.sin_family=AF_INET;
   server.sin_addr.s_addr=INADDR_ANY;
   server.sin_port=htons(atoi(argv[1]));
   if (bind(sock,(struct sockaddr *)&server,length)<0) 
       error("binding");
   fromlen = sizeof(struct sockaddr_in);
   while (1) {
       n = recvfrom(sock,buf,50,0,(struct sockaddr *)&from,&fromlen);
       if (n < 0) error("recvfrom");
       write(1,"Received a datagram: ",21);
       write(1,buf,n);
	retreive(buf,&connection_id,&type,&status,buffer);
       
	if(connection_id==-1)
		{
		printf("Received request packet\n");
		connection_id=4;
		type=2;		
		status=0;
		int i=0;		
		while(buffer[i]!='\0')
			{
			buffer[i]='0';
			i++;
			}		
		encode(connection_id,type,status,buffer,buf);
		}
	n = sendto(sock,buf,strlen(buf),
                  0,(struct sockaddr *)&from,fromlen);
       if (n  < 0) error("sendto");
   }
   return 0;
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
	
	printf("\n%s\n%s\n%s\n%s\n",c,t,s,bu);

	}
