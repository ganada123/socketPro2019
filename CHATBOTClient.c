#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>

#define PORT 11000
#define IPADDR "127.0.0.1"
#define BUFSIZE 100

int main()
{
	int c_socket,n;
	struct sockaddr_in c_addr;
	char rcvBuffer[BUFSIZE]={};
	int rcvBufferLen;
	char sendBuffer[BUFSIZE]={};
	int sendBufferLen;

	c_socket = socket(PF_INET, SOCK_STREAM, 0); 

	memset(&c_addr, 0, sizeof(c_addr));
	c_addr.sin_addr.s_addr = inet_addr(IPADDR); 
	c_addr.sin_family = AF_INET;
	c_addr.sin_port = htons(PORT);
	
	if(connect(c_socket,(struct sockaddr *) &c_addr, sizeof(c_addr)) == -1)
	{
		printf("Cannot connect\n");
		close(c_socket);
		return -1;	
	}
	
	n = read(c_socket, rcvBuffer, sizeof(rcvBuffer));
	if(n<0)
	{
		printf("Read Failed\n");
		return -1;
	}
	printf("received data : %s\n", rcvBuffer);

	while(strcmp(sendBuffer,"quit"))
	{
		printf("외톨이 : ");
		fgets(sendBuffer,BUFSIZE,stdin);
		sendBufferLen=strlen(sendBuffer);
		write(c_socket,sendBuffer,sendBufferLen);
		n = read(c_socket,rcvBuffer,sizeof(rcvBuffer));
		rcvBuffer[n]='\0';
		printf("팀팀이 : %s\n",rcvBuffer);
	}
	close(c_socket);
	return 0;
}
