#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/wait.h>

#include <thread>
#include <chrono>

#define NUM 3
#define SIZE (NUM * sizeof(int))

int main() {
  sem_t* sem_id = sem_open("/sem", O_CREAT, 0666, 0);
  if (sem_id == SEM_FAILED) {
    perror("Writer: sem_open failed!\n");
  }

  int fd = shm_open("/shmem", O_CREAT | O_RDWR, 0666);
  if (fd < 0) {
    perror("Writer: shm_open failed!\n");
    return EXIT_FAILURE;
  }

  ftruncate(fd, SIZE);

  int *data =
      (int *)mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

  for (int i = 0; i < NUM; ++i) {
    data[i] = i;
  }

  if (sem_post(sem_id) < 0) {
    perror("Writer: sem_post failed!\n");
  }

  std::this_thread::sleep_for(std::chrono::seconds(1));

  if (sem_wait(sem_id) < 0) {
    perror("Writer: sem_wait failed!\n");
  }

  printf("Writer: finishing...\n");

  if (sem_close(sem_id) != 0) {
    perror("Writer: sem_close failed!\n");
  }

  if (sem_unlink("/sem") < 0) {
    perror("Writer: sem_unlink failed!\n");
  }

  munmap(data, SIZE);

  close(fd);

  return EXIT_SUCCESS;
}
