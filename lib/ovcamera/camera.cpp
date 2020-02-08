#include <Arduino.h>

#include <camera.h>
#include <bitmap_colors.h>

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
        log_i("Camera initialization successful");
    else
        log_e("Camera initialization failed. Code=%x", result);

    return result;
}

camera::frame::frame()
    : camera_fb_t(*esp_camera_fb_get())
{
}

camera::frame::~frame()
{
    esp_camera_fb_return(this);
}

size_t camera::frame::write_bitmap(File file)
{
    //fmt2bmp()
    switch (format)
    {
    case PIXFORMAT_GRAYSCALE:
        return write_grayscale_bm_header(file) + file.write(buf, len);
    case PIXFORMAT_RGB888:
        return write_rgb888_bm_header(file) + file.write(buf, len);
    }

    return file.write(buf, len);
}

size_t camera::frame::write_rgb888_bm_header(File file)
{
    log_i("Writing bitmap header for rgb888");

    BITMAPFILEHEADER bmfh = {
        .bfType = BF_TYPE,
        .bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + 3 * width * height,
        .bfReserved1 = 0,
        .bfReserved2 = 0,
        .bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)};

    BITMAPINFOHEADER bmih = {
        .biSize = sizeof(BITMAPINFOHEADER),
        .biWidth = (int)width,
        .biHeight = (int)height,
        .biPlanes = 1,
        .biBitCount = 3 * 8,
        .biCompression = BI_RGB,
        .biSizeImage = 0,
        .biXPelsPerMeter = 2835,
        .biYPelsPerMeter = 2835,
        .biClrUsed = 0,
        .biClrImportant = 0};

    return file.write((uint8_t *)&bmfh, sizeof(BITMAPFILEHEADER)) +
           file.write((uint8_t *)&bmih, sizeof(BITMAPINFOHEADER));
}

size_t camera::frame::write_grayscale_bm_header(File file)
{
    log_i("Writing bitmap header for grayscale 1bpp");

    BITMAPFILEHEADER bmfh = {
        .bfType = BF_TYPE,
        .bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + 0x100 * sizeof(RGBQUAD) + width * height,
        .bfReserved1 = 0,
        .bfReserved2 = 0,
        .bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + 0x100 * sizeof(RGBQUAD)};

    BITMAPINFOHEADER bmih = {
        .biSize = sizeof(BITMAPINFOHEADER),
        .biWidth = (int)width,
        .biHeight = (int)height,
        .biPlanes = 1,
        .biBitCount = 8,
        .biCompression = BI_RGB,
        .biSizeImage = 0,
        .biXPelsPerMeter = 2835,
        .biYPelsPerMeter = 2835,
        .biClrUsed = 0,
        .biClrImportant = 0};

    return file.write((uint8_t *)&bmfh, sizeof(BITMAPFILEHEADER)) +
           file.write((uint8_t *)&bmih, sizeof(BITMAPINFOHEADER)) +
           file.write((uint8_t *)&colors_grayscale, 0x100 * sizeof(RGBQUAD));
}
