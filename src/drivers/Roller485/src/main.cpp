#include <cstdio>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <vector>
#include <zephyr/drivers/i2c.h>

int main(void)
{
	const struct device *i2c_dev = DEVICE_DT_GET(DT_NODELABEL(i2c1));
	if (!device_is_ready(i2c_dev)) {
		printk("I2C device not ready\n");
		return 0;
	}

//	int ret = i2c_reg_read_byte(i2c_dev, 0x64, 0x00);
//	if (ret != 0) {
//		printk("Failed to read register: %d\n", ret);
//		return 0;
//	}
		uint8_t addr = 0x64;
	uint8_t motor_enable_cmd[2] = {0x00, 0x01}; // Addr, Command
	uint8_t motor_disable_cmd[2] = {0x00, 0x00}; // Addr, Command
	uint8_t rgb_user_mode_cmd[2] = {0x33, 0x01}; // Addr, Command
	uint8_t rgb_blue_cmd[4] = {0x30, 0xFF, 0x00, 0x00}; // Addr, BGR
	uint8_t rgb_green_cmd[4] = {0x30, 0x00, 0xFF, 0x00}; // Addr, BGR

	uint8_t speed_current_cmd[5] = {0x50, 0x00, 0xF0, 0x00, 0x00}; // Addr, Command
	uint8_t zero_speed_cmd[5] = {0x40, 0x00, 0x00, 0x00, 0x00}; // Addr, Command

	uint8_t medium_speed_cmd[5] = {0x40, 0x00, 0xF0, 0x01, 0x00}; // Addr, Command

	printk("setting max_current to %d\n", (speed_current_cmd[1] + (speed_current_cmd[2] << 8) + (speed_current_cmd[3] << 16) + (speed_current_cmd[4] << 24))/100);
	printk("setting speed to %d\n", (medium_speed_cmd[1] + (medium_speed_cmd[2] << 8) + (medium_speed_cmd[3] << 16) + (medium_speed_cmd[4] << 24))/100);

	int ret_speed_current = i2c_write(i2c_dev, speed_current_cmd, sizeof(speed_current_cmd), addr);
	int ret_rgb_user_mode = i2c_write(i2c_dev, rgb_user_mode_cmd, sizeof(rgb_user_mode_cmd), addr);
	if (ret_rgb_user_mode != 0) {
		printk("Failed to send I2C command: %d\n", ret_rgb_user_mode);
	}
	while(true) {
		int ret = i2c_write(i2c_dev, motor_disable_cmd, sizeof(motor_disable_cmd), addr);
		if (ret != 0) {
			printk("Failed to send I2C command: %d\n", ret);
		}
		int ret_rgb = i2c_write(i2c_dev, rgb_blue_cmd, sizeof(rgb_blue_cmd), addr);
		if (ret_rgb != 0) {
			printk("Failed to send I2C command: %d\n", ret_rgb);
		}
		k_sleep(K_SECONDS(1));


		int ret2 = i2c_write(i2c_dev, motor_enable_cmd, sizeof(motor_enable_cmd), addr);
		if (ret2 != 0) {
			printk("Failed to send I2C command: %d\n", ret);
		}
		int ret2_rgb = i2c_write(i2c_dev, rgb_green_cmd, sizeof(rgb_green_cmd), addr);
		if (ret2_rgb != 0) {
			printk("Failed to send I2C command: %d\n", ret_rgb);
		}

		int ret_speed = i2c_write(i2c_dev, medium_speed_cmd, sizeof(medium_speed_cmd), addr);

		k_sleep(K_SECONDS(5));
		ret_speed = i2c_write(i2c_dev, zero_speed_cmd, sizeof(zero_speed_cmd), addr);
		k_sleep(K_SECONDS(4));
	}

	return 0;
}

