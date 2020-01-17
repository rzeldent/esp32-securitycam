#include "camera.h"

#include <Arduino.h>

bool camera::initialize(framesize_t framesize /*= FRAMESIZE_SVGA*/, pixformat_t pixformat /*= PIXFORMAT_JPEG*/, int jpeg_quality /*= 12*/)
{
    log_i("Camera initialization starting");
    camera_config_t config = {
        .pin_pwdn = PWDN_GPIO_NUM,
        .pin_reset = RESET_GPIO_NUM,
        .pin_xclk = XCLK_GPIO_NUM,
        .pin_sscb_sda = SIOD_GPIO_NUM,
        .pin_sscb_scl = SIOC_GPIO_NUM,
        .pin_d7 = Y9_GPIO_NUM,
        .pin_d6 = Y8_GPIO_NUM,
        .pin_d5 = Y7_GPIO_NUM,
        .pin_d4 = Y6_GPIO_NUM,
        .pin_d3 = Y5_GPIO_NUM,
        .pin_d2 = Y4_GPIO_NUM,
        .pin_d1 = Y3_GPIO_NUM,
        .pin_d0 = Y2_GPIO_NUM,
        .pin_vsync = VSYNC_GPIO_NUM,
        .pin_href = HREF_GPIO_NUM,
        .pin_pclk = PCLK_GPIO_NUM,

        .xclk_freq_hz = 20000000,

        .ledc_timer = LEDC_TIMER_0,
        .ledc_channel = LEDC_CHANNEL_0,

        .pixel_format = pixformat,
        .frame_size = framesize,

        .jpeg_quality = jpeg_quality,
        .fb_count = psramFound() ? (size_t)2 : (size_t)1};

    // camera initialization
    auto result = esp_camera_init(&config);
    if (result == ESP_OK)
        log_i("Camera initialization finished");
    else
        log_e("Camera initialization failed");

    return result;
}

camera::frame::frame()
{
    fb_ = esp_camera_fb_get();
    time_ = esp_timer_get_time();
}

camera::frame::~frame()
{
    esp_camera_fb_return(fb_);
}

camera::frame::buffer::buffer(bool autofree /*= true*/)
{
    autofree_ = autofree;
    data_ = nullptr;
    size_ = 0;
}

camera::frame::buffer::buffer(size_t size, bool autofree /*= true*/)
{
    autofree_ = autofree;
    data_ = new uint8_t[size];
    size_ = data_ ? size : 0;
}

camera::frame::buffer::~buffer()
{
    if (autofree_)
        free(data_);
}

camera::frame::buffer camera::frame::as_jpeg(int jpeg_quality /*= 80*/)
{
    if (fb_->format != PIXFORMAT_JPEG)
    {
        camera::frame::buffer result;
        if (!frame2jpg(fb_, jpeg_quality, &result.data_, &result.size_))
            log_e("Failed to convert to JPEG");

        return result;
    }
    else
    {
        camera::frame::buffer result(false);
        result.size_ = fb_->len;
        result.data_ = fb_->buf;
        return result;
    }
}

camera::frame::buffer camera::frame::as_rbg888()
{
    if (fb_->format != PIXFORMAT_RGB888)
    {
        camera::frame::buffer result(fb_->width * fb_->height * 3);
        if (!fmt2rgb888(fb_->buf, fb_->len, fb_->format, result.data_))
            log_e("Failed to convert to rgb888");

        return result;
    }
    else
    {
        camera::frame::buffer result(false);
        result.size_ = fb_->len;
        result.data_ = fb_->buf;
        return result;
    }
}

std::shared_ptr<camera::frame> camera::get_frame()
{
    return std::shared_ptr<camera::frame>(new frame());
}