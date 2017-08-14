all : testPool

testPool : testPool.o ThreadPool.o
	g++ -o testPool testPool.o ThreadPoolLib.o -lpthread

testPool.o : testPool.c ThreadPoolLib.c
	g++ -c testPool.c

ThreadPool.o : ThreadPoolLib.c
	g++ -c ThreadPoolLib.c

clean :
	rm -rf testPool
