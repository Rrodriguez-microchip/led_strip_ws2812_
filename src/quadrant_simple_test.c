/*
 * Simple Single Quadrant Test - Based on working pattern_ball
 * Tests one bouncing ball in Quadrant 1 only
 * Now with SW0 button to cycle thread priorities!
 */

#include "quadrant_demo.h"
#include <zephyr/logging/log.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <math.h>

LOG_MODULE_REGISTER(quad_simple, LOG_LEVEL_INF);

// Button configuration - SW0 on SAM E54 Xplained Pro
#define SW0_NODE DT_ALIAS(sw0)
#if DT_NODE_HAS_STATUS(SW0_NODE, okay)
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(SW0_NODE, gpios);
static struct gpio_callback button_cb_data;

// Priority cycling - Zephyr uses lower numbers for higher priority
// We'll cycle through: 2 (highest), 4 (high), 6 (medium), 8 (low)
static const int priority_levels[] = {2, 4, 6, 8};
static const char *priority_names[] = {"HIGHEST", "HIGH", "MEDIUM", "LOW"};
static const float speed_levels[] = {1.5f, 1.0f, 0.8f, 1.2f};  // Match Q2, unique, Q3, Q4 speeds
static int current_priority_index = 1;  // Start at priority 4 (HIGH)
static volatile bool priority_changed = false;

// Button press handler
void button_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    // Cycle to next priority level
    current_priority_index = (current_priority_index + 1) % 4;
    priority_changed = true;
    LOG_INF("Button pressed! Switching to priority %s (%d)",
            priority_names[current_priority_index],
            priority_levels[current_priority_index]);
}
#endif

// Simple ball states for quadrant 1 and 2 (no trail buffer)
static float ball1_x = 4.0f;
static float ball1_y = 4.0f;
static float ball1_vx = 0.3f;
static float ball1_vy = 0.25f;
static float ball1_speed = 1.0f;  // Speed multiplier (1.0 = normal)

static float ball2_x = 4.0f;
static float ball2_y = 4.0f;
static float ball2_vx = 0.3f;
static float ball2_vy = 0.25f;
static float ball2_speed = 1.5f;  // 1.5x faster

static float ball3_x = 4.0f;
static float ball3_y = 4.0f;
static float ball3_vx = 0.3f;
static float ball3_vy = 0.25f;
static float ball3_speed = 0.8f;  // 0.8x speed (slower)

static float ball4_x = 4.0f;
static float ball4_y = 4.0f;
static float ball4_vx = 0.3f;
static float ball4_vy = 0.25f;
static float ball4_speed = 1.2f;  // 1.2x faster

// Quadrant colors (rgb_t struct is {g, r, b} order, but LEDs expect BGR order)
// To get specific colors with BGR LEDs: B=1st byte, G=2nd byte, R=3rd byte
// So rgb_t{g, r, b} where r displays as green, b displays as red, g displays as blue
static rgb_t get_priority_color(int priority_level) {
    // Q1 uses priority level 2/4/6/8 to get dynamic colors based on button presses
    // Q2, Q3, Q4 use fixed values (10, 11, 12) to get their static colors
    const rgb_t colors[] = {
        {0, 0, 0},         // 0: Not used
        {0, 0, 0},         // 1: Not used
        {32, 32, 0},       // 2: Q1 at HIGHEST - CYAN {g=32(blue), r=32(green), b=0(red)} = CYAN on display
        {0, 0, 0},         // 3: Not used
        {0, 64, 0},        // 4: Q1 at HIGH - GREEN {g=0(blue), r=64(green), b=0(red)} = GREEN on display
        {0, 0, 0},         // 5: Not used
        {0, 32, 32},       // 6: Q1 at MEDIUM - YELLOW {g=0(blue), r=32(green), b=32(red)} = YELLOW on display
        {0, 0, 0},         // 7: Not used
        {0, 0, 64},        // 8: Q1 at LOW - RED {g=0(blue), r=0(green), b=64(red)} = RED on display
        {0, 0, 0},         // 9: Not used
        {32, 32, 0},       // 10: Q2 - CYAN (matches Q1 highest) {g=32(blue), r=32(green), b=0(red)} = CYAN on display
        {0, 32, 32},       // 11: Q3 - YELLOW {g=0(blue), r=32(green), b=32(red)} = YELLOW on display
        {64, 0, 0},        // 12: Q4 - BLUE {g=64(blue), r=0(green), b=0(red)} = BLUE on display
    };

    if (priority_level >= 0 && priority_level < 13) {
        return colors[priority_level];
    }
    return colors[0];
}

void simple_quad1_animation(int priority) {
    // Update physics with speed multiplier
    ball1_x += ball1_vx * ball1_speed;
    ball1_y += ball1_vy * ball1_speed;

    // Bounce off quadrant walls (8x8, ball is 2x2 so max is 6.0)
    if (ball1_x <= 0.5f || ball1_x >= 6.5f) {
        ball1_vx = -ball1_vx;
        ball1_x = (ball1_x <= 0.5f) ? 0.6f : 6.4f;
    }
    if (ball1_y <= 0.5f || ball1_y >= 6.5f) {
        ball1_vy = -ball1_vy;
        ball1_y = (ball1_y <= 0.5f) ? 0.6f : 6.4f;
    }

    // Get color based on priority
    rgb_t ball_color = get_priority_color(priority);

    // Clear ONLY the old ball position (4 pixels) instead of entire quadrant
    // This prevents overwriting other quadrants when we call ws2812_update()
    static int last_x = -1, last_y = -1;
    if (last_x >= 0) {
        for (int dy = 0; dy <= 1; dy++) {
            for (int dx = 0; dx <= 1; dx++) {
                int px = last_x + dx;
                int py = last_y + dy;
                if (px >= 0 && px < 8 && py >= 0 && py < 8) {
                    ws2812_set_pixel(px, py, (rgb_t){0, 0, 0});
                }
            }
        }
    }
    last_x = (int)(ball1_x);
    last_y = (int)(ball1_y);

    // Draw ball (2x2) in quadrant 1
    for (int dy = 0; dy <= 1; dy++) {
        for (int dx = 0; dx <= 1; dx++) {
            int px = (int)(ball1_x) + dx;
            int py = (int)(ball1_y) + dy;

            if (px >= 0 && px < 8 && py >= 0 && py < 8) {
                ws2812_set_pixel(px, py, ball_color);
            }
        }
    }
}

void simple_quad2_animation(int priority) {
    // Update physics with speed multiplier
    ball2_x += ball2_vx * ball2_speed;
    ball2_y += ball2_vy * ball2_speed;

    // Bounce off quadrant walls (8x8, ball is 2x2 so max is 6.0)
    if (ball2_x <= 0.5f || ball2_x >= 6.5f) {
        ball2_vx = -ball2_vx;
        ball2_x = (ball2_x <= 0.5f) ? 0.6f : 6.4f;
    }
    if (ball2_y <= 0.5f || ball2_y >= 6.5f) {
        ball2_vy = -ball2_vy;
        ball2_y = (ball2_y <= 0.5f) ? 0.6f : 6.4f;
    }

    // Get color based on priority
    rgb_t ball_color = get_priority_color(priority);

    // Clear ONLY the old ball position (4 pixels) instead of entire quadrant
    // This prevents overwriting other quadrants when we call ws2812_update()
    static int last_x = -1, last_y = -1;
    if (last_x >= 0) {
        for (int dy = 0; dy <= 1; dy++) {
            for (int dx = 0; dx <= 1; dx++) {
                int px = last_x + dx + 8;
                int py = last_y + dy;
                if (px >= 8 && px < 16 && py >= 0 && py < 8) {
                    ws2812_set_pixel(px, py, (rgb_t){0, 0, 0});
                }
            }
        }
    }
    last_x = (int)(ball2_x);
    last_y = (int)(ball2_y);

    // Draw ball (2x2) in quadrant 2
    for (int dy = 0; dy <= 1; dy++) {
        for (int dx = 0; dx <= 1; dx++) {
            int px = (int)(ball2_x) + dx + 8;  // +8 offset for quadrant 2
            int py = (int)(ball2_y) + dy;

            if (px >= 8 && px < 16 && py >= 0 && py < 8) {
                ws2812_set_pixel(px, py, ball_color);
            }
        }
    }
}

void simple_quad3_animation(int priority) {
    // Update physics with speed multiplier
    ball3_x += ball3_vx * ball3_speed;
    ball3_y += ball3_vy * ball3_speed;

    // Bounce off quadrant walls (8x8, ball is 2x2 so max is 6.0)
    if (ball3_x <= 0.5f || ball3_x >= 6.5f) {
        ball3_vx = -ball3_vx;
        ball3_x = (ball3_x <= 0.5f) ? 0.6f : 6.4f;
    }
    if (ball3_y <= 0.5f || ball3_y >= 6.5f) {
        ball3_vy = -ball3_vy;
        ball3_y = (ball3_y <= 0.5f) ? 0.6f : 6.4f;
    }

    // Get color based on priority
    rgb_t ball_color = get_priority_color(priority);

    // Clear ONLY the old ball position (4 pixels) instead of entire quadrant
    // This prevents overwriting other quadrants when we call ws2812_update()
    static int last_x = -1, last_y = -1;
    if (last_x >= 0) {
        for (int dy = 0; dy <= 1; dy++) {
            for (int dx = 0; dx <= 1; dx++) {
                int px = last_x + dx;
                int py = last_y + dy + 8;
                if (px >= 0 && px < 8 && py >= 8 && py < 16) {
                    ws2812_set_pixel(px, py, (rgb_t){0, 0, 0});
                }
            }
        }
    }
    last_x = (int)(ball3_x);
    last_y = (int)(ball3_y);

    // Draw ball (2x2) in quadrant 3
    for (int dy = 0; dy <= 1; dy++) {
        for (int dx = 0; dx <= 1; dx++) {
            int px = (int)(ball3_x) + dx;  // No x offset for left side
            int py = (int)(ball3_y) + dy + 8;  // +8 offset for bottom half

            if (px >= 0 && px < 8 && py >= 8 && py < 16) {
                ws2812_set_pixel(px, py, ball_color);
            }
        }
    }
}

void simple_quad4_animation(int priority) {
    // Update physics with speed multiplier
    ball4_x += ball4_vx * ball4_speed;
    ball4_y += ball4_vy * ball4_speed;

    // Bounce off quadrant walls (8x8, ball is 2x2 so max is 6.0)
    if (ball4_x <= 0.5f || ball4_x >= 6.5f) {
        ball4_vx = -ball4_vx;
        ball4_x = (ball4_x <= 0.5f) ? 0.6f : 6.4f;
    }
    if (ball4_y <= 0.5f || ball4_y >= 6.5f) {
        ball4_vy = -ball4_vy;
        ball4_y = (ball4_y <= 0.5f) ? 0.6f : 6.4f;
    }

    // Get color based on priority
    rgb_t ball_color = get_priority_color(priority);

    // Clear ONLY the old ball position (4 pixels) instead of entire quadrant
    // This prevents overwriting other quadrants when we call ws2812_update()
    static int last_x = -1, last_y = -1;
    if (last_x >= 0) {
        for (int dy = 0; dy <= 1; dy++) {
            for (int dx = 0; dx <= 1; dx++) {
                int px = last_x + dx + 8;
                int py = last_y + dy + 8;
                if (px >= 8 && px < 16 && py >= 8 && py < 16) {
                    ws2812_set_pixel(px, py, (rgb_t){0, 0, 0});
                }
            }
        }
    }
    last_x = (int)(ball4_x);
    last_y = (int)(ball4_y);

    // Draw ball (2x2) in quadrant 4
    for (int dy = 0; dy <= 1; dy++) {
        for (int dx = 0; dx <= 1; dx++) {
            int px = (int)(ball4_x) + dx + 8;  // +8 x offset for right side
            int py = (int)(ball4_y) + dy + 8;  // +8 y offset for bottom half

            if (px >= 8 && px < 16 && py >= 8 && py < 16) {
                ws2812_set_pixel(px, py, ball_color);
            }
        }
    }
}

// Quadrant 1 thread
K_THREAD_STACK_DEFINE(simple_quad1_stack, 1024);
struct k_thread simple_quad1_thread_data;

void simple_quad1_thread_entry(void *a, void *b, void *c) {
    LOG_INF("Quadrant 1 thread started - priority demo ball");

    while (1) {
        // Check if priority changed and update this thread's priority AND speed
        if (priority_changed) {
            k_thread_priority_set(k_current_get(), priority_levels[current_priority_index]);
            ball1_speed = speed_levels[current_priority_index];  // Update speed to match priority

            // Copy position and velocity from matching quadrant
            switch (priority_levels[current_priority_index]) {
                case 2:  // Match Q2
                    ball1_x = ball2_x;
                    ball1_y = ball2_y;
                    ball1_vx = ball2_vx;
                    ball1_vy = ball2_vy;
                    break;
                case 6:  // Match Q3
                    ball1_x = ball3_x;
                    ball1_y = ball3_y;
                    ball1_vx = ball3_vx;
                    ball1_vy = ball3_vy;
                    break;
                case 8:  // Match Q4
                    ball1_x = ball4_x;
                    ball1_y = ball4_y;
                    ball1_vx = ball4_vx;
                    ball1_vy = ball4_vy;
                    break;
                // Priority 4 keeps its own unique pattern
            }

            priority_changed = false;  // Reset flag after applying
            LOG_INF("Q1 now at priority %s (%d), speed %.1fx - watch the ball color and speed change!",
                    priority_names[current_priority_index],
                    priority_levels[current_priority_index],
                    ball1_speed);
        }

        k_mutex_lock(&matrix_mutex, K_FOREVER);
        // Pass current priority level to animation for dynamic color
        simple_quad1_animation(priority_levels[current_priority_index]);
        // Display thread handles ws2812_update() now
        k_mutex_unlock(&matrix_mutex);
        k_msleep(50);  // 20 FPS - speed controlled by ball1_speed multiplier
    }
}

// Quadrant 2 thread
K_THREAD_STACK_DEFINE(simple_quad2_stack, 1024);
struct k_thread simple_quad2_thread_data;

void simple_quad2_thread_entry(void *a, void *b, void *c) {
    LOG_INF("Quadrant 2 thread started - fixed priority (highest=2)");

    while (1) {
        k_mutex_lock(&matrix_mutex, K_FOREVER);
        simple_quad2_animation(10);  // Fixed cyan color (index 10)
        // Display thread handles ws2812_update() now
        k_mutex_unlock(&matrix_mutex);
        k_msleep(50);  // 20 FPS - speed controlled by ball2_speed multiplier
    }
}

// Quadrant 3 thread
K_THREAD_STACK_DEFINE(simple_quad3_stack, 1024);
struct k_thread simple_quad3_thread_data;

void simple_quad3_thread_entry(void *a, void *b, void *c) {
    LOG_INF("Quadrant 3 thread started - fixed priority (medium=6)");

    while (1) {
        k_mutex_lock(&matrix_mutex, K_FOREVER);
        simple_quad3_animation(11);  // Fixed yellow color (index 11)
        // Display thread handles ws2812_update() now
        k_mutex_unlock(&matrix_mutex);
        k_msleep(50);  // 20 FPS - speed controlled by ball3_speed multiplier
    }
}

// Quadrant 4 thread
K_THREAD_STACK_DEFINE(simple_quad4_stack, 1024);
struct k_thread simple_quad4_thread_data;

void simple_quad4_thread_entry(void *a, void *b, void *c) {
    LOG_INF("Quadrant 4 thread started - fixed priority (lowest=8)");

    while (1) {
        k_mutex_lock(&matrix_mutex, K_FOREVER);
        simple_quad4_animation(12);  // Fixed blue color (index 12)
        // Display thread handles ws2812_update() now
        k_mutex_unlock(&matrix_mutex);
        k_msleep(50);  // 20 FPS - speed controlled by ball4_speed multiplier
    }
}

// Display thread - handles all LED refreshes at fixed rate
K_THREAD_STACK_DEFINE(display_stack, 1024);
struct k_thread display_thread_data;

void display_thread_entry(void *a, void *b, void *c) {
    LOG_INF("Display thread started - 50 FPS refresh");

    while (1) {
        k_mutex_lock(&matrix_mutex, K_FOREVER);
        ws2812_update();  // Refresh LEDs with current buffer contents
        k_mutex_unlock(&matrix_mutex);
        k_msleep(20);  // 50 FPS (20ms per frame)
    }
}

void simple_test_init(void) {
    LOG_INF("===========================================");
    LOG_INF("  Zephyr Thread Priority Demo");
    LOG_INF("===========================================");
    LOG_INF("Q1 (Top-Left):     VARIABLE priority (button controlled)");
    LOG_INF("                   Color changes with priority:");
    LOG_INF("                   CYAN(2) GREEN(4) YELLOW(6) RED(8)");
    LOG_INF("Q2 (Top-Right):    HIGHEST priority (2) - CYAN");
    LOG_INF("Q3 (Bottom-Left):  MEDIUM priority (6) - YELLOW");
    LOG_INF("Q4 (Bottom-Right): LOWEST priority (8) - BLUE");
    LOG_INF("");
    LOG_INF("Press SW0 to cycle Q1 priority and color:");
    LOG_INF("  CYAN(2) -> GREEN(4) -> YELLOW(6) -> RED(8)");
    LOG_INF("Watch Q1 change color and smoothness!");
    LOG_INF("===========================================");

#if DT_NODE_HAS_STATUS(SW0_NODE, okay)
    // Initialize button
    int ret;

    if (!gpio_is_ready_dt(&button)) {
        LOG_ERR("Button device not ready");
    } else {
        ret = gpio_pin_configure_dt(&button, GPIO_INPUT);
        if (ret != 0) {
            LOG_ERR("Error %d: failed to configure button pin", ret);
        } else {
            ret = gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_TO_ACTIVE);
            if (ret != 0) {
                LOG_ERR("Error %d: failed to configure button interrupt", ret);
            } else {
                gpio_init_callback(&button_cb_data, button_pressed, BIT(button.pin));
                gpio_add_callback(button.port, &button_cb_data);
                LOG_INF("SW0 button configured successfully!");
            }
        }
    }
#else
    LOG_WRN("SW0 button not available in device tree");
#endif

    // Clear entire matrix first
    k_mutex_lock(&matrix_mutex, K_FOREVER);
    ws2812_clear();
    k_mutex_unlock(&matrix_mutex);

    // Create quadrant 1 thread - Variable priority (starts at HIGH = 4)
    k_thread_create(&simple_quad1_thread_data, simple_quad1_stack, 1024,
                    simple_quad1_thread_entry, NULL, NULL, NULL,
                    4, 0, K_NO_WAIT);  // Start at HIGH priority
    k_thread_name_set(&simple_quad1_thread_data, "quad1");

    // Create quadrant 2 thread - Fixed HIGH priority (2)
    k_thread_create(&simple_quad2_thread_data, simple_quad2_stack, 1024,
                    simple_quad2_thread_entry, NULL, NULL, NULL,
                    2, 0, K_NO_WAIT);  // Highest - always fast
    k_thread_name_set(&simple_quad2_thread_data, "quad2");

    // Create quadrant 3 thread - Fixed MEDIUM priority (6)
    k_thread_create(&simple_quad3_thread_data, simple_quad3_stack, 1024,
                    simple_quad3_thread_entry, NULL, NULL, NULL,
                    6, 0, K_NO_WAIT);  // Medium speed
    k_thread_name_set(&simple_quad3_thread_data, "quad3");

    // Create quadrant 4 thread - Fixed LOW priority (8)
    k_thread_create(&simple_quad4_thread_data, simple_quad4_stack, 1024,
                    simple_quad4_thread_entry, NULL, NULL, NULL,
                    8, 0, K_NO_WAIT);  // Lowest - slowest
    k_thread_name_set(&simple_quad4_thread_data, "quad4");

    // Create display thread - HIGHEST priority (1) so it always gets to refresh
    k_thread_create(&display_thread_data, display_stack, 1024,
                    display_thread_entry, NULL, NULL, NULL,
                    1, 0, K_NO_WAIT);  // Priority 1 - highest, ensures consistent refresh
    k_thread_name_set(&display_thread_data, "display");

    LOG_INF("Simple test running - 4 animation threads + 1 display thread!");
}
