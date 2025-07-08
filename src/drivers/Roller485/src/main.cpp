#include <cstdio>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include "Roller485.h"

int main(void)
{
	const uint8_t addr = 0x64;
	roller485::Roller485 motor =
		roller485::Roller485(addr); // Initialize Roller485 with I2C address 0x64

	k_sleep(K_SECONDS(1));
	//	motor.setMode(roller485::Roller485::CURRENT_MODE);
	//	for (int i = 0; i < 5; i++) {
	//		motor.enableMotor();
	//		motor.setCurrent(100);
	//		k_sleep(K_SECONDS(3));
	//		motor.setCurrent(-100);
	//		k_sleep(K_SECONDS(3));
	//
	//		motor.setCurrent(0);
	//		k_sleep(K_SECONDS(2));
	//
	//		motor.disableMotor();
	//		k_sleep(K_SECONDS(3));
	//	}

	// Position mode test
	motor.setMode(roller485::Roller485::POSITION_MODE);
	motor.setMaxCurrent(500, roller485::Roller485::POSITION_MODE);

	k_sleep(K_MSEC(500));
	motor.enableMotor();
	motor.setPositionPID(10.0f, 5.0f, 2000.0f);
	for (int i = 0; i < 5; i++) {

		motor.setPosition(360.5f);
		k_sleep(K_MSEC(2000));
		printk("Position: %.2f\n", motor.getPosition());
		k_sleep(K_MSEC(100));
		motor.setPosition(-360.0f);
		k_sleep(K_MSEC(2000));
		printk("Position: %.2f\n", motor.getPosition());
		k_sleep(K_MSEC(100));
	}
	motor.setPosition(0);
	k_sleep(K_MSEC(1000));
	motor.disableMotor();

	// Speed mode test
	//	motor.setMode(roller485::Roller485::SPEED_MODE);
	//	motor.setMaxCurrent(500, roller485::Roller485::SPEED_MODE);
	//
	//	k_sleep(K_MSEC(500));
	//	motor.enableMotor();
	//	motor.setSpeedPID(15.0f, 0.0001f, 400.0f);
	//	for (int i = 0; i < 5; i++) {
	//
	//		motor.setSpeed(1000);
	//		k_sleep(K_MSEC(1000));
	//		printk("Speed: %d\n", motor.getSpeed());
	//		k_sleep(K_MSEC(100));
	//		motor.setSpeed(-1000);
	//		k_sleep(K_MSEC(1000));
	//		printk("Speed: %d\n", motor.getSpeed());
	//		k_sleep(K_MSEC(100));
	//	}
	//	motor.setSpeed(0);
	//	k_sleep(K_MSEC(1000));
	//	motor.disableMotor();
}
