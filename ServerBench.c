#include<iostream>
#include<unistd.h>
#include<sys/param.h>
#include<sys/socket.h>
#include<pthread.h>
#include<string.h>
#include<string>
#include<stdio.h>
#include<time.h>
#include<signal.h>
#include<stdlib.h>
#include<stdarg.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<ctype.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/wait.h>
#include<stdint.h>
#include <fcntl.h>
#include <netdb.h>
#include<pthread.h>
using namespace std;

volatile int expired=0;
char request[2048];
char host[1024];
int benchTime=30;
int client=30;
int speed=0;
int failed=0;
int bytes=0;

void buildHttpRequest(const char*);
int bench(void);
void *threadFunc(void*);
int mySocket(const char*,int);

void buildHttpRequest(const char* url)
{
	strcpy(request,"GET");
	strcat(request," ");

	if(strstr(url,"://")==NULL){
		cerr<<url<<" is not a valid URL."<<endl;
		return;
	}

	if(strlen(url)>1500){
		cerr<<"URL is too long."<<endl;
		return;
	}
	
	if(strncasecmp("http://",url,7)!=0){
		cerr<<"Only HTTP protocol is supported."<<endl;
		return;
	}

	int hostStart=strstr(url,"://")-url+3;
	if(strchr(url+hostStart,'/')==NULL){
		cerr<<"URL must end with '/'"<<endl;
		return;
	}
	
	strncpy(host,url+hostStart,strcspn(url+hostStart,"/"));
	strcat(request+strlen(request),url+hostStart+strcspn(url+hostStart,"/"));

	strcat(request," HTTP/1.0");
	strcat(request,"\r\n");

	strcat(request,"User-Agent: ServerBench\r\n");
	strcat(request,"Host: ");
	strcat(request,host);
	strcat(request,"\r\n");
	strcat(request,"\r\n");

	cout<<request<<endl;
}

int mySocket(const char* host,int port)
{
	struct sockaddr_in client;
	client.sin_family=AF_INET;
	client.sin_port=htons(port);
	client.sin_addr.s_addr=inet_addr("127.0.0.1");
	struct hostent *hp;
	hp = gethostbyname(host);
        if (hp == NULL){
	    cout<<"host false"<<endl;
            return -1;
	}
	cout<<host<<endl;
        //memcpy(&client.sin_addr, hp->h_addr, hp->h_length);

	int sockfd=socket(AF_INET,SOCK_STREAM,0);
	if(sockfd<0){
		cout<<"socket failed"<<endl;
		return sockfd;
	}
	cout<<inet_ntoa(client.sin_addr)<<endl;
	cout<<sockfd<<endl;
	if(connect(sockfd, (struct sockaddr *)&client, sizeof(client)) < 0){
		cerr<<"connect failed"<<endl;
		return -1;
	}

	return sockfd;
}
int bench(void)
{
	
	int sockfd=mySocket(host,4000);
	if(sockfd<0){
cout<<sockfd<<endl;
		cerr<<"Error: Socket"<<endl;
		return -1;
	}
	
	pthread_t tid[30];
	for(int i=0;i<client;++i){
		pthread_create(&tid[i],NULL,threadFunc,NULL);
	}
	
	for(int i=0;i<client;++i){
		pthread_join(tid[i],NULL);
	}

	return 1;
}
	
void* threadFunc(void*)
{
	cout<<"thread created"<<endl;
		
}
int main(int argc,char *argv[])
{
	if(argc!=2){
		cout<<"Error: Missing url!"<<endl;
		return 1;
	}
	
	buildHttpRequest(argv[1]);

	return bench();
	//return 0;
}

