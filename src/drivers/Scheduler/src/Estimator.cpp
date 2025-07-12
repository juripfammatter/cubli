//
// Created by juri on 09.07.25.
//

#include "Estimator.h"

namespace estimator
{
Estimator::Estimator() : K(4, 2), C(2, 4), A(4, 4), A_cl(4, 4), x_hat(4)
{
	// Initialize the Kalman gain matrix
	K << -0.03163364, 0.00498443, 1.00001507, -0.00000295, -0.98244586, -0.04599809,
		-3.07015894, 1.03438389;
	A << 0.96194636, 0.00770723, 0.00000093, 0.00000005, -7.61072857, 0.54144516, 0.00018593,
		0.00000955, 0.86475484, 0.05209536, 0.97345368, 0.00848698, 172.95096811,
		10.41907149, -5.30926444, 0.69739577;
	C << 0.00058595, 1.00000293, 0., 0., -0.00049736, -0.00000249, -2.65251989, 0.84880637;
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

	for (int i = 0; i < 4; ++i) {
		atomic_set(&latest_state[i], (int32_t)(x_hat[i] * 1000000.0f));
	}
}
} // namespace estimator
