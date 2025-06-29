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
	//	motor.setMaxCurrent(1000, roller485::Roller485::SPEED_MODE);
	motor.setMode(roller485::Roller485::CURRENT_MODE);
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
	motor.setMode(roller485::Roller485::SPEED_MODE);
	k_sleep(K_MSEC(500));
	motor.enableMotor();
	while (true) {

		//		motor.setMode(roller485::Roller485::ENCODER_MODE);
		//		int32_t encoder_value = motor.getEncoderCounter();
		//		printk("Encoder value: %d\n", encoder_value);

		motor.setMode(roller485::Roller485::SPEED_MODE);
		motor.setSpeed(400);
		k_sleep(K_MSEC(100));
	}
}
