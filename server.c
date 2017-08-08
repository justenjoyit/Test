#include<iostream>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<ctype.h>
#include<strings.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/wait.h>
#include<stdlib.h>
#include<stdint.h>
using namespace std;

int startUp(u_short *);

int startUp(u_short *port)
{
	struct sockaddr_in server;

	int serverfd=socket(AF_INET,SOCK_STREAM,0);

	if(serverfd<0){
		cerr<<"server socket failed!"<<endl;
		return -1;
	}

	memset(&server,0,sizeof(server));

	server.sin_family=AF_INET;
	server.sin_port=htons(*port);
	server.sin_addr.s_addr=htonl(INADDR_ANY);

	int on=1;
	if((setsockopt(serverfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on)))<0){
		cerr<<"setsockopt failed!"<<endl;
		return -1;
	}

	if(bind(serverfd,(struct sockaddr*)&server,sizeof(server))<0){
		cerr<<"bind failed!"<<endl;
		return -1;
	}

	if(listen(serverfd,5)<0){
		cerr<<"listen failed!"<<endl;
		return -1;
	}

	return serverfd;
}

int main()
{
	u_short serverPort=4000;
	struct sockaddr_in client;
	socklen_t client_len=sizeof(client);

	int serverSock=startUp(&serverPort);
	cout<<"http running on port "<<serverPort<<endl;

	int clientSock=-1;
	while(1){
		clientSock=accept(serverSock,(struct sockaddr *)&client,&client_len);
		if(clientSock==-1){
			cerr<<"accept failed!"<<endl;
			return -1;
		}
		char clientAddr[1024];
		inet_ntop(AF_INET,&client.sin_addr,clientAddr,sizeof(clientAddr));
		cout<<"Connection from "<<clientAddr<<" port "<<client.sin_port<<endl;

	}

	close(serverSock);
	return 0;
}

