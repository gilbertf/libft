#include "ftcom.hpp"

#include <iostream>
#include <fstream>

using namespace std;

int cyclesAcc;
ofstream outFile;
int OF;

bool simUpdate(const uint8_t pinState, const uint8_t cycles) {
	outFile << cyclesAcc * OF << "\t" << (int) pinState * 5 << endl; //Scale to 5V and oversample with OF
	cyclesAcc += cycles;
	return true;
}

int main() {
	FTCom ftcom(&simUpdate);
	uint8_t data[ftcom.maxNumBytes];
	uint8_t numBytes;

	{
		outFile.open("plots/RemoteOn.test");
		OF = 2;
		numBytes = 2;

		cyclesAcc = 4992/OF;
		simUpdate(1, 0);

		ftcom.setTxRole(FTCom::SLAVE);
		data[0] = 23;
		data[1] = 1;
		ftcom.transmit(data, numBytes);
		while (ftcom.getState() == FTCom::TRANSMITTING) {
			ftcom.timeElapsed();
		}
		cyclesAcc += 1523/OF;

		ftcom.setTxRole(FTCom::MASTER);
		data[0] = 22;
		data[1] = 0;
		ftcom.transmit(data, numBytes);
		while (ftcom.getState() == FTCom::TRANSMITTING) {
			ftcom.timeElapsed();
		}
		
		cyclesAcc += 582/OF;
		ftcom.setTxRole(FTCom::SLAVE);
		data[0] = 35;
		data[1] = 255;
		ftcom.transmit(data, numBytes);
		while (ftcom.getState() == FTCom::TRANSMITTING) {
			ftcom.timeElapsed();
		}
		
		cyclesAcc += 1011/OF - 1;
		ftcom.setTxRole(FTCom::MASTER);
		data[0] = 35;
		data[1] = 84;
		ftcom.transmit(data, numBytes);
		while (ftcom.getState() == FTCom::TRANSMITTING) {
			ftcom.timeElapsed();
		}

		cyclesAcc += 1906/OF - 1;
		ftcom.setTxRole(FTCom::SLAVE);
		data[0] = 23;
		data[1] = 1;
		ftcom.transmit(data, numBytes);
		while (ftcom.getState() == FTCom::TRANSMITTING) {
			ftcom.timeElapsed();
		}

		outFile.close();
	}

	{
		outFile.open("plots/PowerOn.test");
		OF = 8;
		cyclesAcc = 1252/OF;
		simUpdate(1, 0);

		ftcom.setTxRole(FTCom::MASTER);
		numBytes = 2;
		data[0] = 22;
		data[1] = 1;
		ftcom.transmit(data, numBytes);
		while (ftcom.getState() == FTCom::TRANSMITTING) {
			ftcom.timeElapsed();
		}
		cyclesAcc += 7698/OF;

		ftcom.setTxRole(FTCom::SLAVE);
		numBytes = 1;
		data[0] = 29;
		ftcom.transmit(data, numBytes);
		while (ftcom.getState() == FTCom::TRANSMITTING) {
			ftcom.timeElapsed();
		}
		
		cyclesAcc += 4014/OF + 1;
		ftcom.setTxRole(FTCom::MASTER);
		numBytes = 5;
		data[0] = 29;
		data[1] = 0;
		data[2] = 120;
		data[3] = 5;
		data[4] = 24;
		ftcom.transmit(data, numBytes);
		while (ftcom.getState() == FTCom::TRANSMITTING) {
			ftcom.timeElapsed();
		}
		
		outFile.close();
	}	

	{
		outFile.open("plots/RemoteOff.test");
		OF = 4;
		cyclesAcc = 3391/OF;
		simUpdate(1, 0);

		ftcom.setTxRole(FTCom::SLAVE);
		numBytes = 2;
		data[0] = 23;
		data[1] = 172;
		ftcom.transmit(data, numBytes);
		while (ftcom.getState() == FTCom::TRANSMITTING) {
			ftcom.timeElapsed();
		}
		cyclesAcc += 186/OF;

		ftcom.setTxRole(FTCom::SLAVE);
		numBytes = 3;
		data[0] = 35;
		data[1] = 1;
		data[2] = 0;
		ftcom.transmit(data, numBytes);
		while (ftcom.getState() == FTCom::TRANSMITTING) {
			ftcom.timeElapsed();
		}
		cyclesAcc += 1123/OF + 1;

		ftcom.setTxRole(FTCom::MASTER);
		numBytes = 3;
		data[0] = 22;
		data[1] = 0;
		data[2] = 0;
		ftcom.transmit(data, numBytes);
		while (ftcom.getState() == FTCom::TRANSMITTING) {
			ftcom.timeElapsed();
		}
		cyclesAcc += 1003/OF;

		ftcom.setTxRole(FTCom::MASTER);
		numBytes = 2;
		data[0] = 35;
		data[1] = 84;
		ftcom.transmit(data, numBytes);
		while (ftcom.getState() == FTCom::TRANSMITTING) {
			ftcom.timeElapsed();
		}
		cyclesAcc += 1527/OF + 1;

		ftcom.setTxRole(FTCom::SLAVE);
		numBytes = 2;
		data[0] = 160;
		data[1] = 0;
		ftcom.transmit(data, numBytes);
		while (ftcom.getState() == FTCom::TRANSMITTING) {
			ftcom.timeElapsed();
		}
		outFile.close();
	}	
}
