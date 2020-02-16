#include <Arduino.h>

#include <camera.h>

// removed possibility to set the pixel format. Threason is that this crashed the program or allowed only very low resolutions.
// From: https://github.com/m5stack/m5stack-cam-psram/blob/master/README.md:
// If you need RGB data, it is recommended that JPEG is captured and then turned into RGB using fmt2rgb888 or fmt2bmp/frame2bmp.

bool Camera::initialize(framesize_t framesize /*= FRAMESIZE_SVGA*/, int jpeg_quality /*= 6*/)
{
    log_i("Camera initialization");
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

        .pixel_format = PIXFORMAT_JPEG,
        .frame_size = framesize,

        .jpeg_quality = jpeg_quality,
        .fb_count = psramFound() ? (size_t)2 : (size_t)1};

    // camera initialization
    auto result = esp_camera_init(&config);
    if (result == ESP_OK)
        log_d("Camera initialization successful");
    else
        log_e("Camera initialization failed. Code=%d", result);

    return result == ESP_OK;
}

Camera::Frame::Frame()
{
    fb_ = esp_camera_fb_get();
    log_d("New frame. size: %ld.", fb_->len);
}

Camera::Frame::~Frame()
{
    esp_camera_fb_return(fb_);
    log_d("Deleted frame.");
}

//    fmt2bmp

size_t Camera::Frame::write_jpeg(File file)
{
    return file.write(fb_->buf, fb_->len);
}

// bitmap you should not go over 800x600.
size_t Camera::Frame::write_bitmap(File file)
{
    uint8_t *out;
    size_t out_len;
    if (!frame2bmp(fb_, &out, &out_len))
    {
        log_e("Could not allocate buffer for frame2bmp");
        return 0;
    }

    out_len = file.write(out, out_len);
    free(out);
    return out_len;
}
