#define LED_LCD_PIN 27
#define POWER_EN_PIN 16
#define BUZZER_PIN 17
#define VOLT_EN 15
#define VOLT_PIN 14
#define CE_PIN 12
#define CSN_PIN 13
#define IRQ_PIN 35
#define NONE_VALUE 2147483648
bool is_SD_ok;
bool is_RTC_ok;

typedef enum ePage{
  emSetupTime = 0,
  emDrawing
};

struct sTime{
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
};
sTime timeRTC;
struct sMemory{
  uint16_t m_x[6];
  uint16_t m_y[6];
  int16_t d_x[6];
  int16_t d_y[6];
  bool fresh[6];
  uint8_t number[6];
  uint16_t w;
  uint16_t h;
  uint16_t line_color[6];
  uint16_t bg_color[6];

  // variable for draw in loop function
  uint32_t delay;
  uint32_t time_wait;
  bool running;
  uint32_t time_start;
  uint32_t i;
};
sMemory memory;

typedef enum eDrawState{
  emIdle,
  emInitilize,
  emReadGcode,
  emRunning,
  emFinish,
  emError
};
struct sDraw{
  char path[50];
  int16_t base_x;
  int16_t base_y;
  uint32_t delay;
  uint32_t time_wait;
  uint16_t line_color;
  bool draw_en;
  eDrawState state;
  int32_t max_x = 0, max_y = 0, min_x = 0, min_y = 0;
  float old_X = NONE_VALUE, old_Y = NONE_VALUE, X, Y;
  float old_x = -1, old_y = -1;
  int16_t x1, y1, x2, y2;
  float i, d;
  float scale;
  float offset_x;
  float offset_y;
  bool running;
  File file;
  uint8_t Gcode_drawed;
};

sDraw drawing;
#define WIDTH_NUMBER 60
#define HEIGHT_NUMBER 100


typedef enum eDes{
  emMemory,
  emScreen,
  emBoth
};
ePage page = emDrawing;

bool *displaied_1;
bool *displaied_2;
bool *mNumber[10];
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
#define TFT_GREY 0x7BEF

String get_name(const char * path){
  String str = String(path);
  String file_name = "";
  int16_t index = str.length();
  while(index >= 0){
    if(str.charAt(index-1) == '/'){
      break;
    }
    index--;
  }
  while(index < str.length()){
    file_name += String(str.charAt(index));
    index++;
  }
  return file_name;
}
void add_memory(uint16_t m_x, uint16_t m_y, int16_t d_x, int16_t d_y,
uint16_t w, uint16_t h, uint16_t line_color, uint16_t bg_color, uint8_t index){
  memory.m_x[index] = m_x;
  memory.m_y[index] = m_y;
  memory.d_x[index] = d_x;
  memory.d_y[index] = d_y;
  memory.w = w;
  memory.h = h;
  memory.line_color[index] = line_color;
  memory.bg_color[index] = bg_color;
}
uint16_t width_x;
uint16_t height_y;
uint32_t time_readRTC = 0;
