//
// Created by juri on 09.07.25.
//

#include "Controller.h"

namespace controller
{
Controller::Controller(uint32_t i2c_address) : motor(roller485::Roller485(i2c_address)), F(1, 4)
{
	F << 761.18781699, 45.85593561, -0.01859334, -0.0009549;

	motor.setMode(roller485::Roller485::SPEED_MODE);
	motor.setMaxCurrent(10000.0f, roller485::Roller485::SPEED_MODE);
	motor.setSpeedPID(20.0f, 0.000f, 2000.0f);
	motor.enableMotor();
	motor.setSpeed(0.0f);
	k_sleep(K_MSEC(500));
}

Controller::~Controller()
{
	motor.disableMotor();
}
void Controller::compute_input(VectorXf &reference, VectorXf &state, VectorXf &input)
{
	// Simple proportional controller
	if (reference.size() != state.size() || 1 != input.size()) {
		printf("Error: Size mismatch in reference, state, or input vectors.\n");
		return;
	}
	input[0] = 0.0f;
	for (int i = 0; i < reference.size(); ++i) {
		input[0] -= F(0, i) * (state[i] - reference[i]);
	}
}
} // namespace controller
