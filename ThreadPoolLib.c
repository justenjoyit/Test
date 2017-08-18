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

Thread::Thread(const Thread* temp)
{
	cout<<"ready to new"<<endl;
	tid=temp->tid;
	_thread_node_mutex=temp->_thread_node_mutex;
	_thread_node_cond=temp->_thread_node_cond;
	_node_task=temp->_node_task;
	cout<<"new success..."<<endl;
}

void WaitingThread::addThread(Thread* temp)
{
	myWaitingThreadList.push_back(temp);
}

Thread* WaitingThread::getTop()
{
	cout<<"return back...";
	cout<<(myWaitingThreadList.front())->tid<<endl;
	return myWaitingThreadList.front();
}

void WaitingThread::popTop()
{
	cout<<"ready to pop_top..."<<myWaitingThreadList.front()->tid<<endl;
	myWaitingThreadList.pop_front();
	cout<<"pop_top success..."<<endl;
}

void ActiveThread::addThread(Thread* temp)
{
	myActiveThreadList.push_back(temp);
}

Thread* ActiveThread::erase(Thread* temp)
{
	list<Thread*>::iterator i=myActiveThreadList.begin();
	for(;i!=myActiveThreadList.end();++i)
	{
		if(*i==temp)
			break;
	}
	if(i==myActiveThreadList.end())
	{
		cout<<"Error: ActiveThread::erase() "<<endl;
		return NULL;
	}

	Thread* newThread=new Thread(*i);
	myActiveThreadList.erase(i);
	return newThread;
}

int WaitingThread::getSize()
{
	return myWaitingThreadList.size();
}

ActiveThread::ActiveThread()
{
	myActiveThreadList=list<Thread*>(0);
	pthread_mutex_init(&_active_list_mutex,NULL);
}

WaitingThread::WaitingThread()
{
	myWaitingThreadList=list<Thread*>(0);
	pthread_mutex_init(&_waiting_list_mutex,NULL);
}

int ActiveThread::getSize()
{
	return myActiveThreadList.size();
}
void* ThreadPool::child_func(void*args)
{
	//Thread* tempThread=(Thread*)args;
	Thread* tempThread=new Thread();
	tempThread->tid=pthread_self();
	cout<<"hhhhh  "<<tempThread->tid<<endl;
	cout<<"child_func:  "<<pthread_self()<<endl;
	
	pthread_cleanup_push(cleanUpMutex,(void*)&_waitingList._waiting_list_mutex);
	pthread_mutex_lock(&_waitingList._waiting_list_mutex);
	cout<<"add THread p............................"<<endl;
	_waitingList.addThread(tempThread);
	cout<<"add THread suc........................."<<endl;
	pthread_mutex_unlock(&_waitingList._waiting_list_mutex);
	pthread_cleanup_pop(0);

	cout<<"child_func............addThread"<<pthread_self()<<endl;
	while(1)
	{
		//pthread_cleanup_push(cleanUpMutex,(void*)&(tempThread->_thread_node_mutex));
		pthread_mutex_lock(&(tempThread->_thread_node_mutex));
		if(tempThread->_node_task==NULL)
		{
			
			pthread_cond_wait(&(tempThread->_thread_node_cond),&(tempThread->_thread_node_mutex));
		}
		//线程有任务
		//执行任务
		cout<<"................if myFunc....."<<endl;
		tempThread->_node_task->myFunc(tempThread->_node_task->args,tempThread->_node_task->rtns);
		cout<<"-------------=----myFunc succ.."<<endl;
		//查找是否还有任务
		pthread_mutex_lock(&(_taskList._task_mutex));
		if(_taskList.getSize()==0)
		{
			pthread_mutex_unlock(&_taskList._task_mutex);
			//没有任务
			//将线程加入空闲
			delete tempThread->_node_task;
			tempThread->_node_task=NULL;
			
			//从活动线程链表中删除
			//使线程变成游离态
			pthread_mutex_lock(&_activeList._active_list_mutex);
			tempThread=_activeList.erase(tempThread);
			pthread_mutex_unlock(&_activeList._active_list_mutex);
			
			//将线程加入空闲堆栈
			pthread_mutex_lock(&_waitingList._waiting_list_mutex);
			_waitingList.addThread(tempThread);
			pthread_mutex_unlock(&_waitingList._waiting_list_mutex);
		}else{
			//取任务链表头
			tempThread->_node_task=new Task(_taskList.getFront());
			_taskList.popFront();
			pthread_mutex_unlock(&_taskList._task_mutex);
		}

		pthread_mutex_unlock(&(tempThread->_thread_node_mutex));
	}
}

void* ThreadPool::manage_thread(void*args)
{
	cout<<"manage_thread start..."<<endl;
	int flag=0;
	while(1)
	{
		//取任务列表头的任务
		pthread_mutex_lock(&thread_manager_mutex);
		pthread_mutex_lock(&_taskList._task_mutex);
		if(_taskList.getSize()==0)
		{
			pthread_mutex_unlock(&_taskList._task_mutex);
			pthread_cond_wait(&thread_manager_cond,&thread_manager_mutex);
			cout<<"receive from manage_task................."<<endl;
		}else
			pthread_mutex_unlock(&_taskList._task_mutex);

		//pthread_mutex_unlock(&thread_manager_mutex);
		
		//pthread_mutex_lock(&_taskList._task_mutex);
		//Task taskTemp=_taskList.getFront();
		//cout<<"manage_thread popFront()..."<<_taskList.getSize()<<endl;
		//_taskList.popFront();
		//cout<<"manage_thread popFront() success...."<<_taskList.getSize()<<endl;
		//pthread_mutex_unlock(&_taskList._task_mutex);
	
		cout<<"-----------------lock waiting"<<endl;
		//取空闲链表尾的线程
		pthread_mutex_lock(&_waitingList._waiting_list_mutex);
		cout<<"-----------------lock suc"<<endl;
		if(_waitingList.getSize()<=smallestNum)
		{
			cout<<"扩容前...."<<_waitingList.getSize()<<" "<<_activeList.getSize()<<" "<<tids.size()<<endl;
			pthread_mutex_unlock(&_waitingList._waiting_list_mutex);
			//扩容，空闲线程值小于最小空闲线程数
			//扩大为size的两倍
			int count=0;
			for(int i=size;i<maxThreadNum&&i<2*size;++i)
			{
				pthread_t tempTid;
				pthread_create(&tempTid,NULL,childFunc,(void*)this);
				cout<<"扩容id..........."<<tempTid<<endl;
				tids.insert(tempTid);
				count++;
			}
			size+=count;
			largestNum+=count;
			smallestNum+=count;
			cout<<"已扩容...."<<_waitingList.getSize()<<" "<<_activeList.getSize()<<endl;
		}else if(_waitingList.getSize()>largestNum&&_waitingList.getSize()>basicSize&&(flag!=0))
		{
			cout<<"000000000000000   larger"<<endl;
			sleep(1);
			int count=0;
			for(int i=_waitingList.getSize();i>=basicSize&&i>=largestNum;--i)
			{
				Thread* tThread=_waitingList.getTop();
				_waitingList.popTop();
				cout<<"准备销毁线程..."<<tThread->tid<<endl;
				int res;
				if((res=pthread_cancel(tThread->tid))!=0)
				{
					cout<<"销毁失败..."<<tThread->tid<<endl;
				}
				else
				{
					count++;
					tids.erase(tThread->tid);
					delete tThread;
					tThread=NULL;
				}
			}
			size=size-count;
			largestNum-=count;
			smallestNum-=count;
			cout<<"销毁后....."<<_waitingList.getSize()<<" "<<tids.size()<<endl;
			pthread_mutex_unlock(&_waitingList._waiting_list_mutex);
			//缩小，空闲线程数太多
		}else{
			flag=1;
			cout<<"enough thread..............."<<endl;	
			pthread_mutex_lock(&_taskList._task_mutex);
			Task taskTemp=_taskList.getFront();
			cout<<"manage_thread popFront()..."<<_taskList.getSize()<<endl;
			_taskList.popFront();
			cout<<"manage_thread popFront() success...."<<_taskList.getSize()<<endl;
			pthread_mutex_unlock(&_taskList._task_mutex);

			//取线程
			cout<<"manage_thread new Thread..."<<endl;
			Thread* threadTemp;
			//cout<<"1"<<endl;
			cout<<_waitingList.getSize()<<endl;
			cout<<((*(_waitingList.myWaitingThreadList.begin())))->tid<<endl;
			threadTemp=_waitingList.getTop();
			cout<<"manage_thread new success...."<<endl;
			_waitingList.popTop();
			cout<<"manage_thread popTop success..."<<_waitingList.getSize()<<endl;
			pthread_mutex_unlock(&_waitingList._waiting_list_mutex);
		
			//cout<<"manage_thread new Thread..."<<_waitingList.getSize()<<endl;
			threadTemp->_node_task=new Task(taskTemp);
	
			//将线程和任务加入活动队列并唤醒
			pthread_mutex_lock(&_activeList._active_list_mutex);
			_activeList.addThread(threadTemp);
			pthread_mutex_unlock(&_activeList._active_list_mutex);
		
			cout<<"thread to signal..."<<endl;
			pthread_cond_signal(&(threadTemp->_thread_node_cond));
		}
		pthread_mutex_unlock(&thread_manager_mutex);

	}
}

void* ThreadPool::manage_task(void*args)
{
	cout<<"manage_task start..."<<endl;
	while(1)
	{
		pthread_mutex_lock(&task_manager_mutex);
		pthread_mutex_lock(&_taskList._task_mutex);
		if(_taskList.getSize()==0)
		{
			pthread_mutex_unlock(&_taskList._task_mutex);
			
			pthread_cond_wait(&task_manager_cond,&task_manager_mutex);
			cout<<"receive signal from addTask..."<<endl;
			cout<<"task to signal..."<<endl;
			pthread_cond_signal(&thread_manager_cond);
		}else
			pthread_mutex_unlock(&_taskList._task_mutex);
		pthread_mutex_unlock(&task_manager_mutex);
		//cout<<"task to signal..."<<endl;
		//pthread_cond_signal(&thread_manager_cond);
	}
}

ThreadPool::ThreadPool(int num)
{
	pthread_mutex_init(&thread_pool_mutex,NULL);
	pthread_mutex_init(&thread_manager_mutex,NULL);
	pthread_mutex_init(&task_manager_mutex,NULL);
	pthread_cond_init(&thread_manager_cond,NULL);
	pthread_cond_init(&task_manager_cond,NULL);

	cout<<"ThreadPool start..."<<endl;
	pthread_mutex_lock(&thread_pool_mutex);
	size=num+num/10+1;
	largestNum=num;
	smallestNum=size/10+1;
	basicSize=size;
	_taskList=TaskList();
	_waitingList=WaitingThread();
	_activeList=ActiveThread();
	test=1000;
	pthread_mutex_unlock(&thread_pool_mutex);
}

ThreadPool::~ThreadPool()
{
	for(set<pthread_t>::iterator i=tids.begin();i!=tids.end();++i)
		pthread_join(*i,NULL);
	pthread_join(thread_manager,NULL);
	pthread_join(task_manager,NULL);
}

struct Arg
{
	ThreadPool* pThreadPool;
	Thread* pThread;
};

void* childFunc(void* args)
{
	cout<<"create childFunc..."<<endl;
	//cout<<((struct Arg*)args)->pThreadPool->test<<endl;
	//(((struct Arg*)args)->pThreadPool)->child_func(((struct Arg*)args)->pThread);
	((ThreadPool*)args)->child_func(NULL);
}
void ThreadPool::initPool()
{
	cout<<"initPool start..."<<endl;
	pthread_mutex_lock(&thread_pool_mutex);

	//cout<<"lock thread_pool..."<<endl;
	for(int i=0;i<size;++i)
	{
		//pthread_mutex_lock(&_waitingList._waiting_list_mutex);
		//Thread* temp=new Thread();
		//cout<<"lock waiting list..."<<endl;
		//temp=_waitingList.addThread(temp);
		//pthread_mutex_unlock(&_waitingList._waiting_list_mutex);
		//cout<<"unlock waiting list..."<<endl;

		//struct Arg *initArgs;
		//initArgs->pThreadPool=this;
		//initArgs->pThread=temp;
		//cout<<"create child..."<<tids[i]<<endl;
		pthread_t tempTid;
		pthread_create(&tempTid,NULL,childFunc,(void*)this);
		tids.insert(tempTid);
		cout<<"create child..."<<tempTid<<endl;
	}
	pthread_mutex_unlock(&thread_pool_mutex);
	cout<<"initPool end...size "<<_waitingList.getSize()<<endl;
	
//	for(vector<pthread_t>::iterator i=tids.begin();i!=tids.end();++i)
//		pthread_join(*i,NULL);
//	pthread_join(thread_manager,NULL);
//	pthread_join(task_manager,NULL);
}

TaskList::TaskList()
{
	pthread_mutex_init(&_task_mutex,NULL);
}

void TaskList::addTask(void*(*func)(void*,void*),void*a,void*r)
{
	Task temp(func,a,r);
	//pthread_mutex_lock(&_task_mutex);
	myTaskList.push_back(temp);
	cout<<"add Task successfully...size "<<myTaskList.size()<<endl;
	//pthread_mutex_unlock(&_task_mutex);
}

int TaskList::getSize()
{
	return myTaskList.size();
}

void ThreadPool::addTask(void*(*func)(void*,void*),void*args,void*rtns)
{
	cout<<"start............."<<endl;
	pthread_mutex_lock(&_taskList._task_mutex);
	cout<<"if lock..........."<<endl;
	_taskList.addTask(func,args,rtns);
	if(_taskList.getSize()==1){
		cout<<"signal task_manager_cond..."<<endl;
		pthread_cond_signal(&task_manager_cond);
	}
	pthread_mutex_unlock(&_taskList._task_mutex);
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
	//cout<<"manageThread start..."<<endl;
	((ThreadPool*)args)->manage_thread(NULL);
}

void* manageTask(void* args)
{
	cout<<"manageTask start..."<<endl;
	((ThreadPool*)args)->manage_task(NULL);
}

//初始化系统，创建线程管理和任务管理两个线程
//初始化互斥量
void ThreadPool::initSystem()
{
	//pthread_mutex_lock(&thread_pool_mutex);
	cout<<"pthread_create manageThread start..."<<endl;
	pthread_create(&thread_manager,NULL,manageThread,(void*)this);
	cout<<"pthread_create manageTask start..."<<endl;
	pthread_create(&task_manager,NULL,manageTask,(void*)this);
	//pthread_mutex_unlock(&thread_pool_mutex);
	//pthread_mutex_init(&_taskList._task_mutex,NULL);
	
	//pthread_mutex_init(&_waitingList._waiting_list_mutex,NULL);
	
	initPool();
}
