#include"ThreadPoolLib.h"

ThreadPool::ThreadPool(int num)
{
	tids=new pthread_t[num];
	size=num;
}

ThreadPool::~ThreadPool()
{
	for(int i=0;i<size;++i)
	{
		pthread_join(tids[i],NULL);
	}
	cout<<"end"<<endl;
}

void ThreadPool::initPool(void* (*func) (void*),void*args)
{
	for(int i=0;i<size;++i)
	{
		pthread_create(&tids[i],NULL,func,args);
	}
}

TaskList::TaskList()
{
}

void TaskList::addTask(void*(*func)(void*))
{
	Task temp(func);
	myTaskList.push_back(temp);
}

int TaskList::getSize()
{
	return myTaskList.size();
}
