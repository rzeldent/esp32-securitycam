#pragma once

#include <esp_camera.h>
#include "camera_pins.h"

#include <stdlib.h>
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
            buffer(size_t size, uint8_t *data, bool autofree)
            {
                autofree_ = autofree;
                data_ = data;
                size_ = size;
            }
            ~buffer()
            {
                if (autofree_)
                    free(data_);
            };

            const size_t size() const { return size_; }
            const uint8_t *data() const { return data_; }

        private:
            bool autofree_;
            size_t size_;
            uint8_t *data_;
        };

        const size_t width() const
        {
            return fb_->width;
        }
        const size_t height() const
        {
            return fb_->height;
        }
        const pixformat_t format() const
        {
            return fb_->format;
        }

        std::shared_ptr<buffer> as_jpeg(int jpeg_quality = 80) const;
        std::shared_ptr<buffer> as_rbg888() const;

    protected:
        camera_fb_t *fb_;
        int64_t time_;
    };

    bool initialize(framesize_t framesize = FRAMESIZE_SVGA, pixformat_t pixformat = PIXFORMAT_JPEG, int jpeg_quality = 12);
    void deinitialize();

    static std::shared_ptr<frame> get_frame();
};