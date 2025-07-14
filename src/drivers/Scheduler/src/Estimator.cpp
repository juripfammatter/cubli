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
	K << 0.99996064, -0.00006549, 0.00714329, 0.01309727, 0.00750633, -0.07347193, 0.02404322,
		0.94852522;
	A << 0.91163033, 0.00651352, 0.00033522, 0.00001422, -17.67393427, 0.30270451, 0.06704327,
		0.00284405, 2.00799836, 0.0792178, 0.96585826, 0.00816496, 401.59967223, 15.8435596,
		-6.82834732, 0.63299241;
	C << 1.00000293, 0.00500001, 0., 0., -0.00049736, -0.00000249, -2.65251989, 0.84880637;
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
