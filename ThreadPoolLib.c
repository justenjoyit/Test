#include"ThreadPoolLib.h"

void* manage_thread(void*)
{
	//取任务列表头的任务
	pthread_mutex_lock(&_taskList._task_mutex);
	if(_taskList.getSize()==0)
		pthread_cond_wait(&thread_manager_cond,&_taskList._task_mutex);
	Task taskTemp=_taskList.getFront();
	_taskList.popFront();
	pthread_mutex_unlock(&_taskList._task_mutex);

	//取空闲链表尾的线程
	pthread_mutex_lock(&_waitingList._waiting_list_mutex);
	if(_waitingList.getSize()<=smallestNum)
	{
		//扩容，空闲线程值小于最小空闲线程数
	}else if(_waitingList.getSize()>largestNum)
	{
		//缩小，空闲线程数太多
	}

	//取线程
	Thread threadTemp=_waitingList.getTop();
	_waitingList.popTop();
	pthread_mutex_unlock(&_waitingList._waiting_list_mutex);

	threadTemp.myFunc=taskTemp;

	//将线程和任务加入活动队列并唤醒
	pthread_mutex_lock(&_activeList._active_list_mutex);
	_activeList.addThread(threadTemp);
	pthread_mutex_unlock(&_activeList._active_list_mutex);

	pthread_cond_signal(threadTemp._thread_node_cond);
}


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

void ThreadPool::initPool()
{
	for(int i=0;i<size;++i)
	{
		Thread temp;

		pthread_mutex_lock(&_waitingList._waiting_list_mutex);
		list<Thread>::iterator iter=_waitingList.addThread(temp);
		pthread_mutex_unlock(&_waitingList._waiting_list_mutex);

		pthread_create(&tids[i],NULL,child_func,(void*)(iter));
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

//初始化系统，创建线程管理和任务管理两个线程
//初始化互斥量
void ThreadPool::initSystem()
{
	pthread_create(&thread_manager,NULL,manage_thread,NULL);
	pthread_create(&task_manager,NULL,manage_task,NULL);
	
	pthread_mutex_init(&_taskList._task_mutex,NULL);
	
	pthread_mutex_init(&_waitingList._waiting_list_mutex,NULL);
	
	pthread_mutex_init(&_activeList._active_list_mutex,NULL);

	initPool();
}
