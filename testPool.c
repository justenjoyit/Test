#include"ThreadPoolLib.h"

void* create_thread(void*args,void* r)
{
	cout<<"create thread"<<((int*)args)[0]<<endl;
}

int main()
{
	int n;
	cin>>n;

	ThreadPool myThreadPool(n);
	myThreadPool.initPool();
	
	TaskList myList;
	cout<<myList.getSize()<<endl;
	void*r=NULL;
	int a1=1,a2=2,a3=3;
	myList.addTask(create_thread,(void*)&a1,r);
	myList.addTask(create_thread,(void*)&a2,r);
	cout<<myList.getSize()<<endl;

	void* (*myTask)(void*,void*);
	myTask=create_thread;

	
	(*myTask)((void*)&a3,r);
	return 0;
}
