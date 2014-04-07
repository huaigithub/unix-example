/*
 *  date:2014-04-06
 *	Beijing
 *  练习fork()函数
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <fcntl.h>

#define	ERR_EXIT(m)\
	do\
	{\
		perror(m);\
		exit(EXIT_FAILURE);\
	}\
	while(0)\


/*===================================================（概述）===========================================================
*	使用fork函数得到的子进程从父进程的继承了整个进程的地址空间，包括：进程上下文、进程堆栈、内存信息、打开的文件描述符、
*	信号控制设置、进程优先级、进程组号、当前工作目录、根目录、资源限制、控制终端等。
*	子进程与父进程的区别在于：
*	1、父进程设置的锁，子进程不继承（因为如果是排它锁，被继承的话，矛盾了）
*	2、各自的进程ID和父进程ID不同
*	3、子进程的未决告警被清除；
*	4、子进程的未决信号集设置为空集。
* ==============================================================================================================
*/

void testFork01(void);

void testFork02(void);

void testVFork01(void);

void testVFork02(void);

void testVFork03(void);

void testFileFork(void);

int main(void)
{
	testFileFork();

	return 0;
}

void testFork01()
{
	pid_t pid;

	printf("在执行fork函数前，执行当前进程的pid为：%d\n", getpid());

	pid = fork();

	if(pid == -1)
	{
		ERR_EXIT("fork error");
	}

	if(pid == 0)
	{
		printf("这是子进程，子进程的pid为：%d, 父进程的pid为：%d\n", getpid(), getppid());
	}
	if(pid>0)
	{
		printf("这是父进程，父进程的pid为：%d, 父进程产生的子进程id为：%d, 父进程的父进程id为：%d\n", getpid(), pid, getppid());
	}

	printf("------------------------ 测试001 ---------------------------\n");
}

/*
*==============================================================================================================
*  当没给父进程没加sleep时，由于父进程先执行完，子进程成了孤儿进程，系统将其托孤给了1（init）进程，所以ppid =1
*  fork系统调用之后，父子进程将交替执行，执行顺序不定。
*  如果父进程先退出，子进程还没退出那么子进程的父进程将变为init进程（托孤给了init进程）。（注：任何一个进程都必须有父进程）
*  如果子进程先退出，父进程还没退出，那么子进程必须等到父进程捕获到了子进程的退出状态才真正结束，否则这个时候子进程就成为僵进程
*  僵尸进程：只保留一些退出信息供父进程查询）
*  ps -ef|grep a.out
*  子进程先退出，但进程列表中还可以查看到子进程，[a.out] <defunct>，死的意思，
*  即僵尸进程，如果系统中存在过多的僵尸进程，将会使得新的进程不能产生。
*==============================================================================================================
*/
void testFork02()
{
	pid_t pid;
	
	printf("在执行fork函数前，执行当前进程的pid为：%d\n", getpid());

	pid = fork();

	if(pid == -1)
	{
		ERR_EXIT("fork error");
	}

	if(pid == 0)
	{
		printf("这是子进程，子进程的pid为：%d, 父进程的pid为：%d\n", getpid(), getppid());
	}
	if(pid>0)
	{
		sleep(5);
		printf("这是父进程，父进程的pid为：%d, 父进程产生的子进程id为：%d, 父进程的父进程id为：%d\n", getpid(), pid, getppid());
	}
	printf("------------------------ 测试002 ---------------------------\n");
}


/*
*==============================================================================================================
*
*linux系统为了提高系统性能和资源利用率，在fork出一个新进程时，系统并没有真正复制一个副本。
*如果多个进程要读取它们自己的那部分资源的副本，那么复制是不必要的。
*每个进程只要保存一个指向这个资源的指针就可以了。
*如果一个进程要修改自己的那份资源的“副本”，那么就会复制那份资源。这就是写时复制的含义

*在fork还没实现copy on write之前。Unix设计者很关心fork之后立刻执行exec所造成的地址空间浪费，所以引入了vfork系统调用。
*vfork有个限制，子进程必须立刻执行_exit或者exec函数。
*即使fork实现了copy on write，效率也没有vfork高，但是我们不推荐使用vfork，因为几乎每一个vfork的实现，都或多或少存在一定的问题
*
*==============================================================================================================
*/
void testVFork01(void)
{
	pid_t pid;
	int val =1;
	printf("调用函数 fork 之前, val = %d\n", val);
	pid = vfork();

	if(pid == -1)
		ERR_EXIT("fork error");
	if(pid == 0){
		printf("子进程，改变val值前，val=%d\n", val);
		val ++;
		printf("这是子进程，val=%d\n", val);
		_exit(0);
	}
	if(pid>0){
		sleep(1);
		printf("这是父进程， val=%d\n", val);
	}
}

void testVFork02(void)
{
	pid_t pid;
	int val =1;
	printf("调用函数 fork 之前, val = %d\n", val);
	pid = fork();

	if(pid == -1)
		ERR_EXIT("fork error");
	if(pid == 0){
		printf("子进程，改变val值前，val=%d\n", val);
		val ++;
		printf("这是子进程，val=%d\n", val);
		_exit(0);
	}
	if(pid>0){
		sleep(1);
		printf("这是父进程， val=%d\n", val);
	}
}

void testVFork03(void)
{
	pid_t pid;
	int val =1;
	printf("调用函数 fork 之前, val = %d\n", val);
	pid = vfork();

	if(pid == -1)
		ERR_EXIT("fork error");
	if(pid == 0){
		printf("子进程，改变val值前，val=%d\n", val);
		val ++;
		printf("这是子进程，val=%d\n", val);
		//_exit(0);
	}
	if(pid>0){
		sleep(1);
		printf("这是父进程， val=%d\n", val);
	}
}


void testFileFork(void)
{
	pid_t pid;
	int fd;
	fd = open("test.txt", O_WRONLY);
	if(fd == -1){
		ERR_EXIT("OPEN ERROR");
	}
	
	pid = fork();

	if(pid == -1){
		ERR_EXIT("fork error");
	}

	if(pid == 0){
		write(fd, "child", 5);
	}

	if(pid >0 ){
		write(fd, "parent", 6);
	}
}