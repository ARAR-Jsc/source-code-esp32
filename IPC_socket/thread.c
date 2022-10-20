#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <inttypes.h>     //uint8_t
#include <fcntl.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <stdarg.h>       //for exec() family
#include <stdlib.h>       //for EXIT_FAILURE
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h> 
#include <arpa/inet.h>
#include <sys/un.h>

#define WR _IOW('b','b', int32_t*)

#define thread

#define PORT 8080

////#define UDP
#ifndef UDP
#define TCP
#endif


#define msgQueue

#ifdef msgQueue
#include <sys/ipc.h>        //for ftok()
#include <sys/msg.h>
#endif

//#define sameHost

#define SOCKET_NAME "/tmp/socket.socket"

void* sockFD(void* arg)
{
	int sockFd, add_len, newSocket;
	int opt = 1;

	#ifdef sameHost
	struct sockaddr_un address;
	remove("/tmp/socket.socket");
	#else
	struct sockaddr_in address;
	#endif
	char buffer[1024] = { 0 };
	
	add_len = sizeof(address);
	
	//create socket
	#ifdef UDP
	sockFd = socket(AF_INET, SOCK_DGRAM, 0);
	#elif defined(sameHost)
	sockFd = socket(AF_UNIX, SOCK_SEQPACKET, 0);
	#else
	sockFd = socket(AF_INET, SOCK_STREAM, 0);
	#endif
	if(sockFd < 0)
	{
		printf("create socket failed\n");
		exit(EXIT_FAILURE);
	}

	#ifndef sameHost

	//force attach new socket to 8080 port

	if(setsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
	{
		printf("Force attach new socket to port 8080 failed, error num: %d\n", errno);
		exit(EXIT_FAILURE);
	}

	#endif
	//bind
	
	#ifdef sameHost
	memset(&address, 0, sizeof(address));
	address.sun_family = AF_UNIX;
	strncpy(address.sun_path, SOCKET_NAME, sizeof(address.sun_path) - 1);
	#else
	address.sin_family = AF_INET;

	////address.sin_addr.s_addr = inet_addr(INADDR_ANY);
	address.sin_addr.s_addr = inet_addr("192.168.100.6"); //<==> inet_pton
	////if(inet_pton(AF_LOCAL, "192.168.100.6", &address.sin_addr) <= 0)
	////{
	////	printf("wrong ip address\n");
	////}
	address.sin_port = htons(PORT);

	#endif
	if(bind(sockFd, (const struct sockaddr*)&address, add_len))
	{
		printf("bind socket failed exitnum: %d -- errno: %d\n", EXIT_FAILURE, errno);
		if(errno != 98)
		{
			exit(EXIT_FAILURE);
		}
	}
	//listen

	#ifdef TCP
	if(listen(sockFd, 3))
	{
		printf("listen failed, error num: %d\n", errno);
		exit(EXIT_FAILURE);
	}

	//confirm establish connection
	
	if((newSocket = accept(sockFd, (struct sockaddr*)&address, &add_len)) < 0)
	{
		printf("Accept failed\n");
		exit(EXIT_FAILURE);
	}
	
	int readData = read(newSocket, buffer, sizeof(buffer));
	
	printf("received msg from TCP: %s\n", buffer);
	
	//close listen
	close(newSocket);
	//close listening
	shutdown(sockFd, SHUT_RDWR);
	#ifdef sameHost
	remove("/tmp/socket.socket");
	#endif
	#else
	int flags = 0;
	while(1)
	{
		int len = recvfrom(sockFd, buffer, sizeof(buffer), flags, (struct sockaddr*) &address, &add_len);
		if(len < 0)
		{
			printf("error when receive msg\n");
		}
		else
		{
			printf("received msg: %s\n", buffer);
		}
	}

	close(sockFd);
	#endif
	pthread_exit(0);
}


void* messageQueue(void* arg)
{
	//create message buffer struct
	struct msg_buffer{
	long msg_type;
	char msg[100];
	}message;

	int msg_id;
	key_t mykey;
	//create unique key

	mykey = ftok("progfile", 65);  //ftok(pathname, projectNum);

	//create meesage queue, return msg id
	if((msg_id = msgget(mykey, 0666 | IPC_CREAT)) == -1)
	{
		printf("Get msg id failed, errno: %d\n", errno);
	}
	
	/*
	send mess to que, 0 in 3rth argument: if none-zero, when error, 
	return immediately, shall not send message
	*/
	if( msgrcv(msg_id, &message, sizeof(message), 1, 0) == -1)
	{
		printf("xin chao , errno: %d\n", errno);
	}   
	printf("Received msg: %s\n", message.msg);
	//destroy mess queue
	msgctl(msg_id, IPC_RMID, NULL);
	pthread_exit(0);

}





struct ab
{
	int a;
	int b;
}calAB;

int commonVar = 10;


pthread_mutex_t mutexVar;

#ifdef thread

void* calSum(void* arg)
{
	struct ab *calA = arg;
	////printf("dump address :%x\n", &calA->a);
	unsigned int i = 0;
	printf("dump value of sum: a + b : %d\n", calA->a + calA->b);
	for(i = 0; i < 65535; i++)
	{
		pthread_mutex_lock(&mutexVar);
		commonVar += 1;
		pthread_mutex_unlock(&mutexVar);
	}
	printf("id of this thread: %lu\n", syscall(SYS_gettid));
	pthread_exit(0);
}


void* calSub(void* arg)
{
	struct ab* abValue = (struct ab*) arg;
	int* pointer3 = NULL;
	uint16_t count2;

	for(count2 = 0; count2 < 65535; count2++)
	{
		pthread_mutex_lock(&mutexVar);
		commonVar += 1;
		pthread_mutex_unlock(&mutexVar);
	}
	printf("result: a - b: %d\n", abValue->a - abValue->b);
	pointer3 = &(abValue->a);
	printf("id of this thread3: %lu\n", syscall(SYS_gettid));
	pthread_exit(pointer3);
}

////void* getButton(void* arg)
void getButton(void)
{
	int recVal = 0;
	int fd = open("/dev/ebbchar", O_RDWR);
	if (fd < 0)
	{
		printf("open device file failed\n");
	}
	////ioctl(fd, WR, 43);
	ioctl(fd, _IOW('a','a',int32_t*), &recVal);
	close(fd);
}

#endif


void* newPro(void* arg)
{

	char* argv2[] = {"program", NULL};
	printf("pid of parent process: %d\n", getpid());
	#ifdef sameHost
	execv("/home/orangepi/Desktop/clientSocket", argv2);
	#else
	execv("/home/orangepi/Desktop/testDriver", argv2);
	#endif

}

void signalHandler1(int signo, siginfo_t* info, void* context)
{
	printf("the button is pressed\n");
}


int main(int argc, char* argv[])
{
	#ifdef thread
	void *ptr = NULL;
	void *ptr2 = NULL;
	void *ptr3 = NULL;
	void *ptr4 = NULL;
	void *ptrRunSocket = NULL;
	void *ptrRunMsgQueue = NULL;
	if( pthread_mutex_init(&mutexVar, NULL) != 0)
	{
		printf("init mutex failed\n");
	}

	
	pthread_t runSocket;
	pthread_t runMsgQueue;
	pthread_t newThread;
	pthread_t thread2;
	pthread_t thread3;
	pthread_t thread4;
	calAB.a = 10;
	calAB.b = 20;

	//run socket
	pthread_create(&runSocket, NULL, sockFD, NULL);
	pthread_create(&runMsgQueue, NULL, messageQueue, NULL);
	//signal
	struct sigaction sa = { 0 };
	sa.sa_sigaction=signalHandler1;
	////signal(10, signalHandler);
	sigaction(10, &sa, NULL);

	pthread_create(&newThread, NULL, calSum, (void*)&calAB);
	pthread_create(&thread2, NULL, calSub, (void*)&calAB);
	////pthread_create(&thread3, NULL, getButton, NULL);
	getButton();
	pid_t pid = fork();
	if(pid == 0)
	{
		printf("create child process OK\n");
		pthread_create(&thread4, NULL, newPro, NULL);


	}
	
	//pthread_cancel(newThread); //cancel a thread, in this case newThread
	
	pthread_join(runMsgQueue, &ptrRunMsgQueue);
	pthread_join(runSocket, &ptrRunSocket);
	pthread_join(newThread, &ptr); //wait for child thread end
	pthread_join(thread2, &ptr2); //wait for child thread end
	
	////pthread_join(thread3, &ptr3); //wait for child thread end
	
	pthread_join(thread4, &ptr4); //wait for child thread end
	printf("return value from thread2: %u\n", *((int*)ptr2));
	printf("var commonVal: %d\n", commonVar);
	pthread_mutex_destroy(&mutexVar);
	#endif
	while(1);	
	return 0;
}



















