#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <sys/un.h>
#include <sys/msg.h>
#include <pthread.h>

#define PORT 8080

////#define UDP
#ifndef UDP
#define TCP
#endif

//#define sameHost

#define SOCKET_NAME "/tmp/socket.socket"

void* callSocket(void* arg)
{
	int cliSock, clientID;
	#ifdef sameHost
	struct sockaddr_un addressServ;
	////remove("/tmp/socket.socket");
	#else
	struct sockaddr_in addressServ;
	#endif
	char buffer[1024] = "xin chao, this is message from process use udp protocol\n";
	
	////if((cliSock = socket(AF_LOCAL, SOCK_DGRAM, 0)) < 0)
	
	#ifdef UDP
	cliSock = socket(AF_INET, SOCK_DGRAM, 0);
	#elif defined(sameHost)
	cliSock = socket(AF_UNIX, SOCK_SEQPACKET, 0);
	#else
	cliSock = socket(AF_INET, SOCK_STREAM, 0);
	#endif
	if(cliSock < 0)
	{
		printf("create client socket is failed, errno: %d\n", errno);
		exit(EXIT_FAILURE);
	}
	
	#ifdef sameHost
	memset(&addressServ, 0, sizeof(addressServ));
	addressServ.sun_family = AF_UNIX;
	strncpy(addressServ.sun_path, SOCKET_NAME, sizeof(addressServ.sun_path) -1);
	#else
	addressServ.sin_family = AF_INET;

	//convert IPV4, IPV6 from text to binary
	//addressServ.sin_addr.s_addr = inet_addr("192.168.100.17"); // <==> inet_pton
	//if(inet_pton(AF_INET, "192.168.100.17", &addressServ.sin_addr) <= 0)
	if(inet_pton(AF_INET, "192.168.100.6", &addressServ.sin_addr) <= 0)
	{
		printf("IN valid ip address: error num: %d\n", errno);
		exit(EXIT_FAILURE);
	}
	addressServ.sin_port = htons(PORT);

	#endif
	//connect, connect <==> bind in server side
	
	int flags = 0;
	
	#ifdef TCP
	if((clientID = connect(cliSock, (const struct sockaddr*) &addressServ, sizeof(addressServ))) < 0)
	{
		printf("connect faile, error num: %d\n", errno);
	}

	#ifdef sameHost
	write(cliSock, &buffer, strlen(buffer));
	#endif
	int checkSend = send(cliSock, &buffer, strlen(buffer), flags); //for TCP
	
	#else
	int checkSend = sendto(cliSock, &buffer, strlen(buffer), flags, ( struct sockaddr*) &addressServ, sizeof(addressServ)); //for UDP
	#endif
	
	if(checkSend != strlen(buffer))
	{
		printf("send fail :%d\n", errno);
	}
	close(clientID);
	#ifdef sameHost
	remove("/tmp/socket.socket");
	#endif
	pthread_exit(0);

}


void* callMsgQueue(void* arg)
{
	//create message queue struct
	struct msg_buffer{
		long msg_type;
		char msg[100];
	}message;

	key_t mykey;
	int msg_id;

	//create unique key
	mykey = ftok("progfile", 65);

	//create message queue, return msg_id
	msg_id = msgget(mykey, 0666 | IPC_CREAT);

	//set message type;
	message.msg_type = 1;

	//get message from user
	printf("Type message to send: ");
	fgets(message.msg, sizeof(message.msg), stdin);

	//send message to user
	msgsnd(msg_id, &message, sizeof(message), 0);

	//remove msg_id
	msgctl(msg_id, IPC_RMID, NULL);
	pthread_exit(0);
}





int main(int argc, char* argv[])
{
	void* ptrRunSocket = NULL;
	void* ptrRunMsgQueue = NULL;
	pthread_t runSocket;
	pthread_t runMsgQueue;

	//create thread
	pthread_create(&runSocket, NULL, callSocket, NULL);
	pthread_create(&runMsgQueue, NULL, callMsgQueue, NULL);
	pthread_join(runSocket, &ptrRunSocket);
	pthread_join(runMsgQueue, &ptrRunMsgQueue);
	return 0;
}
