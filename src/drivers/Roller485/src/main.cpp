#include <cstdio>
#include "Testing.h"

int main(void)
{
	const uint8_t addr = 0x64;
	roller485tests::Roller485Tests test = roller485tests::Roller485Tests(addr);

	test.resetMotorPosition();
	test.runPositionModeTest();
	test.runCurrentModeTest();
	test.runSpeedModeTest();
	test.runEncoderModeTest();
}
