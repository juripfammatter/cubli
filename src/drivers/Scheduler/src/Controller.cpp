//
// Created by juri on 09.07.25.
//

#include "Controller.h"

namespace controller
{
Controller::Controller(uint32_t i2c_address) : motor(roller485::Roller485(i2c_address))
{
	motor.setMode(roller485::Roller485::SPEED_MODE);
	motor.setSpeedPID(15.0f, 0.0001f, 400.0f);
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
	if (reference.size() != state.size() || reference.size() != input.size()) {
		printf("Error: Size mismatch in reference, state, or input vectors.\n");
		return;
	}

	for (int i = 0; i < reference.size(); ++i) {
		input[i] = reference[i];
	}
}
} // namespace controller
