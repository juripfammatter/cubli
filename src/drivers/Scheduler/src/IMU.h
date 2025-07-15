//
// Created by juri on 09.07.25.
//

#ifndef SCHEDULER_IMU_H
#define SCHEDULER_IMU_H

#include <cstdio>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <vector>
#include <array>

namespace imu
{

class IMU
{
      public:
	explicit IMU();
	virtual ~IMU() = default;

	void getMeasurements(std::array<uint32_t, 7> &measurements);

      private:
	const struct device *const dev;
	struct sensor_value acc[3]{}, gyr[3]{};
};

} // namespace imu

#endif // SCHEDULER_IMU_H
