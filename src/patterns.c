#include "ws2812.h"
#include <math.h>
#include <stdlib.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Pattern 1: Scrolling wave
static int wave_offset = 0;

void pattern_wave(void) {
    for (int y = 0; y < MATRIX_HEIGHT; y++) {
        for (int x = 0; x < MATRIX_WIDTH; x++) {
            // Create sine wave pattern
            int wave_pos = (x + wave_offset) % MATRIX_WIDTH;
            float sine_val = sin(wave_pos * M_PI / 8.0);
            uint8_t brightness = (uint8_t)(128 + 127 * sine_val);

            // Red wave
            rgb_t color = {brightness, brightness, 0};
            ws2812_set_pixel(x, y, color);
        }
    }
    wave_offset = (wave_offset + 1) % MATRIX_WIDTH;
}

// Pattern 2: Bouncing ball
static float ball_x = 8.0, ball_y = 8.0;
static float ball_vx = 0.3, ball_vy = 0.2;

void pattern_ball(void) {
    // Update ball physics
    ball_x += ball_vx;
    ball_y += ball_vy;
    
    // Bounce off walls
    if (ball_x <= 1 || ball_x >= MATRIX_WIDTH - 2) {
        ball_vx = -ball_vx;
        ball_x = (ball_x <= 1) ? 1.1 : MATRIX_WIDTH - 2.1;
    }
    if (ball_y <= 1 || ball_y >= MATRIX_HEIGHT - 2) {
        ball_vy = -ball_vy;
        ball_y = (ball_y <= 1) ? 1.1 : MATRIX_HEIGHT - 2.1;
    }
    
    // Draw ball (3x3 blue)
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            int px = (int)(ball_x + 0.5) + dx;
            int py = (int)(ball_y + 0.5) + dy;

            if (px >= 0 && px < MATRIX_WIDTH && py >= 0 && py < MATRIX_HEIGHT) {
                rgb_t current = ws2812_get_pixel(px, py);
                // Add blue, keep other colors
                current.b = 255;
                ws2812_set_pixel(px, py, current);
            }
        }
    }
}

// Pattern 3: Breathing border
static uint8_t breath_brightness = 0;
static int8_t breath_direction = 1;

void pattern_breath(void) {
    // Update brightness
    breath_brightness += breath_direction * 5;
    
    if (breath_brightness >= 250) {
        breath_brightness = 250;
        breath_direction = -1;
    } else if (breath_brightness <= 5) {
        breath_brightness = 5;
        breath_direction = 1;
    }
    
    // Draw green breathing border
    // Top and bottom edges
    for (int x = 0; x < MATRIX_WIDTH; x++) {
        rgb_t current_top = ws2812_get_pixel(x, 0);
        rgb_t current_bottom = ws2812_get_pixel(x, MATRIX_HEIGHT - 1);

        current_top.g = breath_brightness;
        current_bottom.g = breath_brightness;

        ws2812_set_pixel(x, 0, current_top);
        ws2812_set_pixel(x, MATRIX_HEIGHT - 1, current_bottom);
    }
    
    // Left and right edges
    for (int y = 1; y < MATRIX_HEIGHT - 1; y++) {
        rgb_t current_left = ws2812_get_pixel(0, y);
        rgb_t current_right = ws2812_get_pixel(MATRIX_WIDTH - 1, y);
        
        current_left.g = breath_brightness;
        current_right.g = breath_brightness;
        
        ws2812_set_pixel(0, y, current_left);
        ws2812_set_pixel(MATRIX_WIDTH - 1, y, current_right);
    }
}

// Pattern 4: Random twinkle
void pattern_twinkle(void) {
    // Add random white sparkles
    for (int i = 0; i < 5; i++) {
        int x = rand() % MATRIX_WIDTH;
        int y = rand() % MATRIX_HEIGHT;
        uint8_t brightness = 128 + (rand() % 128);
        
        rgb_t current = ws2812_get_pixel(x, y);
        
        // Add white
        uint8_t add = brightness / 3;
        current.r = (current.r + add > 255) ? 255 : current.r + add;
        current.g = (current.g + add > 255) ? 255 : current.g + add;
        current.b = (current.b + add > 255) ? 255 : current.b + add;
        
        ws2812_set_pixel(x, y, current);
    }
    
    // Fade all pixels
    for (int y = 0; y < MATRIX_HEIGHT; y++) {
        for (int x = 0; x < MATRIX_WIDTH; x++) {
            rgb_t current = ws2812_get_pixel(x, y);
            
            current.r = (uint8_t)(current.r * 0.95);
            current.g = (uint8_t)(current.g * 0.95);
            current.b = (uint8_t)(current.b * 0.95);
            
            ws2812_set_pixel(x, y, current);
        }
    }
}

// ISR Pattern: Flash burst (yellow)
void pattern_flash_burst(void) {
    for (int frame = 0; frame < 10; frame++) {
        uint8_t brightness = 255 - (frame * 25);
        
        k_mutex_lock(&matrix_mutex, K_FOREVER);
        
        // Fill entire matrix with yellow
        for (int y = 0; y < MATRIX_HEIGHT; y++) {
            for (int x = 0; x < MATRIX_WIDTH; x++) {
                rgb_t yellow = {brightness, brightness, 0};
                ws2812_set_pixel(x, y, yellow);
            }
        }
        
        ws2812_update();
        k_mutex_unlock(&matrix_mutex);
        
        k_msleep(30);
    }
}

// Pattern 5: Priority Visualizer
// Each row represents a priority level, shows scheduling activity
#define NUM_PRIORITY_LEVELS 8
static uint8_t priority_activity[NUM_PRIORITY_LEVELS] = {0};
static uint8_t priority_colors[NUM_PRIORITY_LEVELS][3] = {
    {255, 0, 0},     // Priority 0: Red (highest)
    {255, 128, 0},   // Priority 1: Orange
    {255, 255, 0},   // Priority 2: Yellow
    {0, 255, 0},     // Priority 3: Green
    {0, 255, 255},   // Priority 4: Cyan
    {0, 0, 255},     // Priority 5: Blue
    {128, 0, 255},   // Priority 6: Purple
    {255, 0, 255}    // Priority 7: Magenta (lowest)
};

void pattern_priority_visualizer(void) {
    // Simulate thread activity at different priority levels
    // In a real implementation, this would read actual thread states

    // Decay existing activity
    for (int p = 0; p < NUM_PRIORITY_LEVELS; p++) {
        if (priority_activity[p] > 0) {
            priority_activity[p] = (uint8_t)(priority_activity[p] * 0.9);
        }
    }

    // Simulate random thread wakeups at different priorities
    // Higher priority threads are more active (lower numbers = higher priority)
    for (int p = 0; p < NUM_PRIORITY_LEVELS; p++) {
        // Higher priority = more likely to be active
        int threshold = 50 + (p * 30);  // 50, 80, 110, 140, etc.
        if ((rand() % 255) > threshold) {
            priority_activity[p] = 255;
        }
    }

    // Draw visualization: each row represents a priority
    // Two rows per priority level for 16-row matrix
    for (int p = 0; p < NUM_PRIORITY_LEVELS; p++) {
        int row1 = p * 2;
        int row2 = p * 2 + 1;

        if (row1 >= MATRIX_HEIGHT) break;

        // Create moving bar effect based on activity
        uint8_t brightness = priority_activity[p];
        int bar_length = (brightness * MATRIX_WIDTH) / 255;

        for (int x = 0; x < MATRIX_WIDTH; x++) {
            rgb_t color;
            if (x < bar_length) {
                // Active portion - full color
                color.r = (priority_colors[p][0] * brightness) / 255;
                color.g = (priority_colors[p][1] * brightness) / 255;
                color.b = (priority_colors[p][2] * brightness) / 255;
            } else {
                // Inactive portion - dim
                color.r = priority_colors[p][0] / 10;
                color.g = priority_colors[p][1] / 10;
                color.b = priority_colors[p][2] / 10;
            }

            ws2812_set_pixel(x, row1, color);
            if (row2 < MATRIX_HEIGHT) {
                ws2812_set_pixel(x, row2, color);
            }
        }
    }
}

// Pattern 6: Rainbow Sweep
// Simple rainbow gradient that scrolls horizontally
static int rainbow_offset = 0;

// Helper function: Convert HSV to RGB
// H: 0-255, S: 0-255, V: 0-255
rgb_t hsv_to_rgb(uint8_t h, uint8_t s, uint8_t v) {
    rgb_t rgb;
    uint8_t region, remainder, p, q, t;

    if (s == 0) {
        rgb.r = v;
        rgb.g = v;
        rgb.b = v;
        return rgb;
    }

    region = h / 43;
    remainder = (h - (region * 43)) * 6;

    p = (v * (255 - s)) >> 8;
    q = (v * (255 - ((s * remainder) >> 8))) >> 8;
    t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;

    switch (region) {
        case 0:
            rgb.r = v; rgb.g = t; rgb.b = p;
            break;
        case 1:
            rgb.r = q; rgb.g = v; rgb.b = p;
            break;
        case 2:
            rgb.r = p; rgb.g = v; rgb.b = t;
            break;
        case 3:
            rgb.r = p; rgb.g = q; rgb.b = v;
            break;
        case 4:
            rgb.r = t; rgb.g = p; rgb.b = v;
            break;
        default:
            rgb.r = v; rgb.g = p; rgb.b = q;
            break;
    }

    return rgb;
}

void pattern_rainbow_sweep(void) {
    // Fill entire matrix with rainbow gradient
    for (int y = 0; y < MATRIX_HEIGHT; y++) {
        for (int x = 0; x < MATRIX_WIDTH; x++) {
            // Calculate hue based on x position and offset
            // Each column gets a different color from the rainbow
            uint8_t hue = ((x * 255) / MATRIX_WIDTH + rainbow_offset) % 256;

            // Convert HSV to RGB (full saturation and value)
            rgb_t color = hsv_to_rgb(hue, 255, 255);

            ws2812_set_pixel(x, y, color);
        }
    }

    // Scroll the rainbow to the right
    rainbow_offset = (rainbow_offset + 2) % 256;
}