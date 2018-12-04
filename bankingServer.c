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
	printf("NAME: %s\n", name);
	while(temp != NULL)
	{
		if(strcmp(temp->name, name) == 0)
			break;

		temp = temp->next;
	}

	char buffer[256] = {0};
	sprintf(buffer, "%.2f", temp->balance);
	//send(client_socket,buffer, sizeof(buffer), 0);
	printf("Ballance: %f\n", temp->balance);
	
	return temp->balance;
}

double withdraw(double ammount, char* name)
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
	printf("NAME: %s\n", name);
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

void print_accounts()
{
	system("clear");
	printf("\t\t\t\tBank-System\n");
	printf("Account Name\t\tActive (0 or 1)\t\tCurrent Ballance\n");
	struct Account* temp = NULL;
	temp = head;

	while(temp != NULL)
	{
		printf("%s\t\t\t%d\t\t\t%f\n", temp->name, temp->inSession, temp->balance);
		temp = temp->next;
	}
}

void* client_handler(void* fd)
{
	
	int newfd = *(int*)fd;


	char* buffer = malloc(255);
	int status = 0;

	// prompt_main_menu();
	strcat(buffer, "\t\t\t\tMain Menu\n\n1. create <username (char)>\n2. serve <username (char)>\n3. quit\n");

	write(newfd, buffer, 255);
	
	bzero(buffer, 255);

	while(read(newfd, buffer, 255) > 0){

		print_accounts();
		trim_newline(buffer);

		if(strncmp(buffer, "create ", 7) == 0){
			
			char* name = calloc(255, sizeof(char));
			name = &buffer[7];

			if(strlen(name) <= 0){
				write(newfd, "** Server: enter a valid name **\n", 32);
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
				sleep(1);
				continue;
			}
			pthread_mutex_lock(&lock);
			if(add_account(name) == 1)
			{
				write(newfd, "Successfully added account.\n", 28);
				bzero(buffer, 255);
				sleep(1);
				//system("clear");
			}
			pthread_mutex_unlock(&lock);
			
			bzero(buffer, 255);
			
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
				sleep(1);
				continue;
			}

			int active = make_active(storeName);
	
			if(active == 0)
			{
				write(newfd, "Account is already in session\n", 30);
				bzero(buffer, 255);
				sleep(1);
				continue;
			}

			while(recv(newfd, buffer, 255, 0) > 0)
			{
				print_accounts();
				trim_newline(buffer);

				if(strncmp(buffer, "create ", 7) == 0)
				{
					write(newfd, "** Server: Can not open a new account in session **\n", 53);
				}
				else if(strncmp(buffer, "serve ", 6) == 0)
				{
					write(newfd, "** Server: Can not start a session while in session **\n", 58);
				}
				else if(strncmp(buffer, "end", 3) == 0)
				{
						
						int inactive = make_inactive(storeName);
						
						if(inactive == 0)
						{
							write(newfd, "Account is already inactive\n", 28);
							bzero(buffer, 255);
							sleep(1);
							continue;
						}
						write(newfd, "** Server: Session was ended **\n", 35);
						bzero(buffer, 255);
						sleep(1);
						break;
				}
				else if(strncmp(buffer, "deposit ", 8) == 0)
				{
					char* amount = &buffer[7];
					float new_amount = atof(amount);
					deposit(new_amount, storeName);
					write(newfd, "** Server: Deposited funds **\n", 30);
					bzero(buffer, 255);
					sleep(1);
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
						sleep(1);
						continue;			
					}
					printf("Withdrew %f from account \"%s\"\n", new_amount, storeName );
					write(newfd, "** Server: Withdrew funds **\n", 30);
					bzero(buffer, 255);
					sleep(1);					
				}
				else if(strncmp(buffer, "query", 5) == 0 )
				{
					double balance = get_current_balance(storeName);
					bzero(buffer, 255);
					sprintf(buffer, "Current Balance: %.2f", balance);
					write(newfd, buffer, 255);
					sleep(2);
				}
				bzero(buffer, 255);
				strcat(buffer, "\t\t\t\tServe Menu\n\n1. deposit <amount (double)>\n2. withdraw <amount (double)>\n3. query\n4. end\n");

				//send(client_socket, buff, sizeof(buff), 0);
				write(newfd, buffer, 124);
				bzero(buffer, 255);
				
			}
		}
		else if(strncmp(buffer, "quit", 4) == 0)
		{
			bzero(buffer, 255);
			strncat(buffer, "Quitting..", 10);
			write(newfd, buffer, 10);
			break;
		}
		
		strcat(buffer, "\t\t\t\tMain Menu\n\n1. create <username (char)>\n2. serve <username (char)>\n3. quit\n");

		write(newfd, buffer, 255);
		
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

	

	listen(sockfd, listencount);

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
