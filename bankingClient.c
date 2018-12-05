#include "bankingServer.h"



int network_socket = -1;

pthread_t output;
pthread_t input;

void* exit_func(void* fd)
{
    int newfd = *(int*)fd;
    printf("Exiting program..\n");
    close(newfd);
    exit(0);
}



void* send_user_commands(void* fd)
{
    int newfd = *(int*)fd;
    char buff[256] = {0};
    //strcat(buff, "Sending from Client");
    // printf("------------------------------------------\n");
    // printf("     Welcome to Multithreaded-Bank\n");
    // printf("------------------------------------------\n\n");
    while(1)
    {
       
        if(read(0, buff, 255) < 0)
            write(2, "An error occurred in the read.\n", 31);

        write(newfd, buff, sizeof(buff));
        bzero(buff, 255);
    }
    pthread_exit(NULL);
}

void* outputFromServer(void* fd)
{
    int newfd = *(int*)fd;
    
    char buff[256] = {0};

    while(recv(newfd, buff, sizeof(buff), 0) > 0)
    {
        system("clear");
        
        
    
        if(strncmp(buff, "Quitting..", 10) == 0)
        {
            exit_func(fd);
        }
        printf("%s\n", buff);
        bzero(buff,256);
    }
    pthread_exit(NULL);

}

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char* argv[])
{

    system("clear");

    int sockfd, numbytes;  
    char buf[256];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];



    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(argv[1], argv[2], &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) 
        {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) 
        {
            close(sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }

    if (p == NULL) 
    {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);
    printf("client: connecting to %s\n", s);



    if(pthread_create(&output, NULL, outputFromServer, (void*)&sockfd) < 0){
        printf("Error: output thread was not created\n");
        exit(0);
    }
    

    //User Read Thread (INPUT)  
    if(pthread_create(&input, NULL, send_user_commands, (void*)&sockfd) < 0){
        printf("Error: input thread was not created\n");
        exit(0);
    }
    
    pthread_join(output, NULL);
    pthread_join(input, NULL);

    //close(sockfd);






    return 0;
}


