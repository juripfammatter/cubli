/*
* Copyright (c) 2021 Bosch Sensortec GmbH
*
* SPDX-License-Identifier: Apache-2.0
*/

#include <cstdio>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <vector>
#include <array>
#include "../lib/eigen/Eigen/Core"

using Eigen::MatrixXf;
using Eigen::Matrix3f;
using Eigen::Vector3f;
using Eigen::Vector2f;

const int N = 1000;

void dump_measurements(std::vector<std::array<float, 7>> &measurements, std::vector<Vector3f> &state_estimates);

int main(void)
{
	const struct device *const dev = DEVICE_DT_GET_ONE(bosch_bmi270);
	struct sensor_value acc[3], gyr[3];
	struct sensor_value full_scale, sampling_freq, oversampling;

	if (!device_is_ready(dev)) {
	       printf("Device %s is not ready\n", dev->name);
	       return 0;
	}

	printf("Device %p name is %s\n", dev, dev->name);

	/* Setting scale in G, due to loss of precision if the SI unit m/s^2
	* is used
	*/
	full_scale.val1 = 2;            /* G */
	full_scale.val2 = 0;
	sampling_freq.val1 = 100;       /* Hz. Performance mode */
	sampling_freq.val2 = 0;
	oversampling.val1 = 1;          /* Normal mode */
	oversampling.val2 = 0;

	sensor_attr_set(dev, SENSOR_CHAN_ACCEL_XYZ, SENSOR_ATTR_FULL_SCALE,
		       &full_scale);
	sensor_attr_set(dev, SENSOR_CHAN_ACCEL_XYZ, SENSOR_ATTR_OVERSAMPLING,
		       &oversampling);
	/* Set sampling frequency last as this also sets the appropriate
	* power mode. If already sampling, change to 0.0Hz before changing
	* other attributes
	*/
	sensor_attr_set(dev, SENSOR_CHAN_ACCEL_XYZ,
		       SENSOR_ATTR_SAMPLING_FREQUENCY,
		       &sampling_freq);


	/* Setting scale in degrees/s to match the sensor scale */
	full_scale.val1 = 500;          /* dps */
	full_scale.val2 = 0;
	sampling_freq.val1 = 100;       /* Hz. Performance mode */
	sampling_freq.val2 = 0;
	oversampling.val1 = 1;          /* Normal mode */
	oversampling.val2 = 0;

	sensor_attr_set(dev, SENSOR_CHAN_GYRO_XYZ, SENSOR_ATTR_FULL_SCALE,
		       &full_scale);
	sensor_attr_set(dev, SENSOR_CHAN_GYRO_XYZ, SENSOR_ATTR_OVERSAMPLING,
		       &oversampling);
	/* Set sampling frequency last as this also sets the appropriate
	* power mode. If already sampling, change sampling frequency to
	* 0.0Hz before changing other attributes
	*/
	sensor_attr_set(dev, SENSOR_CHAN_GYRO_XYZ,
		       SENSOR_ATTR_SAMPLING_FREQUENCY,
		       &sampling_freq);


	printk("starting measurements\n");

	MatrixXf K(3,2);
	K << 	0.7064475, -0.7064507,
		0.9994832, 0.0004139,
		0.0004168,  0.9994862;
	Matrix3f A;
	A << 	1.00000586,  0.0100000293,  0.0,
		0.00117190537,  1.00000586,  0.0,
		-0.00117190537, -0.0000058595268, 1.0;

	MatrixXf C(2,3);
	C << 	0.000585952684,  1.00000293,  0.0,
		-0.000585952684, -0.00000292976342,  1.0;
	Matrix3f A_cl;
	A_cl = (A-K*C);

	// vector is allocated on heap
	std::vector<std::array<float, 7>> measurements(N, {{0.0}});
	std::vector<Vector3f> x_hat(N, Vector3f::Zero());
	Vector2f z_bar;

	x_hat[0] << 0.0, 0.0, 0.0; // initial state
	for (int i = 0; i < N; ++i) {

		k_sleep(K_MSEC(10));

		sensor_sample_fetch(dev);

		sensor_channel_get(dev, SENSOR_CHAN_ACCEL_XYZ, acc);
		sensor_channel_get(dev, SENSOR_CHAN_GYRO_XYZ, gyr);

		//wall time
		measurements[i][0] = float(k_uptime_get()) / 1000.0f; // convert to seconds
		measurements[i][1] = float(acc[0].val1) + float(acc[0].val2) / 1000000.0f;
		measurements[i][2] = float(acc[1].val1) + float(acc[1].val2) / 1000000.0f;
		measurements[i][3] = float(acc[2].val1) + float(acc[2].val2) / 1000000.0f;
		measurements[i][4] = float(gyr[0].val1) + float(gyr[0].val2) / 1000000.0f;
		measurements[i][5] = float(gyr[1].val1) + float(gyr[1].val2) / 1000000.0f;
		measurements[i][6] = float(gyr[2].val1) + float(gyr[2].val2) / 1000000.0f;

		if (i > 0) {
			// simple state update
			z_bar << measurements[i][6], 0.0; // acc x, y
			x_hat[i] = A_cl* x_hat[i-1] + K * z_bar;
		}
	}
	dump_measurements(measurements, x_hat);

	return 0;
}

void dump_measurements(std::vector<std::array<float, 7>> &measurements, std::vector<Vector3f> &state_estimates){
	for (int i = 0; i < N; ++i){
	       Vector3f state = state_estimates[i];
	       std::array m = measurements[i];
	       // print the measurements and state estimates
	       printk("time: [%0.6f], "
			      "imu: [%0.6f, %0.6f, %0.6f, %0.6f, %0.6f, %0.6f],"
			      "state: [%0.6f, %0.6f, %0.6f]\n",
			      double(m[0]),
			      double(m[1]), double(m[2]), double(m[3]),
			      double(m[4]), double(m[5]), double(m[6]),
			      double(state[0]), double(state[1]), double(state[2]));
       }

       k_sleep(K_SECONDS(2));
}