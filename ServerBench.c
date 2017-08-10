#include<iostream>
#include<unistd.h>
#include<sys/param.h>
#include<pthread.h>
#include<string.h>
#include<string>
#include<stdio.h>
#include<time.h>
#include<signal.h>
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

int main(int argc,char *argv[])
{
	if(argc!=2){
		cout<<"Error: Missing url!"<<endl;
		return 1;
	}
	
	buildHttpRequest(argv[1]);

	//return bench();
	return 0;
}

