# Multi-Threaded Quadrant Demo - Quick Start

## 🚀 Setup in 5 Minutes

### 1. Backup Current Main
```bash
cd src
cp main.c main_backup.c
cp main_quadrant_demo.c main.c
```

### 2. Build & Flash
```bash
west build -b same54_xpro
west flash
```

### 3. Connect Serial Terminal
```bash
# Linux/Mac
screen /dev/ttyACM0 115200

# Windows: Use PuTTY or TeraTerm at 115200 baud
```

### 4. Try Commands
```bash
help                # Show all commands
thread stats        # Show thread statistics
thread suspend 3    # Suspend quadrant 3
thread resume 3     # Resume quadrant 3
thread boost 4      # Boost thread 4 priority
cpuload on          # Add CPU load
cpuload off         # Remove CPU load
```

## 📊 Quadrant Layout

```
+-------------------+-------------------+
| Q1: Blue Wave     | Q2: Red Ball      |
| Priority 5 (High) | Priority 3 (Med)  |
+-------------------+-------------------+
| Q3: Green Spiral  | Q4: Yellow Stars  |
| Priority 7 (Low)  | Priority 1 (Low)  |
+-------------------+-------------------+
```

## 🎨 Border Status Colors

- 🟢 **Green**: Running
- 🟡 **Yellow**: Ready (waiting)
- 🔴 **Red**: Sleeping
- 🔵 **Blue**: Suspended

## 📝 Shell Commands Cheat Sheet

| Command |         Description           |            Example |
|---------|         -------------|-                     --------|
| `thread suspend N` | Suspend thread 1-4 | `thread suspend 3` |
| `thread resume N`  | Resume thread 1-4  | `thread resume 3` |
| `thread stats`     | Show thread info   | `thread stats` |
| `thread boost N`   | Boost priority (5s)| `thread boost 4` |
| `cpuload on/off`   | CPU load simulation| `cpuload on` |
| `help`             | List all commands  | `help` |
| `kernel threads`   | Show all threads   | `kernel threads` |

## 🎯 Quick Experiments

### Experiment 1: See Priority in Action
```bash
thread boost 4
# Watch sparkles become smoother for 5 seconds!
```

### Experiment 2: Demonstrate Preemption
```bash
cpuload on
# Low-priority animations degrade
# High-priority Thread 1 stays smooth
cpuload off
```

### Experiment 3: Thread Suspension
```bash
thread suspend 1
thread suspend 2
thread suspend 3
thread suspend 4
# Watch matrix go dark quadrant by quadrant
# Resume in reverse order
```

## 🔧 Modify Code (Optional)

### Change Animation Speed
Edit [src/quadrant_demo.c:343](src/quadrant_demo.c#L343):
```c
k_msleep(25);  // Was 50ms - now twice as fast!
```

### Change Colors
Edit [src/quadrant_demo.c:96](src/quadrant_demo.c#L96):
```c
uint8_t hue = (uint8_t)(0 + 30 * wave);  // Red-orange instead of blue-cyan
```

### Swap Priorities
Edit [src/quadrant_demo.h:14-17](src/quadrant_demo.h#L14-L17):
```c
#define PRIORITY_QUAD1 1  // Make wave lowest priority
#define PRIORITY_QUAD4 5  // Make sparkles highest priority
```

After changes:
```bash
west build
west flash
```

## 📖 Full Documentation

- [QUADRANT_DEMO.md](QUADRANT_DEMO.md) - Complete technical documentation
- [WORKSHOP_GUIDE.md](WORKSHOP_GUIDE.md) - Workshop facilitator guide

## ❓ Troubleshooting

### LEDs not lighting?
- Check 5V power supply connected
- Verify console shows "WS2812 initialized successfully"
- Try increasing brightness in code

### Shell not responding?
- Press Enter 2-3 times
- Verify 115200 baud rate
- Check USB cable supports data

### Animations jerky?
- Run `cpuload off`
- Reduce brightness: `ws2812_set_brightness(64)`
- Check power supply current rating

## 🎓 What You'll Learn

- ✅ Priority-based scheduling
- ✅ Thread synchronization
- ✅ Mutex protection
- ✅ Thread states
- ✅ Priority inversion
- ✅ Real-time determinism
- ✅ Shell/CLI integration
- ✅ Visual debugging

## 🔗 Resources

- Zephyr Docs: https://docs.zephyrproject.org
- Zephyr Discord: https://discord.gg/zephyr
- Project Documentation: See QUADRANT_DEMO.md

---

**Enjoy exploring Zephyr threading!** 🎨🧵✨
