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


static int PAUSE = 0; 

struct Account{

	char name[255];
	double balance;
	int inSession;
	struct Account* next;

};

struct socket{

	int client_socket;
	int my_num;

};

struct _tids
{
	int tid;
	struct _tids *next;
};
typedef struct _tids tids;