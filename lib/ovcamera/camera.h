#pragma once

#include <esp_camera.h>
#include "FS.h"

#include <camera_pins.h>

class Camera
{
public:
    struct Frame
    {
        Frame();
        ~Frame();
        size_t write_jpeg(File file);
        size_t write_bitmap(File file);

    private:
        camera_fb_t *fb_;
    };

    // 0-63 lower means higher quality
    bool initialize(framesize_t framesize = FRAMESIZE_SVGA, int jpeg_quality = 6);
    void deinitialize();
};