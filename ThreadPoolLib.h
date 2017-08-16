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
	void* (*myTask)(void*,void*);
	void* args;
	void* rtns;
	Task(void*(*func)(void*,void*),void*a,void*r)
	{
		myTask=func;
		args=a;
		rtns=r;
	}
};
class TaskList
{
	private:
	list<Task> myTaskList;
	pthread_mutex_t _task_mutex;

	public:
	TaskList();
	void addTask(void*(*func)(void*,void*),void*,void*);
	void popFront();
	Task getFront();
	int getSize();
};
class ThreadPool
{
	private:
	TaskList _taskList;
	//WaitingThread _waitingThread;
	//ActiveThread _activeThread;
	pthread_t *tids;
	int size;

	public:
	friend void* child_func(void*);

	public:
	ThreadPool(int);
	~ThreadPool();
	void initPool();
	void addTask(void*(*func)(void*,void*),void*,void*);
};
