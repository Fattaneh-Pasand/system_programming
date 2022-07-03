#include<stdio.h>
#include<unistd.h>
#include<semaphore.h>
#include<pthread.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
#include<fcntl.h>
#include<sys/types.h>

#define N 3
#define size 10
#define max_size 25

sem_t *sem;
sem_t empty;
sem_t full;
pthread_mutex_t mutex;
pthread_mutex_t mutexmain;
typedef struct{
    char name[20];
    int thread_num;
}game;
int money;
int fptr;
char board[size][size];
bool find=false;
int spot_x;
int spot_y;
///work thread
void *work(void *arg){
    game my_work=*(game *)arg;
    
   printf("thread_%d:I am %s and ready to start\n",my_work.thread_num,my_work.name);

    while(money<100){
    if(money==100 || find){
        break;
    }
    sem_wait(&empty);
    pthread_mutex_lock(&mutexmain);
    money=+10;
    pthread_mutex_unlock(&mutexmain);
    sem_post(&full);
    sleep(1);
}
}




///guybrush function 0
void *guybrush(void* arg){
game my_name=*(game*)arg;
 int my_money=0;
int f=0;
 char final_line[2 * max_size];
 char line[max_size];

 printf("thread_%d:I am %s and ready to start\n",my_name.thread_num,my_name.name);
 while(my_money<100){
    sem_wait(&full);
    if(my_money==100){
        printf("%s :I have required money and can buy map\n",my_name.name);
        break;
    }
    pthread_mutex_lock(&mutexmain);
    my_money+=money;
    printf("the amount of money that I have is %d\n",my_money);
    pthread_mutex_unlock(&mutexmain);
    sem_post(&empty);
 }
 sem_post(&sem[my_name.thread_num]);
 printf("%s:I am waiting the SVEN to send the map\n",my_name.name);
 sem_wait(&sem[my_name.thread_num+1]);
 pthread_mutex_lock(&mutex);
fptr=open("map.txt",O_RDONLY);
if(fptr==-1){
    printf("ERROR while opening the file \n");

}
char my_board[size][size];
char buff;
int i=0;
int j=0;
while(read(fptr,&buff,1)!=0){
    if(buff !='\n'){
        if(buff !=' '){
            my_board[i][j]=buff;
            j++ ;
            }
        
    }
    else{
        j=0;
        i++;

    }
}
for(i=0 ; i<size ;i++){
        printf("\n");
        for(j=0 ;j<size ;j++){
            printf("%c ",my_board[i][j]);
        }
    }
    close(fptr);
bool selected_point[size][size]= {false};
 while(!find){
    if(find)
    {
    
        printf("Break is requested \n");
        break;
    }
    int my_point_x=rand()%size;
    int my_point_y=rand()%size;
    while(selected_point[my_point_x][my_point_y]){
        my_point_x=rand()%size;
        my_point_y=rand()%size;
       
    }
    printf(" \n Selected point is %d, %d \n", my_point_x, my_point_y);
    selected_point[my_point_x][my_point_y] = true;
    if(my_board[my_point_x][my_point_y]=='X'){
        printf("The treasure has been found and the search is over.\n");
        for(i=0 ; i<size ;i++){
        printf("\n");
        for(j=0 ;j<size ;j++){
            printf("%c ",my_board[i][j]);
        }
    }
        find=true;
    }if(my_board[my_point_x][my_point_y]!='X'){
        my_board[my_point_x][my_point_y]='O';
        printf("treasure was not found,that was hole\n");
        fptr=open("map.txt",O_WRONLY |O_CREAT|F_EXLCK,S_IRUSR|S_IWUSR);
if(fptr== -1){
    printf("there was an error while opening the file.closing the program...\n");
    exit(EXIT_FAILURE);
}
    printf("start writing to file! \n");
for(i=0 ;i< size ;i++){
    for(j=0 ;j<size ;j++){
        line[f]=my_board[i][j];
        f++;
        line[f]=' ';
        f++;
    }
    line[f]='\0';
    sprintf(final_line,"%s\n",line);
    write(fptr,final_line,strlen(final_line));
    f=0;

}
close(fptr);
    }
 pthread_mutex_unlock(&mutex);
 //pthread_exit(NULL);
 }

}
///sven function
void *sven(void *arg){
    game my_name=*(game *)arg;
    int i,j;
    sem_wait(&sem[my_name.thread_num-1]);
    printf("thread_%d:I am %s and ready to start\n",my_name.thread_num,my_name.name);
    pthread_mutex_lock(&mutex);
    printf("%s :I get the required money to sell the map\n",my_name.name);
    spot_x=rand()%size;
    spot_y=rand()%size;
    printf("my spot is %d,%d \n",spot_x,spot_y);
    for(i=0 ; i<size ;i++){
        for(j=0 ;j<size ;j++){
            board[i][j]='H';
        
        }
    }
    board[spot_x][spot_y]='X';
    for(i=0 ; i<size ;i++){
        printf("\n");
        for(j=0 ;j<size ;j++){
            printf("%c ",board[i][j]);
        }
    }
    printf("\n");
 char buf;
 int f=0;
 char final_line[2 * max_size];
 char line[max_size];
 printf("start opening the file! \n");
 // write into the file
fptr=open("map.txt",O_WRONLY |O_CREAT|F_EXLCK,S_IRUSR|S_IWUSR);
if(fptr== -1){
    printf("there was an error while opening the file.closing the program...\n");
    exit(EXIT_FAILURE);
}
    printf("start writing to file! \n");
for(i=0 ;i< size ;i++){
    for(j=0 ;j<size ;j++){
        line[f]=board[i][j];
        f++;
        line[f]=' ';
        f++;
    }
    line[f]='\0';
    sprintf(final_line,"%s\n",line);
    write(fptr,final_line,strlen(final_line));
    f=0;

}
close(fptr);
pthread_mutex_unlock(&mutex);
printf("%s :I sent the map of treasure to guybrush\n",my_name.name);
sem_post(&sem[my_name.thread_num]);
printf("%s is dead\n",my_name.name);
pthread_exit(NULL);

}


////main
int main(int argc ,char*argv[]){
pthread_t *th;
pthread_mutex_init(&mutex,NULL);
pthread_mutex_init(&mutexmain,NULL);
game *player;
int i;
//time_t t;
srand(time(NULL));
player=(game *)malloc(N *sizeof(game));
sem=(sem_t*)malloc(N *sizeof(sem_t));
th=(pthread_t*)malloc(N *sizeof(pthread_t));

//guybrush
strcpy(player[0].name,"guybresh");
player[0].thread_num=0;
//sven
strcpy(player[1].name,"sven");
player[1].thread_num=1;
//work
strcpy(player[2].name,"work");
player[2].thread_num=2;

for(i=0 ;i<2 ;i++){
    sem_init(&sem[i],0,0);
}
sem_init(&empty,0,1);
sem_init(&full,0,0);

//creating the sven and guybrush thraed
pthread_create(&th[0],NULL ,&guybrush ,&player[0]);
pthread_create(&th[1],NULL ,&sven,&player[1]);
pthread_create(&th[2],NULL,&work,&player[2]);



/*while(money<100){
    if(money==100 || find){
        break;
    }
    sem_wait(&empty);
    pthread_mutex_lock(&mutexmain);
    money=+10;
    pthread_mutex_unlock(&mutexmain);
    sem_post(&full);
    sleep(1);
}*/
for(i=0;i<N;i++){
    int join = pthread_join(th[i],NULL);
    if(join != 0){
        printf("Error join\n");
            }

}
sem_destroy(sem);
sem_destroy(&empty);
sem_destroy(&full);
pthread_mutex_destroy(&mutex);
pthread_mutex_destroy(&mutexmain);
free(th);
free(player);
printf("\nSven spot was %d, %d \n", spot_x, spot_y);

return 0;

}
