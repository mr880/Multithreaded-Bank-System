default:  bankingServer.c
	gcc -o server bankingServer.c -pthread
	gcc -o client bankingClient.c -pthread
clean:
	$(RM) bankinServer *.o *~