#include "bankingServer.h"

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

int server_socket = -1;
int client_socket = -1;

// pthread_t* ts2;
int ts_index=0;

struct Account* head = NULL;



void error(char *msg)
{
	perror(msg);
	exit(1);
	
}


void make_active(char* name)
{

	struct Account* temp = NULL;
	temp = head;

	while(temp != NULL)
	{
		if(strcmp(temp->name, name) == 0)
		{
			temp->inSession = 1;
		}
		temp = temp->next;
	}
	printf("%s is in session.\n", name);
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
	printf("NAME: %s\n", name);
	while(temp != NULL)
	{
		if(strcmp(temp->name, name) == 0)
			break;

		temp = temp->next;
	}

	char buffer[256] = {0};
	sprintf(buffer, "%.5f", temp->balance);
	//send(client_socket,buffer, sizeof(buffer), 0);
	write(client_socket, buffer, sizeof(buffer));
	return temp->balance;
}

int withdraw(double ammount, char* name)
{
	struct Account* temp = NULL;
	temp = head;
	printf("NAME: %s\n", name);
	while(temp != NULL)
	{
		if(strcmp(temp->name, name) == 0)
			break;

		temp = temp->next;
	}
	if((temp->balance - ammount) < 0)
		return -1;
	//pthread_mutex_lock(&lock);
	temp->balance -= ammount;
	//pthread_mutex_unlock(&lock);
	return 0;
}

void deposit(double ammount, char* name)
{
	struct Account* temp = NULL;
	temp = head;
	printf("NAME: %s\n", name);
	while(temp != NULL)
	{
		if(strcmp(temp->name, name) == 0)
			break;

		temp = temp->next;
	}
	//pthread_mutex_lock(&lock);
	temp->balance += ammount;
	//pthread_mutex_unlock(&lock);
	printf("Deposited Money\n");
}

int add_account(char* name)
{
	struct Account* new_acct = (struct Account*)malloc(sizeof(struct Account));

	int i = 0; 

	if(name[0] == '\0')
	{
		error("ERROR: account added is empty.");
	}
	while(name[i] != '\n')
	{
		if(name[i] == '\0')
		{
			break;
		} 
		//printf("(%c)", name[i]);
		i++;
	}
	name[i] = '\0';

	char* act_name = (char*)calloc(255,sizeof(char));

	//printf("||%s||\n", name);
	memcpy(act_name, name, strlen(name) + 1);

	strcat(new_acct->name, act_name);

	new_acct->balance = 0.0;
	new_acct->inSession = 0;

	new_acct->next = head;
	head = new_acct;

	printf("Added account (%s)\n", new_acct->name);
	return 1;

}

void prompt_improper_command()
{
	char buff[256] = {0};

	strcat(buff, "\nImproper Input\n");

	//send(client_socket, buff, sizeof(buff), 0);
	write(client_socket, buff, sizeof(buff));
}

void prompt_main_menu()
{
	char buff[256] = {0};

	strcat(buff, "------Main Menu------\n1. create <username (char)>\n2. serve <username (char)>\n3. quit\n---------------------");

	//send(client_socket, buff, sizeof(buff), 0);
	write(client_socket, buff, sizeof(buff));
}

void prompt_serve_menu()
{
	char buff[256] = {0};

	strcat(buff, "------Serve Menu------\n1. deposit <amount (double)>\n2. withdraw <amount (double)>\n3. query\n4. end\n---------------------");

	//send(client_socket, buff, sizeof(buff), 0);
	write(client_socket, buff, sizeof(buff));
}

void* client_handler(void* fd)
{
	
	int newfd = *(int*)fd;


	char buffer[256];
	int status = 0;

	// prompt_main_menu();

	while(read(newfd, buffer, sizeof(buffer)) > 0){
		if(strncmp(buffer, "create ", 7) == 0){
			char* name = &buffer[7];
			if(strlen(name) <= 0){
				write(newfd, "** Server: enter a valid name **\n", 32);
				continue;
			}
			printf("adding %s\n", name);
			int name_check = find_account_by_name(name);
			if(name_check == 1)
			{
				printf("Name already exists\n");
				continue;
			}
			pthread_mutex_lock(&lock);
			if(add_account(name) == 1)
			{
				write(newfd, "Successfully added account.\n", 28);
				bzero(buffer, sizeof(buffer));
				//sleep(1);
				//system("clear");
			}
			pthread_mutex_unlock(&lock);
			strcat(buffer, "------Main Menu------\n1. create <username (char)>\n2. serve <username (char)>\n3. quit\n---------------------");

			write(newfd, buffer, sizeof(buffer));
			bzero(buffer, sizeof(buffer));
			
		}
		else if(strncmp(buffer, "serve ", 6) == 0)
		{
			char* storeName = (char*)malloc(sizeof(buffer));
			char* name = &buffer[6];
			strcpy(storeName, name);
			pthread_mutex_lock(&lock);
			make_active(name);
			pthread_mutex_unlock(&lock);

			if(status == 0)
			{
				continue;
			}

			while(recv(newfd, buffer, sizeof(buffer), 0) > 0)
			{
				if(strncmp(buffer, "create ", 5) == 0)
				{
					write(newfd, "** Server: Can not open a new account in session **\n", 53);
				}
				else if(strncmp(buffer, "serve ", 6) == 0)
				{
					write(newfd, "** Server: Can not start a session while in session **\n", 58);
				}
				else if(strncmp(buffer, "end", 6) == 0)
				{
						// pthread_mutex_lock(&lock);
						// finish(storeName);
						// pthread_mutex_unlock(&lock);
						write(newfd, "** Server: Session was ended **\n", 35);
						break;
				}
				else if(strncmp(buffer, "deposit ", 7) == 0)
				{
					char* amount = &buffer[7];
					float numAmt = atof(amount);
					//credit(storeName, numAmt);
				}
				else if(strncmp(buffer, "withdraw ", 6) == 0)
				{
					char* amount = &buffer[6];
					float numAmt = atof(amount);
					//debit(storeName, numAmt);					
				}
				else
				{
					write(newfd, "** Server: Please enter a valid Command **\n", 44);
				}
			}
		}
		else
		{
			write(newfd, "** Server: either open, start, or exit command **\n", 51);
		}
		bzero(buffer, 256);
		prompt_main_menu();
	}
	close(newfd);
	printf("Client %d has disconnected\n", ts_index);
	ts_index--;
	//printAccts();
	pthread_exit(0);
}



void disconnected(){
	//printAccts();
	printf("Server is disconnecting...\n");
	close(client_socket);
	close(server_socket);
	exit(0);
}

int main(int argc, char* argv[])
{
	system("clear");

	signal(SIGINT, disconnected);

	int port= -1;
	port = atoi(argv[1]);

	int sockfd, newsockfd;
	struct sockaddr_in serv_addr, client_addr;

	pthread_t ts;





	// ts2 = (pthread_t *) malloc(sizeof (pthread_t));
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in server_address;

	server_address.sin_family = AF_INET;

	server_address.sin_port = htons(port);

	server_address.sin_addr.s_addr = INADDR_ANY;



	if(bind(sockfd, (struct sockaddr*) &server_address, sizeof(server_address)) < 0)
		error("ERROR: binding error\n");


	listen(sockfd, 20);

	printf("Waiting for clients on port %d.....\n", port);

	socklen_t clilen = sizeof(client_addr);
	
	while(1)
	{
		newsockfd = accept(sockfd, (struct sockaddr *) &client_addr, &clilen);

		if(newsockfd < 0)
		{
			error("ERROR: accept error\n");
		}
		else
		{
			printf("Connection accepted from [%s, %d]\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
		}
		
		pthread_t id;
		pthread_create(&id, NULL, client_handler, (void*)&newsockfd);

	}
	close(sockfd);
	close(newsockfd);
	return 0;
}
