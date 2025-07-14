//
// Created by juri on 09.07.25.
//

#include "Estimator.h"

namespace estimator
{
Estimator::Estimator()
	: K(4, 3), C(3, 4), A(4, 4), A_cl(4, 4), x_hat(4), z_bar(3), tau(0.1f), tau_g(0.01f),
	  ts(0.005f)
{
	// Initialize the Kalman gain matrix
	K << 0.99999929, -0.00499999, 0., 0., 0.99999929, -0., 0.00000017, 0.00000663, -0.71488305,
		0.00000009, 0.00000332, 0.71505808;
	A << 0.9999414, 0.00992341, 0.00000007, 0.00000006, -0.01172083, 0.98468214, 0.00001321,
		0.00001235, 0.0014627, 0.00191159, 0.99533635, 0.00932247, 0.29254043, 0.38231887,
		-0.93273064, 0.86449367;
	C << 1., 0.005, 0., 0., 0., 1., 0., 0., 0., 0., -0.46620047, 0.93240093;
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
	float a0 = tau / (tau + ts);
	float b0 = ts / (tau + ts);
	float a0_g = tau_g / (tau_g + ts);
	float b0_g = ts / (tau_g + ts);

	ax = a0 * ax + b0 * ax_new;
	ay = a0 * ay + b0 * ay_new;
	gz = a0_g * gz + b0_g * gz_new;

	float theta = -atan2f(ay, ax) - 3.14f * 3.0f / 4.0f; // calculate angle from
							     // accelerometer

	// propagate through Kalman filter
	z_bar << theta, gz, motor_speed;
	x_hat = A_cl * x_hat + K * z_bar;

	for (int i = 0; i < 4; ++i) {
		atomic_set(&latest_state[i], (int32_t)(x_hat[i] * 1000000.0f));
	}
}
} // namespace estimator
