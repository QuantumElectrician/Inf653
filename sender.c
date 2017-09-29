#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int ack = 0;
char s[200];
int PID, code;
int ex = 2;


void start()
{
	 int i, k = 0;
	 printf("Connected\n");
        int lenght = strlen(s);
        for (i = 0; i <= lenght; i++)
        {
                while (s[i] != '\0')
                {
                        k++;
                        code = s[i] % 2;
                        while (ack != 0)
                        {
			}
                        if (code == 0)
                        {
                                kill (PID, 10);
				printf("Send signal 0\n");
                        }
                        else
                        {
                                kill(PID, 12);
				printf("Send signal 1\n");
                        }
                        ack = 1;
               
                        s[i] = (s[i] - code)/2;
                }
        if (k == 7)
	{
		kill(PID, 10);
		printf("Send signal 0\n");
	}
        k = 0;
        printf("  \n");
        }
	printf("Execution code\n\n");
	ex = 0;
}


void back()
{
	ack--;
}

int main() {
	//sender
	
	signal(10, back);
        signal(12, start);
	
	printf("First print PID and string here\n");

	int MyPID = getpid();
	printf("My PID = %d\n",MyPID);
	printf("Enter reciever PID: ");
	scanf("%d", &PID);
	printf("Enter string to push: ");
        scanf("%s", &s);
	
	printf("I'm waiting PID in receiver\n");
	
	while (ex != 0)
	{

	}
	
	for(ex = 0; ex <= 8; ex++ )
	{
		while (ack != 0)
		{
		}
		ack = 1;
		kill(PID, 10);
		printf("Send signal 0 to finish\n");
	}
	printf("\n\n\n");
	return(0);
}

