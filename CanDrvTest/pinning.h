// DISPLAY
/*
 * Pinning DISPLAY
 * [1] RST     15
 *  2  CS      24
 *  3  D/C     11
 *  4  MOSI    9
 *  5  CLK     28
 *  6  VCC     5V
 *  7  BL      R -> 8 //Pin 8(D5) for the board. Pin 10(D7) for Breadboard
 *  8  GND     GND
 */
#define DISPLAY_CS   18
#define DISPLAY_DC   8
#define DISPLAY_RST  12
#define DISPLAY_BL 5
#define DISPLAY_POWER_LS 17 //Pin 23(D17) for Board. Pin 18(D15) for Bradboard
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
 #define CAN_STB 26 //on the Board pin 32(D26). On the Breadboard Pin 30(D24)
