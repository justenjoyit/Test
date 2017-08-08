#include<iostream>
#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
using namespace std;

int main()
{
	struct sockaddr_in client;

	int clientfd=socket(AF_INET,SOCK_STREAM,0);

	client.sin_family=AF_INET;
	client.sin_addr.s_addr=inet_addr("127.0.0.1");
	client.sin_port=htons(4000);

	int result=connect(clientfd,(struct sockaddr *)&client,sizeof(client));

	if(result==-1){
		cerr<<"client failed"<<endl;
		return 1;
	}

	close(clientfd);
	return 0;
}
