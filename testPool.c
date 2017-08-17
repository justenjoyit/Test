#include"ThreadPoolLib.h"

void* create_thread(void*args,void* r)
{
	cout<<"-------My Task---------create thread "<<((int*)args)[0]<<endl;
}

void* test(void*args)
{
	cout<<"test..."<<((ThreadPool*)args)->test<<endl;
}

int main()
{
	int n;
	cin>>n;

	ThreadPool myThreadPool(n);
	myThreadPool.test=1000;
	pthread_t tid1,tid2;
	//pthread_create(&tid1,NULL,test,(void*)&myThreadPool);
	//pthread_create(&tid2,NULL,manageTask,(void*)&myThreadPool);
	myThreadPool.initSystem();
	
	void*r=NULL;
	int *a=new int[100];
	
	//sleep(5);
	cout<<"start adding task..."<<endl;
	for(int i=0;i<10;++i){
		a[i]=i;
		myThreadPool.addTask(create_thread,(void*)&a[i],r);
	}

	pthread_join(tid1,NULL);
	//pthread_join(tid2,NULL);

	return 0;
}
