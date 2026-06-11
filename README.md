# 🚗 RC Car Control
 
> STM32F411RETx 기반 블루투스 수동 조종 + 초음파 3방향 자율 장애물 회피 RC카
 
[![Notion](https://img.shields.io/badge/프로젝트%20상세%20보기-Notion-000000?style=for-the-badge&logo=notion&logoColor=white)](https://app.notion.com/p/minseokim-profile/RC-Car-317b5d65c68c80e0a92dd644f85a637f?source=copy_link)
 
---
 
## 개요
 
| 항목 | 내용 |
|------|------|
| MCU | STM32F411RETx |
| 언어 | C |
| 도구 | STM32CubeIDE |
| 통신 | USART1 (HC-06 Bluetooth, 9600 bps) |
| 모터 제어 | L298N + TIM3 PWM |
| 거리 측정 | HC-SR04 × 3 (TIM2 Input Capture) |
| 개발 기간 | 2026.02.24 ~ 03.01 |
 
---
 
## 주요 기능
 
- **Manual Mode** — 블루투스 앱에서 W·A·S·D·C 명령으로 전후좌우 수동 조종
- **Ultrasonic 거리 측정** — TIM2 Input Capture(1MHz)로 Echo 펄스 폭 측정 → `distanceCm = echoUs / 58`. 3채널 라운드로빈 트리거로 간섭 방지
- **Auto Mode** — 30ms 주기로 전·좌·우 거리 분석 후 직진·회전·정지 자동 결정
- **Side Adjust** — 측면 10cm 미만 감지 시 120ms 보정 회전으로 벽 긁힘 방지
- **Turn Hold** — 회전 명령 후 220ms 동안 방향 유지로 충분한 회전 각도 확보
---
 
## 파일 구성
 
| 파일 | 역할 |
|------|------|
| `main.c` | 초기화, UART 콜백, 메인 루프 |
| `motor.c` | 모터 방향(GPIO) + 속도(PWM) 제어 |
| `ultrasonic.c` | HC-SR04 거리 측정 (Input Capture) |
| `auto.c` | 자율주행 상태머신 |
| `delay.c` | TIM11 기반 µs 딜레이 |
 
---
 
## 주요 구현
 
- **비동기 라운드로빈 측정** — 3개 센서를 50ms 간격으로 순차 트리거, 간섭 없이 실시간 거리 산출
- **인터럽트 기반 UART** — `HAL_UART_Receive_IT()`로 1바이트씩 수신, 콜백에서 즉시 재등록
- **0cm 예외 처리** — 측정 범위 초과 시 반환되는 0cm를 999cm로 치환해 오인식 방지
- **우선순위 상태머신** — 예외처리 → Hold → 직진 → 측면보정 → 회피 → 정지 순으로 주행 상태 결정
---
 
## 트러블슈팅
 
| 문제 | 원인 | 해결 |
|------|------|------|
| 넓은 공간에서 무한 정지 | 측정 범위 초과 → 0cm 반환 → 막힘 오인식 | 0cm → 999cm 치환 예외 처리 |
| 측면 벽면 긁힘 | 전방만 확인 후 무조건 직진 | Side Adjust 로직 추가 |
| 회전 각도 부족 | 30ms마다 재판단으로 회전 중단 | Turn Hold (220ms) 도입 |
| 블루투스 명령 미수신 | USART2 개발 후 USART1 전환 누락 | 콜백 및 수신 함수 USART1으로 통일 |
