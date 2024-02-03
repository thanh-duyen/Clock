#include <map>
#include "esp_sleep.h"
#include <TFT_eSPI.h> // Hardware-specific library
#include <SPI.h>
#include "RTClib.h"
#include "FS.h"
#include "SD.h"
#include <vector>
#include <EEPROM.h>
#include <JPEGDecoder.h>

#define LED_LCD_PIN 27
#define POWER_EN_PIN 16
#define BUZZER_PIN 17
#define VOLT_EN_PIN 15
#define VOLT_PIN 14
#define CE_PIN 12
#define CSN_PIN 13
#define IRQ_PIN 35
#define BUTTON_PIN 0
#define NONE_VALUE 2147483648

bool is_SD_ok;
bool is_RTC_ok;

const uint8_t FONT_COLOR_R[7] = {156,0,252,0,106,232,253};
const uint8_t FONT_COLOR_G[7] = {109,0,212,0,178,79,245};
const uint8_t FONT_COLOR_B[7] = {47,0,64,0,44,61,246};

#define CALIBRATION_FILE "/calibrationData"
#define NUM_KEYS 20
TFT_eSPI_Button key[NUM_KEYS];
uint16_t touch_x, touch_y;
bool is_touching = true;
bool is_touchHandled = false;

typedef enum ePage{
  emHome = 0,
  emSetting = 1,
  emPage = 2
};
ePage page = emHome;
const uint8_t BUTTON_ATPAGE[emPage] = {1,6};
struct sTime{
  int8_t hour;
  int8_t minute;
  int8_t second;
  uint8_t date;
  uint8_t day;
  uint8_t month;
  uint16_t year;
  bool printed_time;
  bool printed_date;
  uint32_t time_read = 0;
};
sTime timeRTC;

#define WIDTH_NUMBER 50
#define HEIGHT_NUMBER 100
#define NUMBER_FONT_SIZE WIDTH_NUMBER*HEIGHT_NUMBER
uint16_t *memory[10];
static uint16_t memory_buffer[50005];
#define MAX_MEMORY_DRAWING 6

struct sDrawMemory{
  uint16_t x,y;
  uint32_t start_memory;
  uint16_t width;
  uint16_t height;
  uint32_t delay;
  uint32_t time_wait;
  bool running;
  uint16_t current_y;
  uint32_t start_time;
};
sDrawMemory drawingMemory[MAX_MEMORY_DRAWING];

typedef enum eDrawState{
  emIdle,
  emInitilize,
  emReadGcode,
  emRunning,
  emFinish,
  emError
};
typedef enum eClockType{
  emAnalog = 0,
  emDigital
};
typedef enum eDes{
  emMemory,
  emImageToMemory,
  emScreen,
  emGetFont,
  emText,
  emImage,
  emClockFace,
  emBoth
};

struct sTextDraw{
  String text;
  uint16_t index_text;
  int32_t next_base_x;
};

struct sMemoryGet{
  uint32_t start_memory;
  uint8_t width_memory;
};

#define FONT_ABC_LEN 85
#define FONT_HOUR_LEN 12
#define FONT_MIN_LEN 60
#define FONT_SEC_LEN 60
struct sFont{
  sFont(uint8_t length){
    length_font = length;
    character = new char[length];
    postion = new uint32_t[length];
    width_char = new uint8_t[length];
    start_x = new uint16_t[length];
  }
  ~sFont(){
    delete [] character;
    delete [] postion;
    delete [] width_char;
    delete [] start_x;
  }
  char *character;
  uint32_t *postion;
  uint8_t *width_char;
  uint16_t *start_x;
  std::map<char,uint8_t> char_map;
  bool original;
  uint16_t height;
  uint32_t index_seek;
  int16_t index;
  uint8_t length_font;
};

sFont fontGcode(FONT_ABC_LEN);
sFont clockHourGcode(FONT_HOUR_LEN);
sFont clockMinGcode(FONT_MIN_LEN);
sFont clockSecGcode(FONT_SEC_LEN);

typedef enum eDrawClockState{
  emClearing,
  emDrawing,
  emDone
};
eDrawClockState clock_state[3] = {emDone,emDone,emDone};
struct sDraw{
  char path[50];
  int16_t base_x;
  int16_t base_y;
  uint32_t delay;
  uint32_t time_wait;
  uint16_t line_color;
  bool draw_en;
  int32_t max_x = 0, max_y = 0, min_x = 0, min_y = 0;
  float old_X = NONE_VALUE, old_Y = NONE_VALUE, X, Y;
  float old_x = -1, old_y = -1;
  int16_t x1, y1, x2, y2;
  float i, d, w;
  float scale;
  float offset_x;
  float offset_y;
  bool running;
  bool restoreImage;
  sFont *fontRun;
  bool close_file;
  bool open_file;
  eDrawState state;
  eDes destination;
  sTextDraw text;
  sMemoryGet memory;
  File file;
};
#define MAX_RUNGCODE 5
struct sDraw core[MAX_RUNGCODE];
#define CLOCKFACE_INDEX 4
#define READFONT_INDEX 1

char monsOfTheYear[12][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
char daysOfTheWeek[7][10] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

String get_name(String path){
  String file_name = "";
  int16_t index = path.length();
  while(index >= 0){
    if(path.charAt(index-1) == '/'){
      break;
    }
    index--;
  }
  while(index < path.length()){
    file_name += String(path.charAt(index));
    index++;
  }
  return file_name;
}
String add_zero(uint16_t num){
  if(num < 10)
    return "0" + String(num);
  return String(num);
}
typedef enum eNewFontState{
  emReading,
  emReadComplete,
  emChangeFontRequest,
  emComplete
};
eNewFontState read_newFontState;
bool is_readNumberFont;
uint8_t numberFontImage;
bool first_readFont;
bool drawClockFace;
eClockType clock_type;
bool data_clockReady;

uint8_t readed_fontNumberStep;
bool readed_font;
int16_t draw_step;
uint32_t time_wait;

uint16_t width_x;
uint16_t height_y;

const char KEY_BOARD[3][40] = {"1234567890qwertyuiopasdfghjklzxcvbnm",
                               "1234567890QWERTYUIOPASDFGHJKLZXCVBNM",
                               "!@#$%^&*()`~+=[]{}:;'|,.<>/?,\"     "};
const char FUNCTION_NAME[5][7]={"ESC","!@","Ab","Fn","Enter"};

bool screen_data[24][24];
struct sMove{
  uint16_t x1, y1, x2, y2;
  uint16_t old_x, old_y;
  int16_t new_x, new_y;
  uint16_t start_x;
  float i;
  bool running;
  uint16_t angle;
  uint16_t color;
  uint32_t time_run;
  uint32_t delay;
  uint16_t step;
};
sMove move[2];
#define RADIUS_CIRCLE 5
#define BLOCK_SIZE 15
