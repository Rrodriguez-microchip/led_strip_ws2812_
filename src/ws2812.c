#include "ws2812.h"
#include <zephyr/device.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(ws2812, LOG_LEVEL_INF);

// LED buffer
static rgb_t led_buffer[NUM_LEDS];

// Global brightness control (0-255, where 255 = full brightness)
static uint8_t global_brightness = 255;  // Start at 25% brightness for testing

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

    // Compensate for bad LED at position 0 by shifting left
    // Physical LED 0 is bad, so we skip it: write to index-1 in buffer
    // This way: buffer[0]→LED 1, buffer[1]→LED 2, etc.
    if (index > 0) {
        index = index - 1;
    } else {
        // index 0 would go to bad LED, skip it
        return;
    }

    // Safety check - with shift, we can only address NUM_LEDS-1
    if(index >= NUM_LEDS - 1) {
        LOG_WRN("Pixel write overflow: x=%d, y=%d, index=%d (limit=%d)", x, y, index, NUM_LEDS-1);
        return;
    }

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

    // Apply same shift as set_pixel
    if (index > 0) {
        index = index - 1;
    } else {
        return (rgb_t){0, 0, 0};
    }

    if(index >= NUM_LEDS) return (rgb_t){0, 0, 0};

    return led_buffer[index];
}

void ws2812_clear(void) {
    memset(led_buffer, 0, sizeof(led_buffer));
}

void ws2812_update(void) {
    // Each WS2812 color byte (8 bits) becomes 8 SPI bytes
    // Since we shift left by 1, we only use NUM_LEDS-1 actual LEDs (255 LEDs)
    // 255 LEDs * 3 colors * 8 SPI bytes per color byte = 6120 bytes
    // Add leading zeros to force line LOW and ensure proper alignment
    // Add trailing zeros at the end to ensure line idles LOW during reset
    static uint8_t spi_buf[24 + (NUM_LEDS - 1) * 3 * 8 + 8];  // +24 leading zeros
    uint16_t spi_idx = 0;

  // Add trailing zeros to force line LOW during reset
    for (int i = 0; i < 8; i++) {
        spi_buf[spi_idx++] = 0x00;
    }
    // Convert RGB buffer to SPI timing pattern
    // Note: Bad LED compensation is now handled in ws2812_set_pixel() by shifting left when writing
    // Since we shift left, the last LED (index NUM_LEDS-1) is never written to, so only send NUM_LEDS-1
    for (int i = 0; i < NUM_LEDS - 1; i++) {
        // Compensate for byte-level shift: rotate color order by sending GRB instead of BGR
        // This compensates for the SPI idle-high causing a bit/byte shift at second LED
        uint8_t colors[3] = {
            (led_buffer[i].g * global_brightness) / 255,  // G first (was B)
            (led_buffer[i].r * global_brightness) / 255,  // R second (was G)
            (led_buffer[i].b * global_brightness) / 255   // B third (was R)
        };

        LOG_DBG("LED %d: B=%d G=%d R=%d", i, colors[0], colors[1], colors[2]);

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
void ws2812_zeroes(void) {
    // Send all zeros to turn off all LEDs
    // Buffer size: same as normal update to ensure proper reset timing
    static uint8_t spi_buf[22 + (NUM_LEDS - 1) * 3 * 8 + 8];

    // Fill entire buffer with zeros
    memset(spi_buf, 0x00, sizeof(spi_buf));

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
        LOG_DBG("SPI zeroes - sent %d bytes", sizeof(spi_buf));
    }

    // WS2812 needs >50us reset time
    k_usleep(60);
}

void ws2812_set_brightness(uint8_t brightness) {
    global_brightness = brightness;
}
