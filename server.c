#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#define PORT 56005
#define MSG_SIZE 100


int create_server_socket()
{
	struct sockaddr_in server_address;
	int sockfd;
	
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	server_address.sin_port = htons(PORT);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd == -1)
	{
		printf("socket creation failed\n");
		printf("%s\n",strerrorname_np(errno));
		exit(-1);
	}

	if(bind(sockfd, (struct sockaddr*)&server_address, sizeof(server_address)) != 0)
	{
		printf("bind failed\n");
		printf("%s\n",strerrorname_np(errno));
		exit(-1);
	}
	printf("Server open on port: %d\n", PORT);

	if(listen(sockfd, 10) != 0)
	{
		printf("listen failed\n");
		printf("%s\n",strerrorname_np(errno));
		exit(-1);
	}
	printf("Server ready !\n");

	return sockfd;
}

void accept_new_connection(int server_socket, fd_set *all_sockets, int *max_fd)
{
	int new_client = accept(server_socket, NULL, NULL);
	if(new_client == -1)
	{
		printf("Accept failed\n");
		printf("%s\n",strerrorname_np(errno));
		exit(-1);
	}
	printf("Client accepted\n");
	FD_SET(new_client, all_sockets);
	if(new_client > *max_fd)
		*max_fd = new_client;

	char *buffer = "You are connected to the server";
	if(send(new_client, buffer, sizeof(buffer), 0) == -1)
	{
		printf("send failed to client %i\n", new_client);
		printf("%s\n",strerrorname_np(errno));
		exit(-1);
	}
}

void read_new_message(int client_socket, fd_set *all_sockets)
{
	char received_msg[MSG_SIZE];

	int status = recv(client_socket, received_msg, MSG_SIZE, 0);
	if(status == 0)
	{
		printf("Client %i disconnected\n", client_socket);
		FD_CLR(client_socket, all_sockets);
	}
	else if(status == -1)
	{
		printf("recv failed\n");
		printf("%s\n",strerrorname_np(errno));
		exit(-1);
	}
	else
		printf("[Client %d]: %s", client_socket, received_msg);
}

int main(int argc, char const *argv[])
{
	int server_socket = create_server_socket();
	
	fd_set all_sockets;
	fd_set read_sockets;
	int max_fd;
	struct timeval timer;
	int readable_fd_nbr;

	FD_ZERO(&all_sockets);
	FD_ZERO(&read_sockets);
	FD_SET(server_socket, &all_sockets);

	max_fd = server_socket;

	while(1)
	{
		read_sockets = all_sockets;

		timer.tv_sec = 1;
		timer.tv_usec = 0;

		readable_fd_nbr = select(max_fd + 1, &read_sockets, NULL, NULL, &timer);
		
		if(readable_fd_nbr == -1)
		{
			printf("Select failed \n");
			printf("%s\n",strerrorname_np(errno));
			exit(-1);
		}
		else if(readable_fd_nbr == 0)
		{
			continue;
		}
		for (int i = 0; i <= max_fd; i++)
		{
			if(FD_ISSET(i, &read_sockets) == 0)
				continue;
			if(i == server_socket)
			{
				accept_new_connection(server_socket, &all_sockets, &max_fd);
			}
			else
			{
				read_new_message(i, &all_sockets);
			}
		}
	}


	return 0;
}