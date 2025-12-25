#include <posix.hpp>

PosixSharedMemory::PosixSharedMemory(const char* name, size_t size)
    : shm_name(name), shm_size(size)
{
    //Open the shared memory object
    shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);

    if(shm_fd < 0) {
        perror("shm_open");
        exit(1);
    }
    if(ftruncate(shm_fd, shm_size) == -1){
        perror("ftruncate");
        exit(1);
    }
    //Memory map the shared memory object
    data = mmap(0, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    if(data == MAP_FAILED){
        perror("mmap");
        exit(1);
    }
}

PosixSharedMemory::~PosixSharedMemory()
{
    //Remove the shared memory object
    munmap(data, shm_size);
    close(shm_fd);
    shm_unlink(shm_name);
}

void PosixSharedMemory::write(const void* src, size_t size)
{
    if(!data) return;
    if(size > shm_size)
    {
        std::cerr << "Shared memory overflow!" <<std::endl;
        return;
    }
    memcpy(data, src, size);
}

void* PosixSharedMemory::get() const
{
    return data;
}