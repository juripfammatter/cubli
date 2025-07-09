//
// Created by juri on 09.07.25.
//

#include "Estimator.h"

namespace estimator
{
Estimator::Estimator() : K(3, 2), C(2, 3)
{
	// Initialize the Kalman gain matrix
	K << 0.70651668, -0.70652081, 0.99958237, 0.00041399, 0.00041691, 0.9995853;
	A << 1.00000586, 0.01000003, 0.0, 0.00117191, 1.00000586, 0.0, -0.00117191, -0.00000586,
		1.0;
	C << 0.00058595, 1.00000293, 0.0, -0.00058595, -0.00000293, 1.0;
	A_cl = (A - K * C);
	x_hat.setZero();
	z_bar.setZero();
}

void Estimator::estimateState(const std::array<uint32_t, 8> &measurements)
{
	float gz = float((int32_t)measurements[6]) / 1000000.0f;
	float motor_speed = float((int32_t)measurements[7]) / 1000000.0f;
	z_bar << gz, motor_speed;
	x_hat = A_cl * x_hat + K * z_bar;

	for (int i = 0; i < 3; ++i) {
		atomic_set(&latest_state[i], (int32_t)(x_hat[i] * 1000000.0f));
	}
}
} // namespace estimator
