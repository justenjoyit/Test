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

using namespace std;

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
	Task _node_task;
};
class WaitingThread
{
	private:
	list<Thread> myWaitingThreadList;

	public:
	pthread_mutex_t _waiting_list_mutex;
	list<Thread>::iterator addThread(Thread);
	Thread getTop();
	void popTop();
};

class ActiveThread
{
	private:
	list<Thread> myActiveThreadList;

	public:
	pthread_mutex_t _active_list_mutex;
	list<Thread>::iterator addThread(Thread);
	void erase(list<Thread>::iterator);
};

class ThreadPool
{
	private:
	TaskList _taskList;
	WaitingThread _waitingList;
	ActiveThread _activeList;
	pthread_t thread_manager,task_manager;
	pthread_cond_t thread_manager_cond;
	int size;
	int largestNum,smallestNum;

	public:
	friend void* child_func(void*);
	friend void* manage_thread(void*);
	friend void* manage_task(void*);

	public:
	ThreadPool(int);
	~ThreadPool();
	void initPool();
	void addTask(void*(*func)(void*,void*),void*,void*);
	void initSystem();
};
