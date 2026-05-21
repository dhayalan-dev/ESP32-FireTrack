#pragma once

#include <Arduino.h>

// ── I2C ──────────────────────────────────────────────────────────
#define PIN_SDA     21
#define PIN_SCL     22
#define MPU_ADDR    0x68    // AD0 pin LOW = 0x68, HIGH = 0x69

// ── Motion ───────────────────────────────────────────────────────
#define MOTION_UPDATE_MS    10      // How often to read the sensor
#define STEP_THRESHOLD      1.2f    // g-force peak to count as a step
#define STEP_COOLDOWN_MS    250     // Minimum ms between steps
#define STRIDE_LENGTH_M     0.75f   // meters per step
#define GYRO_DEADZONE_DPS   0.5f    // ignore gyro noise below this (°/s)

// ── Calibration ────────────────────────────────────────────────────
#define CALIB_SAMPLES       500     // number of samples to average for bias
#define CALIB_DELAY_MS      2       // delay between calibration samples
