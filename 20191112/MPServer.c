#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>

#define PORT 11000
#define BUFSIZE 100

char buffer[100] ={};

int main()
{
	int c_socket, s_socket;
	struct sockaddr_in s_addr,c_addr;
	int len;
	int n;
	int pid;

	char sendBuffer[BUFSIZE]={};
	int sendBufferLen;
	char rcvBuffer[BUFSIZE]={};
	int rcvBufferLen;
	FILE* fp;
	
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
		if((pid=fork())>0)
		{	
			close(c_socket);
			continue;
		}
		else if(pid==0)
		{
			printf("클라이언트 접속 허용\n");

			n=strlen(buffer);
			//write(c_socket, buffer, n);
			while(strcmp(rcvBuffer,"quit"))
			{		
				n=read(c_socket,rcvBuffer,sizeof(rcvBuffer));
				rcvBuffer[n-1]='\0';
				memset(sendBuffer,0,BUFSIZE);

				if(!(strncmp(rcvBuffer,"strlen",6)))
				{
					sprintf(sendBuffer,"%d",strlen(rcvBuffer)-7);
					sendBufferLen=strlen(sendBuffer);
					write(c_socket,sendBuffer,sendBufferLen);
				}
				else if(!(strncmp(rcvBuffer,"strcmp",6)))
				{
					ptr=strtok(rcvBuffer," ");
					ptr=strtok(NULL," ");
					strcpy(tok1,ptr);
					ptr=strtok(NULL," ");
					strcpy(tok2,ptr);

					sprintf(sendBuffer,"%d",strcmp(tok1,tok2));
					sendBufferLen=strlen(sendBuffer);
					write(c_socket,sendBuffer,sendBufferLen);
				}
				else if(!(strncmp(rcvBuffer,"readfile",8)))
				{
					fp=fopen(rcvBuffer + 9, "r");
					if(fp)
					{
						char tmpBuffer[BUFSIZE]={};
						while(fgets(tmpBuffer,sizeof(tmpBuffer),fp))
						{
							strcat(sendBuffer,tmpBuffer);
						}
						sendBufferLen=strlen(sendBuffer);
						write(c_socket, sendBuffer, sendBufferLen);
					}
					else
					{
							strcpy(sendBuffer,"그런 파일이 존재하지 않아요!");
							sendBufferLen=strlen(sendBuffer);
							write(c_socket, sendBuffer, sendBufferLen);
					}
					fclose(fp);
				}
				else if(!(strncmp(rcvBuffer,"exec",4)))
				{			
					char tmpBuffer[BUFSIZE]={};
					sprintf(buffer,"%s > buffer.txt",rcvBuffer+5);
					printf("%s\n",buffer);
					system(buffer);
					fp=fopen("buffer.txt","r");
					while(fgets(tmpBuffer,sizeof(tmpBuffer),fp))
					{
						strcat(sendBuffer,tmpBuffer);
					}
					sendBufferLen=strlen(sendBuffer);
					write(c_socket, sendBuffer, sendBufferLen);
					fclose(fp);

				}
				else if(!(strcmp(rcvBuffer,"안녕하세요")))
				{
					strcpy(sendBuffer,"네 안녕하세요. 만나서 반가워요");
					sendBufferLen=strlen(sendBuffer);
					write(c_socket,sendBuffer,sendBufferLen);
				}
				else if(!(strcmp(rcvBuffer,"이름이 머야?")))
				{
					strcpy(sendBuffer,"이름은 팀팀이에요~~");
					sendBufferLen=strlen(sendBuffer);
					write(c_socket,sendBuffer,sendBufferLen);
				}
				else if(!(strcmp(rcvBuffer,"몇 살이야?")))
				{
					strcpy(sendBuffer,"제 나이는 한살이에요!! 태어난지 얼마 안됐어요ㅠㅠ");
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
	}
	
	close(c_socket);
}
