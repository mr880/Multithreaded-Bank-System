
int main(int argc, char* argv[])
{

	// struct addrinfo hints, *servinfo, *p;
 //    int rv;
 //    memset(&hints, 0, sizeof hints);
 //    hints.ai_family = AF_INET; // use AF_INET6 to force IPv6
 //    hints.ai_socktype = SOCK_STREAM;

 //    if ((rv = getaddrinfo(host, argv[6], &hints, &servinfo)) != 0) 
 //    {
 //        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
 //        exit(1);
 //    }

 //    for(p = servinfo; p != NULL; p = p->ai_next) 
 //    {
 //        if ((sockfd = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == -1) 
 //        {
 //            perror("socket");
 //            continue;
 //        }

 //        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) 
 //        {
 //            perror("connect");
 //            close(sockfd);
 //            continue;
 //        }

 //        break; 
 //    }
 //    if (p == NULL) 
 //    {
 //        fprintf(stderr, "failed to connect\n");
 //        exit(2);
 //    } 
 //    else
 //    {

	// 	printf("connection successful\n");

	// }                                                                       

	// int my_id = -1;
	// read(sockfd, &my_id , 4);
	




	return 0;
}