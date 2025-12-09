#ifndef QUADRANT_DEMO_H
#define QUADRANT_DEMO_H

#include <zephyr/kernel.h>
#include <zephyr/shell/shell.h>
#include "ws2812.h"

// Quadrant definitions for 16x16 matrix
#define QUAD_WIDTH  8
#define QUAD_HEIGHT 8

// Thread priorities (lower number = higher priority in Zephyr)
#define PRIORITY_QUAD1 5  // High priority - smooth wave
#define PRIORITY_QUAD2 3  // Medium-high - bouncing ball
#define PRIORITY_QUAD3 7  // Low priority - rotating spiral
#define PRIORITY_QUAD4 1  // Lowest - random sparkles
#define PRIORITY_MONITOR 4  // Thread state monitor
#define PRIORITY_DISPLAY 6  // Display update thread

// Thread states for visualization
typedef enum {
    THREAD_STATE_RUNNING = 0,   // Green
    THREAD_STATE_READY = 1,     // Yellow
    THREAD_STATE_SLEEPING = 2,  // Red
    THREAD_STATE_SUSPENDED = 3  // Blue
} thread_state_viz_t;

// Quadrant animation functions
void quadrant1_wave_animation(void);
void quadrant2_bounce_animation(void);
void quadrant3_spiral_animation(void);
void quadrant4_sparkle_animation(void);

// Thread state monitoring
void update_thread_state_border(void);
thread_state_viz_t get_thread_state(struct k_thread *thread);

// Thread entry points
void quad1_thread_entry(void *a, void *b, void *c);
void quad2_thread_entry(void *a, void *b, void *c);
void quad3_thread_entry(void *a, void *b, void *c);
void quad4_thread_entry(void *a, void *b, void *c);
void monitor_thread_entry(void *a, void *b, void *c);
void display_thread_entry(void *a, void *b, void *c);

// Shell command handlers
int cmd_thread_suspend(const struct shell *sh, size_t argc, char **argv);
int cmd_thread_resume(const struct shell *sh, size_t argc, char **argv);
int cmd_thread_stats(const struct shell *sh, size_t argc, char **argv);
int cmd_thread_boost(const struct shell *sh, size_t argc, char **argv);
int cmd_cpu_load(const struct shell *sh, size_t argc, char **argv);

// External thread references for shell commands
extern struct k_thread quad1_thread_data;
extern struct k_thread quad2_thread_data;
extern struct k_thread quad3_thread_data;
extern struct k_thread quad4_thread_data;

// Thread state tracking
extern volatile bool thread_running[4];
extern volatile uint32_t thread_cycle_count[4];

// CPU load simulation
extern volatile bool cpu_load_active;

// Main initialization function
void quadrant_demo_init(void);

#endif /* QUADRANT_DEMO_H */
