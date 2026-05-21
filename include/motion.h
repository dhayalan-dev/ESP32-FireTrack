/**
 * @file   motion.h
 * @brief  MPU6050 pedestrian dead reckoning module.
 *
 * Tracks 2D position (x, y) and heading using step detection
 * and gyroscope integration. Designed for chest-mounted MPU6050
 * on ESP32.
 */
#pragma once

#include "config.h"
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

// Motion state structure
struct MotionState
{
    float x; 
    float y;
    float distanceMoved;
    float heading;
    uint16_t stepCount;
    bool isMoving;
};

/**
 * @brief  Initialize the motion tracking module.
 *
 * Initializes I2C, configures MPU6050 and performs bias
 * calibration. Keep the sensor completely still during startup
 * until calibration is complete.
 *
 * @return true if sensor found and initialized, false otherwise.
 */
bool motion_init(void);

/**
 * @brief  Read sensor and update position, heading and step count.
 *
 * Must be called every loop iteration. Internally rate-limited
 * to MOTION_UPDATE_MS intervals — safe to call as fast as possible.
 */
void motion_update(void);

/**
 * @brief  Reset position and heading back to origin.
 *
 * Zeroes x, y, heading, distanceMoved and stepCount.
 * Does not reset sensor calibration bias — call motion_init()
 * for a full hardware reset.
 */
void motion_reset(void);

/**
 * @brief  Get the current motion state.
 *
 * Returns a const reference — do not store the reference,
 * read the values you need immediately.
 *
 * @return const reference to the current MotionState.
 */
const MotionState& motion_getState(void);