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
            ~buffer();

            bool autofree_;
            size_t size_;
            uint8_t *data_;
        };

        buffer as_jpeg(int jpeg_quality /*= 80*/);

    protected:
        camera_fb_t *fb_;
        int64_t time_;
    };

    camera(framesize_t framesize = FRAMESIZE_SVGA, pixformat_t pixformat = PIXFORMAT_JPEG, int jpeg_quality = 12);

    static std::shared_ptr<frame> get_frame();
};