#ifndef PICTUR_H_
#define PICTUR_H_

static inline uint16_t rgb888_to_rgb565(uint8_t r, uint8_t g, uint8_t b);

static void resize_bilinear(const unsigned char *src, int sw, int sh,
                            unsigned char *dst, int dw, int dh);

int capture_screen_to_pic_data(void);

#endif
