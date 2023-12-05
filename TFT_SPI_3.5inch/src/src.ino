#include "esp_sleep.h"
#include <TFT_eSPI.h> // Hardware-specific library
#include <SPI.h>
#include "RTClib.h"
#include "FS.h"
#include "SD.h"
#include "data.h"

TFT_eSPI tft = TFT_eSPI();
RTC_DS3231 rtc;

void deleteFile(fs::FS &fs, const char * path){
    Serial.printf("Deleting file: %s\n", path);
    if(fs.remove(path)){
        Serial.println("File deleted");
    } else {
        Serial.println("Delete failed");
    }
}
void clearMemory(uint32_t x, uint32_t y, uint16_t w, uint16_t h){
  for(uint16_t i = 0; i < h; i++){
    for(uint16_t j = 0; j < w; j++){
      if(x+(i+y)*width_x+j < 76800){
        displaied_1[x+(i+y)*width_x+j] = 0;
      }
      else{
        displaied_2[x+(i+y)*width_x+j-76800] = 0;
      }
    }
  }
}
void copyBigMem2Mem(uint32_t x, uint32_t y, uint8_t index){
  for(uint32_t i = 0; i < HEIGHT_NUMBER; i++){
    for(uint8_t j = 0; j < WIDTH_NUMBER; j++){
      if(x+(i+y)*width_x+j < 76800){
        mNumber[index][i*WIDTH_NUMBER+j] = displaied_1[x+(i+y)*width_x+j];
      }
      else{
        mNumber[index][i*WIDTH_NUMBER+j] = displaied_1[x+(i+y)*width_x+j-76800];
      }
    }
  }
}
void copyMem2BigMem(uint32_t x, uint32_t y, uint8_t index){
  for(uint32_t i = 0; i < HEIGHT_NUMBER; i++){
    for(uint8_t j = 0; j < WIDTH_NUMBER; j++){
      if(x+(i+y)*width_x+j < 76800){
        displaied_1[x+(i+y)*width_x+j] = mNumber[index][i*WIDTH_NUMBER+j];
      }
      else{
        displaied_1[x+(i+y)*width_x+j-76800] = mNumber[index][i*WIDTH_NUMBER+j];
      }
    }
  }
}
void drawMemory(uint32_t delay_time, bool setup_run){
  if(setup_run == true){
    memory.delay = delay_time;
    memory.time_wait = micros();
    memory.running = true;
    memory.i = 0;
    memory.time_start = micros();
    return;
  }
  for(uint16_t i = 0; i < memory.h; i++){
    for(uint8_t k = 0; k < 6; k++){
      if(memory.fresh[k] == true){
        for(uint16_t j = 0; j < memory.w; j++){
          if(memory.m_x[k]+i*width_x+j < 76800){
            if(displaied_1[memory.m_x[k]+i*width_x+j] == 0){
              tft.drawPixel(j+memory.d_x[k],i,memory.bg_color[k]);
            }
            else{
              tft.drawPixel(j+memory.d_x[k],i,memory.line_color[k]);
            }
          }
          else{
            if(displaied_2[memory.m_x[k]+i*width_x+j-76800] == 0){
              tft.drawPixel(j+memory.d_x[k],i,memory.bg_color[k]);
            }
            else{
              tft.drawPixel(j+memory.d_x[k],i,memory.line_color[k]);
            }
          }
        }
      }
    }
    delayMicroseconds(delay_time);
  }
  for(uint8_t k = 0; k < 6; k++){
    memory.fresh[k] = false;
  }
}
void drawMemory(){
  if(memory.running == false){
    return;
  }
  if(micros() >= memory.time_wait){
    for(uint8_t k = 0; k < 6; k++){
      if(memory.fresh[k] == true){
        for(uint16_t j = 0; j < memory.w; j++){
          if(memory.m_x[k]+memory.i*width_x+j < 76800){
            if(displaied_1[memory.m_x[k]+memory.i*width_x+j] == 0){
              tft.drawPixel(j+memory.d_x[k],memory.i,memory.bg_color[k]);
            }
            else{
              tft.drawPixel(j+memory.d_x[k],memory.i,memory.line_color[k]);
            }
          }
          else{
            if(displaied_2[memory.m_x[k]+memory.i*width_x+j-76800] == 0){
              tft.drawPixel(j+memory.d_x[k],memory.i,memory.bg_color[k]);
            }
            else{
              tft.drawPixel(j+memory.d_x[k],memory.i,memory.line_color[k]);
            }
          }
        }
      }
    }
    memory.time_wait = micros()+memory.delay;
    if(++memory.i >= memory.h){
      for(uint8_t k = 0; k < 6; k++){
        memory.fresh[k] = false;
      }
//      Serial.println(micros()-memory.time_start);
      memory.running = false;
    }
  }
}
bool generate_infoFile(fs::FS &fs, const char * path){
  String buff = "";
  float scale, offset_x, offset_y;
  int32_t max_x = 0, max_y = 0, min_x = 0, min_y = 0;
  bool is_info = true;
  bool return_value;
  String str = String(path);
  String file_name = get_name(path);
  char info_filepath[50];
  str.replace(file_name,"."+file_name);
  str.toCharArray(info_filepath,str.length()+1);
  File file = fs.open(info_filepath);
  if(!file){
    Serial.println("Start generate info file");
    is_info = false;
  }
  else{
    return true;
  }
  
  file = fs.open(path);
  if(!file){
    return false;
  }
  while(file.available()){
    //read a line in file
    str = "";
    char c = file.read();
    uint8_t index = 0;
    int8_t pos_x = -1, pos_y = -1, pos_z = -1;
    while(c != '\n' && file.available()){
      str += c;
      if(c == 'X' || c == 'x')
        pos_x = index + 1;
      else if(c == 'Y' || c == 'y')
        pos_y = index + 1;
      else if(c == 'Z' || c == 'z')
        pos_z = index + 1;
      index++;
      c = file.read();
    }
    str += c;      
    //end
    if(pos_x >= 0){
      buff = "";
      uint8_t value = int(str.charAt(pos_x)) - 48;
      while(((value >= 0 && value <= 9) || str.charAt(pos_x) == '-') && pos_x < str.length()){
        buff += str.charAt(pos_x);
        pos_x++;
        value = int(str.charAt(pos_x)) - 48;
      }
      if(buff.toFloat() > max_x){
        max_x = buff.toFloat();
      }
      else if(buff.toFloat() < min_x){
        min_x = buff.toFloat();
      }
    }
    if(pos_y >= 0){
      buff = "";
      uint8_t value = int(str.charAt(pos_y)) - 48;
      while(((value >= 0 && value <= 9) || str.charAt(pos_y) == '-') && pos_y < str.length()){
        buff += str.charAt(pos_y);
        pos_y++;
        value = int(str.charAt(pos_y)) - 48;
      }
      if(buff.toFloat() > max_y){
        max_y = buff.toFloat();
      }
      else if(buff.toFloat() < min_y){
        min_y = buff.toFloat();
      }
    }
  }
  file.close();

  // save max_x,min_x,max_y,min_y
  char file_content[100];
  str = String(max_x) + "\n";
  str += String(min_x) + "\n";
  str += String(max_y) + "\n";
  str += String(min_y) + "\n\n";
  str.toCharArray(file_content,str.length()+1);
  file = fs.open(info_filepath, FILE_WRITE);
  if(!file){
    return false;
  }
  if(!file.print(file_content)){
    file.close();
    return false;
  }
  file.close();

  // Compact command
  #define NONE_VALUE 2147483648
  float old_X = NONE_VALUE, old_Y = NONE_VALUE, X, Y;
  bool draw_en = true;
  file = fs.open(path);
  if(!file){
    return false;
  }
  String available_cmds = "";
  while(file.available() > 0){
    str = "";
    char c = file.read();
    uint8_t index = 0;
    int8_t pos_x = -1, pos_y = -1, pos_z = -1;
    String available_cmd = "";
    while(c != '\n' && file.available() > 0){
      str += c;
      if(c == 'X' || c == 'x')
        pos_x = index + 1;
      else if(c == 'Y' || c == 'y')
        pos_y = index + 1;
      else if(c == 'Z' || c == 'z')
        pos_z = index + 1;
      index++;
      c = file.read();
    }
    str += c;
    X = NONE_VALUE;
    Y = NONE_VALUE;
    if(pos_z >= 0){
      buff = "";
      uint8_t value = int(str.charAt(pos_z)) - 48;
      while(((value >= 0 && value <= 9) || str.charAt(pos_z) == '-') && pos_z < str.length()){
        buff += str.charAt(pos_z);
        pos_z++;
        value = int(str.charAt(pos_z)) - 48;
      }
      available_cmd += "Z" + buff;
      if(buff.toInt() > 0){
        draw_en = false;
        old_X = old_Y = NONE_VALUE;
        X = Y = NONE_VALUE;
      }
      else{
        draw_en = true;
      }
    }
    if(pos_x >= 0){
      buff = "";
      uint8_t value = int(str.charAt(pos_x)) - 48;
      while(((value >= 0 && value <= 9) || str.charAt(pos_x) == '-') && pos_x < str.length()){
        buff += str.charAt(pos_x);
        pos_x++;
        value = int(str.charAt(pos_x)) - 48;
      }
      available_cmd += "X" + buff;
      if(old_X == NONE_VALUE || draw_en == false)
        old_X = buff.toFloat();
      else
        X = buff.toFloat();
    }
    if(pos_y >= 0){
      buff = "";
      uint8_t value = int(str.charAt(pos_y)) - 48;
      while(((value >= 0 && value <= 9) || str.charAt(pos_y) == '-') && pos_y < str.length()){
        buff += str.charAt(pos_y);
        pos_y++;
        value = int(str.charAt(pos_y)) - 48;
      }
      available_cmd += "Y" + buff;
      if(old_Y == NONE_VALUE || draw_en == false)
        old_Y = buff.toFloat();
      else
        Y = buff.toFloat();
    }
    if(old_X != NONE_VALUE && old_Y != NONE_VALUE){
      if(X == NONE_VALUE && Y != NONE_VALUE){
        X = old_X;
      }
      else if(X != NONE_VALUE && Y == NONE_VALUE){
        Y = old_Y;
      }
    }

    if((available_cmd != "" && (old_X != X || old_Y != Y || pos_z >= 0)) || file.available() <= 1){
      available_cmds += available_cmd + "\n";
      if(available_cmds.length() > 80 || file.available() <= 1){
        available_cmds.toCharArray(file_content,available_cmds.length()+1);
        File file_2 = fs.open(info_filepath, FILE_APPEND);
        if(!file_2){
          file.close();
          return false;
        }
        else{
          if(!file_2.print(file_content)){
            file.close();
            file_2.close();
            return false;
          }
        }
        available_cmds = "";
      }
    }
    if(old_X != NONE_VALUE && old_Y != NONE_VALUE && X != NONE_VALUE && Y != NONE_VALUE){
      old_X = X;
      old_Y = Y;
    }
  }
  file.close();
  Serial.println("Done " + String(path));
  return true;
}
bool draw(fs::FS &fs, String path_string, int16_t base_x, int16_t base_y, uint32_t delay_time, 
eDes destination, uint16_t line_color = 0, bool setup_run = false){
  char path[50];
  path_string.toCharArray(path,path_string.length()+1);
  String buff = "";
  float scale, offset_x, offset_y;
  int32_t max_x = 0, max_y = 0, min_x = 0, min_y = 0;
  bool gen_info = false;
  // Check is there info file
  String str = String(path);
  String file_name = get_name(path);
  char info_filepath[50];
  str.replace(file_name,"."+file_name);
  str.toCharArray(info_filepath,str.length()+1);
  File file = fs.open(info_filepath);
  if(!file){
    if(generate_infoFile(fs,path) == false){
      return false;
    }
    gen_info = true;
  }
  if(gen_info){
    file = fs.open(info_filepath);
    if(!file){
      return false;
    }
  }
  if(setup_run == true){
    str.toCharArray(drawing.path,str.length()+1);
    drawing.delay = delay_time;
    drawing.time_wait = micros();
    drawing.base_x = base_x;
    drawing.base_y = base_y;
    drawing.line_color = line_color;
    drawing.state = emInitilize;
    drawing.running = true;
    return true;
  }
  str = "";
  char c = file.read();
  uint8_t index = 0;
  while(file.available() && index < 4){
    if(c == '\n'){
      if(index == 0){
        max_x = str.toInt();
      }
      else if(index == 1){
        min_x = str.toInt();
      }
      else if(index == 2){
        max_y = str.toInt();
      }
      else if(index == 3){
        min_y = str.toInt();
      }
      index++;
      str = "";
    }
    else{
      str += c;
    }
    c = file.read();
  }
  // end
  
  uint8_t ro = tft.getRotation();
  width_x = 480;
  height_y = 320;
  if(ro == 0 || ro == 2){
    width_x = 320;
    height_y = 480;
  }
  scale = 1;
  if(max_x - min_x > width_x || max_y - min_y > height_y){
    if((max_x - min_x - width_x) > (max_y - min_y - height_y)){
      scale = width_x/(max_x - min_x);
    }
    else{
      scale = height_y/(max_y - min_y);
    }
  }

  offset_x = 0;
  if(min_x < 0){
    offset_x = min_x*-1;
  }
  offset_y = 0;
  if(min_y < 0){
    offset_y = min_y*-1;
  }
  
  float old_X = NONE_VALUE, old_Y = NONE_VALUE, X, Y;
  bool draw_en = true;
  // read continue
  while(file.available()){
    str = "";
    char c = file.read();
    uint8_t index = 0;
    int8_t pos_x = -1, pos_y = -1, pos_z = -1;
    bool cmd_added = false;
    String available_cmd = "";
    while(c != '\n' && file.available()){
      str += c;
      if(c == 'X' || c == 'x')
        pos_x = index + 1;
      else if(c == 'Y' || c == 'y')
        pos_y = index + 1;
      else if(c == 'Z' || c == 'z')
        pos_z = index + 1;
      index++;
      c = file.read();
    }
    str += c;
    X = NONE_VALUE;
    Y = NONE_VALUE;
    if(pos_z >= 0){
      buff = "";
      uint8_t value = int(str.charAt(pos_z)) - 48;
      while(((value >= 0 && value <= 9) || str.charAt(pos_z) == '-') && pos_z < str.length()){
        buff += str.charAt(pos_z);
        pos_z++;
        value = int(str.charAt(pos_z)) - 48;
      }
      if(buff.toInt() > 0){
        draw_en = false;
        old_X = old_Y = NONE_VALUE;
        X = Y = NONE_VALUE;
      }
      else{
        draw_en = true;
      }
    }
    if(pos_x >= 0){
      buff = "";
      uint8_t value = int(str.charAt(pos_x)) - 48;
      while(((value >= 0 && value <= 9) || str.charAt(pos_x) == '-') && pos_x < str.length()){
        buff += str.charAt(pos_x);
        pos_x++;
        value = int(str.charAt(pos_x)) - 48;
      }
      if(old_X == NONE_VALUE || draw_en == false)
        old_X = buff.toFloat();
      else
        X = buff.toFloat();
    }
    if(pos_y >= 0){
      buff = "";
      uint8_t value = int(str.charAt(pos_y)) - 48;
      while(((value >= 0 && value <= 9) || str.charAt(pos_y) == '-') && pos_y < str.length()){
        buff += str.charAt(pos_y);
        pos_y++;
        value = int(str.charAt(pos_y)) - 48;
      }
      if(old_Y == NONE_VALUE || draw_en == false)
        old_Y = buff.toFloat();
      else
        Y = buff.toFloat();
    }
    if(old_X != NONE_VALUE && old_Y != NONE_VALUE){
      if(X == NONE_VALUE && Y != NONE_VALUE){
        X = old_X;
      }
      else if(X != NONE_VALUE && Y == NONE_VALUE){
        Y = old_Y;
      }
    }

    if(old_X == X && old_Y == Y){
      continue;
    }
    if(old_X != NONE_VALUE && old_Y != NONE_VALUE && X != NONE_VALUE && Y != NONE_VALUE){
      int16_t x1, y1, x2, y2;
      x1 = (old_X+offset_x)*scale+base_x;
      y1 = height_y - (height_y - (offset_y+max_y)) - (old_Y+offset_y)*scale + base_y;
      x2 = (X+offset_x)*scale+base_x;
      y2 = height_y - (height_y - (offset_y+max_y)) - (Y+offset_y)*scale + base_y;
      float d = sqrt(pow(x2-x1,2)+pow(y2-y1,2));
      uint16_t old_x = -1, old_y = -1;
      for(float i = 0.001; i < 1; i+= 0.001+3.2/(d*10)){
        uint16_t x = x1+(x2-x1)*i;
        uint16_t y = y1+(y2-y1)*i;
        if(destination == emScreen || destination == emBoth){
          if(old_x != x || old_y != y){
            tft.drawPixel(x,y,line_color);
            old_x = x;
            old_y = y;
            delayMicroseconds(delay_time);
          }
        }
        if(destination == emMemory || destination == emBoth){
          if(y*width_x+x < 76800){
            displaied_1[y*width_x+x] = 1;
          }
          else{
            displaied_2[y*width_x+x-76800] = 1;
          }
        }
      }
      old_X = X;
      old_Y = Y;
    }
  }
  file.close();
  return true;
}
void draw(fs::FS &fs){
  if(drawing.running == false){
    return;
  }
  if(micros() >= drawing.time_wait){
    if(drawing.state == emInitilize){
      String str = "";
      drawing.file = fs.open(drawing.path);
      if(!drawing.file){
        drawing.state = emError;
        Serial.println("Cannot open");
        drawing.running = false;
        return;
      }
      char c = drawing.file.read();
      uint8_t index = 0;
      while(drawing.file.available() && index < 4){
        if(c == '\n'){
          if(index == 0){
            drawing.max_x = str.toInt();
          }
          else if(index == 1){
            drawing.min_x = str.toInt();
          }
          else if(index == 2){
            drawing.max_y = str.toInt();
          }
          else if(index == 3){
            drawing.min_y = str.toInt();
          }
          index++;
          str = "";
        }
        else{
          str += c;
        }
        c = drawing.file.read();
      }
      // end
      
      uint8_t ro = tft.getRotation();
      width_x = 480;
      height_y = 320;
      if(ro == 0 || ro == 2){
        width_x = 320;
        height_y = 480;
      }
      drawing.scale = 1;
      if(drawing.max_x - drawing.min_x > width_x || drawing.max_y - drawing.min_y > height_y){
        if((drawing.max_x - drawing.min_x - width_x) > (drawing.max_y - drawing.min_y - height_y)){
          drawing.scale = width_x/(drawing.max_x - drawing.min_x);
        }
        else{
          drawing.scale = height_y/(drawing.max_y - drawing.min_y);
        }
      }
    
      drawing.offset_x = 0;
      if(drawing.min_x < 0){
        drawing.offset_x = drawing.min_x*-1;
      }
      drawing.offset_y = 0;
      if(drawing.min_y < 0){
        drawing.offset_y = drawing.min_y*-1;
      }

      drawing.draw_en = true;
      drawing.state = emReadGcode;
    }
    else if(drawing.state == emReadGcode){
      String str = "";
      String buff = "";
      // read continue
      while(drawing.file.available()){
        char c = drawing.file.read();
        uint8_t index = 0;
        int8_t pos_x = -1, pos_y = -1, pos_z = -1;
        while(c != '\n' && drawing.file.available()){
          str += c;
          if(c == 'X' || c == 'x')
            pos_x = index + 1;
          else if(c == 'Y' || c == 'y')
            pos_y = index + 1;
          else if(c == 'Z' || c == 'z')
            pos_z = index + 1;
          index++;
          c = drawing.file.read();
        }
        str += c;
        drawing.X = NONE_VALUE;
        drawing.Y = NONE_VALUE;
        if(pos_z >= 0){
          buff = "";
          uint8_t value = int(str.charAt(pos_z)) - 48;
          while(((value >= 0 && value <= 9) || str.charAt(pos_z) == '-') && pos_z < str.length()){
            buff += str.charAt(pos_z);
            pos_z++;
            value = int(str.charAt(pos_z)) - 48;
          }
          if(buff.toInt() > 0){
            drawing.draw_en = false;
            drawing.old_X = drawing.old_Y = NONE_VALUE;
            drawing.X = drawing.Y = NONE_VALUE;
          }
          else{
            drawing.draw_en = true;
          }
        }
        if(pos_x >= 0){
          buff = "";
          uint8_t value = int(str.charAt(pos_x)) - 48;
          while(((value >= 0 && value <= 9) || str.charAt(pos_x) == '-') && pos_x < str.length()){
            buff += str.charAt(pos_x);
            pos_x++;
            value = int(str.charAt(pos_x)) - 48;
          }
          if(drawing.old_X == NONE_VALUE || drawing.draw_en == false)
            drawing.old_X = buff.toFloat();
          else
            drawing.X = buff.toFloat();
        }
        if(pos_y >= 0){
          buff = "";
          uint8_t value = int(str.charAt(pos_y)) - 48;
          while(((value >= 0 && value <= 9) || str.charAt(pos_y) == '-') && pos_y < str.length()){
            buff += str.charAt(pos_y);
            pos_y++;
            value = int(str.charAt(pos_y)) - 48;
          }
          if(drawing.old_Y == NONE_VALUE || drawing.draw_en == false)
            drawing.old_Y = buff.toFloat();
          else
            drawing.Y = buff.toFloat();
        }
        if(drawing.old_X != NONE_VALUE && drawing.old_Y != NONE_VALUE){
          if(drawing.X == NONE_VALUE && drawing.Y != NONE_VALUE){
            drawing.X = drawing.old_X;
          }
          else if(drawing.X != NONE_VALUE && drawing.Y == NONE_VALUE){
            drawing.Y = drawing.old_Y;
          }
        }
        if(drawing.old_X == drawing.X && drawing.old_Y == drawing.Y){
          break;
        }
        if(drawing.old_X != NONE_VALUE && drawing.old_Y != NONE_VALUE && drawing.X != NONE_VALUE && drawing.Y != NONE_VALUE){
          drawing.x1 = (drawing.old_X+drawing.offset_x)*drawing.scale+drawing.base_x;
          drawing.y1 = height_y - (height_y - (drawing.offset_y+drawing.max_y)) - (drawing.old_Y+drawing.offset_y)*drawing.scale + drawing.base_y;
          drawing.x2 = (drawing.X+drawing.offset_x)*drawing.scale+drawing.base_x;
          drawing.y2 = height_y - (height_y - (drawing.offset_y+drawing.max_y)) - (drawing.Y+drawing.offset_y)*drawing.scale + drawing.base_y;
          drawing.d = sqrt(pow(drawing.x2-drawing.x1,2)+pow(drawing.y2-drawing.y1,2));
          drawing.old_x = -1, drawing.old_y = -1;
          drawing.i = 0.001;
          drawing.state = emRunning;
          break;
        }
        else{
          break;
        }
      }
      if(!drawing.file.available()){
        drawing.file.close();
        drawing.state = emIdle;
        drawing.running = false;
        
      }
    }
    else if(drawing.state == emRunning){
      uint16_t x = drawing.x1+(drawing.x2-drawing.x1)*drawing.i;
      uint16_t y = drawing.y1+(drawing.y2-drawing.y1)*drawing.i;
      if(drawing.old_x != x || drawing.old_y != y){
        tft.drawPixel(x,y,drawing.line_color);
        drawing.old_x = x;
        drawing.old_y = y;
        drawing.time_wait = micros() + drawing.delay;
      }
      drawing.i+= 0.001+3.2/(drawing.d*10);
      if(drawing.i >= 1){
        drawing.old_X = drawing.X;
        drawing.old_Y = drawing.Y;
        drawing.state = emReadGcode;
        if(drawing.file){
          if(!drawing.file.available()){
            drawing.file.close();
            drawing.state = emIdle;
            drawing.running = false;
          }
        }
      }
    }
  }
}
void setup()
{
  Serial.begin(115200);
  pinMode(LED_LCD_PIN,OUTPUT);
  pinMode(POWER_EN_PIN,OUTPUT);
  digitalWrite(LED_LCD_PIN,HIGH);
  digitalWrite(POWER_EN_PIN,HIGH);
  delay(10);
  
  is_RTC_ok = true;
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    is_RTC_ok = false;
  }
  if(!SD.begin()){
    Serial.println("Card Mount Failed");
    is_SD_ok = false;
  }
  else{
    is_SD_ok = true;
    uint8_t cardType = SD.cardType();
    if(cardType == CARD_NONE){
        Serial.println("No SD card attached");
        is_SD_ok = false;
    }
    else{
      Serial.print("SD Card Type: ");
      if(cardType == CARD_MMC){
          Serial.println("MMC");
      } else if(cardType == CARD_SD){
          Serial.println("SDSC");
      } else if(cardType == CARD_SDHC){
          Serial.println("SDHC");
      } else {
          Serial.println("UNKNOWN");
      }
      uint64_t cardSize = SD.cardSize() / (1024 * 1024);
      Serial.printf("SD Card Size: %lluMB\n", cardSize);
    }
  }

  displaied_1 = new bool[76800];
  displaied_2 = new bool[76800];
  for(uint32_t i = 0; i < 76800; i++){
    displaied_1[i] = 0;
    displaied_2[i] = 0;
  }
  for(uint8_t j = 0; j < 10; j++){
    mNumber[j] = new bool[6000];
    for(uint32_t i = 0; i < 6000; i++){
      mNumber[j][i] = 0;
    }
  }
  
  tft.init();
  tft.setRotation(3);
  tft.fillScreen(TFT_WHITE);

  generate_infoFile(SD,"/0.tap");
  
  for(uint8_t i = 0; i < 10; i++){
    draw(SD,"/"+String(i%10)+".tap",0,0,1,emMemory,TFT_RED);
    copyBigMem2Mem(0,0,i);
    clearMemory(0,0,60,100);
  }
  add_memory(0*60,0,0*60,0,60,100,TFT_RED,TFT_WHITE,0);
  add_memory(1*60,0,1*60,0,60,100,TFT_RED,TFT_WHITE,1);
  add_memory(3*60,0,3*60,0,60,100,TFT_RED,TFT_WHITE,2);
  add_memory(4*60,0,4*60,0,60,100,TFT_RED,TFT_WHITE,3);
  add_memory(6*60,0,6*60,0,60,100,TFT_RED,TFT_WHITE,4);
  add_memory(7*60,0,7*60,0,60,100,TFT_RED,TFT_WHITE,5);
  
  DateTime now = rtc.now();
  time_readRTC = millis() + 1000;
  timeRTC.hour = now.hour();
  timeRTC.minute = now.minute();
  timeRTC.second = now.second();
  draw(SD,"/"+String(timeRTC.hour/10)+".tap",0*60,0,1,emBoth,TFT_RED);
  draw(SD,"/"+String(timeRTC.hour%10)+".tap",1*60,0,1,emBoth,TFT_RED); 
  draw(SD,"/cham.tap",2*60,0,100,emScreen,TFT_RED);
  draw(SD,"/"+String(timeRTC.minute/10)+".tap",3*60,0,1,emBoth,TFT_RED);
  draw(SD,"/"+String(timeRTC.minute%10)+".tap",4*60,0,1,emBoth,TFT_RED);
  draw(SD,"/cham.tap",5*60,0,100,emScreen,TFT_RED);
  draw(SD,"/"+String(timeRTC.second/10)+".tap",6*60,0,1,emBoth,TFT_RED);
  draw(SD,"/"+String(timeRTC.second%10)+".tap",7*60,0,1,emBoth,TFT_RED);

  drawing.Gcode_drawed = 0;
  drawing.state = emIdle;
}

void loop()
{
//  if(Serial.available()){
//    Serial.println("Bye!");
//    delay(10);
//    esp_deep_sleep_start();
//  }
  if(millis() >= time_readRTC){
    DateTime now = rtc.now();
    time_readRTC = millis() + 1000;
    if(timeRTC.second%10 != now.second()%10){
      copyMem2BigMem(7*60,0,now.second()%10);
      memory.fresh[5] = true;
      if(timeRTC.second/10 != now.second()/10){
        copyMem2BigMem(6*60,0,now.second()/10);
        memory.fresh[4] = true;
      }
    }
    if(timeRTC.minute%10 != now.minute()%10){
      copyMem2BigMem(4*60,0,now.minute()%10);
      memory.fresh[3] = true;
      if(timeRTC.minute/10 != now.minute()/10){
        copyMem2BigMem(3*60,0,now.minute()/10);
        memory.fresh[2] = true;
      }
    }
    if(timeRTC.hour%10 != now.hour()%10){
      copyMem2BigMem(1*60,0,now.hour()%10);
      memory.fresh[1] = true;
      if(timeRTC.hour/10 != now.hour()/10){
        copyMem2BigMem(0*60,0,now.hour()/10);
        memory.fresh[0] = true;
      }
    }
    drawMemory(1000, true);
    timeRTC.hour = now.hour();
    timeRTC.minute = now.minute();
    timeRTC.second = now.second();
  }
  drawMemory();
  draw(SD);
  if(drawing.state == emIdle){
    if(drawing.Gcode_drawed == 0){
      draw(SD,"/eclogo.tap",0,0,3000,emScreen,TFT_BLUE,true);
      drawing.Gcode_drawed++;
    }
    else if(drawing.Gcode_drawed == 1){
      draw(SD,"/elec.tap",0,0,2000,emScreen,TFT_BLUE,true);
      drawing.Gcode_drawed++;
    }
    else if(drawing.Gcode_drawed == 2){
      draw(SD,"/channel.tap",0,0,2000,emScreen,TFT_BLUE,true);
      drawing.Gcode_drawed++;
    }
    else if(drawing.Gcode_drawed == 3){
      draw(SD,"/youtube.tap",0,0,3000,emScreen,TFT_RED,true);
      drawing.Gcode_drawed++;
    }
    else if(drawing.Gcode_drawed == 4){
      draw(SD,"/eclogo.tap",0,0,100,emScreen,TFT_WHITE,true);
      drawing.Gcode_drawed++;
    }
    else if(drawing.Gcode_drawed == 5){
      draw(SD,"/elec.tap",0,0,100,emScreen,TFT_WHITE,true);
      drawing.Gcode_drawed++;
    }
    else if(drawing.Gcode_drawed == 6){
      draw(SD,"/channel.tap",0,0,100,emScreen,TFT_WHITE,true);
      drawing.Gcode_drawed++;
    }
    else if(drawing.Gcode_drawed == 7){
      draw(SD,"/youtube.tap",0,0,100,emScreen,TFT_WHITE,true);
      drawing.Gcode_drawed++;
    }
    else if(drawing.Gcode_drawed == 8){
      draw(SD,"/boy_1.tap",0,0,2000,emScreen,TFT_BLACK,true);
      drawing.Gcode_drawed++;
    }
    else if(drawing.Gcode_drawed == 9){
      draw(SD,"/boy_2.tap",0,0,2000,emScreen,TFT_BLACK,true);
      drawing.Gcode_drawed++;
    }
    else if(drawing.Gcode_drawed == 10){
      draw(SD,"/boy_3.tap",0,0,2000,emScreen,TFT_BLACK,true);
      drawing.Gcode_drawed++;
    }
    else{
      drawing.state = emFinish;
    }
  }
}
