#include "ShmemManager.h"
#include <iostream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

ShmemManager* ShmemManager::uniqueInstance = nullptr;

ShmemManager* ShmemManager::getInstance(){
    if(uniqueInstance == nullptr){
        uniqueInstance = new ShmemManager();
    }
    return uniqueInstance;
}

void ShmemManager::startUp(){
    int shm_fd;
    size_t shm_size = sizeof(ORShmem);

    // Create or open the shared memory object
    shm_fd = shm_open(OR_shm_name, O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
    }

    // Configure the size of the shared memory object
    if (ftruncate(shm_fd, shm_size) == -1) {
        perror("ftruncate");
    }

    // Map the shared memory object into the process's address space
    or_shmem = (ORShmem*)mmap(0, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (or_shmem == MAP_FAILED) {
        perror("mmap");
    }
    close(shm_fd);
}

void ShmemManager::shutDown(){
    if (munmap(or_shmem, sizeof(ORShmem)) == -1) {
        perror("munmap");
    }
}

void ShmemManager::write_OR(const OrderResponse& _order_response){
    or_shmem->m_queue[or_shmem->next_write_index] = _order_response;
    or_shmem->next_write_index++;
    if(or_shmem->next_write_index >= OR_QUEUE_SIZE){
        or_shmem->next_write_index = 0;
    }
}