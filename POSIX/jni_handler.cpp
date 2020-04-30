#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include <semaphore.h>
#include <sys/wait.h>

#define SIZE 1024

int main() {
  sem_t* sem_id_jvm = sem_open("/sem-jvm", O_CREAT, 0666, 0);
  if (sem_id_jvm < SEM_FAILED) {
    perror("Reader: sem_open failed!\n");
  }

  sem_t* sem_id_container = sem_open("/sem-container", O_CREAT, 0666, 0);
  if (sem_id_container < SEM_FAILED) {
    perror("Reader: sem_open failed!\n");
  }

  int fd = shm_open("/shmem", O_CREAT | O_RDWR, 0666);
  if (fd < 0) {
    perror("Reader: shm_open failed!\n");
    return EXIT_FAILURE;
  }

  char *data =
      (char *)mmap(0, SIZE, PROT_READ, MAP_SHARED, fd, 0);

  if (sem_wait(sem_id_jvm) < 0) {
    perror("Reader: sem_wait failed!\n");
  }

  printf("Received: %s\n", data);

  if (sem_post(sem_id_container) < 0) {
    perror("Reader: sem_post failed!\n");
  }

  if (sem_close(sem_id_jvm) != 0) {
    perror("Writer: sem_close failed!\n");
  }

  munmap(data, SIZE);

  close(fd);

  return EXIT_SUCCESS;
}
