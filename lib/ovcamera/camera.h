#pragma once

#include <esp_camera.h>
#include "FS.h"

#include <camera_pins.h>
#include <bitmap.h>

class camera
{
public:
    struct frame : camera_fb_t
    {
        frame();
        ~frame();
        size_t write_bitmap(File file);

    private:
        size_t write_rgb888_bm_header(File file);
        size_t write_grayscale_bm_header(File file);
    };

    bool initialize(framesize_t framesize = FRAMESIZE_SVGA, pixformat_t pixformat = PIXFORMAT_JPEG, int jpeg_quality = 12);
    void deinitialize();
};