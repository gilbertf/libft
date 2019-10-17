#pragma once
#include "ftcom.hpp"

class CT : public FTCom {
	public:
		CT(bool(*updatePinTimer)(const uint8_t pinState , const uint8_t cycles), const Role role = SLAVE, const uint8_t rxOF = 1) : FTCom(updatePinTimer, role, rxOF) {};
		void setState(const bool on);
		void setLevel(uint8_t level);
	private:
		uint8_t txBuf[3];
};
