#include "bankingServer.h"

int ts_limit = 5000;

int main(int argc, char * argv[]){

	

	pthread_t* ts = (pthread_t *)malloc (sizeof(pthread_t) * ts_limit);

	


	int sockfd , clientfd, n, portno;
	char buffer[255];

	struct sockaddr_in serv_add , client_add;
	socklen_t clilen;

	

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if(sockfd < 0)
	{
		printf("Error creatin socket\n");
		exit(0);
	}

	int opt = 1;

	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    bzero((char *) &serv_add, sizeof(serv_add));
	portno = atoi(argv[2]);

	serv_add.sin_family = AF_INET;
	serv_add.sin_addr.s_addr = INADDR_ANY;
	serv_add.sin_port = htons(portno);

	if (bind(sockfd , (struct sockaddr *) &serv_add , sizeof(serv_add)) < 0){


		printf("error binding the socket\n");
		exit(0);

	}

	listen(sockfd, 5);

	while(1)
	{

	}


	return 0;

}
