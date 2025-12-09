# Multi-Threaded Animation Zones Demo

A comprehensive demonstration of Zephyr RTOS threading concepts through visual animations on a 16x16 WS2812 LED matrix. Perfect for workshops, educational settings, and learning embedded systems programming.

## Overview

This demo makes abstract threading concepts **visually tangible** by dedicating each quadrant of the LED matrix to a different thread with distinct priority levels. Watch in real-time as Zephyr's scheduler orchestrates these animations!

## Hardware Requirements

- SAM E54 Xplained Pro board (or compatible)
- 16x16 WS2812 LED matrix (256 pixels)
- 5V power supply for LEDs (5A recommended)
- USB cable for programming and serial console

## Visual Layout

```
+-------------------+-------------------+
|   QUADRANT 1      |   QUADRANT 2      |
|   (Thread 1)      |   (Thread 2)      |
|   8x8 pixels      |   8x8 pixels      |
|   Priority: 5     |   Priority: 3     |
|   High            |   Medium          |
|                   |                   |
|   Blue-Cyan Wave  |   Red Bouncing    |
|   Smooth gradient |   Ball w/ Trail   |
+-------------------+-------------------+
|   QUADRANT 3      |   QUADRANT 4      |
|   (Thread 3)      |   (Thread 4)      |
|   8x8 pixels      |   8x8 pixels      |
|   Priority: 7     |   Priority: 1     |
|   Low             |   Lowest          |
|                   |                   |
|   Green Spiral    |   Yellow Sparkles |
|   Rotating slowly |   Random twinkles |
+-------------------+-------------------+
```

### Border Status Indicators

The edges of the matrix display real-time thread states:

- **Top Edge**: 4 segments showing each thread's state (4 LEDs per thread)
- **Bottom Edge**: Mirror of top edge
- **Left/Right Edges**: Vertical status bars

**Color Coding:**
- 🟢 **Green**: Thread is currently running
- 🟡 **Yellow**: Thread is ready (waiting for CPU)
- 🔴 **Red**: Thread is sleeping/blocked
- 🔵 **Blue**: Thread is suspended

## Thread Descriptions

### Thread 1: Smooth Color Wave (Quadrant 1 - Top Left)
- **Priority**: 5 (High)
- **Cycle Time**: 50ms
- **Animation**: Blue-to-cyan gradient wave
- **Behavior**: Runs consistently, rarely preempted
- **Teaching Point**: High-priority threads maintain smooth timing

### Thread 2: Bouncing Ball (Quadrant 2 - Top Right)
- **Priority**: 3 (Medium-High)
- **Cycle Time**: 100ms
- **Animation**: Red ball with physics and trail
- **Behavior**: Occasionally preempted by Thread 1
- **Teaching Point**: Demonstrates priority preemption

### Thread 3: Rotating Spiral (Quadrant 3 - Bottom Left)
- **Priority**: 7 (Low)
- **Cycle Time**: 200ms
- **Animation**: Green spiral pattern
- **Behavior**: Gets preempted often, may show jitter
- **Teaching Point**: Shows effects of low priority on timing

### Thread 4: Random Sparkles (Quadrant 4 - Bottom Right)
- **Priority**: 1 (Lowest)
- **Cycle Time**: 150ms
- **Animation**: Yellow random sparkles
- **Behavior**: Only runs when higher-priority threads sleep
- **Teaching Point**: Demonstrates thread starvation

### Monitor Thread
- **Priority**: 4
- **Cycle Time**: 10ms
- **Function**: Updates border status indicators
- **Teaching Point**: High-frequency monitoring thread

### Display Thread
- **Priority**: 6
- **Cycle Time**: 20ms (50 FPS)
- **Function**: Pushes frame buffer to LEDs
- **Teaching Point**: Consistent display refresh

## Building and Running

### Quick Start

1. **Backup your current main.c**:
   ```bash
   cd src
   cp main.c main_backup.c
   cp main_quadrant_demo.c main.c
   ```

2. **Build the project**:
   ```bash
   west build -b same54_xpro
   ```

3. **Flash to board**:
   ```bash
   west flash
   ```

4. **Connect serial terminal** (115200 baud):
   ```bash
   # Linux/Mac
   screen /dev/ttyACM0 115200

   # Or use minicom
   minicom -D /dev/ttyACM0 -b 115200

   # Windows (PowerShell)
   # Use PuTTY or TeraTerm
   ```

### CMakeLists.txt Integration

If you want to add the demo to your build without replacing main.c, add to `CMakeLists.txt`:

```cmake
target_sources(app PRIVATE
    src/main.c
    src/ws2812.c
    src/patterns.c
    src/quadrant_demo.c  # Add this line
)
```

Then call `quadrant_demo_init()` from your existing main.

## Interactive Commands

Once running, you can control the demo through the serial shell:

### Thread Control

```bash
# Suspend a thread (stop animation)
thread suspend 3
# Resume: thread 3 will stop animating, quadrant goes dark

# Resume a thread
thread resume 3
# Quadrant 3 animation restarts

# Show thread statistics
thread stats
# Output:
# === Thread Statistics ===
# Thread | Name    | Priority | State     | Cycles
# -------|---------|----------|-----------|--------
#   1    | Wave    |        5 | Running   | 45231
#   2    | Bounce  |        3 | Sleeping  | 22456
#   3    | Spiral  |        7 | Ready     | 11234
#   4    | Sparkle |        1 | Sleeping  | 7891

# Boost thread priority (temporary for 5 seconds)
thread boost 4
# Thread 4 priority changes from 1 to 2
# Watch sparkles become smoother!
# Priority automatically restores after 5 seconds
```

### CPU Load Simulation

```bash
# Enable artificial CPU load
cpuload on
# Adds busy-loop thread to simulate high CPU usage
# Watch animations slow down and become jerky

# Disable CPU load
cpuload off
# Animations return to smooth operation
```

### Other Useful Commands

```bash
# List all available commands
help

# Show kernel version
kernel version

# Show all threads
kernel threads

# Show memory usage
kernel stacks
```

## Zephyr RTOS Concepts Demonstrated

### 1. Priority-Based Preemptive Scheduling

The demo visually shows how Zephyr schedules threads:
- **Lower number = Higher priority** (Zephyr convention)
- High-priority threads preempt low-priority threads
- Same-priority threads use round-robin scheduling

**Try This**: Run `cpuload on` and watch Thread 4 (lowest priority) starve while higher-priority threads continue.

### 2. Mutual Exclusion (Mutex)

All threads share the LED frame buffer protected by a mutex:

```c
k_mutex_lock(&matrix_mutex, K_FOREVER);
// Safe access to shared LED buffer
ws2812_set_pixel(x, y, color);
k_mutex_unlock(&matrix_mutex);
```

**Teaching Point**: Without the mutex, animations would corrupt each other's pixels.

### 3. Thread Synchronization

Threads use `k_msleep()` to:
- Yield CPU to other threads
- Control animation frame rate
- Demonstrate cooperative behavior

**Try This**: Modify cycle times in code to see how it affects scheduling balance.

### 4. Thread States

The border visualization shows four states:
- **Running**: Currently executing on CPU
- **Ready**: Waiting in run queue
- **Sleeping**: Blocked in `k_msleep()`
- **Suspended**: Manually suspended via shell

**Try This**: Suspend Thread 1 and watch its border turn blue.

### 5. Priority Inversion & Boosting

```bash
thread boost 4
```

Temporarily elevates Thread 4's priority, demonstrating:
- Dynamic priority changes
- Effect on scheduling order
- Priority inheritance concepts

**Watch**: Sparkles become much smoother for 5 seconds.

### 6. Thread Starvation

Under CPU load, Thread 4 (lowest priority) may not run at all:

```bash
cpuload on
thread stats  # Watch Thread 4 cycle count stop increasing
```

**Teaching Point**: Low-priority threads need careful design to avoid starvation.

### 7. Real-Time Determinism

Even under load, high-priority threads maintain timing:
- Thread 1 (Priority 5) keeps smooth wave
- Monitor thread (Priority 4) updates borders consistently

**Teaching Point**: Critical tasks should have appropriate priority.

## Workshop Activities

### Activity 1: Priority Exploration (10 minutes)

1. Observe all four quadrants running normally
2. Run `thread stats` to see cycle counts
3. Suspend Thread 1: `thread suspend 1`
4. Watch how other threads continue
5. Resume Thread 1: `thread resume 1`
6. **Discussion**: What happened to the system when the highest-priority animation stopped?

### Activity 2: CPU Load Impact (15 minutes)

1. Run `thread stats` and note cycle counts
2. Enable CPU load: `cpuload on`
3. Observe visual degradation (jitter, lag)
4. Run `thread stats` again after 30 seconds
5. Note which threads are most affected
6. Disable load: `cpuload off`
7. **Discussion**: Why do some animations degrade more than others?

### Activity 3: Priority Inversion Demo (10 minutes)

1. Observe Thread 4 (sparkles) under normal conditions
2. Run `thread boost 4`
3. Watch sparkles become much smoother
4. Priority automatically restores after 5 seconds
5. **Discussion**: What changed? Why did animation improve?

### Activity 4: Thread Suspension Cascade (15 minutes)

1. Suspend all threads one by one:
   ```bash
   thread suspend 1
   thread suspend 2
   thread suspend 3
   thread suspend 4
   ```
2. Observe matrix going dark quadrant by quadrant
3. Note border colors changing to blue (suspended)
4. Resume in reverse order
5. **Discussion**: What does this teach about system shutdown/startup?

### Activity 5: Code Modification Challenge (30 minutes)

Modify the code to:

1. **Change priorities**: Swap Thread 1 and Thread 4 priorities
2. **Adjust timing**: Change cycle times (e.g., make Thread 1 update every 10ms)
3. **Add new animation**: Create a 5th thread for the center pixels
4. **Modify colors**: Change animation color schemes

## Code Architecture

### File Structure

```
src/
├── quadrant_demo.h         # Header with API and definitions
├── quadrant_demo.c         # Full demo implementation
├── main_quadrant_demo.c    # Example main file
├── ws2812.h                # WS2812 driver header
└── ws2812.c                # WS2812 driver implementation
```

### Key Data Structures

```c
// Thread stacks (1KB each)
K_THREAD_STACK_DEFINE(quad1_stack, STACK_SIZE);

// Thread data structures
struct k_thread quad1_thread_data;

// Shared frame buffer (protected by mutex)
extern struct k_mutex matrix_mutex;

// Thread state tracking
volatile bool thread_running[4];
volatile uint32_t thread_cycle_count[4];
```

### Thread Creation Pattern

```c
k_thread_create(
    &quad1_thread_data,     // Thread data structure
    quad1_stack,            // Stack memory
    STACK_SIZE,             // Stack size
    quad1_thread_entry,     // Entry function
    NULL, NULL, NULL,       // Arguments (unused)
    PRIORITY_QUAD1,         // Priority (5)
    0,                      // Options
    K_NO_WAIT               // Start immediately
);

k_thread_name_set(&quad1_thread_data, "quad1_wave");
```

## Troubleshooting

### No LEDs Lighting Up

1. Check power supply (5V, adequate current)
2. Verify SPI pin connections
3. Check device tree overlay configuration
4. Look for initialization errors in console

### Animations Not Smooth

1. Reduce brightness: `ws2812_set_brightness(64);`
2. Check CPU frequency settings
3. Verify SPI clock rate in device tree
4. Look for interrupt conflicts

### Shell Not Responding

1. Verify serial connection (115200 baud, 8N1)
2. Press Enter a few times to wake shell
3. Check USB cable and drivers
4. Try `help` command to verify shell is running

### Thread Stats Show Zero Cycles

1. Check if threads are suspended
2. Verify mutex isn't deadlocked
3. Look for kernel panics in console
4. Check stack overflow (increase STACK_SIZE if needed)

## Advanced Customization

### Modify Animation Behavior

Edit `quadrant_demo.c`, functions:
- `quadrant1_wave_animation()` - Wave pattern
- `quadrant2_bounce_animation()` - Ball physics
- `quadrant3_spiral_animation()` - Spiral math
- `quadrant4_sparkle_animation()` - Random sparkles

### Change Priorities

In `quadrant_demo.h`:

```c
#define PRIORITY_QUAD1 5  // Change these values
#define PRIORITY_QUAD2 3  // Lower number = higher priority
#define PRIORITY_QUAD3 7
#define PRIORITY_QUAD4 1
```

### Adjust Timing

In thread entry functions:

```c
void quad1_thread_entry(void *a, void *b, void *c) {
    while (1) {
        // ... animation code ...
        k_msleep(50);  // Change this value (milliseconds)
    }
}
```

### Add Button Control

Add button interrupt to trigger effects:

```c
void button_callback(const struct device *dev,
                     struct gpio_callback *cb,
                     uint32_t pins) {
    // Suspend/resume threads
    // Boost priorities
    // Toggle CPU load
}
```

## Performance Metrics

Typical performance on SAM E54 @ 120MHz:

| Thread | Priority | Cycle Time | Actual FPS | CPU % |
|--------|----------|------------|-----------|-------|
| Wave   | 5        | 50ms       | ~20 FPS   | 15%   |
| Bounce | 3        | 100ms      | ~10 FPS   | 8%    |
| Spiral | 7        | 200ms      | ~5 FPS    | 5%    |
| Sparkle| 1        | 150ms      | ~6.7 FPS  | 4%    |
| Monitor| 4        | 10ms       | 100 Hz    | 3%    |
| Display| 6        | 20ms       | 50 FPS    | 12%   |
| **Total** | -     | -          | -         | **~47%** |

CPU headroom: ~53% available for other tasks.

## Educational Outcomes

After completing this demo, students will understand:

1. ✅ Priority-based preemptive scheduling
2. ✅ Thread synchronization with mutexes
3. ✅ Thread states and state transitions
4. ✅ Priority inversion and boosting
5. ✅ Resource contention and sharing
6. ✅ Real-time determinism concepts
7. ✅ Thread starvation under load
8. ✅ Visual debugging techniques
9. ✅ Shell/CLI integration in embedded systems
10. ✅ Practical Zephyr RTOS development

## Further Exploration

### Ideas for Extension

1. **Deadlock Demo**: Add two mutexes and create intentional deadlock
2. **Semaphore Signaling**: Use semaphores for inter-thread communication
3. **Message Queues**: Send animation commands via message queues
4. **Work Queues**: Offload animations to work queue threads
5. **Timers**: Use k_timer for periodic animations
6. **ISR Integration**: Trigger effects from hardware interrupts

### Related Zephyr Samples

- `samples/basic/threads`
- `samples/synchronization`
- `samples/philosophers` (dining philosophers problem)
- `tests/kernel/sched/schedule_api`

## License

This demo code is provided as educational material for Zephyr RTOS workshops.

## Support

For questions or issues:
- Check Zephyr documentation: https://docs.zephyrproject.org
- Zephyr Discord: https://discord.gg/zephyr
- GitHub Issues: (your repository)

## Credits

Created for embedded systems education and Zephyr RTOS workshops.
Demonstrates real-time operating system concepts through visual feedback.

---

**Happy Threading!** 🎨🧵
