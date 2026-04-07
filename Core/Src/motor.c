
#include "motor.h"

/* 핀/포트 정의 */
#define MOTOR_PORT          GPIOC
#define MOTORA_IN1_PIN      GPIO_PIN_9
#define MOTORA_IN2_PIN      GPIO_PIN_8
#define MOTORB_IN3_PIN      GPIO_PIN_6
#define MOTORB_IN4_PIN      GPIO_PIN_5



void Motor_Init(void)
{
    /* PWM Start: Motor A/B */
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);

    Motor_Stop();
}


void Motor_Stop(void)
{
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0);

    HAL_GPIO_WritePin(MOTOR_PORT,
                      MOTORA_IN1_PIN | MOTORA_IN2_PIN | MOTORB_IN3_PIN | MOTORB_IN4_PIN,
                      GPIO_PIN_RESET);
}


/* 전진 (극성 반전 적용) */
void Motor_Forward(uint16_t speed)
{
    HAL_GPIO_WritePin(MOTOR_PORT, MOTORA_IN1_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MOTOR_PORT, MOTORA_IN2_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(MOTOR_PORT, MOTORB_IN3_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MOTOR_PORT, MOTORB_IN4_PIN, GPIO_PIN_SET);

    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, speed);
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, speed);
}

/* 후진 (극성 반전 적용) */
void Motor_Backward(uint16_t speed)
{
    HAL_GPIO_WritePin(MOTOR_PORT, MOTORA_IN1_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(MOTOR_PORT, MOTORA_IN2_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MOTOR_PORT, MOTORB_IN3_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(MOTOR_PORT, MOTORB_IN4_PIN, GPIO_PIN_RESET);

    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, speed);
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, speed);
}

/* 좌회전 (제자리 회전) */
void Motor_Left(uint16_t speed)
{
    /* Motor A 후진, Motor B 전진 */
    HAL_GPIO_WritePin(MOTOR_PORT, MOTORA_IN1_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MOTOR_PORT, MOTORA_IN2_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(MOTOR_PORT, MOTORB_IN3_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(MOTOR_PORT, MOTORB_IN4_PIN, GPIO_PIN_RESET);

    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, speed);
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, speed);
}

/* 우회전 (제자리 회전) */
void Motor_Right(uint16_t speed)
{
    /* Motor A 전진, Motor B 후진 */
    HAL_GPIO_WritePin(MOTOR_PORT, MOTORA_IN1_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(MOTOR_PORT, MOTORA_IN2_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MOTOR_PORT, MOTORB_IN3_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MOTOR_PORT, MOTORB_IN4_PIN, GPIO_PIN_SET);

    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, speed);
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, speed);
}



