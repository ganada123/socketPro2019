#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h>
void *do_chat(void *); //채팅 메세지를 보내는 함수
int pushClient(int); //새로운 클라이언트가 접속했을 때 클라이언트 정보 추가
int popClient(int); //클라이언트가 종료했을 때 클라이언트 정보 삭제
pthread_t thread;
pthread_mutex_t mutex;
#define MAX_CLIENT 10
#define MAX_CHANNEL 3
#define CHATDATA 1024
#define INVALID_SOCK -1
#define PORT 9000
int    list_c[MAX_CHANNEL][MAX_CLIENT];
char    escape[ ] = "exit\n";
char	whisper[ ] = "/w";
char    greeting[ ] = "Welcome to chatting room\n";
char    CODE200[ ] = "Sorry No More Connection\n";
char	usernick[MAX_CHANNEL][MAX_CLIENT][20];
char	rcvBuffer[100]="";
int main(int argc, char *argv[ ])
{
    int c_socket, s_socket;
    struct sockaddr_in s_addr, c_addr;
    int    len;
    int    i, j, n;
	int channel;
    int    res;
    if(pthread_mutex_init(&mutex, NULL) != 0) {
        printf("Can not create mutex\n");
        return -1;
    }
    s_socket = socket(PF_INET, SOCK_STREAM, 0);
    memset(&s_addr, 0, sizeof(s_addr));
    s_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(PORT);
    if(bind(s_socket, (struct sockaddr *)&s_addr, sizeof(s_addr)) == -1) {
        printf("Can not Bind\n");
        return -1;
    }
    if(listen(s_socket, MAX_CLIENT) == -1) {
        printf("listen Fail\n");
        return -1;
    }
	for(j=0;j<MAX_CHANNEL;j++)
	{
		for(i = 0; i < MAX_CLIENT; i++)
		{
			list_c[j][i] = INVALID_SOCK;
		}
	}
    while(1) 
	{
		len = sizeof(c_addr);
		c_socket = accept(s_socket, (struct sockaddr *) &c_addr, &len);
		read(c_socket,rcvBuffer,sizeof(rcvBuffer));
		read(c_socket,channel,sizeof(channel));
		for(i=0;i<MAX_CLIENT;i++)
		{
			if(list_c[channel][i]==INVALID_SOCK)
			{
				strcpy(usernick[channel][i],rcvBuffer);
				break;
			}
		}
		res = pushClient(c_socket,channel);
		if(res < 0) 
		{ //MAX_CLIENT만큼 이미 클라이언트가 접속해 있다면,
			write(c_socket, CODE200, strlen(CODE200));
			close(c_socket);
		} 
		else 
		{
				write(c_socket, greeting, strlen(greeting));
		}
	}
}

void *do_chat(void *arg, void *num)
{
	int c_socket = *((int *)arg);
	int channel=*((int *)num);

	char chatData[CHATDATA];
	char sndBuf[CHATDATA];
	int i, n;
	char *ptrsnd;
	char *ptrrcv;
	char tok1[20], tok2[20];
	int exist=0;
    while(1) 
	{
		memset(chatData, 0, sizeof(chatData));
		if((n = read(c_socket, chatData, sizeof(chatData))) > 0) 
		{
			if(!(strcmp(chatData, escape)))
			{
				popClient(c_socket,channel);
				break;
			}
			else if(strstr(chatData, whisper)!=NULL)
			{	//chatData == "nickname /w receiver lalalalala"
				//chatData ==  ptrsnd  2tok  ptrrcv  4tok
				exist=0;
				ptrsnd=strtok(chatData," ");
				strtok(NULL," ");
				ptrrcv=strtok(NULL," ");
				pritnf("<ch%d>",channel);
				sprintf(sndBuf,"[w][%s] %s",ptrsnd,strtok(NULL,""));
				//&ptrrcv[strlen(ptrrcv)+1]
//				printf("%s\n",chatData);
				for(i=0;i<MAX_CLIENT;i++)
				{
					if(!(strcmp(usernick[channel][i],ptrrcv)))
					{
						write(list_c[channel][i],sndBuf,strlen(sndBuf));
						exist=1;
						break;
					}
				}
				if(exist==0)
				{
					strcpy(chatData,"Not exist\n");
					write(c_socket,chatData,strlen(chatData));
				}
			}
			else
			{
				for(i=0;i<MAX_CLIENT;i++)
				{
					if(list_c[channel][i]!=INVALID_SOCK)
					{
						sprintf(sndBuf,"<ch%d>%s",channel,chatData);
						write(list_c[i],sndBuf,strlen(sndBuf));
					}
				}
			}
		}
	}
}
int pushClient(int c_socket,int channel) {
	int i;
    //ADD c_socket to list_c array.
	for(i=0;i<MAX_CLIENT;i++)
	{
		printf("%d : %d,%s\n",i,list_c[channel][i],usernick[channel][i]);
	}
	for(i=0; i<MAX_CLIENT;i++)
	{	
		if(list_c[channel][i]==INVALID_SOCK)
		{
			list_c[channel][i]=c_socket;
			pthread_create(&thread,NULL,do_chat,(void*)(&c_socket));
			return i;
		}
	}
    //return -1, if list_c is full.
    //return the index of list_c which c_socket is added.
	return -1;
}
int popClient(int c_socket,int channel)
{
	int i;
	for(i=0;i<MAX_CLIENT;i++)
	{
		if(list_c[channel][i]==c_socket)
		{
			list_c[channel][i]=INVALID_SOCK;
			memset(&usernick[channel][i],0,sizeof(usernick[channel][i]));
		}
	}
    close(c_socket);
    //REMOVE c_socket from list_c array.
}
