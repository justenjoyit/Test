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

using namespace std;

class Task
{
	public:
	void* (*myTask)(void*);
	Task(void*(*func)(void*))
	{
		myTask=func;
	}
};
class TaskList
{
	private:
	list<Task> myTaskList;
	
	public:
	TaskList();
	void addTask(void*(*func)(void*));
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
	ThreadPool(int);
	~ThreadPool();
	void initPool(void*(*func)(void*),void*args);
};
