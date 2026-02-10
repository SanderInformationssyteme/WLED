# Sander Logo Chase Animation - Background Layer

Automatically runs as a background animation that displays the Sander logo with periodic wipe effects. **Designed to run in the background** - other segments with effects will automatically take priority and overlay on top.

## Features

- **Background Layer Operation**: Runs continuously but yields to other active segments
- **Automatic Segment Detection**: Only draws on LEDs not used by other segments
- **Static Display**: Shows Sander logo colors with white background
- **Blue Wipe**: Pixel-by-pixel blue wipe across available LEDs
- **White Wipe**: Pixel-by-pixel white wipe across available LEDs  
- **Automatic Looping**: Continuously cycles through static → blue wipe → white wipe
- **Non-Intrusive**: Create segments with effects on top - Logo Chase will automatically make room

## How It Works

The Logo Chase runs in the background using `handleOverlayDraw()`, but it respects other segments:

1. Checks each LED before drawing
2. If an LED is used by another active segment (with any effect), it skips that LED
3. This allows you to create segments anywhere and run effects on them
4. The Logo Chase only draws on "free" LEDs not covered by other segments

**Example Usage:**
- Logo Chase runs continuously showing Sander colors
- Create a new segment (e.g., LEDs 0-30) with "Rainbow" effect
- Logo Chase automatically stops drawing on LEDs 0-30
- Rainbow effect runs on top with full control
- Delete the segment → Logo Chase resumes on those LEDs automatically

## Requirements

- **LED Count**: 122 LEDs (configurable via Web UI)
- **Sander Logo Colors**:
  - LEDs 84-89: Sander Orange (RGB 255,218,51)
  - LEDs 89-94: Sander Green (RGB 56,255,215)
  - LEDs 94-99: Sander Blue (RGB 128,170,255)
  - Other LEDs: White background with wipe animations

**Note**: No need to create segments manually. Logo Chase draws directly via overlay and automatically yields to any segments you create.

## Configuration

Add to your `cfg.json`:

```json
{
  "SanderLogoChase": {
    "enabled": true,
    "holdDuration": 10000,
    "wipeDelay": 30
  }
}
```

### Parameters

- `enabled` (bool): Enable/disable the animation (default: true)
- `holdDuration` (ms): How long to display static segments before wipe (default: 10000 = 10 seconds)
- `wipeDelay` (ms): Delay between LED updates during wipe effect (default: 30ms, lower = faster)

## Installation

1. Add to `platformio_override.ini`:
```ini
[env:my_build]
extends = env:esp32dev
build_flags = ${common.build_flags} ${esp32.build_flags}
  -D USERMOD_SANDER_LOGO_CHASE
```

2. Add to `usermods_list.cpp`:
```cpp
#ifdef USERMOD_SANDER_LOGO_CHASE
#include "../usermods/Sander_Logo_Chase/Sander_Logo_Chase.h"
#endif
```

3. Build and upload firmware

## Behavior

The Logo Chase runs continuously in the background:

1. **Static Hold**: Displays Sander logo colors for `holdDuration` milliseconds
2. **Blue Wipe**: Wipes blue color across available LEDs from start to end
3. **White Wipe**: Wipes white color across available LEDs from start to end  
4. **Repeat**: Returns to static and cycles continuously

**Background Layer Logic**:
- Runs at all times when enabled
- Before drawing each LED, checks if it's used by another active segment
- If LED is used by a segment with an effect → skips that LED
- If LED is free → draws Logo Chase animation
- This allows other segments to overlay and take priority automatically

**Creating Segments on Top**:
- Simply create a new segment in WLED (e.g., "seg": [{"start": 0, "stop": 50, "fx": 9}])
- The segment's effect will automatically show on those LEDs
- Logo Chase will skip those LEDs and continue on the rest
- Delete the segment → Logo Chase automatically resumes
