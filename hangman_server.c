
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>    //strlen
#include <time.h>
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <fcntl.h> //for open
#include <unistd.h> //for close and write

#define NUM_THREADS 5
void *threadId (void *id);
int getRand();
void checkGuess(char guess, char w[], char trivia[]);
int winner(char trivia[]);
char * words[15];
int n_threads;
int main(int argc, char* argv[])
{
    int socket_desc , client_sock , c , read_size;
    struct sockaddr_in server , client;
    char client_message[2000];
    char test[5];
    FILE *file;
    file = fopen("hangman_words.txt", "r");
    char * line = NULL;
    size_t len = 0;
    int wordcount=0;
    ssize_t read;
    if (file) {
        while ((read = getline(&words[wordcount], &len, file)) != -1)
            {
                words[wordcount][strlen(words[wordcount])-1] = '\0';
                wordcount++;
            }
    fclose(file);
    }
    int i=0;
    n_threads= 0;
    int id,rid;
    id=0;
    pthread_t thread[NUM_THREADS];
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Socket creation failed\n");
    }
    printf("Socket Created\n");
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_port = htons(atoi(argv[2]));
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        printf("Bind failed\n");
        return 1;
    }
    printf("Bind Done\n");
	int count=1;
	while(count>0){
            bzero(test,5);
		    listen(socket_desc , 1);
			if(n_threads <= 0){
		    printf("Waiting for incoming connections...\n");
			}
		    c = sizeof(struct sockaddr_in);
			if(n_threads <3)
			{
			    client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);

			    if (client_sock < 0)
			    {
				printf("Accept Failed\n");
				return 0;
			    }
			   	 printf("Connection Accepted\n");
                bzero(client_message,2000);
                strcpy(client_message,"Ready to start game? (y/n):");
					write(client_sock , client_message, strlen(client_message));
                    recv(client_sock , test, 5 , 0);
                   if(!strcmp(test,"n")){
                        continue;
                    }

	     			n_threads++;
				rid = pthread_create(&thread[id],NULL,&threadId,(void*)(size_t)client_sock);
                id++;
				if(rid!=0)
				{
					printf("pthread_create() failed for Thread # %d",id);
					return 0;
				}
				 if(n_threads <= 0){
					pthread_exit(NULL);
					return 0;
				}
                id--;
            }

			else
			{
				 client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
			    if (client_sock < 0)
			    {
				printf("Accept Failed\n");
				return 0;
			    }
				printf("New request is denied! Server is full\n");
				strcpy(client_message,"Server overloaded\n");
					write(client_sock , client_message, strlen(client_message));
			}

	}
    return 0;
}

int getRand()
{
  int r = rand() % 15;
  return r;
}


void checkGuess(char guess, char w[], char trivia[])
{
  int check=0;

    for(int i=0;i<strlen(w);i++)
    {
      if(w[i] == guess)
      {
        trivia[i] = w[i];
      }
    }

}

int winner(char trivia[])
{
  int check=1;
  for(int i=0;i<strlen(trivia);i++)
  {
    if(trivia[i] == '_')
      check=0;
  }
    return check;
}


void *threadId (void *id)
{

  srand(time(NULL));
	long client_sock = (long)id;
	int read_size =0;
  char client_msg[2000],  word[2000];
  char trivia[2000] ;
    memset(trivia, '\0', sizeof(trivia));
      memset(client_msg, '\0', sizeof(client_msg));

  int word_index=getRand();
  while(words[word_index]==NULL){
      word_index=getRand();
  }
  strcpy(word, words[word_index]);

  for(int i=0;i<strlen(word);i++)
  {
    trivia[i] = '_';
  }

  write(client_sock , trivia, strlen(trivia));

  int end = 0;

     printf("Game Start Client %i\n",n_threads);
      	while(end == 0){
      		if( (read_size = recv(client_sock , client_msg, 2000 , 0)) > 0 )
      		    {
                  checkGuess(client_msg[0], word, trivia);
                  if(strcmp(client_msg,"end")==0)
                    {
                      write(client_sock , word, strlen(word));
                      n_threads--;
                    	pthread_exit(NULL);
                    }

                  if(winner(trivia))
                    end=1;
            			write(client_sock , trivia, strlen(trivia));


      		    }

      	   }
             write(client_sock , word, strlen(word));


	n_threads--;
	pthread_exit(NULL);
}
