//
// Created by juri on 09.07.25.
//

#include "Estimator.h"

namespace estimator
{
Estimator::Estimator()
	: K(4, 2), C(2, 4), A(4, 4), A_cl(4, 4), x_hat(4), z_bar(2), tau(0.1f), ts(0.005f)
{
	// Initialize the Kalman gain matrix
	K << 0.99746916, -0.00023407, 0.50602609, 0.04681492, 0.09122407, -0.71211508, 0.04561204,
		0.71644207;
	A << 0.99998684, 0.00996368, 0.00000002, 0.00000006, -0.00263149, 0.99273541, 0.00000426,
		0.00001248, 0.0003284, 0.00090659, 0.99533746, 0.00932245, 0.06567939, 0.18131709,
		-0.93250714, 0.86449043;
	C << 1., 0.005, 0., 0., 0., 0., -0.46620047, 0.93240093;
	A_cl = (A - K * C);
	x_hat.setZero();
	z_bar.setZero();
}

void Estimator::estimateState(const std::array<uint32_t, 8> &measurements)
{
	// get measurements
	float ax_new = float((int32_t)measurements[1]) / 1000000.0f;
	float ay_new = float((int32_t)measurements[2]) / 1000000.0f;
	float gz_new = float((int32_t)measurements[6]) / 1000000.0f;
	float motor_speed = float((int32_t)measurements[7]) / 1000000.0f;

	// complementary filter
	float a0 = tau / (tau + ts); // 5 ms sample time
	float b0 = ts / (tau + ts);

	ax = a0 * ax + b0 * ax_new;
	ay = a0 * ay + b0 * ay_new;
	gz = a0 * gz + tau * b0 * gz_new;

	float theta = -atan2f(ay, ax) - gz - 3.14f * 3.0f / 4.0f; // calculate angle from
								  // accelerometer

	// propagate through Kalman filter
	z_bar << theta, motor_speed;
	x_hat = A_cl * x_hat + K * z_bar;

	for (int i = 0; i < 4; ++i) {
		atomic_set(&latest_state[i], (int32_t)(x_hat[i] * 1000000.0f));
	}
}
} // namespace estimator
