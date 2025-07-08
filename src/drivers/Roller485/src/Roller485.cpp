//
// Created by juri on 27.06.25.
//
#include "Roller485.h"

static inline void intToFourBytes(int32_t value, int8_t *bytes)
{
	*reinterpret_cast<int32_t *>(bytes) = static_cast<int32_t>(value);
}

static inline void intToFourBytesCurrent(int32_t value, int8_t *bytes)
{
	intToFourBytes(value * 100, bytes);
}

static inline void intToFourBytesSpeed(int32_t value, int8_t *bytes)
{
	intToFourBytes(value * 100, bytes);
}

static inline void intToFourBytesPosition(int32_t value, int8_t *bytes)
{
	intToFourBytes(value * 100, bytes);
}

static inline void floatToFourBytesPosition(float value, int8_t *bytes)
{
	intToFourBytes(static_cast<int32_t>(value * 100), bytes);
}

static inline void floatToFourBytesPD(float value, int8_t *bytes)
{
	intToFourBytes(static_cast<int32_t>(value * 100000), bytes);
}
static inline void floatToFourBytesI(float value, int8_t *bytes)
{
	intToFourBytes(static_cast<int32_t>(value * 10000000), bytes);
}

static inline int32_t fourBytesToInt(const int8_t *bytes)
{
	return *reinterpret_cast<const int32_t *>(bytes);
}

static inline int32_t fourBytesToIntCurrent(const int8_t *bytes)
{
	return fourBytesToInt(bytes) / 100;
}

static inline int32_t fourBytesToIntSpeed(const int8_t *bytes)
{
	return fourBytesToInt(bytes) / 100;
}

static inline int32_t fourBytesToIntPosition(const int8_t *bytes)
{
	return fourBytesToInt(bytes) / 100;
}

static inline float fourBytesToFloatPosition(const int8_t *bytes)
{
	return static_cast<float>(fourBytesToInt(bytes)) / 100.0f;
}

static inline float fourBytesToFloatPD(const int8_t *bytes)
{
	return static_cast<float>(fourBytesToInt(bytes)) / 100000.0f;
}

static inline float fourBytesToFloatI(const int8_t *bytes)
{
	return static_cast<float>(fourBytesToInt(bytes)) / 10000000.0f;
}

// TODO: add logging

namespace roller485
{
Roller485::Roller485(uint8_t i2c_address)
	: i2c_address(i2c_address), i2c_dev(DEVICE_DT_GET(DT_NODELABEL(i2c1)))
{
	if (!device_is_ready(i2c_dev)) {
		printk("I2C device not ready\n");
	}

	printk("Roller485 initialized with I2C address: 0x%02X\n", i2c_address);

	setRGBUserMode();
	disableMotor();
}

Roller485::~Roller485() = default;

int Roller485::sendAndCheck(uint8_t *data, size_t length)
{
	int ret = i2c_write(this->i2c_dev, data, length, this->i2c_address);
	if (ret != 0) {
		printk("Failed to send I2C command: %d\n", ret);
		return ret;
	}
	return 0;
}
void Roller485::enableMotor()
{
	uint8_t command[2] = {MOTOR_ENABLE, 0x01};
	if (sendAndCheck(command, 2) == 0) {
		motor_enabled = true;
		setRGBColor(0, 255, 0);
		printk("Motor enabled\n");
	} else {
		printk("Failed to enable motor\n");
		// TODO: Change to error state
	}
}

void Roller485::disableMotor()
{
	uint8_t command[2] = {MOTOR_ENABLE, 0x00};
	if (sendAndCheck(command, 2) == 0) {
		motor_enabled = false;
		setRGBColor(0, 0, 255);
		printk("Motor disabled\n");
	} else {
		printk("Failed to disable motor\n");
	}
}

void Roller485::setMode(Roller485::motor_mode_t mode)
{
	if (mode < SPEED_MODE || mode > ENCODER_MODE) {
		printk("Invalid motor mode: %d\n", mode);
		return;
	}

	uint8_t command[2] = {MOTOR_MODE, static_cast<uint8_t>(mode)};
	if (sendAndCheck(command, 2) == 0) {
		motor_mode = mode;
		printk("Motor mode set to %d\n", mode);
	} else {
		printk("Failed to set motor mode\n");
	}
}

void Roller485::setRGBUserMode()
{
	uint8_t command[2] = {RGB_USER_MODE, 0x01};
	if (sendAndCheck(command, 2) == 0) {
		printk("Motor mode set to user mode\n");
	} else {
		printk("Failed to set motor mode to user mode\n");
	}
}

void Roller485::setRGBColor(uint8_t red, uint8_t green, uint8_t blue)
{
	uint8_t command[4] = {RGB_COLOR, blue, green, red}; // BGR format
	if (sendAndCheck(command, 2) == 0) {
		//		printk("RGB color set to R:%d G:%d B:%d\n", red, green, blue);
	} else {
		printk("Failed to set RGB color\n");
	}
}
void Roller485::setMaxCurrent(int32_t max_current, Roller485::motor_mode_t mode)
{
	uint8_t address;
	int8_t max_current_bytes[4] = {0, 0, 0, 0};

	switch (mode) {
	case SPEED_MODE:
		address = SPEED_MODE_MAX_CURRENT;
		break;
	case POSITION_MODE:
		address = POSITION_MODE_MAX_CURRENT;
		break;
	case CURRENT_MODE:
		printk("Current mode does not support setting max current\n");
		return;
	case ENCODER_MODE:
		printk("Encoder mode does not support setting max current\n");
		return;
	default:
		printk("Invalid motor mode for setting max current\n");
		return;
	}

	intToFourBytesCurrent(max_current, max_current_bytes);

	printk("Max current bytes: 0x%02X 0x%02X 0x%02X 0x%02X\n", max_current_bytes[0],
	       max_current_bytes[1], max_current_bytes[2], max_current_bytes[3]);
	uint8_t command[5] = {address, static_cast<uint8_t>(max_current_bytes[0]),
			      static_cast<uint8_t>(max_current_bytes[1]),
			      static_cast<uint8_t>(max_current_bytes[2]),
			      static_cast<uint8_t>(max_current_bytes[3])};

	if (sendAndCheck(command, 5) == 0) {
		printk("Max current set to %d mA for mode %d\n",
		       fourBytesToIntCurrent(max_current_bytes), mode);
	} else {
		printk("Failed to set max current\n");
	}
}

void Roller485::setSpeed(int32_t speed)
{
	if (motor_mode != SPEED_MODE) {
		printk("Motor is not in speed mode, setting speed has no effect\n");
	}

	int8_t speed_bytes[4] = {0, 0, 0, 0};
	intToFourBytesSpeed(speed, speed_bytes);
	uint8_t command[5] = {SPEED_MODE_TARGET_SPEED, static_cast<uint8_t>(speed_bytes[0]),
			      static_cast<uint8_t>(speed_bytes[1]),
			      static_cast<uint8_t>(speed_bytes[2]),
			      static_cast<uint8_t>(speed_bytes[3])};

	printk("Speed bytes: 0x%02X 0x%02X 0x%02X 0x%02X\n", command[4], command[3], command[2],
	       command[1]);
	if (sendAndCheck(command, 5) == 0) {
		printk("Speed set to %d RPM\n", fourBytesToIntSpeed(speed_bytes));
	} else {
		printk("Failed to set speed\n");
	}
}

int32_t Roller485::getSpeed()
{
	if (motor_mode != SPEED_MODE) {
		printk("Motor is not in speed mode, getting speed has no effect\n");
	}

	uint8_t buffer[5] = {SPEED_MODE_READBACK_SPEED, 0, 0, 0, 0};
	i2c_write_read(i2c_dev, i2c_address, buffer, 1, &buffer[1], 4);
	int8_t data[4] = {static_cast<int8_t>(buffer[1]), static_cast<int8_t>(buffer[2]),
			  static_cast<int8_t>(buffer[3]), static_cast<int8_t>(buffer[4])};
	return fourBytesToIntSpeed(data);
}

void Roller485::setSpeedPID(float p, float i, float d)
{
	if (motor_mode != SPEED_MODE) {
		printk("Motor is not in speed mode, setting speed PID has no effect\n");
	}

	int8_t p_bytes[4] = {0, 0, 0, 0};
	int8_t i_bytes[4] = {0, 0, 0, 0};
	int8_t d_bytes[4] = {0, 0, 0, 0};

	floatToFourBytesPD(p, p_bytes);
	floatToFourBytesI(i, i_bytes);
	floatToFourBytesPD(d, d_bytes);

	uint8_t command[13] = {SPEED_MODE_PID_CONFIG,
			       static_cast<uint8_t>(p_bytes[0]),
			       static_cast<uint8_t>(p_bytes[1]),
			       static_cast<uint8_t>(p_bytes[2]),
			       static_cast<uint8_t>(p_bytes[3]),
			       static_cast<uint8_t>(i_bytes[0]),
			       static_cast<uint8_t>(i_bytes[1]),
			       static_cast<uint8_t>(i_bytes[2]),
			       static_cast<uint8_t>(i_bytes[3]),
			       static_cast<uint8_t>(d_bytes[0]),
			       static_cast<uint8_t>(d_bytes[1]),
			       static_cast<uint8_t>(d_bytes[2]),
			       static_cast<uint8_t>(d_bytes[3])};

	if (sendAndCheck(command, 13) == 0) {
		printk("Speed PID set to P: %f I: %f D: %f\n",
		       static_cast<double>(fourBytesToFloatPD(p_bytes)),
		       static_cast<double>(fourBytesToFloatI(i_bytes)),
		       static_cast<double>(fourBytesToFloatPD(d_bytes)));
	} else {
		printk("Failed to set speed PID\n");
	}
}

void Roller485::setPosition(float position)
{
	if (motor_mode != POSITION_MODE) {
		printk("Motor is not in position mode, setting position has no effect\n");
	}

	int8_t position_bytes[4] = {0, 0, 0, 0};
	floatToFourBytesPosition(position, position_bytes);
	uint8_t command[5] = {
		POSITION_MODE_TARGET_POSITION, static_cast<uint8_t>(position_bytes[0]),
		static_cast<uint8_t>(position_bytes[1]), static_cast<uint8_t>(position_bytes[2]),
		static_cast<uint8_t>(position_bytes[3])};

	printk("Position bytes: 0x%02X 0x%02X 0x%02X 0x%02X\n", command[4], command[3], command[2],
	       command[1]);

	if (sendAndCheck(command, 5) == 0) {
		printk("Position set to %.2f°\n",
		       static_cast<double>(fourBytesToFloatPosition(position_bytes)));
	} else {
		printk("Failed to set position\n");
	}
}

float Roller485::getPosition()
{
	if (motor_mode != POSITION_MODE) {
		printk("Motor is not in position mode, getting position has no effect\n");
		return 0;
	}

	uint8_t buffer[5] = {POSITION_MODE_READBACK_POSITION, 0, 0, 0, 0};
	i2c_write_read(i2c_dev, i2c_address, buffer, 1, &buffer[1], 4);
	int8_t data[4] = {static_cast<int8_t>(buffer[1]), static_cast<int8_t>(buffer[2]),
			  static_cast<int8_t>(buffer[3]), static_cast<int8_t>(buffer[4])};
	return fourBytesToFloatPosition(data);
}

void Roller485::setPositionPID(float p, float i, float d)
{
	if (motor_mode != POSITION_MODE) {
		printk("Motor is not in position mode, setting position PID has no effect\n");
		return;
	}

	int8_t p_bytes[4] = {0, 0, 0, 0};
	int8_t i_bytes[4] = {0, 0, 0, 0};
	int8_t d_bytes[4] = {0, 0, 0, 0};

	floatToFourBytesPD(p, p_bytes);
	floatToFourBytesI(i, i_bytes);
	floatToFourBytesPD(d, d_bytes);

	uint8_t command[13] = {POSITION_MODE_PID_CONFIG,         static_cast<uint8_t>(p_bytes[0]),
			       static_cast<uint8_t>(p_bytes[1]), static_cast<uint8_t>(p_bytes[2]),
			       static_cast<uint8_t>(p_bytes[3]), static_cast<uint8_t>(i_bytes[0]),
			       static_cast<uint8_t>(i_bytes[1]), static_cast<uint8_t>(i_bytes[2]),
			       static_cast<uint8_t>(i_bytes[3]), static_cast<uint8_t>(d_bytes[0]),
			       static_cast<uint8_t>(d_bytes[1]), static_cast<uint8_t>(d_bytes[2]),
			       static_cast<uint8_t>(d_bytes[3])};

	if (sendAndCheck(command, 13) == 0) {
		printk("Position PID set to P: %f I: %f D: %f\n",
		       static_cast<double>(fourBytesToFloatPD(p_bytes)),
		       static_cast<double>(fourBytesToFloatI(i_bytes)),
		       static_cast<double>(fourBytesToFloatPD(d_bytes)));
	} else {
		printk("Failed to set position PID\n");
	}
}

void Roller485::setCurrent(int32_t current)
{
	if (motor_mode != CURRENT_MODE) {
		printk("Motor is not in current mode, setting current has no effect\n");
	}
	int8_t current_bytes[4] = {0, 0, 0, 0};
	intToFourBytesCurrent(current, current_bytes);
	uint8_t command[5] = {CURRENT_MODE_CURRENT_TARGET, static_cast<uint8_t>(current_bytes[0]),
			      static_cast<uint8_t>(current_bytes[1]),
			      static_cast<uint8_t>(current_bytes[2]),
			      static_cast<uint8_t>(current_bytes[3])};

	printk("Current bytes: 0x%02X 0x%02X 0x%02X 0x%02X\n", command[4], command[3], command[2],
	       command[1]);

	if (sendAndCheck(command, 5) == 0) {
		printk("Current set to %d mA\n", fourBytesToIntCurrent(current_bytes));
	} else {
		printk("Failed to set current\n");
	}
}
int32_t Roller485::getEncoderCounter()
{
	if (motor_mode != ENCODER_MODE) {
		printk("Motor is not in encoder mode, getting encoder counter has no effect\n");
	}
	uint8_t buffer[5] = {ENCODER_COUNTER, 0, 0, 0, 0};
	i2c_write_read(i2c_dev, i2c_address, buffer, 1, &buffer[1], 4);
	printk("Encoder counter response: 0x%02X 0x%02X 0x%02X 0x%02X\n", buffer[4], buffer[3],
	       buffer[2], buffer[1]);
	int8_t data[4] = {static_cast<int8_t>(buffer[1]), static_cast<int8_t>(buffer[2]),
			  static_cast<int8_t>(buffer[3]), static_cast<int8_t>(buffer[4])};
	return fourBytesToInt(data);
}

} // namespace roller485
