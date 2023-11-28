
#include <RTClib.h>
#include <ESP32Time.h>
#ifndef _switch_H
#define _switch_H





void updateWatch(void);




extern TFT_eSPI tft;
extern TFT_eSprite img;
extern ESP32Time rtc;

extern double rad;
extern float x[360],y[360];
extern float px[360],py[360];
extern float lx[360],ly[360];
extern int sx,sy,r,angle;
extern int start[12],startP[60];
extern uint8_t brightrn_screen_flag,turn_off_screen_flag;
extern uint8_t last_time,times_flag;







#endif