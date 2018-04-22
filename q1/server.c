#include<fcntl.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/sem.h>
#include<sys/shm.h>
#include<errno.h>
#include<string.h>
extern int errno;

union semun
{
  int val;
  struct semid_ds *buf;
  ushort *array;
};

int
main ()
{
  unsigned short sem_values[2] = { 1, 0 };
  union semun obj;
  int consumer_pid, semid, i, ret, shmid, fd1;
  semid = semget (ftok ("q1", 42), 2, IPC_CREAT | S_IRWXU);

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
      exit (1);
    }



  ret = semctl (semid, 0, GETALL, obj);
  printf ("setctl returned %d\n", ret);
  if (ret == -1)
    {
      printf ("semctl errno = %d\n", errno);
      exit (1);
    }

  printf ("Sem1 Value= %d, Sem2 Value = %d\n", obj.array[0], obj.array[1]);


  key_t k = ftok ("shared", 42);

  if (k == -1)
    {
      printf ("Error is in ftok         Errno = %d\n", errno);
      exit (1);
    }

  shmid = shmget (k, 100, IPC_CREAT | S_IRWXU);

  if (shmid == -1)
    {
      printf ("2 Errno = %d\n", errno);
      exit (1);
    }

  printf ("Shared id %d\n", shmid);
  void *attach_addr = shmat (shmid, 0, 0);
//memset(&attach_addr,0,sizeof(attach_addr));
  if ((int) attach_addr == -1)
    {
      printf ("3 Errno = %d\n", errno);
      exit (1);
    }
  struct sembuf sembuf_array[2];
  semid = semget (ftok ("project", 42), 0, 0);

  if (semid == -1)
    {
      printf ("semget errno = %d\n", errno);
      exit (1);
    }

  fd1 = open ("new_file1", O_RDONLY);
  if (fd1 < 0)
    {
      printf ("Error is in opeaning file: %d", errno);
      exit (5);
    }
  char pid[5] = { '0' };
  strcpy (pid, (char *) attach_addr);
  printf ("%s\n", pid);
  consumer_pid = atoi (pid);
  printf ("consumer pid : %d\n", consumer_pid);

  char buff[100];
  while ((ret = read (fd1, buff, sizeof (buff))) > 0)
    {

      /* wait on s0 */
      memset (&sembuf_array, 0, sizeof (sembuf_array));
      sembuf_array[0].sem_num = 0;
      sembuf_array[0].sem_op = -1;
      ret = semop (semid, sembuf_array, 1);
      // printf ("Wait semop returned %d\n", ret);
      if (ret == -1)
        {
          printf ("semop errno = %d\n", errno);
          exit (1);
        }
      memset (attach_addr, 0, 100);
      strcpy ((char *) attach_addr, buff);
      memset (buff, 0, 100);

      /* signal on s1 */
      memset (&sembuf_array, 0, sizeof (sembuf_array));
      sembuf_array[0].sem_num = 1;
      sembuf_array[0].sem_op = 1;
      ret = semop (semid, sembuf_array, 1);
      //printf ("Wait semop returned %d\n", ret);
    }
  if (ret == -1)
    {
      printf ("semop errno = %d\n", errno);
      exit (1);
    }




if (shmdt (attach_addr) < 0)
  {
    perror ("Error is in shmdt to detach shared memory");
    exit (7);
  }
if ((kill (consumer_pid, 10)) < 0)
  {
    perror ("Kill failed");
    exit (5);
  }

}
