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
	pthread_mutex_init(&_task_mutex,NULL);
}

void TaskList::addTask(void*(*func)(void*,void*),void*a,void*r)
{
	Task temp(func,a,r);
	pthread_mutex_lock(&_task_mutex);
	myTaskList.push_back(temp);
	pthread_mutex_unlock(&_task_mutex);

	cout<<"TaskList::myTask ";
	(*(myTaskList.back().myTask))(a,r);
}

int TaskList::getSize()
{
	return myTaskList.size();
}

void ThreadPool::addTask(void*(*func)(void*,void*),void*args,void*rtns)
{
	_taskList.addTask(func,args,rtns);
}

void TaskList::popFront()
{
	pthread_mutex_lock(&_task_mutex);
	myTaskList.pop_front();
	pthread_mutex_unlock(&_task_mutex);
}

Task TaskList::getFront()
{
	return myTaskList.front();
}
