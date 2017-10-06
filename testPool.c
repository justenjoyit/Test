#include"ThreadPoolLib.h"
using namespace std;

void* create_thread(void*args,void* r)
{
	cout<<"-------My Task---------create thread "<<((int*)args)[0]<<endl;
}

void* doTask(void*args,void*rtns)
{
	cout<<"-------Trying to do task---------"<<*((int*)args)<<endl;
}

int main()
{
	int n;
	cin>>n;

	ThreadPool *myThreadPool=new ThreadPool(n);
	
	myThreadPool->initSystem();
	
	void*r=NULL;
	int a=0;
	while(1){
		int temp=a;
		if(temp==100000)
			break;
		myThreadPool->addTask(create_thread,(void*)(&temp),r);
		a++;
	}
	delete myThreadPool;

	return 0;
}
