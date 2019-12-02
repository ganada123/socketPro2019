#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>

#define PORT 11000
#define BUFSIZE 100

char buffer[100] ={};

char sendBuffer[BUFSIZE]={};
int sendBufferLen;
char rcvBuffer[BUFSIZE]={};
int rcvBufferLen;

int main()
{
	int c_socket, s_socket;
	struct sockaddr_in s_addr,c_addr;
	int len;
	int n;

	char* ptr;
	char tok1[BUFSIZE/2];
	char tok2[BUFSIZE/2];

	s_socket = socket(PF_INET, SOCK_STREAM,0);
	
	memset(&s_addr, 0, sizeof(s_addr));
	s_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	s_addr.sin_family= AF_INET;
	s_addr.sin_port = htons(PORT);

	if(bind(s_socket,(struct sockaddr *) &s_addr, sizeof(s_addr)) == -1)
	{
		printf("Cannot Bind\n");
		return -1;
	}

	if(listen(s_socket,5) == -1 )
	{
		printf("listen Fail\n");
		return -1;
	}

	while(1)
	{
		len=sizeof(c_addr);
		printf("클라이언트 접속을 기다리는 중....\n");
		c_socket = accept(s_socket, (struct sockaddr *) &c_addr, &len);
		printf("클라이언트 접속 허용\n");

		n=strlen(buffer);
		//write(c_socket, buffer, n);
		while(strcmp(rcvBuffer,"quit"))
		{		
			n=read(c_socket,rcvBuffer,sizeof(rcvBuffer));
			rcvBuffer[n-1]='\0';
			memset(sendBuffer,0,BUFSIZE);

			if(!(strncmp(rcvBuffer,"/w",2)))
			{
				sprintf(sendBuffer,"%d",strlen(rcvBuffer)-7);
				sendBufferLen=strlen(sendBuffer);
				write(c_socket,sendBuffer,sendBufferLen);
			}
			else
			{
				strcpy(sendBuffer,"무슨 말씀을 하시는지 잘 모르겠어요,,,");
				sendBufferLen=strlen(sendBuffer);
				write(c_socket,sendBuffer,sendBufferLen);
			}
		}
	}
	
	close(c_socket);
}
