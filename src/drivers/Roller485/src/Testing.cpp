//
// Created by juri on 08.07.25.
//

#include "Testing.h"

namespace roller485tests
{

Roller485Tests::Roller485Tests(uint8_t i2c_address) : motor(i2c_address)
{
	printk("Roller485Tests initialized with I2C address: 0x%02X\n", i2c_address);
	printk("-------------Starting tests-------------\n");
	k_sleep(K_MSEC(100));
}

Roller485Tests::~Roller485Tests()
{
	printk("-------------Tests finished-------------\n");
}

void Roller485Tests::resetMotorPosition()
{
	motor.setMode(roller485::Roller485::POSITION_MODE);
	motor.setMaxCurrent(200.0f, roller485::Roller485::POSITION_MODE);
	k_sleep(K_MSEC(500));

	printk("Resetting motor position\n");
	motor.enableMotor();
	motor.setPositionPID(0.05f, 1.0f, 400.0f);
	motor.setPosition(0);
	k_sleep(K_MSEC(5000));
}
void Roller485Tests::runPositionModeTest()
{
	printk("-------------Position mode test-------------\n");
	motor.setMode(roller485::Roller485::POSITION_MODE);
	motor.setMaxCurrent(200.0f, roller485::Roller485::POSITION_MODE);
	k_sleep(K_MSEC(500));

	motor.setPositionPID(10.0f, 5.0f, 2000.0f);
	for (int i = 0; i < 5; i++) {

		motor.setPosition(360.5f);
		k_sleep(K_MSEC(2000));
		printk("Position: %.2f\n", static_cast<double>(motor.getPosition()));
		k_sleep(K_MSEC(100));
		motor.setPosition(-360.0f);
		k_sleep(K_MSEC(2000));
		printk("Position: %.2f\n", static_cast<double>(motor.getPosition()));
		k_sleep(K_MSEC(100));
	}
	motor.setPosition(0);
	k_sleep(K_MSEC(1000));
	motor.disableMotor();
}
void Roller485Tests::runCurrentModeTest()
{
	printk("-------------Current mode test-------------\n");
	motor.setMode(roller485::Roller485::CURRENT_MODE);
	motor.enableMotor();
	for (int i = 0; i < 5; i++) {

		motor.setCurrent(100.0f);
		k_sleep(K_SECONDS(1));
		printk("Current: %.2f mA\n", static_cast<double>(motor.getCurrent()));
		printk("Temperature: %.2f C\n",
		       static_cast<double>(motor.getInternalTemperature()));
		motor.setCurrent(-100.0f);
		k_sleep(K_SECONDS(1));
		printk("Current: %.2f mA\n", static_cast<double>(motor.getCurrent()));
	}
	motor.setCurrent(0.0f);
	k_sleep(K_SECONDS(3));
	motor.disableMotor();
}
void Roller485Tests::runSpeedModeTest()
{
	printk("-------------Speed mode test-------------\n");
	motor.setMode(roller485::Roller485::SPEED_MODE);
	motor.setMaxCurrent(1000, roller485::Roller485::SPEED_MODE);

	k_sleep(K_MSEC(500));
	motor.enableMotor();
	motor.setSpeedPID(15.0f, 0.0001f, 400.0f);
	for (int i = 0; i < 5; i++) {

		motor.setSpeed(1000.0f);
		k_sleep(K_MSEC(1000));
		printk("Speed: %f\n", static_cast<double>(motor.getSpeed()));
		printk("Temperature: %.2f C\n",
		       static_cast<double>(motor.getInternalTemperature()));

		k_sleep(K_MSEC(100));
		motor.setSpeed(-1000.0f);
		k_sleep(K_MSEC(1000));
		printk("Speed: %f\n", static_cast<double>(motor.getSpeed()));
		printk("Temperature: %.2f C\n",
		       static_cast<double>(motor.getInternalTemperature()));

		k_sleep(K_MSEC(100));
	}
	motor.setSpeed(0.0f);
	k_sleep(K_MSEC(1000));
	motor.disableMotor();
}
void Roller485Tests::runEncoderModeTest()
{
	printk("-------------Encoder mode test-------------\n");
	motor.setMode(roller485::Roller485::ENCODER_MODE);

	for (int i = 0; i < 50; i++) {
		printk("Encoder counter: %.2f\n", static_cast<double>(motor.getEncoderCounter()));
		k_sleep(K_MSEC(100));
	}
}
} // namespace roller485tests
