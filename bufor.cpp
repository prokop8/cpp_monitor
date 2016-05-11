#include "monitor.h"
#include <pthread.h>
#include <chrono>
#include <time.h>
#include <thread>
#include <iostream>

class Bufor : public Monitor {
Condition full, empty;
int *bufor;
int rozmiarBuf;  
int max;
public:
void get(void *);
void set(void *);
Bufor(int n){
rozmiarBuf=0;
max=n;
bufor = new int[n]{0};
}
};

void Bufor::set(void* nr){
int x=1;
enter();  //wejscie do sekcji krytycznej
if(rozmiarBuf==max){ 
std::cout<<"----------------------BUFOR PELNY!!!-------------------"<<std::endl;
wait(full);
}
bufor[rozmiarBuf] = x;
++rozmiarBuf;
std::cout<<"Producent wyprodukowal,                            w buforze "<<rozmiarBuf<<" el."<<std::endl;
if(rozmiarBuf==1) signal(empty);
leave();  //wyjscie do sekcji krytycznej
}
void Bufor::get(void* nr){
int tmp;
enter(); //wejscie do sekcji krytycznej
if(rozmiarBuf==0) {
std::cout<<"----------------------BUFOR PUSTY!!!-------------------"<<std::endl;
wait(empty);
}
tmp=bufor[rozmiarBuf] ;
bufor[rozmiarBuf]=0;
--rozmiarBuf;
std::cout<<"Konsument zabral,                                  w buforze "<<rozmiarBuf<<" el."<<std::endl;
if(rozmiarBuf==max-1) signal(full);
leave();  //wyjscie do sekcji krytycznej
}
Bufor bufor(4);
void* producent(void* nr){
srand(time(NULL));
while(true){
//sleep(rand()%10+1);
int a=rand()%1000+1;
std::this_thread::sleep_for(std::chrono::milliseconds(a));
bufor.set(nr);
}
};

void* consument(void* nr){
srand(time(NULL));
while(true){
//sleep(rand()%10+1);
int a=rand()%1000+1;
std::this_thread::sleep_for(std::chrono::milliseconds(a));
bufor.get(nr);
}
};

int main(int argc, char *argv[]){

if( argc != 3 ) {
printf("Prosze podac najpierw dlugosc buforu potem ilosc producentow i potem ilosc konsumentow");
exit(0);
}
int prod=atoi(argv[1]);
int kons=atoi(argv[2]);
pthread_t t_id[kons+prod];
for(int i=0;i<prod;i++){
pthread_create(&(t_id[i]),NULL,producent,(void*)i+1);
printf("p: %d\n", i);
}
for(int i=prod;i<kons+prod;i++){
pthread_create(&(t_id[i]),NULL,consument,(void*)i-prod+1);
printf("k: %d\n", i);		
}
for ( int i = 0; i < kons+prod; i++){
pthread_join(t_id[i],(void **)NULL);
}
return 0;	
}

