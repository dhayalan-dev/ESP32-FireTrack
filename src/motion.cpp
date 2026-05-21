#include "motion.h"

static Adafruit_MPU6050 _mpu;
static MotionState _state;

static unsigned long _lastStepMs = 0;       // timestamp of last confirmed step
static unsigned long _lastUpdateMs = 0;     // timestamp of last sensor read
static float _biasMagnitude = 0.0f;         // magnitude offset measured during calibration
static float _heading = 0.0f;               // internal heading in degree

bool motion_init()
{
    // Initialize the I2C communication
    Wire.begin(PIN_SDA, PIN_SCL);

    // Check for the MPU with the address
    if (!(_mpu.begin(MPU_ADDR)))
    {
        Serial.println("[motion] MPU6050 not found");
        return false;
    }

    // Configure the MPU6050 settings
    _mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
    _mpu.setGyroRange(MPU6050_RANGE_250_DEG);
    _mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

    // Calibrate the sensor to find the bias magnitude
    _state = {0};
    _heading = 0.0f;
    _lastStepMs = 0;
    _lastUpdateMs = 0;

    Serial.println("[motion] Keep still - calibrating...");

    float sum = 0.0f;
    sensors_event_t accel, gyro, temp;

    for (int i = 0; i < CALIB_SAMPLES; i++)
    {
        _mpu.getEvent(&accel, &gyro, &temp);
        float ax = accel.acceleration.x;
        float ay = accel.acceleration.y;
        float az = accel.acceleration.z;

        sum += sqrt(ax*ax + ay*ay + az*az);
        delay(CALIB_DELAY_MS);
    }

    _biasMagnitude = sum / CALIB_SAMPLES;
    Serial.printf("[motion] Calibration done. Bias: %.4f m/s²\n", _biasMagnitude);

    return true;
}

void motion_update()
{
    unsigned long now = millis();
    if (now - _lastUpdateMs < MOTION_UPDATE_MS) return;

    sensors_event_t accel, gyro, temp;
    if (!_mpu.getEvent(&accel, &gyro, &temp))
    {
        Serial.println("[motion] Error reading sensor data");
        _lastUpdateMs = now;
        return;
    }

    float dt = (now - _lastUpdateMs) / 1000.0f;
    _lastUpdateMs = now;

    float ax = accel.acceleration.x;
    float ay = accel.acceleration.y;
    float az = accel.acceleration.z;

    float magnitude = sqrt(ax*ax + ay*ay + az*az);      // holds the magnitude of all the three axes
    float corrected = magnitude - _biasMagnitude;

    float gyroYdps = (gyro.gyro.y) * (180.0f / PI);     // holds the value of gyro in (°/s)

    if( fabs(gyroYdps) > GYRO_DEADZONE_DPS )
    {
        _heading += gyroYdps * dt;
    }

    // Keep the heading value under 360 degrees
    if (_heading < 0)       _heading += 360.0f;
    if (_heading >= 360)    _heading -= 360.0f;

    if (corrected > (STEP_THRESHOLD * 9.81f))
    {
        if (now - _lastStepMs > STEP_COOLDOWN_MS)
        {
            // confirmed step
            _lastStepMs = now;
            _state.stepCount++;
            _state.isMoving = true;

            // update position
            float headingRad = _heading * (PI / 180.0f);
            _state.x += STRIDE_LENGTH_M * sin(headingRad);
            _state.y += STRIDE_LENGTH_M * cos(headingRad);
            _state.distanceMoved += STRIDE_LENGTH_M;
            _state.heading = _heading;
        }
    }

    if (now - _lastStepMs > 2000) 
    { 
        _state.isMoving = false;
    }
}

void motion_reset()
{
    _state = {0};
    _heading = 0.0f;
    _lastStepMs = 0;
    _lastUpdateMs = 0;

    Serial.println("[motion] Position reset to Origin");
}

const MotionState& motion_getState()
{
    return _state;
}