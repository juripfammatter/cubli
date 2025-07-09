//
// Created by juri on 09.07.25.
//

#include "IMU.h"

namespace imu
{
IMU::IMU() : dev(DEVICE_DT_GET_ONE(bosch_bmi270))
{

	if (!device_is_ready(dev)) {
		printk("Device %s is not ready\n", dev->name);
		return;
	}

	printk("Device %p name is %s\n", dev, dev->name);

	struct sensor_value full_scale{}, sampling_freq{}, oversampling{};

	/* Setup accelerometer */
	full_scale.val1 = 2; /* G */
	full_scale.val2 = 0;
	sampling_freq.val1 = 100; /* Hz. Performance mode */
	sampling_freq.val2 = 0;
	oversampling.val1 = 1; /* Normal mode */
	oversampling.val2 = 0;

	sensor_attr_set(dev, SENSOR_CHAN_ACCEL_XYZ, SENSOR_ATTR_FULL_SCALE, &full_scale);
	sensor_attr_set(dev, SENSOR_CHAN_ACCEL_XYZ, SENSOR_ATTR_OVERSAMPLING, &oversampling);
	sensor_attr_set(dev, SENSOR_CHAN_ACCEL_XYZ, SENSOR_ATTR_SAMPLING_FREQUENCY, &sampling_freq);

	/* Setup gyroscope */
	full_scale.val1 = 500; /* dps */
	full_scale.val2 = 0;
	sampling_freq.val1 = 100; /* Hz. Performance mode */
	sampling_freq.val2 = 0;
	oversampling.val1 = 1; /* Normal mode */
	oversampling.val2 = 0;

	sensor_attr_set(dev, SENSOR_CHAN_GYRO_XYZ, SENSOR_ATTR_FULL_SCALE, &full_scale);
	sensor_attr_set(dev, SENSOR_CHAN_GYRO_XYZ, SENSOR_ATTR_OVERSAMPLING, &oversampling);
	sensor_attr_set(dev, SENSOR_CHAN_GYRO_XYZ, SENSOR_ATTR_SAMPLING_FREQUENCY, &sampling_freq);

	printk("IMU initialized successfully\n");
}
void IMU::getMeasurements(std::array<uint32_t, 7> &measurements)
{
	if (sensor_sample_fetch(dev)) {
		printf("Failed to fetch sensor sample\n");
		return;
	};

	sensor_channel_get(dev, SENSOR_CHAN_ACCEL_XYZ, acc);
	sensor_channel_get(dev, SENSOR_CHAN_GYRO_XYZ, gyr);

	measurements[0] = k_uptime_get();                      // / 1000.0f;  wall time in seconds
	measurements[1] = acc[0].val1 * 1000000 + acc[0].val2; // / 1000000.0f;
	measurements[2] = acc[1].val1 * 1000000 + acc[1].val2; // / 1000000.0f;
	measurements[3] = acc[2].val1 * 1000000 + acc[2].val2; // / 1000000.0f;
	measurements[4] = gyr[0].val1 * 1000000 + gyr[0].val2; // / 1000000.0f;
	measurements[5] = gyr[1].val1 * 1000000 + gyr[1].val2; // / 1000000.0f;
	measurements[6] = gyr[2].val1 * 1000000 + gyr[2].val2; // / 1000000.0f;
}

} // namespace imu
