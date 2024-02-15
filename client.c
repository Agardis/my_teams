#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#define PORT 56005
#define BUF_SIZE 100


int create_socket(int port)
{
	struct sockaddr_in server_address;

	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
	server_address.sin_port = htons(port);

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		printf("socket creation failed\n");
		printf("%s\n",strerrorname_np(errno));
		exit(-1);
	}

	if(connect(sockfd, (struct sockaddr*)&server_address, sizeof(server_address)) != 0)
	{
		printf("Could not connect to %s:%d\n", "127.0.0.1", port);
		printf("%s\n",strerrorname_np(errno));
		exit(-1);
	}
	printf("Connected to %s:%d\n", "127.0.0.1", port);

	return sockfd;
}

void handle_receive_msg(int sockfd)
{
	char buffer[BUF_SIZE];
	int status;

	memset(buffer, '\0', BUF_SIZE);
	status = recv(sockfd, buffer, BUF_SIZE, 0);
	if(status == 0)
		printf("Server is offline !\n");
	else if(status == -1)
	{
		printf("recv failed\n");
		printf("%s\n",strerrorname_np(errno));
		exit(-1);
	}
	printf("%s\n", buffer);
}

void handle_user_input(int sockfd)
{
	char buffer[BUF_SIZE];

	memset(buffer, '\0', BUF_SIZE);

	fgets(buffer, BUF_SIZE, stdin);

	if(send(sockfd, buffer, BUF_SIZE, 0) == -1)
	{
		printf("send failed\n");
		printf("%s\n",strerrorname_np(errno));
		exit(-1);
	}
}

int main(int argc, char const *argv[])
{
	int sockfd = create_socket(PORT);

	fd_set all_sockets;
	fd_set read_sockets;
	int max_fd;
	struct timeval timer;
	int readable_fd_nbr;

	FD_ZERO(&all_sockets);
	FD_ZERO(&read_sockets);
	FD_SET(STDIN_FILENO, &all_sockets);
	FD_SET(sockfd, &all_sockets);

	max_fd = sockfd;

	while(1)
	{
		read_sockets = all_sockets;

		timer.tv_sec = 1;
		timer.tv_usec = 0;

		readable_fd_nbr = select(max_fd + 1, &read_sockets, NULL, NULL, &timer);
		if(readable_fd_nbr == -1)
		{
			printf("select failed\n");
			printf("%s\n",strerrorname_np(errno));
			exit(-1);
		}
		if(readable_fd_nbr == 0)
			continue;
		for (int i = 0; i <= max_fd; ++i)
		{
			if(FD_ISSET(i, &read_sockets) == 0)
				continue;
			if(i == sockfd)
				handle_receive_msg(sockfd);
			else
				handle_user_input(sockfd);
		}
	}

	close(sockfd);

	return 0;
}