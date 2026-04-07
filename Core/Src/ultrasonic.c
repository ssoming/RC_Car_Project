#include "ultrasonic.h"
#include "delay.h"
#include <stdio.h>

/* TRIG 핀 정의  */
#define FRONT_TRIG_PORT  GPIOA
#define FRONT_TRIG_PIN   GPIO_PIN_4

#define LEFT_TRIG_PORT   GPIOB
#define LEFT_TRIG_PIN    GPIO_PIN_0

#define RIGHT_TRIG_PORT  GPIOB
#define RIGHT_TRIG_PIN   GPIO_PIN_4

/* Echo는 TIM2 CH1/2/3 고정 가정 */
#define FRONT_CHANNEL TIM_CHANNEL_1
#define LEFT_CHANNEL  TIM_CHANNEL_2
#define RIGHT_CHANNEL TIM_CHANNEL_3

/* 트리거/출력 주기(ms) - 필요하면 여기만 수정 */
#define US_TRIG_INTERVAL_MS   30U
#define US_PRINT_INTERVAL_MS  200U

typedef struct
{
  GPIO_TypeDef *trigPort;
  uint16_t      trigPin;
  uint32_t      channel;

  uint16_t      rise;
  uint16_t      fall;
  uint16_t      echoUs;
  uint16_t      distanceCm;

  uint8_t       state;     /* 0: rising 대기, 1: falling 대기 */
} Ultrasonic_t;

/* 모듈 내부 상태 (외부에서 접근 불가) */
static Ultrasonic_t us[US_COUNT];
static TIM_HandleTypeDef *htim_ic = 0;

static uint32_t trigTickMs  = 0;
static uint32_t printTickMs = 0;
static uint8_t  trigIndex   = 0;

/* 상승/하강 캡처값으로 펄스폭 계산(16bit 오버플로우 고려) */
static uint16_t calc_pulse_us(uint16_t rise, uint16_t fall)
{
  if (fall >= rise) return (uint16_t)(fall - rise);
  return (uint16_t)((0xFFFF - rise) + fall + 1U);
}

/* 캡처 상태를 초기화하고 다음 측정을 rising에서 시작하도록 설정 */
static void reset_capture(TIM_HandleTypeDef *htim, Ultrasonic_t *s)
{
  s->state = 0U;
  __HAL_TIM_SET_CAPTUREPOLARITY(htim, s->channel, TIM_INPUTCHANNELPOLARITY_RISING);
}

static void trigger_sensor(const Ultrasonic_t *s)
{
  /* HC-SR04: TRIG HIGH 10us 이상 */
  HAL_GPIO_WritePin(s->trigPort, s->trigPin, GPIO_PIN_RESET);
  delay_us(1);

  HAL_GPIO_WritePin(s->trigPort, s->trigPin, GPIO_PIN_SET);
  delay_us(10);

  HAL_GPIO_WritePin(s->trigPort, s->trigPin, GPIO_PIN_RESET);
}

void Ultrasonic_Init(TIM_HandleTypeDef *htim)
{
  htim_ic = htim;

  us[US_FRONT] = (Ultrasonic_t){ FRONT_TRIG_PORT, FRONT_TRIG_PIN, FRONT_CHANNEL, 0,0,0,0,0 };
  us[US_LEFT]  = (Ultrasonic_t){ LEFT_TRIG_PORT,  LEFT_TRIG_PIN,  LEFT_CHANNEL,  0,0,0,0,0 };
  us[US_RIGHT] = (Ultrasonic_t){ RIGHT_TRIG_PORT, RIGHT_TRIG_PIN, RIGHT_CHANNEL, 0,0,0,0,0 };

  for (uint8_t i = 0; i < US_COUNT; i++)
  {
    reset_capture(htim_ic, &us[i]);
  }

  trigTickMs  = HAL_GetTick();
  printTickMs = HAL_GetTick();
  trigIndex   = 0;
}

//void Ultrasonic_Task(uint32_t nowMs)
//{
//  /* 1) 센서 트리거: 라운드로빈 (간섭 방지 목적) */
//  if ((uint32_t)(nowMs - trigTickMs) >= US_TRIG_INTERVAL_MS)
//  {
//    trigger_sensor(&us[trigIndex]);
//    trigIndex = (uint8_t)((trigIndex + 1U) % US_COUNT);
//    trigTickMs = nowMs;
//  }
//
//  /* 2) 출력 주기 */
//  if ((uint32_t)(nowMs - printTickMs) >= US_PRINT_INTERVAL_MS)
//  {
//    printf("left: %ucm / front: %ucm / right: %ucm\n",
//           (unsigned)us[US_LEFT].distanceCm,
//           (unsigned)us[US_FRONT].distanceCm,
//           (unsigned)us[US_RIGHT].distanceCm);
//
//    printTickMs = nowMs;
//  }
//}

void Ultrasonic_Task(uint32_t nowMs)
{
  if ((uint32_t)(nowMs - trigTickMs) >= US_TRIG_INTERVAL_MS)
  {
    trigger_sensor(&us[trigIndex]);
    trigIndex = (uint8_t)((trigIndex + 1U) % US_COUNT);
    trigTickMs = nowMs;
  }
}

void Ultrasonic_OnCapture(TIM_HandleTypeDef *htim)
{
  Ultrasonic_t *s = 0;

  if (htim->Instance != TIM2) return;

  /* 현재 인터럽트를 발생시킨 채널에 해당하는 센서를 선택 */
  if      (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) s = &us[US_FRONT];
  else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) s = &us[US_LEFT];
  else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3) s = &us[US_RIGHT];
  else return;

  /* 1) Rising에서 시작 → Rise 저장 후 Falling으로 전환 */
  if (s->state == 0U)
  {
    s->rise = (uint16_t)HAL_TIM_ReadCapturedValue(htim, s->channel);
    s->state = 1U;
    __HAL_TIM_SET_CAPTUREPOLARITY(htim, s->channel, TIM_INPUTCHANNELPOLARITY_FALLING);
    return;
  }

  /* 2) Falling 캡처 → 펄스폭/거리 계산 후 Rising으로 복구 */
  s->fall = (uint16_t)HAL_TIM_ReadCapturedValue(htim, s->channel);
  s->echoUs = calc_pulse_us(s->rise, s->fall);

  /* distance(cm) = echo_us / 58  (HC-SR04 근사식) */
  s->distanceCm = (uint16_t)(s->echoUs / 58U);

  reset_capture(htim, s);
}

uint16_t Ultrasonic_GetDistance(UsId_t id)
{
  return us[id].distanceCm;
}
