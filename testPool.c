#include"ThreadPoolLib.h"

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

	ThreadPool myThreadPool(n);
	//pthread_t tid1;
	
	myThreadPool.initSystem();
	
	void*r=NULL;
	int *a=new int[100];
	
	sleep(3);
	cout<<"start adding task..."<<endl;
	for(int i=0;i<10;++i){
		a[i]=i;
		myThreadPool.addTask(create_thread,(void*)&a[i],r);
	}
	
	sleep(3);
	cout<<endl;
	cout<<"start adding task...."<<endl;
	for(int i=11;i<20;++i)
	{
		a[i]=i;
		myThreadPool.addTask(doTask,(void*)&a[i],r);
	}

	sleep(3);
	cout<<endl;
	cout<<"start adding task..."<<endl;
	for(int i=21;i<30;++i)
	{
		a[i]=i;
		myThreadPool.addTask(create_thread,(void*)&a[i],r);
	}

	//pthread_join(tid1,NULL);
	

	return 0;
}
