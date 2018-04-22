#include<fcntl.h>
#include<string.h>
#include<signal.h>
#include<stdio.h>
#include<sys/sem.h>
#include<sys/ipc.h>
#include<unistd.h>
#include<stdlib.h>
#include<errno.h>
#include<sys/shm.h>
#include<sys/types.h>
#include<sys/stat.h>
extern int errno;

union semun
{
  int val;
  struct semid_ds *buf;
  ushort *array;
};

int main ()
{
  unsigned short sem_values[2] = { 1, 0 };
  char buff[100];
  union semun obj;
  int fd1,semid, ret, i, shmid;
  semid = semget (ftok ("project", 42), 2, IPC_CREAT | S_IRWXU);

  printf ("semid returned %d\n", semid);
  if (semid == -1)
    {
      printf ("Error is in creating semid \n errno = %d\n", errno);
      exit (1);
    }

  obj.array = sem_values;

  printf ("Sem1 Value= %d, Sem2 Value = %d\n", obj.array[0], obj.array[1]);


  ret = semctl (semid, 0, SETALL, obj);
  printf ("setctl returned %d\n", ret);
  if (ret == -1)
    {
      printf ("semctl errno = %d\n", errno);
      exit(1);
    }



  ret = semctl (semid, 0, GETALL, obj);
  printf ("setctl returned %d\n", ret);
  if (ret == -1)
    {
      printf ("semctl errno = %d\n", errno);
      exit (1);
    }
printf("now");
//  printf ("Sem1 Value= %d, Sem2 Value = %d\n", obj.array[0], obj.array[1]);
printf("reached");


key_t k = ftok ("shared", 42);
printf("reached here");
if (k == -1)
  {
    printf ("key_t Errno = %d\n", errno);

    exit (1);
  }

    printf ("Reached 1");
shmid = shmget (k, 100,IPC_CREAT|S_IRWXU);

if (shmid == -1)
  {
    printf ("shmid Errno = %d\n", errno);
    exit (1);
  }

    printf ("Reached 2");
void *attach_addr = shmat(shmid,0,0);

if ((int) attach_addr == -1)
  {
    printf ("attach Errno = %d\n", errno);
    exit (1);
  }

    printf ("Reached 3");
struct sembuf sembuf_array[2];
semid = semget (ftok ("project", 42), 0, 0);
if (semid == -1)
  {
    printf ("semget errno = %d\n", errno);
    exit (1);
  }

void errorhandler()
{
if(shmdt(attach_addr)<0)
{
perror("Error in shmdt");
exit(6);
}
exit(0);

}

fd1=open(("new_file1"),O_CREAT|O_WRONLY,S_IRWXU);
if(fd1<0)
{
perror("Error in creating and opeaning file");
exit(8);
}
//memset(attach_addr,0,100);
sprintf((char *)attach_addr,"%d",getpid());

while(1)
   { /* wait on s1 */
    memset (&sembuf_array, 0, sizeof (sembuf_array));
    sembuf_array[0].sem_num = 1;
    sembuf_array[0].sem_op = -1;
    int ret = semop (semid, sembuf_array, 1);
    if (ret == -1)
      {
        printf ("semop errno = %d\n", errno);
        exit (1);
      }

memset(buff,0,100);
    strcpy(buff,(char *)attach_addr);
    ret=write(fd1,buff,sizeof(buff));
memset(attach_addr,0,100);

    /* signal on s0 */
    memset (&sembuf_array, 0, sizeof (sembuf_array));
    sembuf_array[0].sem_num = 0;
    sembuf_array[0].sem_op = 1;
    ret = semop (semid, sembuf_array, 1);
    if (ret == -1)
      {
        printf ("semop errno = %d\n", errno);
        exit (1);
      }
      signal(10,errorhandler);

  }


return 0;
}