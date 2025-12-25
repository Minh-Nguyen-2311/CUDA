#pragma once
#include <cstddef>

#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <cstddef>

class PosixSharedMemory
{
    public:
        PosixSharedMemory(const char* name, size_t size);

        ~PosixSharedMemory();

        void write(const void* src, size_t size);

        void* get() const;
    
    private:
        const char* shm_name;
        size_t shm_size;
        int shm_fd;
        void *data;
};