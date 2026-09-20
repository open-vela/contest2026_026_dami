#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "server_config.h"

static inline uint16_t rgb888_to_rgb565(uint8_t r, uint8_t g, uint8_t b) {
    return (uint16_t)(((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3));
}

static uint8_t extract_channel(unsigned long pixel, unsigned long mask) {
    if (mask == 0) return 0;

    int shift = 0;
    unsigned long m = mask;
    while (!(m & 1UL)) { m >>= 1; shift++; }

    int bits = 0;
    while (m & 1UL) { bits++; m >>= 1; }

    unsigned long v = (pixel & mask) >> shift;

    if (bits == 8) return (uint8_t)v;
    if (bits <  8) return (uint8_t)(v << (8 - bits));
    return (uint8_t)(v >> (bits - 8));
}

static void resize_bilinear(const unsigned char *src, int sw, int sh,
                            unsigned char *dst, int dw, int dh) {
    for (int y = 0; y < dh; y++) {
        float src_y = (y + 0.5f) * (float)sh / dh - 0.5f;
        if (src_y < 0) src_y = 0;
        int y0 = (int)src_y;
        if (y0 >= sh - 1) y0 = sh - 2;
        int y1 = y0 + 1;
        float wy = src_y - y0;

        for (int x = 0; x < dw; x++) {
            float src_x = (x + 0.5f) * (float)sw / dw - 0.5f;
            if (src_x < 0) src_x = 0;
            int x0 = (int)src_x;
            if (x0 >= sw - 1) x0 = sw - 2;
            int x1 = x0 + 1;
            float wx = src_x - x0;

            for (int c = 0; c < 3; c++) {
                float v00 = src[(y0 * sw + x0) * 3 + c];
                float v01 = src[(y0 * sw + x1) * 3 + c];
                float v10 = src[(y1 * sw + x0) * 3 + c];
                float v11 = src[(y1 * sw + x1) * 3 + c];
                float v0 = v00 * (1 - wx) + v01 * wx;
                float v1 = v10 * (1 - wx) + v11 * wx;
                float v  = v0 * (1 - wy) + v1 * wy;
                if (v < 0) v = 0;
                if (v > 255) v = 255;
                dst[(y * dw + x) * 3 + c] = (unsigned char)(v + 0.5f);
            }
        }
    }
}

int capture_screen_to_pic_data(void) {
    Display *display = XOpenDisplay(NULL);
    if (!display) return -1;

    Window root = DefaultRootWindow(display);
    XWindowAttributes gwa;
    XGetWindowAttributes(display, root, &gwa);
    int width  = gwa.width;
    int height = gwa.height;

    XImage *image = XGetImage(display, root, 0, 0, width, height,
                              AllPlanes, ZPixmap);
    if (!image) {
        XCloseDisplay(display);
        return -2;
    }

    unsigned char *raw_rgb = malloc((size_t)width * height * 3);
    if (!raw_rgb) {
        XDestroyImage(image);
        XCloseDisplay(display);
        return -3;
    }
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            unsigned long pixel = XGetPixel(image, x, y);
            int idx = (y * width + x) * 3;
            raw_rgb[idx]     = extract_channel(pixel, image->red_mask);
            raw_rgb[idx + 1] = extract_channel(pixel, image->green_mask);
            raw_rgb[idx + 2] = extract_channel(pixel, image->blue_mask);
        }
    }

    unsigned char *scaled_rgb = malloc((size_t)PIC_W * PIC_H * 3);
    if (!scaled_rgb) {
        free(raw_rgb);
        XDestroyImage(image);
        XCloseDisplay(display);
        return -4;
    }
    resize_bilinear(raw_rgb, width, height, scaled_rgb, PIC_W, PIC_H);

    for (int i = 0; i < PIC_W * PIC_H; i++) {
        uint8_t r = scaled_rgb[i * 3];
        uint8_t g = scaled_rgb[i * 3 + 1];
        uint8_t b = scaled_rgb[i * 3 + 2];

        uint16_t v = rgb888_to_rgb565(r, g, b);
        pic_data[i * 2]     = (uint8_t)(v & 0xFF);          // low  byte
        pic_data[i * 2 + 1] = (uint8_t)((v >> 8) & 0xFF);   // high byte
    }

    free(raw_rgb);
    free(scaled_rgb);
    XDestroyImage(image);
    XCloseDisplay(display);
    return 0;
}
