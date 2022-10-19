#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>

#define PORT 8080

#define UDP
#ifndef UDP
#define TCP
#endif

int main(int argc, char* argv[])
{
	int cliSock, clientID;
	struct sockaddr_in addressServ;
	char buffer[1204] = "xin chao, this is message from process use udp protocol\n";
	char* msg = "xin chao, this is message from process use udp protocol";
	
	////if((cliSock = socket(AF_LOCAL, SOCK_DGRAM, 0)) < 0)
	
	#ifdef UDP
	cliSock = socket(AF_INET, SOCK_DGRAM, 0);
	#elif defined(sameHost)
	cliSock = socket(AF_LOCAL, SOCK_DGRAM, 0);
	#else
	cliSock = socket(AF_INET, SOCK_STREAM, 0);
	#endif
	if(cliSock < 0)
	{
		printf("create client socket is failed, errno: %d\n", errno);
		return cliSock;
	}
	
	#ifdef sameHost
	addressServ.sin_family = AF_LOCAL;
	#else
	addressServ.sin_family = AF_INET;
	#endif

	//convert IPV4, IPV6 from text to binary
	//addressServ.sin_addr.s_addr = inet_addr("192.168.100.17"); // <==> inet_pton
	if(inet_pton(AF_INET, "192.168.100.17", &addressServ.sin_addr) <= 0)
	{
		printf("IN valid ip address: error num: %d\n", errno);
		return -1;
	}
	addressServ.sin_port = htons(PORT);

	//connect, connect <==> bind in server side
	
	int flags = 0;
	
	#ifdef TCP
	if((clientID = connect(cliSock, (const struct sockaddr*) &addressServ, sizeof(addressServ))) < 0)
	{
		printf("connect faile, error num: %d\n", errno);
	}
	int checkSend = send(cliSock, msg, strlen(msg), flags); //for TCP
	
	#else
	int checkSend = sendto(cliSock, msg, strlen(msg), flags, ( struct sockaddr*) &addressServ, sizeof(addressServ)); //for UDP
	#endif
	
	if(checkSend != strlen(msg))
	{
		printf("send fail :%d\n", errno);
	}
	close(clientID);
	return 0;
}
