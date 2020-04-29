#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include <semaphore.h>
#include <sys/wait.h>

#define NUM 3
#define NAME "/shmem"
#define SIZE (NUM * sizeof(int))

int main() {
  sem_t* sem_id = sem_open("/sem", O_CREAT, 0666, 0);
  if (sem_id < SEM_FAILED) {
    perror("Reader: sem_open failed!\n");
  }

  int fd = shm_open("/shmem", O_CREAT | O_RDWR, 0666);
  if (fd < 0) {
    perror("Reader: shm_open failed!\n");
    return EXIT_FAILURE;
  }

  int *data =
      (int *)mmap(0, SIZE, PROT_READ, MAP_SHARED, fd, 0);

  if (sem_wait(sem_id) < 0) {
    perror("Reader: sem_wait failed!\n");
  }

  for (int i = 0; i < NUM; ++i) {
    printf("%d\n", data[i]);
  }

  if (sem_post(sem_id) < 0) {
    perror("Reader: sem_post failed!\n");
  }

  munmap(data, SIZE);

  close(fd);

  //shm_unlink(NAME);

  return EXIT_SUCCESS;
}
