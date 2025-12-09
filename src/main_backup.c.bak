#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include "ws2812.h"

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

// External pattern functions
void pattern_wave(void);
void pattern_ball(void);
void pattern_breath(void);
void pattern_twinkle(void);
void pattern_flash_burst(void);
void pattern_priority_visualizer(void);
void pattern_rainbow_sweep(void);

// Semaphore for button interrupt
K_SEM_DEFINE(button_sem, 0, 1);

// Button configuration (SW0 on PB31)
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(DT_ALIAS(sw0), gpios);
static struct gpio_callback button_cb_data;

// Button ISR
void button_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins) {
    k_sem_give(&button_sem);
}

// Thread stacks
#define STACK_SIZE 1024
K_THREAD_STACK_DEFINE(wave_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(ball_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(breath_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(twinkle_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(display_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(button_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(priority_viz_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(rainbow_stack, STACK_SIZE);

struct k_thread wave_thread_data;
struct k_thread ball_thread_data;
struct k_thread breath_thread_data;
struct k_thread twinkle_thread_data;
struct k_thread display_thread_data;
struct k_thread button_thread_data;
struct k_thread priority_viz_thread_data;
struct k_thread rainbow_thread_data;

// Thread 1: Wave pattern
void wave_thread(void *a, void *b, void *c) {
    LOG_INF("Wave thread started");
    while (1) {
        k_mutex_lock(&matrix_mutex, K_FOREVER);
        pattern_wave();
        k_mutex_unlock(&matrix_mutex);
        k_msleep(100);
    }
}

// Thread 2: Ball pattern
void ball_thread(void *a, void *b, void *c) {
    LOG_INF("Ball thread started");
    while (1) {
        k_mutex_lock(&matrix_mutex, K_FOREVER);
        ws2812_clear();  // Clear when ball runs alone
        pattern_ball();
        k_mutex_unlock(&matrix_mutex);
        k_msleep(50);
    }
}

// Thread 3: Breathing border
void breath_thread(void *a, void *b, void *c) {
    LOG_INF("Breath thread started");
    while (1) {
        k_mutex_lock(&matrix_mutex, K_FOREVER);
        pattern_breath();
        k_mutex_unlock(&matrix_mutex);
        k_msleep(50);
    }
}

// Thread 4: Twinkle
void twinkle_thread(void *a, void *b, void *c) {
    LOG_INF("Twinkle thread started");
    while (1) {
        k_mutex_lock(&matrix_mutex, K_FOREVER);
        pattern_twinkle();
        k_mutex_unlock(&matrix_mutex);
        k_msleep(100);
    }
}

// Thread 5: Display update
void display_thread(void *a, void *b, void *c) {
    LOG_INF("Display thread started");
    while (1) {
        k_mutex_lock(&matrix_mutex, K_FOREVER);
        ws2812_update();
        k_mutex_unlock(&matrix_mutex);
        k_msleep(20);  // 50 FPS
    }
}

// Thread 6: Button handler (highest priority)
void button_thread(void *a, void *b, void *c) {
    LOG_INF("Button thread started");
    while (1) {
        k_sem_take(&button_sem, K_FOREVER);
        LOG_INF("Button pressed - triggering flash!");
        pattern_flash_burst();
    }
}

// Thread 7: Priority Visualizer
void priority_viz_thread(void *a, void *b, void *c) {
    LOG_INF("Priority visualizer thread started");
    while (1) {
        k_mutex_lock(&matrix_mutex, K_FOREVER);
        pattern_priority_visualizer();
        k_mutex_unlock(&matrix_mutex);
        k_msleep(50);  // 20 FPS
    }
}

// Thread 8: Rainbow Sweep
void rainbow_thread(void *a, void *b, void *c) {
    LOG_INF("Rainbow sweep thread started");
    while (1) {
        k_mutex_lock(&matrix_mutex, K_FOREVER);
        pattern_rainbow_sweep();
        k_mutex_unlock(&matrix_mutex);
        k_msleep(30);  // ~33 FPS
    }
}

int main(void) {
    int ret;
    
    LOG_INF("===========================================");
    LOG_INF("LED Matrix Multi-Threading Demo");
    LOG_INF("Board: SAM E54 Xplained Pro");
    LOG_INF("===========================================");
    
    // Initialize WS2812
    ret = ws2812_init();
    ws2812_set_brightness(128);  // Set to 50% brightness
    if (ret != 0) {
        LOG_ERR("Failed to initialize WS2812: %d", ret);
        return ret;
    }
    LOG_INF("WS2812 initialized successfully");

    // Simple test pattern - first 5 LEDs
    // LOG_INF("Testing first 5 LEDs with simple pattern...");
    // ws2812_clear();

    // // LED 0: Red
    // ws2812_set_pixel(0, 0, (rgb_t){0, 255, 0});
    // // LED 1: Green
    // ws2812_set_pixel(1, 0, (rgb_t){255, 0, 0});
    // // LED 2: Blue
    // ws2812_set_pixel(2, 0, (rgb_t){0, 0, 255});
    // // LED 3: Yellow
    // ws2812_set_pixel(3, 0, (rgb_t){255, 255, 0});
    // // LED 4: Cyan
    // ws2812_set_pixel(4, 0, (rgb_t){255, 0, 255});

    // ws2812_update();
    LOG_INF("Test pattern sent - LEDs should be: R, G, B, Y, C");

    // Wait and blink for testing
    // while (1) {
    //     k_msleep(2000);
    //     ws2812_clear();
    //     ws2812_update();
    //     LOG_INF("LEDs OFF");
    //     k_msleep(1000);
        // Restore test pattern
    //     ws2812_set_pixel(0, 0, (rgb_t){0, 255, 0});
    //     ws2812_set_pixel(1, 0, (rgb_t){255, 0, 0});
    //     ws2812_set_pixel(2, 0, (rgb_t){0, 0, 255});
    //     ws2812_set_pixel(3, 0, (rgb_t){255, 255, 0});
    //     ws2812_set_pixel(4, 0, (rgb_t){255, 0, 255});
    //     ws2812_update();
    //     LOG_INF("LEDs ON");
    // }

    // Configure button
    if (!device_is_ready(button.port)) {
        LOG_ERR("Button device not ready");
        return -ENODEV;
    }
    
    ret = gpio_pin_configure_dt(&button, GPIO_INPUT);
    if (ret < 0) {
        LOG_ERR("Failed to configure button: %d", ret);
        return ret;
    }
    
    ret = gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_TO_ACTIVE);
    if (ret < 0) {
        LOG_ERR("Failed to configure button interrupt: %d", ret);
        return ret;
    }
    
    gpio_init_callback(&button_cb_data, button_pressed, BIT(button.pin));
    gpio_add_callback(button.port, &button_cb_data);
    
    LOG_INF("Button SW0 configured");
    LOG_INF("Press SW0 for interrupt animation!");
    
    // Create threads with priorities (lower number = higher priority)

    // k_thread_create(&button_thread_data, button_stack, STACK_SIZE,
    //                 button_thread, NULL, NULL, NULL,
    //                 3, 0, K_NO_WAIT);
    // k_thread_name_set(&button_thread_data, "button");

    k_thread_create(&display_thread_data, display_stack, STACK_SIZE,
                    display_thread, NULL, NULL, NULL,
                    4, 0, K_NO_WAIT);
    k_thread_name_set(&display_thread_data, "display");

    // k_thread_create(&wave_thread_data, wave_stack, STACK_SIZE,
    //                 wave_thread, NULL, NULL, NULL,
    //                 5, 0, K_NO_WAIT);
    // k_thread_name_set(&wave_thread_data, "wave");

    // k_thread_create(&ball_thread_data, ball_stack, STACK_SIZE,
    //                 ball_thread, NULL, NULL, NULL,
    //                 6, 0, K_NO_WAIT);
    // k_thread_name_set(&ball_thread_data, "ball");

    // k_thread_create(&breath_thread_data, breath_stack, STACK_SIZE,
    //                 breath_thread, NULL, NULL, NULL,
    //                 7, 0, K_NO_WAIT);
    // k_thread_name_set(&breath_thread_data, "breath");

    // k_thread_create(&twinkle_thread_data, twinkle_stack, STACK_SIZE,
    //                 twinkle_thread, NULL, NULL, NULL,
    //                 8, 0, K_NO_WAIT);
    // k_thread_name_set(&twinkle_thread_data, "twinkle");

    // k_thread_create(&priority_viz_thread_data, priority_viz_stack, STACK_SIZE,
    //                 priority_viz_thread, NULL, NULL, NULL,
    //                 5, 0, K_NO_WAIT);
    // k_thread_name_set(&priority_viz_thread_data, "priority_viz");

    k_thread_create(&rainbow_thread_data, rainbow_stack, STACK_SIZE,
                    rainbow_thread, NULL, NULL, NULL,
                    5, 0, K_NO_WAIT);
    k_thread_name_set(&rainbow_thread_data, "rainbow");

    LOG_INF("Rainbow Sweep running!");
    // LOG_INF("Demo running!");

    // Main thread monitors system
    while (1) {
        k_msleep(5000);
        LOG_INF("System running - %d threads active",
                CONFIG_NUM_PREEMPT_PRIORITIES);
    }
    

    return 0;
}