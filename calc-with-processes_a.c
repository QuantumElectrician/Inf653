/*Иллюстрация работы с разделяемой памятью*/

/*
	Организуем разделяемую память для массива из трех
	целых чисел. Первый элемент массива является счетчиком
	числа запусков программы 1, т.е. данной программы,
	второй элемент массива - счетчиком числа запусков
	программы 2, третий элемент массива - счетчиком
	числа запусков обеих программ 
*/
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

int main() {
	int  *array; /* Указатель на разделяемую память */
	int shmid; /* IPC дескриптор для области разделяемой памяти */
	int is_new = 1; /* Флаг необходимости инициализации элементов массива */
	key_t key = 123; /* IPC ключ, ipcrm удалит по ключу */
	
    int task[2];
    
    int semid; /* IPC-дескриптор для массива IPC семафоров */
    key_t keySem = 124; /* IPC ключ */
    struct sembuf mybuf; /* Структура для задания операции над семафором */

    printf("Enter a number:");
	scanf("%d", task);

	/* Пытаемся создать разделяемую память для сгенерированного ключа, 
	т.е. если для этого ключа она уже существует, системный вызов вернет
	отрицательное значение. Размер памяти определяем как размер массива из трех целых 
	переменных, права доступа 0666 - чтение и запись разрешены для всех */
	if ((shmid = shmget(key, 2 * sizeof(int), 0666 | IPC_CREAT | IPC_EXCL)) < 0) {
		/*В случае ошибки пытаемся определить возникла ли она из-за того, что 
		сегмент разделяемой памяти уже существует или по другой причине */
		if(errno != EEXIST) {
			/* По другой причине...*/
			printf("Can't create shared memory\n");
			exit(-1);
		} else {
			/* Если из-за того, что разделяемая память уже существует, то пытаемся получить 
			её IPC дескриптор и, в случае удачи, сбрасываем флаг необходимости
			инициализации элементов массива
			*/
			if((shmid = shmget(key, 2 * sizeof(int), 0)) < 0){
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
    
    printf("Waiting for other process...\n");
    
    array[0] = task[0];

    /* Выполним операцию D(semid , 1) для нашего массива семафоров. Для этого
     сначала заполним структуру mybuf */
    mybuf.sem_op = 1;
    mybuf.sem_flg = 0;
    mybuf.sem_num = 0;
    
    if (semop(semid , &mybuf , 1) < 0) {
        printf("Can`t wait for condition\n");
        exit(-1);
    }
    
    mybuf.sem_op = -1;
    mybuf.sem_flg = 0;
    mybuf.sem_num = 1;
	
    if (semop(semid , &mybuf , 1) < 0) {
        printf("Can`t wait for condition\n");
        exit(-1);
    }
    
    mybuf.sem_op = -1;
    mybuf.sem_flg = 0;
    mybuf.sem_num = 1;
    
    if (semop(semid , &mybuf , 1) < 0) {
        printf("Can`t wait for condition\n");
        exit(-1);
    }
    
    printf("Result is %d\n", array[1]);
	
	/*
	Удаляем разделяемую память из адресного пространства текущего
	процесса
	*/
	if (shmdt(array) < 0) {
		printf("Can't detach shared memory\n");
		exit(-1);
	}

	return 0;
}
