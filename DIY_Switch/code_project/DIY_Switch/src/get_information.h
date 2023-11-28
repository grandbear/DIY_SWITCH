#ifndef _get_information_H
#define _get_information_H


void attain_time(String url);
void attain_weather(String url);
int getDayOfWeek(int year, int month, int day);
void draw_heart_shape(void);
void get_bat_votage(void);
void adc_cal_init(void);


extern int hour,minute,second;
extern int year,month,day;
extern String city,weather_condition,winddirection;
extern int tem;
extern int heart_x[629],heart_y[629];
extern float radius,heartsize ;
extern int heart_change_flag,fill_screen_flag;
extern uint16_t buffer1[240][240],buffer2[240][240];
extern int ADC_BAT_PIN;
extern float bat_voltage;
extern uint8_t bat_flag;
extern uint8_t electric_quantities;


#endif