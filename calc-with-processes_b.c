/*
Смотри все описание в программе А: calc-with-processes_a.c
*/

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
	key_t key = 123; /* IPC ключ общей памяти */
    
    int task[2];

    int semid; /* IPC-дескриптор для массива IPC семафоров */
    key_t keySem = 124; /* IPC ключ семафоров */
    struct sembuf mybuf; /* Структура для задания операции над семафором */

/* Пытаемся создать разделяемую память для сгенерированного ключа, 
          т.е. если для этого ключа она уже существует, системный вызов вернет
          отрицательное значение. Размер памяти определяем как размер массива из трех целых 
          переменных, права доступа 0666 - чтение и запись разрешены для всех */
	if ((shmid = shmget(key, 2 * sizeof(int), 0666 | IPC_CREAT | IPC_EXCL)) < 0) {
	/*В случае ошибки пытаемся определить возникла ли она из-за того, что 
                  сегмент разделяемой памяти уже существует или по другой причине */
		if(errno != EEXIST){
			/* По другой причине...*/
			printf("Can't create shared memory\n");
			exit(-1);
		} else {
			/* Если из-за того, что разделяемая память уже существует, то пытаемся получить 
                          её IPC дескриптор и, в случае удачи, сбрасываем флаг необходимости
                          инициализации элементов массива
                          */
			if((shmid = shmget(key, 2 * sizeof(int), 0)) < 0) {
				printf("Can't find shared memory\n");
				exit(-1);
			}
			is_new = 0;
		}
	}
	
	    /*
          Пытаемся отобразить разделяемую память в адресное пространство текущего
          процесса. Для правильного сравнения мы явно приводим -1 к
          указателю на целое число
          */
	if ((array = (int *)shmat(shmid, NULL, 0)) == (int *)(-1)) {
		printf("Can't attach shared memory\n");
		exit(-1);
	}
	/* Пытаемся получить доступ по ключу к массиву семафоров, если он существует,
       или создать его из одного семафора, если его ещё не существует, с правами
       доступа rw для всех пользователей */

    if ((semid = semget(keySem , 2, 0666 | IPC_CREAT)) < 0) {
        printf("Can`t get semid\n");
        exit(-1);
    }
 
	//семафорим программе А, что мы запустились, поднимая её семафор на единицу   
    mybuf.sem_op = 1;
    mybuf.sem_flg = 0;
    mybuf.sem_num = 1;
    
    if (semop(semid , &mybuf , 1) < 0) {
        printf("Can`t wait for condition\n");
        exit(-1);
    }
    
    printf("Waiting for another process...\n");
    	//ждём программу А, пока она не поднимет наш светофор на единицу (что будет означать, что она тоже запустилась)
    mybuf.sem_op = -1;
    mybuf.sem_flg = 0;
    mybuf.sem_num = 0;
    
    if (semop(semid , &mybuf , 1) < 0) {
        printf("Can`t wait for condition\n");
        exit(-1);
    }
    
	printf("Counting...\n");
    
    array[1] = array[0] * array[0];
    
	//семафорим программе А, что мы досчитали и она может забирать число из общей памяти
    mybuf.sem_op = 1;
    mybuf.sem_flg = 0;
    mybuf.sem_num = 1;
    
    if (semop(semid , &mybuf , 1) < 0) {
        printf("Can`t wait for condition\n");
        exit(-1);
    }
	//останавливаемся и ждём сигнала от программы А, что она прочитала число из общей памяи,и общую память можно зачистить
    mybuf.sem_op = -1;
    mybuf.sem_flg = 0;
    mybuf.sem_num = 0;

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
