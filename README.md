# Zephyr RTOS Thread Priority Demo

A visual demonstration of Zephyr RTOS thread priority scheduling using a 16x16 WS2812 LED matrix with bouncing balls.

## Hardware

- SAM E54 Xplained Pro board
- 16x16 WS2812 LED matrix (256 LEDs)
- 5V power supply for LEDs

## What It Does

Four colored balls bounce in separate quadrants, each controlled by a thread with different priorities:

```
+-------------------+-------------------+
| Q1: Green Ball    | Q2: Cyan Ball     |
| Variable Priority | Priority 2 (HIGH) |
| (Button Control)  |                   |
+-------------------+-------------------+
| Q3: Yellow Ball   | Q4: Blue Ball     |
| Priority 6 (MED)  | Priority 8 (LOW)  |
+-------------------+-------------------+
```

### Thread Priorities

- **Q1 (Top-Left)**: Variable priority controlled by SW0 button
  - Cycles through: 2→4→6→8→2... (HIGH→MED→LOW→LOWEST)
  - Color changes with priority: CYAN→GREEN→YELLOW→RED
- **Q2 (Top-Right)**: Fixed priority 2 (HIGHEST) - Cyan ball
- **Q3 (Bottom-Left)**: Fixed priority 6 (MEDIUM) - Yellow ball
- **Q4 (Bottom-Right)**: Fixed priority 8 (LOWEST) - Blue ball

**Note**: In Zephyr, lower priority number = higher priority.

## Button Control

Press SW0 to cycle Q1's priority and watch:
- Ball color change instantly
- Animation smoothness change as thread priority changes
- Comparison against other quadrants' fixed priorities

## Build and Flash

```bash
west build -b same54_xpro
west flash
```

## Hardware Setup

1. Connect LED matrix data line to SPI MOSI
2. Connect 5V power supply to LED matrix
3. Connect ground between board and LEDs
4. Flash firmware

## Features Demonstrated

- Priority-based preemptive scheduling
- Dynamic priority changes
- Thread synchronization with mutex
- Visual feedback of scheduling behavior
- Real-time button input handling

## LED Quirks

- Physical LED #1 and #256 remain solid (bad LED compensation)
- LEDs may take 15-20 minutes to "settle" on first run (initialization artifact)
- Color order: RGB (not standard GRB)

## Project Structure

```
src/
├── main.c                    # Entry point
├── quadrant_simple_test.c    # 4-quadrant ball demo with priority control
├── quadrant_simple_test.h    # Demo header
├── ws2812.c                  # WS2812 LED driver
├── ws2812.h                  # Driver header
└── patterns.c                # Legacy patterns (not used)

prj.conf                      # Zephyr project configuration
```

## Configuration

Key settings in `prj.conf`:
- Thread monitoring enabled
- GPIO and SPI enabled
- Math library for ball physics

## Learning Outcomes

This demo teaches:
- How priority affects thread execution
- Visual debugging techniques
- Zephyr threading concepts
- Real-time scheduling behavior
- Mutex protection for shared resources

## Resources

- Zephyr Documentation: https://docs.zephyrproject.org
- Threading Guide: https://docs.zephyrproject.org/latest/kernel/services/threads/
