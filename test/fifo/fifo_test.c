#include "fifo.h"
#include <stdio.h>

fifo_t fifo1;
uint8_t bsize =5;
uint8_t buffer[5];
char storage;
char *storages;

void emptycheck(fifo_t* fifo) {
if(fifo_is_empty(fifo)) {
printf("empty\n");
}
else{
printf("not empty\n");
}
}

void fullcheck(fifo_t* fifo) {
if(fifo_is_full(fifo)) {
printf("full\n");
}
else{
printf("not full\n");
}
}

void fillit(fifo_t* fifo) {
printf("fillit_start |");
fifo_write(&fifo1, 'a');
fifo_write(&fifo1, 'b');
fifo_write(&fifo1, 'c');
fifo_write(&fifo1, 'd');
fifo_write(&fifo1, 'e');
printf("fillit_end\n");
 }

int main(void){


fifo_init(&fifo1, buffer, bsize);


printf("-----Test_1_full_empty-----\n");
//empty

emptycheck(&fifo1);
fullcheck(&fifo1);

printf("write a\n");
fifo_write(&fifo1, 'a');
emptycheck(&fifo1);
fullcheck(&fifo1);

printf("write b\n");
fifo_write(&fifo1, 'b');
emptycheck(&fifo1);
fullcheck(&fifo1);

printf("write c\n");
fifo_write(&fifo1, 'c');
emptycheck(&fifo1);
fullcheck(&fifo1);

printf("write d\n");
fifo_write(&fifo1, 'd');
emptycheck(&fifo1);
fullcheck(&fifo1);

printf("write e\n");
fifo_write(&fifo1, 'e');
emptycheck(&fifo1);
fullcheck(&fifo1);

//full

fifo_read(&fifo1, &storage);
printf("%c\n",storage);
emptycheck(&fifo1);
fullcheck(&fifo1);

fifo_read(&fifo1, &storage);
printf("%c\n",storage);
emptycheck(&fifo1);
fullcheck(&fifo1);

fifo_read(&fifo1, &storage);
printf("%c\n",storage);
emptycheck(&fifo1);
fullcheck(&fifo1);

fifo_read(&fifo1, &storage);
printf("%c\n",storage);
emptycheck(&fifo1);
fullcheck(&fifo1);

fifo_read(&fifo1, &storage);
printf("%c\n",storage);
emptycheck(&fifo1);
fullcheck(&fifo1);


printf("-----Test_2_clear-----\n");
//empty
emptycheck(&fifo1);
fullcheck(&fifo1);

fillit(&fifo1);
//full

fifo_clear(&fifo1);

emptycheck(&fifo1);
fullcheck(&fifo1);

//empty

printf("-----Test_3_cde-----\n");


fillit(&fifo1);
//full

storage = 'z';
if(fifo_cmp_pgm(&fifo1, &storage)) {
printf("found FAILURE\n");
}
else{
printf("Not found. RIGHT\n");
}

storages ="a";
if(fifo_cmp_pgm(&fifo1, storages)) {
printf("a compared and found\n");
}
else{
printf("a compared and NOT found\n");
}

storages = "b";
if(fifo_cmp_pgm(&fifo1, storages)) {
printf("b compared and found\n");
}
else{
printf("b compared and NOT found\n");
}

fifo_read(&fifo1, &storage);
printf("%c\n",storage);

fifo_read(&fifo1, &storage);
printf("%c\n",storage);

fifo_read(&fifo1, &storage);
printf("%c\n",storage);

//empty




//full
printf("-----strstr_TEST_4_de-----\n");
fillit(&fifo1);
storage = 'c';
fifo_strstr_pgm(&fifo1, &storage);

fifo_read(&fifo1, &storage);
printf("%c\n",storage);

fifo_read(&fifo1, &storage);
printf("%c\n",storage);


printf("-----TEST_END-----\n");
return 0;}
