
#ifndef INC_ULTRASONIC_H_
#define INC_ULTRASONIC_H_

#include "stm32f4xx_hal.h"


/* 센서 식별자 */
typedef enum
{
  US_FRONT = 0,
  US_LEFT  = 1,
  US_RIGHT = 2,
  US_COUNT = 3
} UsId_t;

/**
 * @brief  초음파 모듈 초기화
 * @note   - TIM2 CH1~CH3 Input Capture Start_IT는 main에서 수행
 *         - TIM11은 delay_us 용도로 이미 동작 중이어야 함
 */
void Ultrasonic_Init(TIM_HandleTypeDef *htim_ic);

/**
 * @brief  주기 실행 함수(논블로킹)
 * @note   내부에서 3개 센서를 라운드로빈으로 트리거하고,
 *         설정된 출력 주기마다 printf를 호출한다.
 * @param  nowMs HAL_GetTick() 값(현재 ms)
 */
void Ultrasonic_Task(uint32_t nowMs);

/**
 * @brief  Input Capture 콜백 처리
 * @note   HAL_TIM_IC_CaptureCallback()에서 그대로 넘겨주면 됨
 */
void Ultrasonic_OnCapture(TIM_HandleTypeDef *htim);

/**
 * @brief  마지막 측정 거리(cm) 조회
 */
uint16_t Ultrasonic_GetDistance(UsId_t id);

#endif /* INC_ULTRASONIC_H_ */
