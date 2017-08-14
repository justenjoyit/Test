#include"ThreadPoolLib.h"

void* create_thread(void*args)
{
	cout<<"create thread"<<endl;
}

int main()
{
	int n;
	cin>>n;

	ThreadPool myThreadPool(n);
	myThreadPool.initPool(create_thread,NULL);
	
	TaskList myList;
	cout<<myList.getSize()<<endl;
	myList.addTask(create_thread);
	myList.addTask(create_thread);
	cout<<myList.getSize()<<endl;
	return 0;
}
