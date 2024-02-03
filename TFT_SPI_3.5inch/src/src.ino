#include "data.h"

TFT_eSPI tft = TFT_eSPI();
RTC_DS3231 rtc;

#define BG_COLOR TFT_BLACK

void display_keyPadChar(uint8_t kind){
  tft.setTextSize(1); //12x15
  tft.setTextColor(TFT_GREEN);
  for(uint8_t i = 0; i < 20; i++){
    tft.fillRoundRect((i%10)*48,80+48*(i/10),48,48,6,BG_COLOR);
    tft.drawRoundRect((i%10)*48,80+48*(i/10),48,48,6,~BG_COLOR);
    tft.drawCentreString(String(KEY_BOARD[kind][i]),(i%10)*48+24,(i/10)*48+80+15,4);
  }
  
  for(uint8_t i = 0; i < 9; i++){
    tft.fillRoundRect(i*48+10,80+48*2,48,48,6,BG_COLOR);
    tft.drawRoundRect(i*48+10,80+48*2,48,48,6,~BG_COLOR);
    tft.drawCentreString(String(KEY_BOARD[kind][i+20]),(i%10)*48+24+10,2*48+80+15,4);
  }
  
  tft.drawRoundRect(0,80+48*3,40,48,6,~BG_COLOR);
  tft.fillTriangle(10,224+23,20,224+9,30,224+23,TFT_GREEN);
  tft.fillRect(15,224+23,11,13,TFT_GREEN);
  for(uint8_t i = 0; i < 7; i++){
    tft.fillRoundRect(i*48+40,80+48*3,48,48,6,BG_COLOR);
    tft.drawRoundRect(i*48+40,80+48*3,48,48,6,~BG_COLOR);
    tft.drawCentreString(String(KEY_BOARD[kind][i+29]),(i%10)*48+24+40,3*48+80+15,4);
  }
  tft.drawRoundRect(7*48+40,80+48*3,104,48,6,~BG_COLOR);
  tft.fillTriangle(376+38,224+24,376+53,224+14,376+52,224+34,TFT_GREEN);
  tft.fillRect(376+52,224+19,13,10,TFT_GREEN);
  
  for(uint8_t i = 0; i < 3; i++){
    tft.drawRoundRect(i*53,80+48*4,53,48,6,~BG_COLOR);
    tft.drawCentreString(FUNCTION_NAME[i],i*53+26,4*48+80+15,4);
  }
  tft.drawRoundRect(3*53,80+48*4,53*3,48,6,~BG_COLOR);
  tft.drawRoundRect(6*53,80+48*4,53,48,6,~BG_COLOR);
  tft.drawCentreString(FUNCTION_NAME[3],6*53+26,4*48+80+15,4);
  tft.drawRoundRect(7*53,80+48*4,53*2,48,6,~BG_COLOR);
  tft.drawCentreString(FUNCTION_NAME[4],7*53+53,4*48+80+15,4);
}
String keyInput(String comment, String mes, uint32_t delay_time){
  char dataBuff[64] = {NULL};
  uint8_t key = 0, previous_key = 0;
  tft.fillRect(0,0,480,320,BG_COLOR);
  display_keyPadChar(0);
  tft.setTextColor(~BG_COLOR);
  tft.drawCentreString(comment,240,1,4);
  uint32_t time_blinkCursor = 0;
  uint16_t cursor_postion = 0;
  bool cursor_status = 1;
  uint16_t tx = 0, ty = 0;
  boolean pressed;
  bool is_print = false;
  uint16_t index_mes = 0;
  uint32_t time_execute = millis()+1000;
  while(true) {
    if(millis() >= time_blinkCursor){
      time_blinkCursor = millis() + cursor_status*200 + 300;
      cursor_status = !cursor_status;
      tft.fillRect(cursor_postion,50,2,26,(~BG_COLOR)*cursor_status);
    }
    pressed = false;
    if(millis() >= time_execute && index_mes < mes.length()){
      pressed = true;
      if(mes.charAt(index_mes) == ' '){
        tx = 53/2 + 4*53;
        ty = 80 + 28 + 4*48;
        index_mes++;
      }
      else{
        uint8_t c = 37;
        int8_t key_to = -1;
        if(c >= 37){
          for(c = 0; c < 37; c++){
            if(mes.charAt(index_mes) == KEY_BOARD[0][c]){
              key_to = 0;
              break;
            }
          }
        }
        if(c >= 37){
          for(c = 0; c < 37; c++){
            if(mes.charAt(index_mes) == KEY_BOARD[1][c]){
              key_to = 1;
              break;
            }
          }
        }
        if(c >= 37){
          for(c = 0; c < 37; c++){
            if(mes.charAt(index_mes) == KEY_BOARD[2][c]){
              key_to = 2;
              break;
            }
          }
        }
        if(key != key_to){
          if(key_to == 0){
            if(key == 1){ tx = 20; ty = 248; }
            else{ previous_key = 0; tx = 53/2 + 2*53; ty = 296; }
          }
          else if(key_to == 1){
            if(key == 0){ tx = 20; ty = 248; }
            else{ previous_key = 1; tx = 53/2 + 2*53; ty = 296; }
          }
          else if(key_to == 2){
            tx = 53/2 + 1*53; 
            ty = 296;
          }
          c = 37;
        }
        
        if(c < 37){
          if(c < 20){
            tx = 24 + (c%10)*48;
            ty = 80 + 28 + (c/10)*48;
          }
          else if(c < 29){
            tx = 24 + (c%20)*48 + 10;
            ty = 80 + 28 + 2*48;
          }
          else if(c < 37){
            tx = 24 + (c%29)*48 + 40;
            ty = 80 + 28 + 3*48;
          }
          index_mes++;
        }
      }
      time_execute = millis()+300+delay_time;
    }
    else if(millis() >= time_execute && index_mes >= mes.length()){
      delay(1000);
      pressed = true;
      tx = 440;
      ty = 296;
    }
    if (pressed && ty > 80){
      tft.setTextSize(1); //12x15
      tft.setTextColor(BG_COLOR);
      if(ty < 176){
        tft.fillRoundRect(tx/48*48,(ty-80)/48*48+80,48,48,6,TFT_GREEN);
        tft.drawCentreString(String(KEY_BOARD[key][((ty-80)/48)*10 + tx/48]),tx/48*48+24,(ty-80)/48*48+80+15,4);
      }
      else if(ty < 224){
        if(tx >= 10 && tx <= 442){
          tft.fillRoundRect(tx/48*48+10,(ty-80)/48*48+80,48,48,6,TFT_GREEN);
          tft.drawCentreString(String(KEY_BOARD[key][20 + (tx-10)/48]),tx/48*48+24+10,(ty-80)/48*48+80+15,4);
        }
      }
      else if(ty < 272){
        if(tx < 40){
          tft.fillRoundRect(0,80+48*3,40,48,6,TFT_GREEN);
          tft.fillTriangle(10,224+23,20,224+9,30,224+23,BG_COLOR);
          tft.fillRect(15,224+23,11,13,BG_COLOR);
        }
        else if(tx >= 40 && tx <= 376){
          tft.fillRoundRect((tx-40)/48*48+40,(ty-80)/48*48+80,48,48,6,TFT_GREEN);
          tft.drawCentreString(String(KEY_BOARD[key][29 + (tx-40)/48]),(tx-40)/48*48+24+40,(ty-80)/48*48+80+15,4);
        }
        else{
          tft.fillRoundRect(7*48+40,80+48*3,104,48,6,TFT_GREEN);
          tft.fillTriangle(376+38,224+24,376+53,224+14,376+52,224+34,BG_COLOR);
          tft.fillRect(376+52,224+19,13,10,BG_COLOR);
        }
      }
      else{
        if(tx < 48*3){
          tft.fillRoundRect(tx/53*53,(ty-80)/48*48+80,53,48,6,TFT_GREEN);
          tft.drawCentreString(FUNCTION_NAME[tx/53],tx/53*53+26,4*48+80+15,4);
        }
        else if(tx >= 48*3 && tx <= 48*6){
          tft.fillRoundRect(3*53,80+48*4,53*3,48,6,TFT_GREEN);
        }
        else if(tx >= 48*6 && tx <= 48*7){
          tft.fillRoundRect(6*53,80+48*4,53,48,6,TFT_GREEN);
          tft.drawCentreString(FUNCTION_NAME[3],6*53+26,4*48+80+15,4);
        }
        else{
          tft.fillRoundRect(7*53,80+48*4,53*2,48,6,TFT_GREEN);
          tft.drawCentreString(FUNCTION_NAME[4],7*53+53,4*48+80+15,4);
        }
      }
      delay(300);
      tft.setTextColor(TFT_GREEN);
      if(ty < 176){
        tft.fillRoundRect(tx/48*48,(ty-80)/48*48+80,48,48,6,BG_COLOR);
        tft.drawRoundRect(tx/48*48,(ty-80)/48*48+80,48,48,6,~BG_COLOR);
        tft.drawCentreString(String(KEY_BOARD[key][((ty-80)/48)*10 + tx/48]),tx/48*48+24,(ty-80)/48*48+80+15,4);
      }
      else if(ty < 224){
        if(tx >= 10 && tx <= 442){
          tft.fillRoundRect(tx/48*48+10,(ty-80)/48*48+80,48,48,6,BG_COLOR);
          tft.drawRoundRect(tx/48*48+10,(ty-80)/48*48+80,48,48,6,~BG_COLOR);
          tft.drawCentreString(String(KEY_BOARD[key][20 + (tx-10)/48]),tx/48*48+24+10,(ty-80)/48*48+80+15,4);
        }
      }
      else if(ty < 272){
        if(tx < 40){
          tft.fillRoundRect(0,80+48*3,40,48,6,BG_COLOR);
          tft.drawRoundRect(0,80+48*3,40,48,6,~BG_COLOR);
          tft.fillTriangle(10,224+23,20,224+9,30,224+23,TFT_GREEN);
          tft.fillRect(15,224+23,11,13,TFT_GREEN);
        }
        else if(tx >= 40 && tx <= 376){
          tft.fillRoundRect((tx-40)/48*48+40,(ty-80)/48*48+80,48,48,6,BG_COLOR);
          tft.drawRoundRect((tx-40)/48*48+40,(ty-80)/48*48+80,48,48,6,~BG_COLOR);
          tft.drawCentreString(String(KEY_BOARD[key][29 + (tx-40)/48]),(tx-40)/48*48+24+40,(ty-80)/48*48+80+15,4);
        }
        else{
          tft.fillRoundRect(7*48+40,80+48*3,104,48,6,BG_COLOR);
          tft.drawRoundRect(7*48+40,80+48*3,104,48,6,~BG_COLOR);
          tft.fillTriangle(376+38,224+24,376+53,224+14,376+52,224+34,TFT_GREEN);
          tft.fillRect(376+52,224+19,13,10,TFT_GREEN);
        }
      }
      else{
        if(tx < 48*3){
          tft.fillRoundRect(tx/53*53,4*48+80,53,48,6,BG_COLOR);
          tft.drawRoundRect(tx/53*53,4*48+80,53,48,6,~BG_COLOR);
          tft.drawCentreString(FUNCTION_NAME[tx/53],tx/53*53+26,4*48+80+15,4);
        }
        else if(tx >= 48*3 && tx <= 48*6){
          tft.fillRoundRect(3*53,80+48*4,53*3,48,6,BG_COLOR);
          tft.drawRoundRect(3*53,80+48*4,53*3,48,6,~BG_COLOR);
        }
        else if(tx >= 48*6 && tx <= 48*7){
          tft.fillRoundRect(6*53,80+48*4,53,48,6,BG_COLOR);
          tft.drawRoundRect(6*53,80+48*4,53,48,6,~BG_COLOR);
          tft.drawCentreString(FUNCTION_NAME[3],6*53+26,4*48+80+15,4);
        }
        else{
          tft.fillRoundRect(7*53,80+48*4,53*2,48,6,BG_COLOR);
          tft.drawRoundRect(7*53,80+48*4,53*2,48,6,~BG_COLOR);
          tft.drawCentreString(FUNCTION_NAME[4],7*53+53,4*48+80+15,4);
        }
      }
      
      if((ty-80)/48 < 4) {
        if(tx < 40 && (ty-80)/48 == 3){
          if(key != 2){
            key = !key;
            display_keyPadChar(key);
          }
        }
        else if(tx >= 376 && (ty-80)/48 == 3 && strlen(dataBuff) > 0){
          dataBuff[strlen(dataBuff)-1] = NULL;
          tft.fillRect(0,50,cursor_postion,27,BG_COLOR);
          is_print = true;
        }
        else if(cursor_postion < 450){
          if((ty-80)/48 < 2)
            dataBuff[strlen(dataBuff)] = KEY_BOARD[key][((ty-80)/48)*10+tx/48];
          else if((ty-80)/48 < 3)
            dataBuff[strlen(dataBuff)] = KEY_BOARD[key][(tx-10)/48+20];
          else if((ty-80)/48 == 3)
            dataBuff[strlen(dataBuff)] = KEY_BOARD[key][(tx-40)/48+29];
          is_print = true;
        }
      }
      else if((ty-80)/48 == 4) {
        if(tx/53 == 0) {
          return "::exit";
        }
        else if(tx/53 == 1) {
          if(key != 2) {
            previous_key = key;
            key = 2;
            display_keyPadChar(key);
          }
        }
        else if(tx/53 == 2) {
          if(key == 2) {
            key = previous_key;
            display_keyPadChar(key);
          }
        }
        else if(tx/53 >= 3 && tx/53 < 6){
          if(cursor_postion < 450){
            uint8_t len = strlen(dataBuff);
            dataBuff[len] = ' ';
            dataBuff[len+1] = NULL;
            is_print = true;
          }
        }
        else if(tx/53 >= 7 && tx/53 < 9){
          return String(dataBuff);
        }
      }
      
      if(is_print){
        tft.setTextSize(1);
        tft.setTextColor(TFT_GREEN);
        tft.fillRect(cursor_postion,50,2,26,BG_COLOR);
        cursor_postion = tft.drawString(dataBuff,0,50,4);
        tft.fillRect(cursor_postion,50,2,26,~BG_COLOR);
        is_print = false;
        cursor_status = true;
        time_blinkCursor = millis() + 300;
      }
    }
  }
}
String keyInput(String comment){
  char dataBuff[64] = {NULL};
  uint8_t key = 0, previous_key = 0;
  tft.fillRect(0,0,480,320,BG_COLOR);
  display_keyPadChar(0);
  tft.setTextColor(~BG_COLOR);
  tft.drawCentreString(comment,240,1,4);
  uint32_t time_blinkCursor = 0;
  uint16_t cursor_postion = 0;
  bool cursor_status = 1;
  uint16_t tx = 0, ty = 0;
  boolean pressed;
  bool is_print = false;
  while(true) {
    if(millis() >= time_blinkCursor){
      time_blinkCursor = millis() + cursor_status*200 + 300;
      cursor_status = !cursor_status;
      tft.fillRect(cursor_postion,50,2,26,(~BG_COLOR)*cursor_status);
    }
    pressed = tft.getTouch(&tx, &ty);
    if (pressed && ty > 80){
      tft.setTextSize(1); //12x15
      tft.setTextColor(BG_COLOR);
      if(ty < 176){
        tft.fillRoundRect(tx/48*48,(ty-80)/48*48+80,48,48,6,TFT_GREEN);
      }
      else if(ty < 224){
        if(tx >= 10 && tx <= 442){
          tft.fillRoundRect(tx/48*48+10,(ty-80)/48*48+80,48,48,6,TFT_GREEN);
        }
      }
      else if(ty < 272){
        if(tx < 40){
          tft.fillRoundRect(0,80+48*3,40,48,6,TFT_GREEN);
          tft.fillTriangle(10,224+23,20,224+9,30,224+23,BG_COLOR);
          tft.fillRect(15,224+23,11,13,BG_COLOR);
        }
        else if(tx >= 40 && tx <= 376){
          tft.fillRoundRect((tx-40)/48*48+40,(ty-80)/48*48+80,48,48,6,TFT_GREEN);
        }
        else{
          tft.fillRoundRect(7*48+40,80+48*3,104,48,6,TFT_GREEN);
          tft.fillTriangle(376+38,224+24,376+53,224+14,376+52,224+34,BG_COLOR);
          tft.fillRect(376+52,224+19,13,10,BG_COLOR);
        }
      }
      else{
        if(tx < 48*3){
          tft.fillRoundRect(tx/53*53,(ty-80)/48*48+80,53,48,6,TFT_GREEN);
        }
        else if(tx >= 48*3 && tx <= 48*6){
          tft.fillRoundRect(3*53,80+48*4,53*3,48,6,TFT_GREEN);
        }
        else if(tx >= 48*6 && tx <= 48*7){
          tft.fillRoundRect(6*53,80+48*4,53,48,6,TFT_GREEN);
        }
        else{
          tft.fillRoundRect(7*53,80+48*4,53*2,48,6,TFT_GREEN);
        }
      }
      while(true){
        bool state, oldstate;
        uint8_t count = 0;
        while (count < 5) {
          state = tft.getTouch(&touch_x, &touch_y);
          if (state == oldstate) count++;
          else count = 0;
          oldstate = state;
          delay(1);
        }
        if(oldstate == false){
          break;
        }
      }
      tft.setTextColor(TFT_GREEN);
      if(ty < 176){
        tft.fillRoundRect(tx/48*48,(ty-80)/48*48+80,48,48,6,BG_COLOR);
        tft.drawRoundRect(tx/48*48,(ty-80)/48*48+80,48,48,6,~BG_COLOR);
        tft.drawCentreString(String(KEY_BOARD[key][((ty-80)/48)*10 + tx/48]),tx/48*48+24,(ty-80)/48*48+80+15,4);
      }
      else if(ty < 224){
        if(tx >= 10 && tx <= 442){
          tft.fillRoundRect(tx/48*48+10,(ty-80)/48*48+80,48,48,6,BG_COLOR);
          tft.drawRoundRect(tx/48*48+10,(ty-80)/48*48+80,48,48,6,~BG_COLOR);
          tft.drawCentreString(String(KEY_BOARD[key][20 + (tx-10)/48]),tx/48*48+24+10,(ty-80)/48*48+80+15,4);
        }
      }
      else if(ty < 272){
        if(tx < 40){
          tft.fillRoundRect(0,80+48*3,40,48,6,BG_COLOR);
          tft.drawRoundRect(0,80+48*3,40,48,6,~BG_COLOR);
          tft.fillTriangle(10,224+23,20,224+9,30,224+23,TFT_GREEN);
          tft.fillRect(15,224+23,11,13,TFT_GREEN);
        }
        else if(tx >= 40 && tx <= 376){
          tft.fillRoundRect((tx-40)/48*48+40,(ty-80)/48*48+80,48,48,6,BG_COLOR);
          tft.drawRoundRect((tx-40)/48*48+40,(ty-80)/48*48+80,48,48,6,~BG_COLOR);
          tft.drawCentreString(String(KEY_BOARD[key][29 + (tx-40)/48]),(tx-40)/48*48+24+40,(ty-80)/48*48+80+15,4);
        }
        else{
          tft.fillRoundRect(7*48+40,80+48*3,104,48,6,BG_COLOR);
          tft.drawRoundRect(7*48+40,80+48*3,104,48,6,~BG_COLOR);
          tft.fillTriangle(376+38,224+24,376+53,224+14,376+52,224+34,TFT_GREEN);
          tft.fillRect(376+52,224+19,13,10,TFT_GREEN);
        }
      }
      else{
        if(tx < 48*3){
          tft.fillRoundRect(tx/53*53,4*48+80,53,48,6,BG_COLOR);
          tft.drawRoundRect(tx/53*53,4*48+80,53,48,6,~BG_COLOR);
          tft.drawCentreString(FUNCTION_NAME[tx/53],tx/53*53+26,4*48+80+15,4);
        }
        else if(tx >= 48*3 && tx <= 48*6){
          tft.fillRoundRect(3*53,80+48*4,53*3,48,6,BG_COLOR);
          tft.drawRoundRect(3*53,80+48*4,53*3,48,6,~BG_COLOR);
        }
        else if(tx >= 48*6 && tx <= 48*7){
          tft.fillRoundRect(6*53,80+48*4,53,48,6,BG_COLOR);
          tft.drawRoundRect(6*53,80+48*4,53,48,6,~BG_COLOR);
          tft.drawCentreString(FUNCTION_NAME[3],6*53+26,4*48+80+15,4);
        }
        else{
          tft.fillRoundRect(7*53,80+48*4,53*2,48,6,BG_COLOR);
          tft.drawRoundRect(7*53,80+48*4,53*2,48,6,~BG_COLOR);
          tft.drawCentreString(FUNCTION_NAME[4],7*53+53,4*48+80+15,4);
        }
      }
      
      if((ty-80)/48 < 4) {
        if(tx < 40 && (ty-80)/48 >= 3){
          if(key != 2){
            key = !key;
            display_keyPadChar(key);
          }
        }
        else if(tx >= 376 && (ty-80)/48 >= 3 && strlen(dataBuff) > 0){
          dataBuff[strlen(dataBuff)-1] = NULL;
          tft.fillRect(0,50,cursor_postion,27,BG_COLOR);
          is_print = true;
        }
        else if(cursor_postion < 450){
          if((ty-80)/48 < 2)
            dataBuff[strlen(dataBuff)] = KEY_BOARD[key][((ty-80)/48)*10+tx/48];
          else if((ty-80)/48 < 3)
            dataBuff[strlen(dataBuff)] = KEY_BOARD[key][(tx-10)/48+20];
          else if((ty-80)/48 == 3)
            dataBuff[strlen(dataBuff)] = KEY_BOARD[key][(tx-40)/48+29];
          is_print = true;
        }
      }
      else if((ty-80)/48 == 4) {
        if(tx/53 == 0) {
          return "::exit";
        }
        else if(tx/53 == 1) {
          if(key != 2) {
            previous_key = key;
            key = 2;
            display_keyPadChar(key);
          }
        }
        else if(tx/53 == 2) {
          if(key == 2) {
            key = previous_key;
            display_keyPadChar(key);
          }
        }
        else if(tx/53 >= 3 && tx/53 < 6){
          if(cursor_postion < 450){
            uint8_t len = strlen(dataBuff);
            dataBuff[len] = ' ';
            dataBuff[len+1] = NULL;
            is_print = true;
          }
        }
        else if(tx/53 >= 7 && tx/53 < 9){
          return String(dataBuff);
        }
      }
      
      if(is_print){
        tft.setTextSize(1);
        tft.setTextColor(TFT_GREEN);
        tft.fillRect(cursor_postion,50,2,26,BG_COLOR);
        cursor_postion = tft.drawString(dataBuff,0,50,4);
        tft.fillRect(cursor_postion,50,2,26,~BG_COLOR);
        is_print = false;
        cursor_status = true;
        time_blinkCursor = millis() + 300;
      }
    }
  }
}
void check_touch(bool is_run = false){
  uint16_t calData[5];
  uint8_t calDataOK = 0;
  if (!SPIFFS.begin()) {
    Serial.println("Formating file system");
    SPIFFS.format();
    SPIFFS.begin();
  }
  if (SPIFFS.exists(CALIBRATION_FILE)) {
    if (is_run) {
      SPIFFS.remove(CALIBRATION_FILE);
    }
    else {
      File f = SPIFFS.open(CALIBRATION_FILE, "r");
      if (f) {
        if (f.readBytes((char *)calData, 14) == 14)
          calDataOK = 1;
        f.close();
      }
    }
  }

  if (calDataOK && !is_run) {
    tft.setTouch(calData);
  } else {
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(20, 0);
    tft.setTextFont(2);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    tft.println("Touch corners as indicated");
    tft.setTextFont(1);
    tft.println();

    tft.calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);
    
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.println("Calibration complete!");
    delay(1000);
    File f = SPIFFS.open(CALIBRATION_FILE, "w");
    if (f) {
      f.write((const unsigned char *)calData, 14);
      f.close();
    }
  }
}
void deleteFile(fs::FS &fs, const char * path){
    Serial.printf("Deleting file: %s\n", path);
    if(fs.remove(path)){
        Serial.println("File deleted");
    } else {
        Serial.println("Delete failed");
    }
}
void display_date(){
  char chr[30];
  String str = String(daysOfTheWeek[timeRTC.date]) + " ";
  str += String(monsOfTheYear[timeRTC.month-1]) + ".";
  str += add_zero(timeRTC.day) + ".";
  str += String(timeRTC.year);
  str.toCharArray(chr,str.length()+1);
  tft.setFreeFont(&FreeSansBold12pt7b);
  if(clock_type == emAnalog){
    key[0].initButton(&tft, 240, 15, 480, 30, TFT_BLACK, TFT_BLACK, TFT_WHITE, chr, 1);
  }
  else{
    uint8_t r = FONT_COLOR_R[numberFontImage];
    uint8_t g = FONT_COLOR_G[numberFontImage];
    uint8_t b = FONT_COLOR_B[numberFontImage];
    key[0].initButton(&tft, 240, 15, 480, 30, TFT_BLACK, TFT_BLACK, tft.color565(r,g,b), chr, 1);
  }
  key[0].drawButton(false);
  timeRTC.printed_date = true;
}
void drawMemory(uint32_t start, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t delay_time, uint8_t index){
  drawingMemory[index].delay = delay_time;
  drawingMemory[index].time_wait = millis();
  drawingMemory[index].running = true;
  drawingMemory[index].start_memory = start;
  drawingMemory[index].x = x;
  drawingMemory[index].y = y;
  drawingMemory[index].width = w;
  drawingMemory[index].height = h;
  drawingMemory[index].current_y = 0;
  drawingMemory[index].start_time = micros();
}
void drawMemory(){
  for(uint8_t i = 0; i < MAX_MEMORY_DRAWING; i++){
    if(drawingMemory[i].running == true && micros() >= drawingMemory[i].time_wait){
      for(uint16_t j = 0; j < drawingMemory[i].width; j++){
        uint32_t pos_mem = drawingMemory[i].start_memory+drawingMemory[i].current_y*drawingMemory[i].width+j;
        uint8_t number = pos_mem/5000;
        tft.drawPixel(drawingMemory[i].x+j, drawingMemory[i].y+drawingMemory[i].current_y, memory[number][pos_mem%5000]);
      }
      drawingMemory[i].time_wait = micros() + drawingMemory[i].delay;
      drawingMemory[i].current_y++;
      if(drawingMemory[i].current_y >= drawingMemory[i].height){
        drawingMemory[i].running = false;
      }
    }
  }
  if(read_newFontState == emChangeFontRequest && drawingMemory[0].running == false){
    for(uint16_t i = 0; i < 10; i++){
      for(uint16_t j = 0; j < 5000; j++){
        memory[i][j] = memory_buffer[i*5000+j];
      }
    }
    is_readNumberFont = false;
    timeRTC.printed_time = false;
    read_newFontState = emComplete;
    if(timeRTC.printed_date == false){
      display_date();
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
    Serial.print("Start generate info file ");
    Serial.println(path);
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
      if(str.indexOf("[S]") < 0){
        if(c == 'X' || c == 'x')
          pos_x = index + 1;
        else if(c == 'Y' || c == 'y')
          pos_y = index + 1;
        else if(c == 'Z' || c == 'z')
          pos_z = index + 1;
      }
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
    bool is_add = false;
    if(available_cmd == ""){
      if(str.indexOf("[S]") >= 0 || str.indexOf("[E]") >= 0){
        is_add = true;
        available_cmd = str;
      }
    }
    if((available_cmd != "" && (old_X != X || old_Y != Y || pos_z >= 0)) || file.available() <= 1 || is_add){
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
// for print gcode
uint16_t widthText(String text, sFont *font){
  uint8_t index = 0;
  uint16_t width = 0;
  while(index < text.length()){
    if(text.charAt(index) == ' '){
      width += 15;
    }
    else{
      uint8_t index_char = font->char_map[text.charAt(index)];
      width += font->width_char[index_char] + 5;
    }
    index++;
  }
  if(text.charAt(text.length()-1) != ' '){
    width -= 5;
  }
  return width;
}
void draw(fs::FS &fs, String path_string, int16_t base_x, int16_t base_y, uint32_t delay_time, uint16_t line_color = 0, uint8_t index_run = 0){
  if(is_SD_ok == false)
    return;
  char path_char[50];
  bool gen_info = false;
  path_string.toCharArray(path_char,path_string.length()+1);
  char info_filepath[50];
  String file_name = get_name(path_string);
  path_string.replace(file_name,"."+file_name);
  path_string.toCharArray(info_filepath,path_string.length()+1);
  File file = fs.open(info_filepath);
  if(!file){
    if(generate_infoFile(fs,path_char) == false){
      Serial.println("Cannot generate info file " + String(info_filepath));
    }
    gen_info = true;
  }
  file.close();
  if(gen_info){
    file = fs.open(info_filepath);
    if(!file){
      Serial.println("Cannot open info file " + String(info_filepath));
    }
    file.close();
  }
  core[index_run].destination = emScreen;
  path_string.toCharArray(core[index_run].path,path_string.length()+1);
  core[index_run].delay = delay_time;
  core[index_run].time_wait = micros();
  core[index_run].base_x = base_x;
  core[index_run].base_y = base_y;
  core[index_run].line_color = line_color;
  core[index_run].state = emInitilize;
  core[index_run].running = true;
}
void drawImage(fs::FS &fs, String path_string, uint16_t x, uint16_t y, uint16_t w, uint32_t delay_time, uint8_t index_run, bool saveToMem = false){
  if(is_SD_ok == false)
    return;
  char path_char[50];
  bool gen_info = false;
  path_string.toCharArray(path_char,path_string.length()+1);
  File file = fs.open(path_char);
  if(!file){
    return;
  }
  file.close();
  if(saveToMem == false){
    core[index_run].destination = emImage;
  }
  else{
    core[index_run].destination = emClockFace;
  }
  path_string.toCharArray(core[index_run].path,path_string.length()+1);
  core[index_run].delay = delay_time;
  core[index_run].time_wait = micros();
  core[index_run].base_x = x;
  core[index_run].base_y = y;
  core[index_run].x1 = 0;
  core[index_run].y1 = 0;
  core[index_run].w = w;
  core[index_run].state = emInitilize;
  core[index_run].running = true;
}
void readNumberFontFromImage(fs::FS &fs, String path_string, uint8_t index_run, uint32_t start_memory){
  if(is_SD_ok == false)
    return;
  char path_char[50];
  bool gen_info = false;
  path_string.toCharArray(path_char,path_string.length()+1);
  File file = fs.open(path_char);
  if(!file){
    Serial.println("Cannot open file " + path_string);
    return;
  }
  file.close();
  core[index_run].destination = emImageToMemory;
  core[index_run].memory.start_memory = start_memory;
  path_string.toCharArray(core[index_run].path,path_string.length()+1);
  core[index_run].time_wait = micros();
  core[index_run].x1 = 0;
  core[index_run].y1 = 0;
  core[index_run].state = emInitilize;
  core[index_run].running = true;
}
void readNumberFontFromGcode(fs::FS &fs, String path_string, uint8_t index_run, uint32_t start_memory, uint8_t width_memory){
  if(is_SD_ok == false)
    return;
  char path_char[50];
  bool gen_info = false;
  path_string.toCharArray(path_char,path_string.length()+1);
  char info_filepath[50];
  String file_name = get_name(path_string);
  path_string.replace(file_name,"."+file_name);
  path_string.toCharArray(info_filepath,path_string.length()+1);
  File file = fs.open(info_filepath);
  if(!file){
    if(generate_infoFile(fs,path_char) == false){
      Serial.println("Cannot generate info file " + String(info_filepath));
    }
    gen_info = true;
  }
  file.close();
  if(gen_info){
    file = fs.open(info_filepath);
    if(!file){
      Serial.println("Cannot open info file " + String(info_filepath));
    }
    file.close();
  }
  core[index_run].destination = emMemory;
  core[index_run].memory.width_memory = width_memory;
  core[index_run].memory.start_memory = start_memory;
  path_string.toCharArray(core[index_run].path,path_string.length()+1);
  core[index_run].time_wait = micros();
  core[index_run].base_x = 0;
  core[index_run].base_y = 0;
  core[index_run].state = emInitilize;
  core[index_run].running = true;
}
void readTextFont(fs::FS &fs, String path_string, uint8_t index_run, sFont *fontSave, bool original){
  char path_char[50];
  bool gen_info = false;
  path_string.toCharArray(path_char,path_string.length()+1);
  char info_filepath[50];
  String file_name = get_name(path_string);
  path_string.replace(file_name,"."+file_name);
  path_string.toCharArray(info_filepath,path_string.length()+1);
  File file = fs.open(info_filepath);
  if(!file){
    if(generate_infoFile(fs,path_char) == false){
      Serial.println("Cannot generate info file " + String(info_filepath));
    }
    gen_info = true;
  }
  file.close();
  if(gen_info){
    file = fs.open(info_filepath);
    if(!file){
      Serial.println("Cannot open info file " + String(info_filepath));
    }
    file.close();
  }
  core[index_run].fontRun = fontSave;
  core[index_run].fontRun->original = original;
  core[index_run].fontRun->index = 0;
  core[index_run].fontRun->index_seek = 0;
  core[index_run].destination = emGetFont;
  path_string.toCharArray(core[index_run].path,path_string.length()+1);
  core[index_run].time_wait = micros();
  core[index_run].min_x = 55555;
  core[index_run].max_x = 0;
  core[index_run].state = emInitilize;
  core[index_run].running = true;
}
void printText(fs::FS &fs, String path_string, String text, int16_t base_x, int16_t base_y, uint8_t index_run, uint32_t delay_time, uint16_t line_color, sFont *fontSave, bool open_f = true, bool close_f = true){
  if(is_SD_ok == false)
    return;
  char path_char[50];
  bool gen_info = false;
  path_string.toCharArray(path_char,path_string.length()+1);
  char info_filepath[50];
  String file_name = get_name(path_string);
  path_string.replace(file_name,"."+file_name);
  path_string.toCharArray(info_filepath,path_string.length()+1);
  File file = fs.open(info_filepath);
  if(!file){
    if(generate_infoFile(fs,path_char) == false){
      Serial.println("Cannot generate info file " + String(info_filepath));
    }
    gen_info = true;
  }
  file.close();
  if(gen_info){
    file = fs.open(info_filepath);
    if(!file){
      Serial.println("Cannot open info file " + String(info_filepath));
    }
    file.close();
  }
  core[index_run].fontRun = fontSave;
  core[index_run].destination = emText;
  core[index_run].restoreImage = false;
  core[index_run].close_file = close_f;
  if(!core[index_run].file){
    core[index_run].open_file = true;
  }
  else{
    core[index_run].open_file = open_f;
  }
  path_string.toCharArray(core[index_run].path,path_string.length()+1);
  core[index_run].text.text = text;
  core[index_run].text.index_text = 0;
  core[index_run].text.next_base_x = 0;
  core[index_run].line_color = line_color;
  core[index_run].delay = delay_time;
  core[index_run].time_wait = micros();
  core[index_run].base_x = base_x;
  core[index_run].base_y = base_y;
  core[index_run].state = emInitilize;
  core[index_run].running = true;
}
void restoreDisplay(fs::FS &fs, String path_string, String text, int16_t base_x, int16_t base_y, uint8_t index_run, sFont *fontSave, bool open_f = true, bool close_f = true){
  if(is_SD_ok == false)
    return;
  char path_char[50];
  bool gen_info = false;
  path_string.toCharArray(path_char,path_string.length()+1);
  char info_filepath[50];
  String file_name = get_name(path_string);
  path_string.replace(file_name,"."+file_name);
  path_string.toCharArray(info_filepath,path_string.length()+1);
  File file = fs.open(info_filepath);
  if(!file){
    if(generate_infoFile(fs,path_char) == false){
      Serial.println("Cannot generate info file " + String(info_filepath));
    }
    gen_info = true;
  }
  file.close();
  if(gen_info){
    file = fs.open(info_filepath);
    if(!file){
      Serial.println("Cannot open info file " + String(info_filepath));
    }
    file.close();
  }
  core[index_run].fontRun = fontSave;
  core[index_run].destination = emText;
  core[index_run].restoreImage = true;
  core[index_run].close_file = close_f;
  if(!core[index_run].file){
    core[index_run].open_file = true;
  }
  else{
    core[index_run].open_file = open_f;
  }
  path_string.toCharArray(core[index_run].path,path_string.length()+1);
  core[index_run].text.text = text;
  core[index_run].text.index_text = 0;
  core[index_run].text.next_base_x = 0;
  core[index_run].delay = 1;
  core[index_run].time_wait = micros();
  core[index_run].base_x = base_x;
  core[index_run].base_y = base_y;
  core[index_run].state = emInitilize;
  core[index_run].running = true;
}
void stop_draw(int8_t index){
  if(is_SD_ok == false)
    return;
  uint8_t start;
  if(index < 0){
    start = 0;
    index = MAX_RUNGCODE - 1;
  }
  else
    start = index;
  for(uint8_t i = start; i <= index; i++){
    if(core[i].file){
      core[i].file.close();
    }
    core[i].state = emIdle;
    core[i].running = false;
  }
}
void draw(fs::FS &fs){
  for(uint8_t m = 0; m < MAX_RUNGCODE; m++){
    if(core[m].running == false){
      continue;
    }
    if(m == 4){
      if(core[0].running || core[1].running || core[2].running){
        continue;
      }
    }
    if(micros() >= core[m].time_wait){
      if(core[m].state == emInitilize){
        String str = "";
        if(core[m].open_file == true || core[m].destination != emText){
          core[m].file = fs.open(core[m].path);
          if(!core[m].file){
            core[m].state = emError;
            Serial.println("Cannot open file " + String(core[m].path));
            core[m].running = false;
            return;
          }
        }
        if(core[m].destination != emImage && core[m].destination != emImageToMemory){
          if(core[m].destination != emGetFont && core[m].destination != emText){
            char c = core[m].file.read();
            uint8_t index = 0;
            while(core[m].file.available() && index < 4){
              if(c == '\n'){
                if(index == 0){
                  core[m].max_x = str.toInt();
                }
                else if(index == 1){
                  core[m].min_x = str.toInt();
                }
                else if(index == 2){
                  core[m].max_y = str.toInt();
                }
                else if(index == 3){
                  core[m].min_y = str.toInt();
                }
                index++;
                str = "";
              }
              else{
                str += c;
              }
              c = core[m].file.read();
            }
            // end
            
            uint8_t ro = tft.getRotation();
            width_x = 480;
            height_y = 320;
            if(ro == 0 || ro == 2){
              width_x = 320;
              height_y = 480;
            }
            core[m].scale = 1;
            if(core[m].max_x - core[m].min_x > width_x || core[m].max_y - core[m].min_y > height_y){
              if((core[m].max_x - core[m].min_x - width_x) > (core[m].max_y - core[m].min_y - height_y)){
                core[m].scale = width_x/(core[m].max_x - core[m].min_x);
              }
              else{
                core[m].scale = height_y/(core[m].max_y - core[m].min_y);
              }
            }
            
            core[m].offset_x = 0;
            if(core[m].min_x < 0){
              core[m].offset_x = core[m].min_x*-1;
            }
            core[m].offset_y = 0;
            if(core[m].min_y < 0){
              core[m].offset_y = core[m].min_y*-1;
            }
          }
          else if(core[m].destination == emText){
            core[m].offset_y = 0;
            core[m].old_X = NONE_VALUE;
            core[m].old_Y = NONE_VALUE;
            core[m].old_x = -1;
            core[m].old_y = -1;
            core[m].x1 = -1;
            core[m].y1 = -1;
            core[m].x2 = -1;
            core[m].y2 = -1;
            core[m].scale = 1;
            uint8_t index_char = core[m].fontRun->char_map[core[m].text.text.charAt(core[m].text.index_text)];
            if(core[m].fontRun->original == false){
              core[m].base_x = core[m].text.next_base_x + core[m].base_x;
              core[m].offset_x = - core[m].fontRun->start_x[index_char];
              core[m].text.next_base_x = core[m].base_x + core[m].fontRun->width_char[index_char]+5;
            }
            core[m].file.seek(core[m].fontRun->postion[index_char] + 5);
            core[m].text.index_text++;
          }
          core[m].draw_en = false;
        }
        core[m].state = emReadGcode;
      }
      else if(core[m].state == emReadGcode){
        String str = "";
        String buff = "";
        while(core[m].file.available()){
          char c = core[m].file.read();
          uint8_t index = 1;
          int8_t pos_x = -1, pos_y = -1, pos_z = -1;
          while(c != '\n' && core[m].file.available()){
            str += c;
            if(str.indexOf("[S]") < 0){
              if(c == 'X' || c == 'x')
                pos_x = index;
              else if(c == 'Y' || c == 'y')
                pos_y = index;
              else if(c == 'Z' || c == 'z')
                pos_z = index;
            }
            index++;
            c = core[m].file.read();
          }

          if(core[m].destination == emImage || core[m].destination == emClockFace){
            if(str.length() > 1){
              tft.drawPixel(core[m].base_x+core[m].x1,core[m].base_y+core[m].y1,str.toInt());
              if(core[m].destination == emClockFace){
                if(core[m].y1 < 170){
                  // per memory can save 17 row
                  memory[core[m].y1/17][core[m].x1+core[m].y1*290-core[m].y1/17*17*290] = str.toInt();
                }
                else{
                  memory_buffer[core[m].x1+core[m].y1*290-49300] = str.toInt();
                }
              }
            }
            core[m].x1++;
            if(core[m].x1 >= core[m].w){
              core[m].time_wait = micros()+core[m].delay;
              core[m].y1++;
              core[m].x1 = 0;
            }
            break;
          }
          else if(core[m].destination == emImageToMemory){
            if(str.length() > 1){
              memory_buffer[core[m].memory.start_memory+core[m].x1] = str.toInt();
            }
            else{
              memory_buffer[core[m].memory.start_memory+core[m].x1] = 0;
            }
            core[m].x1++;
            break;
          }
          else{
            str += c;
            if(core[m].destination == emGetFont){
              if(str.indexOf("[S]") >= 0 || str.indexOf("[E]") >= 0){
                if(core[m].fontRun->index > 0){
                  core[m].fontRun->width_char[core[m].fontRun->index-1] = core[m].max_x-core[m].min_x;
                  core[m].fontRun->start_x[core[m].fontRun->index-1] = core[m].min_x;
                  core[m].old_X = NONE_VALUE;
                  core[m].old_Y = NONE_VALUE;
                  core[m].min_x = 555555;
                  core[m].max_x = 0;
                }
                else{
                  core[m].min_x = 555555;
                  core[m].max_x = 0;
                }
                if(str.indexOf("[E]") >= 0 || core[m].fontRun->index >= core[m].fontRun->length_font){
                  core[m].file.close();
                  core[m].state = emIdle;
                  core[m].running = false;
                  break;
                }
                core[m].fontRun->postion[core[m].fontRun->index] = core[m].fontRun->index_seek;
                core[m].fontRun->character[core[m].fontRun->index] = str.charAt(3);
                core[m].fontRun->char_map[core[m].fontRun->character[core[m].fontRun->index]] = core[m].fontRun->index;
                core[m].fontRun->index++;
              }
              core[m].fontRun->index_seek += index;
            }
            else if(core[m].destination == emText){
              if(str.indexOf("[S]") >= 0 || str.indexOf("[E]") >= 0){
                while(core[m].text.text.charAt(core[m].text.index_text) == ' '){
                  core[m].text.index_text++;
                  core[m].text.next_base_x += 15;
                }
                if(core[m].text.index_text + 1 > core[m].text.text.length()){
                  if(core[m].close_file == true){
                    core[m].file.close();
                  }
                  core[m].state = emIdle;
                  core[m].running = false;
                  break;
                }
                uint8_t index_char = core[m].fontRun->char_map[core[m].text.text.charAt(core[m].text.index_text)];
                if(core[m].fontRun->original == false){
                  core[m].base_x = core[m].text.next_base_x;
                  core[m].offset_x = - core[m].fontRun->start_x[index_char];
                  core[m].text.next_base_x = core[m].text.next_base_x + core[m].fontRun->width_char[index_char]+5;
                }
                core[m].file.seek(core[m].fontRun->postion[index_char] + 5);
                core[m].old_X = NONE_VALUE;
                core[m].old_Y = NONE_VALUE;
                core[m].old_x = -1;
                core[m].old_y = -1;
                core[m].x1 = -1;
                core[m].y1 = -1;
                core[m].x2 = -1;
                core[m].y2 = -1;
                core[m].text.index_text++;
              }
            }
            core[m].X = NONE_VALUE;
            core[m].Y = NONE_VALUE;
            if(pos_z >= 0){
              buff = "";
              uint8_t value = int(str.charAt(pos_z)) - 48;
              while(((value >= 0 && value <= 9) || str.charAt(pos_z) == '-') && pos_z < str.length()){
                buff += str.charAt(pos_z);
                pos_z++;
                value = int(str.charAt(pos_z)) - 48;
              }
              if(buff.toInt() > 0){
                core[m].draw_en = false;
                core[m].X = core[m].Y = NONE_VALUE;
              }
              else{
                core[m].draw_en = true;
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
              if(core[m].old_X == NONE_VALUE || core[m].draw_en == false)
                core[m].old_X = buff.toFloat();
              else
                core[m].X = buff.toFloat();
            }
            if(pos_y >= 0){
              buff = "";
              uint8_t value = int(str.charAt(pos_y)) - 48;
              while(((value >= 0 && value <= 9) || str.charAt(pos_y) == '-') && pos_y < str.length()){
                buff += str.charAt(pos_y);
                pos_y++;
                value = int(str.charAt(pos_y)) - 48;
              }
              if(core[m].old_Y == NONE_VALUE || core[m].draw_en == false)
                core[m].old_Y = buff.toFloat();
              else
                core[m].Y = buff.toFloat();
            }
            if(core[m].old_X != NONE_VALUE && core[m].old_Y != NONE_VALUE){
              if(core[m].X == NONE_VALUE && core[m].Y != NONE_VALUE){
                core[m].X = core[m].old_X;
              }
              else if(core[m].X != NONE_VALUE && core[m].Y == NONE_VALUE){
                core[m].Y = core[m].old_Y;
              }
            }
            if(core[m].destination == emGetFont){
              if(core[m].old_X > core[m].max_x && core[m].old_X != NONE_VALUE){
                core[m].max_x = core[m].old_X;
              }
              if(core[m].old_X < core[m].min_x && core[m].old_X != NONE_VALUE){
                core[m].min_x = core[m].old_X;
              }
              if(core[m].X > core[m].max_x && core[m].X != NONE_VALUE){
                core[m].max_x = core[m].X;
              }
              if(core[m].X < core[m].min_x && core[m].X != NONE_VALUE){
                core[m].min_x = core[m].X;
              }
              break;
            }
            if(core[m].old_X == core[m].X && core[m].old_Y == core[m].Y){
              break;
            }
            if(core[m].old_X != NONE_VALUE && core[m].old_Y != NONE_VALUE && core[m].X != NONE_VALUE && core[m].Y != NONE_VALUE){
              core[m].x1 = (core[m].old_X+core[m].offset_x)*core[m].scale+core[m].base_x;
              core[m].y1 = height_y - (height_y - (core[m].offset_y+core[m].max_y)) - (core[m].old_Y+core[m].offset_y)*core[m].scale + core[m].base_y;
              core[m].x2 = (core[m].X+core[m].offset_x)*core[m].scale+core[m].base_x;
              core[m].y2 = height_y - (height_y - (core[m].offset_y+core[m].max_y)) - (core[m].Y+core[m].offset_y)*core[m].scale + core[m].base_y;
              core[m].d = sqrt(pow(core[m].x2-core[m].x1,2)+pow(core[m].y2-core[m].y1,2));
              core[m].old_x = -1, core[m].old_y = -1;
              core[m].i = 0.001;
              core[m].state = emRunning;
              break;
            }
            else{
              break;
            }
          }
        }
        if(!core[m].file.available()){
          if(core[m].destination != emText || core[m].close_file == true){
            core[m].file.close();
            //Serial.print("Close file " + String(core[m].path));
          }
          core[m].state = emIdle;
          core[m].running = false;
          //Serial.println(" -> Done core " + String(m));
          break;
        }
      }
      else if(core[m].state == emRunning){
        uint16_t x = core[m].x1+(core[m].x2-core[m].x1)*core[m].i;
        uint16_t y = core[m].y1+(core[m].y2-core[m].y1)*core[m].i;
        if(core[m].old_x != x || core[m].old_y != y){
          if(core[m].destination == emScreen || core[m].destination == emText){
            if(core[m].restoreImage == true && core[m].destination == emText){
              uint16_t y_mem = y - 30;
              uint16_t x_mem = x;
              if(core[m].base_x > 0){
                x_mem = x - 95;
              }
              if(y_mem < 170){
                tft.drawPixel(x,y,memory[y_mem/17][x_mem+y_mem*290-y_mem/17*17*290]);
              }
              else if(y_mem < 290){
                tft.drawPixel(x,y,memory_buffer[x_mem+y_mem*290-49300]);
              }
            }
            else{
              tft.drawPixel(x,y,core[m].line_color);
              core[m].time_wait = micros() + core[m].delay;
            }
            core[m].old_x = x;
            core[m].old_y = y;
          }
          if(core[m].destination == emMemory){
            memory_buffer[core[m].memory.start_memory+y*core[m].memory.width_memory+x] = TFT_WHITE;
          }
        }
        
        core[m].i+= 0.001+3.2/(core[m].d*10);
        if(core[m].i >= 1){
          core[m].old_X = core[m].X;
          core[m].old_Y = core[m].Y;
          core[m].state = emReadGcode;
        }
      }
    }
  }
}
void IRQ_handler(){
  is_touching = 1;
  detachInterrupt(digitalPinToInterrupt(IRQ_PIN));
}
void display_page(){
  if(page == emHome){
    tft.fillScreen(BG_COLOR);
    if(clock_type == emAnalog){
      display_date();
      drawImage(SD,"/clockFace_1.txt", 95, 30, 290, 1, CLOCKFACE_INDEX, true);
      drawClockFace = false;
      data_clockReady = false;
      draw_step = 0;
    }
    else{
      is_readNumberFont = true;
      first_readFont = true;
      read_newFontState = emComplete;
      if(data_clockReady == false){
        readed_fontNumberStep = 0;
        numberFontImage = 0;
      }
      else{
        readed_fontNumberStep = 11;
        display_date();
      }
      draw_step = 5;
    }
    timeRTC.time_read = millis() + 100000;
  }
  else if(page == emSetting){
    tft.fillScreen(BG_COLOR);
    tft.setFreeFont(&FreeSansBold12pt7b);
    tft.setTextColor(TFT_BLUE);
    tft.drawRoundRect(0,0,480,120,10,TFT_BLUE);
    tft.drawCentreString("Time/date setting",240,5,4);
    char chr[15];
    String str = add_zero(timeRTC.hour) + ":" + add_zero(timeRTC.minute) + ":" + add_zero(timeRTC.second);
    str.toCharArray(chr,str.length()+1);
    key[0].initButton(&tft, 240, 55, 474, 40, TFT_GREEN, TFT_BLACK, TFT_GREEN, chr, 1);
    str = add_zero(timeRTC.month) + "." + add_zero(timeRTC.day) + "." + String(timeRTC.year);
    str.toCharArray(chr,str.length()+1);
    key[1].initButton(&tft, 240, 97, 474, 40, TFT_GREEN, TFT_BLACK, TFT_GREEN, chr, 1);
    key[2].initButton(&tft, 440, 300, 80, 40, TFT_GREEN, TFT_BLACK, TFT_GREEN, "Finish", 1);
    key[3].initButton(&tft, 240, 120+30, 480, 50, TFT_GREEN, TFT_BLACK, TFT_GREEN, "Style clock to Analog", 1);
    key[4].initButton(&tft, 240, 120+80, 480, 50, TFT_GREEN, TFT_BLACK, TFT_GREEN, "Style clock to Digital", 1);
    key[5].initButton(&tft, 240, 120+130, 480, 50, TFT_GREEN, TFT_BLACK, TFT_RED, "Some messages", 1);
    for(uint8_t i = 0; i < BUTTON_ATPAGE[emSetting]; i++){
      key[i].drawButton(false);
    }
  }
}
void update_block(){
  for(uint8_t i = 0; i < 12; i++){
    for(uint8_t j = 0; j < 12; j++){
      if(screen_data[i][j] != screen_data[i+12][j+12]){
        tft.fillRect(move[0].start_x+5+j*15,135+i*15,15,15,move[!screen_data[i][j]].color);
        screen_data[i+12][j+12] = screen_data[i][j];
        break;
      }
    }
  }
}
void move_circle_init(uint32_t delay, uint16_t color_1, uint16_t color_2, uint16_t start){
  move[0].x1 = move[0].x2 = move[0].old_x = random(5+15,5+15*4);
  move[0].y1 = move[0].y2 = move[0].old_y = random(135+15,300);
  move[1].x1 = move[1].x2 = move[1].old_x = random(5+15*7,5+15*10);
  move[1].y1 = move[1].y2 = move[1].old_y = random(135+15,300);
  move[0].color = color_1;
  move[1].color = color_2;
  
  for(uint8_t i = 0; i < 12; i++){
    for(uint8_t j = 6; j < 12; j++){
      screen_data[i][j] = 1;
    }
  }
  
  for(uint8_t i = 0; i < 2; i++){
    move[i].start_x = start;
    move[i].delay = delay;
    move[i].running = true;
    move[i].time_run = millis();
    move[i].angle = 225;
    float theta = PI*move[i].angle/180;
    move[i].new_x = move[i].x1 + cos(theta)*1000;
    move[i].new_y = move[i].y1 + sin(theta)*1000;
    move[i].i = 0.001;
  }
  move[0].step = 0;
}
void move_circle_handle(){
  if(move[0].step < 5){
    if(move[0].running == true && millis() >= move[0].time_run){
      tft.drawRect(move[0].start_x+move[0].step,130+move[0].step,190-move[0].step*2,190-move[0].step*2,TFT_RED);
      move[0].step++;
      move[0].time_run = millis() + move[0].delay*10;
    }
  }
  else if(move[0].step < 149){
    if(move[0].running == true && millis() >= move[0].time_run){
      uint8_t col = (move[0].step-5)%12;
      uint8_t row = (move[0].step-5)/12;
      tft.fillRect(move[0].start_x+5+col*15,135+row*15,15,15,move[!screen_data[row][col]].color);
      move[0].step++;
      move[0].time_run = millis() + move[0].delay*2;
      if(move[0].step >= 149){
        draw_step = 0;
      }
    }
  }
  else{
    for(uint8_t i = 0; i < 2; i++){
      if(move[i].running == true && millis() >= move[i].time_run){
        while(move[i].x2 == move[i].x1 || move[i].y2 == move[i].y1){
          move[i].x2 = move[i].old_x+(move[i].new_x-move[i].old_x)*move[i].i;
          move[i].y2 = move[i].old_y+(move[i].new_y-move[i].old_y)*move[i].i;
          move[i].i += 0.001;
          if(move[i].y2 < 135 + RADIUS_CIRCLE){
            move[i].y2 = 135 + RADIUS_CIRCLE;
            break;
          }
          else if(move[i].y2 + RADIUS_CIRCLE >= 315){
            move[i].y2 = 314 - RADIUS_CIRCLE;
            break;
          }
          else if(move[i].x2 < 5 + RADIUS_CIRCLE){
            move[i].x2 = 5 + RADIUS_CIRCLE;
            break;
          }
          else if(move[i].x2 + RADIUS_CIRCLE >= 185){
            move[i].x2 = 184 - RADIUS_CIRCLE;
            break;
          }
        }
        tft.fillCircle(move[i].start_x + move[i].x1, move[i].y1, RADIUS_CIRCLE, move[!i].color);
        tft.fillCircle(move[i].start_x + move[i].x2, move[i].y2, RADIUS_CIRCLE, move[i].color);
        
        uint16_t data_bottom = screen_data[(move[i].y2-134+RADIUS_CIRCLE)/BLOCK_SIZE][(move[i].x2-5)/BLOCK_SIZE];
        uint16_t data_top = screen_data[(move[i].y2-135-RADIUS_CIRCLE)/BLOCK_SIZE][(move[i].x2-5)/BLOCK_SIZE];
        uint16_t data_right = screen_data[(move[i].y2-135)/BLOCK_SIZE][(move[i].x2-4+RADIUS_CIRCLE)/BLOCK_SIZE];
        uint16_t data_left = screen_data[(move[i].y2-135)/BLOCK_SIZE][(move[i].x2-5-RADIUS_CIRCLE)/BLOCK_SIZE];
        if(move[i].y2 >= 313 - RADIUS_CIRCLE || data_bottom == !i){
          move[i].old_x = move[i].x2;
          move[i].old_y = move[i].y2;
          if(move[i].new_x > move[i].old_x) move[i].angle = 315;
          else move[i].angle = 225;
          float theta = PI*move[i].angle/180;
          move[i].new_x = move[i].x2 + cos(theta)*1000;
          move[i].new_y = move[i].y2 + sin(theta)*1000;
          move[i].i = 0.001;
          if(data_bottom == !i){
            screen_data[(move[i].y2-134+RADIUS_CIRCLE)/BLOCK_SIZE][(move[i].x2-5)/BLOCK_SIZE] = i;
            uint16_t x = (move[i].x2-5)/BLOCK_SIZE*BLOCK_SIZE+5;
            uint16_t y = (move[i].y2-134+RADIUS_CIRCLE)/BLOCK_SIZE*BLOCK_SIZE+135;
            update_block();
          }
        }
        if(move[i].y2 <= 135 + RADIUS_CIRCLE || data_top == !i){
          move[i].old_x = move[i].x2;
          move[i].old_y = move[i].y2;
          if(move[i].new_x > move[i].old_x) move[i].angle = 45;
          else move[i].angle = 135;
          float theta = PI*move[i].angle/180;
          move[i].new_x = move[i].x2 + cos(theta)*1000;
          move[i].new_y = move[i].y2 + sin(theta)*1000;
          move[i].i = 0.001;
          if(data_top == !i){
            screen_data[(move[i].y2-136-RADIUS_CIRCLE)/BLOCK_SIZE][(move[i].x2-5)/BLOCK_SIZE] = i;
            uint16_t x = (move[i].x2-5)/BLOCK_SIZE*BLOCK_SIZE+5;
            uint16_t y = (move[i].y2-136-RADIUS_CIRCLE)/BLOCK_SIZE*BLOCK_SIZE+135;
            update_block();
          }
        }
        if(move[i].x2 <= RADIUS_CIRCLE + 5 || data_left == !i){
          move[i].old_x = move[i].x2;
          move[i].old_y = move[i].y2;
          if(move[i].new_y < move[i].old_y) move[i].angle = 315;
          else move[i].angle = 45;
          float theta = PI*move[i].angle/180;
          move[i].new_x = move[i].x2 + cos(theta)*1000;
          move[i].new_y = move[i].y2 + sin(theta)*1000;
          move[i].i = 0.001;
          if(data_left == !i){
            screen_data[(move[i].y2-135)/BLOCK_SIZE][(move[i].x2-6-RADIUS_CIRCLE)/BLOCK_SIZE] = i;
            uint16_t x = (move[i].x2-6-RADIUS_CIRCLE)/BLOCK_SIZE*BLOCK_SIZE+5;
            uint16_t y = (move[i].y2-135)/BLOCK_SIZE*BLOCK_SIZE+135;
            update_block();
          }
        }
        if(move[i].x2 >= 183 - RADIUS_CIRCLE || data_right == !i){
          move[i].old_x = move[i].x2;
          move[i].old_y = move[i].y2;
          if(move[i].new_y < move[i].old_y) move[i].angle = 225;
          else move[i].angle = 135;
          float theta = PI*move[i].angle/180;
          move[i].new_x = move[i].x2 + cos(theta)*1000;
          move[i].new_y = move[i].y2 + sin(theta)*1000;
          move[i].i = 0.001;
          if(data_right == !i){
            screen_data[(move[i].y2-135)/BLOCK_SIZE][(move[i].x2-4+RADIUS_CIRCLE)/BLOCK_SIZE] = i;
            uint16_t x = (move[i].x2-4+RADIUS_CIRCLE)/BLOCK_SIZE*BLOCK_SIZE+5;
            uint16_t y = (move[i].y2-135)/BLOCK_SIZE*BLOCK_SIZE+135;
            update_block();
          }
        }
        move[i].x1 = move[i].x2;
        move[i].y1 = move[i].y2;
        move[i].time_run = millis() + move[i].delay;
      }
    }
  }
}
void change_clock(eClockType type){
  if(clock_type == emDigital && type == emAnalog){
    stop_draw(-1);
    for(uint8_t i = 0; i < MAX_MEMORY_DRAWING; i++){
      drawingMemory[i].running = false;
    }
    move[0].running = false;
    move[1].running = false;
    tft.fillRect(0,30,480,290,BG_COLOR);
    clock_type = emAnalog;
    drawImage(SD,"/clockFace_1.txt", 95, 30, 290, 1, CLOCKFACE_INDEX, true);
    drawClockFace = false;
    timeRTC.time_read = millis() + 100000;
    EEPROM.write(0,0);
    EEPROM.commit();
    draw_step = 0;
  }
  else if(clock_type == emAnalog && type == emDigital){
    stop_draw(-1);
    tft.fillRect(0,30,480,290,BG_COLOR);
    clock_type = emDigital;
    readed_fontNumberStep = 0;
    numberFontImage = 0;
    first_readFont = true;
    is_readNumberFont = true;
    timeRTC.time_read = millis() + 100000;
    EEPROM.write(0,1);
    EEPROM.commit();
    draw_step = 5;
  }
}
void button_handler(){
  static bool button_status = 1;
  static uint32_t time_pressed = 0;
  static uint8_t command = 0;
  uint8_t times = 0;
  while(digitalRead(BUTTON_PIN) != button_status && times < 10){
    times++;
    delay(1);
  }
  if(times >= 10){
    time_pressed = millis();
    button_status = !button_status;
    if(button_status == 1){
      if(command == 0){
        tft.fillScreen(TFT_BLACK);
        tft.setFreeFont(&FreeSansBold12pt7b);
        key[0].initButton(&tft, 240, 160, 100, 100, TFT_BLACK, TFT_BLACK, TFT_GREEN, "Bye!", 2);
        key[0].drawButton(false);
        delay(3000);
        esp_deep_sleep_start();
      }
      else{
        if(command == 1){
          if(clock_type == emAnalog){
            change_clock(emDigital);
          }
          else{
            change_clock(emAnalog);
            display_date();
          }
        }
        else{
          display_date();
        }
      }
    }
    else{
      command = 0;
      key[0].initButton(&tft, 240, 15, 480, 30, TFT_BLACK, TFT_BLACK, TFT_RED, "Turn off", 1);
      key[0].drawButton(false);
    }
  }
  if(button_status == 0){
    if(millis() >= time_pressed + 1000 && command == 0){
      key[0].initButton(&tft, 240, 15, 480, 30, TFT_BLACK, TFT_BLACK, TFT_RED, "Change clock style", 1);
      key[0].drawButton(false);
      command++;
    }
    else if(millis() >= time_pressed + 3000 && command == 1){
      key[0].initButton(&tft, 240, 15, 480, 30, TFT_BLACK, TFT_BLACK, TFT_RED, "Cancel", 1);
      key[0].drawButton(false);
      command++;
    }
  }
}
void drawSdJpeg(const char *filename, int xpos, int ypos) {
  File jpegFile = SD.open( filename, FILE_READ);  // or, file handle reference for SD library
  if ( !jpegFile ) {
    Serial.print("ERROR: File \""); Serial.print(filename); Serial.println ("\" not found!");
    return;
  }

  bool decoded = JpegDec.decodeSdFile(jpegFile);  // Pass the SD file handle to the decoder,
  if (decoded) {
    uint16_t *pImg;
    uint16_t mcu_w = JpegDec.MCUWidth;
    uint16_t mcu_h = JpegDec.MCUHeight;
    uint32_t max_x = JpegDec.width;
    uint32_t max_y = JpegDec.height;
  
    bool swapBytes = tft.getSwapBytes();
    tft.setSwapBytes(true);
    uint32_t min_w = jpg_min(mcu_w, max_x % mcu_w);
    uint32_t min_h = jpg_min(mcu_h, max_y % mcu_h);
  
    uint32_t win_w = mcu_w;
    uint32_t win_h = mcu_h;
  
    max_x += xpos;
    max_y += ypos;
  
    while (JpegDec.read()) {    // While there is more data in the file
      pImg = JpegDec.pImage ;   // Decode a MCU (Minimum Coding Unit, typically a 8x8 or 16x16 pixel block)
      int mcu_x = JpegDec.MCUx * mcu_w + xpos;
      int mcu_y = JpegDec.MCUy * mcu_h + ypos;
      if (mcu_x + mcu_w <= max_x) win_w = mcu_w;
      else win_w = min_w;
 
      if (mcu_y + mcu_h <= max_y) win_h = mcu_h;
      else win_h = min_h;
  
      if (win_w != mcu_w)
      {
        uint16_t *cImg;
        int p = 0;
        cImg = pImg + win_w;
        for (int h = 1; h < win_h; h++)
        {
          p += mcu_w;
          for (int w = 0; w < win_w; w++)
          {
            *cImg = *(pImg + w + p);
            cImg++;
          }
        }
      }
  
      uint32_t mcu_pixels = win_w * win_h;
  
      if (( mcu_x + win_w ) <= tft.width() && ( mcu_y + win_h ) <= tft.height())
        tft.pushImage(mcu_x, mcu_y, win_w, win_h, pImg);
      else if ( (mcu_y + win_h) >= tft.height())
        JpegDec.abort(); // Image has run off bottom of screen so abort decoding
    }
  
    tft.setSwapBytes(swapBytes);
  }
  else {
    Serial.println("Jpeg file format not supported!");
  }
}
void setup(){
  Serial.begin(115200);
  EEPROM.begin(10);
  pinMode(LED_LCD_PIN,OUTPUT);
  pinMode(POWER_EN_PIN,OUTPUT);
  pinMode(BUTTON_PIN,INPUT_PULLUP);
  pinMode(IRQ_PIN,INPUT);
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
//  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  tft.init();
  tft.setRotation(3);
  check_touch();
  tft.fillScreen(TFT_BLACK);
  for(uint8_t i = 0; i < 10; i++){
    memory[i] = new uint16_t [5000];
    for(uint32_t j = 0; j < 5000; j++){
      memory[i][j] = 0;
    }
  }
  for(uint32_t i = 0; i < 50005; i++){
    memory_buffer[i] = 0;
  }
  for(uint8_t i = 0; i < 24; i++){
    for(uint32_t j = 0; j < 24; j++){
      screen_data[i][j] = 0;
    }
  }
  DateTime now = rtc.now();
  timeRTC.day = now.day();
  timeRTC.date = now.dayOfTheWeek();
  timeRTC.month = now.month();
  timeRTC.year = now.year();
  timeRTC.time_read = millis() + 100000;
  
  for(uint8_t i = 0; i < MAX_RUNGCODE; i++){
    core[i].state = emIdle;
  }
  readTextFont(SD,"/abc.tap",3,&fontGcode,false);
  readTextFont(SD,"/secClock_1.tap",2,&clockSecGcode,true);
  readTextFont(SD,"/hourClock_1.tap",0,&clockHourGcode,true);
  readTextFont(SD,"/minClock_1.tap",1,&clockMinGcode,true);
  
  data_clockReady = false;
  if(EEPROM.read(0) != 0 && EEPROM.read(0) != 1){
    EEPROM.write(0,0);
    EEPROM.commit();
  }
  clock_type = (eClockType)EEPROM.read(0);
  page = emHome;
  display_page();
}
void loop(){
  if(Serial.available()){
    delay(5);
    String str = "";
    while(Serial.available()){
      str += (char)Serial.read();
    }
    if(str.indexOf("analog") == 0){
      change_clock(emAnalog);
    }
    else if(str.indexOf("digital") == 0){
      change_clock(emDigital);
    }
  }
  if(is_touching){
    bool state, oldstate;
    uint8_t count = 0;
    while (count < 5) {
      state = tft.getTouch(&touch_x, &touch_y);
      if (state == oldstate) count++;
      else count = 0;
      oldstate = state;
      delay(1);
    }

    tft.setFreeFont(&FreeSansBold12pt7b);
    for (uint8_t b = 0; b < BUTTON_ATPAGE[page]; b++) {
      if (oldstate && key[b].contains(touch_x, touch_y))
        key[b].press(true);
      else
        key[b].press(false);
    }
    for (uint8_t b = 0; b < BUTTON_ATPAGE[page]; b++) {
      if (key[b].justPressed()) {
        key[b].drawButton(true);
      }
      if (key[b].justReleased()) {
        key[b].drawButton(false);
      }
    }
    if(oldstate){
      is_touchHandled = true;
    }
    else{
      if(is_touchHandled){
        int8_t key_pressed = -1;
        for (uint8_t b = 0; b < BUTTON_ATPAGE[page]; b++) {
          if (key[b].contains(touch_x, touch_y)){
            key_pressed = b;
            break;
          }
        }
        if(key_pressed >= 0){
          if(page == emHome){
            if(key_pressed == 0){ // setting
              stop_draw(-1);
              for(uint8_t i = 0; i < MAX_MEMORY_DRAWING; i++){
                drawingMemory[i].running = false;
              }
              move[0].running = false;
              move[1].running = false;
              page = emSetting;
              display_page();
            }
          }
          else if(page == emSetting){
            if(key_pressed < 2){
              String str = "::exit";
              if(key_pressed == 0){ // setting time
                str = keyInput("hh mm ss");
              }
              else if(key_pressed == 1){ // setting date
                str = keyInput("mm dd yyyy");
              }
              if(str != "::exit"){
                uint16_t value[3];
                String buff = "";
                for(uint8_t i = 0, j = 0; j < 3 && i < str.length(); i++){
                  if(str.charAt(i) == ' ' || (i + 1) >= str.length()){
                    if(str.charAt(i) != ' ')
                      buff += str.charAt(i);
                    value[j] = buff.toInt();
                    if(++j >= 3){
                      break;
                    }
                    buff = "";
                  }
                  else{
                    buff += str.charAt(i);
                  }
                }
                if(key_pressed == 0){
                  timeRTC.hour = value[0];
                  timeRTC.minute = value[1];
                  timeRTC.second = value[2];
                }
                else if(key_pressed == 1){
                  timeRTC.month = value[0];
                  timeRTC.day = value[1];
                  timeRTC.year = value[2];
                }
                rtc.adjust(DateTime(timeRTC.year, timeRTC.month, timeRTC.day, timeRTC.hour, timeRTC.minute, timeRTC.second));
              }
              display_page();
            }
            else if(key_pressed == 2){
              page = emHome;
              display_page();
            }
            else if(key_pressed == 3){
              page = emHome;
              change_clock(emAnalog);
              display_page();
            }
            else if(key_pressed == 4){
              page = emHome;
              change_clock(emDigital);
              display_page();
            }
            else if(key_pressed == 5){
              keyInput("","Hello friends! I am Duyen :)",50);
              tft.fillScreen(BG_COLOR);
              keyInput("","Thank you for watching!",200);
              tft.fillScreen(BG_COLOR);
              keyInput("","Subscribe for more interesting video!",50);
              display_page();
            }
          }
        }
      }
      is_touching = false;
      is_touchHandled = false;
      attachInterrupt(digitalPinToInterrupt(IRQ_PIN), IRQ_handler, CHANGE);
    }
  }
  else if(page == emHome){
    if(millis() >= timeRTC.time_read){
      DateTime now = rtc.now();
      timeRTC.time_read = millis() + 1000;
      if(clock_type == emDigital){
        if(timeRTC.second%10 != now.second()%10 || timeRTC.printed_time == false){
          drawMemory((now.second()%10)*NUMBER_FONT_SIZE, (WIDTH_NUMBER+10)*7+5, 30, WIDTH_NUMBER, HEIGHT_NUMBER, 500, 0);
          if(timeRTC.second/10 != now.second()/10 || timeRTC.printed_time == false){
            drawMemory((now.second()/10)*NUMBER_FONT_SIZE, (WIDTH_NUMBER+10)*6+5, 30, WIDTH_NUMBER, HEIGHT_NUMBER, 500, 1);
          }
        }
        if(timeRTC.minute%10 != now.minute()%10 || timeRTC.printed_time == false){
          drawMemory((now.minute()%10)*NUMBER_FONT_SIZE, (WIDTH_NUMBER+10)*4+5, 30, WIDTH_NUMBER, HEIGHT_NUMBER, 500, 2);
          if(timeRTC.minute/10 != now.minute()/10 || timeRTC.printed_time == false){
            drawMemory((now.minute()/10)*NUMBER_FONT_SIZE, (WIDTH_NUMBER+10)*3+5, 30, WIDTH_NUMBER, HEIGHT_NUMBER, 500, 3);
          }
        }
        if(timeRTC.hour%10 != now.hour()%10 || timeRTC.printed_time == false){
          drawMemory((now.hour()%10)*NUMBER_FONT_SIZE, (WIDTH_NUMBER+10)*1+5, 30, WIDTH_NUMBER, HEIGHT_NUMBER, 500, 4);
          if(timeRTC.hour/10 != now.hour()/10 || timeRTC.printed_time == false){
            drawMemory((now.hour()/10)*NUMBER_FONT_SIZE, WIDTH_NUMBER*0+5, 30, WIDTH_NUMBER, HEIGHT_NUMBER, 500, 5);
          }
        }
      }
      else{
        if(timeRTC.second != now.second() || timeRTC.printed_time == false){
          if(now.second() == 0){
            restoreDisplay(SD,"/secClock_1.tap","k",-3+95,30,2,&clockSecGcode,false,false);
          }
          else if(now.second() < 60){
            char old_clock = timeRTC.second+48;
            restoreDisplay(SD,"/secClock_1.tap",String(old_clock),-3+95,30,2,&clockSecGcode,false,false);
          }
        }
        if(timeRTC.minute != now.minute() || timeRTC.printed_time == false){
          if(now.minute() == 0){
            restoreDisplay(SD,"/minClock_1.tap","k",-3+95,30,1,&clockMinGcode,false,false);
          }
          else if(now.minute() < 60){
            char old_clock = timeRTC.minute+48;
            restoreDisplay(SD,"/minClock_1.tap",String(old_clock),-3+95,30,1,&clockMinGcode,false,false);
          }
        }
        if(timeRTC.hour != now.hour() || timeRTC.printed_time == false){
          if(now.hour() == 0){
            restoreDisplay(SD,"/hourClock_1.tap",";",-3+95,320,0,&clockHourGcode,false,false);
          }
          else if(now.hour()%12 < 12){
            char old_clock = timeRTC.hour%12+48;
            restoreDisplay(SD,"/hourClock_1.tap",String(old_clock),-3+95,320,0,&clockHourGcode,false,false);
          }
        }
        clock_state[2] = emClearing;
        clock_state[1] = emClearing;
        clock_state[0] = emClearing;
      }
      if(timeRTC.day != now.day()){
        tft.fillRect(0,0,480,30,BG_COLOR);
        timeRTC.day = now.day();
        timeRTC.date = now.dayOfTheWeek();
        timeRTC.month = now.month();
        timeRTC.year = now.year();
        display_date();
      }
      timeRTC.hour = now.hour();
      timeRTC.minute = now.minute();
      timeRTC.second = now.second();
      timeRTC.printed_time = true;
    }
    if(clock_type == emAnalog){
      if(drawClockFace == false && core[CLOCKFACE_INDEX].state == emIdle){
        drawClockFace = true;
        timeRTC.time_read = millis();
      }
      if(drawClockFace == true && timeRTC.printed_time == true){
        if(core[2].state == emIdle && clock_state[2] == emClearing){
          char new_clock = timeRTC.second+48;
          printText(SD,"/secClock_1.tap",String(new_clock),-3+95,30,2,1,TFT_RED,&clockSecGcode,false,false);   
          clock_state[2] = emDrawing;
        }
        else if(core[1].state == emIdle && core[2].state == emIdle && clock_state[1] == emClearing){
          char new_clock = timeRTC.minute+48;
          printText(SD,"/minClock_1.tap",String(new_clock),-3+95,30,1,1,TFT_GREEN,&clockMinGcode,false,false);
          clock_state[1] = emDrawing;
        }
        else if(core[0].state == emIdle && core[1].state == emIdle && core[2].state == emIdle && clock_state[0] == emClearing){
          char new_clock = timeRTC.hour%12+48;
          printText(SD,"/hourClock_1.tap",String(new_clock),-3+95,320,0,1,TFT_YELLOW,&clockHourGcode,false,false);
          clock_state[0] = emDrawing;
        }
    
        for(uint8_t i = 0; i < 3; i++){
          if(core[i].state == emIdle && clock_state[i] == emDrawing){
            clock_state[i] = emDone;
          }
        }
      }
      if(drawClockFace == true && core[3].state == emIdle){
        if(draw_step == 0){
          drawImage(SD,"/coolGirl_1.txt", 0, 70, 95, 5000, 3, false);
          draw_step++;
        }
        else if(draw_step == 1){
          drawImage(SD,"/coolBoy_1.txt", 385, 70, 95, 5000, 3, false);
          draw_step++;
        }
      }
    }
    else if(clock_type == emDigital){
      if(core[READFONT_INDEX].state == emIdle && is_readNumberFont == true){
        if(readed_fontNumberStep < 14){
          if(readed_fontNumberStep == 0){
            for(uint32_t i = 0; i < 50005; i++){
              memory_buffer[i] = 0;
            }
            drawSdJpeg("/waiting.jpg",96,0);
          }
          if(readed_fontNumberStep < 10){
            readNumberFontFromImage(SD,"/"+String(readed_fontNumberStep)+"_" + String(numberFontImage) + ".txt",1,readed_fontNumberStep*NUMBER_FONT_SIZE);
          }
          else if(readed_fontNumberStep == 11){
            tft.fillRect(0,0,480,320,BG_COLOR);
            drawImage(SD,"/cham_"+String(numberFontImage)+".txt",15+60*2,65,23,3000,READFONT_INDEX);
            if(core[2].state == emIdle){
              drawImage(SD,"/cham_"+String(numberFontImage)+".txt",15+60*5,65,23,3000,2);
              read_newFontState = emReadComplete;
              readed_fontNumberStep++;
            }
          }
          else if(readed_fontNumberStep == 12){
            tft.fillRect(310,60,30,70,BG_COLOR);
            drawImage(SD,"/cham_"+String(numberFontImage)+".txt",15+60*5,65,23,3000,READFONT_INDEX);
            read_newFontState = emReadComplete;
          }
          else if(readed_fontNumberStep == 13){
            if(first_readFont == true){
              for(uint16_t i = 0; i < 10; i++){
                for(uint16_t j = 0; j < 5000; j++){
                  memory[i][j] = memory_buffer[i*5000+j];
                }
              }
              timeRTC.time_read = millis();
              first_readFont = false;
              timeRTC.printed_time = false;
              is_readNumberFont = false;
              read_newFontState = emComplete;
              display_date();
              move_circle_init(15,TFT_GREEN,TFT_BLUE,145);
            }
            else{
              read_newFontState = emChangeFontRequest;
            }
            data_clockReady = true;
          }
          readed_fontNumberStep++;
        }
      }
      drawMemory();
      if(core[3].state == emIdle){
        if(draw_step == 0){
          draw(SD, "/elecchannel_140x190.tap", 0, 130, 5000, TFT_GREEN, 3);
          draw_step++;
        }
        else if(draw_step == 1){
          draw(SD, "/doremon_122x190.tap", 11+335, 130, 5000, TFT_BLUE, 3);
          draw_step++;
        }
      }
    }
    draw(SD);
    if(core[3].state == emIdle && readed_font == false){
      readed_font = true;
    }
    move_circle_handle();
    button_handler();
  }
}
