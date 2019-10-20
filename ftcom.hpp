#pragma once

#include <inttypes.h>

/** Implementation of a serial protocol used by the company Festool e. g. for the communication of their vacuum cleaners from the CT series with a bluetooth remote control module.
 * Protocol specification, as far as currently known, are given in \"protospec.pdf\" .
 * The 4-pin connector on the vacuum cleaner has the following assignment with pin numbering from the top.:
 * 	1: +5V
 * 	2: unknown
 * 	3: Serial IO with a pull-up to 5V by the master device
 * 	4: GND */

class FTCom {
	public:
		/** State of the transceiver */
		enum State : uint8_t {
			/** Ready for transmitting and receiving */
			IDLE = 0,
			/** Receiving or waiting for user to read received data */
			TRANSMITTING = 1,
			/** Transmitting */
			RECEIVING = 2,
			/** Collision detected by user routine updatePinTimer when testing bus before changing its state */
			TX_COLLISION = 3,
			/** Better, this never happens */
			INVALID_STATE = 4
		};

		/** Allowed maximum is 32, due to bitIdx variable definition */
		static const uint8_t maxNumBytes = 6;
		
		enum Role : uint8_t {
			/** The device with an internal pull-up e. g. the vacuum cleaner */
			MASTER = 0,
			/** Communication partner with open-drain output */
			SLAVE = 1
		};

		~FTCom();

		/** Set transmitter role */
		void setTxRole(const Role txRole);

		/**
		 * \param updatePinTimer is a user-defined funtion for updating the mcu peripherals
		 * \param role is SLAVE for a device with no internal pull-up like the Festool bluetooth module */
		FTCom(bool(*updatePinTimer)(const uint8_t pinState , const uint8_t duration), Role role = SLAVE, const uint8_t rxOF = 1);

		/**
		 * To be called by the timer ISR
		 * \dotfile rxGraph.dot */
		void timeElapsed();

		/**
		 * Transmit new data block
		 * \param data Pointer to input data
		 * \param numBytes Number of bytes to be read from data
		 * \return true on success, false if transmit is in progress (data is not accepted) or numBytes exceeds maxNumBytes */
		bool transmit(const uint8_t* data, const uint8_t numBytes);

		/**
		 * State of transceiver */
		FTCom::State getState();

		/**
		 * To be called by client, when edge on serial io pin is detected, e. g. using port interrupt
		 * \return Number of bytes received (on last edge of receive sequence),
		 * 0: Receiving in progress,
		 * -1: Receiving not possible, transmitting in progress
		 * \dotfile txGraph.dot */
		int8_t nextEdge(const uint16_t duration);

		/**
		 * Copies received values to data
		 * \param dataOut Allocated memory block of bufferLen
		 * \param Size of buffer
		 * \return Number of bytes received, -1 if buffer is not large enough */
		int8_t readData(uint8_t* dataOut, const uint8_t bufferLen);

		/**
		 * Get role of received packet
		 * \return We expect the value to be the opposite of the txRole set. This function is useful when processing bus recordings. */
		Role getRxRole();

		/**
		 * Get error counter of receiver unit
		 * \param clear Reset counter on read */
		uint16_t getRxErrors(const bool clear = false);

	private:
		enum TrxState : uint8_t {START_0, START_1, START_2, BIT_HALF, BIT_FULL, STOP, WAIT_DATA, COLLISION};

		TrxState txState;
		Role txRole;
		uint8_t txBitIdx;
		uint8_t* txBuffer;
		uint8_t txNumBits;
		uint8_t txBit;
		uint8_t txLastBit;

		/**
		 * \param updatePinTimer is a used-defined function with the following two requirements
		 * a) Start a timer with given duration value
		 * b) On timer hit: Update the serial port pin to pinState and call timeElapsed()
		 *
		 * \param duration duration * 0.5 ms specifies the time to wait till changing the state of the serial io pin to pinState
		 * \param pinState is the new state of the serial io pin
		 * \return true: collision on bus detected
		 * */
		bool (*updatePinTimer)(const uint8_t pinState, const uint8_t duration);

		TrxState rxState;
		Role rxRole;
		uint8_t rxBitIdx;
		uint8_t* rxBuffer;
		uint8_t rxLastBit;
		uint16_t rxErr;
		uint8_t rxOF;

		bool checkPuls(const uint16_t real, const uint8_t expected, const bool allowLarger = false);
		bool setNextBitInv(const bool invert);
		void zerroRxBuffer();

};
