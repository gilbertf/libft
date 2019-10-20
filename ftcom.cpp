#include "ftcom.hpp"
#include <math.h>

using namespace std;

extern "C" {
	FTCom* newFTCom(bool (*updatePinTimer)(const uint8_t pinState, const uint8_t duration)) {
		return new FTCom(updatePinTimer);
	}

	void deleteFTCom(FTCom* ftcom) {
		delete ftcom;
	}


	void FTCom_timeElapsed(FTCom* ftcom) {
		ftcom->timeElapsed();
	}

	bool FTCom_transmit(FTCom* ftcom, const uint8_t* data, const uint8_t numBytes) {
		return ftcom->transmit(data, numBytes);
	}

	void FTCom_setTxRole(FTCom* ftcom, uint8_t role) {
		ftcom->setTxRole((FTCom::Role) role);
	}

	uint8_t FTCom_getState(FTCom* ftcom) {
		return ftcom->getState();
	}

	uint8_t FTCom_getMaxNumBytes(FTCom* ftcom) {
		return ftcom->maxNumBytes;
	}

}


FTCom::FTCom(bool(*updatePinTimerIn)(uint8_t , uint8_t), const Role txRoleIn, const uint8_t rxOFIn) {
	/*** Tx ***/
	txRole = txRoleIn;
	updatePinTimer = updatePinTimerIn;
	txState = WAIT_DATA;
	txBuffer = new uint8_t[maxNumBytes];

	/*** Rx ***/
	rxBuffer = new uint8_t[maxNumBytes];
	rxState = START_0;
	rxOF = rxOFIn;
	rxErr = 0;
}

FTCom::~FTCom() {
	delete[] txBuffer;
	delete[] rxBuffer;
}

void FTCom::setTxRole(Role txRoleIn) {
	txRole = txRoleIn;
}

FTCom::Role FTCom::getRxRole() {
	return rxRole;
}

uint16_t FTCom::getRxErrors(const bool clear) {
	uint16_t tmp = rxErr;
	if (clear) {
		rxErr = 0;
	}
	return tmp;
}

FTCom::State FTCom::getState() {
	if (txState == WAIT_DATA && rxState == START_0) {
		return IDLE;
	} else if (txState == COLLISION) {
		return TX_COLLISION;
	} else if (txState == WAIT_DATA) {
		return  RECEIVING;
	} else if (rxState == START_0) {
		return TRANSMITTING;
	}
	return INVALID_STATE;
}

bool FTCom::transmit(const uint8_t* txBufferIn, const uint8_t numBytes) {
	if ((getState() == IDLE) && (numBytes <= maxNumBytes)) {
		txNumBits = numBytes * 8;
		for (int b = 0; b < numBytes; b++) {
			txBuffer[b] = txBufferIn[b];
		}
		txState = START_0;
		txBitIdx = 0;
		timeElapsed();
		return true;
	}
	return false;
}

bool FTCom::checkPuls(const uint16_t realIn, const uint8_t expected, const bool allowLarger) {
	uint8_t real = round((double) realIn / (double) rxOF);
	if (expected - 1 <= real && (expected + 1 >= real || allowLarger)) {
		return true;
	}
	return false;
}

int8_t FTCom::readData(uint8_t* dataOut, const uint8_t bufferLen) {
	if (rxState == WAIT_DATA) {
		uint8_t numBytes = rxBitIdx / 8;
		if (numBytes > 0) {
			if (numBytes <= bufferLen) {
				for (int byteIdx = 0; byteIdx < numBytes; byteIdx++) {
					*(dataOut + byteIdx) = rxBuffer[byteIdx];
				}
				rxState = START_0;
				return numBytes;
			} else {
				return -1;
			}
		}
	}
	return 0;
}

void FTCom::zerroRxBuffer() {
	for (int b = 0; b < maxNumBytes; b++) {
		*(rxBuffer + b) = 0;
	}
}

int8_t FTCom::nextEdge(const uint16_t duration) {
	if (txState != WAIT_DATA) {
		return -1;
	}

	switch (rxState) {
		case START_0:
			if (checkPuls(duration, 8)) { // <11111111>
				rxState = START_1;
				rxLastBit = 0;
				rxBitIdx = 0;
				zerroRxBuffer();
			} else {
				rxErr++;
			}
			break;
		case START_1:
			if (checkPuls(duration, 4)) { // 11111111<0000>1....
				rxState = START_2;
				rxRole = SLAVE;
			} else if (checkPuls(duration, 9)) { //11111111<000000000>1...
				rxState = START_2;
				rxRole = MASTER;
			} else {
				rxErr++;
				rxState = START_0;
			}
			break;
		case START_2:
			if (checkPuls(duration, 2)) { // ...0<11>0... state change with end of header
				rxState = BIT_HALF;
			} else if (checkPuls(duration, 4)) { // ...0<11|11>0... end of header and two bit samples together 
				setNextBitInv(true);
				rxState = BIT_FULL;
			} else {
				rxErr++;
				rxState = START_0;
			}
			break;
		case BIT_HALF:
			if (checkPuls(duration, 2)) {
				rxState = BIT_FULL;
				setNextBitInv(false);
			} else {
				rxErr++;
				rxState = START_0;
			}
			break;
		case BIT_FULL:
			if (checkPuls(duration, 2)) {
				rxState = BIT_HALF;
			} else if (checkPuls(duration, 4)) {
				rxState = BIT_FULL;
				setNextBitInv(true);
			} else if (rxLastBit == 0 && checkPuls(duration, 8) && (rxBitIdx % 8 == 0)) {
				rxState = STOP;
			} else if (rxLastBit == 1 && checkPuls(duration, 8, true) && (rxBitIdx % 8 == 0)) {
				rxState = WAIT_DATA;
				return rxBitIdx / 8;
			} else {
				rxErr++;
				rxState = START_0;
			}
			break;
		case STOP:
			rxState = WAIT_DATA;
			return rxBitIdx / 8;
		default:
			break;
	}
	return 0;
}

bool FTCom::setNextBitInv(const bool invert) {
	if (rxBitIdx < maxNumBytes * 8) {
		uint8_t curBit = rxLastBit ^ invert;
		*(rxBuffer + (rxBitIdx / 8)) |= curBit << rxBitIdx % 8;
		rxLastBit = curBit;
		rxBitIdx++;
		return true;
	}
	return false;
}

void FTCom::timeElapsed() {
	bool hasNextBit = true;
	switch (txState) {
		case START_2:
		case BIT_FULL:
			if (txBitIdx < txNumBits) {
				txLastBit = txBit;
				txBit = (*(txBuffer+(txBitIdx / 8)) >> (txBitIdx % 8)) & 1;
				txBitIdx++;
			} else {
				hasNextBit = false;
			}
			break;
		default:
			break;
	}

	switch (txState) {
		case WAIT_DATA:
			break;
		case START_0:
			txState = START_1;
			if (!updatePinTimer(0, 8)) {
				txState = COLLISION;
			}
			break;
		case START_1:
			txState = START_2;
			if (txRole == MASTER) {
				if (!updatePinTimer(1, 9)) {
					txState = COLLISION;
				}
			} else {
				if (!updatePinTimer(1, 4)) {
					txState = COLLISION;
				}
			}
			break;
		case START_2:
			if (txBit == 0) {
				txState = BIT_HALF;
				if (!updatePinTimer(0, 2)) {
					txState = COLLISION;
				}
			} else {
				txState = BIT_FULL;
				if (!updatePinTimer(0, 4)) {
					txState = COLLISION;
				}
			}
			break;
		case BIT_HALF:
			txState = BIT_FULL;
			if (!updatePinTimer(txBit ^ 1, 2)) {
				txState = COLLISION;
			}
			break;
		case BIT_FULL:
			if (hasNextBit) {
				if (txLastBit == txBit) {
					txState = BIT_HALF;
					if (!updatePinTimer(txBit, 2)) {
						txState = COLLISION;
					}
				} else {
					txState = BIT_FULL;
					if (!updatePinTimer(txBit ^ 1, 4)) {
						txState = COLLISION;
					}
				}
			} else {
				if (txBit == 0) {
					txState = STOP;
				} else {
					txState = WAIT_DATA;
				}
				if (!updatePinTimer(txBit, 8)) {
					txState = COLLISION;
				}
			}
			break;
		case STOP:
			txState = WAIT_DATA;
			if (!updatePinTimer(1, 0)) {
				txState = COLLISION;
			}
			break;
		default:
			break;
	}
}
