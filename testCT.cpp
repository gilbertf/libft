#include "ct.hpp"

#include <iostream>
#include <fstream>

using namespace std;

int cyclesAcc;
ofstream outFile;
int OF = 1;

bool simUpdate(const uint8_t pinState, const uint8_t cycles) {
	outFile << cyclesAcc * OF << "\t" << (int) pinState * 5 << endl; //Scale to 5V and oversample with 2
	cyclesAcc += cycles;
	return true;
}

int main() {
	CT ct(&simUpdate);

	ct.setTxRole(FTCom::SLAVE);
	outFile.open("plots/ctOff.test");
	ct.setState(0);
	while (ct.getState() == FTCom::TRANSMITTING) {
		ct.timeElapsed();
	}
	outFile.close();

	outFile.open("plots/ctOn.test");
	ct.setState(1);
	while (ct.getState() == FTCom::TRANSMITTING) {
		ct.timeElapsed();
	}
	outFile.close();
}
