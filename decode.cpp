#include "ftcom.hpp"

#include <iostream>
#include <fstream>

using namespace std;

ifstream inFile;

bool simUpdate(const uint8_t pinState, const uint8_t cycles) {
	return true;
}

int decide(double value, double mean) {
	if (value > mean) {
		return 1;
	} else {
		return 0;
	}
}

void printData(FTCom& ftcom, int numBytesReq) {
	uint8_t data[numBytesReq];
	if (ftcom.getRxRole() == FTCom::MASTER) {
		cout << "Master: ";
	} else {
		cout << "Slave : ";
	}
	int numBytes = ftcom.readData(data, numBytesReq);
	for (int b = 0; b < numBytes; b++) {
		cout << (int) *(data + b) << " ";
	}
	cout << endl;
}

int main(int argc, char** argv) {
	if (argc <= 1 || argc >= 4) {
		cout << "Syntax: " << endl;
		cout << "\t" << argv[0] << " file" << endl;
		cout << "\t" << argv[0] << " file OF" << endl;
		return -1;
	}

	int OF = 2;

	if (argc == 3) {
		try {
			OF = stoi(argv[2]);
		} catch (...) {
			cout << "Unable to parse OF" << endl;
			return -1;
		}
	}

	inFile.open(argv[1]);
	if (!inFile.good()) {
		cout << "Unable to read from " << argv[1] << endl;
		return -1;
	}

	cout << "Reading " << argv[1] << " with OF of " << OF << endl;

	FTCom ftcom(&simUpdate, FTCom::SLAVE, OF);

	double time;
	double value;
	double mean;

	{
		bool firstRound = true;
		double minValue;
		double maxValue;
		while (inFile >> time >> value) {
			if (firstRound) {
				minValue = value;
				maxValue = value;
				firstRound = false;
			}
			if (value > maxValue) {
				maxValue = value;
			}
			if (value < minValue) {
				minValue = value;
			}
		}
		inFile.close();

		mean = minValue + abs(maxValue - minValue) / 2;

		cout << "Max: " << maxValue << " Min: " << minValue << endl;
		cout << "Decision level: " << mean << endl;
	}

	{
		bool firstRound = true;
		int lastCnt;
		int cnt = 0;
		int lastState;
		int state;
		inFile.open(argv[1]);
		while (inFile >> time >> value) {
			if (firstRound) {
				lastCnt = cnt;
				lastState = decide(value, mean);
				firstRound = false;
			}
			state = decide(value, mean);
			if (lastState != state) {
				int duration = cnt - lastCnt;
				int bytesRead = ftcom.nextEdge(duration);
				if (bytesRead > 0) {
					printData(ftcom, bytesRead);
				}
				lastState = state;
				lastCnt = cnt;
			}
			cnt++;
		}
		inFile.close();

		// Simulate final toggle
		int duration = cnt - lastCnt;
		int bytesRead = ftcom.nextEdge(duration);
		if (bytesRead > 0) {
			printData(ftcom, bytesRead);
		}
		cout << "Errors: " << ftcom.getRxErrors() << endl;
	}
}
