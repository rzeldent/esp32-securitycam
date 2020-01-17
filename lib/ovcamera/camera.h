#pragma once

#include <esp_camera.h>
#include "camera_pins.h"

#include <memory>

class camera
{
public:
    struct frame
    {
    public:
        frame();
        ~frame();

        struct buffer
        {
            buffer(bool autofree = true);
            buffer(size_t size, bool autofree = true);
            ~buffer();

            bool autofree_;
            size_t size_;
            uint8_t *data_;
        };

        buffer as_jpeg(int jpeg_quality /*= 80*/);
        buffer as_rbg888();

    protected:
        camera_fb_t *fb_;
        int64_t time_;
    };

    bool initialize(framesize_t framesize = FRAMESIZE_SVGA, pixformat_t pixformat = PIXFORMAT_JPEG, int jpeg_quality = 12);
    void deinitialize();

    static std::shared_ptr<frame> get_frame();
};