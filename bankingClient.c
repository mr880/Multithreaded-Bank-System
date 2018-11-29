#include "bankingServer.h"


int server_socket = -1;
int network_socket = -1;

void exit_func()
{
    printf("Exiting program..\n");
    close(server_socket);
    exit(0);
}

char* 
trimwhitespace(char *str)
{
    char *end;

    // Trim leading space
    while(isspace((unsigned char)*str)) str++;

    if(*str == 0)  // All spaces?
        return str;

    // Trim trailing space
    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end))
    {
        end--;
    }

    // Write new null terminator character
    end[1] = '\0';

    return str;
}

void* send_user_commands()
{
    char buff[256] = {0};
    //strcat(buff, "Sending from Client");
    printf("------------------------------------\n");
    printf("      Welcome to Server-Bank!!\n");
    printf("------------------------------------\n\n");
    while(1)
    {
        //prompt user here
        // printf("Enter input from list: \n");
        // printf("-\"create\"\n");
        // printf("-\"serve\"\n");
        // printf("-\"deposit\"\n");
        // printf("-\"withdraw\"\n");
        // printf("-\"query\"\n");
        // printf("-\"end\"\n");
        // printf("-\"quit\"\n");

        if(read(0, buff, 255) < 0)
            write(2, "An error occurred in the read.\n", 31);
        

        //char* newbuff = trimwhitespace(buff);
        //printf("newbuff: %s\n", newbuff);
        if(strncmp(buff, "quit", 4) == 0)
        {
            printf("Disconnecting from the server. Ending client process.\n");
            exit_func();
        }


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

    }
    pthread_exit(NULL);

}



int main(int argc, char* argv[])
{
    signal(SIGINT, exit_func);
    pthread_t* input = (pthread_t*)malloc(sizeof(pthread_t));
    pthread_t* output = (pthread_t*)malloc(sizeof(pthread_t));


    network_socket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9002);
    server_address.sin_addr.s_addr = INADDR_ANY;

    while(connect(network_socket, (struct sockaddr *) &server_address, sizeof(server_address)))
    {
        printf("Waiting to connect...\n");
        sleep(3);
    }

    printf("Connection Successful\n");

    if(pthread_create(output, NULL, outputFromServer, NULL) < 0){
        printf("Error: output thread was not created\n");
        exit(0);
    }
    

    //User Read Thread (INPUT)  
    if(pthread_create(input, NULL, send_user_commands, NULL) < 0){
        printf("Error: input thread was not created\n");
        exit(0);
    }
    
    pthread_join(*output, NULL);
    pthread_join(*input, NULL);








    return 0;
}


