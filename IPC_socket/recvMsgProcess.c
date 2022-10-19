#define msgQueue


#ifdef msgQueue

#include <stdio.h>
#include <sys/msg.h>
#include <sys/ipc.h>        //for ftok()


struct msg_buffer{
	long msg_type;
	char msg[100];
}message;

int main(int argc, char* argv[])
{
	int msg_id;
	int my_key;

	//create msg id
	my_key = ftok("progfile", 65);

	//create message queue, return msg id
	msg_id = msgget(my_key, 0666 | IPC_CREAT);

	////message.msg_type = 1;

	msgrcv(msg_id, &message, sizeof(message), 1, 0);

	printf("Received meaasge: %s\n", message.msg);

	//remove (destroy) message queue

	msgctl(msg_id, IPC_RMID, NULL);

	return 0;
}

#else
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <sys/un.h>
#include <string.h>
#define SOCKET_NAME "/tmp/acbdcdc.socket"


int main(int argc, char* argv[])
{
	struct sockaddr_un socketName;
	int sockServ, newSockServ;
	int addLen = sizeof(socketName);
	char buffer[1024] = {0};

	//create socket

	sockServ = socket(AF_UNIX, SOCK_SEQPACKET, 0);

	if(sockServ < 0)
	{
		printf("create socket failed, error no: %d\n", errno);
	}

	//remove address struct

	memset(&socketName, 0, sizeof(socketName));
	
	socketName.sun_family = AF_UNIX;
	//copy data to address
	strncpy(socketName.sun_path, SOCKET_NAME, sizeof(socketName.sun_path) -1);

	//bind

	if(bind(sockServ, (const struct sockaddr*)&socketName, sizeof(socketName)))
	{
		printf("Bind failed, error no: %d\n", errno);
	}

	if(listen(sockServ, 10))
	{
		printf("Listen failed, error no: %d\n", errno);
	}
	
	if((newSockServ = accept(sockServ, (struct sockaddr*)&socketName, &addLen)) < 0)
	{
		printf("Accept failed, error no: %d\n", errno);
	}

		
	if(read(newSockServ, &buffer, sizeof(buffer)) < 0)
	{
		printf("Read failed, error no, %d\n", errno);
	}
	
	printf("received msg from socket: %s\n", buffer);

	//close scoket;
	close (newSockServ);

	//stop listen
	shutdown(sockServ, SHUT_RDWR);
		
	//remove socket file
	if(remove("/tmp/acbdcdc.socket"))
	{
		printf("Remove file failed, error no, %d\n", errno);
	}

	return 0;
}

#endif
