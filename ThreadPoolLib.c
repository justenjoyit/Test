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

void* child_func(void* args)
{
	//while(1)
	//{
		//wait signal
	//}
}
void ThreadPool::initPool()
{
	for(int i=0;i<size;++i)
	{
		pthread_create(&tids[i],NULL,child_func,NULL);
	}
}

TaskList::TaskList()
{
}

void TaskList::addTask(void*(*func)(void*,void*),void*a,void*r)
{
	Task temp(func,a,r);
	myTaskList.push_back(temp);
	cout<<"TaskList::myTask ";
	(*(myTaskList.back().myTask))(a,r);
}

int TaskList::getSize()
{
	return myTaskList.size();
}
