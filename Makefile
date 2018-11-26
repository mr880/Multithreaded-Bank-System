default:  bankingServer.c
	gcc -o bankingServer bankingServer.c -pthread
clean:
	$(RM) bankinServer *.o *~