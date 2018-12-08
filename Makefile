all: hangman_client hangman_server

hangman_client: hangman_client.c
	gcc -o hangman_client hangman_client.c -lpthread
hangman_server: hangman_server.c
	gcc  hangman_server.c -o hangman_server -lpthread

clean:
	$(RM) hangman_client hangman_server
