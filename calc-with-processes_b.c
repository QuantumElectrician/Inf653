#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/sem.h>

int main()
{
	int *array;
	int shmid;
	int is_new = 1;
	key_t key = 123; /* IPC ключ */
    
    int task[2];

    int semid; /* IPC-дескриптор для массива IPC семафоров */
    key_t keySem = 124; /* IPC ключ */
    struct sembuf mybuf; /* Структура для задания операции над семафором */

	if ((shmid = shmget(key, 2 * sizeof(int), 0666 | IPC_CREAT | IPC_EXCL)) < 0) {
		if(errno != EEXIST){
			printf("Can't create shared memory\n");
			exit(-1);
		} else {
			if((shmid = shmget(key, 2 * sizeof(int), 0)) < 0) {
				printf("Can't find shared memory\n");
				exit(-1);
			}
			is_new = 0;
		}
	}

	if ((array = (int *)shmat(shmid, NULL, 0)) == (int *)(-1)) {
		printf("Can't attach shared memory\n");
		exit(-1);
	}

    if ((semid = semget(keySem , 2, 0666 | IPC_CREAT)) < 0) {
        printf("Can`t get semid\n");
        exit(-1);
    }
    
    mybuf.sem_op = 1;
    mybuf.sem_flg = 0;
    mybuf.sem_num = 1;
    
    if (semop(semid , &mybuf , 1) < 0) {
        printf("Can`t wait for condition\n");
        exit(-1);
    }
    
    printf("Waiting for another process...\n");
    
    mybuf.sem_op = -1;
    mybuf.sem_flg = 0;
    mybuf.sem_num = 0;
    
    if (semop(semid , &mybuf , 1) < 0) {
        printf("Can`t wait for condition\n");
        exit(-1);
    }
    
	printf("Counting...\n");
    
    array[1] = array[0] * array[0];
    
    mybuf.sem_op = 1;
    mybuf.sem_flg = 0;
    mybuf.sem_num = 1;
    
    if (semop(semid , &mybuf , 1) < 0) {
        printf("Can`t wait for condition\n");
        exit(-1);
    }
	
    printf("Now you will find the result in the first process\n");
	
	if(shmdt(array) < 0){
		printf("Can't detach shared memory\n");
		exit(-1);
	}
	system("ipcrm -M 123");
    system("ipcrm -S 124");
	return 0;
}
