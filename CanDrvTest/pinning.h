// DISPLAY
/*
 * Pinning DISPLAY
 * [1] RST     15
 *  2  CS      24
 *  3  D/C     11
 *  4  MOSI    9
 *  5  CLK     28
 *  6  VCC     5V
 *  7  BL      R -> 10
 *  8  GND     GND
 */
#define DISPLAY_CS   18
#define DISPLAY_DC   8
#define DISPLAY_RST  12
#define DISPLAY_BL 7
#define DISPLAY_POWER_LS 15
#define DISPLAY_MOSI MOSI
#define DISPLAY_SCK SCK

// ShutdownController
/*
 * Pinning Shutdown Controller
 * Control LED     17
 * CAN_STB         23
 * CAN_RX_PIN      30
 */
 #define SHUTDOWN_LED 14
 #define SHUTDOWN_WAKEUP 24
 #define CAN_STB 17 
