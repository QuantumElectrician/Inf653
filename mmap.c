//
//  main.c
//  Matrix
//
//  Created by Владислав Агафонов on 11.11.2017.
//  Copyright © 2017 Владислав Агафонов. All rights reserved.
//

//файл == памяти (в данном случае структура = файл)


#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define FILE_NAME "example.txt"
#define FILE_NAME1 "GOPR1664.MP4"

typedef struct
{
    int i;
    double x;
    char c[10];
} s_t;

int main(int argc, const char * argv[])
{
    int fd = open(FILE_NAME, O_RDWR | O_CREAT, 0666); //открытие файла с правами на чтение и запись и флагом на создание, если его нет
    ftruncate(fd, sizeof(s_t)); //зачистка файла с дескриптором fd и вторым аргументом новый размер файла
    
    s_t* x; //создание указателя на пользовательскую структуру (область в оперативе)
    /*
        2 аргумент - размер памяти mmap (у них одинаковый размер)
     */
    x = mmap(NULL, sizeof(s_t), PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
    
    //заполение x
    x->c[0] = 'q';
    x->c[1] = 'w';
    x->c[2] = 'e';
    
    //затем проверим файл
    //*идем в терминал смотреть файл*
    //принудительно синхронизируем структуру и файл, закрыв его
    close(fd);
    return 0;
}

