#include<iostream>
#include<stdio.h>
#include<pthread.h>
#include<sys/types.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<strings.h>
#include<string>
#include<list>
#include<semaphore.h>
#include<vector>
#include<set>

using namespace std;

void* manageThread(void*);
void* manageTask(void*);
void* childFunc(void*);
const int maxThreadNum=200;
class Task
{
	public:
	void* (*myFunc)(void*,void*);
	void* args;
	void* rtns;
	Task(void*(*func)(void*,void*),void*a,void*r)
	{
		myFunc=func;
		args=a;
		rtns=r;
	}
	Task();
	Task(const Task&);
};
class TaskList
{
	private:
	list<Task> myTaskList;

	public:
	pthread_mutex_t _task_mutex;
	TaskList();
	void addTask(void*(*func)(void*,void*),void*,void*);
	void popFront();
	Task getFront();
	int getSize();
};

class Thread
{
	public:
	pthread_t tid;
	pthread_mutex_t _thread_node_mutex;
	pthread_cond_t _thread_node_cond;
	Task* _node_task;
	Thread();
	Thread(const Thread*);
};
class WaitingThread
{
	public:
	list<Thread*> myWaitingThreadList;

	public:
	pthread_mutex_t _waiting_list_mutex;
	void addThread(Thread*);
	Thread* getTop();
	void popTop();
	int getSize();
	WaitingThread();
};

class ActiveThread
{
	private:
	list<Thread*> myActiveThreadList;

	public:
	pthread_mutex_t _active_list_mutex;
	void addThread(Thread*);
	Thread* erase(Thread*);
	ActiveThread();
	int getSize();
};

class ThreadPool
{
	private:
	TaskList _taskList;
	WaitingThread _waitingList;
	ActiveThread _activeList;
	pthread_t thread_manager,task_manager;
	pthread_cond_t thread_manager_cond,task_manager_cond;
	pthread_mutex_t thread_pool_mutex,thread_manager_mutex,task_manager_mutex;
	int size;
	int largestNum,smallestNum;
	int basicSize;
	set<pthread_t> tids;

	public:
	void* child_func(void*);
	void* manage_thread(void*);
	void* manage_task(void*);

	public:
	ThreadPool(int);
	~ThreadPool();
	void initPool();
	void addTask(void*(*func)(void*,void*),void*,void*);
	void initSystem();
	int test;
};
