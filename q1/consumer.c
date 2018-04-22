#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/ipc.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <unistd.h>

int flag=0;
int *at_adr;

void myhandler()
{
        printf("HI HI\n");
        flag=1;
        if(shmdt(at_adr)<0)
        {
                perror("shmdt failed");
                exit(10);
        }
        //return;
        exit(0);
}

int main(int argc, char * argv[])
{
        key_t k=ftok("myfile",11);
        printf("cons pid = %d\n",getpid());
        printf("Key is: %d\n",k);
        if(k<0)
        {
                perror("ftok failed");
                exit(1);
        }

        int shmid=shmget(k,100,IPC_CREAT|S_IRWXU);
        printf("Shmid is: %d\n",shmid);
        if(shmid<0)
        {
                perror("shmget failed");
                exit(2);
        }

        at_adr=shmat(shmid,0,0);
        /*if((int)at_adr<0)
        {
                perror("shmat failed");
                exit(3);
        }*/

        int semid=semget(k,2,IPC_CREAT|S_IRWXU);
        printf("Semid is: %d\n",semid);
        if(semid<0)
        {
                perror("semget failed");
                exit(4);
        }

        int fd=open("myfile_new",O_CREAT|O_WRONLY,S_IRWXU);
        if(fd<0)
        {
                perror("open failed");
                exit(5);
        }



        memset(at_adr,0,100);
        sprintf((char *)at_adr,"%d",getpid());


        int ret;
        char buf[100];
        struct sembuf sembuf_array[1];
        while(flag==0)
        {
                memset(&sembuf_array,0,sizeof(sembuf_array));
                sembuf_array[0].sem_num=1;
                sembuf_array[0].sem_op=-1;
                int x=semop(semid,sembuf_array,1);
                if(x<0)
                {
                        perror("semop 6 failed");
                        exit(6);
                }

                char buf[100];
                //read(at_adr,buf,100);                 //creates myfile_new in binary form
                strcpy(buf,(char *)at_adr);
                //printf("%s\n",(char *)at_adr);
                //printf("from buf %s\n",buf);
                ret=write(fd,buf,sizeof(buf));
                if(ret<0)
                {
                        perror("write failed");
                        exit(7);
                }

                //char pid[10]={'0'};
        //      memset(at_adr,0,100);
        //      sprintf((char *)at_adr,"%d",getpid());
                //strcpy(at_adr,pid);

                memset(&sembuf_array,0,sizeof(sembuf_array));
                sembuf_array[0].sem_num=0;
                sembuf_array[0].sem_op=1;
                x=semop(semid,sembuf_array,1);
                if(x<0)
                {
                        perror("semop 8 failed");
                        exit(8);
                }

                signal(10,myhandler);
        }

        if(shmdt(at_adr)<0)
        {
                perror("shmdt failed");
                exit(10);
        }

        return 0;
}