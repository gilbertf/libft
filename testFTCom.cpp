#include "ftcom.hpp"

#include <iostream>
#include <fstream>

using namespace std;

FTCom* tx;
FTCom* rx;

void printData(FTCom& ftcom, int numBytesReq) {
	cout << "Received from ";
	uint8_t data[numBytesReq];
	if (ftcom.getRxRole() == FTCom::MASTER) {
		cout << "Master  : ";
	} else {
		cout << "Slave   : ";
	}
	int numBytes = ftcom.readData(data, numBytesReq);
	for (int b = 0; b < numBytes; b++) {
		cout << (int) *(data + b) << " ";
	}
	cout << endl;
}

bool simUpdate(const uint8_t pinState, const uint8_t cycles) {
	int numBytes = rx->nextEdge(cycles);
	if (numBytes > 0) {
		printData(*rx, numBytes);
	}
	return true;
}

int main(int argc, char** argv) {
	tx = new FTCom(&simUpdate, FTCom::SLAVE);
	rx = new FTCom(&simUpdate, FTCom::MASTER);
	uint8_t txData[tx->maxNumBytes];
	uint8_t numTxBytes = 3;

	{
		txData[0] = 23;
		txData[1] = 33;
		txData[2] = 43;

		uint8_t rxData[rx->maxNumBytes];

		cout << "Transmitting as SLAVE : ";
		for (int b = 0; b < numTxBytes; b++) {
			cout << (int) txData[b] << " ";
		}
		cout << endl;

		tx->transmit(txData, numTxBytes);
		while (tx->getState() == FTCom::TRANSMITTING) {
			tx->timeElapsed();
		}
	}

	cout << endl;

	{
		txData[0] = 44;
		txData[1] = 55;
		txData[2] = 66;

		uint8_t rxData[rx->maxNumBytes];

		tx->setTxRole(FTCom::MASTER);
		cout << "Transmitting as MASTER: ";
		for (int b = 0; b < numTxBytes; b++) {
			cout << (int) txData[b] << " ";
		}
		cout << endl;

		tx->transmit(txData, numTxBytes);
		while (tx->getState() == FTCom::TRANSMITTING) {
			tx->timeElapsed();
		}
	}

	delete tx;
	delete rx;
}
