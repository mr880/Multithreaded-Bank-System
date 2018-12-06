#include "bankingServer.h"

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

// int server_socket = -1;
// int client_socket = -1;

// pthread_t* ts2;
int ts_index=0;

struct Account* head = NULL;



void error(char *msg)
{
	perror(msg);
	exit(1);
	
}

char* trim_newline(char* input)
{
	int i = 0; 

	while(input[i] != '\n')
	{
		if(input[i] == '\0')
		{
			break;
		} 
		//printf("(%c)", name[i]);
		i++;
	}
	input[i] = '\0';

	return input;
}

int make_inactive(char* name)
{

	struct Account* temp = NULL;
	temp = head;

	while(temp != NULL)
	{
		if(strcmp(temp->name, name) == 0 )
		{
			if(temp->inSession == 1)
			{
				pthread_mutex_lock(&lock);
				temp->inSession = 0;
				pthread_mutex_unlock(&lock);
			}
			else
			{
				printf("%s was not in session, returning...\n", name);
				return 0;
			}	
		}
		temp = temp->next;
	}
	printf("%s is out of session.\n", name);
	return 1;
}

int make_active(char* name)
{

	struct Account* temp = NULL;
	temp = head;

	while(temp != NULL)
	{
		if(strcmp(temp->name, name) == 0)
		{
			if(temp->inSession == 0)
			{
				pthread_mutex_lock(&lock);
				temp->inSession = 1;
				pthread_mutex_unlock(&lock);
			}
			else
			{
				printf("%s was already in session, returning...\n", name);
				return 0;
			}	
		}
		temp = temp->next;
	}
	printf("%s is in session.\n", name);
	return 1;
}

int find_account_by_name(char* name)
{
	struct Account* temp = NULL;
	temp = head;

	while(temp != NULL)
	{
		if(strcmp(temp->name, name) == 0)
		{
			return 1;
		}
		temp = temp->next;
	}

	return 0;
}

double get_current_balance(char* name)
{
	struct Account* temp = NULL;
	temp = head;
	//printf("NAME: %s\n", name);
	while(temp != NULL)
	{
		if(strcmp(temp->name, name) == 0)
			break;

		temp = temp->next;
	}

	char buffer[256] = {0};
	sprintf(buffer, "%.2f", temp->balance);
	//send(client_socket,buffer, sizeof(buffer), 0);
	printf("Ballance: %.2f\n", temp->balance);
	
	return temp->balance;
}

double withdraw(double ammount, char* name)
{
	struct Account* temp = NULL;
	temp = head;
	//printf("NAME: %s\n", name);
	while(temp != NULL)
	{
		if(strcmp(temp->name, name) == 0)
			break;

		temp = temp->next;
	}
	if((temp->balance - ammount) < 0)
		return -1.1;
	pthread_mutex_lock(&lock);
	temp->balance -= ammount;
	pthread_mutex_unlock(&lock);
	return temp->balance;
}

void deposit(double ammount, char* name)
{
	struct Account* temp = NULL;
	temp = head;
	//printf("NAME: %s\n", name);
	while(temp != NULL)
	{
		if(strcmp(temp->name, name) == 0)
			break;

		temp = temp->next;
	}
	pthread_mutex_lock(&lock);
	temp->balance += ammount;
	pthread_mutex_unlock(&lock);
	printf("Deposited Money\n");
}

int add_account(char* name)
{
	struct Account* new_acct = (struct Account*)malloc(sizeof(struct Account));

	//char* act_name = (char*)calloc(255,sizeof(char));

	//printf("||%s||\n", name);
	memcpy(new_acct->name, name, 255);

	//strcat(new_acct->name, act_name);

	new_acct->balance = 0.0;
	new_acct->inSession = 0;

	new_acct->next = head;
	head = new_acct;

	printf("Added account (%s)\n", new_acct->name);
	return 1;

}

void print_accounts()
{
	PAUSE = 1;
	system("clear");
	printf("\t\t\t\tBank-System\n");
	printf("Account Name\t\tCurrent Ballance\t\tIn Service\n");
	struct Account* temp = NULL;
	temp = head;

	while(temp != NULL)
	{
		printf("%s\t\t\t$%.2f\t\t\t\t%d\n", temp->name, temp->balance, temp->inSession);
		temp = temp->next;
	}
	sleep(5);
	PAUSE = 0;
	alarm(15);

}

void* client_handler(void* fd)
{
	
	ts_index++;

	int newfd = *(int*)fd;
	//printf("%d\n", newfd);

	char buffer[255] = {0}; 
	int status = 0;

	// prompt_main_menu();
	strcat(buffer, "\t\t\t\tMain Menu\n\n1. create <username (char)>\n2. serve <username (char)>\n3. quit\n");

	write(newfd, buffer, 255);
	//write(newfd, "JSDIFSDJF", 11);
	//printf("why is this not sending?\n");
	bzero(buffer, 255);
	

	while(read(newfd, buffer, 255) > 0)
	{
	
		trim_newline(buffer);

		if(strncmp(buffer, "create ", 7) == 0 && PAUSE == 0){
			
			char* name = calloc(255, sizeof(char));
			memcpy(name,&buffer[7],255);

			printf("name: %s\n", name);
			if(strlen(name) <= 0){
				write(newfd, "** Server: enter a valid name **\n", 32);
				//sleep(2);
				//free(name);
				write(newfd, "\t\t\t\tMain Menu\n\n1. create <username (char)>\n2. serve <username (char)>\n3. quit\n", 82);
				continue;
			}

			pthread_mutex_lock(&lock);
			int name_check = find_account_by_name(name);
			pthread_mutex_unlock(&lock);

			if(name_check == 1)
			{
				printf("Name already exists\n");
				write(newfd, "Name already exists.", 20);
				bzero(buffer, 255);
				//sleep(2);
				//free(name);
				write(newfd, "\t\t\t\tMain Menu\n\n1. create <username (char)>\n2. serve <username (char)>\n3. quit\n", 82);
				continue;
			}
			pthread_mutex_lock(&lock);
			if(add_account(name) == 1)
			{
				write(newfd, "Successfully added account.\n", 28);
				bzero(buffer, 255);
				//sleep(2);
				//system("clear");
			}
			pthread_mutex_unlock(&lock);
			
			bzero(buffer, 255);
			//free(name);
			//continue;
		}
		else if(strncmp(buffer, "create ", 7) == 0 && PAUSE == 1)
		{
			write(newfd, "Waiting for server to update...", 31);
			//bzero(buffer, 255);
			//sleep(2);
			while(PAUSE == 1)
			{
				//do nothing
			}
			char* name = calloc(255, sizeof(char));
			memcpy(name,&buffer[7],255);

			if(strlen(name) <= 0){
				write(newfd, "** Server: enter a valid name **\n", 32);
				//sleep(2);
				write(newfd, "\t\t\t\tMain Menu\n\n1. create <username (char)>\n2. serve <username (char)>\n3. quit\n", 82);
				continue;

			}

			pthread_mutex_lock(&lock);
			int name_check = find_account_by_name(name);
			pthread_mutex_unlock(&lock);

			if(name_check == 1)
			{
				printf("Name already exists\n");
				write(newfd, "Name already exists.", 20);
				bzero(buffer, 255);
				//sleep(2);
				write(newfd, "\t\t\t\tMain Menu\n\n1. create <username (char)>\n2. serve <username (char)>\n3. quit\n", 82);
				continue;
			}
			pthread_mutex_lock(&lock);
			if(add_account(name) == 1)
			{
				write(newfd, "Successfully added account.\n", 28);
				bzero(buffer, 255);
				//sleep(2);
				//system("clear");
			}
			pthread_mutex_unlock(&lock);
			
			bzero(buffer, 255);
			//continue;

		}
		else if(strncmp(buffer, "serve ", 6) == 0)
		{



			char* storeName = (char*)malloc(sizeof(buffer));
			char* name = &buffer[6];
			strcpy(storeName, name);

			pthread_mutex_lock(&lock);
			int name_check = find_account_by_name(storeName);
			pthread_mutex_unlock(&lock);

			if(name_check == 0)
			{
				write(newfd, "Account name does not exist.\n", 29);
				bzero(buffer, 255);
				//sleep(2);
				write(newfd, "\t\t\t\tMain Menu\n\n1. create <username (char)>\n2. serve <username (char)>\n3. quit\n", 82);
				continue;
			}

			int active = make_active(storeName);
	
			if(active == 0)
			{
				write(newfd, "Account is already in session\n", 30);
				bzero(buffer, 255);
				//sleep(2);
				write(newfd, "\t\t\t\tMain Menu\n\n1. create <username (char)>\n2. serve <username (char)>\n3. quit\n", 82);
				continue;
			}

			write(newfd, "** Server: Entering Serve Menu ** \n", 35);
			bzero(buffer, 255);
			//sleep(2);

			while(recv(newfd, buffer, 255, 0) > 0)
			{
				//print_accounts();
				trim_newline(buffer);

				if(strncmp(buffer, "create ", 7) == 0)
				{
					write(newfd, "** Server: Can not open a new account in session **\n", 53);
					//sleep(2);
				}
				else if(strncmp(buffer, "serve ", 6) == 0)
				{
					write(newfd, "** Server: Can not start a session while in session **\n", 58);
					//sleep(2);
				}
				else if(strncmp(buffer, "end", 3) == 0)
				{
						
						int inactive = make_inactive(storeName);
						
						if(inactive == 0)
						{
							write(newfd, "Account is already inactive\n", 28);
							bzero(buffer, 255);
							//sleep(2);
							continue;
						}
						write(newfd, "** Server: Session was ended **\n", 35);
						bzero(buffer, 255);
						//sleep(2);
						break;
				}
				else if(strncmp(buffer, "deposit ", 8) == 0)
				{
					char* amount = &buffer[7];
					float new_amount = atof(amount);
					deposit(new_amount, storeName);
					write(newfd, "** Server: Deposited funds **\n", 30);
					bzero(buffer, 255);
					//sleep(2);
				}
				else if(strncmp(buffer, "withdraw ", 9) == 0)
				{
					char* amount = &buffer[9];
					float new_amount = atof(amount);
					double newbalance = withdraw(new_amount, storeName);

					if(newbalance == -1.1)
					{
						write(newfd, "** Server: Insufficient funds **\n", 33);
						bzero(buffer, 255);
						//sleep(2);
						continue;			
					}
					printf("Withdrew %f from account \"%s\"\n", new_amount, storeName );
					write(newfd, "** Server: Withdrew funds **\n", 30);
					bzero(buffer, 255);
					//sleep(2);				
				}
				else if(strncmp(buffer, "query", 5) == 0 )
				{
					double balance = get_current_balance(storeName);
					bzero(buffer, 255);
					sprintf(buffer, "Current Balance: %.2f", balance);
					write(newfd, buffer, 255);
					//sleep(2);
				}
				bzero(buffer, 255);
				//strcat(buffer, "\t\t\t\tServe Menu\n\n1. deposit <amount (double)>\n2. withdraw <amount (double)>\n3. query\n4. end\n");

				//send(client_socket, buff, sizeof(buff), 0);
				write(newfd, "\t\t\t\tServe Menu\n\n1. deposit <amount (double)>\n2. withdraw <amount (double)>\n3. query\n4. end\n", 124);
				bzero(buffer, 255);
				//sleep(2);
				
			}
		}
		else if(strncmp(buffer, "quit", 4) == 0)
		{
			bzero(buffer, 255);
			strncat(buffer, "Quitting..", 10);
			write(newfd, buffer, 10);
			//sleep(2);
			break;
		}

		write(newfd, "\t\t\t\tMain Menu\n\n1. create <username (char)>\n2. serve <username (char)>\n3. quit\n", 82);
		//printf("WTF?\n");
		bzero(buffer, 255);
		// prompt_main_menu();
	}
	close(newfd);
	printf("Client %d has disconnected\n", ts_index);
	ts_index--;
	//printAccts();
	pthread_exit(0);
}



void disconnected(){
	//printAccts();
	char buffer[255];
	bzero(buffer, 255);

	printf("Server is disconnecting...\n");
	free(head);
	struct socket* tempsock;
	tempsock = sockhead;

	while(tempsock != NULL)
	{
		//printf("sockhead: %d\n", sockhead->fd);
		send(tempsock->fd, "** Server disconnected **",25,0);
		bzero(buffer, 255);

		tempsock = tempsock->next;


	}
	free(sockhead);
	// close(client_socket);
	// close(server_socket);
	exit(0);
}

void sigchld_handler(int s)
{
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void* server_handler(void* port_num)
{
	//printf("hihihih!\n");
	int port= -1;
	int opt = 1;
	port = atoi((char*)port_num);

	int sockfd, newsockfd;
	struct addrinfo hints, *res, *p;
	struct sockaddr_storage their_addr;
	socklen_t addr_size;

	char s[INET6_ADDRSTRLEN];

	struct sigaction sa;

	memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;  // use IPv4 or IPv6, whichever
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

    getaddrinfo(NULL, (char*)port_num, &hints, &res);


	struct sockaddr_in serv_addr, client_addr;

	pthread_t ts = (pthread_t) malloc(sizeof (pthread_t));

	for(p = res; p != NULL; p = p->ai_next) 
	{
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) 
        {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == -1) 
        {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) 
        {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }


    if(listen(sockfd, 20) < 0)
    {
    	perror("Listening\n");
    	exit(0);
    }

	freeaddrinfo(res); // all done with this structure

	if (p == NULL)  
	{
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }
	// listen(sockfd, listencount);
    if (listen(sockfd, 20) == -1) {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

	printf("Waiting for clients on port %d.....\n", port);

	// socklen_t clilen = sizeof(client_addr);
	
	addr_size = sizeof their_addr;

	while(1)
	{
		newsockfd = accept(sockfd, (struct sockaddr *) &their_addr, &addr_size);

		if(newsockfd < 0)
		{
			perror("accept");
			continue;
		}
		inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
        printf("server: got connection from %s\n", s);
			//printf("Connection accepted from [%s, %d]\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

		struct socket* new_socket = (struct socket*)malloc(sizeof(struct socket));
	    new_socket->fd = newsockfd;
	    new_socket->next = sockhead;
	    sockhead = new_socket;

	    

	    if(first_call == 0)
		{
			system("clear");
			printf("\t\t\t\tBank-System\n");
			printf("Account Name\tCurrent Ballance\tIn Service\n");

			first_call = 1;

			pthread_t refresh_func = (pthread_t)malloc(sizeof(pthread_t));

			pthread_create(&refresh_func, NULL, set_alarm, NULL);
		}
		pthread_t id;
		pthread_create(&id, NULL, client_handler, (void*)&newsockfd);

	}
	
	close(sockfd);
	close(newsockfd);

}

void* set_alarm()
{
	signal(SIGALRM, print_accounts);
	//schedule the first alarm
	alarm(15);

	while(1)
		pause();

	pthread_exit(0);
}

int main(int argc, char* argv[])
{
	system("clear");

	

	signal(SIGINT, disconnected);

	pthread_t server_thread = (pthread_t)malloc(sizeof(pthread_t));

	pthread_create(&server_thread, NULL, server_handler, (void*)argv[1]);


	pthread_join(server_thread, NULL);


	return 0;
}
