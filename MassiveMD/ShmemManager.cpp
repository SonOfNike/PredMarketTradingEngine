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
    size_t shm_size = sizeof(ValShmem);

    // Create or open the shared memory object
    shm_fd = shm_open(VAL_shm_name, O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
    }

    // Configure the size of the shared memory object
    if (ftruncate(shm_fd, shm_size) == -1) {
        perror("ftruncate");
    }

    // Map the shared memory object into the process's address space
    val_shmem = (ValShmem*)mmap(0, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (val_shmem == MAP_FAILED) {
        perror("mmap");
    }
    close(shm_fd);
}

void ShmemManager::shutDown(){
    if (munmap(val_shmem, sizeof(ValShmem)) == -1) {
        perror("munmap");
    }
}

void ShmemManager::write_Val(const ValUpdate& _val_update){
    val_shmem->m_queue[val_shmem->next_write_index] = _val_update;
    val_shmem->next_write_index++;
    if(val_shmem->next_write_index >= VAL_QUEUE_SIZE){
        val_shmem->next_write_index = 0;
    }
}