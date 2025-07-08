//
// Created by juri on 27.06.25.
//

#ifndef CPP_TEMPLATE_ROLLER485_H
#define CPP_TEMPLATE_ROLLER485_H

#include <cstdio>
#include <cstdint>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>

namespace roller485
{

class Roller485
{

      public:
	typedef enum {
		SPEED_MODE = 0x01,
		POSITION_MODE = 0x02,
		CURRENT_MODE = 0x03,
		ENCODER_MODE = 0x04,
	} motor_mode_t;

	explicit Roller485(uint8_t i2c_address);
	virtual ~Roller485();

	void enableMotor();
	void disableMotor();
	void setMode(motor_mode_t mode);
	void setRGBUserMode();
	void setRGBColor(uint8_t red, uint8_t green, uint8_t blue);
	void setMaxCurrent(int32_t max_current, motor_mode_t mode);
	void setSpeed(int32_t speed);
	int32_t getSpeed();
	void setSpeedPID(float p, float i, float d);
	void setPosition(float position);
	float getPosition();
	void setPositionPID(float p, float i, float d);
	void setCurrent(int32_t current);
	int32_t getCurrent();
	int32_t getInternalTemperature();
	int32_t getEncoderCounter();

      private:
	int sendAndCheck(uint8_t *data, size_t length);

	const uint8_t i2c_address;
	const struct device *i2c_dev;

	bool motor_enabled = false;
	motor_mode_t motor_mode = SPEED_MODE;

	typedef enum {
		MOTOR_ENABLE = 0x00,
		MOTOR_MODE = 0x01,
		MOTOR_OVER_RANGE_PROTECTION = 0x0A,
		REMOVE_JAM_PROTECTION = 0x0B,
		MOTOR_STATUS = 0x0C,
		MOTOR_ERROR_STATUS = 0x0D,
		ENABLE_BUTTON_MODE_SWITCHING = 0x0E,
		ENABLE_MOTOR_JAM_PROTECTION = 0x0F,
		DEVICE_ID = 0x10,
		RS485_BAUD_RATE = 0x11,
		RGB_LED_BRIGHTNESS = 0x12,
		POSITION_MODE_MAX_CURRENT = 0x20,
		RGB_COLOR = 0x30,
		RGB_USER_MODE = 0x33,
		POWER_VIN = 0x34,
		INTERNAL_TEMP = 0x38,
		ENCODER_COUNTER = 0x3C,
		SPEED_MODE_TARGET_SPEED = 0x40,
		SPEED_MODE_MAX_CURRENT = 0x50,
		SPEED_MODE_READBACK_SPEED = 0x60,
		SPEED_MODE_PID_CONFIG = 0x70,
		POSITION_MODE_TARGET_POSITION = 0x80,
		POSITION_MODE_READBACK_POSITION = 0x90,
		POSITION_MODE_PID_CONFIG = 0xA0,
		CURRENT_MODE_CURRENT_TARGET = 0xB0,
		CURRENT_MODE_READBACK_CURRENT = 0xC0,
		SAVE_FLASH = 0xF0,
		FIRMWARE_VERSION = 0xFE,
		I2C_ADDRESS = 0xFF,
	} i2c_address_t;
};

} // namespace roller485

#endif // CPP_TEMPLATE_ROLLER485_H
