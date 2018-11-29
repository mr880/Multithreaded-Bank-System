#include "bankingServer.h"

int server_socket = -1;
int client_socket = -1;
pthread_t* ts; 

struct Account* head = NULL;

int find_account_by_name(char* name)
{
	struct Account* temp = NULL;
	temp = head;

	while(temp != NULL)
	{
		if(strcmp(temp->name, name) == 0)
			return 1;
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
	send(client_socket,buffer, sizeof(buffer), 0);
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

	temp->balance -= ammount;
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

	temp->balance += ammount;
	printf("Deposited Money\n");
}

void add_account(char* name)
{
	struct Account* new_acct = (struct Account*)malloc(sizeof(struct Account));

	char* act_name = (char*)malloc(sizeof(char)*255);
	memcpy(act_name, name, strlen(name) + 1);

	strcat(new_acct->name, act_name);

	new_acct->balance = 0.0;
	new_acct->inSession = 1;

	new_acct->next = head;
	head = new_acct;
	printf("Added name!\n");
}

void prompt_improper_command()
{
	char buff[256] = {0};

	strcat(buff, "\nImproper Input\n");

	send(client_socket, buff, sizeof(buff), 0);
}

void prompt_main_menu()
{
	char buff[256] = {0};

	strcat(buff, "------Main Menu------\n1. create <username (char)>\n2. serve <username (char)>\n3. quit\n---------------------");

	send(client_socket, buff, sizeof(buff), 0);
}

void prompt_serve_menu()
{
	char buff[256] = {0};

	strcat(buff, "------Serve Menu------\n1. deposit <amount (double)>\n2. withdraw <amount (double)>\n3. query\n4. end\n---------------------");

	send(client_socket, buff, sizeof(buff), 0);
}

void* client_handler(char* hi)
{
	int firstprompt = 1;
	char buffer[256] = {0};
	prompt_main_menu();
	printf("-------Client Handler-------\n");
	while(recv(client_socket, buffer, sizeof(buffer), 0) > 0)
	{
		
		printf("-------Inside Main Menu-------\n");
		if(strncmp(buffer, "create", 6) == 0)
		{
			char* name = &buffer[7];
			printf("Creating %s", name);
			int check = find_account_by_name(name);
			if(check == 0)
			{
				add_account(name);
				bzero(buffer, 256);
				strcat(buffer, "\nAccount Added.");
				send(client_socket,buffer, sizeof(buffer), 0);
			}
			else
			{
				bzero(buffer, 256);
				strcat(buffer, "\nAccount Name Taken.");
				send(client_socket,buffer, sizeof(buffer), 0);

				prompt_main_menu();
				continue;
			}
			
			buffer[0] = '\0';
			
		}	
		else if(strncmp(buffer,"serve", 5) == 0)
		{

			char* name = &buffer[6];
			char* heapname = (char*)malloc(sizeof(name));
			memcpy(heapname, name, strlen(name) + 1);
			//buffer[0] = '\0';
			int check = find_account_by_name(name);
			//printf("Serving %s", name);
			if(check == 1)
			{
				bzero(buffer, 256);
				
				// strcat(buffer, "1. deposit <double>\n2. withdraw <double>\n3. end");
				// send(client_socket,buffer, sizeof(buffer), 0);
			
			}
			else
			{
				bzero(buffer, 256);
				strcat(buffer, "\nCould not locate account\n");
				send(client_socket,buffer, sizeof(buffer), 0);
				prompt_main_menu();
				continue;
			}
			
			printf("-------Inside Serve------\n");
			prompt_serve_menu();

			char newBuffer[256] = {0};
			while(recv(client_socket, newBuffer, sizeof(newBuffer), 0) > 0)
			{
				if(strncmp(newBuffer,"deposit", 7) == 0)
				{
					char* charAmount = &newBuffer[8];
					
					double ammount = atof(charAmount);
					deposit(ammount, heapname);
					
					bzero(buffer, 256);
					strcat(buffer, "\nFunds deposited into account.\n");
					send(client_socket,buffer, sizeof(buffer), 0);

				}
				else if(strncmp(newBuffer,"withdraw", 8) == 0)
				{
					char* charAmount = &newBuffer[8];
					
					double ammount = atof(charAmount);

					int withdraw_check = withdraw(ammount, heapname);

					if(withdraw_check == -1)
					{
						bzero(buffer, 256);
						strcat(buffer, "\nNot enough funds in account.\n");
						send(client_socket,buffer, sizeof(buffer), 0);
						prompt_serve_menu();
						continue;
					}
					bzero(buffer, 256);
					strcat(buffer, "\nWithdrew funds.\n");
					send(client_socket,buffer, sizeof(buffer), 0);

					
				}
				else if(strncmp(newBuffer,"query", 5) == 0)
				{
					bzero(buffer, 256);
					strcat(buffer, "\nBalance: ");
					send(client_socket,buffer, sizeof(buffer), 0);

					double balance = get_current_balance(heapname);
					
				}
				else if(strncmp(newBuffer,"end", 3) == 0)
				{
					bzero(buffer, 256);
					strcat(buffer, "\nExiting Serve Session");
					send(client_socket,buffer, sizeof(buffer), 0);
					printf("end\n");
					break;
				}
				else
				{
					prompt_improper_command();
				}
				prompt_serve_menu();
			}
		}
		
		else if(strncmp(buffer,"quit", 4) == 0)
		{
			printf("quit\n");
			break;
		}
		else
		{
			prompt_improper_command();
		}
		
		prompt_main_menu();
	}
	pthread_exit(NULL);
}


int main(int argc, char* argv[])
{



	ts = (pthread_t *) malloc(sizeof (pthread_t) * 1048);
	char server_message[256] = "You have reached teh server!";

	
	server_socket = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(9002);
	server_address.sin_addr.s_addr = INADDR_ANY;

	bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address));

	listen(server_socket, 5);

	char buff[255];
	while(1)
	{
		

		
		client_socket = accept(server_socket,NULL, NULL);
		printf("Accepted Client Socket.\n");
		send(client_socket, server_message, sizeof(server_message), 0);

		pthread_create(ts,NULL,(void*)client_handler, NULL);
		
		

	}
	close(server_socket);


	//pthread_join(*ts, NULL);
	return 0;
}











































