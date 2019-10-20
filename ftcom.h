#pragma once

#include <stdbool.h>
#include <inttypes.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct FTCom FTCom;

FTCom* newFTCom(bool (*updatePinTimer)(const uint8_t pinState, const uint8_t duration));
void deleteFTCom(FTCom* ftcom);

void FTCom_timeElapsed(FTCom* ftcom);
bool FTCom_transmit(FTCom* ftcom, const uint8_t* data, const uint8_t numBytes);
uint8_t FTCom_getState(FTCom* ftcom);

bool (*updatePinTimer)(const uint8_t pinState, const uint8_t duration);
enum FTComState { FTCOM_IDLE = 0, FTCOM_TRANSMITTING = 1, FTCOM_RECEIVING = 2, FTCOM_TX_COLLISION = 3, FTCOM_INVALID_STATE = 4 };

#ifdef __cplusplus
}
#endif
