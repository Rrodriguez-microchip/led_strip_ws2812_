/*
 * Multi-Threaded Quadrant Animation Demo
 *
 * This demo showcases Zephyr RTOS threading concepts through visual animations
 * on a 16x16 WS2812 LED matrix. Perfect for workshops and educational purposes.
 *
 * Features:
 * - 4 independent animation threads (one per quadrant)
 * - Priority-based scheduling visualization
 * - Thread state monitoring via LED borders
 * - Interactive shell commands for runtime control
 * - CPU load simulation
 *
 * To use this demo:
 * 1. Rename this file to main.c (backup your current main.c first)
 * 2. Build and flash to your board
 * 3. Connect via serial terminal (115200 baud)
 * 4. Use shell commands to interact with the demo
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/logging/log.h>
#include "ws2812.h"
#include "quadrant_simple_test.h"  // Using simple test instead

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

int main(void) {
    int ret;

    LOG_INF("===========================================");
    LOG_INF("  Zephyr RTOS Thread Priority Demo");
    LOG_INF("  Board: SAM E54 Xplained Pro");
    LOG_INF("  16x16 WS2812 LED Matrix");
    LOG_INF("===========================================");

    // Initialize WS2812 driver
    ret = ws2812_init();
    if (ret != 0) {
        LOG_ERR("Failed to initialize WS2812: %d", ret);
        return ret;
    }

    // Set brightness to 50% for safety
    ws2812_set_brightness(255);
    LOG_INF("WS2812 initialized (brightness: 50%%)");

    // Clear display
    ws2812_clear();
    ws2812_update();

    // Initialize and start the SIMPLE test (two balls)
    simple_test_init();

    LOG_INF("");
    LOG_INF("Demo running! Press SW0 to change Q1 priority");
    LOG_INF("");

    // Main thread just sleeps
    while (1) {
        k_msleep(60000);  // Just sleep
    }

    return 0;
}
