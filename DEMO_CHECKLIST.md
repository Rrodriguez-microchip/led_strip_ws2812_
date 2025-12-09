# Multi-Threaded Quadrant Demo - Verification Checklist

Use this checklist to verify that your demo is set up correctly and ready for use.

## 📦 Files Checklist

### Source Code Files
- [x] `src/quadrant_demo.h` - Demo header file
- [x] `src/quadrant_demo.c` - Demo implementation (~600 lines)
- [x] `src/main_quadrant_demo.c` - Example main file

### Documentation Files
- [x] `README_QUADRANT_DEMO.md` - Main README
- [x] `QUICK_START.md` - 5-minute setup guide
- [x] `QUADRANT_DEMO.md` - Technical documentation
- [x] `WORKSHOP_GUIDE.md` - Facilitator's guide
- [x] `ARCHITECTURE.md` - System architecture
- [x] `DEMO_FILES_SUMMARY.md` - File organization
- [x] `CMakeLists_quadrant_demo.txt` - Build guide
- [x] `DEMO_CHECKLIST.md` - This file

### Configuration Files
- [x] `prj.conf` - Updated with shell and stats configs

## ⚙️ Configuration Checklist

### prj.conf Settings

Verify these lines are in `prj.conf`:

```ini
# Shell support
- [ ] CONFIG_SHELL=y
- [ ] CONFIG_SHELL_BACKEND_SERIAL=y
- [ ] CONFIG_SHELL_CMD_BUFF_SIZE=256
- [ ] CONFIG_SHELL_PRINTF_BUFF_SIZE=256

# Thread runtime statistics
- [ ] CONFIG_THREAD_RUNTIME_STATS=y
- [ ] CONFIG_SCHED_THREAD_USAGE=y

# Thread monitoring
- [ ] CONFIG_THREAD_MONITOR=y
- [ ] CONFIG_THREAD_NAME=y
- [ ] CONFIG_THREAD_STACK_INFO=y

# Math library
- [ ] CONFIG_NEWLIB_LIBC=y
- [ ] CONFIG_NEWLIB_LIBC_FLOAT_PRINTF=y

# Console
- [ ] CONFIG_CONSOLE=y
- [ ] CONFIG_UART_CONSOLE=y

# Other required configs
- [ ] CONFIG_GPIO=y
- [ ] CONFIG_SPI=y
- [ ] CONFIG_LED_STRIP=y
- [ ] CONFIG_NUM_PREEMPT_PRIORITIES=10
```

### CMakeLists.txt

Verify `CMakeLists.txt` includes:

```cmake
- [ ] target_sources includes src/quadrant_demo.c
```

Example:
```cmake
target_sources(app PRIVATE
    src/main.c
    src/ws2812.c
    src/patterns.c
    src/quadrant_demo.c    # This line
)
```

## 🔨 Build Checklist

### Before Building

```bash
- [ ] Backed up original main.c
      cp src/main.c src/main_backup.c

- [ ] Copied demo main to main.c
      cp src/main_quadrant_demo.c src/main.c

- [ ] Verified all source files present
      ls src/quadrant_demo.*
      ls src/main_quadrant_demo.c
```

### Build Process

```bash
- [ ] Clean build succeeds
      west build -t pristine
      west build -b same54_xpro

- [ ] No compilation errors
- [ ] No warnings about undefined references
- [ ] Build output shows quadrant_demo.c compiled
```

### Expected Build Output

Look for these in build output:
```
[x] Building quadrant_demo.c
[x] Linking app
[x] Memory regions: FLASH, SRAM
[x] Total image size: ~XXX KB
```

## 🔌 Hardware Checklist

### Before Powering On

```
- [ ] 16x16 WS2812 LED matrix connected
- [ ] Data pin connected to correct SPI MOSI pin
- [ ] Ground connected between board and LEDs
- [ ] 5V power supply connected to LED matrix
- [ ] Power supply rated for adequate current (5A min)
- [ ] Board USB cable connected for programming
- [ ] Serial terminal cable/USB connected
```

### Power-On Sequence

```
1. [ ] Connect USB to board (programming)
2. [ ] Flash firmware: west flash
3. [ ] Connect 5V power to LED matrix
4. [ ] Power on LED supply
5. [ ] LEDs should start animating immediately
```

## 🖥️ Serial Terminal Checklist

### Terminal Configuration

```
- [ ] Baud rate: 115200
- [ ] Data bits: 8
- [ ] Stop bits: 1
- [ ] Parity: None
- [ ] Flow control: None
```

### Connection

```bash
# Linux/Mac
- [ ] Find device: ls /dev/ttyACM* or /dev/ttyUSB*
- [ ] Connect: screen /dev/ttyACM0 115200
      OR: minicom -D /dev/ttyACM0 -b 115200

# Windows
- [ ] Find COM port in Device Manager
- [ ] Use PuTTY/TeraTerm at 115200 baud
```

### Expected Console Output

Look for these messages:
```
[x] "===========================================
[x] "Multi-Threaded Quadrant Animation Demo"
[x] "WS2812 initialized successfully"
[x] "Quadrant 1 thread started (Priority 5 - High)"
[x] "Quadrant 2 thread started (Priority 3 - Medium)"
[x] "Quadrant 3 thread started (Priority 7 - Low)"
[x] "Quadrant 4 thread started (Priority 1 - Lowest)"
[x] "Monitor thread started (Priority 4)"
[x] "Display thread started (Priority 6)"
[x] "All threads created successfully!"
[x] "Shell Commands:" (list of commands)
```

## 🎨 Visual Verification Checklist

### Quadrant Animations

```
Quadrant 1 (Top-Left):
- [ ] Shows blue-cyan wave pattern
- [ ] Wave scrolls smoothly from left to right
- [ ] Animation is smooth (no jitter)

Quadrant 2 (Top-Right):
- [ ] Shows red bouncing ball
- [ ] Ball has motion trail
- [ ] Ball bounces off quadrant edges

Quadrant 3 (Bottom-Left):
- [ ] Shows green spiral pattern
- [ ] Spiral rotates slowly
- [ ] May show slight jitter (lowest priority)

Quadrant 4 (Bottom-Right):
- [ ] Shows yellow sparkles
- [ ] Random pixels appear and fade
- [ ] Sparkles twinkle randomly
```

### Border Status Indicators

```
- [ ] Top edge shows 4 colored segments
- [ ] Bottom edge mirrors top edge
- [ ] Left edge shows vertical status bars
- [ ] Right edge shows vertical status bars
- [ ] Colors change dynamically (green/yellow/red/blue)
```

## 🎮 Shell Command Verification

### Test Each Command

```bash
help
- [ ] Lists available commands
- [ ] Shows "thread" subcommands
- [ ] Shows "cpuload" command

thread stats
- [ ] Displays thread table
- [ ] Shows all 4 threads
- [ ] Shows priorities (5, 3, 7, 1)
- [ ] Shows cycle counts (increasing numbers)
- [ ] Shows thread states

thread suspend 3
- [ ] Quadrant 3 animation stops
- [ ] Quadrant 3 goes dark
- [ ] Border shows blue for Thread 3
- [ ] Console shows "Thread 3 suspended"

thread resume 3
- [ ] Quadrant 3 animation resumes
- [ ] Green spiral appears again
- [ ] Border returns to normal colors
- [ ] Console shows "Thread 3 resumed"

thread boost 4
- [ ] Sparkles become noticeably smoother
- [ ] Console shows priority boost message
- [ ] After 5 seconds, console shows restore message
- [ ] Sparkles return to previous behavior

cpuload on
- [ ] Console shows "CPU load simulation enabled"
- [ ] Animations slow down or become jerky
- [ ] Thread 1 (high priority) stays relatively smooth
- [ ] Thread 4 (low priority) shows most degradation

cpuload off
- [ ] Console shows "CPU load simulation disabled"
- [ ] Animations return to normal smoothness
```

## 🧪 Functional Tests

### Test 1: Thread Suspension Cascade

```bash
thread suspend 1
- [ ] Quadrant 1 stops, shows border blue

thread suspend 2
- [ ] Quadrant 2 stops, shows border blue

thread suspend 3
- [ ] Quadrant 3 stops, shows border blue

thread suspend 4
- [ ] Quadrant 4 stops, shows border blue
- [ ] Entire matrix dark except borders

thread resume 4
thread resume 3
thread resume 2
thread resume 1
- [ ] All quadrants resume in order
- [ ] Borders return to dynamic colors
```

### Test 2: Priority Effects Under Load

```bash
thread stats  # Note initial cycle counts
cpuload on
# Wait 30 seconds
thread stats
- [ ] Thread 1 cycle count increased significantly
- [ ] Thread 4 cycle count barely increased
- [ ] Demonstrates priority scheduling
```

### Test 3: Real-Time Performance

```bash
# All animations running normally
- [ ] Thread 1 wave: Smooth, consistent
- [ ] Thread 2 ball: Smooth bouncing
- [ ] Thread 3 spiral: Some jitter acceptable
- [ ] Thread 4 sparkles: Occasional gaps acceptable
- [ ] Display refresh: No visible flicker
- [ ] Border updates: Responsive to state changes
```

## 📊 Performance Metrics

### Expected CPU Utilization

```
Normal operation (cpuload off):
- [ ] Total CPU usage: ~40-50%
- [ ] System remains responsive
- [ ] No stack overflows in console

With CPU load (cpuload on):
- [ ] Total CPU usage: 90-100%
- [ ] Low-priority threads affected
- [ ] High-priority threads maintain performance
```

### Expected Timing

```
- [ ] Display refresh: 50 FPS (20ms cycle)
- [ ] Thread 1 updates: ~20 FPS (50ms cycle)
- [ ] Thread 2 updates: ~10 FPS (100ms cycle)
- [ ] Thread 3 updates: ~5 FPS (200ms cycle)
- [ ] Thread 4 updates: ~6-7 FPS (150ms cycle)
```

## ⚠️ Common Issues Checklist

### Issue: LEDs not lighting

```
Check:
- [ ] Power supply turned on
- [ ] Power LED on LED strip lit
- [ ] Voltage at LED strip: ~5V DC
- [ ] Data signal connected to SPI MOSI
- [ ] Ground connected
- [ ] Console shows "WS2812 initialized successfully"
```

### Issue: Only some LEDs working

```
Check:
- [ ] First LED receives signal
- [ ] Adequate power supply current
- [ ] No broken LEDs in chain
- [ ] Data line not damaged
```

### Issue: Shell not responding

```
Check:
- [ ] Correct COM port selected
- [ ] Baud rate: 115200
- [ ] USB cable supports data
- [ ] Press Enter 2-3 times
- [ ] Try typing "help" + Enter
```

### Issue: Compilation errors

```
Check:
- [ ] quadrant_demo.c in CMakeLists.txt
- [ ] All CONFIG_ options in prj.conf
- [ ] Header files in correct location
- [ ] No typos in file names
```

### Issue: Runtime errors

```
Check console for:
- [ ] Stack overflow warnings
- [ ] Mutex deadlock messages
- [ ] Thread creation failures
- [ ] Memory allocation errors

If found, increase:
- [ ] STACK_SIZE in quadrant_demo.c
- [ ] CONFIG_HEAP_MEM_POOL_SIZE in prj.conf
```

## 🎓 Workshop Readiness Checklist

### For Each Student Station

```
Hardware:
- [ ] Board + LED matrix
- [ ] Power supply
- [ ] USB cables (programming + serial)
- [ ] All connections tested

Software:
- [ ] Zephyr SDK installed
- [ ] Project cloned
- [ ] Demo builds successfully
- [ ] Demo flashed and verified
- [ ] Serial terminal installed

Documentation:
- [ ] QUICK_START.md printed or available
- [ ] WORKSHOP_GUIDE.md for facilitator
- [ ] Example commands sheet
```

### Facilitator Preparation

```
- [ ] Reviewed WORKSHOP_GUIDE.md
- [ ] Practiced all demonstrations
- [ ] Tested all shell commands
- [ ] Prepared backup hardware
- [ ] Created USB backup drives with project
- [ ] Tested projector setup
- [ ] Prepared troubleshooting tools
```

## ✅ Final Verification

### System Health Check

```bash
# Run these commands and verify output:

kernel threads
- [ ] Shows 7+ threads
- [ ] All quadrant threads present
- [ ] Monitor and display threads present

thread stats
- [ ] All 4 animation threads listed
- [ ] Cycle counts increasing
- [ ] Priorities correct (5, 3, 7, 1)

# Visual check:
- [ ] All 4 quadrants animating
- [ ] Borders showing status
- [ ] No flickering or artifacts
- [ ] Smooth animations
```

### Demo Ready Criteria

The demo is ready for use when:

```
- [x] All source files created
- [x] All documentation complete
- [x] Configuration files updated
- [ ] Build succeeds without errors
- [ ] Hardware connected and tested
- [ ] LEDs animating in all quadrants
- [ ] Borders showing thread states
- [ ] Shell commands working
- [ ] All test cases pass
- [ ] Performance within expected ranges
```

## 📋 Workshop Checklist

### 30 Minutes Before Workshop

```
- [ ] All stations powered on
- [ ] Demo running on all stations
- [ ] Projector/screen set up
- [ ] Demo running on presenter's station
- [ ] Backup materials ready
- [ ] Serial terminals connected
```

### During Workshop

```
- [ ] Introduce demo and objectives
- [ ] Show working demo first
- [ ] Guide through shell commands
- [ ] Monitor student progress
- [ ] Help with troubleshooting
- [ ] Encourage experimentation
```

### After Workshop

```
- [ ] Collect feedback
- [ ] Note any issues for next time
- [ ] Share additional resources
- [ ] Follow up on questions
```

## 🎉 Success Indicators

Workshop is successful when students can:

```
- [ ] Explain priority-based scheduling
- [ ] Identify thread states from border colors
- [ ] Use shell commands confidently
- [ ] Demonstrate CPU load effects
- [ ] Modify code and rebuild
- [ ] Explain mutex purpose
- [ ] Describe real-time determinism
```

---

## ✅ Sign-Off

Demo verified by: _________________ Date: _________

Issues found: _________________________________________

Resolution: ___________________________________________

Demo status: [ ] Ready [ ] Needs work [ ] Not ready

---

**Use this checklist before every workshop or demo!** ✅
