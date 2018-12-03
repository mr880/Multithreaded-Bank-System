#include "bankingServer.h"



int network_socket = -1;

pthread_t output;
pthread_t input;

void exit_func()
{
    printf("Exiting program..\n");
    close(network_socket);
    exit(0);
}



void* send_user_commands()
{
    char buff[256] = {0};
    //strcat(buff, "Sending from Client");
    printf("------------------------------------------\n");
    printf("     Welcome to Multithreaded-Bank\n");
    printf("------------------------------------------\n\n");
    while(1)
    {
       
        if(read(0, buff, 255) < 0)
            write(2, "An error occurred in the read.\n", 31);
        

        // if(strncmp(buff, "quit", 4) == 0)
        // {
        //     printf("Disconnecting from the server. Ending client process.\n");
        //     exit_func();
        // }


        write(network_socket, buff, sizeof(buff));
        bzero(buff, 255);
    }
    pthread_exit(NULL);
}

void* outputFromServer()
{
    char buff[256] = {0};

    while(recv(network_socket, buff, sizeof(buff), 0) > 0)
    {
       
        printf("%s\n", buff);
        
        bzero(buff,256);
        
    }
    pthread_exit(NULL);

}



int main(int argc, char* argv[])
{

    system("clear");
    int port = -1;
    struct hostent* IP;
    struct sockaddr_in server_struct;

    signal(SIGINT, exit_func);

    // pthread_t* input = (pthread_t*)malloc(sizeof(pthread_t));
    // pthread_t* output = (pthread_t*)malloc(sizeof(pthread_t));

    if(argc < 3)
    {
        printf("Not Enough Arguments Entered\n");
        exit(0);
    }

    port = atoi(argv[2]);
    IP = gethostbyname(argv[1]);

    network_socket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    bcopy((char *)IP->h_addr, (char *)&server_struct.sin_addr.s_addr, IP->h_length);
    //server_address.sin_addr.s_addr = INADDR_ANY;

    while(connect(network_socket, (struct sockaddr *) &server_address, sizeof(server_address)))
    {
        printf("Waiting to connect...\n");
        sleep(3);
    }

    printf("Connection Successful\n");

    if(pthread_create(&output, NULL, outputFromServer, NULL) < 0){
        printf("Error: output thread was not created\n");
        exit(0);
    }
    

    //User Read Thread (INPUT)  
    if(pthread_create(&input, NULL, send_user_commands, NULL) < 0){
        printf("Error: input thread was not created\n");
        exit(0);
    }
    
    pthread_join(output, NULL);
    pthread_join(input, NULL);








    return 0;
}


