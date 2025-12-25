#include "vision.hpp"
#include <iostream>
#include <stdio.h>

VisionPipeline::VisionPipeline()
    : shm0("/frame_buf_0", frame_bytes),
      shm1("/frame_buf_1", frame_bytes),
      shm_idx("/frame_index", sizeof(int))
{}

VisionPipeline::~VisionPipeline()
{
    cap.release();
}

void VisionPipeline::run()
{
    // Advance usage: select any API backend
    int deviceID = 0;             // 0 = open default camera
    int apiID = cv::CAP_ANY;      // 0 = autodetect default API
    // Open selected camera using selected API
    cap.open(deviceID, apiID);

    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    cap.set(cv::CAP_PROP_AUTO_EXPOSURE, 1);
    cap.set(cv::CAP_PROP_EXPOSURE, -6);
    cap.set(cv::CAP_PROP_GAIN, 0);

    if(!cap.isOpened()) return;

    //Sobel X,Y
    auto sobelX = cv::cuda::createSobelFilter(CV_8UC3, CV_8UC3, 1, 0);
    auto sobelY = cv::cuda::createSobelFilter(CV_8UC3, CV_8UC3, 0, 1);

    for(;;)
    {
        cap >> frame;
        if(frame.empty()) break;

        // ===== CUDA PIPELINE ===== //
        
        // (1) Upload BRG frame to GPU
        gpu_in.upload(frame);

        // (2) Sobel
        sobelX->apply(gpu_in, gpu_out);
        sobelY->apply(gpu_in, gpu_out);

        // (3) Download back to CPU
        cv::Mat out_cpu;
        gpu_out.download(out_cpu);

        // Choose buffer
        uint8_t *target_buf = nullptr;
        if(idx == 0) target_buf = static_cast<uint8_t*>(shm0.get());
        else target_buf = static_cast<uint8_t*>(shm1.get());

        // Copy frame to buffer
        memcpy(target_buf, out_cpu.data, frame_bytes);
        // memcpy(target_buf, random_color.data, frame_bytes);

        // Publish index
        *static_cast<int*>(shm_idx.get()) = idx;

        // Flip buffer
        idx ^= 1;

        // Esc to shutdown app
        if(cv::waitKey(1) == 27)
        {
            cap.release();
            break;
        }
    }
}