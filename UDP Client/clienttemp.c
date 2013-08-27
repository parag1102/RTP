/* UDP client in the internet domain */
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
	printf("Enter the path\n");	
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
   write(1,"Got an ack: ",12);
   write(1,buf,n);
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
