#pragma once

#include <esp_camera.h>
#include "FS.h"

#include <camera_pins.h>

class Camera
{
public:
    struct Frame
    {
        camera_fb_t *fb_;

        Frame();
        ~Frame();

        size_t write_jpeg(File file);
        size_t write_bitmap(File file);
    };

    // 0-63 lower means higher quality
    bool initialize(framesize_t framesize = FRAMESIZE_SVGA, int jpeg_quality = 6);
    void deinitialize();
};