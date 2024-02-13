#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#define PORT 56005



void handle_messages(int client_socket)
{
	char buffer[80];

	while(1)
	{
		read(client_socket, buffer, sizeof(buffer));

		printf("client: %s", buffer);
	}	
}



int main(int argc, char const *argv[])
{
	struct sockaddr_in server_address, client_address;

	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = htons(PORT);

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		printf("socket creation failed \n");
		exit(0);
	}
	printf("socket created ! \n");

	if((bind(sockfd, (struct sockaddr*)&server_address, sizeof(server_address))) != 0)
	{
		printf("bind failed \n");
		printf("%s\n",strerrorname_np(errno));
		exit(0);
	}
	printf("socket binded to port : %d \n", PORT);

	if(listen(sockfd, 3) != 0)
	{
		printf("listen failed \n");
		exit(0);
	}
	printf("server is listening !\n");

	int len = sizeof(client_address);

	int client_socket = accept(sockfd, (struct sockaddr*)&client_address, &len);

	if( client_socket < 0)
	{
		printf("connection rejected \n");
		printf("%s\n",strerror(errno));
		exit(0);
	}

	handle_messages(client_socket);

	close(sockfd);

	return 0;
}