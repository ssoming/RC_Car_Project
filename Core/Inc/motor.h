
#ifndef INC_MOTOR_H_
#define INC_MOTOR_H_

#include "stm32f4xx_hal.h"
#include "tim.h"   // htim3 사용

/* 모터 구동 API */
void Motor_Init(void);
void Motor_Forward(uint16_t speed);
void Motor_Backward(uint16_t speed);
void Motor_Stop(void);

void Motor_Left(uint16_t speed);
void Motor_Right(uint16_t speed);





#endif /* INC_MOTOR_H_ */
