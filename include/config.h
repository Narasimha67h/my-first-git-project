#ifndef CONFIG_H
#define CONFIG_H

// Wi-Fi credentials
#define WIFI_SSID       "Lakshiv"
#define WIFI_PASSWORD   "kanGan#072010"

// Telegram Bot credentials
// Create a bot with @BotFather and obtain your chat ID.
#define TELEGRAM_BOT_TOKEN "9255257096:AAG5bfTA--LwjuB_WVTkXdx9vbsgjCsxavI"
#define TELEGRAM_CHAT_ID1   "6643367659"
#define TELEGRAM_CHAT_ID2   "1385587539"
//#define NTFY_TOPIC "esp8266ultra"
//#define NTFY_URL "ntfy.sh/espsmartwater4637"
// ESP8266 OTA settings
#define OTA_HOSTNAME "esp8266ultra"
#define OTA_PASSWORD "kanGan#072010"

// NodeMCU pin mapping
// Sensor TX -> D5 / GPIO14 (ESP8266 software UART RX)
// Sensor RX -> D6 / GPIO12 (measurement trigger)
#define SENSOR_UART_RX_PIN D5
#define SENSOR_TRIGGER_PIN D6

#define SENSOR_BAUD_RATE 9600
#define SENSOR_TRIGGER_INTERVAL_MS 1000UL

// Calibration assumptions:
// EMPTY_DISTANCE_MM = sensor face to bucket bottom with bucket EMPTY.
// Bucket internal usable height = 300 mm.
//#define EMPTY_DISTANCE_MM 900
#define BUCKET_HEIGHT_MM       200
#define FULL_DISTANCE_MM       80
#define HALF_DISTANCE_MM      150
#define QUARTER_DISTANCE_MM   210
#define EMPTY_DISTANCE_MM     280
// Three notification thresholds
#define LEVEL_25_PERCENT 25
#define LEVEL_50_PERCENT 50
#define LEVEL_95_PERCENT 95

// Hysteresis: re-arm an alert after level falls this many percentage points
#define LEVEL_RESET_HYSTERESIS_PERCENT 5

// Ignore impossible sensor readings
#define SENSOR_MIN_DISTANCE_MM 30
#define SENSOR_MAX_DISTANCE_MM 4500

#endif
