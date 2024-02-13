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



int main(int argc, char const *argv[])
{
	struct sockaddr_in server_address;

	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
	server_address.sin_port = htons(PORT);

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		printf("socket creation failed \n");
		exit(0);
	}
	printf("socket created ! \n");

	if (connect(sockfd, (struct sockaddr*)&server_address, sizeof(server_address)) != 0)
	{
		printf("connection failed \n");
		printf("%s\n", strerror(errno));
		exit(0);
	}
	printf("Connected to server \n");

	char buffer[80];

	while(1)
	{
		int i = 0;
		while((buffer[i++] = getchar()) != '\n');

		send(sockfd, buffer, sizeof(buffer), 0);
	}

	close(sockfd);

	return 0;
}