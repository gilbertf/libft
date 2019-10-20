#include "ftcom.h"

bool simUpdate(const uint8_t pinState, const uint8_t cycles) {
	printf("State %d for %d cycles\n", pinState, cycles);
	return true;
}

int main() {
	struct FTCom* ftcom = newFTCom(&simUpdate);

	uint8_t txData[FTCom_getMaxNumBytes(ftcom)];
	uint8_t numTxBytes = 3;
	txData[0] = 23;
	txData[1] = 33;
	txData[2] = 43;

	FTCom_transmit(ftcom, txData, numTxBytes);
	while (FTCom_getState(ftcom) == FTCOM_TRANSMITTING) {
		FTCom_timeElapsed(ftcom);
	}

}
