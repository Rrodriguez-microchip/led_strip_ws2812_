#ifndef WS2812_H
#define WS2812_H

#include <zephyr/kernel.h>

#define MATRIX_WIDTH  16
#define MATRIX_HEIGHT 16
#define NUM_LEDS (MATRIX_WIDTH * MATRIX_HEIGHT)

typedef struct {
    uint8_t g;
    uint8_t r;
    uint8_t b;
} rgb_t;

// Initialize WS2812 driver
int ws2812_init(void);

// Set a single pixel (x, y, color)
void ws2812_set_pixel(uint8_t x, uint8_t y, rgb_t color);

// Get pixel color
rgb_t ws2812_get_pixel(uint8_t x, uint8_t y);

// Clear all pixels
void ws2812_clear(void);

// Send buffer to LEDs (call this to update display)
void ws2812_update(void);

// Set global brightness (0-255, where 255 = full brightness)
void ws2812_set_brightness(uint8_t brightness);

// Mutex for thread-safe access
extern struct k_mutex matrix_mutex;

#endif /* WS2812_H */