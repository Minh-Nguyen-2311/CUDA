import sys
import cv2
import os
import struct
import mmap
import time
import numpy as np
import posix_ipc

#----- CONFIGURE HERE -----
BUF0_NAME = "/frame_buf_0"
BUF1_NAME = "/frame_buf_1"
IDX_NAME = "/frame_index"
W = 640
H = 480
C = 3
FRAME_BYTES = W * H * C
#---------------------------

def open_shm(name, size):
    shm = posix_ipc.SharedMemory(name)
    mm = mmap.mmap(shm.fd, size, mmap.MAP_SHARED, mmap.PROT_READ)
    shm.close_fd()
    return mm

def main():
    print("[LOG] Opening shared memory ...")

    buf0 = open_shm(BUF0_NAME, FRAME_BYTES)
    buf1 = open_shm(BUF1_NAME, FRAME_BYTES)
    idx_mm = open_shm(IDX_NAME, 4)

    print("[LOG] Shared memory connected")

    last_idx = -1
    fps_prev = time.time()
    frame_count = 0

    while True:
        # Read current index
        idx = int.from_bytes(idx_mm[:4], byteorder="little")

        # Read new frame
        if idx != last_idx:
            last_idx = idx
            frame_count += 1

        # Choose buffer
        if idx == 0:
            mm = buf0
        elif idx == 1:
            mm = buf1
        else:
            print("[LOG] Invalid buffer detected ...")
            break

        # Map raw bytes ==> numpy array
        frame = np.frombuffer(mm, dtype=np.uint8).reshape(H,W,C)
        frame_vis = cv2.normalize(frame, None, 0, 255, cv2.NORM_MINMAX)

        cv2.imshow("POSIX SHM Viewer", frame)

        #FPS per sec
        fps_curr = time.time()
        if fps_curr - fps_prev >= 1.0:
            fps = frame_count / (fps_curr - fps_prev)
            print(f"FPS: {fps:.1f}")
            frame_count = 0
            fps_prev = fps_curr

        if cv2.waitKey(1) & 0xFF == 27:
            break

    cv2.destroyAllWindows()

if __name__ == "__main__":
    main()

            