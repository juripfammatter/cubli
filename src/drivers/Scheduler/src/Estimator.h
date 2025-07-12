//
// Created by juri on 09.07.25.
//

#ifndef SCHEDULER_ESTIMATOR_H
#define SCHEDULER_ESTIMATOR_H

#include <cstdio>
#include <zephyr/kernel.h>
#include <zephyr/sys/atomic.h>
#include <autoconf.h>
#include <array>
#include "Synchronization.h"

#include "../lib/eigen/Eigen/Core"

using Eigen::Matrix3f;
using Eigen::MatrixXf;
using Eigen::Vector2f;
using Eigen::VectorXf;

namespace estimator
{

class Estimator
{
      public:
	explicit Estimator();
	virtual ~Estimator() = default;

	void estimateState(const std::array<uint32_t, 8> &measurements);

      private:
	MatrixXf K, C, A, A_cl;
	VectorXf x_hat;
	Vector2f z_bar;
};

} // namespace estimator

#endif // SCHEDULER_ESTIMATOR_H
