#include "ws2812.h"
#include <zephyr/device.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(ws2812, LOG_LEVEL_INF);

// LED buffer
static rgb_t led_buffer[NUM_LEDS];

// Global brightness control (0-255, where 255 = full brightness)
static uint8_t global_brightness = 14;  // Start at 25% brightness for testing

// Mutex for thread-safe access
K_MUTEX_DEFINE(matrix_mutex);

// SPI device
static const struct device *spi_dev;
static struct spi_config spi_cfg = {
    .frequency = 6400000,  // 6.4 MHz for WS2812 timing
    .operation = SPI_WORD_SET(8) | SPI_TRANSFER_MSB | SPI_OP_MODE_MASTER,
    .slave = 0,
    .cs = {
        .gpio = {0},
        .delay = 0,
    },
};

// WS2812 bit patterns using SPI
// At 6.4MHz SPI: each bit is ~156ns
// WS2812 timing: 0 = 400ns H + 850ns L, 1 = 800ns H + 450ns L
// Using SPI_MODE_CPOL to invert clock and hopefully MOSI idle state
#define WS2812_0 0xC0  // Binary: 11000000 (~312ns high, ~938ns low)
#define WS2812_1 0xF0  // Binary: 11110000 (~625ns high, ~625ns low)

int ws2812_init(void) {
    spi_dev = DEVICE_DT_GET(DT_NODELABEL(sercom4));

    if (!device_is_ready(spi_dev)) {
        LOG_ERR("SPI device not ready");
        return -ENODEV;
    }

    LOG_INF("WS2812 driver initialized on SERCOM4 - Direct SPI");

    ws2812_clear();
    ws2812_update();
    return 0;
}

void ws2812_set_pixel(uint8_t x, uint8_t y, rgb_t color) {
    if (x >= MATRIX_WIDTH || y >= MATRIX_HEIGHT) return;

    // Convert x,y to linear index
    // Assumes zigzag wiring pattern - adjust based on your matrix
    uint16_t index;
    if (y % 2 == 0) {
        // Even rows: left to right
        index = y * MATRIX_WIDTH + x;
    } else {
        // Odd rows: right to left (zigzag)
        index = y * MATRIX_WIDTH + (MATRIX_WIDTH - 1 - x);
    }

    if(index >= NUM_LEDS) return; // Safety check

    led_buffer[index] = color;
}

rgb_t ws2812_get_pixel(uint8_t x, uint8_t y) {
    if (x >= MATRIX_WIDTH || y >= MATRIX_HEIGHT) {
        return (rgb_t){0, 0, 0};
    }

    uint16_t index;
    if (y % 2 == 0) {
        index = y * MATRIX_WIDTH + x;
    } else {
        index = y * MATRIX_WIDTH + (MATRIX_WIDTH - 1 - x);
    }

    return led_buffer[index];
}

void ws2812_clear(void) {
    memset(led_buffer, 0, sizeof(led_buffer));
}

void ws2812_update(void) {
    // Each WS2812 color byte (8 bits) becomes 8 SPI bytes
    // For testing with 5 LEDs: 5 LEDs * 3 colors * 8 bits = 120 bytes
    // Add extra reset bytes at the end to ensure line idles LOW
    static uint8_t spi_buf[NUM_LEDS * 3 * 8 + 8];  // Extra 8 bytes for reset
    uint16_t spi_idx = 0;

    // Convert RGB buffer to SPI timing pattern
    // Shift buffer by 1: send led_buffer[i+1] to physical LED i
    // This compensates for bad LED at physical position 0
    for (int i = 0; i < NUM_LEDS - 1; i++) {
        // WS2812 expects GRB order - apply brightness scaling
        uint8_t colors[3] = {
            (led_buffer[i + 1].g * global_brightness) / 255,
            (led_buffer[i + 1].r * global_brightness) / 255,
            (led_buffer[i + 1].b * global_brightness) / 255
        };

        LOG_DBG("LED %d: G=%d R=%d B=%d", i, colors[0], colors[1], colors[2]);

        // Convert each color byte to SPI bits
        for (int c = 0; c < 3; c++) {
            for (int bit = 7; bit >= 0; bit--) {
                spi_buf[spi_idx++] = (colors[c] & (1 << bit)) ? WS2812_1 : WS2812_0;
            }
        }
    }

    // Add trailing zeros to force line LOW during reset
    for (int i = 0; i < 8; i++) {
        spi_buf[spi_idx++] = 0x00;
    }

    // Send via SPI
    const struct spi_buf tx_buf = {
        .buf = spi_buf,
        .len = sizeof(spi_buf)
    };
    const struct spi_buf_set tx = {
        .buffers = &tx_buf,
        .count = 1
    };

    int ret = spi_write(spi_dev, &spi_cfg, &tx);
    if (ret < 0) {
        LOG_ERR("SPI write failed: %d", ret);
    } else {
        LOG_DBG("SPI write OK - sent %d bytes", sizeof(spi_buf));
    }

    // WS2812 needs >50us reset time (line will idle at last bit = 0)
    k_usleep(60);
}

void ws2812_set_brightness(uint8_t brightness) {
    global_brightness = brightness;
}
