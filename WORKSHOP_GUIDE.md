# Workshop Facilitator's Quick Reference Guide

## Pre-Workshop Setup (30 minutes before)

### Hardware Checklist
- [ ] SAM E54 Xplained Pro boards (one per participant)
- [ ] 16x16 WS2812 LED matrices
- [ ] 5V power supplies (5A minimum)
- [ ] USB cables for programming
- [ ] Power cables and connectors
- [ ] Backup hardware (at least 2 spare sets)

### Software Setup
- [ ] Zephyr SDK installed on all computers
- [ ] Project cloned and tested on each machine
- [ ] Serial terminal software installed (PuTTY, minicom, screen)
- [ ] Demo pre-compiled and tested
- [ ] Backup USB drives with project files

### Test Procedure (per station)
```bash
cd led_strip_ws2812_
west build -b same54_xpro
west flash
# Verify LEDs animate correctly
```

## Workshop Timeline (2 hours)

### Introduction (15 minutes)
- Welcome and introductions
- Overview of Zephyr RTOS
- Demo hardware overview
- Show working demo first (!)

### Hands-On Activities (90 minutes)

#### Activity 1: Observation (10 min)
**Objective**: Understand the baseline behavior

1. Power on demo
2. Observe all quadrants
3. Identify each animation pattern
4. Note border status indicators

**Key Talking Points**:
- "Each quadrant is a separate thread"
- "Border shows thread states in real-time"
- "Lower priority number = higher priority"

#### Activity 2: Shell Commands (15 min)
**Objective**: Learn interactive control

```bash
# Start with help
help

# Show thread stats
thread stats

# Suspend a thread
thread suspend 3

# Resume it
thread resume 3

# Boost priority
thread boost 4
```

**Common Issues**:
- Shell not responding: Press Enter 2-3 times
- Typos: Use Tab completion
- Wrong baud rate: Should be 115200

#### Activity 3: CPU Load (15 min)
**Objective**: Demonstrate priority scheduling

```bash
# Enable load
cpuload on

# Watch animations degrade
# Wait 30 seconds

# Check stats
thread stats

# Disable load
cpuload off
```

**Discussion Questions**:
- Which quadrants degraded most? (Lower priority ones)
- Why did Thread 1 stay smooth? (High priority)
- What happens to cycle counts? (Low-priority counts slow)

**Visual Indicators to Point Out**:
- Thread 4 sparkles become jerky/slow
- Thread 1 wave stays smooth
- Border might show more "ready" (yellow) states

#### Activity 4: Priority Experiments (20 min)
**Objective**: Understand priority effects

**Experiment A: Suspend High Priority**
```bash
thread suspend 1
# Quadrant 1 goes dark
# Other threads get more CPU time
thread stats  # Show increased cycles
thread resume 1
```

**Experiment B: Boost Low Priority**
```bash
thread boost 4
# Sparkles become smoother for 5 seconds
# Watch border show more green for Thread 4
```

**Experiment C: Cascade Suspension**
```bash
thread suspend 1
thread suspend 2
thread suspend 3
thread suspend 4
# Matrix goes dark quadrant by quadrant
# All borders show blue (suspended)

# Resume in reverse
thread resume 4
thread resume 3
thread resume 2
thread resume 1
```

#### Activity 5: Code Modification (30 min)
**Objective**: Hands-on coding

**Challenge 1: Change Colors**
Modify [quadrant_demo.c:96](src/quadrant_demo.c#L96):
```c
// Change from blue-cyan to red-orange
uint8_t hue = (uint8_t)(0 + 30 * wave);  // Red-orange range
```

**Challenge 2: Speed Up Animation**
Modify [quadrant_demo.c:343](src/quadrant_demo.c#L343):
```c
k_msleep(25);  // Changed from 50ms - now 40 FPS
```

**Challenge 3: Swap Priorities**
Modify [quadrant_demo.h:14-17](src/quadrant_demo.h#L14-L17):
```c
#define PRIORITY_QUAD1 1  // Was 5 - now lowest!
#define PRIORITY_QUAD4 5  // Was 1 - now high!
```

**After Each Change**:
```bash
west build -b same54_xpro
west flash
# Observe differences
```

### Q&A and Wrap-Up (15 minutes)
- Open discussion
- Review key concepts
- Additional resources
- Next steps

## Key Concepts to Emphasize

### 1. Priority-Based Scheduling
**Visual**: Point to specific quadrants
- "Thread 1 has priority 5 - that's high in Zephyr"
- "Lower number = higher priority (confusing but standard)"
- "High priority threads preempt low priority threads"

**Demo**:
```bash
thread boost 4
```
"Watch Thread 4 improve immediately!"

### 2. Thread States
**Visual**: Point to borders
- Green = Running on CPU right now
- Yellow = Ready, waiting for CPU time
- Red = Sleeping/blocked (k_msleep)
- Blue = Manually suspended

**Demo**: Suspend/resume and watch border change

### 3. Mutual Exclusion
**Explain**: "All threads share the LED buffer"
- Without mutex: Pixels would be corrupted
- With mutex: Clean animations

**Code Review**: Show [quadrant_demo.c:343-347](src/quadrant_demo.c#L343-L347):
```c
k_mutex_lock(&matrix_mutex, K_FOREVER);
quadrant1_wave_animation();
k_mutex_unlock(&matrix_mutex);
```

### 4. Real-Time Determinism
**Visual**: Under CPU load, Thread 1 stays smooth
- "High-priority threads get deterministic timing"
- "Critical tasks should have appropriate priority"
- "Low-priority tasks may starve under load"

### 5. Thread Starvation
**Demo**:
```bash
cpuload on
thread stats  # Show Thread 4 cycle count
# Wait 30 seconds
thread stats  # Thread 4 barely increased
```

"Thread 4 is starving - it's not getting CPU time!"

## Common Questions & Answers

### Q: "Why is Thread 1 priority 5, not 1?"
**A**: "In Zephyr, lower numbers are higher priority. Priority 0 is the highest (often reserved for critical tasks). We use 1-7 for our demo threads, with 5 being 'high' for animations."

### Q: "What happens if two threads have the same priority?"
**A**: "Zephyr uses round-robin scheduling - they take turns getting equal CPU time slices."

### Q: "Why do we need the display thread?"
**A**: "It pushes the frame buffer to the LEDs at a consistent 50 FPS. Without it, animations would only update when their thread runs, causing flicker."

### Q: "Can't the animations just update the LEDs directly?"
**A**: "They could, but separating frame generation from display refresh is a common pattern. It ensures consistent refresh rate regardless of animation complexity."

### Q: "What's the matrix_mutex protecting?"
**A**: "The shared LED frame buffer. Without it, threads might corrupt each other's pixel data, causing visual artifacts."

### Q: "Why does Thread 4 seem to run when all others are suspended?"
**A**: "Exactly! When higher-priority threads sleep or are suspended, the lowest-priority thread finally gets CPU time. This demonstrates how the scheduler works."

### Q: "What's the CPU load thread doing?"
**A**: "It runs a busy loop (wasteful computation) to simulate high CPU usage. It has the lowest possible priority so it only runs when nothing else needs CPU."

## Troubleshooting Guide

### Issue: LEDs not lighting up
**Check**:
1. Power supply connected and on
2. SPI pins wired correctly
3. Console shows "WS2812 initialized successfully"
4. Try `ws2812_set_brightness(255)` for full brightness

**Quick Fix**:
```c
// In main, add diagnostic
ws2812_set_pixel(0, 0, (rgb_t){0, 255, 0});  // Red (GRB order)
ws2812_update();
k_msleep(1000);
```

### Issue: Shell not responding
**Check**:
1. Baud rate is 115200
2. Correct COM port selected
3. USB cable is data-capable (not charge-only)

**Quick Fix**:
- Press Enter 3-4 times
- Try typing `help` + Enter
- Reconnect USB cable

### Issue: Only one quadrant working
**Check**:
1. Other threads not suspended
2. Console for thread creation errors
3. Stack overflow? Increase STACK_SIZE

**Quick Fix**:
```bash
thread stats  # Check thread states
thread resume 1  # Resume if suspended
thread resume 2
thread resume 3
thread resume 4
```

### Issue: Animations very jerky
**Possible Causes**:
1. CPU load active: `cpuload off`
2. Brightness too high: `ws2812_set_brightness(64)`
3. Insufficient power supply
4. SPI clock rate too low

### Issue: Build fails
**Common Errors**:
- Missing `CONFIG_SHELL=y` in prj.conf
- CMakeLists.txt not including quadrant_demo.c
- Header file path wrong

**Quick Fix**:
```bash
west build -t pristine
west build -b same54_xpro
```

## Advanced Discussion Topics

### For Advanced Students

#### Priority Inversion Problem
"What if a low-priority thread holds a mutex that a high-priority thread needs?"

**Zephyr's Solution**: Priority inheritance - temporarily boost the low-priority thread.

#### Deadlock
"What if Thread A waits for mutex 1 while holding mutex 2, and Thread B waits for mutex 2 while holding mutex 1?"

**Demo Idea**: Modify code to create intentional deadlock (carefully!)

#### Work Queues vs. Threads
"When should you use a work queue instead of a dedicated thread?"

**Discussion**: Work queues are better for sporadic tasks; threads for continuous work.

#### ISR Context Limitations
"Why can't we call `k_msleep()` from an interrupt handler?"

**Answer**: ISRs must be fast and non-blocking; use semaphores to signal threads.

## Post-Workshop Resources

### Students Should Leave With:
- [ ] Complete project source code
- [ ] This documentation
- [ ] Link to Zephyr docs: https://docs.zephyrproject.org
- [ ] Link to Zephyr Discord: https://discord.gg/zephyr

### Suggested Next Steps:
1. Modify animations (change colors, patterns)
2. Add button control
3. Implement new effects
4. Try other Zephyr samples
5. Build their own project

### Additional Zephyr Samples to Try:
- `samples/basic/threads` - Simple threading
- `samples/synchronization` - Mutexes, semaphores
- `samples/philosophers` - Classic CS problem
- `samples/userspace/hello_world_user` - User space threads

## Facilitator Checklist

### Before Workshop:
- [ ] Test all hardware
- [ ] Verify software builds
- [ ] Print handouts (this guide)
- [ ] Prepare backup USB drives
- [ ] Set up projector/screen
- [ ] Test serial terminal on projector

### During Workshop:
- [ ] Introduce yourself and agenda
- [ ] Show working demo first
- [ ] Walk around during hands-on
- [ ] Watch for common issues
- [ ] Encourage experimentation
- [ ] Take photos (with permission)

### After Workshop:
- [ ] Collect feedback
- [ ] Share resources via email
- [ ] Follow up on questions
- [ ] Document issues for next time

## Timing Variants

### Short Version (1 hour)
- Introduction: 10 min
- Activity 1-2: 30 min
- Code modification: 15 min
- Q&A: 5 min

### Extended Version (3 hours)
- Include all activities
- Add advanced topics (priority inversion, deadlock)
- More complex code modifications
- Individual project time

### Half-Day Version (4 hours)
- Morning: Core concepts + activities 1-4
- Break: 15 min
- Afternoon: Code modifications + free exploration
- Final presentations (optional)

## Success Metrics

Workshop is successful when students can:
- [ ] Explain priority-based scheduling
- [ ] Identify thread states visually
- [ ] Use shell commands confidently
- [ ] Modify code and rebuild
- [ ] Understand mutex purpose
- [ ] Describe real-time determinism
- [ ] Debug common issues

## Contact & Support

For workshop facilitation support:
- Create issue on project GitHub
- Ask on Zephyr Discord #education channel
- Email Zephyr community list

---

**Good luck with your workshop!** 🎓🚀
