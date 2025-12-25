#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/core/cuda.hpp>
#include <opencv2/core.hpp>         // Basic OpenCV structures
#include <opencv2/imgproc.hpp>      // Image processing methods for the CPU
#include <opencv2/imgcodecs.hpp>    // Images IO
#include <opencv2/cudaarithm.hpp>   // CUDA matrix operations
#include <opencv2/cudafilters.hpp>  // CUDA image filters
#include "posix.hpp"

class VisionPipeline
{
    public:
        VisionPipeline();
        ~VisionPipeline();
        void run();

    private:
        int idx = 0;

        static const int W = 640;
        static const int H = 480;
        //1 if mono, 3 if RGB
        static const int C = 3;
        static const int frame_bytes = W * H * C;

        cv::VideoCapture cap;
        cv::cuda::GpuMat gpu_in, gpu_out;
        cv::Mat frame, random_color;

        PosixSharedMemory shm0;
        PosixSharedMemory shm1;
        PosixSharedMemory shm_idx;
};