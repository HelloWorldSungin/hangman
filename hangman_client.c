
#include <ctype.h>
#include <stdio.h> //printf
#include <string.h>    //strlen
#include <sys/socket.h>    //socket
#include <arpa/inet.h> //inet_addr
#include <stdlib.h> //atoi
#include <fcntl.h> //for open
#include <unistd.h> //for close

int checkGuess(char g, char trivia[]);
int winner(char trivia[]);

int main(int argc, char* argv[])
{
    int sock;
    struct sockaddr_in server;
    char message[1000] , server_reply[2000], incorrect_guesses[50], guess[50], trivia[50];
    memset(incorrect_guesses, '\0', sizeof(incorrect_guesses));
    bzero(message,1000);
    bzero(server_reply,2000);
    bzero(incorrect_guesses,50);
    bzero(guess,50);
    bzero(trivia,50);
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        printf("Could not create socket\n");
    }
    printf("Socket created\n");
    server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[2]));
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        printf("Connect failed. Error\n");
        return 1;
    }
	if( recv(sock , server_reply , 2000 , 0) < 0)
		    {
			 puts("recv failed\n");
		    }

		    printf("%s\n",server_reply);
  if(strcmp(server_reply,"Server overloaded\n") == 0)
  {
    close(sock);
    return 0;
  }
  fgets(server_reply, sizeof(server_reply), stdin);
  if (server_reply[strlen(server_reply) - 1] == '\n'){
    server_reply[strlen(server_reply) - 1] = '\0';
  }
    send(sock ,server_reply , strlen(server_reply),0);
if(strcmp(server_reply,"y") == 0){
    int count = 7;
  	while(count > 0 )
    {
            if( recv(sock , trivia , 2000 , 0) < 0)
           {
          puts("recv failed\n");
           }
           if(winner(trivia)){
              close(sock);
              break;
           }
           if(!checkGuess(guess[0],trivia)){
              strcat(incorrect_guesses,guess);
		          count--;
           }
           printf("%s\n",trivia);
  		    printf("Incorrect Guesses: ");
            printf("%s\n\n",incorrect_guesses);
          printf("Letter to Guess: ");
          fgets(guess, sizeof(server_reply), stdin);
          if (guess[strlen(guess) - 1] == '\n'){
            guess[strlen(guess) - 1] = '\0';
          }
          for(int i=0;i<strlen(guess);i++){
            guess[i]=tolower(guess[i]);
          }
          int wrong = 0;
        while(wrong == 0)
        {
          if(strlen(guess)==1 && ((guess[0]>=65 && guess[0]<=90) || (guess[0]>=97 && guess[0]<=122)) )
          {
      		    if( send(sock , guess , strlen(guess) , 0) < 0)
      		    {
          			 printf("Send failed\n");
          			 return 1;
      		    }
              wrong = 1;
          }
          else
          {
              printf("Error! Please guess one letter.\n");
              printf("Letter to Guess: ");
              fgets(guess, sizeof(guess), stdin);
              if (guess[strlen(guess) - 1] == '\n'){
                guess[strlen(guess) - 1] = '\0';
              }
              for(int i=0;i<strlen(guess);i++){
            guess[i]=tolower(guess[i]);
            }
          }
        }
       }
      bzero(guess,50);
      recv(sock , guess , 2000 , 0);
       if(winner(trivia)){
         printf("The Word: %s\n",trivia);
       printf("You Win!\nGame Over!\n");
     }
       else if(count==0)
       {
           printf("You Lose :(\nGame Over!\n");
           strcpy(guess,"end");
           if( send(sock , guess , strlen(guess) , 0) < 0)
           {
              printf("Send failed\n");
              return 1;
           }

           if( recv(sock , guess , 2000 , 0) < 0)
          {
              puts("recv failed\n");
          }
          printf("Correct Word was: %s\n",guess);
        }
   }
    close(sock);
    return 0;
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


int checkGuess(char g, char trivia[])
{
  int check=0;
  for(int i=0;i<strlen(trivia);i++)
  {
    if(trivia[i] == g)
      check=1;
  }
    return check;
}
