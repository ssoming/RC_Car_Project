
#ifndef INC_AUTO_H_
#define INC_AUTO_H_


#include "stm32f4xx_hal.h"
#include "tim.h"   // htim3 사용
#include "motor.h"
#include "ultrasonic.h"

void Auto_Init(void);

void Auto_On(void);     // p → Auto Mode ON
void Auto_Off(void);    // c → Auto OFF + Stop

void Auto_Task(uint32_t nowMs);

#endif /* INC_AUTO_H_ */
