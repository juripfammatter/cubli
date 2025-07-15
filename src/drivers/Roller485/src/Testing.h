//
// Created by juri on 08.07.25.
//

#ifndef CPP_TEMPLATE_TESTING_H
#define CPP_TEMPLATE_TESTING_H

#endif // CPP_TEMPLATE_TESTING_H

#include <cstdio>
#include <cstdint>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include "Roller485.h"

namespace roller485tests
{

class Roller485Tests
{
      public:
	explicit Roller485Tests(uint8_t i2c_address);
	virtual ~Roller485Tests();

	void resetMotorPosition();
	void runPositionModeTest();
	void runCurrentModeTest();
	void runSpeedModeTest();
	void runEncoderModeTest();

      private:
	roller485::Roller485 motor;
};
} // namespace roller485tests
