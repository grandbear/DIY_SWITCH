#include <TFT_eSPI.h>
#include <SPI.h>
#include "switch.h"
#include "fonts.h"
#include <RTClib.h>
#include <ESP32Time.h>
#include <ArduinoJson.h>
#include "get_information.h"
#include "zh_front_20.h"
#include "Picture.h"

uint8_t brightrn_screen_flag=1,turn_off_screen_flag=0;

int angle = 0;
int rAngle = 359;
double rad = 0.01745;    //  Π/180°，将度数转换为弧度
float x[360],y[360];
float px[360],py[360];
float lx[360],ly[360];
int sx = 115,sy=120,r=99;
int start[12],startP[60];
String cc[12] = {"45", "40", "35", "30", "25", "20", "15", "10", "05", "0", "55", "50"};
String weeks[7] = {"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"};
int hh=0,mm=0,ss=0;
int month2=0,day2=0;
int d1=0,d2=2,m1=0,m2=0;


int frameIndex = 0;  //动画帧的索引
//定义动画显示所需要的帧延时变量
unsigned long previousTime = 0;  // 上一次执行任务的时间
unsigned long delayTime = 100;  // 延时时间（毫秒）
unsigned long currentTime = millis();  // 获取当前时间


TFT_eSPI tft;
TFT_eSprite img = TFT_eSprite(&tft);
ESP32Time rtc(0);   //时间补偿
uint8_t last_time=0,times_flag=0;

void updateWatch(void) 
{
    
    hh=rtc.getTime("%H").toInt();
    mm=rtc.getTime("%M").toInt();
    ss=rtc.getTime("%S").toInt();

    if(times_flag==0)
    {
      last_time=ss;
      times_flag=1;
    }

    if(abs(ss-last_time)>=5&&turn_off_screen_flag==0)
    {
      turn_off_screen_flag=1;
      sigmaDeltaWrite(5, 10);
    }
    else if(abs(ss-last_time)>=10&&turn_off_screen_flag==1)
    {
      tft.fillScreen(TFT_BLACK);
      sigmaDeltaWrite(5, 0);
      brightrn_screen_flag=0;
      turn_off_screen_flag=2;
    }

    month2=month;
    day2=day;

    img.setTextFont(1);
    img.setTextSize(1);
    img.setTextColor(TFT_DARKGREY, TFT_BLACK);
  
    img.fillSprite(TFT_BLACK);       //清除上一帧的数据
    img.pushImage(91,138,50,47,frame[frameIndex]);   //清屏后保留动画帧
    angle = rtc.getSecond() * 6;    //每秒转动的角度
    rAngle = rAngle - 2;           //小球移动的速度

    if (angle >= 360)   angle = 0;
    if (rAngle <= 0)    rAngle = 359;

    //表盘部分
    for (int i = 0; i < 12; i++)   //显示刻度以及转动表盘
    {
      if (start[i] + angle < 360) 
      {
        img.drawString(cc[i], x[start[i] + angle], y[start[i] + angle],2);  
        img.drawLine(px[start[i] + angle], py[start[i] + angle], lx[start[i] + angle], ly[start[i] + angle], TFT_DARKGREY);
      }
      else
      {
        img.drawString(cc[i], x[(start[i] + angle) - 360], y[(start[i] + angle) - 360],2);
        img.drawLine(px[(start[i] + angle) - 360], py[(start[i] + angle) - 360], lx[(start[i] + angle) - 360], ly[(start[i] + angle) - 360], TFT_DARKGREY);
      }
    }    

    for (int i = 0; i < 60; i++)
    {
    if (startP[i] + angle < 360)
        img.fillCircle(px[startP[i] + angle], py[startP[i] + angle], 1, TFT_DARKGREY);
    else
        img.fillCircle(px[(startP[i] + angle) - 360], py[(startP[i] + angle) - 360], 1, TFT_DARKGREY);
    }
    img.fillTriangle(sx - 1, sy - 68, sx - 3, sy - 58, sx + 2, sy - 58, TFT_BROWN);
    img.fillCircle(px[rAngle], py[rAngle], 6, TFT_RED);

    //时间部分
    if(hh>=10&&hh<20) img.setCursor(50,136);     //调整时间位置
    else  img.setCursor(58, 136);
    
    img.setFreeFont(&DSEG7_Classic_Regular_28);
    img.printf("%02d",hh);

    if(ss%2==0)
    {
      img.setTextColor(TFT_DARKGREY);
      if(mm>=10&&mm<20)
      {
        img.setCursor(img.getCursorX()+8, img.getCursorY());
        img.printf(" : ");
      } 
      else  img.printf(" : ");
        
    }
    else
    {
      img.setTextColor(TFT_BLACK);
      if(mm>=10&&mm<20)
      {
        img.setCursor(img.getCursorX()+8, img.getCursorY());
        img.printf("   ");
      } 
      else  img.printf("   ");
    }
    
    img.setTextColor(TFT_DARKGREY);
    img.printf("%02d",mm);

    //日期部分
    img.setTextFont(0);
    img.setTextColor(TFT_DARKGREY, TFT_BLACK);
    img.drawString("MON", 79, 61);
    img.drawString("DAY", 148, 61);

    img.fillRect(66, 75, 12, 20, 0x5ACB);
    img.fillRect(80, 75, 12, 20, 0x5ACB);
    img.fillRect(135, 75, 12, 20, 0x5ACB);
    img.fillRect(149, 75, 12, 20, 0x5ACB);

    if (month2 >= 10)
    {
      m1 = month2 / 10;
      m2 = month2 % 10;

    }
    else
    {
      m1 = 0;
      m2 = month2;
    }
    if (day2 >= 10)
    {
      d1 = day2 / 10;
      d2 = day2 % 10;
    }
    else
    {
      d1 = 0;
      d2 = day2;
    }

    
    img.setTextColor(TFT_WHITE, 0x5ACB);
    img.drawString(String(m1), 73, 85, 2);
    img.drawString(String(m2), 87, 85, 2);

    img.drawString(String(d1), 142, 85, 2);
    img.drawString(String(d2), 156, 85, 2);

    img.setTextColor(0xB47D,TFT_BLACK);
    img.loadFont(font_20);
    if(weather_condition.length()==3)  img.drawString(weather_condition.substring(0,3), 113,85);
    else if(weather_condition.length()==6)  img.drawString(weather_condition.substring(0,6), 113,85);
    img.unloadFont();

    img.setTextFont(1);
    img.setTextColor(0x3C96, TFT_BLACK);
    img.drawString(weeks[getDayOfWeek(year,month,day)], 113, 68);



    //电量部分
    if(ss==0)   get_bat_votage();         //减少adc采值次数
    

    img.drawRect(148, 154, 20, 12, TFT_DARKGREY);
    img.drawFastVLine(153, 154, 12, TFT_DARKGREY);
    img.drawFastVLine(158, 154, 12, TFT_DARKGREY);
    img.drawFastVLine(163, 154, 12, TFT_DARKGREY);

    img.fillRect(149, 155, 3, 10 , TFT_BLACK);
    img.fillRect(154, 155, 3, 10, TFT_BLACK);
    img.fillRect(159, 155, 3, 10, TFT_BLACK);
    img.fillRect(164, 155, 3, 10, TFT_BLACK);

    if(bat_flag==1)
    {
      img.fillRect(149, 155, 3, 10 , TFT_RED);
    }
    else if(bat_flag==2)
    {
      img.fillRect(149, 155, 3, 10 , TFT_GREEN);
    }
    else if(bat_flag==3)
    {
      img.fillRect(149, 155, 3, 10, TFT_GREEN);
      img.fillRect(154, 155, 3, 10, TFT_GREEN);
    }
    else if(bat_flag==4)
    {
      img.fillRect(149, 155, 3, 10, TFT_GREEN);
      img.fillRect(154, 155, 3, 10, TFT_GREEN);
      img.fillRect(159, 155, 3, 10, TFT_GREEN);
    }
    else if(bat_flag==5)
    {
      img.fillRect(149, 155, 3, 10, TFT_GREEN);
      img.fillRect(154, 155, 3, 10, TFT_GREEN);
      img.fillRect(159, 155, 3, 10, TFT_GREEN);
      img.fillRect(164, 155, 3, 10, TFT_GREEN);
    }

    img.drawString(String(electric_quantities)+"%", 73,160, 1);       //显示电量
    img.drawString(String(bat_voltage),73,175,1);


    
    


    // img.drawTriangle(76,150,76,158,70,158,TFT_DARKGREY);       //充电闪电标志
    // img.drawTriangle(74,158,81,158,74,166,TFT_DARKGREY);

    // if(bat_flag==1)
    // {
    //   img.fillTriangle(76,150,76,158,70,158,TFT_LIGHTGREY);
    //   img.fillTriangle(74,158,81,158,74,166,TFT_LIGHTGREY);
    // }
    // else 
    // {
    //   img.fillTriangle(76,150,76,158,70,158,TFT_DARKGREY);
    //   img.fillTriangle(74,158,81,158,74,166,TFT_DARKGREY);
    // }


    

    
  
    //动画显示部分
    currentTime = millis();
    if (currentTime - previousTime >= delayTime) 
    {
      img.pushImage(91,138,50,47,frame[frameIndex]);
      // img.drawRect(91, 138, 50, 47, TFT_WHITE);
      frameIndex++;
      //播放到最后一帧之后，又回到第一帧开始播放
      if (frameIndex >= 8) 
      {
        frameIndex = 0;
      }
      previousTime = currentTime;  // 更新上一次执行任务的时间
    }
        
     
            



    img.pushSprite(5, 5);
    
}
