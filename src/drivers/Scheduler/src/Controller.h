//
// Created by juri on 09.07.25.
//

#ifndef SCHEDULER_CONTROLLER_H
#define SCHEDULER_CONTROLLER_H

#include <cstdio>
#include <zephyr/kernel.h>
#include <zephyr/sys/atomic.h>
#include <autoconf.h>
#include <array>
#include "Synchronization.h"
#include "Roller485.h"

#include "../lib/eigen/Eigen/Core"

using Eigen::Matrix3f;
using Eigen::MatrixXf;
using Eigen::Vector2f;
using Eigen::Vector3f;
using Eigen::VectorXf;

namespace controller
{

class Controller
{
      public:
	explicit Controller(uint32_t i2c_address);
	virtual ~Controller();

	void compute_input(VectorXf &reference, VectorXf &state, VectorXf &input);

	roller485::Roller485 motor;
	MatrixXf F; // controller gain matrix

      private:
};

} // namespace controller

#endif // SCHEDULER_CONTROLLER_H
