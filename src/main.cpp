/*******************************************************************************
* Draw an analog clock on an oscilloscope
* Original by Dave Burke, 2020, https://github.com/daveyburke/Oscilloscope-Clock
* Optimized for PlatformIO & corrected for XY Oscilloscope polarity mapping
* by Wolfhard Eick, 2026
*******************************************************************************/

#include <Arduino.h>
#include <Wire.h>

// Hardware Pin Definitions
#define X_PIN 5   
#define Y_PIN 6 
#define CLOCK_ADV_PIN 2

// Timing configurations
// Lower values increase refresh rate (reduce flickering on analog scopes)
// Adjust if vectors distort due to hardware RC low-pass filter limitations
#define DELAY 30

// Graphic definitions
#define RADIUS 125.0
#define XY_OFFSET RADIUS

// Time calculation helpers
#define SECS_PER_MIN  (60UL)
#define SECS_PER_HOUR (3600UL)
#define SECS_PER_DAY  (SECS_PER_HOUR * 24L)
#define numberOfSeconds(_time_) (_time_ % SECS_PER_MIN)  
#define numberOfMinutes(_time_) ((_time_ / SECS_PER_MIN) % SECS_PER_MIN)
#define numberOfHours(_time_) (( _time_% SECS_PER_DAY) / SECS_PER_HOUR)

// Global state variables
int gLastButtonState = LOW;
unsigned long gBootTimeSec = 0;
unsigned long gTimeOffset = 0;

// === C++ Function Prototypes for PlatformIO ===
uint64_t millis64();
void drawPoint(int x, int y);
void drawRadialLine(float angle, int r1, int r2);
void drawArc(float a1, float a2, float r);

/**
 * @brief 64-bit millis implementation to prevent rollover issues.
 * Essential since TCCR0B modification accelerates the timer frequency by 64x.
 */
uint64_t millis64() {
    static uint32_t low32, high32; 
    uint32_t newLow32 = millis();
    if (newLow32 < low32) high32++; 
    low32 = newLow32;
    return ((uint64_t) high32 << 32 | low32) / 64UL;
}

/**
 * @brief Outputs raw X/Y coordinates via PWM to the RC low-pass filters.
 */
void drawPoint(int x, int y) {
    analogWrite(X_PIN, x + XY_OFFSET); 
    analogWrite(Y_PIN, y + XY_OFFSET);
    delayMicroseconds(DELAY);
}

/**
 * @brief Draws a vector from radius r1 to r2 at a specific angle.
 * @param angle Normalized angle measured in turns (0.0 to 1.0)
 */
void drawRadialLine(float angle, int r1, int r2) {
    int dir = r1 > r2 ? -5 : 5;
    for (int r = r1; r1 > r2 ? r > r2 : r < r2; r += dir) {
        /* * POLARITY ADJUSTMENT:
         * Removing all negative signs targets the inverted scope channels directly.
         * This re-aligns the vertical phase to place 12 o'clock at the top and 
         * flips the sweep direction back to a clean clockwise rotation.
         */
        drawPoint(sin(2 * PI * angle) * r, cos(2 * PI * angle) * r);
    }
}

/**
 * @brief Draws a circular arc between two angles at a fixed radius.
 */
void drawArc(float a1, float a2, float r) {
    float dir = a2 > a1 ? .005 : -.005;
    for (float a = a2 > a1 ? a1 : a2; a2 > a1 ? a < a2 : a > a1; a += dir) {
        /*
         * POLARITY ADJUSTMENT:
         * Standard positive (sin, cos) space to sync the face rendering
         * with the updated hands mapping.
         */
        drawPoint(sin(2 * PI * a) * r, cos(2 * PI * a) * r);
    }
}

void setup() {
    pinMode(X_PIN, OUTPUT);
    pinMode(Y_PIN, OUTPUT);
    pinMode(CLOCK_ADV_PIN, INPUT);
    pinMode(13, OUTPUT); // Status LED for button feedback

    // Increase PWM frequency on Pins 5 & 6 to 62500 Hz to minimize ripple after the RC filter.
    // Prescaler configuration changes Timer 0, which accelerates millis() speed by factor 64.
    TCCR0B = (TCCR0B & 0b11111000) | 0x01;

    gBootTimeSec = millis64() / 1000UL;  
}

void loop() {  
    // Main system time tracking
    unsigned long elapsedSec = millis64() / 1000UL - gBootTimeSec + gTimeOffset;
    int hrs = numberOfHours(elapsedSec);
    int mins = numberOfMinutes(elapsedSec);
    int secs = numberOfSeconds(elapsedSec);

    // Dynamic clock setting via hardware input button (D2)
    if (digitalRead(CLOCK_ADV_PIN) == HIGH) {
        digitalWrite(13, HIGH); // Visually confirm button press on onboard LED
        gTimeOffset += (gLastButtonState == LOW) ? SECS_PER_MIN : SECS_PER_MIN * 10;
        gLastButtonState = HIGH;
    } else {
        digitalWrite(13, LOW);  
        gLastButtonState = LOW;
    }
  
    // 1. Render Second Hand
    float secsAngle = secs / 60.0;
    drawRadialLine(secsAngle + 0.5, 0, 30);  // Reverse counterweight part of the hand
    drawRadialLine(secsAngle, 0, RADIUS);
    
    // 2. Render Outer Dial Tick Marks & Arcs
    for (int i = secs; i < secs + 60; i++) {
        float a = i / 60.0;
        if (i % 5 == 0) {
            drawRadialLine(a, RADIUS, RADIUS - 20);
            drawRadialLine(a, RADIUS - 20, RADIUS);
        }
        drawArc(a, a + 1.0 / 60.0, RADIUS);
    }
    drawRadialLine(secsAngle, RADIUS, 0);
    
    // 3. Render Minute Hand
    float minsAngle = mins / 60.0;
    drawRadialLine(minsAngle + 0.5, 0, 30);
    drawRadialLine(minsAngle, 0, RADIUS - 10);
    drawRadialLine(minsAngle, RADIUS - 10, 0);
  
    // 4. Render Hour Hand (includes interpolation based on passing minutes)
    float hrsAngle = hrs / 12.0 + minsAngle / 12.0;
    drawRadialLine(hrsAngle + 0.5, 0, 30);
    drawRadialLine(hrsAngle, 0, RADIUS - 45);
    drawRadialLine(hrsAngle, RADIUS - 45, 0);
}