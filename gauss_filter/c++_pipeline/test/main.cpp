#include "posix.hpp"
#include "vision.hpp"
#include <iostream>
#include <unistd.h>

int main()
{
    VisionPipeline vision;
    vision.run();
    return 0;
}
