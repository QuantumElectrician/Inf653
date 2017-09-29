#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

char s[200];
char c = '\0';
int ex = 2;
int k, PID = 0;
int pointer = 1;


void zero()
{
	printf("Okay, received 0\n");
	if (k == 8)
	{
		s[pointer] = c;
	       	printf("OK. I write c = %d as s[%d]\n", c, pointer);
		if (c =='\0')
		{
			ex = 0;
		}
		k = 1;
		pointer++;
		c = 0;
	}
	else
	{
		k++;
	}
	
	printf("%d\n", c);

	if (ex != 0)
	{	
	kill(PID, 10);
	}
}

void one()
{
	printf("Okay, received 1\n");
	int i = 0;
	int st = 1;

        if (k == 8)
        {
		s[pointer] = c;
		printf("OK. I write c = %d as s[%d]\n", c, pointer);
                k = 1;
                pointer++;
                c = 1;
        }
        else
        {
		st = 1;
		if (k == 0)
		{
			st = 1;
		}
		else
		{
			for (i = 0; i < k; i++)
			{
				st = 2*st;
			}
		}
		c = c + st;
		k++;
        }

	kill(PID, 10);
	printf("%d\n", c);
}




int main() {
        //receiver
        int MyPID = getpid();
        int code;
	printf("First print PID and string in sender\n");
        printf("My PID = %d\n",MyPID);
        printf("Enter sender PID: ");
        scanf("%d", &PID);

        signal(10, zero);
	signal(12, one);
       
	kill(PID, 12);

	while (ex != 0)
	{
		
	}
	
	printf("Execution code\n\n\n");
	code = 1;
	while (s[code] != '\0')
	{
		printf("%c", s[code]);
		code++;
	}

        printf("\n\n");
        return(0);
}

