#include <Arduino.h>
#include "motion.h"

unsigned long _lastPrintMs = 0;

void setup() 
{
  Serial.begin(115200);
  if (!motion_init())
  {
    Serial.println("Motion sensor initialization failed. Check connections.");
    while (1) delay(1000);
  }
}

void loop() 
{
  unsigned long now = millis();
  motion_update();
  const MotionState& state = motion_getState();
  
  if (now - _lastPrintMs >= 500)
  {
    _lastPrintMs = now;
    // Print position, heading and step count
    // Comma Seperated Values (CSV) format for easy parsing in Python
    Serial.printf("%.2f,%.2f,%.1f,%d,%d\n",
                  state.x, state.y, state.heading, state.stepCount, state.isMoving ? 1 : 0);

    // More human-readable format for debugging
    // Serial.printf("Position: (%.2f, %.2f) | Heading: %.1f° | Steps: %d | Moving: %s\n",
                  // state.x, state.y, state.heading, state.stepCount, state.isMoving ? "Yes" : "No");
  }
}