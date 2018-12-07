#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
#include<ctype.h>
#include<math.h>
#include <sys/types.h>
#include<sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include<pthread.h>
#include <errno.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>



int listencount = 20;
int listencount2 = 0;

int first_call = 0;
static int PAUSE = 0; 


struct Account{

	char name[255];
	double balance;
	int inSession;
	struct Account* next;

};

struct socket{

	int fd;
	struct socket* next;

};

struct socket* sockhead = NULL;



void* client_handler(void*);
void* outputFromServer();
void print_accounts();
void* set_alarm();
void* server_handler(void*);
void *get_in_addr(struct sockaddr*);
void sigchld_handler(int);
void exit_func();
void disconnected();
int add_account(char*);
void deposit(double, char*);
double withdraw(double, char*);
double get_current_balance(char*);
int find_account_by_name(char*);
int make_active(char*);
int make_inactive(char*);
char* trim_newline(char*);
void* send_user_commands(void*);






