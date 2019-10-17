#include "ct.hpp"

void CT::setState(const bool on) {
	if (on) {
		txBuf[0] = 23;
		txBuf[1] = 1;
	} else {
		txBuf[0] = 23;
		txBuf[1] = 172;
	}
	transmit(txBuf, 2);
};

void CT::setLevel(uint8_t level) {
	txBuf[0] = 35;
	txBuf[1] = 1;
	txBuf[2] = level;
	transmit(txBuf, 3);
};
