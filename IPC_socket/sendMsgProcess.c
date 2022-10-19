
#define msgQueue

#ifdef msgQueue  // message queue IPC

#include <stdio.h>
#include <sys/msg.h>
#include <sys/ipc.h>      //for ftok()


struct msg_buffer {
	long msg_type;
	char msg[100];
}message;


int main(int argc, char* argv[])
{
	int msg_id;
	key_t my_key;

	//create unique key
	my_key = ftok("progfile", 65);
	
	// create message queue, return message id

	msg_id = msgget(my_key, 0666 | IPC_CREAT);
	message.msg_type = 1;
	printf("write message: ");
	
	//get message inut from user
	fgets(message.msg, 100, stdin);
	
	// 0 in 3rth argument: none_zero (ex -1),if error --> return immediately, not send msg 
	msgsnd(msg_id, &message, sizeof(message), 0); 
	
	printf("Sent message: %s\n", message.msg);


	return 0;
}

#else  // socket IPC

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
	struct sockaddr_un address;
	int sockLocal;
	char buffer[1024] = "xin chao";

	//create socket

	sockLocal = socket(AF_UNIX, SOCK_SEQPACKET, 0);

	if(sockLocal < 0)
	{
		printf("create socket failed, error no: %d\n", errno);
	}

	//remove address struct

	memset(&address, 0, sizeof(address));
	
	address.sun_family = AF_UNIX;
	//copy data to address
	strncpy(address.sun_path, SOCKET_NAME, sizeof(address.sun_path) -1);


	//connect
	if(connect(sockLocal, (struct sockaddr*)&address, sizeof(address)))
	{
		printf("Connect failed, error no: %d\n", errno);
	}

	//send data to socket
	if(write(sockLocal, &buffer, sizeof(buffer)) == -1)
	{
		printf("write failed, error no: %d\n", errno);
	}

	//close
	close(sockLocal);


	
	return 0;
}


#endif








































