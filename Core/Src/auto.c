//
//#include "auto.h"
//#include "motor.h"
//#include "ultrasonic.h"
//#include <stdio.h>
//
///* ================== 튜닝 파라미터 ================== */
////#define AUTO_SAFE_CM              25U
//#define AUTO_SPEED_FWD            850U
//#define AUTO_SPEED_TURN           800U
//#define AUTO_SPEED_ADJUST         500U  // 측면 보정용 낮은 속도 (뒷부분 긁힘 방지)
//
//#define AUTO_DECIDE_PERIOD_MS     30U
//
//#define AUTO_FWD_CM      30U   // 이 이상이면 직진
//#define AUTO_TURN_CM     25U   // 이 미만이면 회전(=회피 시작)
//#define AUTO_STOP_CM     15U   // 이 미만으로 3방향 다 막히면 정지
//
//#define AUTO_PRINT_PERIOD_MS   200U   // 방향/거리 출력 주기 (너무 짧으면 UART 버벅임)
//#define AUTO_TURN_HOLD_MS      200U  /* 회전 최소 유지 시간(회전이 약하면 250~350으로) */
//#define AUTO_SIDE_MARGIN_CM    7U    /* 좌/우 차이 마진(노이즈 	억제) */
//
//
///* --- [추가 파라미터] --- */
//#define AUTO_SIDE_DANGER_CM       10U   // (문제2 해결) 측면 벽과 너무 가까울 때의 기준 거리
//#define AUTO_SIDE_ADJUST_MS       80U  // (문제2 해결) 벽을 긁기 전 살짝 틀어주는(보정)
//
//
///* =================================================== */
//
//typedef enum
//{
//    ACT_IDLE = 0,
//    ACT_FWD,
//    ACT_LEFT,
//    ACT_RIGHT,
//    ACT_STOP
//} AutoAct_t;
//
//static uint8_t   g_autoEnabled     = 0U;
//static uint32_t  g_lastDecideMs    = 0U;
//
//static uint32_t  g_lastPrintMs     = 0U;
//static uint32_t  g_turnHoldUntilMs = 0U;
//static AutoAct_t g_act            = ACT_IDLE;
//
//static const char* act_str(AutoAct_t a)
//{
//    switch (a)
//    {
//        case ACT_FWD:   return "FWD";
//        case ACT_LEFT:  return "LEFT_TURN";
//        case ACT_RIGHT: return "RIGHT_TURN";
//        case ACT_STOP:  return "STOP";
//        default:        return "IDLE";
//    }
//}
//
//void Auto_Init(void)
//{
//    g_autoEnabled      = 0U;
//    g_lastDecideMs     = HAL_GetTick();
//
//    g_lastPrintMs      = HAL_GetTick();
//    g_turnHoldUntilMs  = 0U;
//    g_act              = ACT_IDLE;
//}
//
//void Auto_On(void)
//{
//    g_autoEnabled      = 1U;
//    g_lastDecideMs     = 0U;   /* 켜자마자 판단 */
//    g_lastPrintMs      = 0U;   /* 켜자마자 출력 1회 */
//    g_turnHoldUntilMs  = 0U;
//    g_act              = ACT_IDLE;
//}
//
//void Auto_Off(void)
//{
//    g_autoEnabled      = 0U;
//    g_turnHoldUntilMs  = 0U;
//    g_act              = ACT_STOP;
//    Motor_Stop();
//}
//
//
//void Auto_Task(uint32_t nowMs)
//{
//    if (g_autoEnabled == 0U) return;
//
//    if ((uint32_t)(nowMs - g_lastDecideMs) < AUTO_DECIDE_PERIOD_MS) return;
//    g_lastDecideMs = nowMs;
//
//    uint16_t front = Ultrasonic_GetDistance(US_FRONT);
//    uint16_t left  = Ultrasonic_GetDistance(US_LEFT);
//    uint16_t right = Ultrasonic_GetDistance(US_RIGHT);
//
//    /* --- [문제 1 해결] 넓은 공간 0cm 치환 로직 --- */
//    /* 거리가 너무 멀어 측정이 안 된 0cm를 매우 먼 거리(999cm)로 치환합니다. */
//    if (front == 0) front = 999;
//    if (left  == 0) left  = 999;
//    if (right == 0) right = 999;
//    /* --------------------------------------------- */
//
//    /* 1) 회전 유지시간이 남아있으면 이전 회전 방향 유지 */
//    if (nowMs < g_turnHoldUntilMs)
//    {
//        if (g_act == ACT_LEFT)       Motor_Left(AUTO_SPEED_TURN);
//        else if (g_act == ACT_RIGHT) Motor_Right(AUTO_SPEED_TURN);
//        goto PRINT;
//    }
//
//    /* 2) 막힘 판단(3방향 모두 위험) */
//    if ((front < AUTO_STOP_CM) && (left < AUTO_STOP_CM) && (right < AUTO_STOP_CM))
//    {
//        Motor_Stop();
//        g_act = ACT_STOP;
//        goto PRINT;
//    }
//
//    /* --- [문제 2 해결] 벽면 긁힘 방지 (측면 보정) --- */
//    /* 전방에 당장 충돌할 장애물이 없을 때만(front >= AUTO_TURN_CM) 측면을 확인합니다. */
////    if (front >= AUTO_TURN_CM)
////    {
////        /* 왼쪽 벽에 너무 가깝다면 우회전으로 살짝 튕겨냅니다 */
////        if (left < AUTO_SIDE_DANGER_CM && left < right)
////        {
////            Motor_Right(AUTO_SPEED_TURN);
////            g_act = ACT_RIGHT;
////            g_turnHoldUntilMs = nowMs + AUTO_SIDE_ADJUST_MS; // 아주 짧게만 회전(자세 제어)
////            goto PRINT;
////        }
////        /* 오른쪽 벽에 너무 가깝다면 좌회전으로 살짝 튕겨냅니다 */
////        else if (right < AUTO_SIDE_DANGER_CM && right < left)
////        {
////            Motor_Left(AUTO_SPEED_TURN);
////            g_act = ACT_LEFT;
////            g_turnHoldUntilMs = nowMs + AUTO_SIDE_ADJUST_MS; // 아주 짧게만 회전(자세 제어)
////            goto PRINT;
////        }
////    }
////    /* --------------------------------------------- */
//    /* --- [보정 로직 수정] --- */
//        if (front >= AUTO_TURN_CM) {
//            if (left < AUTO_SIDE_DANGER_CM && left < right) {
//                Motor_Right(AUTO_SPEED_ADJUST); // 낮은 속도로 보정
//                g_act = ACT_RIGHT;
//                g_turnHoldUntilMs = nowMs + AUTO_SIDE_ADJUST_MS; // 짧은 시간 적용
//                goto PRINT;
//            }
//            else if (right < AUTO_SIDE_DANGER_CM && right < left) {
//                Motor_Left(AUTO_SPEED_ADJUST); // 낮은 속도로 보정
//                g_act = ACT_LEFT;
//                g_turnHoldUntilMs = nowMs + AUTO_SIDE_ADJUST_MS;
//                goto PRINT;
//            }
//        }
//
//    /* 3) 전방 충분 → 직진(복귀) */
//    if (front >= AUTO_FWD_CM)
//    {
//        Motor_Forward(AUTO_SPEED_FWD);
//        g_act = ACT_FWD;
//        goto PRINT;
//    }
//
//    /* 4) 전방이 회피 구간이면(프론트 < AUTO_TURN_CM) 장애물 회피 강한 회전 */
//    if (front < AUTO_TURN_CM)
//    {
//        if (right > (uint16_t)(left + AUTO_SIDE_MARGIN_CM))
//        {
//            Motor_Right(AUTO_SPEED_TURN);
//            g_act = ACT_RIGHT;
//            g_turnHoldUntilMs = nowMs + AUTO_TURN_HOLD_MS; // 큰 각도 회전
//        }
//        else if (left > (uint16_t)(right + AUTO_SIDE_MARGIN_CM))
//        {
//            Motor_Left(AUTO_SPEED_TURN);
//            g_act = ACT_LEFT;
//            g_turnHoldUntilMs = nowMs + AUTO_TURN_HOLD_MS; // 큰 각도 회전
//        }
//        else
//        {
//            Motor_Stop();
//            g_act = ACT_STOP;
//        }
//        goto PRINT;
//    }
//
//    /* 5) 그 외(25~29cm): 완충 구간 → 저속 직진 */
//    Motor_Forward((uint16_t)(AUTO_SPEED_FWD > 650U ? 650U : AUTO_SPEED_FWD));
//    g_act = ACT_FWD;
//
//PRINT:
//    if ((uint32_t)(nowMs - g_lastPrintMs) >= AUTO_PRINT_PERIOD_MS)
//    {
//        printf("front:%u cm | left:%u cm | right:%u cm | %s\r\n",
//               (unsigned)front, (unsigned)left, (unsigned)right, act_str(g_act));
//        g_lastPrintMs = nowMs;
//    }
//}



#include "auto.h"
#include "motor.h"
#include "ultrasonic.h"
#include <stdio.h>

/* ================== 튜닝 파라미터 ================== */
//#define AUTO_SAFE_CM              25U
#define AUTO_SPEED_FWD            700U
#define AUTO_SPEED_TURN           800U
#define AUTO_DECIDE_PERIOD_MS     30U

#define AUTO_FWD_CM      30U   // 이 이상이면 직진
#define AUTO_TURN_CM     25U   // 이 미만이면 회전(=회피 시작)
#define AUTO_STOP_CM     15U   // 이 미만으로 3방향 다 막히면 정지

#define AUTO_PRINT_PERIOD_MS   200U   // 방향/거리 출력 주기 (너무 짧으면 UART 버벅임)
#define AUTO_TURN_HOLD_MS      220U  /* 회전 최소 유지 시간(회전이 약하면 250~350으로) */
#define AUTO_SIDE_MARGIN_CM    4U    /* 좌/우 차이 마진(노이즈 억제) */


/* --- [추가 파라미터] --- */
#define AUTO_SIDE_DANGER_CM       10U   // (문제2 해결) 측면 벽과 너무 가까울 때의 기준 거리
#define AUTO_SIDE_ADJUST_MS       120U  // (문제2 해결) 벽을 긁기 전 살짝 틀어주는(보정)


/* =================================================== */

typedef enum
{
    ACT_IDLE = 0,
    ACT_FWD,
    ACT_LEFT,
    ACT_RIGHT,
    ACT_STOP
} AutoAct_t;

static uint8_t   g_autoEnabled     = 0U;
static uint32_t  g_lastDecideMs    = 0U;

static uint32_t  g_lastPrintMs     = 0U;
static uint32_t  g_turnHoldUntilMs = 0U;
static AutoAct_t g_act            = ACT_IDLE;

static const char* act_str(AutoAct_t a)
{
    switch (a)
    {
        case ACT_FWD:   return "FWD";
        case ACT_LEFT:  return "LEFT_TURN";
        case ACT_RIGHT: return "RIGHT_TURN";
        case ACT_STOP:  return "STOP";
        default:        return "IDLE";
    }
}

void Auto_Init(void)
{
    g_autoEnabled      = 0U;
    g_lastDecideMs     = HAL_GetTick();

    g_lastPrintMs      = HAL_GetTick();
    g_turnHoldUntilMs  = 0U;
    g_act              = ACT_IDLE;
}

void Auto_On(void)
{
    g_autoEnabled      = 1U;
    g_lastDecideMs     = 0U;   /* 켜자마자 판단 */
    g_lastPrintMs      = 0U;   /* 켜자마자 출력 1회 */
    g_turnHoldUntilMs  = 0U;
    g_act              = ACT_IDLE;
}

void Auto_Off(void)
{
    g_autoEnabled      = 0U;
    g_turnHoldUntilMs  = 0U;
    g_act              = ACT_STOP;
    Motor_Stop();
}

//void Auto_Task(uint32_t nowMs)
//{
//    if (g_autoEnabled == 0U) return;
//
//    if ((uint32_t)(nowMs - g_lastDecideMs) < AUTO_DECIDE_PERIOD_MS) return;
//    g_lastDecideMs = nowMs;
//
//    uint16_t front = Ultrasonic_GetDistance(US_FRONT);
//    uint16_t left  = Ultrasonic_GetDistance(US_LEFT);
//    uint16_t right = Ultrasonic_GetDistance(US_RIGHT);
//
//    /* 1) 회전 유지시간이 남아있으면 이전 회전 방향 유지 */
//    if (nowMs < g_turnHoldUntilMs)
//    {
//        if (g_act == ACT_LEFT)       Motor_Left(AUTO_SPEED_TURN);
//        else if (g_act == ACT_RIGHT) Motor_Right(AUTO_SPEED_TURN);
//        /* 출력은 아래 공통 블록에서 */
//        goto PRINT;
//    }
//
//    /* 2) 막힘 판단(3방향 모두 위험) */
//    if ((front < AUTO_STOP_CM) && (left < AUTO_STOP_CM) && (right < AUTO_STOP_CM))
//    {
//        Motor_Stop();
//        g_act = ACT_STOP;
//        goto PRINT;
//    }
//
//    /* 3) 전방 충분 → 직진(복귀) */
//    if (front >= AUTO_FWD_CM)
//    {
//        Motor_Forward(AUTO_SPEED_FWD);
//        g_act = ACT_FWD;
//        goto PRINT;
//    }
//
//    /* 4) 전방이 회피 구간이면(프론트 < AUTO_TURN_CM) 회전 */
//    if (front < AUTO_TURN_CM)
//    {
//        /* 좌/우 차이가 충분히 날 때만 한 방향으로 확실히 회전 */
//        if (right > (uint16_t)(left + AUTO_SIDE_MARGIN_CM))
//        {
//            Motor_Right(AUTO_SPEED_TURN);
//            g_act = ACT_RIGHT;
//            g_turnHoldUntilMs = nowMs + AUTO_TURN_HOLD_MS;
//        }
//        else if (left > (uint16_t)(right + AUTO_SIDE_MARGIN_CM))
//        {
//            Motor_Left(AUTO_SPEED_TURN);
//            g_act = ACT_LEFT;
//            g_turnHoldUntilMs = nowMs + AUTO_TURN_HOLD_MS;
//        }
//        else
//        {
//            /* 애매하면 잠깐 정지(또는 저속 전진으로 바꿔도 됨) */
//            Motor_Stop();
//            g_act = ACT_STOP;
//        }
//
//        goto PRINT;
//    }
//
//    /* 5) 그 외(25~29cm): 회피 구간 진입 전 완충 → 저속 직진(또는 정지)
//       - 속도가 너무 빠르면 여기서 긁힐 수 있으니, 필요하면 500~650으로 낮추세요. */
//    Motor_Forward((uint16_t)(AUTO_SPEED_FWD > 650U ? 650U : AUTO_SPEED_FWD));
//    g_act = ACT_FWD;
//
//PRINT:
//    if ((uint32_t)(nowMs - g_lastPrintMs) >= AUTO_PRINT_PERIOD_MS)
//    {
//        printf("front:%u cm | left:%u cm | right:%u cm | %s\r\n",
//               (unsigned)front, (unsigned)left, (unsigned)right, act_str(g_act));
//        g_lastPrintMs = nowMs;
//    }
//}


void Auto_Task(uint32_t nowMs)
{
    if (g_autoEnabled == 0U) return;

    if ((uint32_t)(nowMs - g_lastDecideMs) < AUTO_DECIDE_PERIOD_MS) return;
    g_lastDecideMs = nowMs;

    uint16_t front = Ultrasonic_GetDistance(US_FRONT);
    uint16_t left  = Ultrasonic_GetDistance(US_LEFT);
    uint16_t right = Ultrasonic_GetDistance(US_RIGHT);

    /* --- [문제 1 해결] 넓은 공간 0cm 치환 로직 --- */
    /* 거리가 너무 멀어 측정이 안 된 0cm를 매우 먼 거리(999cm)로 치환합니다. */
    if (front == 0) front = 999;
    if (left  == 0) left  = 999;
    if (right == 0) right = 999;
    /* --------------------------------------------- */

    /* 1) 회전 유지시간이 남아있으면 이전 회전 방향 유지 */
    if (nowMs < g_turnHoldUntilMs)
    {
        if (g_act == ACT_LEFT)       Motor_Left(AUTO_SPEED_TURN);
        else if (g_act == ACT_RIGHT) Motor_Right(AUTO_SPEED_TURN);
        goto PRINT;
    }

    /* 2) 막힘 판단(3방향 모두 위험) */
    if ((front < AUTO_STOP_CM) && (left < AUTO_STOP_CM) && (right < AUTO_STOP_CM))
    {
        Motor_Stop();
        g_act = ACT_STOP;
        goto PRINT;
    }

    /* --- [문제 2 해결] 벽면 긁힘 방지 (측면 보정) --- */
    /* 전방에 당장 충돌할 장애물이 없을 때만(front >= AUTO_TURN_CM) 측면을 확인합니다. */
    if (front >= AUTO_TURN_CM)
    {
        /* 왼쪽 벽에 너무 가깝다면 우회전으로 살짝 튕겨냅니다 */
        if (left < AUTO_SIDE_DANGER_CM && left < right)
        {
            Motor_Right(AUTO_SPEED_TURN);
            g_act = ACT_RIGHT;
            g_turnHoldUntilMs = nowMs + AUTO_SIDE_ADJUST_MS; // 아주 짧게만 회전(자세 제어)
            goto PRINT;
        }
        /* 오른쪽 벽에 너무 가깝다면 좌회전으로 살짝 튕겨냅니다 */
        else if (right < AUTO_SIDE_DANGER_CM && right < left)
        {
            Motor_Left(AUTO_SPEED_TURN);
            g_act = ACT_LEFT;
            g_turnHoldUntilMs = nowMs + AUTO_SIDE_ADJUST_MS; // 아주 짧게만 회전(자세 제어)
            goto PRINT;
        }
    }
    /* --------------------------------------------- */

    /* 3) 전방 충분 → 직진(복귀) */
    if (front >= AUTO_FWD_CM)
    {
        Motor_Forward(AUTO_SPEED_FWD);
        g_act = ACT_FWD;
        goto PRINT;
    }

    /* 4) 전방이 회피 구간이면(프론트 < AUTO_TURN_CM) 장애물 회피 강한 회전 */
    if (front < AUTO_TURN_CM)
    {
        if (right > (uint16_t)(left + AUTO_SIDE_MARGIN_CM))
        {
            Motor_Right(AUTO_SPEED_TURN);
            g_act = ACT_RIGHT;
            g_turnHoldUntilMs = nowMs + AUTO_TURN_HOLD_MS; // 큰 각도 회전
        }
        else if (left > (uint16_t)(right + AUTO_SIDE_MARGIN_CM))
        {
            Motor_Left(AUTO_SPEED_TURN);
            g_act = ACT_LEFT;
            g_turnHoldUntilMs = nowMs + AUTO_TURN_HOLD_MS; // 큰 각도 회전
        }
        else
        {
            Motor_Stop();
            g_act = ACT_STOP;
        }
        goto PRINT;
    }

    /* 5) 그 외(25~29cm): 완충 구간 → 저속 직진 */
    Motor_Forward((uint16_t)(AUTO_SPEED_FWD > 650U ? 650U : AUTO_SPEED_FWD));
    g_act = ACT_FWD;

PRINT:
    if ((uint32_t)(nowMs - g_lastPrintMs) >= AUTO_PRINT_PERIOD_MS)
    {
        printf("front:%u cm | left:%u cm | right:%u cm | %s\r\n",
               (unsigned)front, (unsigned)left, (unsigned)right, act_str(g_act));
        g_lastPrintMs = nowMs;
    }
}

