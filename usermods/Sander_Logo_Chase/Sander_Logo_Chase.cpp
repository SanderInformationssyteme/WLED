#include "wled.h"

/*
 * Sander Logo Chase Animation - Background Layer
 * 
 * Runs as a background animation that automatically yields to any other active segments.
 * This allows you to create segments with effects on top, and the Logo Chase will only
 * draw on LEDs that are not used by other segments.
 * 
 * Animation sequence:
 * 1. Static display with Sander logo colors (10 seconds default)
 * 2. Blue wipe effect across all available LEDs
 * 3. White wipe effect across all available LEDs
 * 4. Return to static display and repeat
 * 
 * Sander logo colors (always visible on LEDs 84-99):
 * - LEDs 84-89:  Sander Orange (RGB 255,218,51)
 * - LEDs 89-94:  Sander Green (RGB 56,255,215)
 * - LEDs 94-99:  Sander Blue (RGB 128,170,255)
 * - Other LEDs:  White background with wipe animations
 */

class SanderLogoChaseUsermod : public Usermod {
private:
    // Animation states
    enum AnimationState {
        STATIC_HOLD,
        WIPE_BLUE,
        WIPE_WHITE
    };

    // Sander color definitions
    static const uint32_t COLOR_SANDER_ORANGE = 0xFFDA33; // RGB(255, 218, 51)
    static const uint32_t COLOR_SANDER_GREEN  = 0x38FFD7; // RGB(56, 255, 215)
    static const uint32_t COLOR_SANDER_BLUE   = 0x80AAFF; // RGB(128, 170, 255)
    static const uint32_t COLOR_WHITE         = 0xFFFFFF; // RGB(255, 255, 255)
    static const uint32_t COLOR_BLUE          = 0x80AAFF; // RGB(0, 0, 255)

    // Configuration
    bool enabled = true;
    unsigned long holdDuration = 10000;  // 10 seconds static display
    unsigned long wipeDelay = 30;        // 30ms between LED updates during wipe
    uint16_t ledCount = 122;             // LED count for Sander logo
    
    // State tracking
    AnimationState currentState = STATIC_HOLD;
    unsigned long stateStartTime = 0;
    unsigned long lastWipeUpdate = 0;
    int wipePosition = 0;
    bool initDone = false;

public:
    void setup() override {
        initDone = true;
        stateStartTime = millis();
        currentState = STATIC_HOLD;
        Serial.println("Sander Logo Chase: Initialized");
        Serial.printf("LED Count: %d, Hold Duration: %dms\n", ledCount, holdDuration);
    }

    void loop() override {
        if (!enabled || strip.isUpdating()) return;

        // Logo Chase läuft immer im Hintergrund - handleOverlayDraw() respektiert andere Segmente
        unsigned long now = millis();

        switch (currentState) {
            case STATIC_HOLD:
                // Wait for hold duration, then start blue wipe
                if (now - stateStartTime >= holdDuration) {
                    Serial.println("Starting BLUE wipe");
                    startWipe(WIPE_BLUE);
                }
                break;

            case WIPE_BLUE:
                // Advance wipe position
                if (now - lastWipeUpdate >= wipeDelay) {
                    lastWipeUpdate = now;
                    wipePosition++;
                    
                    if (wipePosition >= ledCount) {
                        Serial.println("Starting WHITE wipe");
                        startWipe(WIPE_WHITE);
                    }
                }
                break;

            case WIPE_WHITE:
                // Advance wipe position
                if (now - lastWipeUpdate >= wipeDelay) {
                    lastWipeUpdate = now;
                    wipePosition++;
                    
                    if (wipePosition >= ledCount) {
                        Serial.println("Back to STATIC");
                        // Return to static
                        currentState = STATIC_HOLD;
                        stateStartTime = now;
                    }
                }
                break;
        }
    }

    void handleOverlayDraw() override {
        if (!enabled) return;
        
        // Draw Logo Chase animation only on LEDs not used by other segments
        // This allows other segments to run effects on top of the Logo Chase
        
        // First, always draw the Sander logo stripes (they never change)
        // Only draw if not covered by another segment
        for (int i = 84; i < 89; i++) {
            if (!isLedUsedByOtherSegment(i)) {
                strip.setPixelColor(i, COLOR_SANDER_ORANGE);
            }
        }
        for (int i = 89; i < 94; i++) {
            if (!isLedUsedByOtherSegment(i)) {
                strip.setPixelColor(i, COLOR_SANDER_GREEN);
            }
        }
        for (int i = 94; i < 99; i++) {
            if (!isLedUsedByOtherSegment(i)) {
                strip.setPixelColor(i, COLOR_SANDER_BLUE);
            }
        }
        
        // Then handle the white areas based on state
        switch (currentState) {
            case STATIC_HOLD:
                // Draw white in both white areas (if not covered)
                for (int i = 0; i < 84; i++) {
                    if (!isLedUsedByOtherSegment(i)) {
                        strip.setPixelColor(i, COLOR_WHITE);
                    }
                }
                for (int i = 99; i < ledCount; i++) {
                    if (!isLedUsedByOtherSegment(i)) {
                        strip.setPixelColor(i, COLOR_WHITE);
                    }
                }
                break;

            case WIPE_BLUE:
                // Blue wipe from start, but preserve stripes and respect other segments
                for (int i = 0; i < ledCount; i++) {
                    if (i >= 84 && i < 99) continue; // Skip the colored stripes (handled above)
                    if (isLedUsedByOtherSegment(i)) continue; // Skip if covered by another segment
                    
                    if (i <= wipePosition) {
                        strip.setPixelColor(i, COLOR_BLUE);
                    } else {
                        strip.setPixelColor(i, COLOR_WHITE);
                    }
                }
                break;

            case WIPE_WHITE:
                // White wipe from start (removes blue), but preserve stripes and respect other segments
                for (int i = 0; i < ledCount; i++) {
                    if (i >= 84 && i < 99) continue; // Skip the colored stripes (handled above)
                    if (isLedUsedByOtherSegment(i)) continue; // Skip if covered by another segment
                    
                    if (i <= wipePosition) {
                        strip.setPixelColor(i, COLOR_WHITE);
                    } else {
                        strip.setPixelColor(i, COLOR_BLUE);
                    }
                }
                break;
        }
    }

    void addToConfig(JsonObject& root) override {
        JsonObject top = root.createNestedObject(FPSTR("SanderLogoChase"));
        top[FPSTR("enabled")] = enabled;
        top[FPSTR("holdDuration")] = holdDuration;
        top[FPSTR("wipeDelay")] = wipeDelay;
        top[FPSTR("ledCount")] = ledCount;
    }

    bool readFromConfig(JsonObject& root) override {
        JsonObject top = root[FPSTR("SanderLogoChase")];
        
        bool configComplete = !top.isNull();
        
        configComplete &= getJsonValue(top[FPSTR("enabled")], enabled);
        configComplete &= getJsonValue(top[FPSTR("holdDuration")], holdDuration);
        configComplete &= getJsonValue(top[FPSTR("wipeDelay")], wipeDelay);
        configComplete &= getJsonValue(top[FPSTR("ledCount")], ledCount);
        
        return configComplete;
    }

    uint16_t getId() override {
        return USERMOD_ID_UNSPECIFIED;
    }

private:
    // Check if a specific LED is used by any active segment (other than main segment 0)
    // Returns true if LED should be skipped (used by another segment)
    bool isLedUsedByOtherSegment(int ledIndex) {
        // Check all segments
        for (uint8_t i = 0; i < strip.getSegmentsNum(); i++) {
            Segment& seg = strip.getSegment(i);
            
            // Skip inactive segments
            if (!seg.isActive() || !seg.isSelected()) continue;
            
            // Check if this LED is in the segment's range
            // Note: We want to yield to ALL active segments including segment 0
            // if they have a non-static effect running
            if (ledIndex >= seg.start && ledIndex < seg.stop) {
                // LED is in this segment's range - check if segment has an effect
                if (seg.mode != FX_MODE_STATIC) {
                    return true; // This LED is used by an active effect
                }
            }
        }
        return false; // LED is free for Logo Chase
    }

    void startWipe(AnimationState newState) {
        currentState = newState;
        wipePosition = 0;
        lastWipeUpdate = millis();
    }
};

// Register the usermod
static SanderLogoChaseUsermod sander_logo_chase_instance;
REGISTER_USERMOD(sander_logo_chase_instance);
