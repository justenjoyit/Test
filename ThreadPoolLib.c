#include"ThreadPoolLib.h"

void cleanUpMutex(void* arg)
{
	pthread_mutex_unlock((pthread_mutex_t*)arg);
}

Task::Task()
{
	myFunc=NULL;
	args=NULL;
	rtns=NULL;
}

Task::Task(const Task& temp)
{
	myFunc=temp.myFunc;
	args=temp.args;
	rtns=temp.rtns;
}

Thread::Thread()
{
	tid=0;
	pthread_mutex_init(&_thread_node_mutex,NULL);
	pthread_cond_init(&_thread_node_cond,NULL);
	_node_task=NULL;
}

Thread::~Thread()
{
	pthread_mutex_destroy(&_thread_node_mutex);
	pthread_cond_destroy(&_thread_node_cond);
	delete _node_task;
	_node_task=NULL;
}

Thread::Thread(const Thread* temp)
{
	tid=temp->tid;
	_thread_node_mutex=temp->_thread_node_mutex;
	_thread_node_cond=temp->_thread_node_cond;
	_node_task=temp->_node_task;
}

void WaitingThread::addThread(Thread* temp)
{
	myWaitingThreadList.push_back(temp);
}

Thread* WaitingThread::getTop()
{
	return myWaitingThreadList.back();
}

void WaitingThread::popTop()
{
	myWaitingThreadList.pop_back();
}

void ActiveThread::addThread(Thread* temp)
{
	myActiveThreadList.push_back(temp);
}

void ActiveThread::erase(Thread* temp)
{
	std::list<Thread*>::iterator i=myActiveThreadList.begin();
	for(;i!=myActiveThreadList.end();++i)
	{
		if(*i==temp)
			break;
	}
	if(i==myActiveThreadList.end())
	{
		std::cout<<"Error: ActiveThread::erase() "<<std::endl;
		//return NULL;
	}

	//Thread* newThread=new Thread(*i);
	myActiveThreadList.erase(i);
	//return temp;
}

int WaitingThread::getSize()
{
	return myWaitingThreadList.size();
}

ActiveThread::ActiveThread()
{
	myActiveThreadList=std::list<Thread*>(0);
}

WaitingThread::WaitingThread()
{
	myWaitingThreadList=std::list<Thread*>(0);
}

int ActiveThread::getSize()
{
	return myActiveThreadList.size();
}
void* ThreadPool::child_func(void*args)
{
	Thread* tempThread=new Thread();
	tempThread->tid=pthread_self();
		
	//pthread_cleanup_push(cleanUpMutex,(void*)&_waiting_list_mutex);
	pthread_mutex_lock(&_waiting_list_mutex);
	
	_waitingList.addThread(tempThread);
	
	pthread_mutex_unlock(&_waiting_list_mutex);
	//pthread_cleanup_pop(0);

	
	while(1)
	{

		pthread_mutex_lock(&(tempThread->_thread_node_mutex));
		if(tempThread->_node_task==NULL)
		{
			
			pthread_cond_wait(&(tempThread->_thread_node_cond),&(tempThread->_thread_node_mutex));
		}
		//线程有任务
		//执行任务
		
		tempThread->_node_task->myFunc(tempThread->_node_task->args,tempThread->_node_task->rtns);
		
		//查找是否还有任务
		pthread_mutex_lock(&(_task_mutex));
		if(_taskList.getSize()==0)
		{
			pthread_mutex_unlock(&_task_mutex);
			//没有任务
			//将线程加入空闲
			delete tempThread->_node_task;
			tempThread->_node_task=NULL;
			
			//从活动线程链表中删除
			//使线程变成游离态
			pthread_mutex_lock(&_active_list_mutex);
			_activeList.erase(tempThread);
			pthread_mutex_unlock(&_active_list_mutex);
			
			//将线程加入空闲堆栈
			pthread_mutex_lock(&_waiting_list_mutex);
			_waitingList.addThread(tempThread);
			pthread_mutex_unlock(&_waiting_list_mutex);
		}else{
			//取任务链表头
			tempThread->_node_task=new Task(_taskList.getFront());
			_taskList.popFront();
			pthread_mutex_unlock(&_task_mutex);
		}

		pthread_mutex_unlock(&(tempThread->_thread_node_mutex));
	}
	//delete tempThread;
	//tempThread=NULL;
}

void* ThreadPool::manage_thread(void*args)
{
	int flag=0;
	while(1)
	{
		//取任务列表头的任务
		pthread_mutex_lock(&thread_manager_mutex);
		pthread_mutex_lock(&_task_mutex);
		if(_taskList.getSize()==0)
		{
			pthread_mutex_unlock(&_task_mutex);
			pthread_cond_wait(&thread_manager_cond,&thread_manager_mutex);
		}else
			pthread_mutex_unlock(&_task_mutex);
		
		//取空闲链表尾的线程
		pthread_mutex_lock(&_waiting_list_mutex);
		
		if(_waitingList.getSize()<=smallestNum)
		{
			std::cout<<"扩容前...."<<tids.size()<<std::endl;
			pthread_mutex_unlock(&thread_manager_mutex);
			pthread_mutex_unlock(&_waiting_list_mutex);
			//扩容，空闲线程值小于最小空闲线程数
			//扩大为size的两倍
			int count=0;
			for(int i=size;i<maxThreadNum&&i<2*size;++i)
			{
				pthread_t tempTid;
				pthread_create(&tempTid,NULL,childFunc,(void*)this);
				tids.insert(tempTid);
				count++;
			}
			size+=count;
			largestNum+=count;
			smallestNum+=count;
			std::cout<<"扩容后...."<<tids.size()<<std::endl;

		}else if(_waitingList.getSize()>largestNum&&_waitingList.getSize()>basicSize&&(flag!=0))
		{
			std::cout<<"缩容前...."<<tids.size()<<std::endl;
			int count=0;
			for(int i=_waitingList.getSize();i>=basicSize&&i>=largestNum;--i)
			{
				Thread* tThread=_waitingList.getTop();
				_waitingList.popTop();
				
				int res;
				if((res=pthread_cancel(tThread->tid))!=0)
				{
					std::cout<<"销毁失败..."<<tThread->tid<<std::endl;
				}
				else
				{
					count++;
					tids.erase(tThread->tid);
					//pthread_mutex_destroy(&tThread->_thread_node_mutex);
					//pthread_cond_destroy(&tThread->_thread_node_cond);
					//delete tThread;
					//tThread=NULL;
				}
				
			}
			size=size-count;
			largestNum-=count;
			smallestNum-=count;
			std::cout<<"缩容后....."<<tids.size()<<std::endl;
			pthread_mutex_unlock(&_waiting_list_mutex);
			pthread_mutex_unlock(&thread_manager_mutex);
			//缩小，空闲线程数太多
		}else{
			pthread_mutex_unlock(&thread_manager_mutex);
			flag=1;
				
			pthread_mutex_lock(&_task_mutex);
			
			Task taskTemp=_taskList.getFront();
			_taskList.popFront();
			
			pthread_mutex_unlock(&_task_mutex);

			//取线程
			
			Thread* threadTemp;
			
			threadTemp=_waitingList.getTop();
			_waitingList.popTop();
			
			pthread_mutex_unlock(&_waiting_list_mutex);
			
			if(threadTemp->_node_task!=NULL)
			{
				delete threadTemp->_node_task;
				threadTemp->_node_task=NULL;
			}

			threadTemp->_node_task=new Task(taskTemp);
	
			//将线程和任务加入活动队列并唤醒
			pthread_mutex_lock(&_active_list_mutex);
			_activeList.addThread(threadTemp);
			pthread_mutex_unlock(&_active_list_mutex);
		
			pthread_cond_signal(&(threadTemp->_thread_node_cond));
		}
		//pthread_mutex_unlock(&thread_manager_mutex);

	}
}

void* ThreadPool::manage_task(void*args)
{
	while(1)
	{
		pthread_mutex_lock(&task_manager_mutex);
		pthread_mutex_lock(&_task_mutex);
		if(_taskList.getSize()==0)
		{
			pthread_mutex_unlock(&_task_mutex);
			
			pthread_cond_wait(&task_manager_cond,&task_manager_mutex);
			pthread_cond_signal(&thread_manager_cond);
		}else
			pthread_mutex_unlock(&_task_mutex);
		pthread_mutex_unlock(&task_manager_mutex);	
	}
}

ThreadPool::ThreadPool(int num)
{
	pthread_mutex_init(&thread_pool_mutex,NULL);
	pthread_mutex_init(&thread_manager_mutex,NULL);
	pthread_mutex_init(&task_manager_mutex,NULL);
	pthread_mutex_init(&_active_list_mutex,NULL);
	pthread_mutex_init(&_waiting_list_mutex,NULL);
	pthread_mutex_init(&_task_mutex,NULL);

	pthread_cond_init(&thread_manager_cond,NULL);
	pthread_cond_init(&task_manager_cond,NULL);

	pthread_mutex_lock(&thread_pool_mutex);
	size=num+num/10+1;
	largestNum=num;
	smallestNum=size/10+1;
	basicSize=size;
	_taskList=TaskList();
	_waitingList=WaitingThread();
	_activeList=ActiveThread();
	pthread_mutex_unlock(&thread_pool_mutex);
}

ThreadPool::~ThreadPool()
{
	for(std::set<pthread_t>::iterator i=tids.begin();i!=tids.end();++i)
		pthread_cancel(*i);
	pthread_cancel(thread_manager);
	pthread_cancel(task_manager);
}

void* childFunc(void* args)
{
	((ThreadPool*)args)->child_func(NULL);
}

void ThreadPool::initPool()
{
	pthread_mutex_lock(&thread_pool_mutex);

	for(int i=0;i<size;++i)
	{
		pthread_t tempTid;
		pthread_create(&tempTid,NULL,childFunc,(void*)this);
		tids.insert(tempTid);
	}
	pthread_mutex_unlock(&thread_pool_mutex);
}

TaskList::TaskList()
{
}

void TaskList::addTask(void*(*func)(void*,void*),void*a,void*r)
{
	Task temp(func,a,r);
	myTaskList.push_back(temp);
}

int TaskList::getSize()
{
	return myTaskList.size();
}

void ThreadPool::addTask(void*(*func)(void*,void*),void*args,void*rtns)
{
	pthread_mutex_lock(&_task_mutex);
	
	_taskList.addTask(func,args,rtns);
	if(_taskList.getSize()==1){
		pthread_cond_signal(&task_manager_cond);
	}
	pthread_mutex_unlock(&_task_mutex);
}

void TaskList::popFront()
{
	myTaskList.pop_front();
}

Task TaskList::getFront()
{
	return myTaskList.front();
}

void* manageThread(void* args)
{
	((ThreadPool*)args)->manage_thread(NULL);
}

void* manageTask(void* args)
{
	((ThreadPool*)args)->manage_task(NULL);
}

//初始化系统，创建线程管理和任务管理两个线程
//初始化互斥量
void ThreadPool::initSystem()
{
	pthread_create(&thread_manager,NULL,manageThread,(void*)this);
	pthread_create(&task_manager,NULL,manageTask,(void*)this);
		
	initPool();
}
