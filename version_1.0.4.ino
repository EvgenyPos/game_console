#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <iarduino_RTC.h>
#include <Preferences.h>
#include <GyverBME280.h>
#include <PT2257.h>
#include "pictures.h"

Adafruit_ST7789 tft = Adafruit_ST7789(15, 16, 17);//CS, DC, RST

iarduino_RTC RTCtime(RTC_DS3231);

GyverBME280 bme;

PT2257 rt;

Preferences pref;

#define seaLevelPressure_hPa 1013.25
#define SPEED 2
#define SPEEDBOT 2
#define SPEEDBAM 6
#define KD 700
#define SLEEP 1
#define SLEEPSB 15

#define VERSION "firmware version: 1.0.4"

#define PEZOPIN 33

#define U_BUTT 4 //12
#define D_BUTT 2 //13
#define L_BUTT 13 //4
#define R_BUTT 12 //2
#define OK_BUTT 14
#define HOME_BUTT 27
//#define CENTR_BUTT 25
//пины которые можно свободно использовать: 25, 26. Насчёт остальных нужно смотреть документацию

int8_t forMapBS[10][12];



//карты для игры Battle city. Можно редактировать
//-1 - точка появления своего танка
//-2 - -4 точки появления 3 ботов
// 0 - пустота
// 1 - неразрушимая стена
// 2 - кирпичная стена
// 3 - вода
// 2 - 6 - степени разрушенности стен

int8_t  map1BS [] PROGMEM = {
  -1, 0, 0, 1, 2, 2, 2, 2, 1, 0, 0, -2, 0, 0, 0, 2,
  3, 3, 3, 3, 2, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3,
  0, 0, 0, 0, 1, 2, 0, 0, 0, 1, 1, 0, 0, 0, 2, 1,
  2, 3, 0, 0, 0, 2, 2, 0, 0, 0, 3, 2, 2, 3, 0, 0,
  0, 2, 2, 0, 0, 0, 3, 2, 1, 2, 0, 0, 0, 1, 1, 0,
  0, 0, 2, 1, 0, 0, 0, 0, 3, 3, 3, 3, 0, 0, 0, 0,
  0, 0, 0, 2, 3, 3, 3, 3, 2, 0, 0, 0, -3, 0, 0, 1,
  2, 2, 2, 2, 1, 0, 0, -4
};

int8_t  map2BS [] PROGMEM = {
  -1, 0, 2, 2, 0, 0, 0, 0, 2, 2, 0, -2, 0, 0, 2, 0,
  0, 0, 0, 0, 0, 2, 0, 0, 1, 0, 0, 0, 3, 3, 3, 3,
  0, 0, 0, 1, 1, 0, 0, 0, 3, 3, 3, 3, 0, 0, 0, 1,
  1, 2, 0, 0, 3, 2, 2, 3, 0, 0, 2, 1, 1, 2, 0, 0,
  3, 2, 2, 3, 0, 0, 2, 1, 1, 0, 0, 0, 3, 3, 3, 3,
  0, 0, 0, 1, 1, 0, 0, 0, 3, 3, 3, 3, 0, 0, 0, 1,
  0, 0, 2, 0, 0, 0, 0, 0, 0, 2, 0, 0, -3, 0, 2, 2,
  0, 0, 0, 0, 2, 2, 0, -4
};

int8_t  map3BS [] PROGMEM = {
  -1, 0, 2, 0, 1, 1, 1, 1, 0, 2, 0, -2, 0, 0, 2, 0,
  0, 0, 0, 0, 0, 2, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 2, 2, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 2,
  2, 1, 0, 0, 2, 2, 2, 2, 0, 0, 1, 2, 2, 1, 0, 0,
  2, 2, 2, 2, 0, 0, 1, 2, 2, 1, 0, 0, 1, 1, 1, 1,
  0, 0, 1, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2,
  0, 0, 2, 0, 0, 0, 0, 0, 0, 2, 0, 0, -3, 0, 2, 0,
  1, 1, 1, 1, 0, 2, 0, -4
};

int kordboost[4];

//карты для PacMan. с ними всё сложнее, лучше не трогать

int8_t map1PM[8][11] {
  {10, 1, 8, 0, 2, 0, 9, 1, 1, 11, -9},
  { 2, 0, 0, 0, 2, 0, 0, 0, 0, 2, -9},
  { 8, 0, 15, 0, 5, 1, 11, 0, 14, 4, 1},
  { 0, 0, 2, 0, 13, -1, 13, 0, 0, 0, -9},
  {12, 0, 2, 0, 0, -9, 0, 0, 0, 14, 1},
  { 0, -9, 9, 12, 0, 14, 1, 12, -9, 0, -9},
  {11, -2, -9, 0, 0, 0, 0, -9, -3, 10, 1},
  { 9, 1, 11, 0, 15, 0, 10, 1, 1, 8, -9},
};

int8_t map2PM[8][11] {
  {10, 8, 0, 9, 1, 1, 7, 0, 9, 11, -9},
  { 8, 0, 0, 0, 0, 0, 13, 0, 0, 9, 1},
  { 0, 0, 10, 1, 11, 0, 0, 0, 0, 0, 0},
  {11, 0, 13, -1, 9, 12, 0, 16, 0, 10, 1},
  { 8, 0, 0, -9, 0, 0, 0, 0, 0, 9, 1},
  { 0, -9, 14, 1, 1, 12, 0, 16, -9, 0, 0},
  {11, -2, -9, 0, 0, 0, 0, -9, -3, 10, 1},
  { 9, 11, 0, 10, 1, 1, 11, 0, 10, 8, -9},
};

int8_t map3PM[8][11] {
  {10, 8, 0, 9, 1, 1, 8, 0, 9, 11, -9},
  { 2, 0, 0, 0, -9, -9, 0, 0, 0, 2, -9},
  { 4, 11, 0, 15, -2, -3, 15, 0, 0, 5, 1},
  { 0, 13, 0, 9, 1, 6, 8, 0, 14, 8, 0},
  { 0, 0, 0, 0, -9, 2, 0, 0, 0, 0, 0},
  {11, 0, 14, 12, -1, 9, 1, 12, 0, 10, 1},
  { 2, 0, 0, 0, -9, 0, 0, 0, 0, 2, -9},
  { 9, 11, 0, 10, 1, 1, 11, 0, 10, 8, -9},
};

//карты для arkaoid. можно редактировать.
// 0 - пустота
//-1 - то же что и 0, но в этой клетке не будут появляться бонусы
// 1 - неломающийся блок
// 5 - неломающийся блок. нужен для фона
// 2 - 12 - текстуры разных цветов, некоторые разрушаются с 2 попаданий

int8_t map1SB[24][32] {
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 2, 0, 3, 0, 2, 0, 3, -1, 2, -1, 3, -1, 2, -1, 5, 5, 5},
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, 5, 5, 5},
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, -1, 2, -1, -1, -1, -1, -1, -1, 5, 5, 5},
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 4, 0, 2, 3, 2, -1, -1, 2, -1, -1, 5, 5, 5},
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 4, 4, 0, 2, 2, -1, -1, 2, 2, -1, -1, 5, 5, 5},
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 4, 4, 0, 2, -1, -1, -1, 2, 2, -1, -1, -1, 5, 5, 5},
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 4, 4, 0, 2, 4, 4, -1, 2, 2, -1, -1, -1, -1, 5, 5, 5},
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 3, 4, 4, 0, 2, 4, 4, 3, -1, 2, -1, -1, -1, 3, -1, 5, 5, 5},
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 4, 4, 0, 2, 4, 4, 3, -1, -1, -1, -1, 2, -1, 3, 3, 5, 5, 5},
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 3, 4, 4, 0, 2, 4, 4, 3, 0, -1, 2, 2, 2, -1, 2, -1, -1, 5, 5, 5},
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 3, 4, 0, 2, 4, 4, 3, 0, 0, 2, 3, 3, 2, 2, -1, 2, -1, 5, 5, 5},
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 3, 4, 0, 2, 4, 3, 0, 0, 2, 3, 4, 4, 3, 2, 2, -1, -1, 5, 5, 5},
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 3, 4, 0, 2, 4, 3, 0, 0, 2, 3, 4, 4, 3, 2, 2, -1, -1, 5, 5, 5},
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 3, 4, 0, 2, 4, 4, 3, 0, 0, 2, 3, 3, 2, 2, -1, 2, -1, 5, 5, 5},
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 3, 4, 4, 0, 2, 4, 4, 3, 0, -1, 2, 2, 2, -1, 2, -1, -1, 5, 5, 5},
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 4, 4, 0, 2, 4, 4, 3, -1, -1, -1, -1, 2, -1, 3, 3, 5, 5, 5},
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 3, 4, 4, 0, 2, 4, 4, 3, -1, 2, -1, -1, -1, 3, -1, 5, 5, 5},
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 4, 4, 0, 2, 4, 4, -1, 2, 2, -1, -1, -1, -1, 5, 5, 5},
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 4, 4, 0, 2, -1, -1, -1, 2, 2, -1, -1, -1, 5, 5, 5},
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 4, 4, 0, 2, 2, -1, -1, 2, 2, -1, -1, 5, 5, 5},
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 4, 0, 2, 3, 2, -1, -1, 2, -1, -1, 5, 5, 5},
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, -1, 2, -1, -1, -1, -1, -1, -1, 5, 5, 5},
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, 5, 5, 5},
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 2, 0, 3, 0, 2, 0, 3, -1, 2, -1, 3, -1, 2, -1, 5, 5, 5},
};

int8_t map2SB[24][32] {
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, 5, 5, 5},
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, 5, 5, 5},
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 6, 0, 0, 0, 0, 0, -1, -1, -1, -1, 6, 6, -1, -1, 5, 5, 5},
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, -1, -1, -1, 6, 7, 7, 6, -1, 5, 5, 5},
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 7, 0, 6, 6, 3, 3, 3, 3, 3, -1, 2, 7, 6, -1, 5, 5, 5},
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 7, 0, 3, 6, 6, 6, -1, -1, -1, -1, 6, -1, -1, 5, 5, 5},
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 3, 0, 0, 0, 7, 6, 6, -1, 3, -1, -1, -1, 5, 5, 5},
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 2, 0, 3, 6, 0, 3, -1, 6, 2, 6, -1, 3, -1, -1, 5, 5, 5},
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 3, 7, 3, 0, -1, 2, -1, -1, 3, 3, -1, -1, 5, 5, 5},
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 6, 7, 2, 2, 2, -1, -1, 3, 7, 3, -1, -1, 5, 5, 5},
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 6, 7, 2, 0, 0, -1, -1, 3, 3, -1, -1, 3, -1, 5, 5, 5},
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 3, 7, 2, 7, 2, 2, 2, 2, 2, 7, -1, -1, 3, -1, 5, 5, 5},
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 3, 7, 2, 7, 2, 2, 2, 2, 2, 7, -1, -1, 3, -1, 5, 5, 5},
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 6, 7, 2, 0, 0, -1, -1, 3, 3, -1, -1, 3, -1, 5, 5, 5},
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 6, 0, 2, 2, 2, -1, -1, 3, 7, 3, -1, -1, 5, 5, 5},
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 2, 3, 0, 3, 0, -1, 2, -1, -1, 3, 3, -1, -1, 5, 5, 5},
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 2, 0, 3, 6, 0, 3, -1, 6, 2, 6, -1, 3, -1, -1, 5, 5, 5},
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 3, 0, 0, 0, 7, 6, 6, -1, 3, -1, -1, -1, 5, 5, 5},
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 7, 0, 3, 6, 6, 6, -1, -1, -1, -1, 6, -1, -1, 5, 5, 5},
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 7, 0, 6, 6, 3, 3, 3, 3, 3, -1, 2, 7, 6, -1, 5, 5, 5},
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, -1, -1, -1, 6, 7, 7, 6, -1, 5, 5, 5},
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 6, 0, 0, 0, 0, 0, -1, -1, -1, -1, 6, 6, -1, -1, 5, 5, 5},
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, 5, 5, 5},
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, 5, 5, 5},
};

int8_t map3SB[24][32] {
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 10, 9, -1, -1, -1, -1, -1, 5, 5, 5},
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 10, -1, -1, -1, -1, -1, 5, 5, 5},
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 12, 10, 10, -1, -1, -1, -1, 5, 5, 5},
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 12, 12, 9, 10, -1, -1, -1, 5, 5, 5},
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 12, 12, 12, 12, 10, 10, -1, -1, 5, 5, 5},
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 12, 12, 12, 9, 9, 9, 10, -1, -1, 5, 5, 5},
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 10, 10, 10, 10, 10, 12, 12, 12, 9, 9, 9, 9, 10, -1, -1, 5, 5, 5},
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 9, 9, 9, 10, 11, 12, 12, 12, 12, 12, 9, 9, 9, 9, 10, -1, -1, 5, 5, 5},
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 11, 12, 12, 12, 12, 12, 12, 9, 9, 9, 10, -1, -1, 5, 5, 5},
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 10, 10, 11, 11, 12, 12, 12, 12, 12, 12, 10, 10, -1, -1, -1, 5, 5, 5},
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 9, 9, 9, 10, 11, 11, 12, 12, 12, 12, 12, 10, 10, -1, -1, -1, -1, 5, 5, 5},
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 11, 11, 12, 12, 12, 12, 12, 10, -1, -1, -1, -1, -1, 5, 5, 5},
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 11, 11, 12, 12, 12, 12, 12, 10, -1, -1, -1, -1, -1, 5, 5, 5},
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 11, 11, 12, 12, 12, 12, 12, 10, -1, -1, -1, -1, -1, 5, 5, 5},
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 11, 11, 12, 12, 12, 12, 12, 10, -1, -1, -1, -1, -1, 5, 5, 5},
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 11, 11, 12, 12, 12, 12, 12, 10, -1, -1, -1, -1, -1, 5, 5, 5},
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 10, 10, 11, 11, 12, 12, 12, 12, 12, 10, -1, -1, -1, -1, -1, 5, 5, 5},
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 9, 9, 9, 10, 11, 11, 11, 11, 11, 11, 11, 10, -1, -1, -1, -1, -1, 5, 5, 5},
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 11, 11, 11, 11, 11, 11, 11, 10, -1, -1, -1, -1, -1, 5, 5, 5},
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, -1, -1, -1, -1, 5, 5, 5},
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 10, -1, -1, -1, -1, 5, 5, 5},
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, 9, 9, 10, -1, -1, -1, 5, 5, 5},
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, 9, 9, 10, -1, -1, 5, 5, 5},
  { -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, 9, 9, -1, -1, 5, 5, 5},
};

//карты для змейки. можно менять.
// 0 - пустота
// 1 - стена

int8_t map2S[24][32] {
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  { 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0},
  { 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
  { 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
  { 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0},
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  { 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0},
  { 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
  { 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
  { 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0},
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};

int8_t map3S[24][32] {
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  { 0, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 0},
  { 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0},
  { 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
  { 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0},
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  { 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0},
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
  { 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0},
  { 0, 1, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 1, 0},
  { 0, 1, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 1, 0},
  { 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0},
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
  { 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 1, 1, 1},
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  { 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0},
  { 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
  { 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0},
  { 0, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 0},
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};

int8_t mapM[120][120];

int8_t tetfon[21][11];

int8_t forMapSB[24][32];

int ys[200];
int xs[200];

byte b1_n[6];
byte b2_n[6];
byte b3_n[6];

uint32_t tmr1, tmr2, tmr3, tmr4, tmrbot1, tmrbot2, tmrbot3, tmrkill1, tmrkill2, tmrkill3, tmrzvuk, tmr5, tmr6;

int dlinatela, ix2, xp1, yp1, xp2, yp2, xp3, yp3, xp4, yp4, ix1, iy, iyst, ix1st, ix2st, a, b, l, o, x, y, yt1, yt2, s, hp, hpst, se, sd, sst,
    xbotst1, ybotst1, nbot1, nbotst1, bambot1, priravbot1, xbambot1, ybambot1, xbambotst1, ybambotst1, bampriravbot1, nbambot1, xbot1, ybot1,
    xbotst2, ybotst2, nbot2, nbotst2, bambot2, priravbot2, xbambot2, ybambot2, xbambotst2, ybambotst2, bampriravbot2, nbambot2, xbot2, ybot2,
    xbotst3, ybotst3, nbot3, nbotst3, bambot3, priravbot3, xbambot3, ybambot3, xbambotst3, ybambotst3, bampriravbot3, nbambot3, xbot3, ybot3,
    m, zv, botsBS, botsPM, kuda, forbotBS, forbotPM, p, q, t, forTema, sety, setyst, timeh, timem, i, xt, yt, dlaspase, xc, yc, dlac, xv, yv,
    dlav, sx, sy, xys, xst, yst, xp, yp, SR, n, n2, n3, nbam, xbam, ybam, xbamst, ybamst, sp, csp, xh, yh, dlah, sstst, mins, per, xbst, ybst, roketsize,
    kord, xboost, yboost, xb, yb, setty, settx, settyst, zvuky, fm, nst, ist, NG, rotat, bright, UP_BUTT, DOWN_BUTT, RIGHT_BUTT, LEFT_BUTT, timeSH;

long ran, ran1, ran2, spase, tabkord, ckord, vkord, hkord;

bool zvukbool, bomba0, p_b, zvukon, prig, pad, setnach = false, proris = true, flag, e2, e3, bam, bamprirav, g2, h2, gen, bird, kill1, kill2, kill3, edapr1, SH, ob15, ob14, ob13, ob12, ob11, ob10, ob9, ob8, ob7, ob6, ob5, ob4, ob3, ob2, ob1,
                                               sel, edapr2, kill0, tab, c, v, hep, igranach, redboll, redboll2, redboll3, kontakt, rokbig, rokmin, bollfet, bolllight, fet, light, armor, many, armoreffekt;

double arad, delitel, xcos, ysin, g, xcos2, ysin2, xcos3, ysin3;

int temp, pres, higr;

String str1, str2;

int16_t COLOR1, COLOR2, COLORSCREEN;

int spee = 4;

unsigned int topskore;

void res() { // функция для перезагруски платы
  ESP.restart();
}

void setup() {
  delay(200);
  Serial.begin(115200);
  tft.init(240, 320);
  tft.setSPISpeed(40000000);
  tft.invertDisplay(false);
  pref.begin("game", false);
  rotat = pref.getUChar("rotate", 1);
  tft.setRotation(rotat);
  if (rotat == 3) {
    UP_BUTT = U_BUTT;
    DOWN_BUTT = D_BUTT;
    LEFT_BUTT = L_BUTT;
    RIGHT_BUTT = R_BUTT;
  } else {
    UP_BUTT = D_BUTT;
    DOWN_BUTT = U_BUTT;
    LEFT_BUTT = R_BUTT;
    RIGHT_BUTT = L_BUTT;
  }
  tft.fillScreen(0x0000);
  Wire.begin();
  RTCtime.begin();
  bme.begin(0x76);
  Serial.println(" ");
  Serial.println(VERSION);
  bright = pref.getUChar("bright", 4);
  if (bright > 3) {
    a = 0;
  } else if (bright == 3) {
    a = 69;
  } else if (bright == 2) {
    a = 70;
  } else if (bright < 2) {
    a = 71;
  }
  rt.setRight(a);
  tft.drawBitmap(3, 3, loadC, 88, 12, 0xFFFF);
  tft.setTextSize(2);
  tft.setTextColor(0xFFFF);
  tft.setCursor(200, 220);
  tft.print("Loading.  ");
  tft.drawRGBBitmap(122, 90, load1, 78, 60);
  tft.drawRGBBitmap(122, 90, load2, 78, 60);
  delay(200);
  tft.setCursor(200, 220);
  tft.print("Loading.. ");
  tft.drawRGBBitmap(122, 90, load1, 78, 60);
  delay(200);
  tft.drawRGBBitmap(122, 90, load2, 78, 60);
  delay(200);
  tft.setCursor(200, 220);
  tft.print("Loading...");
  tft.drawRGBBitmap(122, 90, load1, 78, 60);
  delay(200);
  tft.drawRGBBitmap(122, 90, load2, 78, 60);
  delay(200);
  tft.setCursor(200, 220);
  tft.setTextColor(0x0000);
  tft.print("Loading...");
  tft.setCursor(200, 220);
  tft.setTextColor(0xFFFF);
  tft.print("Loading.  ");
  tft.drawRGBBitmap(122, 90, load1, 78, 60);
  pinMode(32, OUTPUT);
  digitalWrite(32, HIGH);
  delay(200);
  tft.drawRGBBitmap(122, 90, load2, 78, 60);
  delay(200);
  pinMode(32, OUTPUT);
  digitalWrite(32, HIGH);
  tft.setCursor(200, 220);
  tft.print("Loading.. ");
  tft.drawRGBBitmap(122, 90, load1, 78, 60);
  forTema = pref.getUChar("forTema", 0);
  if (forTema == 0) {
    COLOR1 = ST77XX_GREEN;
    COLOR2 = 0x915C;
    COLORSCREEN = 0x0000;
  } else if (forTema == 1) {
    COLOR1 = ST77XX_WHITE;
    COLOR2 = ST77XX_BLUE;
    COLORSCREEN = 0x0000;
  } else if (forTema == 2) {
    COLOR1 = 0x0622;
    COLOR2 = 0xFF40;
    COLORSCREEN = 0x0000;
  } else if (forTema == 3) {
    COLOR1 = ST77XX_WHITE;
    COLOR2 = ST77XX_RED;
    COLORSCREEN = 0x0000;
  }
  delay(200);
  tft.drawRGBBitmap(122, 90, load2, 78, 60);
  delay(200);
  tft.setCursor(200, 220);
  tft.print("Loading...");
  tft.drawRGBBitmap(122, 90, load1, 78, 60);
  zvukon = pref.getUChar("zvukon", 1);
  kuda = pref.getUChar("kuda", 0);
  botsBS = pref.getUChar("botsBS", 3);
  botsPM = pref.getUChar("botsPM", 2);
  delay(200);
  tft.drawRGBBitmap(122, 90, load2, 78, 60);
  delay(200);
  tft.setCursor(200, 220);
  tft.setTextColor(0x0000);
  tft.print("Loading...");
  tft.setCursor(200, 220);
  tft.setTextColor(0xFFFF);
  tft.print("Loading.  ");
  tft.drawRGBBitmap(122, 90, load1, 78, 60);
  fm = 0;
  if (kuda == 1) {
    tft.drawBitmap(3, 3, loadC, 88, 12, 0x0000);
    fm = 1;
    tft.setCursor(200, 220);
    tft.setTextColor(0x0000);
    tft.print("Loading.  ");
    tft.fillRect(122, 90, 78, 60, 0x0000);
    pref.putUChar("kuda", 0);
    igranach = true;
    settx = 0;
    setty = 3;
    iy = 1;
    ix2 = 1;
    proris = false;
    tft.setCursor(10, 20);
    tft.setTextColor(COLOR1);
    tft.setTextSize(2);
    tft.print(" time");
    tft.setCursor(10, 44);
    tft.print(" sound");
    tft.setCursor(10, 116);
    tft.print(" rotate");
    tft.setCursor(10, 140);
    tft.print(" reset");
    tft.setCursor(10, 164);
    tft.print(" meteo");
    tft.drawLine(114, 0, 114, 240, COLOR1);
    igrasettings();
    loop();
  } else if (kuda == 2) {
    tft.drawBitmap(3, 3, loadC, 88, 12, 0x0000);
    fm = 1;
    tft.setCursor(200, 220);
    tft.setTextColor(0x0000);
    tft.print("Loading.  ");
    tft.fillRect(122, 90, 78, 60, 0x0000);
    pref.putUChar("kuda", 0);
    igranach = true;
    settx = 0;
    setty = 5;
    iy = 1;
    ix2 = 1;
    proris = false;
    tft.setCursor(10, 20);
    tft.setTextColor(COLOR1);
    tft.setTextSize(2);
    tft.print(" time");
    tft.setCursor(10, 44);
    tft.print(" sound");
    tft.setCursor(10, 68);
    tft.print(" bright");
    tft.setCursor(10, 92);
    tft.print(" decor");
    tft.setCursor(10, 116);
    tft.print(" rotate");
    tft.setCursor(10, 164);
    tft.print(" meteo");
    tft.drawLine(114, 0, 114, 240, COLOR1);
    igrasettings();
    loop();
  } else if (kuda == 3) {
    tft.drawBitmap(3, 3, loadC, 88, 12, 0x0000);
    fm = 1;
    tft.setCursor(200, 220);
    tft.setTextColor(0x0000);
    tft.print("Loading.  ");
    tft.fillRect(122, 90, 78, 60, 0x0000);
    pref.putUChar("kuda", 0);
    igranach = true;
    settx = 0;
    setty = 4;
    iy = 1;
    ix2 = 1;
    proris = false;
    tft.setCursor(10, 20);
    tft.setTextColor(COLOR1);
    tft.setTextSize(2);
    tft.print(" time");
    tft.setCursor(10, 44);
    tft.print(" sound");
    tft.setCursor(10, 68);
    tft.print(" bright");
    tft.setCursor(10, 92);
    tft.print(" decor");
    tft.setCursor(10, 140);
    tft.print(" reset");
    tft.setCursor(10, 164);
    tft.print(" meteo");
    tft.drawLine(114, 0, 114, 240, COLOR1);
    igrasettings();
    loop();
  }
  if (fm == 0) {
    delay(200);
    tft.drawRGBBitmap(122, 90, load2, 78, 60);
    delay(200);
    tft.setCursor(200, 220);
    tft.print("Loading.. ");
    tft.drawRGBBitmap(122, 90, load1, 78, 60);
    delay(200);
    tft.drawRGBBitmap(122, 90, load2, 78, 60);
    delay(200);
    tft.setCursor(200, 220);
    tft.setTextColor(0x0000);
    tft.print("Loading.. ");
    tft.fillRect(122, 90, 78, 60, 0x0000);
    tft.drawBitmap(3, 3, loadC, 88, 12, 0x0000);
    tft.setCursor(30, 5);
    tft.setTextColor(COLOR1);
    tft.setTextSize(9);
    tft.print(RTCtime.gettime("H:i"));
    menuprilozh();
  }
} 

void loop() {//программа меню
  ix1st = ix1;
  ix2st = ix2;
  iyst = iy;
  if (millis() - tmrkill3 > 30000) {
    igrasleep();
  }
  if (digitalRead(LEFT_BUTT) == 0 || digitalRead(RIGHT_BUTT) == 0 || digitalRead(UP_BUTT) == 0 || digitalRead(DOWN_BUTT) == 0 || digitalRead(OK_BUTT) == 1 || digitalRead(HOME_BUTT) == 1) {
    tmrkill3 = millis();
  }
  if (millis() - tmr6 > 100) {
    if (digitalRead(RIGHT_BUTT) == 0) {
      if (iy == 0) {
        ix1 = ix1 + 1;
        if (ix1 > 9) {
          ix1 = 0;
        }
      } else if (iy == 1) {
        ix2 = ix2 - 1;
        if (ix2 < 0) {
          ix2 = 0;
        }
      }
      if (ix1 != ix1st || ix2 != ix2st || iy != iyst) {
        menuprilozh();
      }
      while (digitalRead(RIGHT_BUTT) == 0) {}
      tmr6 = millis();
    } else if (digitalRead(LEFT_BUTT) == 0) {
      if (iy == 0) {
        ix1 = ix1 - 1;
        if (ix1 < 0) {
          ix1 = 9;
        }
      } else if (iy == 1) {
        ix2 = ix2 + 1;
        if (ix2 > 1) {
          ix2 = 1;
        }
      }
      if (ix1 != ix1st || ix2 != ix2st || iy != iyst) {
        menuprilozh();
      }
      while (digitalRead(LEFT_BUTT) == 0) {}
      tmr6 = millis();
    } else if (digitalRead(UP_BUTT) == 0) {
      iy = iy - 1;
      if (iy < 0) {
        iy = 0;
      }
      if (ix1 != ix1st || ix2 != ix2st || iy != iyst) {
        ix2 = 1;
        menuprilozh();
      }
      while (digitalRead(UP_BUTT) == 0) {}
      tmr6 = millis();
    } else if (digitalRead(DOWN_BUTT) == 0) {
      iy = iy + 1;
      if (iy > 1) {
        iy = 1;
      }
      if (ix1 != ix1st || ix2 != ix2st || iy != iyst) {
        ix2 = 1;
        menuprilozh();
      }
      while (digitalRead(DOWN_BUTT) == 0) {}
      tmr6 = millis();
    }
  }
  if (millis() - tmr1 >= 60000) {
    tft.fillRect(30, 5, 261, 63, COLORSCREEN);
    tft.setCursor(30, 5);
    tft.setTextColor(COLOR1);
    tft.setTextSize(9);
    tft.print(RTCtime.gettime("H i"));
    tmr1 = millis();
  }
  if (millis() - tmr2 >= 1000) {
    flag = !flag;
    if (flag == false) {
      tft.setCursor(138, 5);
      tft.setTextColor(COLOR1);
      tft.setTextSize(9);
      tft.print(":");
    } else {
      tft.setCursor(138, 5);
      tft.setTextColor(COLORSCREEN);
      tft.setTextSize(9);
      tft.print(":");
    }
    tmr2 = millis();
  }
  if (digitalRead(OK_BUTT) == 1) {
    igranach = true;
    zvukbool = true;
    zv = 2;
    tmrzvuk = millis();
    zvuk();
    if (iy == 0) {
      if (ix1 == 0) {
        y = 0;
        fm = -1;
        igratanks();
        while (digitalRead(HOME_BUTT) == 1) {}
      } else if (ix1 == 1) {
        igrapong();
        while (digitalRead(HOME_BUTT) == 1) {}
      } else if (ix1 == 2) {
        igradudler();
        while (digitalRead(HOME_BUTT) == 1) {}
      } else if (ix1 == 3) {
        igrabird();
        while (digitalRead(HOME_BUTT) == 1) {}
      } else if (ix1 == 4) {
        y = 0;
        fm = -1;
        igrasnake();
        while (digitalRead(HOME_BUTT) == 1) {}
      } else if (ix1 == 5) {
        y = 0;
        fm = -1;
        igrapac();
        while (digitalRead(HOME_BUTT) == 1) {}
      } else if (ix1 == 6) {
        y = 0;
        fm = -1;
        igrasb();
        while (digitalRead(HOME_BUTT) == 1) {}
      } else if (ix1 == 7) {
        igratrax();
        while (digitalRead(HOME_BUTT) == 1) {}
      } else if (ix1 == 8) {
        y = 0;
        igratetris();
        while (digitalRead(HOME_BUTT) == 1) {}
      } else if (ix1 == 9) {
        y = 0;
        igramaze();
        while (digitalRead(HOME_BUTT) == 1) {}
      }
    } else if (iy == 1) {
      if (ix2 == 1) {
        igrasettings();
        while (digitalRead(HOME_BUTT) == 1) {}
      } else if (ix2 == 0) {
        igrainfo();
        while (digitalRead(HOME_BUTT) == 1) {}
      }
    }
  }
}

void menuprilozh() {
  if (iy == 0) {
    tft.drawBitmap(20, 100, obodikon, 80, 80, COLOR1);
    tft.drawBitmap(120, 100, obodikon, 80, 80, COLOR2);
    tft.drawBitmap(220, 100, obodikon, 80, 80, COLOR1);
    tft.drawBitmap(248, 200, shester, 21, 21, COLOR1);
    tft.drawBitmap(279, 200, info, 21, 21, COLOR1);
    tft.fillRect(20, 200, 220, 30, COLORSCREEN);
    tft.setCursor(20, 200);
    tft.setTextColor(COLOR2);
    tft.setTextSize(3);
    if (ix1 == 0) {
      tft.drawRGBBitmap(35, 115, menuL, 50, 50);
      tft.drawRGBBitmap(135, 115, menuBS, 50, 50);
      tft.drawRGBBitmap(235, 115, menuPP, 50, 50);
      tft.print("BATTLE CITY");
    } else if (ix1 == 1) {
      tft.drawRGBBitmap(35, 115, menuBS, 50, 50);
      tft.drawRGBBitmap(135, 115, menuPP, 50, 50);
      tft.drawRGBBitmap(235, 115, menuDJ, 50, 50);
      tft.print("PING-PONG");
    } else if (ix1 == 2) {
      tft.drawRGBBitmap(35, 115, menuPP, 50, 50);
      tft.drawRGBBitmap(135, 115, menuDJ, 50, 50);
      tft.drawRGBBitmap(235, 115, menuFB, 50, 50);
      tft.print("DOG JUMP");
    } else if (ix1 == 3) {
      tft.drawRGBBitmap(35, 115, menuDJ, 50, 50);
      tft.drawRGBBitmap(135, 115, menuFB, 50, 50);
      tft.drawRGBBitmap(235, 115, menuS, 50, 50);
      tft.print("FLAPPY BIRD");
    } else if (ix1 == 4) {
      tft.drawRGBBitmap(35, 115, menuFB, 50, 50);
      tft.drawRGBBitmap(135, 115, menuS, 50, 50);
      tft.drawRGBBitmap(235, 115, menuPM, 50, 50);
      tft.print("SNAKE");
    } else if (ix1 == 5) {
      tft.drawRGBBitmap(35, 115, menuS, 50, 50);
      tft.drawRGBBitmap(135, 115, menuPM, 50, 50);
      tft.drawRGBBitmap(235, 115, menuSB, 50, 50);
      tft.print("PAC-MAN");
    } else if (ix1 == 6) {
      tft.drawRGBBitmap(35, 115, menuPM, 50, 50);
      tft.drawRGBBitmap(135, 115, menuSB, 50, 50);
      tft.drawRGBBitmap(235, 115, menuDJ2, 50, 50);
      tft.print("ARKANOID");
    } else if (ix1 == 7) {
      tft.drawRGBBitmap(35, 115, menuSB, 50, 50);
      tft.drawRGBBitmap(135, 115, menuDJ2, 50, 50);
      tft.drawRGBBitmap(235, 115, menuT, 50, 50);
      tft.print("DOG JUMP 2");
    } else if (ix1 == 8) {
      tft.drawRGBBitmap(35, 115, menuDJ2, 50, 50);
      tft.drawRGBBitmap(135, 115, menuT, 50, 50);
      tft.drawRGBBitmap(235, 115, menuL, 50, 50);
      tft.print("TETRIS");
    } else if (ix1 == 9) {
      tft.drawRGBBitmap(35, 115, menuT, 50, 50);
      tft.drawRGBBitmap(135, 115, menuL, 50, 50);
      tft.drawRGBBitmap(235, 115, menuBS, 50, 50);
      tft.print("LABIRINTH");
    }
  } else if (iy == 1) {
    tft.drawBitmap(20, 100, obodikon, 80, 80, COLOR1);
    tft.drawBitmap(120, 100, obodikon, 80, 80, COLOR1);
    tft.drawBitmap(220, 100, obodikon, 80, 80, COLOR1);
    tft.setCursor(20, 200);
    tft.setTextColor(COLOR2);
    tft.setTextSize(3);
    tft.fillRect(20, 200, 220, 30, COLORSCREEN);
    if (ix2 == 0) {
      tft.drawBitmap(279, 200, info, 21, 21, COLOR2);
      tft.drawBitmap(248, 200, shester, 21, 21, COLOR1);
      tft.print("INFORMATION");
    } else {
      tft.drawBitmap(248, 200, shester, 21, 21, COLOR2);
      tft.drawBitmap(279, 200, info, 21, 21, COLOR1);
      tft.print("SETTINGS");
    }
  }
}

void menuprilozhy1() {
  tft.drawBitmap(20, 100, obodikon, 80, 80, COLOR1);
  tft.drawBitmap(120, 100, obodikon, 80, 80, COLOR1);
  tft.drawBitmap(220, 100, obodikon, 80, 80, COLOR1);
  if (ix1 == 0) {
    tft.drawRGBBitmap(35, 115, menuL, 50, 50);
    tft.drawRGBBitmap(135, 115, menuBS, 50, 50);
    tft.drawRGBBitmap(235, 115, menuPP, 50, 50);
  } else if (ix1 == 1) {
    tft.drawRGBBitmap(35, 115, menuBS, 50, 50);
    tft.drawRGBBitmap(135, 115, menuPP, 50, 50);
    tft.drawRGBBitmap(235, 115, menuDJ, 50, 50);
  } else if (ix1 == 2) {
    tft.drawRGBBitmap(35, 115, menuPP, 50, 50);
    tft.drawRGBBitmap(135, 115, menuDJ, 50, 50);
    tft.drawRGBBitmap(235, 115, menuFB, 50, 50);
  } else if (ix1 == 3) {
    tft.drawRGBBitmap(35, 115, menuDJ, 50, 50);
    tft.drawRGBBitmap(135, 115, menuFB, 50, 50);
    tft.drawRGBBitmap(235, 115, menuS, 50, 50);
  } else if (ix1 == 4) {
    tft.drawRGBBitmap(35, 115, menuFB, 50, 50);
    tft.drawRGBBitmap(135, 115, menuS, 50, 50);
    tft.drawRGBBitmap(235, 115, menuPM, 50, 50);
  } else if (ix1 == 5) {
    tft.drawRGBBitmap(35, 115, menuS, 50, 50);
    tft.drawRGBBitmap(135, 115, menuPM, 50, 50);
    tft.drawRGBBitmap(235, 115, menuSB, 50, 50);
  } else if (ix1 == 6) {
    tft.drawRGBBitmap(35, 115, menuPM, 50, 50);
    tft.drawRGBBitmap(135, 115, menuSB, 50, 50);
    tft.drawRGBBitmap(235, 115, menuDJ2, 50, 50);
  } else if (ix1 == 7) {
    tft.drawRGBBitmap(35, 115, menuSB, 50, 50);
    tft.drawRGBBitmap(135, 115, menuDJ2, 50, 50);
    tft.drawRGBBitmap(235, 115, menuT, 50, 50);
  } else if (ix1 == 8) {
    tft.drawRGBBitmap(35, 115, menuDJ2, 50, 50);
    tft.drawRGBBitmap(135, 115, menuT, 50, 50);
    tft.drawRGBBitmap(235, 115, menuL, 50, 50);
  } else if (ix1 == 9) {
    tft.drawRGBBitmap(35, 115, menuT, 50, 50);
    tft.drawRGBBitmap(135, 115, menuL, 50, 50);
    tft.drawRGBBitmap(235, 115, menuBS, 50, 50);
  }
  tft.setCursor(20, 200);
  tft.setTextColor(COLOR2);
  tft.setTextSize(3);
  if (ix2 == 0) {
    tft.drawBitmap(279, 200, info, 21, 21, COLOR2);
    tft.drawBitmap(248, 200, shester, 21, 21, COLOR1);
    tft.print("INFORMATION");
  } else {
    tft.drawBitmap(248, 200, shester, 21, 21, COLOR2);
    tft.drawBitmap(279, 200, info, 21, 21, COLOR1);
    tft.print("SETTINGS");
  }
}

void igratanks() {
  if (y != 4 && fm == -1) {
    tft.fillScreen(0x0000);
    tft.drawRGBBitmap(60, 10, forBS, 200, 64);
    tft.setTextSize(1);
    tft.setTextColor(0xFFFF);
    tft.setCursor(40, 100);
    topskore = pref.getUChar("skoreBS", 0);
    tft.print("TOP SKORE: " + String(topskore));
    tft.setCursor(40, 120);
    tft.setTextSize(2);
    tft.print("MAP:");
    tft.drawRGBBitmap(29, 150, mapBS1, 68, 56);
    tft.drawRGBBitmap(126, 150, mapBS2, 68, 56);
    tft.drawRGBBitmap(223, 150, mapBS3, 68, 56);
    tft.drawBitmap(29, 150, forMap, 68, 56, 0xFFFF);
    tft.drawBitmap(126, 150, forMap, 68, 56, 0x73AE);
    tft.drawBitmap(223, 150, forMap, 68, 56, 0x73AE);
    if (fm == -1) {
      fm = 0;
    }
    while (digitalRead(OK_BUTT) == 1) {}
    while (digitalRead(OK_BUTT) == 0 && y != 4) {
      if (digitalRead(RIGHT_BUTT) == 0) {
        fm = fm + 1;
        if (fm > 2) {
          fm = 2;
        } else {
          if (fm == 1) {
            tft.drawBitmap(29, 150, forMap, 68, 56, 0x73AE);
            tft.drawBitmap(126, 150, forMap, 68, 56, 0xFFFF);
            tft.drawBitmap(223, 150, forMap, 68, 56, 0x73AE);
          } else {
            tft.drawBitmap(126, 150, forMap, 68, 56, 0x73AE);
            tft.drawBitmap(223, 150, forMap, 68, 56, 0xFFFF);
          }
        }
        while (digitalRead(RIGHT_BUTT) == 0) {}
        delay(100);
      }
      if (digitalRead(LEFT_BUTT) == 0) {
        fm = fm - 1;
        if (fm < 0) {
          fm = 0;
        } else {
          if (fm == 0) {
            tft.drawBitmap(29, 150, forMap, 68, 56, 0xFFFF);
            tft.drawBitmap(126, 150, forMap, 68, 56, 0x73AE);
          } else {
            tft.drawBitmap(29, 150, forMap, 68, 56, 0x73AE);
            tft.drawBitmap(126, 150, forMap, 68, 56, 0xFFFF);
            tft.drawBitmap(223, 150, forMap, 68, 56, 0x73AE);
          }
        }
        while (digitalRead(LEFT_BUTT) == 0) {}
        delay(100);
      }
      if (digitalRead(HOME_BUTT) == 1) {
        zvukbool = true;
        zv = 2;
        tmrzvuk = millis();
        zvuk();
        igranach = false;
        y = 4;
      }
    }
  }
  if (y != 4) {
    s = 0;
    sst = -1;
    tft.setTextSize(1);
    nachalo();
    NG = 1;
    while (igranach == true) {
      //if (millis() - tmr5 > SLEEP) {
      vistrel();
      dvizhenie();
      if (millis() - tmr6 > 60) {
        e2 = !e2;
        tmr6 = millis();
      }
      bot1();
      bot2();
      bot3();
      popal();
      a = a + 1;
      tmr5 = millis();
      //}
      if (hp != hpst) {
        if (hp == 2) {
          tft.drawBitmap(295, 100, h, 18, 18, ST77XX_BLACK);
        } else if (hp == 1) {
          tft.drawBitmap(295, 80, h, 18, 18, ST77XX_BLACK);
        } else if (hp == 0) {
          tft.drawBitmap(295, 60, h, 18, 18, ST77XX_BLACK);
          stope();
        }
        hpst = hp;
      }
      if (sst != s) {
        tft.fillRect(288, 0, 32, 20, 0x7BEF);
        tft.setCursor(293, 6);
        tft.setTextColor(ST77XX_WHITE);
        tft.print(s);
        sst = s;
      }
      if (millis() - tmr2 >= 1000) {
        tft.fillRect(288, 220, 32, 20, 0x7BEF);
        tft.setCursor(293, 226);
        tft.setTextColor(ST77XX_WHITE);
        tft.print(a);
        a = 0;
        tmr2 = millis();
      }
      pausa();
    }
  }
  igranach = false;
  tft.fillScreen(COLORSCREEN);
  tft.setCursor(30, 5);
  tft.setTextColor(COLOR1);
  tft.setTextSize(9);
  tft.print(RTCtime.gettime("H i"));
  tmr1 = millis();
  tmrkill3 = millis();
  menuprilozh();
  while (digitalRead(OK_BUTT) == 1) {}
}

void igramaze() {
  tft.fillScreen(0x0000);
  tft.drawRGBBitmap(96, 10, forL, 128, 64);
  tft.setTextSize(1);
  tft.setTextColor(0xFFFF);
  tft.setCursor(40, 100);
  topskore = pref.getUChar("skoreL", 0);
  tft.print("TOP SKORE: " + String(topskore));
  tft.setTextSize(2);
  tft.setCursor(40, 140);
  tft.print("TAP TO START");
  while (digitalRead(OK_BUTT) == 1) {}
  while (digitalRead(OK_BUTT) == 0) {
    if (digitalRead(HOME_BUTT) == 1) {
      zvukbool = true;
      zv = 2;
      tmrzvuk = millis();
      zvuk();
      y = 4;
      break;
    }
  }
  if (y != 4) {
    s = 0;
    timeSH = 50;
    igranach = true;
    NG = 10;
    while (igranach == true) {
      delamaze();
      while (e3 == true) {
        if (millis() - tmr6 > timeSH) {
          mob0();
          ranLab();
          tmr6 = millis();
        }
        if (x == xbot2 && y == ybot2) {
          e3 = false;
        }
        pausa();
      }
    }
  }
  igranach = false;
  tft.fillScreen(COLORSCREEN);
  tft.setCursor(30, 5);
  tft.setTextColor(COLOR1);
  tft.setTextSize(9);
  tft.print(RTCtime.gettime("H i"));
  tmr1 = millis();
  tmrkill3 = millis();
  menuprilozh();
  while (digitalRead(OK_BUTT) == 1) {}
}

void igratetris() {
  tft.fillScreen(0x0000);
  tft.drawRGBBitmap(64, 10, forT, 191, 56);
  tft.drawRGBBitmap(220, 160, forTpic, 64, 62);
  tft.setTextSize(1);
  tft.setTextColor(0xFFFF);
  tft.setCursor(40, 100);
  topskore = pref.getUChar("skoreT", 0);
  tft.print("TOP SKORE: " + String(topskore));
  tft.setTextSize(2);
  tft.setCursor(40, 140);
  tft.print("TAP TO START");
  while (digitalRead(OK_BUTT) == 1) {}
  while (digitalRead(OK_BUTT) == 0) {
    if (digitalRead(HOME_BUTT) == 1) {
      zvukbool = true;
      zv = 2;
      tmrzvuk = millis();
      zvuk();
      y = 4;
      break;
    }
  }
  if (y != 4) {
    tft.fillScreen(0x0000);
    tft.drawLine(121, 0, 121, 240, ST77XX_GREEN);
    x = 0;
    y = 0;
    while (y < 20) {
      while (x < 10) {
        tetfon[y][x] = 0;
        x = x + 1;
      }
      x = 0;
      y = y + 1;
    }
    kill0 = true;
    n = 0;
    x = 0;
    y = 0;
    i = 0;
    s = -1;
    e2 = true;
    igranach = true;
    NG = 9;
    while (igranach == true) {
      delatetris();
      pausa();
    }
  }
  y = 0;
  igranach = false;
  tft.fillScreen(COLORSCREEN);
  tft.setCursor(30, 5);
  tft.setTextColor(COLOR1);
  tft.setTextSize(9);
  tft.print(RTCtime.gettime("H i"));
  tmr1 = millis();
  tmrkill3 = millis();
  menuprilozh();
  while (digitalRead(OK_BUTT) == 1) {}
}

void igratrax() {
  tft.fillScreen(ST77XX_CYAN);
  tft.drawRGBBitmap(60, 10, forDJ2, 200, 60);
  tft.setTextSize(1);
  tft.setTextColor(0x0000);
  tft.setCursor(40, 100);
  topskore = pref.getUChar("skoreDJ2", 0);
  tft.print("TOP SKORE: " + String(topskore));
  tft.setTextSize(2);
  tft.setCursor(40, 140);
  tft.print("TAP TO START");
  y = 0;
  while (digitalRead(OK_BUTT) == 1) {}
  while (digitalRead(OK_BUTT) == 0) {
    if (digitalRead(HOME_BUTT) == 1) {
      zvukbool = true;
      zv = 2;
      tmrzvuk = millis();
      zvuk();
      y = 4;
      break;
    }
  }
  if (y != 4) {
    tft.fillScreen(ST77XX_CYAN);
    tft.fillRect(0, 200, 320, 40, 0x03E0);
    igranach = true;
    y = 200;
    xbot1 = 160;
    xbot2 = 320;
    kill1 = false;
    kill2 = false;
    se = 2;
    sd = 3;
    o = 320;
    s = 0;
    sst = 1;
    prig = false;
    NG = 8;
    while (igranach == true) {
      pausa();
      m = m + 1;
      if (millis() - tmr5 > 1000) {
        tft.setTextSize(1);
        tft.fillRect(0, 0, 50, 10, ST77XX_CYAN);
        tft.setCursor(0, 0);
        tft.setTextColor(0x0000);
        tft.print("FPS: " + String(m));
        m = 0;
        tmr5 = millis();
      }
      if (s != sst) {
        tft.setTextSize(1);
        tft.setCursor(50, 0);
        tft.setTextColor(ST77XX_CYAN);
        tft.print("       " + String(sst));
        tft.setCursor(50, 0);
        tft.setTextColor(0x0000);
        tft.print("skore: " + String(s));
        sst = s;
      }
      begrex();
      preprex();
      if (millis() - tmr4 > 200) {
        o = o - 4;
        tft.drawBitmap(o, 20, obloko1, 70, 25, 0xFFFF);
        tft.drawBitmap(o, 20, obloko2, 70, 25, 0x07FF);
        tft.fillRect(o + 70, 20, 4, 25, 0x07FF);
        if (o < -70) {
          o = 320;
        }
        tmr4 = millis();
      }
    }
  }
  igranach = false;
  tft.fillScreen(COLORSCREEN);
  tft.setCursor(30, 5);
  tft.setTextColor(COLOR1);
  tft.setTextSize(9);
  tft.print(RTCtime.gettime("H i"));
  tmr1 = millis();
  tmrkill3 = millis();
  menuprilozh();
  while (digitalRead(OK_BUTT) == 1) {}
}

void igrasnake() {
  if (y != 4 && fm == -1) {
    tft.fillScreen(0x0000);
    tft.drawRGBBitmap(80, 10, forS, 160, 32);
    tft.setTextSize(1);
    tft.setTextColor(0xFFFF);
    tft.setCursor(40, 100);
    topskore = pref.getUChar("skoreS", 0);
    tft.print("TOP SKORE: " + String(topskore));
    tft.setCursor(40, 120);
    tft.setTextSize(2);
    tft.print("MAP:");
    tft.drawRGBBitmap(26, 150, mapS1, 72, 56);
    tft.drawRGBBitmap(124, 150, mapS2, 72, 56);
    tft.drawRGBBitmap(222, 150, mapS3, 72, 56);
    tft.drawBitmap(26, 150, forMap3, 72, 56, 0xFFFF);
    tft.drawBitmap(124, 150, forMap3, 72, 56, 0x73AE);
    tft.drawBitmap(222, 150, forMap3, 72, 56, 0x73AE);
    if (fm == -1) {
      fm = 0;
    }
    while (digitalRead(OK_BUTT) == 1) {}
    while (digitalRead(OK_BUTT) == 0 && y != 4) {
      if (digitalRead(RIGHT_BUTT) == 0) {
        fm = fm + 1;
        if (fm > 2) {
          fm = 2;
        } else {
          if (fm == 1) {
            tft.drawBitmap(26, 150, forMap3, 72, 56, 0x73AE);
            tft.drawBitmap(124, 150, forMap3, 72, 56, 0xFFFF);
            tft.drawBitmap(222, 150, forMap3, 72, 56, 0x73AE);
          } else {
            tft.drawBitmap(124, 150, forMap3, 72, 56, 0x73AE);
            tft.drawBitmap(222, 150, forMap3, 72, 56, 0xFFFF);
          }
        }
        while (digitalRead(RIGHT_BUTT) == 0) {}
        delay(100);
      }
      if (digitalRead(LEFT_BUTT) == 0) {
        fm = fm - 1;
        if (fm < 0) {
          fm = 0;
        } else {
          if (fm == 0) {
            tft.drawBitmap(26, 150, forMap3, 72, 56, 0xFFFF);
            tft.drawBitmap(124, 150, forMap3, 72, 56, 0x73AE);
          } else {
            tft.drawBitmap(26, 150, forMap3, 72, 56, 0x73AE);
            tft.drawBitmap(124, 150, forMap3, 72, 56, 0xFFFF);
            tft.drawBitmap(222, 150, forMap3, 72, 56, 0x73AE);
          }
        }
        while (digitalRead(LEFT_BUTT) == 0) {}
        delay(100);
      }
      if (digitalRead(HOME_BUTT) == 1) {
        zvukbool = true;
        zv = 2;
        tmrzvuk = millis();
        zvuk();
        igranach = false;
        y = 4;
      }
    }
  }
  if (y != 4) {
    tft.fillScreen(0x0000);
    mapSnake();
    dlinatela = 4;
    x = 100;
    y = 100;
    n = 1;
    s = 0;
    edapr1 = false;
    igranach = true;
    NG = 5;
    while (igranach == true) {
      pausa();
      delasnake();
    }
  }
  igranach = false;
  tft.fillScreen(COLORSCREEN);
  tft.setCursor(30, 5);
  tft.setTextColor(COLOR1);
  tft.setTextSize(9);
  tft.print(RTCtime.gettime("H i"));
  tmr1 = millis();
  tmrkill3 = millis();
  menuprilozh();
  while (digitalRead(OK_BUTT) == 1) {}
}

void igradudler() {
  tft.fillScreen(0xFF16);
  tft.drawRGBBitmap(60, 10, forDJ, 200, 60);
  tft.setTextSize(1);
  tft.setTextColor(0x0000);
  tft.setCursor(40, 100);
  topskore = pref.getUChar("skoreDJ", 0);
  tft.print("TOP SKORE: " + String(topskore));
  tft.setTextSize(2);
  tft.setCursor(40, 140);
  tft.print("TAP TO START");
  y = 0;
  while (digitalRead(OK_BUTT) == 1) {}
  while (digitalRead(OK_BUTT) == 0) {
    if (digitalRead(HOME_BUTT) == 1) {
      zvukbool = true;
      zv = 2;
      tmrzvuk = millis();
      zvuk();
      y = 4;
      break;
    }
  }
  if (y != 4) {
    tft.fillScreen(0xFF16);
    s = 0;
    y = 160;
    x = 140;
    n = 1;
    prig = true;
    g = 10;
    pad = false;
    spawnplot();
    NG = 3;
    while (igranach == true) {
      pausa();
      duddela();
    }
  }
  igranach = false;
  tft.fillScreen(COLORSCREEN);
  tft.setCursor(30, 5);
  tft.setTextColor(COLOR1);
  tft.setTextSize(9);
  tft.print(RTCtime.gettime("H i"));
  tmr1 = millis();
  tmrkill3 = millis();
  menuprilozh();
  while (digitalRead(OK_BUTT) == 1) {}
}

void igrabird() {
  tft.fillScreen(0x07FF);
  tft.drawRGBBitmap(52, 10, forFB, 216, 54);
  tft.setTextSize(1);
  tft.setTextColor(0x0000);
  tft.setCursor(40, 100);
  topskore = pref.getUChar("skoreFB", 0);
  tft.print("TOP SKORE: " + String(topskore));
  tft.setTextSize(2);
  tft.setCursor(40, 140);
  tft.setTextColor(0x0000);
  tft.print("TAP TO START");
  y = 0;
  while (digitalRead(OK_BUTT) == 1) {}
  while (digitalRead(OK_BUTT) == 0) {
    if (digitalRead(HOME_BUTT) == 1) {
      zvukbool = true;
      zv = 2;
      tmrzvuk = millis();
      zvuk();
      y = 4;
      break;
    }
  }
  if (y != 4) {
    tft.setTextSize(1);
    tft.fillScreen(0x07FF);
    xbot1 = 320;
    xbot2 = 520;
    y = 120;
    ran1 = random(1, 8) * 22;
    ran2 = random(1, 8) * 22;
    NG = 4;
    s = 0;
    g = 0;
    while (igranach == true) {
      pausa();
      dela();
    }
  }
  igranach = false;
  tft.fillScreen(COLORSCREEN);
  tft.setCursor(30, 5);
  tft.setTextColor(COLOR1);
  tft.setTextSize(9);
  tft.print(RTCtime.gettime("H i"));
  tmr1 = millis();
  tmrkill3 = millis();
  menuprilozh();
  while (digitalRead(OK_BUTT) == 1) {}
}

void igrapong() {
  tft.fillScreen(0x0000);
  tft.drawRGBBitmap(85, 10, forPP, 150, 84);
  tft.setTextSize(2);
  tft.setCursor(40, 140);
  tft.setTextColor(0xFFFF);
  tft.print("TAP TO START");
  y = 0;
  while (digitalRead(OK_BUTT) == 1) {}
  while (digitalRead(OK_BUTT) == 0) {
    if (digitalRead(HOME_BUTT) == 1) {
      zvukbool = true;
      zv = 2;
      tmrzvuk = millis();
      zvuk();
      y = 4;
      break;
    }
  }
  if (y != 4) {
    e2 = true;
    n = 0;
    o = 0;
    q = 0;
    p = 0;
    tft.fillScreen(ST77XX_BLACK);
    for (int i = 0; i < 154; i = i + 2) {
      tft.fillRect(157, i * 16, 6, 16, 0x31A6);
    }
    tft.drawBitmap(10, y, roketMap, 10, 40, 0xFF80);
    NG = 2;
    xbot1 = 152;
    ybot1 = 112;
    kill1 = true;
    xbot2 = 152;
    ybot2 = 112;
    kill2 = true;
    xbot3 = 152;
    ybot3 = 112;
    kill3 = true;
    tft.setTextColor(0x31A6);
    tft.setTextSize(7);
    if (rotat == 3) {
      tft.setRotation(2);
    } else {
      tft.setRotation(4);
    }
    tft.setCursor(91, 221);
    tft.print("000");
    if (rotat == 3) {
      tft.setRotation(4);
    } else {
      tft.setRotation(2);
    }
    tft.setCursor(91, 221);
    tft.print("000");
    tft.setRotation(rotat);
    while (igranach == true) {
      pausa();
      roket();
      roketbot();
      mathBoll1();
      mathBoll2();
      mathBoll3();
      polepong();
    }
  }
  igranach = false;
  tft.fillScreen(COLORSCREEN);
  tft.setCursor(30, 5);
  tft.setTextColor(COLOR1);
  tft.setTextSize(9);
  tft.print(RTCtime.gettime("H i"));
  tmr1 = millis();
  tmrkill3 = millis();
  menuprilozh();
  while (digitalRead(OK_BUTT) == 1) {}
}

void igrasb() {
  if (y != 4 && fm == -1) {
    tft.fillScreen(0x0000);
    tft.drawRGBBitmap(60, 10, forSB, 224, 42);
    tft.setTextSize(1);
    tft.setTextColor(0xFFFF);
    tft.setCursor(40, 100);
    topskore = pref.getUChar("skoreSB", 0);
    tft.print("TOP SKORE: " + String(topskore));
    tft.setCursor(40, 120);
    tft.setTextSize(2);
    tft.print("MAP:");
    tft.drawRGBBitmap(20, 150, mapSB1, 80, 68);
    tft.drawRGBBitmap(120, 150, mapSB2, 80, 68);
    tft.drawRGBBitmap(220, 150, mapSB3, 80, 68);
    tft.drawBitmap(20, 150, for2Map, 80, 68, 0xFFFF);
    tft.drawBitmap(120, 150, for2Map, 80, 68, 0x73AE);
    tft.drawBitmap(220, 150, for2Map, 80, 68, 0x73AE);
    if (fm == -1) {
      fm = 0;
    }
    while (digitalRead(OK_BUTT) == 1) {}
    while (digitalRead(OK_BUTT) == 0 && y != 4) {
      if (digitalRead(RIGHT_BUTT) == 0) {
        fm = fm + 1;
        if (fm > 2) {
          fm = 2;
        } else {
          if (fm == 1) {
            tft.drawBitmap(20, 150, for2Map, 80, 68, 0x73AE);
            tft.drawBitmap(120, 150, for2Map, 80, 68, 0xFFFF);
            tft.drawBitmap(220, 150, for2Map, 80, 68, 0x73AE);
          } else {
            tft.drawBitmap(120, 150, for2Map, 80, 68, 0x73AE);
            tft.drawBitmap(220, 150, for2Map, 80, 68, 0xFFFF);
          }
        }
        while (digitalRead(RIGHT_BUTT) == 0) {}
        delay(100);
      }
      if (digitalRead(LEFT_BUTT) == 0) {
        fm = fm - 1;
        if (fm < 0) {
          fm = 0;
        } else {
          if (fm == 0) {
            tft.drawBitmap(20, 150, for2Map, 80, 68, 0xFFFF);
            tft.drawBitmap(120, 150, for2Map, 80, 68, 0x73AE);
          } else {
            tft.drawBitmap(20, 150, for2Map, 80, 68, 0x73AE);
            tft.drawBitmap(120, 150, for2Map, 80, 68, 0xFFFF);
            tft.drawBitmap(220, 150, for2Map, 80, 68, 0x73AE);
          }
        }
        while (digitalRead(LEFT_BUTT) == 0) {}
        delay(100);
      }
      if (digitalRead(HOME_BUTT) == 1) {
        zvukbool = true;
        zv = 2;
        tmrzvuk = millis();
        zvuk();
        igranach = false;
        y = 4;
      }
    }
  }
  if (y != 4) {
    y = 0;
    tft.setTextSize(1);
    nachalokart();
    kill0 = true;
    s = 0;
    bollfet = false;
    bolllight = false;
    rokmin = false;
    armor = false;
    rokbig = false;
    many = false;
    NG = 7;
    while (igranach == true) {
      pausa();
      boostsb();
      popalsb();
      if (millis() - tmr5 > SLEEPSB) {
        roketsb();
        bollsb();
        a = a + 1;
        tmr5 = millis();
      }
      if (hp != hpst) {
        if (hp == 3) {
          tft.drawBitmap(290, 180, coinsb, 10, 10, ST77XX_MAGENTA);
          tft.drawBitmap(300, 180, coinsb, 10, 10, ST77XX_MAGENTA);
          tft.drawBitmap(310, 180, coinsb, 10, 10, ST77XX_MAGENTA);
        } else if (hp == 2) {
          tft.drawBitmap(290, 180, coinsb, 10, 10, ST77XX_MAGENTA);
          tft.drawBitmap(300, 180, coinsb, 10, 10, ST77XX_MAGENTA);
          tft.drawBitmap(310, 180, coinsb, 10, 10, ST77XX_BLACK);
        } else if (hp == 1) {
          tft.drawBitmap(290, 180, coinsb, 10, 10, ST77XX_MAGENTA);
          tft.drawBitmap(300, 180, coinsb, 10, 10, ST77XX_BLACK);
          tft.drawBitmap(310, 180, coinsb, 10, 10, ST77XX_BLACK);
        } else if (hp < 1) {
          tft.drawBitmap(290, 180, coinsb, 10, 10, ST77XX_BLACK);
          tft.drawBitmap(300, 180, coinsb, 10, 10, ST77XX_BLACK);
          tft.drawBitmap(310, 180, coinsb, 10, 10, ST77XX_BLACK);
          stopsb();

        }
        hpst = hp;
      }
      if (sst != s) {
        tft.fillRect(290, 190, 30, 25, 0xC618);
        tft.setCursor(295, 195);
        tft.setTextColor(ST77XX_WHITE);
        tft.print(s);
        sst = s;
      }
      if (millis() - tmrbot1 >= 1000) {
        tft.fillRect(290, 215, 30, 25, 0xC618);
        tft.setCursor(295, 220);
        tft.setTextColor(ST77XX_RED);
        tft.print(a);
        a = 0;
        tmrbot1 = millis();
      }
      if (millis() - tmr3 > 3000 && (fet == true || light == true)) {
        fet = false;
        light = false;
        tft.drawBitmap(290, 120, effekt5, 30, 30, ST77XX_WHITE);
        tft.drawBitmap(290, 150, effekt6, 30, 30, ST77XX_WHITE);
      }
      if (millis() - tmr4 > 10000 && armoreffekt == true) {
        armoreffekt = false;
        tft.fillRect(16, 0, 4, 240, 0x07FF);
        tft.drawBitmap(290, 90, effekt4, 30, 30, ST77XX_WHITE);
      }
    }
  }
  igranach = false;
  tft.fillScreen(COLORSCREEN);
  tft.setCursor(30, 5);
  tft.setTextColor(COLOR1);
  tft.setTextSize(9);
  tft.print(RTCtime.gettime("H i"));
  tmr1 = millis();
  tmrkill3 = millis();
  menuprilozh();
  while (digitalRead(OK_BUTT) == 1) {}
}

void igrainfo() {
  tft.fillScreen(COLORSCREEN);
  tft.drawBitmap(0, 0, info_text, 320, 240, COLOR1);
  while (igranach == true) {
    if (digitalRead(HOME_BUTT) == 1) {
      zvukbool = true;
      zv = 2;
      tmrzvuk = millis();
      zvuk();
      igranach = false;
      break;
    }
  }
  igranach = false;
  tft.fillScreen(COLORSCREEN);
  tft.setCursor(30, 5);
  tft.setTextColor(COLOR1);
  tft.setTextSize(9);
  tft.print(RTCtime.gettime("H i"));
  tmr1 = millis();
  tmrkill3 = millis();
  menuprilozhy1();
}

void igrapac() {
  if (y != 4 && fm == -1) {
    tft.fillScreen(0x0000);
    tft.drawRGBBitmap(60, 10, forPM, 200, 39);
    tft.setTextSize(1);
    tft.setTextColor(0xFFFF);
    tft.setCursor(40, 100);
    topskore = pref.getUChar("skorePM", 0);
    tft.print("TOP SKORE: " + String(topskore));
    tft.setCursor(40, 120);
    tft.setTextSize(2);
    tft.print("MAP:");
    tft.drawRGBBitmap(29, 150, mapPM1, 68, 56);
    tft.drawRGBBitmap(126, 150, mapPM2, 68, 56);
    tft.drawRGBBitmap(223, 150, mapPM3, 68, 56);
    tft.drawBitmap(29, 150, forMap, 68, 56, 0xFFFF);
    tft.drawBitmap(126, 150, forMap, 68, 56, 0x73AE);
    tft.drawBitmap(223, 150, forMap, 68, 56, 0x73AE);
    if (fm == -1) {
      fm = 0;
    }
    while (digitalRead(OK_BUTT) == 1) {}
    while (digitalRead(OK_BUTT) == 0 && y != 4) {
      if (digitalRead(RIGHT_BUTT) == 0) {
        fm = fm + 1;
        if (fm > 2) {
          fm = 2;
        } else {
          if (fm == 1) {
            tft.drawBitmap(29, 150, forMap, 68, 56, 0x73AE);
            tft.drawBitmap(126, 150, forMap, 68, 56, 0xFFFF);
            tft.drawBitmap(223, 150, forMap, 68, 56, 0x73AE);
          } else {
            tft.drawBitmap(126, 150, forMap, 68, 56, 0x73AE);
            tft.drawBitmap(223, 150, forMap, 68, 56, 0xFFFF);
          }
        }
        while (digitalRead(RIGHT_BUTT) == 0) {}
        delay(100);
      }
      if (digitalRead(LEFT_BUTT) == 0) {
        fm = fm - 1;
        if (fm < 0) {
          fm = 0;
        } else {
          if (fm == 0) {
            tft.drawBitmap(29, 150, forMap, 68, 56, 0xFFFF);
            tft.drawBitmap(126, 150, forMap, 68, 56, 0x73AE);
          } else {
            tft.drawBitmap(29, 150, forMap, 68, 56, 0x73AE);
            tft.drawBitmap(126, 150, forMap, 68, 56, 0xFFFF);
            tft.drawBitmap(223, 150, forMap, 68, 56, 0x73AE);
          }
        }
        while (digitalRead(LEFT_BUTT) == 0) {}
        delay(100);
      }
      if (digitalRead(HOME_BUTT) == 1) {
        zvukbool = true;
        zv = 2;
        tmrzvuk = millis();
        zvuk();
        igranach = false;
        y = 4;
      }
    }
  }
  if (y != 4) {
    n = 1;
    nbot1 = 1;
    nbot2 = 1;
    hp = 3;
    tab = false;
    s = 0;
    nachpac();
    NG = 6;
    while (igranach == true) {
      pausa();
      if (millis() - tmr5 > SLEEP) {
        dvizh();
        pacbot1();
        pacbot2();
        a = a + 1;
        tmr5 = millis();
      }
      if (millis() - tmr1 > 200) {
        e2 = !e2;
        tmr1 = millis();
      }
      if (hp != hpst) {
        if (hp == 4) {
          tft.drawBitmap(302, 0, h, 18, 18, 0xFEA0);
          tft.drawBitmap(302, 20, h, 18, 18, ST77XX_RED);
          tft.drawBitmap(302, 40, h, 18, 18, ST77XX_RED);
        } else if (hp == 5) {
          tft.drawBitmap(302, 0, h, 18, 18, 0xFEA0);
          tft.drawBitmap(302, 20, h, 18, 18, 0xFEA0);
          tft.drawBitmap(302, 40, h, 18, 18, ST77XX_RED);
        } else if (hp == 6) {
          tft.drawBitmap(302, 0, h, 18, 18, 0xFEA0);
          tft.drawBitmap(302, 20, h, 18, 18, 0xFEA0);
          tft.drawBitmap(302, 40, h, 18, 18, 0xFEA0);
        } else if (hp == 7) {
          hp = hp - 1;
        } else if (hp == 3) {
          tft.drawBitmap(302, 0, h, 18, 18, ST77XX_RED);
          tft.drawBitmap(302, 20, h, 18, 18, ST77XX_RED);
          tft.drawBitmap(302, 40, h, 18, 18, ST77XX_RED);
        } else if (hp == 2) {
          tft.drawBitmap(302, 0, h, 18, 18, ST77XX_RED);
          tft.drawBitmap(302, 20, h, 18, 18, ST77XX_RED);
          tft.drawBitmap(302, 40, h, 18, 18, ST77XX_BLACK);
        } else if (hp == 1) {
          tft.drawBitmap(302, 0, h, 18, 18, ST77XX_RED);
          tft.drawBitmap(302, 20, h, 18, 18, ST77XX_BLACK);
          tft.drawBitmap(302, 40, h, 18, 18, ST77XX_BLACK);
        } else if (hp < 1) {
          tft.drawBitmap(302, 0, h, 18, 18, ST77XX_BLACK);
          tft.drawBitmap(302, 20, h, 18, 18, ST77XX_BLACK);
          tft.drawBitmap(302, 40, h, 18, 18, ST77XX_BLACK);
          stops();
        }
        hpst = hp;
      }
      if (sst != s) {
        tft.fillRect(295, 208, 25, 20, 0x7BEF);
        tft.setCursor(300, 210);
        tft.setTextColor(ST77XX_WHITE);
        tft.print(s);
        sst = s;
      }
      if (millis() - tmr3 >= 1000) {
        tft.fillRect(295, 228, 25, 10, 0x7BEF);
        tft.setCursor(300, 230);
        tft.setTextColor(ST77XX_RED);
        tft.print(a);
        a = 0;
        tmr3 = millis();
      }
      boost();
    }
  }
  igranach = false;
  tft.fillScreen(COLORSCREEN);
  tft.setCursor(30, 5);
  tft.setTextColor(COLOR1);
  tft.setTextSize(9);
  tft.print(RTCtime.gettime("H i"));
  tmr1 = millis();
  tmrkill3 = millis();
  menuprilozh();
  while (digitalRead(OK_BUTT) == 1) {}
}

void igrasettings() {
  settyst = 100;
  if (proris == true) {
    setty = 0;
    tft.fillScreen(COLORSCREEN);
    tft.setCursor(10, 20);
    tft.setTextColor(COLOR2);
    tft.setTextSize(2);
    tft.print(">time");
    tft.setCursor(10, 44);
    tft.setTextColor(COLOR1);
    tft.print(" sound");
    tft.setCursor(10, 68);
    tft.print(" bright");
    tft.setCursor(10, 92);
    tft.print(" decor");
    tft.setCursor(10, 116);
    tft.print(" rotate");
    tft.setCursor(10, 140);
    tft.print(" reset");
    tft.setCursor(10, 164);
    tft.print(" meteo");
    tft.drawLine(114, 0, 114, 240, COLOR1);
  }
  while (igranach == true) {
    if (digitalRead(RIGHT_BUTT) == 0) {
      settx = settx + 1;
      if (settx > 2) {
        settx = 2;
      }
    } else if (digitalRead(LEFT_BUTT) == 0) {
      settx = settx - 1;
      if (settx < 0) {
        settx = 0;
      }
    }
    if (digitalRead(DOWN_BUTT) == 0) {
      setty = setty + 1;
      if (setty > 6) {
        setty = 6;
      }
    } else if (digitalRead(UP_BUTT) == 0) {
      setty = setty - 1;
      if (setty < 0) {
        setty = 0;
      }
    }
    if (settx == 0) {
      if (setty != settyst) {
        settyst = setty;
        if (setty == 0) {
          tft.setCursor(10, 20);
          tft.setTextColor(COLOR2);
          tft.setTextSize(2);
          tft.print(">time");
          tft.setCursor(10, 44);
          tft.setTextColor(COLORSCREEN);
          tft.print(">");
          tft.setTextColor(COLOR1);
          tft.print("sound");
          tft.fillRect(120, 20, 200, 120, COLORSCREEN);
          str1 = RTCtime.gettime("H");
          str2 = RTCtime.gettime("i");
          timeh = str1.toInt();
          timem = str2.toInt();
          sety = 0;
          tft.setCursor(133, 20);
          tft.setTextSize(6);
          tft.setTextColor(COLOR1);
          if (timeh > 9) {
            tft.print(timeh);
          } else {
            tft.print("0" + String(timeh));
          }
          if (timem > 9) {
            tft.print(":" + String(timem));
          } else {
            tft.print(":0" + String(timem));
          }
        } else if (setty == 1) {
          tft.setCursor(10, 20);
          tft.setTextColor(COLORSCREEN);
          tft.setTextSize(2);
          tft.print(">");
          tft.setTextColor(COLOR1);
          tft.print("time");
          tft.setCursor(10, 44);
          tft.setTextColor(COLOR2);
          tft.print(">sound");
          tft.setCursor(10, 68);
          tft.setTextColor(COLORSCREEN);
          tft.print(">");
          tft.setTextColor(COLOR1);
          tft.print("bright");
          tft.fillRect(120, 20, 200, 220, COLORSCREEN);
          tft.setCursor(143, 20);
          tft.setTextSize(3);
          tft.setTextColor(COLOR1);
          if (zvukon == true) {
            tft.print("ON");
          } else if (zvukon == false) {
            tft.print("OFF");
          }
        } else if (setty == 2) {
          tft.setTextSize(2);
          tft.setCursor(10, 44);
          tft.setTextColor(COLORSCREEN);
          tft.print(">");
          tft.setTextColor(COLOR1);
          tft.print("sound");
          tft.setCursor(10, 68);
          tft.setTextColor(COLOR2);
          tft.print(">bright");
          tft.setCursor(10, 92);
          tft.setTextColor(COLORSCREEN);
          tft.print(">");
          tft.setTextColor(COLOR1);
          tft.print("decor");
          tft.fillRect(120, 20, 200, 220, COLORSCREEN);
          tft.setCursor(133, 20);
          tft.setTextColor(COLOR1);
          tft.print("SCREEN BRIGHT");
          tft.drawRGBBitmap(134, 51, lvl, 20, 80);
          tft.drawRect(133, 50, 22, 82, COLOR1);
          if (bright > 3) {
            tft.setCursor(160, 49);
          } else if (bright == 3) {
            tft.setCursor(160, 69);
          } else if (bright == 2) {
            tft.setCursor(160, 89);
          } else if (bright < 2) {
            tft.setCursor(160, 109);
          }
          tft.print("<");
        } else if (setty == 3) {
          tft.setTextSize(2);
          tft.setCursor(10, 68);
          tft.setTextColor(COLORSCREEN);
          tft.print(">");
          tft.setTextColor(COLOR1);
          tft.print("bright");
          tft.setCursor(10, 92);
          tft.setTextColor(COLOR2);
          tft.print(">decor");
          tft.setCursor(10, 116);
          tft.setTextColor(COLORSCREEN);
          tft.print(">");
          tft.setTextColor(COLOR1);
          tft.print("rotate");
          tft.fillRect(120, 20, 200, 220, COLORSCREEN);
          tft.drawBitmap(120, 20, temaobod, 40, 40, COLOR1);
          tft.drawBitmap(120, 20, tema1, 40, 40, ST77XX_GREEN);
          tft.drawBitmap(120, 20, tema2, 40, 40, 0x915C);
          tft.drawBitmap(120, 20, tema3, 40, 40, 0x0000);
          tft.drawBitmap(170, 20, temaobod, 40, 40, COLOR1);
          tft.drawBitmap(170, 20, tema1, 40, 40, 0xFFFF);
          tft.drawBitmap(170, 20, tema2, 40, 40, ST77XX_BLUE);
          tft.drawBitmap(170, 20, tema3, 40, 40, 0x0000);
          tft.drawBitmap(220, 20, temaobod, 40, 40, COLOR1);
          tft.drawBitmap(220, 20, tema1, 40, 40, 0x0622);
          tft.drawBitmap(220, 20, tema2, 40, 40, 0xFF40);
          tft.drawBitmap(220, 20, tema3, 40, 40, 0x0000);
          tft.drawBitmap(270, 20, temaobod, 40, 40, COLOR1);
          tft.drawBitmap(270, 20, tema1, 40, 40, 0xFFFF);
          tft.drawBitmap(270, 20, tema2, 40, 40, ST77XX_RED);
          tft.drawBitmap(270, 20, tema3, 40, 40, 0x0000);
        } else if (setty == 5) {
          tft.setTextSize(2);
          tft.setCursor(10, 116);
          tft.setTextColor(COLORSCREEN);
          tft.print(">");
          tft.setTextColor(COLOR1);
          tft.print("rotate");
          tft.setCursor(10, 140);
          tft.setTextColor(COLOR2);
          tft.print(">reset");
          tft.setCursor(10, 164);
          tft.setTextColor(COLORSCREEN);
          tft.print(">");
          tft.setTextColor(COLOR1);
          tft.print("meteo");
          tft.fillRect(120, 20, 200, 220, COLORSCREEN);
          tft.setCursor(133, 20);
          tft.setTextColor(COLOR1);
          tft.print("REKORDS");
          tft.setCursor(133, 40);
          tft.print("SETTINGS");
          tft.setCursor(133, 60);
          tft.print("FULL RESET");
        } else if (setty == 6) {
          tft.setTextSize(2);
          tft.setCursor(10, 140);
          tft.setTextColor(COLORSCREEN);
          tft.print(">");
          tft.setTextColor(COLOR1);
          tft.print("reset");
          tft.setCursor(10, 164);
          tft.setTextColor(COLOR2);
          tft.print(">meteo");
          tft.fillRect(120, 20, 200, 220, COLORSCREEN);
          tft.setCursor(133, 20);
          tft.setTextColor(COLOR1);
          tft.print("GO TO METEO");
          tft.setCursor(133, 40);
          tft.print("MODE");
        } else if (setty == 4) {
          tft.setTextSize(2);
          tft.setCursor(10, 92);
          tft.setTextColor(COLORSCREEN);
          tft.print(">");
          tft.setTextColor(COLOR1);
          tft.print("decor");
          tft.setCursor(10, 116);
          tft.setTextColor(COLOR2);
          tft.print(">rotate");
          tft.setCursor(10, 140);
          tft.setTextColor(COLORSCREEN);
          tft.print(">");
          tft.setTextColor(COLOR1);
          tft.print("reset");
          tft.fillRect(120, 20, 200, 220, COLORSCREEN);
          tft.setCursor(133, 20);
          tft.setTextColor(COLOR1);
          tft.print("ROTATE SKREEN");
          tft.drawBitmap(133, 40, up, 10, 14, COLOR1);
          tft.setTextColor(COLOR1);
          tft.setCursor(143, 40);
          tft.print(" UP");
        }
        while (digitalRead(UP_BUTT) == 0 || digitalRead(DOWN_BUTT) == 0) {}
        delay(100);
      }
    } else if (settx == 1) {
      if (setty == 0) {
        while (digitalRead(LEFT_BUTT) == 0 || digitalRead(RIGHT_BUTT) == 0) {}
        timeset();
      } else if (setty == 1) {
        while (digitalRead(LEFT_BUTT) == 0 || digitalRead(RIGHT_BUTT) == 0) {}
        soundset();
      } else if (setty == 2) {
        while (digitalRead(LEFT_BUTT) == 0 || digitalRead(RIGHT_BUTT) == 0) {}
        brightset();
      } else if (setty == 3) {
        while (digitalRead(LEFT_BUTT) == 0 || digitalRead(RIGHT_BUTT) == 0) {}
        decor();
      } else if (setty == 4) {
        while (digitalRead(LEFT_BUTT) == 0 || digitalRead(RIGHT_BUTT) == 0) {}
        rotateset();
      } else if (setty == 5) {
        while (digitalRead(LEFT_BUTT) == 0 || digitalRead(RIGHT_BUTT) == 0) {}
        resetskset();
      } else if (setty == 6) {
        while (digitalRead(LEFT_BUTT) == 0 || digitalRead(RIGHT_BUTT) == 0) {}
        igrasleep();
      }
    }
    if (digitalRead(HOME_BUTT) == 1) {
      zvukbool = true;
      zv = 2;
      tmrzvuk = millis();
      zvuk();
      igranach = false;
      break;
    }
  }
  proris = true;
  settx = 0;
  setty = 0;
  tft.fillScreen(COLORSCREEN);
  tft.setCursor(30, 5);
  tft.setTextColor(COLOR1);
  tft.setTextSize(9);
  tft.print(RTCtime.gettime("H i"));
  tmr1 = millis();
  tmrkill3 = millis();
  menuprilozhy1();
}

void igrasleep() {
  tft.fillScreen(0x0000);
  setnach = true;
  tft.fillRect(0, 0, 320, 5, COLOR2);
  tft.fillRect(0, 235, 320, 5, COLOR2);
  tft.fillRect(0, 0, 5, 240, COLOR2);
  tft.fillRect(315, 0, 5, 240, COLOR2);
  tft.fillRect(0, 110, 320, 5, COLOR2);
  tft.fillRect(105, 110, 5, 210, COLOR2);
  tft.fillRect(210, 110, 5, 210, COLOR2);
  tft.fillRect(1, 1, 318, 3, COLORSCREEN);
  tft.fillRect(1, 236, 318, 3, COLORSCREEN);
  tft.fillRect(1, 1, 3, 238, COLORSCREEN);
  tft.fillRect(316, 1, 3, 238, COLORSCREEN);
  tft.fillRect(1, 111, 318, 3, COLORSCREEN);
  tft.fillRect(106, 111, 3, 128, COLORSCREEN);
  tft.fillRect(211, 111, 3, 128, COLORSCREEN);
  tft.setCursor(30, 26);
  tft.setTextColor(COLOR1);
  tft.setTextSize(9);
  tft.print(RTCtime.gettime("H:i"));
  tft.setTextSize(1);
  tft.setCursor(23, 125);
  tft.print("Temperature");
  tft.setCursor(133, 125);
  tft.print("Humidity");
  tft.setCursor(238, 125);
  tft.print("Pressure");
  tft.setCursor(80, 210);
  tft.print("o");
  tft.setTextSize(2);
  tft.setCursor(88, 215);
  tft.print("C");
  tft.setCursor(190, 215);
  tft.print("%");
  tft.setCursor(250, 215);
  tft.print("mm Hg");
  tft.setTextSize(4);
  tft.setCursor(33, 160);
  x = bme.readTemperature() - 1;
  //x = (bmp.readTemperature() + sensor.readTemperature()) / 2;
  tft.print(x);
  tft.setCursor(231, 160);
  x = bme.readPressure() / 133;
  //x = bmp.readPressure() / 133;
  tft.print(x);
  tft.setCursor(138, 160);
  x = bme.readHumidity() + 3;
  //x = sensor.readHumidity();
  tft.print(x);
  temp = bme.readTemperature();
  pres = bme.readPressure() / 133;
  higr = bme.readHumidity();
  str2 = RTCtime.gettime("i");
  m = str2.toInt();
  tmr1 = millis();
  while (digitalRead(LEFT_BUTT) == 1 && digitalRead(RIGHT_BUTT) == 1 && digitalRead(UP_BUTT) == 1 && digitalRead(DOWN_BUTT) == 1 && digitalRead(OK_BUTT) == 0 && digitalRead(HOME_BUTT) == 0) {
    if (millis() - tmr1 >= 15000) {
      tft.setTextColor(COLOR1);
      tft.setTextSize(4);
      x = bme.readTemperature() - 1;
      if (temp != x) {
        temp = x;
        tft.fillRect(33, 160, 44, 28, COLORSCREEN);
        tft.setCursor(33, 160);
        tft.print(x);
      }
      x = bme.readPressure() / 133;
      if (pres != x) {
        pres = x;
        tft.fillRect(231, 160, 68, 28, COLORSCREEN);
        tft.setCursor(231, 160);
        tft.print(x);
      }
      x = bme.readHumidity() + 3;
      if (higr != x) {
        higr = x;
        tft.fillRect(138, 160, 44, 28, COLORSCREEN);
        tft.setCursor(138, 160);
        tft.print(x);
      }
      str2 = RTCtime.gettime("i");
      timem = str2.toInt();
      if (m != timem) {
        str2 = RTCtime.gettime("i");
        m = str2.toInt();
        tft.fillRect(30, 26, 261, 63, COLORSCREEN);
        tft.setCursor(30, 26);
        tft.setTextColor(COLOR1);
        tft.setTextSize(9);
        tft.print(RTCtime.gettime("H i"));
      }
      tmr1 = millis();
    }
    if (millis() - tmr2 >= 1000) {
      flag = !flag;
      if (flag == false) {
        tft.setCursor(138, 26);
        tft.setTextColor(COLOR1);
        tft.setTextSize(9);
        tft.print(":");
      } else {
        tft.setCursor(138, 26);
        tft.setTextColor(COLORSCREEN);
        tft.setTextSize(9);
        tft.print(":");
      }
      tmr2 = millis();
    }
  }
  while (digitalRead(LEFT_BUTT) == 0 || digitalRead(RIGHT_BUTT) == 0 || digitalRead(UP_BUTT) == 0 || digitalRead(DOWN_BUTT) == 0 || digitalRead(OK_BUTT) == 1 || digitalRead(HOME_BUTT) == 1) {}
  tft.fillScreen(COLORSCREEN);
  tft.setCursor(30, 5);
  tft.setTextColor(COLOR1);
  tft.setTextSize(9);
  tft.print(RTCtime.gettime("H i"));
  tmr1 = millis();
  tmrkill3 = millis();
  if (iy == 0) {
    menuprilozh();
  } else {
    if (igranach == false) {
      menuprilozhy1();
    }
  }
  igranach = false;
}

void rotateset() {
  xbot1 = rotat;
  tft.setTextColor(COLOR2);
  tft.setTextSize(2);
  tft.setCursor(143, 20);
  rotat = pref.getUChar("rotate", 3);
  tft.drawBitmap(133, 40, up, 10, 14, COLOR2);
  tft.setTextColor(COLOR2);
  tft.setCursor(143, 40);
  tft.print(" UP");
  setnach = true;
  while (setnach == true) {
    if (digitalRead(LEFT_BUTT) == 0 && sety == 0) {
      setnach = false;
      proris = false;
      settx = 0;
      settyst = 50;
    }
    if (digitalRead(DOWN_BUTT) == 0 || digitalRead(UP_BUTT) == 0) {
      xbot1 = xbot1 + 2;
      if (xbot1 > 3) {
        xbot1 = 1;
      }
      if (rotat == 1) {
        if (xbot1 == 1) {
          tft.fillRect(133, 40, 10, 20, COLORSCREEN);
          tft.drawBitmap(133, 40, up, 10, 14, COLOR2);
        } else {
          tft.fillRect(133, 40, 10, 20, COLORSCREEN);
          tft.drawBitmap(133, 40, down, 10, 14, COLOR2);
        }
      } else {
        if (xbot1 == 1) {
          tft.fillRect(133, 40, 10, 20, COLORSCREEN);
          tft.drawBitmap(133, 40, down, 10, 14, COLOR2);
        } else {
          tft.fillRect(133, 40, 10, 20, COLORSCREEN);
          tft.drawBitmap(133, 40, up, 10, 14, COLOR2);
        }
      }
      while (digitalRead(DOWN_BUTT) == 0 || digitalRead(UP_BUTT) == 0) {}
      delay(100);
    }
    if (digitalRead(HOME_BUTT) == 1) {
      zvukbool = true;
      zv = 2;
      tmrzvuk = millis();
      zvuk();
      setnach = false;
      settx = 0;
      igranach = false;
    }
    if (digitalRead(OK_BUTT) == 1) {
      pref.putUChar("rotate", xbot1);
      rotat = xbot1;
      zvukbool = true;
      zv = 2;
      tmrzvuk = millis();
      zvuk();
      pref.putUChar("zvukon", zvukon);
      tft.setCursor(120, 190);
      tft.setTextSize(2);
      tft.setTextColor(COLOR1);
      tft.print("Complate");
      delay(1000);
      tft.setCursor(120, 190);
      tft.setTextColor(COLORSCREEN);
      tft.print("Complate");
      tft.setTextSize(3);
      pref.putUChar("kuda", 3);
      delay(1000);
      res();
    }
  }
}

void timeset() {
  str1 = RTCtime.gettime("H");
  str2 = RTCtime.gettime("i");
  timeh = str1.toInt();
  timem = str2.toInt();
  sety = 0;
  tft.setCursor(133, 20);
  tft.setTextSize(6);
  tft.setTextColor(COLOR2);
  if (timeh > 9) {
    tft.print(timeh);
  } else {
    tft.print("0" + String(timeh));
  }
  tft.setTextColor(COLOR1);
  if (timem > 9) {
    tft.print(":" + String(timem));
  } else {
    tft.print(":0" + String(timem));
  }
  tft.setTextColor(COLOR2);
  tft.setCursor(151, 70);
  tft.print("^");
  setnach = true;
  while (setnach == true) {
    if (digitalRead(LEFT_BUTT) == 0 && sety == 0) {
      settx = 0;
      setnach = false;
      proris = false;
      while (digitalRead(LEFT_BUTT) == 0) {}
    } else if (digitalRead(LEFT_BUTT) == 0) {
      sety = 0;
      tft.setCursor(133, 20);
      tft.setTextColor(COLOR2);
      if (timeh > 9) {
        tft.print(timeh);
      } else {
        tft.print("0" + String(timeh));
      }
      tft.setTextColor(COLOR1);
      if (timem > 9) {
        tft.print(" " + String(timem));
      } else {
        tft.print(" 0" + String(timem));
      }
      tft.setTextColor(COLOR2);
      tft.setCursor(151, 70);
      tft.print("^");
      tft.setTextColor(COLORSCREEN);
      tft.setCursor(259, 70);
      tft.print("^");
      while (digitalRead(LEFT_BUTT) == 0) {}
    } else if (digitalRead(RIGHT_BUTT) == 0) {
      sety = 1;
      tft.setCursor(133, 20);
      tft.setTextColor(COLOR1);
      if (timeh > 9) {
        tft.print(timeh);
      } else {
        tft.print("0" + String(timeh));
      }
      tft.setTextColor(COLOR2);
      if (timem > 9) {
        tft.print(" " + String(timem));
      } else {
        tft.print(" 0" + String(timem));
      }
      tft.setCursor(259, 70);
      tft.print("^");
      tft.setTextColor(COLORSCREEN);
      tft.setCursor(151, 70);
      tft.print("^");
      while (digitalRead(RIGHT_BUTT) == 0) {}
    }
    if (digitalRead(UP_BUTT) == 0) {
      if (sety == 0) {
        timeh = timeh + 1;
        tft.setTextColor(COLOR2);
        if (timeh > 23) {
          timeh = 0;
        }
        tft.fillRect(133, 20, 66, 42, COLORSCREEN);
        if (timeh > 9) {
          tft.setCursor(133, 20);
          tft.print(timeh);
        } else {
          tft.setCursor(133, 20);
          tft.print("0" + String(timeh));
        }
        delay(200);
      } else if (sety == 1) {
        tft.setTextColor(COLOR2);
        timem = timem + 1;
        delay(200);
        if (timem > 59) {
          timem = 0;
        }
        tft.fillRect(241, 20, 66, 42, COLORSCREEN);
        if (timem > 9) {
          tft.setCursor(241, 20);
          tft.print(timem);
        } else {
          tft.setCursor(241, 20);
          tft.print("0" + String(timem));
        }
      }
    } else if (digitalRead(DOWN_BUTT) == 0) {
      if (sety == 0) {
        timeh = timeh - 1;
        tft.setTextColor(COLOR2);
        if (timeh < 0) {
          timeh = 23;
        }
        tft.fillRect(133, 20, 66, 42, COLORSCREEN);
        if (timeh > 9) {
          tft.setCursor(133, 20);
          tft.print(timeh);
        } else {
          tft.setCursor(133, 20);
          tft.print("0" + String(timeh));
        }
        delay(200);
      } else if (sety == 1) {
        tft.setTextColor(COLOR2);
        timem = timem - 1;
        delay(200);
        if (timem < 0) {
          timem = 59;
        }
        tft.fillRect(241, 20, 66, 42, COLORSCREEN);
        if (timem > 9) {
          tft.setCursor(241, 20);
          tft.print(timem);
        } else {
          tft.setCursor(241, 20);
          tft.print("0" + String(timem));
        }
      }
    }
    if (digitalRead(OK_BUTT) == 1) {
      zvukbool = true;
      zv = 2;
      tmrzvuk = millis();
      zvuk();
      RTCtime.settime(0, timem, timeh, 0, 0, 0, 1);
      tft.setTextSize(2);
      tft.setCursor(120, 190);
      tft.setTextColor(COLOR1);
      tft.print("Complate");
      delay(1000);
      tft.setCursor(120, 190);
      tft.setTextColor(COLORSCREEN);
      tft.print("Complate");
      tft.setTextSize(6);
    }
    if (digitalRead(HOME_BUTT) == 1) {
      zvukbool = true;
      zv = 2;
      tmrzvuk = millis();
      zvuk();
      setnach = false;
      settx = 0;
      igranach = false;
    }
  }
  if (igranach == true) {
    tft.setCursor(133, 20);
    tft.setTextColor(COLOR1);
    if (timeh > 9) {
      tft.print(timeh);
    } else {
      tft.print("0" + String(timeh));
    }
    tft.setTextColor(COLORSCREEN);
    tft.setCursor(151, 70);
    tft.print("^");
  }
}

void soundset() {
  tft.setTextColor(COLOR2);
  tft.setTextSize(3);
  tft.setCursor(143, 20);
  if (zvukon == true) {
    tft.print("ON");
    zvuky = 1;
  } else if (zvukon == false) {
    tft.print("OFF");
    zvuky = 0;
  }
  setnach = true;
  while (setnach == true) {
    if (digitalRead(LEFT_BUTT) == 0 && sety == 0) {
      setnach = false;
      proris = false;
      settx = 0;
      settyst = 50;
    }
    if (digitalRead(DOWN_BUTT) == 0 || digitalRead(UP_BUTT) == 0) {
      zvuky = zvuky + 1;
      if (zvuky > 1) {
        zvuky = 0;
      }
      if (zvuky == 1) {
        tft.setTextColor(COLORSCREEN);
        tft.setCursor(143, 20);
        tft.print("OFF");
        tft.setTextColor(COLOR2);
        tft.setCursor(143, 20);
        tft.print("ON");
      } else {
        tft.setTextColor(COLORSCREEN);
        tft.setCursor(143, 20);
        tft.print("ON");
        tft.setTextColor(COLOR2);
        tft.setCursor(143, 20);
        tft.print("OFF");
      }
      while (digitalRead(DOWN_BUTT) == 0 || digitalRead(UP_BUTT) == 0) {}
      delay(100);
    }
    if (digitalRead(HOME_BUTT) == 1) {
      zvukbool = true;
      zv = 2;
      tmrzvuk = millis();
      zvuk();
      setnach = false;
      settx = 0;
      igranach = false;
    }
    if (digitalRead(OK_BUTT) == 1) {
      if (zvuky == 1) {
        zvukon = true;
      } else if (zvuky == 0) {
        zvukon = false;
      }
      zvukbool = true;
      zv = 2;
      tmrzvuk = millis();
      zvuk();
      pref.putUChar("zvukon", zvukon);
      tft.setCursor(120, 190);
      tft.setTextSize(2);
      tft.setTextColor(COLOR1);
      tft.print("Complate");
      delay(1000);
      tft.setCursor(120, 190);
      tft.setTextColor(COLORSCREEN);
      tft.print("Complate");
      tft.setTextSize(3);
    }
  }
}

void brightset() {
  tft.setTextColor(COLOR2);
  tft.setTextSize(2);
  if (bright > 3) {
    tft.setCursor(160, 49);
  } else if (bright == 3) {
    tft.setCursor(160, 69);
  } else if (bright == 2) {
    tft.setCursor(160, 89);
  } else if (bright < 2) {
    tft.setCursor(160, 109);
  }
  tft.print("<");
  setnach = true;
  xbot1 = bright;
  xbotst1 = xbot1;
  while (setnach == true) {
    if (digitalRead(LEFT_BUTT) == 0 && sety == 0) {
      if (bright > 3) {
        a = 0;
      } else if (bright == 3) {
        a = 69;
      } else if (bright == 2) {
        a = 70;
      } else if (bright < 2) {
        a = 71;
      }
      rt.setRight(a);
      setnach = false;
      proris = false;
      settx = 0;
      settyst = 50;
    }
    if (digitalRead(UP_BUTT) == 0 || digitalRead(DOWN_BUTT) == 0) {
      if (digitalRead(UP_BUTT) == 0) {
        xbot1 = xbot1 + 1;
        if (xbot1 > 4) {
          xbot1 = 4;
        }
      }
      if (digitalRead(DOWN_BUTT) == 0) {
        xbot1 = xbot1 - 1;
        if (xbot1 < 1) {
          xbot1 = 1;
        }
      }
      tft.setTextColor(COLORSCREEN);
      if (xbot1 > 3) {
        tft.setCursor(160, 69);
      } else if (xbot1 == 3) {
        tft.setCursor(160, 49);
        tft.print("<");
        tft.setCursor(160, 89);
      } else if (xbot1 == 2) {
        tft.setCursor(160, 69);
        tft.print("<");
        tft.setCursor(160, 109);
      } else if (xbot1 < 2) {
        tft.setCursor(160, 89);
      }
      tft.print("<");
      if (xbot1 != xbotst1) {
        xbotst1 = xbot1;
        if (xbot1 > 3) {
          a = 0;
          tft.setCursor(160, 49);
        } else if (xbot1 == 3) {
          a = 69;
          tft.setCursor(160, 69);
        } else if (xbot1 == 2) {
          a = 70;
          tft.setCursor(160, 89);
        } else if (xbot1 < 2) {
          a = 71;
          tft.setCursor(160, 109);
        }
        tft.setTextColor(COLOR2);
        tft.print("<");
        rt.setRight(a);
      }
      while (digitalRead(DOWN_BUTT) == 0 || digitalRead(UP_BUTT) == 0) {}
      delay(100);
    }
    if (digitalRead(HOME_BUTT) == 1) {
      if (bright > 3) {
        a = 0;
      } else if (bright == 3) {
        a = 69;
      } else if (bright == 2) {
        a = 70;
      } else if (bright < 2) {
        a = 71;
      }
      rt.setRight(a);
      zvukbool = true;
      zv = 2;
      tmrzvuk = millis();
      zvuk();
      setnach = false;
      settx = 0;
      igranach = false;
    }
    if (digitalRead(OK_BUTT) == 1) {
      pref.putUChar("bright", xbot1);
      bright = xbot1;
      tft.setCursor(120, 190);
      tft.setTextSize(2);
      tft.setTextColor(COLOR1);
      tft.print("Complate");
      delay(1000);
      tft.setCursor(120, 190);
      tft.setTextColor(COLORSCREEN);
      tft.print("Complate");
    }
  }
}

void decor() {
  tft.drawBitmap(120, 20, temaobod, 40, 40, COLOR2);
  setnach = true;
  while (setnach == true) {
    if (digitalRead(LEFT_BUTT) == 0) {
      sety = sety - 1;
      if (sety < 0) {
        sety = 0;
      }
    } else if (digitalRead(RIGHT_BUTT) == 0) {
      sety = sety + 1;
      if (sety > 3) {
        sety = 3;
      }
    }
    if (sety != setyst) {
      setyst = sety;
      if (sety == 0) {
        tft.drawBitmap(120, 20, temaobod, 40, 40, COLOR2);
        tft.drawBitmap(170, 20, temaobod, 40, 40, COLOR1);
        tft.drawBitmap(220, 20, temaobod, 40, 40, COLOR1);
        tft.drawBitmap(270, 20, temaobod, 40, 40, COLOR1);
      } else if (sety == 1) {
        tft.drawBitmap(120, 20, temaobod, 40, 40, COLOR1);
        tft.drawBitmap(170, 20, temaobod, 40, 40, COLOR2);
        tft.drawBitmap(220, 20, temaobod, 40, 40, COLOR1);
        tft.drawBitmap(270, 20, temaobod, 40, 40, COLOR1);
      } else if (sety == 2) {
        tft.drawBitmap(120, 20, temaobod, 40, 40, COLOR1);
        tft.drawBitmap(170, 20, temaobod, 40, 40, COLOR1);
        tft.drawBitmap(220, 20, temaobod, 40, 40, COLOR2);
        tft.drawBitmap(270, 20, temaobod, 40, 40, COLOR1);
      } else if (sety == 3) {
        tft.drawBitmap(120, 20, temaobod, 40, 40, COLOR1);
        tft.drawBitmap(170, 20, temaobod, 40, 40, COLOR1);
        tft.drawBitmap(220, 20, temaobod, 40, 40, COLOR1);
        tft.drawBitmap(270, 20, temaobod, 40, 40, COLOR2);
      }
      while (digitalRead(LEFT_BUTT) == 0 || digitalRead(RIGHT_BUTT) == 0) {}
    }
    if (digitalRead(LEFT_BUTT) == 0 && sety == 0) {
      setnach = false;
      proris = false;
      settx = 0;
      settyst = 50;
    }
    if (digitalRead(HOME_BUTT) == 1) {
      zvukbool = true;
      zv = 2;
      tmrzvuk = millis();
      zvuk();
      setnach = false;
      settx = 0;
      igranach = false;
    }
    if (digitalRead(OK_BUTT) == 1) {
      zvukbool = true;
      zv = 2;
      tmrzvuk = millis();
      zvuk();
      pref.putUChar("forTema", sety);
      tft.setCursor(120, 190);
      tft.setTextColor(COLOR1);
      tft.setTextSize(2);
      tft.print("Complate");
      pref.putUChar("kuda", 1);
      delay(1000);
      res();
    }
  }
}

void resetskset() {
  sety = 0;
  tft.setCursor(133, 20);
  tft.setTextColor(COLOR2);
  tft.print("REKORDS");
  tft.setCursor(133, 40);
  tft.setTextColor(COLOR1);
  tft.print("SETTINGS");
  while (true) {
    if (digitalRead(DOWN_BUTT) == 0) {
      sety = sety + 1;
      if (sety > 2) {
        sety = 2;
      }
      while (digitalRead(DOWN_BUTT) == 0) {}
    }
    if (digitalRead(UP_BUTT) == 0) {
      sety = sety - 1;
      if (sety < 0) {
        sety = 0;
      }
      while (digitalRead(UP_BUTT) == 0) {}
    }
    if (sety != setyst) {
      setyst = sety;
      if (sety == 0) {
        tft.setCursor(133, 20);
        tft.setTextColor(COLOR2);
        tft.print("REKORDS");
        tft.setCursor(133, 40);
        tft.setTextColor(COLOR1);
        tft.print("SETTINGS");
        tft.setCursor(133, 60);
        tft.setTextColor(COLOR1);
        tft.print("FULL RESET");
      } else if (sety == 1) {
        tft.setCursor(133, 20);
        tft.setTextColor(COLOR1);
        tft.print("REKORDS");
        tft.setCursor(133, 40);
        tft.setTextColor(COLOR2);
        tft.print("SETTINGS");
        tft.setCursor(133, 60);
        tft.setTextColor(COLOR1);
        tft.print("FULL RESET");
      } else if (sety == 2) {
        tft.setCursor(133, 20);
        tft.setTextColor(COLOR1);
        tft.print("REKORDS");
        tft.setCursor(133, 40);
        tft.print("SETTINGS");
        tft.setCursor(133, 60);
        tft.setTextColor(COLOR2);
        tft.print("FULL RESET");
      }
    }
    if (digitalRead(OK_BUTT) == 1) {
      if (sety == 0) {
        pref.putUInt("skoreBS", 0);
        pref.putUInt("skoreFB", 0);
        pref.putUInt("skoreSB", 0);
        pref.putUInt("skorePM", 0);
        pref.putUInt("skoreDJ", 0);
        pref.putUInt("skoreS", 0);
        pref.putUInt("skoreDJ2", 0);
        pref.putUInt("skoreT", 0);
        tft.setTextSize(2);
        tft.setCursor(120, 190);
        tft.setTextColor(COLOR1);
        tft.print("Complate");
        delay(1000);
        tft.setCursor(120, 190);
        tft.setTextColor(COLORSCREEN);
        tft.print("Complate");
      } else if (sety == 1) {
        pref.putUChar("forTema", 0);
        pref.putUChar("kuda", 2);
        pref.putUChar("botsBS", 3);
        pref.putUChar("botsPM", 2);
        pref.putUChar("zvukon", 1);
        pref.putUChar("rotate", 1);
        tft.setTextSize(2);
        tft.setCursor(120, 190);
        tft.setTextColor(COLOR1);
        tft.print("Complate");
        delay(1000);
        res();
      } else if (sety == 2) {
        pref.clear();
        tft.setTextSize(2);
        tft.setCursor(120, 190);
        tft.setTextColor(COLOR1);
        tft.print("Complate");
        delay(1000);
        res();
      }
    }
    if (digitalRead(HOME_BUTT) == 1) {
      settx = 0;
      igranach = false;
      break;
    }
    if (digitalRead(LEFT_BUTT) == 0) {
      proris = false;
      tft.setTextSize(2);
      tft.setCursor(10, 140);
      tft.setTextColor(COLOR2);
      tft.print(">reset");
      settyst = 50;
      break;
    }
  }
}

void nachalo() {
  sp = 0;
  csp = 0;
  i = 0;
  while (sp < 10) {
    while (csp < 12) {
      if (fm == 0) {
        forMapBS[sp][csp] = map1BS[i];
      } else if (fm == 1) {
        forMapBS[sp][csp] = map2BS[i];
      } else {
        forMapBS[sp][csp] = map3BS[i];
      }
      i = i + 1;
      csp = csp + 1;
    }
    csp = 0;
    sp = sp + 1;
  }
  tft.fillScreen(ST77XX_BLACK);
  bam = false;
  n = 4;
  hp = 3;
  bambot1 = false;
  bambot2 = false;
  bambot3 = false;
  nbot1 = 4;
  nbot2 = 4;
  nbot3 = 4;
  karta();
}

void karta() {
  for (int i = 0; i < 10; i++) {
    sy = i;
    for (int a = 0; a < 12; a++) {
      sx = a;
      xys = forMapBS[sy][sx];
      if (xys == 0) {} else if (xys == 1) {
        tft.fillRect(sx * 24, sy * 24, 24, 24, 0xC618);
        tft.drawBitmap(sx * 24, sy * 24, b_1, 24, 24, 0x7BEF);
      } else if (xys == 2) {
        tft.fillRect(sx * 24, sy * 24, 24, 24, ST77XX_ORANGE);
        tft.drawBitmap(sx * 24, sy * 24, b_2, 24, 24, 0xC618);
      } else if (xys == 3) {
        tft.fillRect(sx * 24, sy * 24, 24, 24, 0x03EF);
        tft.drawBitmap(sx * 24, sy * 24, b_3, 24, 24, 0xFFFF);
      } else if (xys == 4) {
        tft.fillRect(sx * 24, sy * 24, 24, 24, ST77XX_ORANGE);
        tft.drawBitmap(sx * 24, sy * 24, b_2, 24, 24, 0xC618);
        tft.drawBitmap(sx * 24, sy * 24, r_1, 24, 24, ST77XX_BLACK);
      } else if (xys == 5) {
        tft.fillRect(sx * 24, sy * 24, 24, 24, ST77XX_ORANGE);
        tft.drawBitmap(sx * 24, sy * 24, b_2, 24, 24, 0xC618);
        tft.drawBitmap(sx * 24, sy * 24, r_2, 24, 24, ST77XX_BLACK);
      } else if (xys == 6) {
        tft.fillRect(sx * 24, sy * 24, 24, 24, ST77XX_ORANGE);
        tft.drawBitmap(sx * 24, sy * 24, b_2, 24, 24, 0xC618);
        tft.drawBitmap(sx * 24, sy * 24, r_3, 24, 24, ST77XX_BLACK);
      } else if (xys == -1) {
        x = sx * 24;
        y = sy * 24;
        tft.drawBitmap(x, y, t1n4, 24, 24, ST77XX_ORANGE);
        tft.drawBitmap(x, y, t2n4, 24, 24, 0x915C);
      } else if (xys == -2) {
        xbot1 = sx * 24;
        ybot1 = sy * 24;
        xbotst1 = sx * 24;
        ybotst1 = sy * 24;
        nbot1 = 4;
        bambot1 = false;
        xbambot1 = 600;
        ybambot1 = 600;
        tft.drawBitmap(xbot1, ybot1, t1n4, 24, 24, ST77XX_ORANGE);
        tft.drawBitmap(xbot1, ybot1, t2n4, 24, 24, 0x9A60);
      } else if (xys == -3) {
        xbot2 = sx * 24;
        ybot2 = sy * 24;
        xbotst2 = sx * 24;
        ybotst2 = sy * 24;
        nbot2 = 4;
        bambot2 = false;
        xbambot2 = 700;
        ybambot2 = 700;
        tft.drawBitmap(xbot2, ybot2, t1n4, 24, 24, ST77XX_ORANGE);
        tft.drawBitmap(xbot2, ybot2, t2n4, 24, 24, 0x9A60);
      } else if (xys == -4) {
        xbot3 = sx * 24;
        ybot3 = sy * 24;
        xbotst3 = sx * 24;
        ybotst3 = sy * 24;
        nbot3 = 4;
        bambot3 = false;
        xbambot3 = 800;
        ybambot3 = 800;
        tft.drawBitmap(xbot3, ybot3, t1n4, 24, 24, ST77XX_ORANGE);
        tft.drawBitmap(xbot3, ybot3, t2n4, 24, 24, 0x9A60);
      }
    }
  }
  tft.fillRect(288, 0, 32, 240, 0xC618);
  tft.drawBitmap(295, 60, h, 18, 18, ST77XX_RED);
  tft.drawBitmap(295, 80, h, 18, 18, ST77XX_RED);
  tft.drawBitmap(295, 100, h, 18, 18, ST77XX_RED);
  while (digitalRead(LEFT_BUTT) == 1 && digitalRead(RIGHT_BUTT) == 1 && digitalRead(DOWN_BUTT) == 1 && digitalRead(UP_BUTT) == 1 && digitalRead(OK_BUTT) == 0 && digitalRead(HOME_BUTT) == 0) {}
}

void popal() {
  //я по ботам
  if (xbam > xbot1 - 4 && ybam > ybot1 - 4 && xbam < xbot1 + 24 && ybam < ybot1 + 24 && millis() - tmrkill1 > 2000) {
    tft.drawBitmap(xbam, ybam, bammap, 4, 4, ST77XX_BLACK);
    tft.drawRGBBitmap(xbot1, ybot1, bah, 24, 24);
    kill1 = true;
    s = s + 1;
    bam = false;
    xbam = 500;
    ybam = 500;
    tmr5 = millis() + 10;
  }
  if (xbam > xbot2 - 4 && ybam > ybot2 - 4 && xbam < xbot2 + 24 && ybam < ybot2 + 24 && millis() - tmrkill2 > 2000) {
    tft.drawBitmap(xbam, ybam, bammap, 4, 4, ST77XX_BLACK);
    tft.drawRGBBitmap(xbot2, ybot2, bah, 24, 24);
    kill2 = true;
    s = s + 1;
    bam = false;
    xbam = 500;
    ybam = 500;
    tmr5 = millis() + 10;
  }
  if (xbam > xbot3 - 4 && ybam > ybot3 - 4 && xbam < xbot3 + 24 && ybam < ybot3 + 24 && millis() - tmrkill3 > 2000) {
    tft.drawBitmap(xbam, ybam, bammap, 4, 4, ST77XX_BLACK);
    tft.drawRGBBitmap(xbot3, ybot3, bah, 24, 24);
    kill3 = true;
    s = s + 1;
    bam = false;
    xbam = 500;
    ybam = 500;
    tmr5 = millis() + 10;
  }
  //боты по мне
  if (xbambot1 > x - 4 && ybambot1 > y - 4 && xbambot1 < x + 24 && ybambot1 < y + 24) {
    tft.drawBitmap(xbambot1, ybambot1, bammap, 4, 4, ST77XX_BLACK);
    ybambot1 = 600;
    xbambot1 = 600;
    bambot1 = false;
    tmrbot1 = millis();
    hp = hp - 1;
  }
  if (xbambot2 > x - 4 && ybambot2 > y - 4 && xbambot2 < x + 24 && ybambot2 < y + 24) {
    tft.drawBitmap(xbambot2, ybambot2, bammap, 4, 4, ST77XX_BLACK);
    ybambot2 = 700;
    xbambot2 = 700;
    bambot2 = false;
    tmrbot2 = millis();
    hp = hp - 1;
  }
  if (xbambot3 > x - 4 && ybambot3 > y - 4 && xbambot3 < x + 24 && ybambot3 < y + 24) {
    tft.drawBitmap(xbambot3, ybambot3, bammap, 4, 4, ST77XX_BLACK);
    ybambot3 = 800;
    xbambot3 = 800;
    bambot3 = false;
    tmrbot3 = millis();
    hp = hp - 1;
  }
  //бот по боту
  if (xbambot1 > xbot2 - 4 && ybambot1 > ybot2 - 4 && xbambot1 < xbot2 + 24 && ybambot1 < ybot2 + 24) {
    tft.drawBitmap(xbambot1, ybambot1, bammap, 4, 4, ST77XX_BLACK);
    ybambot1 = 600;
    xbambot1 = 600;
    bambot1 = false;
    tmrbot1 = millis();
  }
  if (xbambot2 > xbot1 - 4 && ybambot2 > ybot1 - 4 && xbambot2 < xbot1 + 24 && ybambot2 < ybot1 + 24) {
    tft.drawBitmap(xbambot2, ybambot2, bammap, 4, 4, ST77XX_BLACK);
    ybambot2 = 700;
    xbambot2 = 700;
    bambot2 = false;
    tmrbot2 = millis();
  }
  if (xbambot3 > xbot1 - 4 && ybambot3 > ybot1 - 4 && xbambot3 < xbot1 + 24 && ybambot3 < ybot1 + 24) {
    tft.drawBitmap(xbambot3, ybambot3, bammap, 4, 4, ST77XX_BLACK);
    ybambot3 = 800;
    xbambot3 = 800;
    bambot3 = false;
    tmrbot3 = millis();
  }
  if (xbambot1 > xbot3 - 4 && ybambot1 > ybot3 - 4 && xbambot1 < xbot3 + 24 && ybambot1 < ybot3 + 24) {
    tft.drawBitmap(xbambot1, ybambot1, bammap, 4, 4, ST77XX_BLACK);
    ybambot1 = 600;
    xbambot1 = 600;
    bambot1 = false;
    tmrbot1 = millis();
  }
  if (xbambot2 > xbot3 - 4 && ybambot2 > ybot3 - 4 && xbambot2 < xbot3 + 24 && ybambot2 < ybot3 + 24) {
    tft.drawBitmap(xbambot2, ybambot2, bammap, 4, 4, ST77XX_BLACK);
    ybambot2 = 700;
    xbambot2 = 700;
    bambot2 = false;
    tmrbot2 = millis();
  }
  if (xbambot3 > xbot2 - 4 && ybambot3 > ybot2 - 4 && xbambot3 < xbot2 + 24 && ybambot3 < ybot2 + 24) {
    tft.drawBitmap(xbambot3, ybambot3, bammap, 4, 4, ST77XX_BLACK);
    ybambot3 = 800;
    xbambot3 = 800;
    bambot3 = false;
    tmrbot3 = millis();
  }
  //снаряд в снаряд
  if (xbam > xbambot1 - 6 && ybam > ybambot1 - 6 && xbam < xbambot1 + 6 && ybam < ybambot1 + 6) {
    tft.drawBitmap(xbam, ybam, bammap, 4, 4, ST77XX_BLACK);
    tft.drawBitmap(xbambot1, ybambot1, bammap, 4, 4, ST77XX_BLACK);
    bam = false;
    ybambot1 = 600;
    xbambot1 = 600;
    bambot1 = false;
    tmrbot1 = millis();
    xbam = 500;
    ybam = 500;
  }
  if (xbam > xbambot2 - 6 && ybam > ybambot2 - 6 && xbam < xbambot2 + 6 && ybam < ybambot2 + 6) {
    tft.drawBitmap(xbam, ybam, bammap, 4, 4, ST77XX_BLACK);
    tft.drawBitmap(xbambot2, ybambot2, bammap, 4, 4, ST77XX_BLACK);
    bam = false;
    ybambot2 = 700;
    xbambot2 = 700;
    bambot2 = false;
    tmrbot2 = millis();
    xbam = 500;
    ybam = 500;
  }
  if (xbam > xbambot3 - 6 && ybam > ybambot3 - 6 && xbam < xbambot3 + 6 && ybam < ybambot3 + 6) {
    tft.drawBitmap(xbam, ybam, bammap, 4, 4, ST77XX_BLACK);
    tft.drawBitmap(xbambot3, ybambot3, bammap, 4, 4, ST77XX_BLACK);
    bam = false;
    ybambot3 = 800;
    xbambot3 = 800;
    bambot3 = false;
    tmrbot3 = millis();
    xbam = 500;
    ybam = 500;
  }
  if (xbambot2 > xbambot1 - 6 && ybambot2 > ybambot1 - 6 && xbambot2 < xbambot1 + 6 && ybambot2 < ybambot1 + 6) {
    tft.drawBitmap(xbambot2, ybambot2, bammap, 4, 4, ST77XX_BLACK);
    tft.drawBitmap(xbambot1, ybambot1, bammap, 4, 4, ST77XX_BLACK);
    ybambot2 = 700;
    xbambot2 = 700;
    bambot2 = false;
    tmrbot2 = millis();
    ybambot1 = 600;
    xbambot1 = 600;
    bambot1 = false;
    tmrbot1 = millis();
  }
  if (xbambot3 > xbambot1 - 6 && ybambot3 > ybambot1 - 6 && xbambot3 < xbambot1 + 6 && ybambot3 < ybambot1 + 6) {
    tft.drawBitmap(xbambot3, ybambot3, bammap, 4, 4, ST77XX_BLACK);
    tft.drawBitmap(xbambot1, ybambot1, bammap, 4, 4, ST77XX_BLACK);
    ybambot3 = 800;
    xbambot3 = 800;
    bambot3 = false;
    tmrbot3 = millis();
    ybambot1 = 600;
    xbambot1 = 600;
    bambot1 = false;
    tmrbot1 = millis();
  }
  if (xbambot3 > xbambot2 - 6 && ybambot3 > ybambot2 - 6 && xbambot3 < xbambot2 + 6 && ybambot3 < ybambot2 + 6) {
    tft.drawBitmap(xbambot3, ybambot3, bammap, 4, 4, ST77XX_BLACK);
    tft.drawBitmap(xbambot2, ybambot2, bammap, 4, 4, ST77XX_BLACK);
    ybambot3 = 800;
    xbambot3 = 800;
    bambot3 = false;
    tmrbot3 = millis();
    ybambot2 = 700;
    xbambot2 = 700;
    bambot2 = false;
    tmrbot2 = millis();
  }
}

void vistrel() {
  if (digitalRead(OK_BUTT) == 1 && bam == false) {
    bam = true;
    bamprirav = true;
  }
  if (bam == true) {
    tft.drawBitmap(xbam, ybam, bammap, 4, 4, ST77XX_BLACK);
    xbamst = xbam;
    ybamst = ybam;
    if (bamprirav == true) {
      if (n == 1) {
        xbam = x + 24;
        ybam = y + 10;
      } else if (n == 2) {
        xbam = x - 4;
        ybam = y + 10;
      } else if (n == 3) {
        xbam = x + 10;
        ybam = y - 4;
      } else if (n == 4) {
        xbam = x + 10;
        ybam = y + 24;
      }
      nbam = n;
      bamprirav = false;
    }
    if (nbam == 1) {
      xbam = xbam + SPEEDBAM;
    } else if (nbam == 2) {
      xbam = xbam - SPEEDBAM;
    } else if (nbam == 3) {
      ybam = ybam - SPEEDBAM;
    } else if (nbam == 4) {
      ybam = ybam + SPEEDBAM;
    }
    if (xbam > 287 || ybam > 239 || xbam < 0 || ybam < 0) {
      ybam = 500;
      xbam = 500;
      bam = false;
    }
    sy = 0;
    sx = 0;
    while (sy < 10) {
      while (sx < 12) {
        SR = forMapBS[sy][sx];
        xp = sx * 24;
        yp = sy * 24;
        if (SR == 0) {} else if (SR == 1) {
          if (xbam > xp - 4 && ybam > yp - 4 && xbam < xp + 24 && ybam < yp + 24) {
            ybam = 500;
            xbam = 500;
            bam = false;
          }
        } else if (SR == 2) {
          if (xbam > xp - 4 && ybam > yp - 4 && xbam < xp + 24 && ybam < yp + 24) {
            ybam = 500;
            xbam = 500;
            bam = false;
            tft.fillRect(sx * 24, sy * 24, 24, 24, ST77XX_ORANGE);
            tft.drawBitmap(sx * 24, sy * 24, b_2, 24, 24, 0xC618);
            tft.drawBitmap(sx * 24, sy * 24, r_1, 24, 24, ST77XX_BLACK);
            forMapBS[sy][sx] = 4;
          }
        } else if (SR == 3) {
          if (xbam > xp - 4 && ybam > yp - 4 && xbam < xp + 24 && ybam < yp + 24) {
            tft.fillRect(sx * 24, sy * 24, 24, 24, 0x03EF);
            tft.drawBitmap(sx * 24, sy * 24, b_3, 24, 24, 0xFFFF);
            if (forMapBS[sy - 1][sx] == 3) {
              tft.fillRect(sx * 24, (sy - 1) * 24, 24, 24, 0x03EF);
              tft.drawBitmap(sx * 24, (sy - 1) * 24, b_3, 24, 24, 0xFFFF);
            }
            if (forMapBS[sy + 1][sx] == 3) {
              tft.fillRect(sx * 24, (sy + 1) * 24, 24, 24, 0x03EF);
              tft.drawBitmap(sx * 24, (sy + 1) * 24, b_3, 24, 24, 0xFFFF);
            }
            if (forMapBS[sy][sx - 1] == 3) {
              tft.fillRect((sx - 1) * 24, sy * 24, 24, 24, 0x03EF);
              tft.drawBitmap((sx - 1) * 24, sy * 24, b_3, 24, 24, 0xFFFF);
            }
            if (forMapBS[sy][sx + 1] == 3) {
              tft.fillRect((sx + 1) * 24, sy * 24, 24, 24, 0x03EF);
              tft.drawBitmap((sx + 1) * 24, sy * 24, b_3, 24, 24, 0xFFFF);
            }
          }
        } else if (SR == 4) {
          if (xbam > xp - 4 && ybam > yp - 4 && xbam < xp + 24 && ybam < yp + 24) {
            ybam = 500;
            xbam = 500;
            bam = false;
            tft.fillRect(sx * 24, sy * 24, 24, 24, ST77XX_ORANGE);
            tft.drawBitmap(sx * 24, sy * 24, b_2, 24, 24, 0xC618);
            tft.drawBitmap(sx * 24, sy * 24, r_2, 24, 24, ST77XX_BLACK);
            forMapBS[sy][sx] = 5;
          }
        } else if (SR == 5) {
          if (xbam > xp - 4 && ybam > yp - 4 && xbam < xp + 24 && ybam < yp + 24) {
            ybam = 500;
            xbam = 500;
            bam = false;
            tft.fillRect(sx * 24, sy * 24, 24, 24, ST77XX_ORANGE);
            tft.drawBitmap(sx * 24, sy * 24, b_2, 24, 24, 0xC618);
            tft.drawBitmap(sx * 24, sy * 24, r_3, 24, 24, ST77XX_BLACK);
            forMapBS[sy][sx] = 6;
          }
        } else if (SR == 6) {
          if (xbam > xp - 4 && ybam > yp - 4 && xbam < xp + 24 && ybam < yp + 24) {
            ybam = 500;
            xbam = 500;
            bam = false;
            tft.fillRect(sx * 24, sy * 24, 24, 24, ST77XX_BLACK);
            forMapBS[sy][sx] = 0;
          }
        }
        sx = sx + 1;
      }
      sy = sy + 1;
      sx = 0;
    }
    tft.drawBitmap(xbam, ybam, bammap, 4, 4, 0xF800);
  }
}

void dvizhenie() {
  if (xst != x || yst != y) {
    if (millis() - tmr1 > 60) {
      e3 = !e3;
      tmr1 = millis();
    }
  }
  xst = x;
  yst = y;
  if (digitalRead(RIGHT_BUTT) == 0) {
    if (n == 1) {
      if (x < 264) {
        x = x + SPEED;
      }
    }
    n = 1;
  } else if (digitalRead(LEFT_BUTT) == 0) {
    if (n == 2) {
      if (x > 0) {
        x = x - SPEED;
      }
    }
    n = 2;
  } else if (digitalRead(UP_BUTT) == 0) {
    if (n == 3) {
      if (y > 0) {
        y = y - SPEED;
      }
    }
    n = 3;
  } else if (digitalRead(DOWN_BUTT) == 0) {
    if (n == 4) {
      if (y < 216) {
        y = y + SPEED;
      }
    }
    n = 4;
  }
  if ((x + 24 > xbot1 && x < xbot1 + 24 && y + 24 > ybot1 && y < ybot1 + 24) || (x + 24 > xbot3 && x < xbot3 + 24 && y + 24 > ybot3 && y < ybot3 + 24) || (x + 24 > xbot2 && x < xbot2 + 24 && y + 24 > ybot2 && y < ybot2 + 24)) {
    x = xst;
    y = yst;
  }
  sy = 0;
  sx = 0;
  while (sy < 10) {
    while (sx < 12) {
      SR = forMapBS[sy][sx];
      if (SR > 0) {
        xp = sx * 24;
        yp = sy * 24;
        if (x > xp - 24 && y > yp - 24 && x < xp + 24 && y < yp + 24) {
          x = xst;
          y = yst;
        }
      }
      sx = sx + 1;
    }
    sy = sy + 1;
    sx = 0;
  }
  if (x != xst || y != yst) {
    if (n == 1) {
      tft.fillRect(x - SPEED, y, SPEED, 24, 0x0000);
    } else if (n == 2) {
      tft.fillRect(x + 24, y, SPEED, 24, 0x0000);
    } else if (n == 3) {
      tft.fillRect(x, y + 24, 24, SPEED, 0x0000);
    } else if (n == 4) {
      tft.fillRect(x, y - SPEED, 24, SPEED, 0x0000);
    }
  }
  if (e3 == false) {
    if (n == 1) {
      tft.drawBitmap(x, y, t1n1, 24, 24, ST77XX_ORANGE);
      tft.drawBitmap(x, y, t2n1, 24, 24, 0x915C);
    } else if (n == 2) {
      tft.drawBitmap(x, y, t1n2, 24, 24, ST77XX_ORANGE);
      tft.drawBitmap(x, y, t2n2, 24, 24, 0x915C);
    } else if (n == 3) {
      tft.drawBitmap(x, y, t1n3, 24, 24, ST77XX_ORANGE);
      tft.drawBitmap(x, y, t2n3, 24, 24, 0x915C);
    } else if (n == 4) {
      tft.drawBitmap(x, y, t1n4, 24, 24, ST77XX_ORANGE);
      tft.drawBitmap(x, y, t2n4, 24, 24, 0x915C);
    }
  } else {
    if (n == 1) {
      tft.drawBitmap(x, y, t1n1_2, 24, 24, ST77XX_ORANGE);
      tft.drawBitmap(x, y, t2n1_2, 24, 24, 0x915C);
    } else if (n == 2) {
      tft.drawBitmap(x, y, t1n2_2, 24, 24, ST77XX_ORANGE);
      tft.drawBitmap(x, y, t2n2_2, 24, 24, 0x915C);
    } else if (n == 3) {
      tft.drawBitmap(x, y, t1n3_2, 24, 24, ST77XX_ORANGE);
      tft.drawBitmap(x, y, t2n3_2, 24, 24, 0x915C);
    } else if (n == 4) {
      tft.drawBitmap(x, y, t1n4_2, 24, 24, ST77XX_ORANGE);
      tft.drawBitmap(x, y, t2n4_2, 24, 24, 0x915C);
    }
  }
}

void bot1() {
  if (bambot1 == false && millis() - tmrbot1 > KD) {
    bambot1 = true;
    bampriravbot1 = true;
  }
  if (bambot1 == true) {
    tft.drawBitmap(xbambot1, ybambot1, bammap, 4, 4, ST77XX_BLACK);
    xbambotst1 = xbambot1;
    ybambotst1 = ybambot1;
    if (bampriravbot1 == true) {
      if (kill1 == true) {} else {
        if (nbot1 == 1) {
          xbambot1 = xbot1 + 24;
          ybambot1 = ybot1 + 10;
        } else if (nbot1 == 2) {
          xbambot1 = xbot1 - 4;
          ybambot1 = ybot1 + 10;
        } else if (nbot1 == 3) {
          xbambot1 = xbot1 + 10;
          ybambot1 = ybot1 - 4;
        } else if (nbot1 == 4) {
          xbambot1 = xbot1 + 10;
          ybambot1 = ybot1 + 24;
        }
        nbambot1 = nbot1;
        bampriravbot1 = false;
      }
    }
    if (nbambot1 == 1) {
      xbambot1 = xbambot1 + SPEEDBAM;
    } else if (nbambot1 == 2) {
      xbambot1 = xbambot1 - SPEEDBAM;
    } else if (nbambot1 == 3) {
      ybambot1 = ybambot1 - SPEEDBAM;
    } else if (nbambot1 == 4) {
      ybambot1 = ybambot1 + SPEEDBAM;
    }
    if (xbambot1 > 287 || ybambot1 > 239 || xbambot1 < 0 || ybambot1 < 0) {
      ybambot1 = 600;
      xbambot1 = 600;
      bambot1 = false;
      tmrbot1 = millis();
    }
    sy = 0;
    sx = 0;
    while (sy < 10) {
      while (sx < 12) {
        SR = forMapBS[sy][sx];
        xp = sx * 24;
        yp = sy * 24;
        if (SR == 0) {} else if (SR == 1) {
          if (xbambot1 > xp - 4 && ybambot1 > yp - 4 && xbambot1 < xp + 24 && ybambot1 < yp + 24) {
            ybambot1 = 600;
            xbambot1 = 600;
            bambot1 = false;
            tmrbot1 = millis();
          }
        } else if (SR == 2) {
          if (xbambot1 > xp - 4 && ybambot1 > yp - 4 && xbambot1 < xp + 24 && ybambot1 < yp + 24) {
            ybambot1 = 600;
            xbambot1 = 600;
            bambot1 = false;
            tmrbot1 = millis();
            tft.fillRect(sx * 24, sy * 24, 24, 24, ST77XX_ORANGE);
            tft.drawBitmap(sx * 24, sy * 24, b_2, 24, 24, 0xC618);
            tft.drawBitmap(sx * 24, sy * 24, r_1, 24, 24, ST77XX_BLACK);
            forMapBS[sy][sx] = 4;
          }
        } else if (SR == 3) {
          if (xbambot1 > xp - 4 && ybambot1 > yp - 4 && xbambot1 < xp + 24 && ybambot1 < yp + 24) {
            tft.fillRect(sx * 24, sy * 24, 24, 24, 0x03EF);
            tft.drawBitmap(sx * 24, sy * 24, b_3, 24, 24, 0xFFFF);
            if (forMapBS[sy - 1][sx] == 3) {
              tft.fillRect(sx * 24, (sy - 1) * 24, 24, 24, 0x03EF);
              tft.drawBitmap(sx * 24, (sy - 1) * 24, b_3, 24, 24, 0xFFFF);
            }
            if (forMapBS[sy + 1][sx] == 3) {
              tft.fillRect(sx * 24, (sy + 1) * 24, 24, 24, 0x03EF);
              tft.drawBitmap(sx * 24, (sy + 1) * 24, b_3, 24, 24, 0xFFFF);
            }
            if (forMapBS[sy][sx - 1] == 3) {
              tft.fillRect((sx - 1) * 24, sy * 24, 24, 24, 0x03EF);
              tft.drawBitmap((sx - 1) * 24, sy * 24, b_3, 24, 24, 0xFFFF);
            }
            if (forMapBS[sy][sx + 1] == 3) {
              tft.fillRect((sx + 1) * 24, sy * 24, 24, 24, 0x03EF);
              tft.drawBitmap((sx + 1) * 24, sy * 24, b_3, 24, 24, 0xFFFF);
            }
          }
        } else if (SR == 4) {
          if (xbambot1 > xp - 4 && ybambot1 > yp - 4 && xbambot1 < xp + 24 && ybambot1 < yp + 24) {
            ybambot1 = 600;
            xbambot1 = 600;
            bambot1 = false;
            tmrbot1 = millis();
            tft.fillRect(sx * 24, sy * 24, 24, 24, ST77XX_ORANGE);
            tft.drawBitmap(sx * 24, sy * 24, b_2, 24, 24, 0xC618);
            tft.drawBitmap(sx * 24, sy * 24, r_2, 24, 24, ST77XX_BLACK);
            forMapBS[sy][sx] = 5;
          }
        } else if (SR == 5) {
          if (xbambot1 > xp - 4 && ybambot1 > yp - 4 && xbambot1 < xp + 24 && ybambot1 < yp + 24) {
            ybambot1 = 600;
            xbambot1 = 600;
            bambot1 = false;
            tmrbot1 = millis();
            tft.fillRect(sx * 24, sy * 24, 24, 24, ST77XX_ORANGE);
            tft.drawBitmap(sx * 24, sy * 24, b_2, 24, 24, 0xC618);
            tft.drawBitmap(sx * 24, sy * 24, r_3, 24, 24, ST77XX_BLACK);
            forMapBS[sy][sx] = 6;
          }
        } else if (SR == 6) {
          if (xbambot1 > xp - 4 && ybambot1 > yp - 4 && xbambot1 < xp + 24 && ybambot1 < yp + 24) {
            ybambot1 = 600;
            xbambot1 = 600;
            bambot1 = false;
            tmrbot1 = millis();
            tft.fillRect(sx * 24, sy * 24, 24, 24, ST77XX_BLACK);
            forMapBS[sy][sx] = 0;
          }
        }
        sx = sx + 1;
      }
      sy = sy + 1;
      sx = 0;
    }
    tft.drawBitmap(xbambot1, ybambot1, bammap, 4, 4, 0xF800);
  }
  if (kill1 == false) {
    xbotst1 = xbot1;
    ybotst1 = ybot1;
    if (nbot1 == 1) {
      if (xbot1 < 264) {
        xbot1 = xbot1 + SPEED;
      }
    } else if (nbot1 == 2) {
      if (xbot1 > 0) {
        xbot1 = xbot1 - SPEED;
      }
    } else if (nbot1 == 3) {
      if (ybot1 > 0) {
        ybot1 = ybot1 - SPEED;
      }
    } else if (nbot1 == 4) {
      if (ybot1 < 216) {
        ybot1 = ybot1 + SPEED;
      }
    }
    if ((xbot1 + 24 > xbot2 && xbot1 < xbot2 + 24 && ybot1 + 24 > ybot2 && ybot1 < ybot2 + 24) || (xbot1 + 24 > xbot3 && xbot1 < xbot3 + 24 && ybot1 + 24 > ybot3 && ybot1 < ybot3 + 24) || (xbot1 + 24 > x && xbot1 < x + 24 && ybot1 + 24 > y && ybot1 < y + 24)) {
      xbot1 = xbotst1;
      ybot1 = ybotst1;
    }
    sy = 0;
    sx = 0;
    while (sy < 10) {
      while (sx < 12) {
        SR = forMapBS[sy][sx];
        if (SR > 0) {
          xp = sx * 24;
          yp = sy * 24;
          if (xbot1 > xp - 24 && ybot1 > yp - 24 && xbot1 < xp + 24 && ybot1 < yp + 24) {
            xbot1 = xbotst1;
            ybot1 = ybotst1;
          }
        }
        sx = sx + 1;
      }
      sy = sy + 1;
      sx = 0;
    }
    if (xbot1 == xbotst1 && ybot1 == ybotst1) {
      nbot1 = random(1, 5);
    }
    if (xbot1 != xbotst1 || ybot1 != ybotst1) {
      if (nbot1 == 1) {
        tft.fillRect(xbot1 - SPEED, ybot1, SPEED, 24, 0x0000);
      } else if (nbot1 == 2) {
        tft.fillRect(xbot1 + 24, ybot1, SPEED, 24, 0x0000);
      } else if (nbot1 == 3) {
        tft.fillRect(xbot1, ybot1 + 24, 24, SPEED, 0x0000);
      } else if (nbot1 == 4) {
        tft.fillRect(xbot1, ybot1 - SPEED, 24, SPEED, 0x0000);
      }
    }
    if (e2 == false) {
      if (nbot1 == 1) {
        tft.drawBitmap(xbot1, ybot1, t1n1, 24, 24, ST77XX_ORANGE);
        tft.drawBitmap(xbot1, ybot1, t2n1, 24, 24, 0x9A60);
      } else if (nbot1 == 2) {
        tft.drawBitmap(xbot1, ybot1, t1n2, 24, 24, ST77XX_ORANGE);
        tft.drawBitmap(xbot1, ybot1, t2n2, 24, 24, 0x9A60);
      } else if (nbot1 == 3) {
        tft.drawBitmap(xbot1, ybot1, t1n3, 24, 24, ST77XX_ORANGE);
        tft.drawBitmap(xbot1, ybot1, t2n3, 24, 24, 0x9A60);
      } else if (nbot1 == 4) {
        tft.drawBitmap(xbot1, ybot1, t1n4, 24, 24, ST77XX_ORANGE);
        tft.drawBitmap(xbot1, ybot1, t2n4, 24, 24, 0x9A60);
      }
    } else {
      if (nbot1 == 1) {
        tft.drawBitmap(xbot1, ybot1, t1n1_2, 24, 24, ST77XX_ORANGE);
        tft.drawBitmap(xbot1, ybot1, t2n1_2, 24, 24, 0x9A60);
      } else if (nbot1 == 2) {
        tft.drawBitmap(xbot1, ybot1, t1n2_2, 24, 24, ST77XX_ORANGE);
        tft.drawBitmap(xbot1, ybot1, t2n2_2, 24, 24, 0x9A60);
      } else if (nbot1 == 3) {
        tft.drawBitmap(xbot1, ybot1, t1n3_2, 24, 24, ST77XX_ORANGE);
        tft.drawBitmap(xbot1, ybot1, t2n3_2, 24, 24, 0x9A60);
      } else if (nbot1 == 4) {
        tft.drawBitmap(xbot1, ybot1, t1n4_2, 24, 24, ST77XX_ORANGE);
        tft.drawBitmap(xbot1, ybot1, t2n4_2, 24, 24, 0x9A60);
      }
    }
  } else {
    tmrkill1 = millis();
    kill1 = false;
    for (int i = 0; i < 10; i++) {
      sy = i;
      for (int a = 0; a < 12; a++) {
        sx = a;
        xys = forMapBS[sy][sx];
        if (xys == -2) {
          tft.fillRect(xbot1, ybot1, 24, 24, ST77XX_BLACK);
          xbot1 = sx * 24;
          ybot1 = sy * 24;
          nbot1 = 4;
        }
      }
    }
  }
}

void bot2() {
  if (bambot2 == false && millis() - tmrbot2 > KD) {
    bambot2 = true;
    bampriravbot2 = true;
  }
  if (bambot2 == true) {
    tft.drawBitmap(xbambot2, ybambot2, bammap, 4, 4, ST77XX_BLACK);
    xbambotst2 = xbambot2;
    ybambotst2 = ybambot2;
    if (bampriravbot2 == true) {
      if (kill2 == true) {} else {
        if (nbot2 == 1) {
          xbambot2 = xbot2 + 24;
          ybambot2 = ybot2 + 10;
        } else if (nbot2 == 2) {
          xbambot2 = xbot2 - 4;
          ybambot2 = ybot2 + 10;
        } else if (nbot2 == 3) {
          xbambot2 = xbot2 + 10;
          ybambot2 = ybot2 - 4;
        } else if (nbot2 == 4) {
          xbambot2 = xbot2 + 10;
          ybambot2 = ybot2 + 24;
        }
        nbambot2 = nbot2 ;
        bampriravbot2 = false;
      }
    }
    if (nbambot2 == 1) {
      xbambot2 = xbambot2 + SPEEDBAM;
    } else if (nbambot2 == 2) {
      xbambot2 = xbambot2 - SPEEDBAM;
    } else if (nbambot2 == 3) {
      ybambot2 = ybambot2 - SPEEDBAM;
    } else if (nbambot2 == 4) {
      ybambot2 = ybambot2 + SPEEDBAM;
    }
    if (xbambot2 > 287 || ybambot2 > 239 || xbambot2 < 0 || ybambot2 < 0) {
      ybambot2 = 700;
      xbambot2 = 700;
      bambot2 = false;
      tmrbot2 = millis();
    }
    sy = 0;
    sx = 0;
    while (sy < 10) {
      while (sx < 12) {
        SR = forMapBS[sy][sx];
        xp = sx * 24;
        yp = sy * 24;
        if (SR == 0) {} else if (SR == 1) {
          if (xbambot2 > xp - 4 && ybambot2 > yp - 4 && xbambot2 < xp + 24 && ybambot2 < yp + 24) {
            ybambot2 = 700;
            xbambot2 = 700;
            bambot2 = false;
            tmrbot2 = millis();
          }
        } else if (SR == 2) {
          if (xbambot2 > xp - 4 && ybambot2 > yp - 4 && xbambot2 < xp + 24 && ybambot2 < yp + 24) {
            ybambot2 = 700;
            xbambot2 = 700;
            bambot2 = false;
            tmrbot2 = millis();
            tft.fillRect(sx * 24, sy * 24, 24, 24, ST77XX_ORANGE);
            tft.drawBitmap(sx * 24, sy * 24, b_2, 24, 24, 0xC618);
            tft.drawBitmap(sx * 24, sy * 24, r_1, 24, 24, ST77XX_BLACK);
            forMapBS[sy][sx] = 4;
          }
        } else if (SR == 3) {
          if (xbambot2 > xp - 4 && ybambot2 > yp - 4 && xbambot2 < xp + 24 && ybambot2 < yp + 24) {
            tft.fillRect(sx * 24, sy * 24, 24, 24, 0x03EF);
            tft.drawBitmap(sx * 24, sy * 24, b_3, 24, 24, 0xFFFF);
            if (forMapBS[sy - 1][sx] == 3) {
              tft.fillRect(sx * 24, (sy - 1) * 24, 24, 24, 0x03EF);
              tft.drawBitmap(sx * 24, (sy - 1) * 24, b_3, 24, 24, 0xFFFF);
            }
            if (forMapBS[sy + 1][sx] == 3) {
              tft.fillRect(sx * 24, (sy + 1) * 24, 24, 24, 0x03EF);
              tft.drawBitmap(sx * 24, (sy + 1) * 24, b_3, 24, 24, 0xFFFF);
            }
            if (forMapBS[sy][sx - 1] == 3) {
              tft.fillRect((sx - 1) * 24, sy * 24, 24, 24, 0x03EF);
              tft.drawBitmap((sx - 1) * 24, sy * 24, b_3, 24, 24, 0xFFFF);
            }
            if (forMapBS[sy][sx + 1] == 3) {
              tft.fillRect((sx + 1) * 24, sy * 24, 24, 24, 0x03EF);
              tft.drawBitmap((sx + 1) * 24, sy * 24, b_3, 24, 24, 0xFFFF);
            }
          }
        } else if (SR == 4) {
          if (xbambot2 > xp - 4 && ybambot2 > yp - 4 && xbambot2 < xp + 24 && ybambot2 < yp + 24) {
            ybambot2 = 700;
            xbambot2 = 700;
            bambot2 = false;
            tmrbot2 = millis();
            tft.fillRect(sx * 24, sy * 24, 24, 24, ST77XX_ORANGE);
            tft.drawBitmap(sx * 24, sy * 24, b_2, 24, 24, 0xC618);
            tft.drawBitmap(sx * 24, sy * 24, r_2, 24, 24, ST77XX_BLACK);
            forMapBS[sy][sx] = 5;
          }
        } else if (SR == 5) {
          if (xbambot2 > xp - 4 && ybambot2 > yp - 4 && xbambot2 < xp + 24 && ybambot2 < yp + 24) {
            ybambot2 = 700;
            xbambot2 = 700;
            bambot2 = false;
            tmrbot2 = millis();
            tft.fillRect(sx * 24, sy * 24, 24, 24, ST77XX_ORANGE);
            tft.drawBitmap(sx * 24, sy * 24, b_2, 24, 24, 0xC618);
            tft.drawBitmap(sx * 24, sy * 24, r_3, 24, 24, ST77XX_BLACK);
            forMapBS[sy][sx] = 6;
          }
        } else if (SR == 6) {
          if (xbambot2 > xp - 4 && ybambot2 > yp - 4 && xbambot2 < xp + 24 && ybambot2 < yp + 24) {
            ybambot2 = 700;
            xbambot2 = 700;
            bambot2 = false;
            tmrbot2 = millis();
            tft.fillRect(sx * 24, sy * 24, 24, 24, ST77XX_BLACK);
            forMapBS[sy][sx] = 0;
          }
        }
        sx = sx + 1;
      }
      sy = sy + 1;
      sx = 0;
    }
    tft.drawBitmap(xbambot2, ybambot2, bammap, 4, 4, 0xF800);
  }
  if (kill2 == false) {
    xbotst2 = xbot2;
    ybotst2 = ybot2;
    if (nbot2 == 1) {
      if (xbot2 < 264) {
        xbot2 = xbot2 + SPEED;
      }
    } else if (nbot2 == 2) {
      if (xbot2 > 0) {
        xbot2 = xbot2 - SPEED;
      }
    } else if (nbot2 == 3) {
      if (ybot2 > 0) {
        ybot2 = ybot2 - SPEED;
      }
    } else if (nbot2 == 4) {
      if (ybot2 < 216) {
        ybot2 = ybot2 + SPEED;
      }
    }
    if ((xbot1 + 24 > xbot2 && xbot1 < xbot2 + 24 && ybot1 + 24 > ybot2 && ybot1 < ybot2 + 24) || (xbot2 + 24 > xbot3 && xbot2 < xbot3 + 24 && ybot2 + 24 > ybot3 && ybot2 < ybot3 + 24) || (xbot2 + 24 > x && xbot2 < x + 24 && ybot2 + 24 > y && ybot2 < y + 24)) {
      xbot2 = xbotst2;
      ybot2 = ybotst2;
    }
    sy = 0;
    sx = 0;
    while (sy < 10) {
      while (sx < 12) {
        SR = forMapBS[sy][sx];
        if (SR > 0) {
          xp = sx * 24;
          yp = sy * 24;
          if (xbot2 > xp - 24 && ybot2 > yp - 24 && xbot2 < xp + 24 && ybot2 < yp + 24) {
            xbot2 = xbotst2;
            ybot2 = ybotst2;
          }
        }
        sx = sx + 1;
      }
      sy = sy + 1;
      sx = 0;
    }
    if (xbot2 == xbotst2 && ybot2 == ybotst2) {
      nbot2 = random(1, 5);
    }
    if (xbot2 != xbotst2 || ybot2 != ybotst2) {
      if (nbot2 == 1) {
        tft.fillRect(xbot2 - SPEED, ybot2, SPEED, 24, 0x0000);
      } else if (nbot2 == 2) {
        tft.fillRect(xbot2 + 24, ybot2, SPEED, 24, 0x0000);
      } else if (nbot2 == 3) {
        tft.fillRect(xbot2, ybot2 + 24, 24, SPEED, 0x0000);
      } else if (nbot2 == 4) {
        tft.fillRect(xbot2, ybot2 - SPEED, 24, SPEED, 0x0000);
      }
    }
    if (e2 == false) {
      if (nbot2 == 1) {
        tft.drawBitmap(xbot2, ybot2, t1n1, 24, 24, ST77XX_ORANGE);
        tft.drawBitmap(xbot2, ybot2, t2n1, 24, 24, 0x9A60);
      } else if (nbot2 == 2) {
        tft.drawBitmap(xbot2, ybot2, t1n2, 24, 24, ST77XX_ORANGE);
        tft.drawBitmap(xbot2, ybot2, t2n2, 24, 24, 0x9A60);
      } else if (nbot2 == 3) {
        tft.drawBitmap(xbot2, ybot2, t1n3, 24, 24, ST77XX_ORANGE);
        tft.drawBitmap(xbot2, ybot2, t2n3, 24, 24, 0x9A60);
      } else if (nbot2 == 4) {
        tft.drawBitmap(xbot2, ybot2, t1n4, 24, 24, ST77XX_ORANGE);
        tft.drawBitmap(xbot2, ybot2, t2n4, 24, 24, 0x9A60);
      }
    } else {
      if (nbot2 == 1) {
        tft.drawBitmap(xbot2, ybot2, t1n1_2, 24, 24, ST77XX_ORANGE);
        tft.drawBitmap(xbot2, ybot2, t2n1_2, 24, 24, 0x9A60);
      } else if (nbot2 == 2) {
        tft.drawBitmap(xbot2, ybot2, t1n2_2, 24, 24, ST77XX_ORANGE);
        tft.drawBitmap(xbot2, ybot2, t2n2_2, 24, 24, 0x9A60);
      } else if (nbot2 == 3) {
        tft.drawBitmap(xbot2, ybot2, t1n3_2, 24, 24, ST77XX_ORANGE);
        tft.drawBitmap(xbot2, ybot2, t2n3_2, 24, 24, 0x9A60);
      } else if (nbot2 == 4) {
        tft.drawBitmap(xbot2, ybot2, t1n4_2, 24, 24, ST77XX_ORANGE);
        tft.drawBitmap(xbot2, ybot2, t2n4_2, 24, 24, 0x9A60);
      }
    }
  } else {
    tmrkill2 = millis();
    kill2 = false;
    for (int i = 0; i < 10; i++) {
      sy = i;
      for (int a = 0; a < 12; a++) {
        sx = a;
        xys = forMapBS[sy][sx];
        if (xys == -3) {
          tft.fillRect(xbot2, ybot2, 24, 24, ST77XX_BLACK);
          xbot2 = sx * 24;
          ybot2 = sy * 24;
          nbot2 = 4;
        }
      }
    }
  }
}

void bot3() {
  if (bambot3 == false && millis() - tmrbot3 > KD) {
    bambot3 = true;
    bampriravbot3 = true;
  }
  if (bambot3 == true) {
    tft.drawBitmap(xbambot3, ybambot3, bammap, 4, 4, ST77XX_BLACK);
    xbambotst3 = xbambot3;
    ybambotst3 = ybambot3;
    if (bampriravbot3 == true) {
      if (kill3 == true) {} else {
        if (nbot3 == 1) {
          xbambot3 = xbot3 + 24;
          ybambot3 = ybot3 + 10;
        } else if (nbot3 == 2) {
          xbambot3 = xbot3 - 4;
          ybambot3 = ybot3 + 10;
        } else if (nbot3 == 3) {
          xbambot3 = xbot3 + 10;
          ybambot3 = ybot3 - 4;
        } else if (nbot3 == 4) {
          xbambot3 = xbot3 + 10;
          ybambot3 = ybot3 + 24;
        }
        nbambot3 = nbot3;
        bampriravbot3 = false;
      }
    }
    if (nbambot3 == 1) {
      xbambot3 = xbambot3 + SPEEDBAM;
    } else if (nbambot3 == 2) {
      xbambot3 = xbambot3 - SPEEDBAM;
    } else if (nbambot3 == 3) {
      ybambot3 = ybambot3 - SPEEDBAM;
    } else if (nbambot3 == 4) {
      ybambot3 = ybambot3 + SPEEDBAM;
    }
    if (xbambot3 > 287 || ybambot3 > 239 || xbambot3 < 0 || ybambot3 < 0) {
      ybambot3 = 800;
      xbambot3 = 800;
      bambot3 = false;
      tmrbot3 = millis();
    }
    sy = 0;
    sx = 0;
    while (sy < 10) {
      while (sx < 12) {
        SR = forMapBS[sy][sx];
        xp = sx * 24;
        yp = sy * 24;
        if (SR == 0) {} else if (SR == 1) {
          if (xbambot3 > xp - 4 && ybambot3 > yp - 4 && xbambot3 < xp + 24 && ybambot3 < yp + 24) {
            ybambot3 = 800;
            xbambot3 = 800;
            bambot3 = false;
            tmrbot3 = millis();
          }
        } else if (SR == 2) {
          if (xbambot3 > xp - 4 && ybambot3 > yp - 4 && xbambot3 < xp + 24 && ybambot3 < yp + 24) {
            ybambot3 = 800;
            xbambot3 = 800;
            bambot3 = false;
            tmrbot3 = millis();
            tft.fillRect(sx * 24, sy * 24, 24, 24, ST77XX_ORANGE);
            tft.drawBitmap(sx * 24, sy * 24, b_2, 24, 24, 0xC618);
            tft.drawBitmap(sx * 24, sy * 24, r_1, 24, 24, ST77XX_BLACK);
            forMapBS[sy][sx] = 4;
          }
        } else if (SR == 3) {
          if (xbambot3 > xp - 4 && ybambot3 > yp - 4 && xbambot3 < xp + 24 && ybambot3 < yp + 24) {
            tft.fillRect(sx * 24, sy * 24, 24, 24, 0x03EF);
            tft.drawBitmap(sx * 24, sy * 24, b_3, 24, 24, 0xFFFF);
            if (forMapBS[sy - 1][sx] == 3) {
              tft.fillRect(sx * 24, (sy - 1) * 24, 24, 24, 0x03EF);
              tft.drawBitmap(sx * 24, (sy - 1) * 24, b_3, 24, 24, 0xFFFF);
            }
            if (forMapBS[sy + 1][sx] == 3) {
              tft.fillRect(sx * 24, (sy + 1) * 24, 24, 24, 0x03EF);
              tft.drawBitmap(sx * 24, (sy + 1) * 24, b_3, 24, 24, 0xFFFF);
            }
            if (forMapBS[sy][sx - 1] == 3) {
              tft.fillRect((sx - 1) * 24, sy * 24, 24, 24, 0x03EF);
              tft.drawBitmap((sx - 1) * 24, sy * 24, b_3, 24, 24, 0xFFFF);
            }
            if (forMapBS[sy][sx + 1] == 3) {
              tft.fillRect((sx + 1) * 24, sy * 24, 24, 24, 0x03EF);
              tft.drawBitmap((sx + 1) * 24, sy * 24, b_3, 24, 24, 0xFFFF);
            }
          }
        } else if (SR == 4) {
          if (xbambot3 > xp - 4 && ybambot3 > yp - 4 && xbambot3 < xp + 24 && ybambot3 < yp + 24) {
            ybambot3 = 800;
            xbambot3 = 800;
            bambot3 = false;
            tmrbot3 = millis();
            tft.fillRect(sx * 24, sy * 24, 24, 24, ST77XX_ORANGE);
            tft.drawBitmap(sx * 24, sy * 24, b_2, 24, 24, 0xC618);
            tft.drawBitmap(sx * 24, sy * 24, r_2, 24, 24, ST77XX_BLACK);
            forMapBS[sy][sx] = 5;
          }
        } else if (SR == 5) {
          if (xbambot3 > xp - 4 && ybambot3 > yp - 4 && xbambot3 < xp + 24 && ybambot3 < yp + 24) {
            ybambot3 = 800;
            xbambot3 = 800;
            bambot3 = false;
            tmrbot3 = millis();
            tft.fillRect(sx * 24, sy * 24, 24, 24, ST77XX_ORANGE);
            tft.drawBitmap(sx * 24, sy * 24, b_2, 24, 24, 0xC618);
            tft.drawBitmap(sx * 24, sy * 24, r_3, 24, 24, ST77XX_BLACK);
            forMapBS[sy][sx] = 6;
          }
        } else if (SR == 6) {
          if (xbambot3 > xp - 4 && ybambot3 > yp - 4 && xbambot3 < xp + 24 && ybambot3 < yp + 24) {
            ybambot3 = 800;
            xbambot3 = 800;
            bambot3 = false;
            tmrbot3 = millis();
            tft.fillRect(sx * 24, sy * 24, 24, 24, ST77XX_BLACK);
            forMapBS[sy][sx] = 0;
          }
        }
        sx = sx + 1;
      }
      sy = sy + 1;
      sx = 0;
    }
    tft.drawBitmap(xbambot3, ybambot3, bammap, 4, 4, 0xF800);
  }
  if (kill3 == false) {
    xbotst3 = xbot3;
    ybotst3 = ybot3;
    if (nbot3 == 1) {
      if (xbot3 < 264) {
        xbot3 = xbot3 + SPEED;
      }
    } else if (nbot3 == 2) {
      if (xbot3 > 0) {
        xbot3 = xbot3 - SPEED;
      }
    } else if (nbot3 == 3) {
      if (ybot3 > 0) {
        ybot3 = ybot3 - SPEED;
      }
    } else if (nbot3 == 4) {
      if (ybot3 < 216) {
        ybot3 = ybot3 + SPEED;
      }
    }
    if ((xbot3 + 24 > xbot1 && xbot3 < xbot1 + 24 && ybot3 + 24 > ybot1 && ybot3 < ybot1 + 24) || (xbot2 + 24 > xbot3 && xbot2 < xbot3 + 24 && ybot2 + 24 > ybot3 && ybot2 < ybot3 + 24) || (xbot3 + 24 > x && xbot3 < x + 24 && ybot3 + 24 > y && ybot3 < y + 24)) {
      xbot3 = xbotst3;
      ybot3 = ybotst3;
    }
    sy = 0;
    sx = 0;
    while (sy < 10) {
      while (sx < 12) {
        SR = forMapBS[sy][sx];
        if (SR > 0) {
          xp = sx * 24;
          yp = sy * 24;
          if (xbot3 > xp - 24 && ybot3 > yp - 24 && xbot3 < xp + 24 && ybot3 < yp + 24) {
            xbot3 = xbotst3;
            ybot3 = ybotst3;
          }
        }
        sx = sx + 1;
      }
      sy = sy + 1;
      sx = 0;
    }
    if (xbot3 == xbotst3 && ybot3 == ybotst3) {
      nbot3 = random(1, 5);
    }
    if (xbot3 != xbotst3 || ybot3 != ybotst3) {
      if (nbot3 == 1) {
        tft.fillRect(xbot3 - SPEED, ybot3, SPEED, 24, 0x0000);
      } else if (nbot3 == 2) {
        tft.fillRect(xbot3 + 24, ybot3, SPEED, 24, 0x0000);
      } else if (nbot3 == 3) {
        tft.fillRect(xbot3, ybot3 + 24, 24, SPEED, 0x0000);
      } else if (nbot3 == 4) {
        tft.fillRect(xbot3, ybot3 - SPEED, 24, SPEED, 0x0000);
      }
    }
    if (e2 == false) {
      if (nbot3 == 1) {
        tft.drawBitmap(xbot3, ybot3, t1n1, 24, 24, ST77XX_ORANGE);
        tft.drawBitmap(xbot3, ybot3, t2n1, 24, 24, 0x9A60);
      } else if (nbot3 == 2) {
        tft.drawBitmap(xbot3, ybot3, t1n2, 24, 24, ST77XX_ORANGE);
        tft.drawBitmap(xbot3, ybot3, t2n2, 24, 24, 0x9A60);
      } else if (nbot3 == 3) {
        tft.drawBitmap(xbot3, ybot3, t1n3, 24, 24, ST77XX_ORANGE);
        tft.drawBitmap(xbot3, ybot3, t2n3, 24, 24, 0x9A60);
      } else if (nbot3 == 4) {
        tft.drawBitmap(xbot3, ybot3, t1n4, 24, 24, ST77XX_ORANGE);
        tft.drawBitmap(xbot3, ybot3, t2n4, 24, 24, 0x9A60);
      }
    } else {
      if (nbot3 == 1) {
        tft.drawBitmap(xbot3, ybot3, t1n1_2, 24, 24, ST77XX_ORANGE);
        tft.drawBitmap(xbot3, ybot3, t2n1_2, 24, 24, 0x9A60);
      } else if (nbot3 == 2) {
        tft.drawBitmap(xbot3, ybot3, t1n2_2, 24, 24, ST77XX_ORANGE);
        tft.drawBitmap(xbot3, ybot3, t2n2_2, 24, 24, 0x9A60);
      } else if (nbot3 == 3) {
        tft.drawBitmap(xbot3, ybot3, t1n3_2, 24, 24, ST77XX_ORANGE);
        tft.drawBitmap(xbot3, ybot3, t2n3_2, 24, 24, 0x9A60);
      } else if (nbot3 == 4) {
        tft.drawBitmap(xbot3, ybot3, t1n4_2, 24, 24, ST77XX_ORANGE);
        tft.drawBitmap(xbot3, ybot3, t2n4_2, 24, 24, 0x9A60);
      }
    }
  } else {
    kill3 = false;
    tmrkill3 = millis();
    for (int i = 0; i < 10; i++) {
      sy = i;
      for (int a = 0; a < 12; a++) {
        sx = a;
        xys = forMapBS[sy][sx];
        if (xys == -4) {
          tft.fillRect(xbot3, ybot3, 24, 24, ST77XX_BLACK);
          xbot3 = sx * 24;
          ybot3 = sy * 24;
          nbot3 = 4;
        }
      }
    }
  }
}

void stope() {
  tft.drawRoundRect(54, 60, 212, 120, 10, ST77XX_WHITE);
  tft.drawRoundRect(55, 61, 210, 118, 9, ST77XX_WHITE);
  tft.drawRoundRect(56, 62, 208, 116, 8, ST77XX_BLACK);
  tft.drawRoundRect(57, 63, 206, 114, 7, ST77XX_BLACK);
  tft.drawRoundRect(58, 64, 204, 112, 6, ST77XX_WHITE);
  tft.drawRoundRect(59, 65, 202, 110, 5, ST77XX_WHITE);
  tft.fillRoundRect(60, 66, 200, 108, 4, ST77XX_BLACK);
  tft.drawBitmap(62, 72, go, 196, 25, ST77XX_WHITE);
  tft.setCursor(100, 120);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);
  tft.print("skore = " + String(s));
  topskore = pref.getUInt("skoreBS", 0);
  tft.setCursor(100, 140);
  tft.print("top skore = " + String(topskore));
  if (s > topskore) {
    pref.putUInt("skoreBS", s);
  }
  delay(200);
  while (digitalRead(OK_BUTT) == 0) {
    if (digitalRead(HOME_BUTT) == 1) {
      zvukbool = true;
      zv = 2;
      tmrzvuk = millis();
      zvuk();
      igranach = false;
      break;
    }
  }
  if (igranach == false) {} else {
    igratanks();
  }
}

void dela() {
  if (millis() - tmr1 > 14) {
    xbot1 = xbot1 - 3;
    tft.fillRect(xbot1 + 3, 0, 3, ran1, 0x03E0);
    tft.fillRect(xbot1 + 3, ran1 + 64, 3, 176 - ran1, 0x03E0);
    tft.fillRect(xbot1 + 26, 0, 3, ran1, ST77XX_CYAN);
    tft.fillRect(xbot1 + 26, ran1 + 64, 3, 176 - ran1, ST77XX_CYAN);
    tft.drawBitmap(xbot1, ran1, trubaU, 32, 6, 0x03E0);
    tft.drawBitmap(xbot1, ran1 + 58, trubaD, 32, 6, 0x03E0);
    tft.drawBitmap(xbot1 + 26, ran1, trubaForU, 6, 6, ST77XX_CYAN);
    tft.drawBitmap(xbot1 + 26, ran1 + 58, trubaForD, 6, 6, ST77XX_CYAN);
    xbot2 = xbot2 - 3;
    tft.fillRect(xbot2 + 3, 0, 3, ran2, 0x03E0);
    tft.fillRect(xbot2 + 3, ran2 + 64, 3, 176 - ran2, 0x03E0);
    tft.fillRect(xbot2 + 26, 0, 3, ran2, ST77XX_CYAN);
    tft.fillRect(xbot2 + 26, ran2 + 64, 3, 176 - ran2, ST77XX_CYAN);
    tft.drawBitmap(xbot2, ran2, trubaU, 32, 6, 0x03E0);
    tft.drawBitmap(xbot2, ran2 + 58, trubaD, 32, 6, 0x03E0);
    tft.drawBitmap(xbot2 + 26, ran2, trubaForU, 6, 6, ST77XX_CYAN);
    tft.drawBitmap(xbot2 + 26, ran2 + 58, trubaForD, 6, 6, ST77XX_CYAN);
    if (xbot1 < -32) {
      xbot1 = xbot2 + 200;
      ran1 = random(1, 8) * 22;
      s = s + 1;
      tft.setTextSize(2);
      tft.fillRect(0, 0, 16, 20, ST77XX_CYAN);
      tft.setCursor(0, 0);
      tft.setTextColor(0x0000);
      tft.print(s);
      tft.setTextSize(1);
    }
    if (xbot2 < -32) {
      xbot2 = xbot1 + 200;
      ran2 = random(1, 8) * 22;
      s = s + 1;
      tft.setTextSize(2);
      tft.fillRect(0, 0, 16, 20, ST77XX_CYAN);
      tft.setCursor(0, 0);
      tft.setTextColor(0x0000);
      tft.print(s);
      tft.setTextSize(1);
    }
    if (bird == false) {
      tft.drawBitmap(50, y, b_s_1, 24, 23, ST77XX_CYAN);
    } else {
      tft.drawBitmap(50, y, b_s_2, 24, 23, ST77XX_CYAN);
    }
    if (millis() - tmr2 > 200) {
      bird = !bird;
      tmr2 = millis();
    }
    if (digitalRead(DOWN_BUTT) == 0 && prig == false) {
      prig = true;
      g = -3.4;
    }
    if (digitalRead(DOWN_BUTT) == 1) {
      prig = false;
    }
    y = y + g;
    g = g + 0.4;
    if (bird == false) {
      tft.drawBitmap(50, y, b_o_1, 24, 23, 0x9A60);
      tft.drawBitmap(50, y, b_k_1, 24, 23, 0xFDA0);
    } else {
      tft.drawBitmap(50, y, b_o_2, 24, 23, 0x9A60);
      tft.drawBitmap(50, y, b_k_2, 24, 23, 0xFDA0);
    }
    if (((y < ran1 || y + 23 > ran1 + 64) && xbot1 < 74 && xbot1 + 32 > 50) || ((y < ran2 || y + 23 > ran2 + 64) && xbot2 < 74 && xbot2 + 32 > 50)) {
      tft.drawRoundRect(54, 60, 212, 120, 10, ST77XX_WHITE);
      tft.drawRoundRect(55, 61, 210, 118, 9, ST77XX_WHITE);
      tft.drawRoundRect(56, 62, 208, 116, 8, ST77XX_BLACK);
      tft.drawRoundRect(57, 63, 206, 114, 7, ST77XX_BLACK);
      tft.drawRoundRect(58, 64, 204, 112, 6, ST77XX_WHITE);
      tft.drawRoundRect(59, 65, 202, 110, 5, ST77XX_WHITE);
      tft.fillRoundRect(60, 66, 200, 108, 4, ST77XX_BLACK);
      tft.drawBitmap(62, 72, go, 196, 25, ST77XX_WHITE);
      tft.setCursor(100, 120);
      tft.setTextColor(ST77XX_WHITE);
      tft.print("skore = " + String(s));
      topskore = pref.getUInt("skoreFB", 0);
      tft.setCursor(100, 140);
      tft.setTextColor(ST77XX_WHITE);
      tft.print("top skore = " + String(topskore));
      if (s > topskore) {
        pref.putUInt("skoreFB", s);
      }
      delay(200);
      while (digitalRead(OK_BUTT) == 0 && digitalRead(DOWN_BUTT) == 1) {
        if (digitalRead(HOME_BUTT) == 1) {
          zvukbool = true;
          zv = 2;
          tmrzvuk = millis();
          zvuk();
          igranach = false;
          break;
        }
      }
      if (igranach == false) {} else {
        tft.fillScreen(ST77XX_CYAN);
        xbot1 = 320;
        xbot2 = 520;
        y = 120;
        ran1 = random(1, 8) * 22;
        ran2 = random(1, 8) * 22;
        s = 0;
      }
    }
    tmr1 = millis();
  }
}

void roket() {
  if (digitalRead(DOWN_BUTT) == 0) {
    y = y + 2;
    if (y + 40 > 240) {
      y = 200;
    } else {
      tft.drawBitmap(10, y - 2, forRoketMapD, 10, 7, 0x0000);
      tft.drawBitmap(10, y, roketMap, 10, 40, 0xFF80);
    }
  } else if (digitalRead(UP_BUTT) == 0) {
    y = y - 2;
    if (y < 0) {
      y = 0;
    } else {
      tft.drawBitmap(10, y + 35, forRoketMapU, 10, 7, 0x0000);
      tft.drawBitmap(10, y, roketMap, 10, 40, 0xFF80);
    }
  }
}

void roketbot() {
  if (xbot1 > xbot2 && (n == 1 || n == 4)) {
    if (xbot1 > xbot3 && (n == 1 || n == 4)) {
      if (ybot1 + 9 > ybam + 20) {
        ybam = ybam + 1;
        if (ybam + 40 > 240) {
          ybam = 200;
        } else {
          tft.drawBitmap(300, ybam - 2, forRoketMapD, 10, 7, 0x0000);
          tft.drawBitmap(300, ybam, roketMap, 10, 40, 0x2720);
        }
      } else if (ybot1 + 9 < ybam + 20) {
        ybam = ybam - 1;
        if (ybam < 0) {
          ybam = 0;
        } else {
          tft.drawBitmap(300, ybam + 35, forRoketMapU, 10, 7, 0x0000);
          tft.drawBitmap(300, ybam, roketMap, 10, 40, 0x2720);
        }
      }
    } else {
      if (ybot3 + 9 > ybam + 20) {
        ybam = ybam + 1;
        if (ybam + 40 > 240) {
          ybam = 200;
        } else {
          tft.drawBitmap(300, ybam - 2, forRoketMapD, 10, 7, 0x0000);
          tft.drawBitmap(300, ybam, roketMap, 10, 40, 0x2720);
        }
      } else if (ybot3 + 9 < ybam + 20) {
        ybam = ybam - 1;
        if (ybam < 0) {
          ybam = 0;
        } else {
          tft.drawBitmap(300, ybam + 35, forRoketMapU, 10, 7, 0x0000);
          tft.drawBitmap(300, ybam, roketMap, 10, 40, 0x2720);
        }
      }
    }
  } else {
    if (xbot2 > xbot3 && (n2 == 1 || n2 == 4)) {
      if (ybot2 + 9 > ybam + 20) {
        ybam = ybam + 1;
        if (ybam + 40 > 240) {
          ybam = 200;
        } else {
          tft.drawBitmap(300, ybam - 2, forRoketMapD, 10, 7, 0x0000);
          tft.drawBitmap(300, ybam, roketMap, 10, 40, 0x2720);
        }
      } else if (ybot2 + 9 < ybam + 20) {
        ybam = ybam - 1;
        if (ybam < 0) {
          ybam = 0;
        } else {
          tft.drawBitmap(300, ybam + 35, forRoketMapU, 10, 7, 0x0000);
          tft.drawBitmap(300, ybam, roketMap, 10, 40, 0x2720);
        }
      }
    } else {
      if (ybot3 + 9 > ybam + 20) {
        ybam = ybam + 1;
        if (ybam + 40 > 240) {
          ybam = 200;
        } else {
          tft.drawBitmap(300, ybam - 2, forRoketMapD, 10, 7, 0x0000);
          tft.drawBitmap(300, ybam, roketMap, 10, 40, 0x2720);
        }
      } else if (ybot3 + 9 < ybam + 20) {
        ybam = ybam - 1;
        if (ybam < 0) {
          ybam = 0;
        } else {
          tft.drawBitmap(300, ybam + 35, forRoketMapU, 10, 7, 0x0000);
          tft.drawBitmap(300, ybam, roketMap, 10, 40, 0x2720);
        }
      }
    }
  }
}

void mathBoll1() {
  if (millis() - tmrkill1 > 15) {
    if (kill1 == true) {
      ran = random(1, 5);
      n = ran;
      ran = random(20, 71);
      delitel = 180 / ran;
      arad = PI / delitel;
      xcos = cos(arad);
      ysin = sin(arad);
      kill1 = false;
    }
    xbotst1 = xbot1;
    ybotst1 = ybot1;
    if (n == 1 || n == 4) {
      if (round(3 * xcos) < 1) {
        xbot1 = xbot1 + 1;
      } else {
        xbot1 = xbot1 + round(3 * xcos);
      }
    } else {
      if (round(3 * xcos) < 1) {
        xbot1 = xbot1 - 1;
      } else {
        xbot1 = xbot1 - round(3 * xcos);
      }
    }
    if (n == 3 || n == 4) {
      if (round(3 * ysin) < 1) {
        ybot1 = ybot1 + 1;
      } else {
        ybot1 = ybot1 + round(3 * ysin);
      }
    } else {
      if (round(3 * ysin) < 1) {
        ybot1 = ybot1 - 1;
      } else {
        ybot1 = ybot1 - round(3 * ysin);
      }
    }
    if (xbot1 < 15) {
      redboll = true;
      tmr4 = millis();
    }
    if (xbot1 > 287) {
      redboll = true;
      tmr4 = millis();
    }
    if (xbot1 < 20 && ybot1 < y + 40 && ybot1 + 18 > y && redboll == false) {
      tft.drawBitmap(xbot1, ybot1, boll, 18, 18, 0x0000);
      xbot1 = 20;
      zvukbool = true;
      zv = 1;
      tmrzvuk = millis();
      zvuk();
      if (n == 2) {
        n = 1;
        ran = random(20, 71);
        delitel = 180 / ran;
        arad = PI / delitel;
        xcos = cos(arad);
        ysin = sin(arad);
      } else if (n == 3) {
        n = 4;
        ran = random(20, 71);
        delitel = 180 / ran;
        arad = PI / delitel;
        xcos = cos(arad);
        ysin = sin(arad);
      } else {
        kill1 = true;
      }
    } else if (xbot1 > 282 && ybot1 < ybam + 40 && ybot1 + 18 > ybam && redboll == false) {
      tft.drawBitmap(xbot1, ybot1, boll, 18, 18, 0x0000);
      xbot1 = 282;
      zvukbool = true;
      zv = 1;
      tmrzvuk = millis();
      zvuk();
      if (n == 1) {
        n = 2;
        ran = random(20, 71);
        delitel = 180 / ran;
        arad = PI / delitel;
        xcos = cos(arad);
        ysin = sin(arad);
      } else if (n == 4) {
        n = 3;
        ran = random(20, 71);
        delitel = 180 / ran;
        arad = PI / delitel;
        xcos = cos(arad);
        ysin = sin(arad);
      } else {
        kill1 = true;
      }
    }
    if (xbot1 < -3) {
      if (n == 2) {
        n = 1;
      } else if (n == 3) {
        n = 4;
      }
      tft.drawBitmap(xbot1, ybot1, boll, 18, 18, 0x0000);
      xbot1 = -3;
      zvukbool = true;
      zv = 1;
      tmrzvuk = millis();
      zvuk();
    } else if (xbot1 > 305) {
      if (n == 1) {
        n = 2;
      } else if (n == 4) {
        n = 3;
      }
      tft.drawBitmap(xbot1, ybot1, boll, 18, 18, 0x0000);
      xbot1 = 305;
      zvukbool = true;
      zv = 1;
      tmrzvuk = millis();
      zvuk();
    }
    if (ybot1 < -3) {
      if (n == 1) {
        n = 4;
      } else if (n == 2) {
        n = 3;
      }
      tft.drawBitmap(xbot1, ybot1, boll, 18, 18, 0x0000);
      ybot1 = 0;
      zvukbool = true;
      zv = 1;
      tmrzvuk = millis();
      zvuk();
    } else if (ybot1 > 225) {
      if (n == 4) {
        n = 1;
      } else if (n == 3) {
        n = 2;
      }
      tft.drawBitmap(xbot1, ybot1, boll, 18, 18, 0x0000);
      ybot1 = 225;
      zvukbool = true;
      zv = 1;
      tmrzvuk = millis();
      zvuk();
    }
    if (millis() - tmr4 > 200 && redboll == true) {
      redboll = false;
      tft.setTextSize(7);
      tft.setTextColor(0x0000);
      if (xbot1 > 160) {
        if (rotat == 3) {
          tft.setRotation(4);
        } else {
          tft.setRotation(2);
        }
        tft.setCursor(91, 221);
        tft.print(String(int(p / 100)) + String(int((p / 10) % 10)) + String(int((p % 100) % 10)));
        p = p + 1;
        tft.setTextColor(0x31A6);
        tft.setCursor(91, 221);
        tft.print(String(int(p / 100)) + String(int((p / 10) % 10)) + String(int((p % 100) % 10)));
      } else {
        if (rotat == 3) {
          tft.setRotation(2);
        } else {
          tft.setRotation(4);
        }
        tft.setCursor(91, 221);
        tft.print(String(int(q / 100)) + String(int((q / 10) % 10)) + String(int((q % 100) % 10)));
        q = q + 1;
        tft.setTextColor(0x31A6);
        tft.setCursor(91, 221);
        tft.print(String(int(q / 100)) + String(int((q / 10) % 10)) + String(int((q % 100) % 10)));
      }
      tft.setRotation(rotat);
    }
    if (redboll == true) {
      tft.drawBitmap(xbot1, ybot1, boll, 18, 18, ST77XX_RED);
    } else {
      tft.drawBitmap(xbot1, ybot1, boll, 18, 18, 0x867D);
    }
    tft.drawBitmap(xbot1, ybot1, forBoll, 18, 18, 0x0000);
    tmrkill1 = millis();
  }
}

void mathBoll2() {
  if (millis() - tmrkill2 > 15) {
    if (kill2 == true) {
      ran = random(1, 5);
      n2 = ran;
      ran = random(20, 71);
      delitel = 180 / ran;
      arad = PI / delitel;
      xcos2 = cos(arad);
      ysin2 = sin(arad);
      kill2 = false;
    }
    xbotst2 = xbot2;
    ybotst2 = ybot2;
    if (n2 == 1 || n2 == 4) {
      if (round(3 * xcos2) < 1) {
        xbot2 = xbot2 + 1;
      } else {
        xbot2 = xbot2 + round(3 * xcos2);
      }
    } else {
      if (round(3 * xcos2) < 1) {
        xbot2 = xbot2 - 1;
      } else {
        xbot2 = xbot2 - round(3 * xcos2);
      }
    }
    if (n2 == 3 || n2 == 4) {
      if (round(3 * ysin2) < 1) {
        ybot2 = ybot2 + 1;
      } else {
        ybot2 = ybot2 + round(3 * ysin2);
      }
    } else {
      if (round(3 * ysin2) < 1) {
        ybot2 = ybot2 - 1;
      } else {
        ybot2 = ybot2 - round(3 * ysin2);
      }
    }
    if (xbot2 < 15) {
      redboll2 = true;
      tmr5 = millis();
    }
    if (xbot2 > 287) {
      redboll2 = true;
      tmr5 = millis();
    }
    if (xbot2 < 20 && ybot2 < y + 40 && ybot2 + 18 > y && redboll2 == false) {
      tft.drawBitmap(xbot2, ybot2, boll, 18, 18, 0x0000);
      xbot2 = 20;
      zvukbool = true;
      zv = 1;
      tmrzvuk = millis();
      zvuk();
      if (n2 == 2) {
        n2 = 1;
        ran = random(20, 71);
        delitel = 180 / ran;
        arad = PI / delitel;
        xcos2 = cos(arad);
        ysin2 = sin(arad);
      } else if (n2 == 3) {
        n2 = 4;
        ran = random(20, 71);
        delitel = 180 / ran;
        arad = PI / delitel;
        xcos2 = cos(arad);
        ysin2 = sin(arad);
      } else {
        kill2 = true;
      }
    } else if (xbot2 > 282 && ybot2 < ybam + 40 && ybot2 + 18 > ybam && redboll2 == false) {
      tft.drawBitmap(xbot2, ybot2, boll, 18, 18, 0x0000);
      xbot2 = 282;
      zvukbool = true;
      zv = 1;
      tmrzvuk = millis();
      zvuk();
      if (n2 == 1) {
        n2 = 2;
        ran = random(20, 71);
        delitel = 180 / ran;
        arad = PI / delitel;
        xcos2 = cos(arad);
        ysin2 = sin(arad);
      } else if (n2 == 4) {
        n2 = 3;
        ran = random(20, 71);
        delitel = 180 / ran;
        arad = PI / delitel;
        xcos2 = cos(arad);
        ysin2 = sin(arad);
      } else {
        kill2 = true;
      }
    }
    if (xbot2 < -3) {
      if (n2 == 2) {
        n2 = 1;
      } else if (n2 == 3) {
        n2 = 4;
      }
      tft.drawBitmap(xbot2, ybot2, boll, 18, 18, 0x0000);
      xbot2 = -3;
      zvukbool = true;
      zv = 1;
      tmrzvuk = millis();
      zvuk();
    } else if (xbot2 > 305) {
      if (n2 == 1) {
        n2 = 2;
      } else if (n2 == 4) {
        n2 = 3;
      }
      tft.drawBitmap(xbot2, ybot2, boll, 18, 18, 0x0000);
      xbot2 = 305;
      zvukbool = true;
      zv = 1;
      tmrzvuk = millis();
      zvuk();
    }
    if (ybot2 < -3) {
      if (n2 == 1) {
        n2 = 4;
      } else if (n2 == 2) {
        n2 = 3;
      }
      tft.drawBitmap(xbot2, ybot2, boll, 18, 18, 0x0000);
      ybot2 = 0;
      zvukbool = true;
      zv = 1;
      tmrzvuk = millis();
      zvuk();
    } else if (ybot2 > 225) {
      if (n2 == 4) {
        n2 = 1;
      } else if (n2 == 3) {
        n2 = 2;
      }
      tft.drawBitmap(xbot2, ybot2, boll, 18, 18, 0x0000);
      ybot2 = 225;
      zvukbool = true;
      zv = 1;
      tmrzvuk = millis();
      zvuk();
    }
    if (millis() - tmr5 > 200 && redboll2 == true) {
      redboll2 = false;
      tft.setTextSize(7);
      tft.setTextColor(0x0000);
      if (xbot2 > 160) {
        if (rotat == 3) {
          tft.setRotation(4);
        } else {
          tft.setRotation(2);
        }
        tft.setCursor(91, 221);
        tft.print(String(int(p / 100)) + String(int((p / 10) % 10)) + String(int((p % 100) % 10)));
        p = p + 1;
        tft.setTextColor(0x31A6);
        tft.setCursor(91, 221);
        tft.print(String(int(p / 100)) + String(int((p / 10) % 10)) + String(int((p % 100) % 10)));
      } else {
        if (rotat == 3) {
          tft.setRotation(2);
        } else {
          tft.setRotation(4);
        }
        tft.setCursor(91, 221);
        tft.print(String(int(q / 100)) + String(int((q / 10) % 10)) + String(int((q % 100) % 10)));
        q = q + 1;
        tft.setTextColor(0x31A6);
        tft.setCursor(91, 221);
        tft.print(String(int(q / 100)) + String(int((q / 10) % 10)) + String(int((q % 100) % 10)));
      }
      tft.setRotation(rotat);
    }
    if (redboll2 == true) {
      tft.drawBitmap(xbot2, ybot2, boll, 18, 18, ST77XX_RED);
    } else {
      tft.drawBitmap(xbot2, ybot2, boll, 18, 18, 0x867D);
    }
    tft.drawBitmap(xbot2, ybot2, forBoll, 18, 18, 0x0000);
    tmrkill2 = millis();
  }
}

void mathBoll3() {
  if (millis() - tmrkill3 > 15) {
    if (kill3 == true) {
      ran = random(1, 5);
      n3 = ran;
      ran = random(20, 71);
      delitel = 180 / ran;
      arad = PI / delitel;
      xcos3 = cos(arad);
      ysin3 = sin(arad);
      kill3 = false;
    }
    xbotst3 = xbot3;
    ybotst3 = ybot3;
    if (n3 == 1 || n3 == 4) {
      if (round(3 * xcos3) < 1) {
        xbot3 = xbot3 + 1;
      } else {
        xbot3 = xbot3 + round(3 * xcos3);
      }
    } else {
      if (round(3 * xcos3) < 1) {
        xbot3 = xbot3 - 1;
      } else {
        xbot3 = xbot3 - round(3 * xcos3);
      }
    }
    if (n3 == 3 || n3 == 4) {
      if (round(3 * ysin3) < 1) {
        ybot3 = ybot3 + 1;
      } else {
        ybot3 = ybot3 + round(3 * ysin3);
      }
    } else {
      if (round(3 * ysin3) < 1) {
        ybot3 = ybot3 - 1;
      } else {
        ybot3 = ybot3 - round(3 * ysin3);
      }
    }
    if (xbot3 < 15) {
      redboll3 = true;
      tmr6 = millis();
    }
    if (xbot3 > 287) {
      redboll3 = true;
      tmr6 = millis();
    }
    if (xbot3 < 20 && ybot3 < y + 40 && ybot3 + 18 > y && redboll3 == false) {
      tft.drawBitmap(xbot3, ybot3, boll, 18, 18, 0x0000);
      xbot3 = 20;
      zvukbool = true;
      zv = 1;
      tmrzvuk = millis();
      zvuk();
      if (n3 == 2) {
        n3 = 1;
        ran = random(20, 71);
        delitel = 180 / ran;
        arad = PI / delitel;
        xcos3 = cos(arad);
        ysin3 = sin(arad);
      } else if (n3 == 3) {
        n3 = 4;
        ran = random(20, 71);
        delitel = 180 / ran;
        arad = PI / delitel;
        xcos3 = cos(arad);
        ysin3 = sin(arad);
      } else {
        kill3 = true;
      }
    } else if (xbot3 > 282 && ybot3 < ybam + 40 && ybot3 + 18 > ybam && redboll3 == false) {
      tft.drawBitmap(xbot3, ybot3, boll, 18, 18, 0x0000);
      xbot3 = 282;
      zvukbool = true;
      zv = 1;
      tmrzvuk = millis();
      zvuk();
      if (n3 == 1) {
        n3 = 2;
        ran = random(20, 71);
        delitel = 180 / ran;
        arad = PI / delitel;
        xcos3 = cos(arad);
        ysin3 = sin(arad);
      } else if (n3 == 4) {
        n3 = 3;
        ran = random(20, 71);
        delitel = 180 / ran;
        arad = PI / delitel;
        xcos3 = cos(arad);
        ysin3 = sin(arad);
      } else {
        kill3 = true;
      }
    }
    if (xbot3 < -3) {
      if (n3 == 2) {
        n3 = 1;
      } else if (n3 == 3) {
        n3 = 4;
      }
      tft.drawBitmap(xbot3, ybot3, boll, 18, 18, 0x0000);
      xbot3 = -3;
      zvukbool = true;
      zv = 1;
      tmrzvuk = millis();
      zvuk();
    } else if (xbot3 > 305) {
      if (n3 == 1) {
        n3 = 2;
      } else if (n3 == 4) {
        n3 = 3;
      }
      tft.drawBitmap(xbot3, ybot3, boll, 18, 18, 0x0000);
      xbot3 = 305;
      zvukbool = true;
      zv = 1;
      tmrzvuk = millis();
      zvuk();
    }
    if (ybot3 < -3) {
      if (n3 == 1) {
        n3 = 4;
      } else if (n3 == 2) {
        n3 = 3;
      }
      tft.drawBitmap(xbot3, ybot3, boll, 18, 18, 0x0000);
      ybot3 = 0;
      zvukbool = true;
      zv = 1;
      tmrzvuk = millis();
      zvuk();
    } else if (ybot3 > 225) {
      if (n3 == 4) {
        n3 = 1;
      } else if (n3 == 3) {
        n3 = 2;
      }
      tft.drawBitmap(xbot3, ybot3, boll, 18, 18, 0x0000);
      ybot3 = 225;
      zvukbool = true;
      zv = 1;
      tmrzvuk = millis();
      zvuk();
    }
    if (millis() - tmr6 > 200 && redboll3 == true) {
      redboll3 = false;
      tft.setTextSize(7);
      tft.setTextColor(0x0000);
      if (xbot3 > 160) {
        if (rotat == 3) {
          tft.setRotation(4);
        } else {
          tft.setRotation(2);
        }
        tft.setCursor(91, 221);
        tft.print(String(int(p / 100)) + String(int((p / 10) % 10)) + String(int((p % 100) % 10)));
        p = p + 1;
        tft.setTextColor(0x31A6);
        tft.setCursor(91, 221);
        tft.print(String(int(p / 100)) + String(int((p / 10) % 10)) + String(int((p % 100) % 10)));
      } else {
        if (rotat == 3) {
          tft.setRotation(2);
        } else {
          tft.setRotation(4);
        }
        tft.setCursor(91, 221);
        tft.print(String(int(q / 100)) + String(int((q / 10) % 10)) + String(int((q % 100) % 10)));
        q = q + 1;
        tft.setTextColor(0x31A6);
        tft.setCursor(91, 221);
        tft.print(String(int(q / 100)) + String(int((q / 10) % 10)) + String(int((q % 100) % 10)));
      }
      tft.setRotation(rotat);
    }
    if (redboll3 == true) {
      tft.drawBitmap(xbot3, ybot3, boll, 18, 18, ST77XX_RED);
    } else {
      tft.drawBitmap(xbot3, ybot3, boll, 18, 18, 0x867D);
    }
    tft.drawBitmap(xbot3, ybot3, forBoll, 18, 18, 0x0000);
    tmrkill3 = millis();
  }
}

void polepong() {
  if (xbot1 < 163 && xbot1 + 18 > 157 && ybot1 < 16) {
    ob1 = true;
  } else if (ob1 == true) {
    ob1 = false;
    tft.fillRect(157, 0, 6, 16, 0x31A6);
  }
  if (xbot1 < 163 && xbot1 + 18 > 157 && ybot1 < 48 && ybot1 + 18 > 32) {
    ob2 = true;
  } else if (ob2 == true) {
    ob2 = false;
    tft.fillRect(157, 32, 6, 16, 0x31A6);
  }
  if (xbot1 < 163 && xbot1 + 18 > 157 && ybot1 < 80 && ybot1 + 18 > 64) {
    ob3 = true;
  } else if (ob3 == true) {
    ob3 = false;
    tft.fillRect(157, 64, 6, 16, 0x31A6);
  }
  if (xbot1 < 163 && xbot1 + 18 > 157 && ybot1 < 112 && ybot1 + 18 > 96) {
    ob4 = true;
  } else if (ob4 == true) {
    ob4 = false;
    tft.fillRect(157, 96, 6, 16, 0x31A6);
  }
  if (xbot1 < 163 && xbot1 + 18 > 157 && ybot1 < 144 && ybot1 + 18 > 128) {
    ob5 = true;
  } else if (ob5 == true) {
    ob5 = false;
    tft.fillRect(157, 128, 6, 16, 0x31A6);
  }
  if (xbot1 < 163 && xbot1 + 18 > 157 && ybot1 < 176 && ybot1 + 18 > 160) {
    ob6 = true;
  } else if (ob6 == true) {
    ob6 = false;
    tft.fillRect(157, 160, 6, 16, 0x31A6);
  }
  if (xbot1 < 163 && xbot1 + 18 > 157 && ybot1 < 208 && ybot1 + 18 > 192) {
    ob7 = true;
  } else if (ob7 == true) {
    ob7 = false;
    tft.fillRect(157, 192, 6, 16, 0x31A6);
  }
  if (xbot1 < 163 && xbot1 + 18 > 157 && ybot1 + 18 > 224) {
    ob8 = true;
  } else if (ob8 == true) {
    ob8 = false;
    tft.fillRect(157, 224, 6, 16, 0x31A6);
  }
  if (xbot2 < 163 && xbot2 + 18 > 157 && ybot2 < 16) {
    ob1 = true;
  } else if (ob1 == true) {
    ob1 = false;
    tft.fillRect(157, 0, 6, 16, 0x31A6);
  }
  if (xbot2 < 163 && xbot2 + 18 > 157 && ybot2 < 48 && ybot2 + 18 > 32) {
    ob2 = true;
  } else if (ob2 == true) {
    ob2 = false;
    tft.fillRect(157, 32, 6, 16, 0x31A6);
  }
  if (xbot2 < 163 && xbot2 + 18 > 157 && ybot2 < 80 && ybot2 + 18 > 64) {
    ob3 = true;
  } else if (ob3 == true) {
    ob3 = false;
    tft.fillRect(157, 64, 6, 16, 0x31A6);
  }
  if (xbot2 < 163 && xbot2 + 18 > 157 && ybot2 < 112 && ybot2 + 18 > 96) {
    ob4 = true;
  } else if (ob4 == true) {
    ob4 = false;
    tft.fillRect(157, 96, 6, 16, 0x31A6);
  }
  if (xbot2 < 163 && xbot2 + 18 > 157 && ybot2 < 144 && ybot2 + 18 > 128) {
    ob5 = true;
  } else if (ob5 == true) {
    ob5 = false;
    tft.fillRect(157, 128, 6, 16, 0x31A6);
  }
  if (xbot2 < 163 && xbot2 + 18 > 157 && ybot2 < 176 && ybot2 + 18 > 160) {
    ob6 = true;
  } else if (ob6 == true) {
    ob6 = false;
    tft.fillRect(157, 160, 6, 16, 0x31A6);
  }
  if (xbot2 < 163 && xbot2 + 18 > 157 && ybot2 < 208 && ybot2 + 18 > 192) {
    ob7 = true;
  } else if (ob7 == true) {
    ob7 = false;
    tft.fillRect(157, 192, 6, 16, 0x31A6);
  }
  if (xbot2 < 163 && xbot2 + 18 > 157 && ybot2 + 18 > 224) {
    ob8 = true;
  } else if (ob8 == true) {
    ob8 = false;
    tft.fillRect(157, 224, 6, 16, 0x31A6);
  }
  if (xbot3 < 163 && xbot3 + 18 > 157 && ybot3 < 16) {
    ob1 = true;
  } else if (ob1 == true) {
    ob1 = false;
    tft.fillRect(157, 0, 6, 16, 0x31A6);
  }
  if (xbot3 < 163 && xbot3 + 18 > 157 && ybot3 < 48 && ybot3 + 18 > 32) {
    ob2 = true;
  } else if (ob2 == true) {
    ob2 = false;
    tft.fillRect(157, 32, 6, 16, 0x31A6);
  }
  if (xbot3 < 163 && xbot3 + 18 > 157 && ybot3 < 80 && ybot3 + 18 > 64) {
    ob3 = true;
  } else if (ob3 == true) {
    ob3 = false;
    tft.fillRect(157, 64, 6, 16, 0x31A6);
  }
  if (xbot3 < 163 && xbot3 + 18 > 157 && ybot3 < 112 && ybot3 + 18 > 96) {
    ob4 = true;
  } else if (ob4 == true) {
    ob4 = false;
    tft.fillRect(157, 96, 6, 16, 0x31A6);
  }
  if (xbot3 < 163 && xbot3 + 18 > 157 && ybot3 < 144 && ybot3 + 18 > 128) {
    ob5 = true;
  } else if (ob5 == true) {
    ob5 = false;
    tft.fillRect(157, 128, 6, 16, 0x31A6);
  }
  if (xbot3 < 163 && xbot3 + 18 > 157 && ybot3 < 176 && ybot3 + 18 > 160) {
    ob6 = true;
  } else if (ob6 == true) {
    ob6 = false;
    tft.fillRect(157, 160, 6, 16, 0x31A6);
  }
  if (xbot3 < 163 && xbot3 + 18 > 157 && ybot3 < 208 && ybot3 + 18 > 192) {
    ob7 = true;
  } else if (ob7 == true) {
    ob7 = false;
    tft.fillRect(157, 192, 6, 16, 0x31A6);
  }
  if (xbot3 < 163 && xbot3 + 18 > 157 && ybot3 + 18 > 224) {
    ob8 = true;
  } else if (ob8 == true) {
    ob8 = false;
    tft.fillRect(157, 224, 6, 16, 0x31A6);
  }
  if (xbot1 < 99 && xbot1 + 18 > 50 && ybot1 < 210 && ybot1 + 18 > 175) {
    ob9 = true;
  } else if (ob9 == true) {
    ob9 = false;
    tft.setTextColor(0x31A6);
    tft.setTextSize(7);
    if (rotat == 3) {
      tft.setRotation(4);
    } else {
      tft.setRotation(2);
    }
    tft.setCursor(91, 221);
    tft.print("  " + String(int((p % 100) % 10)));
    tft.setRotation(rotat);
  }
  if (xbot1 < 99 && xbot1 + 18 > 50 && ybot1 < 168 && ybot1 + 18 > 133) {
    ob10 = true;
  } else if (ob10 == true) {
    ob10 = false;
    tft.setTextColor(0x31A6);
    tft.setTextSize(7);
    if (rotat == 3) {
      tft.setRotation(4);
    } else {
      tft.setRotation(2);
    }
    tft.setCursor(91, 221);
    tft.print(" " + String(int((p / 10) % 10)));
    tft.setRotation(rotat);
  }
  if (xbot1 < 99 && xbot1 + 18 > 50 && ybot1 < 126 && ybot1 + 18 > 91) {
    ob11 = true;
  } else if (ob11 == true) {
    ob11 = false;
    tft.setTextColor(0x31A6);
    tft.setTextSize(7);
    if (rotat == 3) {
      tft.setRotation(4);
    } else {
      tft.setRotation(2);
    }
    tft.setCursor(91, 221);
    tft.print(String(int(p / 100)));
    tft.setRotation(rotat);
  }
  if (xbot1 < 270 && xbot1 + 18 > 221 && ybot1 < 65 && ybot1 + 18 > 30) {
    ob12 = true;
  } else if (ob12 == true) {
    ob12 = false;
    tft.setTextColor(0x31A6);
    tft.setTextSize(7);
    if (rotat == 3) {
      tft.setRotation(2);
    } else {
      tft.setRotation(4);
    }
    tft.setCursor(91, 221);
    tft.print("  " + String(int((q % 100) % 10)));
    tft.setRotation(rotat);
  }
  if (xbot1 < 270 && xbot1 + 18 > 221 && ybot1 < 107 && ybot1 + 18 > 72) {
    ob13 = true;
  } else if (ob13 == true) {
    ob13 = false;
    tft.setTextColor(0x31A6);
    tft.setTextSize(7);
    if (rotat == 3) {
      tft.setRotation(2);
    } else {
      tft.setRotation(4);
    }
    tft.setCursor(91, 221);
    tft.print(" " + String(int((q / 10) % 10)));
    tft.setRotation(rotat);
  }
  if (xbot1 < 270 && xbot1 + 18 > 221 && ybot1 < 149 && ybot1 + 18 > 114) {
    ob14 = true;
  } else if (ob14 == true) {
    ob14 = false;
    tft.setTextColor(0x31A6);
    tft.setTextSize(7);
    if (rotat == 3) {
      tft.setRotation(2);
    } else {
      tft.setRotation(4);
    }
    tft.setCursor(91, 221);
    tft.print(String(int(q / 100)));
    tft.setRotation(rotat);
  }
  if (xbot2 < 99 && xbot2 + 18 > 50 && ybot2 < 210 && ybot2 + 18 > 175) {
    ob9 = true;
  } else if (ob9 == true) {
    ob9 = false;
    tft.setTextColor(0x31A6);
    tft.setTextSize(7);
    if (rotat == 3) {
      tft.setRotation(4);
    } else {
      tft.setRotation(2);
    }
    tft.setCursor(91, 221);
    tft.print("  " + String(int((p % 100) % 10)));
    tft.setRotation(rotat);
  }
  if (xbot2 < 99 && xbot2 + 18 > 50 && ybot2 < 168 && ybot2 + 18 > 133) {
    ob10 = true;
  } else if (ob10 == true) {
    ob10 = false;
    tft.setTextColor(0x31A6);
    tft.setTextSize(7);
    if (rotat == 3) {
      tft.setRotation(4);
    } else {
      tft.setRotation(2);
    }
    tft.setCursor(91, 221);
    tft.print(" " + String(int((p / 10) % 10)));
    tft.setRotation(rotat);
  }
  if (xbot2 < 99 && xbot2 + 18 > 50 && ybot2 < 126 && ybot2 + 18 > 91) {
    ob11 = true;
  } else if (ob11 == true) {
    ob11 = false;
    tft.setTextColor(0x31A6);
    tft.setTextSize(7);
    if (rotat == 3) {
      tft.setRotation(4);
    } else {
      tft.setRotation(2);
    }
    tft.setCursor(91, 221);
    tft.print(String(int(p / 100)));
    tft.setRotation(rotat);
  }
  if (xbot2 < 270 && xbot2 + 18 > 221 && ybot2 < 65 && ybot2 + 18 > 30) {
    ob12 = true;
  } else if (ob12 == true) {
    ob12 = false;
    tft.setTextColor(0x31A6);
    tft.setTextSize(7);
    if (rotat == 3) {
      tft.setRotation(2);
    } else {
      tft.setRotation(4);
    }
    tft.setCursor(91, 221);
    tft.print("  " + String(int((q % 100) % 10)));
    tft.setRotation(rotat);
  }
  if (xbot2 < 270 && xbot2 + 18 > 221 && ybot2 < 107 && ybot2 + 18 > 72) {
    ob13 = true;
  } else if (ob13 == true) {
    ob13 = false;
    tft.setTextColor(0x31A6);
    tft.setTextSize(7);
    if (rotat == 3) {
      tft.setRotation(2);
    } else {
      tft.setRotation(4);
    }
    tft.setCursor(91, 221);
    tft.print(" " + String(int((q / 10) % 10)));
    tft.setRotation(rotat);
  }
  if (xbot2 < 270 && xbot2 + 18 > 221 && ybot2 < 149 && ybot2 + 18 > 114) {
    ob14 = true;
  } else if (ob14 == true) {
    ob14 = false;
    tft.setTextColor(0x31A6);
    tft.setTextSize(7);
    if (rotat == 3) {
      tft.setRotation(2);
    } else {
      tft.setRotation(4);
    }
    tft.setCursor(91, 221);
    tft.print(String(int(q / 100)));
    tft.setRotation(rotat);
  }
  if (xbot3 < 99 && xbot3 + 18 > 50 && ybot3 < 210 && ybot3 + 18 > 175) {
    ob9 = true;
  } else if (ob9 == true) {
    ob9 = false;
    tft.setTextColor(0x31A6);
    tft.setTextSize(7);
    if (rotat == 3) {
      tft.setRotation(4);
    } else {
      tft.setRotation(2);
    }
    tft.setCursor(91, 221);
    tft.print("  " + String(int((p % 100) % 10)));
    tft.setRotation(rotat);
  }
  if (xbot3 < 99 && xbot3 + 18 > 50 && ybot3 < 168 && ybot3 + 18 > 133) {
    ob10 = true;
  } else if (ob10 == true) {
    ob10 = false;
    tft.setTextColor(0x31A6);
    tft.setTextSize(7);
    if (rotat == 3) {
      tft.setRotation(4);
    } else {
      tft.setRotation(2);
    }
    tft.setCursor(91, 221);
    tft.print(" " + String(int((p / 10) % 10)));
    tft.setRotation(rotat);
  }
  if (xbot3 < 99 && xbot3 + 18 > 50 && ybot3 < 126 && ybot3 + 18 > 91) {
    ob11 = true;
  } else if (ob11 == true) {
    ob11 = false;
    tft.setTextColor(0x31A6);
    tft.setTextSize(7);
    if (rotat == 3) {
      tft.setRotation(4);
    } else {
      tft.setRotation(2);
    }
    tft.setCursor(91, 221);
    tft.print(String(int(p / 100)));
    tft.setRotation(rotat);
  }
  if (xbot3 < 270 && xbot3 + 18 > 221 && ybot3 < 65 && ybot3 + 18 > 30) {
    ob12 = true;
  } else if (ob12 == true) {
    ob12 = false;
    tft.setTextColor(0x31A6);
    tft.setTextSize(7);
    if (rotat == 3) {
      tft.setRotation(2);
    } else {
      tft.setRotation(4);
    }
    tft.setCursor(91, 221);
    tft.print("  " + String(int((q % 100) % 10)));
    tft.setRotation(rotat);
  }
  if (xbot3 < 270 && xbot3 + 18 > 221 && ybot3 < 107 && ybot3 + 18 > 72) {
    ob13 = true;
  } else if (ob13 == true) {
    ob13 = false;
    tft.setTextColor(0x31A6);
    tft.setTextSize(7);
    if (rotat == 3) {
      tft.setRotation(2);
    } else {
      tft.setRotation(4);
    }
    tft.setCursor(91, 221);
    tft.print(" " + String(int((q / 10) % 10)));
    tft.setRotation(rotat);
  }
  if (xbot3 < 270 && xbot3 + 18 > 221 && ybot3 < 149 && ybot3 + 18 > 114) {
    ob14 = true;
  } else if (ob14 == true) {
    ob14 = false;
    tft.setTextColor(0x31A6);
    tft.setTextSize(7);
    if (rotat == 3) {
      tft.setRotation(2);
    } else {
      tft.setRotation(4);
    }
    tft.setCursor(91, 221);
    tft.print(String(int(q / 100)));
    tft.setRotation(rotat);
  }
  if (xbot1 < 270 && ybot1 < y + 10 && ybot1 + 18 > y) {
    ob15 = true;
  } else if (ob15 == true) {
    ob15 = false;
    tft.drawBitmap(10, y, roketMap, 10, 40, 0xFF80);
  }
  if (xbot2 < 270 && ybot2 < y + 10 && ybot2 + 18 > y) {
    ob15 = true;
  } else if (ob15 == true) {
    ob15 = false;
    tft.drawBitmap(10, y, roketMap, 10, 40, 0xFF80);
  }
  if (xbot3 < 270 && ybot3 < y + 10 && ybot3 + 18 > y) {
    ob15 = true;
  } else if (ob15 == true) {
    ob15 = false;
    tft.drawBitmap(10, y, roketMap, 10, 40, 0xFF80);
  }
}

void boost() {
  if (tab == false) {
    zvukbool = true;
    zv = 1;
    tmrzvuk = millis();
    zvuk();
    tabkord = random(1, spase);
    while (tabkord == kordboost[3] || tabkord == kordboost[2] || tabkord == kordboost[1]) {
      tabkord = random(1, spase);
    }
    kordboost[0] = tabkord;
    dlaspase = 0;
    for (int i = 0; i < 8; i++) {
      sy = i;
      for (int a = 0; a < 10; a++) {
        sx = a;
        if (fm == 0) {
          xys = map1PM[sy][sx];
        } else if (fm == 1) {
          xys = map2PM[sy][sx];
        } else {
          xys = map3PM[sy][sx];
        }
        if (xys == 0) {
          dlaspase = dlaspase + 1;
        }
        if (dlaspase == tabkord - 1) {
          xt = sx * 30;
          yt = sy * 30;
          a = 11;
          i = 9;
        }
      }
    }
    if (xt > -1 && xt < 30 && yt > -1 && yt < 30) {
      tab = false;
    } else {
      tft.drawBitmap(xt, yt, tabletka, 30, 30, 0xFE19);
      tab = true;
    }
  }
  if (c == false) {
    zvukbool = true;
    zv = 1;
    tmrzvuk = millis();
    zvuk();
    ckord = random(1, spase);
    while (ckord == kordboost[3] || ckord == kordboost[2] || ckord == kordboost[0]) {
      ckord = random(1, spase);
    }
    kordboost[1] = ckord;
    dlac = 0;
    for (int i = 0; i < 8; i++) {
      sy = i;
      for (int a = 0; a < 10; a++) {
        sx = a;
        if (fm == 0) {
          xys = map1PM[sy][sx];
        } else if (fm == 1) {
          xys = map2PM[sy][sx];
        } else {
          xys = map3PM[sy][sx];
        }
        if (xys == 0) {
          dlac = dlac + 1;
        }
        if (dlac == ckord - 1) {
          xc = sx * 30;
          yc = sy * 30;
          a = 11;
          i = 9;
        }
      }
    }
    if (xc > -1 && xc < 30 && yc > -1 && yc < 30) {
      c = false;
    } else {
      tft.drawBitmap(xc, yc, coin, 30, 30, ST77XX_YELLOW);
      c = true;
    }
  }
  if (v == false) {
    zvukbool = true;
    zv = 1;
    tmrzvuk = millis();
    zvuk();
    zvukbool = true;
    zv = 1;
    tmrzvuk = millis();
    zvuk();
    vkord = random(1, spase);
    while (vkord == kordboost[3] || vkord == kordboost[1] || vkord == kordboost[0]) {
      vkord = random(1, spase);
    }
    kordboost[2] = vkord;
    dlav = 0;
    for (int i = 0; i < 8; i++) {
      sy = i;
      for (int a = 0; a < 10; a++) {
        sx = a;
        if (fm == 0) {
          xys = map1PM[sy][sx];
        } else if (fm == 1) {
          xys = map2PM[sy][sx];
        } else {
          xys = map3PM[sy][sx];
        }
        if (xys == 0) {
          dlav = dlav + 1;
        }
        if (dlav == vkord - 1) {
          xv = sx * 30;
          yv = sy * 30;
          a = 11;
          i = 9;
        }
      }
    }
    if (xv > -1 && xv < 30 && yv > -1 && yv < 30) {
      v = false;
    } else {
      tft.drawBitmap(xv, yv, vishnya, 30, 30, ST77XX_RED);
      v = true;
    }
  }
  if (hep == false && s > sstst + 20) {
    zvukbool = true;
    zv = 1;
    tmrzvuk = millis();
    zvuk();
    sstst = s;
    hkord = random(1, spase);
    while (hkord == kordboost[2] || hkord == kordboost[1] || hkord == kordboost[0]) {
      hkord = random(1, spase);
    }
    kordboost[3] = hkord;
    dlah = 0;
    for (int i = 0; i < 8; i++) {
      sy = i;
      for (int a = 0; a < 10; a++) {
        sx = a;
        if (fm == 0) {
          xys = map1PM[sy][sx];
        } else if (fm == 1) {
          xys = map2PM[sy][sx];
        } else {
          xys = map3PM[sy][sx];
        }
        if (xys == 0) {
          dlah = dlah + 1;
        }
        if (dlah == hkord - 1) {
          xh = sx * 30;
          yh = sy * 30;
          a = 12;
          i = 9;
        }
      }
    }
    if (xh > -1 && xh < 30 && yh > -1 && yh < 30) {
      hep = false;
    } else {
      tft.drawBitmap(xh, yh, plshp, 30, 30, ST77XX_RED);
      hep = true;
    }
  }
}

void nachpac() {
  tft.fillScreen(ST77XX_BLACK);
  spase = 0;
  for (int i = 0; i < 8; i++) {
    sy = i;
    for (int a = 0; a < 11; a++) {
      sx = a;
      if (fm == 0) {
        xys = map1PM[sy][sx];
      } else if (fm == 1) {
        xys = map2PM[sy][sx];
      } else {
        xys = map3PM[sy][sx];
      }
      if (xys == 0) {
        spase = spase + 1;
      } else if (xys == 1) {
        tft.drawBitmap(sx * 30, sy * 30, stena1, 30, 30, 0x915C);
      } else if (xys == 2) {
        tft.drawBitmap(sx * 30, sy * 30, stena2, 30, 30, 0x915C);
      } else if (xys == 3) {
        tft.drawBitmap(sx * 30, sy * 30, stena3, 30, 30, 0x915C);
      } else if (xys == 4) {
        tft.drawBitmap(sx * 30, sy * 30, stena4, 30, 30, 0x915C);
      } else if (xys == 5) {
        tft.drawBitmap(sx * 30, sy * 30, stena5, 30, 30, 0x915C);
      } else if (xys == 6) {
        tft.drawBitmap(sx * 30, sy * 30, stena6, 30, 30, 0x915C);
      } else if (xys == 7) {
        tft.drawBitmap(sx * 30, sy * 30, stena7, 30, 30, 0x915C);
      } else if (xys == 8) {
        tft.drawBitmap(sx * 30, sy * 30, stena8, 30, 30, 0x915C);
      } else if (xys == 9) {
        tft.drawBitmap(sx * 30, sy * 30, stena9, 30, 30, 0x915C);
      } else if (xys == 10) {
        tft.drawBitmap(sx * 30, sy * 30, stena10, 30, 30, 0x915C);
      } else if (xys == 11) {
        tft.drawBitmap(sx * 30, sy * 30, stena11, 30, 30, 0x915C);
      } else if (xys == 12) {
        tft.drawBitmap(sx * 30, sy * 30, stena12, 30, 30, 0x915C);
      } else if (xys == 13) {
        tft.drawBitmap(sx * 30, sy * 30, stena13, 30, 30, 0x915C);
      } else if (xys == 14) {
        tft.drawBitmap(sx * 30, sy * 30, stena14, 30, 30, 0x915C);
      } else if (xys == 15) {
        tft.drawBitmap(sx * 30, sy * 30, stena15, 30, 30, 0x915C);
      } else if (xys == 16) {
        tft.drawBitmap(sx * 30, sy * 30, stena16, 30, 30, 0x915C);
      } else if (xys == -1) {
        x = sx * 30;
        y = sy * 30;
        tft.drawBitmap(x, y, pac1n3, 30, 30, ST77XX_YELLOW);
      } else if (xys == -2) {
        xbot1 = sx * 30;
        ybot1 = sy * 30;
        tft.drawRGBBitmap(xbot1, ybot1, g1n1G, 30, 30);
      } else if (xys == -3) {
        xbot2 = sx * 30;
        ybot2 = sy * 30;
        tft.drawRGBBitmap(xbot2, ybot2, g1n1, 30, 30);
      }
    }
  }
  tft.drawBitmap(302, 0, h, 18, 18, ST77XX_RED);
  tft.drawBitmap(302, 20, h, 18, 18, ST77XX_RED);
  tft.drawBitmap(302, 40, h, 18, 18, ST77XX_RED);
  n = 3;
  nbot1 = 3;
  nbot2 = 3;
  hep = false;
  v = false;
  c = false;
  tab = false;
  while (digitalRead(LEFT_BUTT) == 1 && digitalRead(RIGHT_BUTT) == 1 && digitalRead(DOWN_BUTT) == 1 && digitalRead(UP_BUTT) == 1 && digitalRead(OK_BUTT) == 0) {
    if (digitalRead(HOME_BUTT) == 1) {
      zvukbool = true;
      zv = 2;
      tmrzvuk = millis();
      zvuk();
      igranach = false;
      break;
    }
  }
  if (igranach == false) {} else {
    tft.setCursor(105, 120);
    tft.setTextSize(3);
    tft.setTextColor(ST77XX_RED);
    tft.print("READY");
    delay(1000);
    tft.setCursor(105, 120);
    tft.setTextColor(ST77XX_BLACK);
    tft.print("READY");
    tft.setCursor(135, 120);
    tft.setTextColor(ST77XX_RED);
    tft.print("GO");
    delay(700);
    tft.setCursor(135, 120);
    tft.setTextColor(ST77XX_BLACK);
    tft.print("GO");
    tft.setTextSize(1);
    tft.fillRect(295, 208, 60, 20, 0x7BEF);
    tft.setCursor(300, 210);
    tft.setTextColor(ST77XX_WHITE);
    tft.print(s);
    sst = s;
  }
}

void dvizh() {
  if (kill0 == false) {
    if (digitalRead(RIGHT_BUTT) == 0) {
      n = 1;
    } else if (digitalRead(LEFT_BUTT) == 0) {
      n = 2;
    } else if (digitalRead(UP_BUTT) == 0) {
      n = 3;
    } else if (digitalRead(DOWN_BUTT) == 0) {
      n = 4;
    }
    if (n == 1) {
      x = x + 4;
    } else if (n == 2) {
      x = x - 4;
    } else if (n == 3) {
      y = y - 4;
    } else if (n == 4) {
      y = y + 4;
    }
    if (x > 295) {
      tft.fillRect(x - 2, y, 34, 30, ST77XX_BLACK);
      x = 0;
    } else if (x < -5) {
      tft.fillRect(x - 2, y, 34, 30, ST77XX_BLACK);
      x = 290;
    }
    if (y > 215) {
      tft.fillRect(x, y - 2, 30, 34, ST77XX_BLACK);
      y = 0;
    } else if (y < -5) {
      tft.fillRect(x, y - 2, 30, 34, ST77XX_BLACK);
      y = 210;
    }
    if (x + 27 > xbot1 && x < xbot1 + 27 && y + 27 > ybot1 && y < ybot1 + 27) {
      if (edapr1 == false) {
        hp = hp - 1;
        kill0 = true;
      } else {
        kill1 = true;
      }
    }
    if (x + 27 > xbot2 && x < xbot2 + 27 && y + 27 > ybot2 && y < ybot2 + 27) {
      if (edapr2 == false) {
        hp = hp - 1;
        kill0 = true;
      } else {
        kill2 = true;
      }
    }
    if (x + 30 > xt + 10 && x < xt + 20 && y + 30 > yt + 10 && y < yt + 20) {
      tmr2 = millis();
      edapr1 = true;
      edapr2 = true;
      tft.drawBitmap(xt, yt, tabletka, 30, 30, ST77XX_BLACK);
      tab = false;
      xt = 500;
      yt = 500;
    }
    if (millis() - tmr2 > 7000) {
      edapr1 = false;
      edapr2 = false;
    }
    if (x + 30 > xc + 10 && x < xc + 20 && y + 30 > yc + 10 && y < yc + 20) {
      tft.drawBitmap(xc, yc, coin, 30, 30, ST77XX_BLACK);
      c = false;
      s = s + 1;
      xc = 500;
      yc = 500;
    }
    if (x + 30 > xv + 10 && x < xv + 20 && y + 30 > yv + 10 && y < yv + 20) {
      tft.drawBitmap(xv, yv, vishnya, 30, 30, ST77XX_BLACK);
      v = false;
      s = s + 4;
      xv = 500;
      yv = 500;
    }
    if (x + 30 > xh + 10 && x < xh + 20 && y + 30 > yh + 10 && y < yh + 20) {
      tft.drawBitmap(xh, yh, plshp, 30, 30, ST77XX_BLACK);
      hep = false;
      s = s + 1;
      hp = hp + 1;
      xh = 500;
      yh = 500;
    }
    sy = 0;
    sx = 0;
    while (sy < 8) {
      while (sx < 10) {
        if (fm == 0) {
          SR = map1PM[sy][sx];
        } else if (fm == 1) {
          SR = map2PM[sy][sx];
        } else {
          SR = map3PM[sy][sx];
        }
        if (SR > 0) {
          xp = sx * 30;
          yp = sy * 30;
          //tft.fillRect(xp - 29, yp - 29, 60, 60, ST77XX_RED);
          if (x + 28 > xp + 3 && y + 28 > yp + 3 && x + 2 < xp + 26 && y + 2 < yp + 26) {
            x = xst;
            y = yst;
          }
          //tft.fillRect(xp - 29, yp - 29, 60, 60, ST77XX_BLACK);
        }
        sx = sx + 1;
      }
      sy = sy + 1;
      sx = 0;
    }
    if (xst != x || yst != y) {
      if (e2 == false) {
        if (n == 1) {
          tft.drawBitmap(x, y, pac1n1, 30, 30, ST77XX_YELLOW);
          tft.drawBitmap(x, y, pacf1n1, 30, 30, ST77XX_BLACK);
          tft.drawLine(x - 1, y, x - 1, y + 30, ST77XX_BLACK);
        } else if (n == 2) {
          tft.drawBitmap(x, y, pac1n2, 30, 30, ST77XX_YELLOW);
          tft.drawBitmap(x, y, pacf1n2, 30, 30, ST77XX_BLACK);
          tft.drawLine(x + 30, y, x + 30, y + 30, ST77XX_BLACK);
        } else if (n == 3) {
          tft.drawBitmap(x, y, pac1n3, 30, 30, ST77XX_YELLOW);
          tft.drawBitmap(x, y, pacf1n3, 30, 30, ST77XX_BLACK);
          tft.drawLine(x, y + 30, x + 30, y + 30, ST77XX_BLACK);
        } else if (n == 4) {
          tft.drawBitmap(x, y, pac1n4, 30, 30, ST77XX_YELLOW);
          tft.drawBitmap(x, y, pacf1n4, 30, 30, ST77XX_BLACK);
          tft.drawLine(x, y - 1, x + 30, y - 1, ST77XX_BLACK);
        }
      } else {
        if (n == 1) {
          tft.drawBitmap(x, y, pac2n1, 30, 30, ST77XX_YELLOW);
          tft.drawBitmap(x, y, pacf2n1, 30, 30, ST77XX_BLACK);
          tft.drawLine(x - 1, y, x - 1, y + 30, ST77XX_BLACK);
        } else if (n == 2) {
          tft.drawBitmap(x, y, pac2n2, 30, 30, ST77XX_YELLOW);
          tft.drawBitmap(x, y, pacf2n2, 30, 30, ST77XX_BLACK);
          tft.drawLine(x + 30, y, x + 30, y + 30, ST77XX_BLACK);
        } else if (n == 3) {
          tft.drawBitmap(x, y, pac2n3, 30, 30, ST77XX_YELLOW);
          tft.drawBitmap(x, y, pacf2n3, 30, 30, ST77XX_BLACK);
          tft.drawLine(x, y + 30, x + 30, y + 30, ST77XX_BLACK);
        } else if (n == 4) {
          tft.drawBitmap(x, y, pac2n4, 30, 30, ST77XX_YELLOW);
          tft.drawBitmap(x, y, pacf2n4, 30, 30, ST77XX_BLACK);
          tft.drawLine(x, y - 1, x + 30, y - 1, ST77XX_BLACK);
        }
      }
    }
    xst = x;
    yst = y;
  } else {
    kill0 = false;
    tft.fillRect(x, y, 30, 30, ST77XX_BLACK);
    for (int i = 0; i < 8; i++) {
      sy = i;
      for (int a = 0; a < 10; a++) {
        sx = a;
        if (fm == 0) {
          xys = map1PM[sy][sx];
        } else if (fm == 1) {
          xys = map2PM[sy][sx];
        } else {
          xys = map3PM[sy][sx];
        }
        if (xys == -1) {
          x = sx * 30;
          y = sy * 30;
          i = 8;
          a = 10;
        }
      }
    }
  }
}

void pacbot1() {
  if (kill1 == false) {
    if (nbot1 == 1) {
      xbot1 = xbot1 + 3;
    } else if (nbot1 == 2) {
      xbot1 = xbot1 - 3;
    } else if (nbot1 == 3) {
      ybot1 = ybot1 - 3;
    } else if (nbot1 == 4) {
      ybot1 = ybot1 + 3;
    }
    if (xbot1 > 295) {
      tft.fillRect(xbot1 - 2, ybot1, 34, 30, ST77XX_BLACK);
      xbot1 = 0;
    } else if (xbot1 < -5) {
      tft.fillRect(xbot1 - 2, ybot1, 34, 30, ST77XX_BLACK);
      xbot1 = 290;
    }
    if (ybot1 > 215) {
      tft.fillRect(xbot1, ybot1 - 2, 30, 34, ST77XX_BLACK);
      ybot1 = 0;
    } else if (ybot1 < -5) {
      tft.fillRect(xbot1, ybot1 - 2, 30, 34, ST77XX_BLACK);
      ybot1 = 210;
    }
    if (xbot1 + 30 > xbot2 && xbot1 < xbot2 + 30 && ybot1 + 30 > ybot2 && ybot1 < ybot2 + 30) {
      xbot1 = xbotst1;
      ybot1 = ybotst1;
    }
    if (xbot1 + 30 > xt + 10 && xbot1 < xt + 20 && ybot1 + 30 > yt + 10 && ybot1 < yt + 20) {
      tft.drawBitmap(xt, yt, tabletka, 30, 30, ST77XX_BLACK);
      tab = false;
      xt = 500;
      yt = 500;
    }
    if (xbot1 + 30 > xc + 10 && xbot1 < xc + 20 && ybot1 + 30 > yc + 10 && ybot1 < yc + 20) {
      tft.drawBitmap(xc, yc, coin, 30, 30, ST77XX_BLACK);
      c = false;
      xc = 500;
      yc = 500;
    }
    if (xbot1 + 30 > xv + 10 && xbot1 < xv + 20 && ybot1 + 30 > yv + 10 && ybot1 < yv + 20) {
      tft.drawBitmap(xv, yv, vishnya, 30, 30, ST77XX_BLACK);
      v = false;
      xv = 500;
      yv = 500;
    }
    if (xbot1 + 30 > xh + 10 && xbot1 < xh + 20 && ybot1 + 30 > yh + 10 && ybot1 < yh + 20) {
      tft.drawBitmap(xh, yh, plshp, 30, 30, ST77XX_BLACK);
      hep = false;
      xh = 500;
      yh = 500;
    }
    sy = 0;
    sx = 0;
    while (sy < 8) {
      while (sx < 10) {
        if (fm == 0) {
          SR = map1PM[sy][sx];
        } else if (fm == 1) {
          SR = map2PM[sy][sx];
        } else {
          SR = map3PM[sy][sx];
        }
        if (SR > 0) {
          xp = sx * 30;
          yp = sy * 30;
          //tft.fillRect(xp - 29, yp - 29, 60, 60, ST77XX_RED);
          if (xbot1 + 28 > xp + 3 && ybot1 + 28 > yp + 3 && xbot1 + 2 < xp + 26 && ybot1 + 2 < yp + 26) {
            xbot1 = xbotst1;
            ybot1 = ybotst1;
          }
          //tft.fillRect(xp - 29, yp - 29, 60, 60, ST77XX_BLACK);
        }
        sx = sx + 1;
      }
      sy = sy + 1;
      sx = 0;
    }
    if (edapr1 == false) {
      if (e2 == true) {
        if (nbot1 == 1) {
          tft.drawRGBBitmap(xbot1, ybot1, g1n1G, 30, 30);
        } else if (nbot1 == 2) {
          tft.drawRGBBitmap(xbot1, ybot1, g1n2G, 30, 30);
        } else if (nbot1 == 3) {
          tft.drawRGBBitmap(xbot1, ybot1, g1n3G, 30, 30);
        } else if (nbot1 == 4) {
          tft.drawRGBBitmap(xbot1, ybot1, g1n4G, 30, 30);
        }
      } else {
        if (nbot1 == 1) {
          tft.drawRGBBitmap(xbot1, ybot1, g2n1G, 30, 30);
        } else if (nbot1 == 2) {
          tft.drawRGBBitmap(xbot1, ybot1, g2n2G, 30, 30);
        } else if (nbot1 == 3) {
          tft.drawRGBBitmap(xbot1, ybot1, g2n3G, 30, 30);
        } else if (nbot1 == 4) {
          tft.drawRGBBitmap(xbot1, ybot1, g2n4G, 30, 30);
        }
      }
    } else {
      if (e2 == true) {
        if (nbot1 == 1) {
          tft.drawRGBBitmap(xbot1, ybot1, g1n1B, 30, 30);
        } else if (nbot1 == 2) {
          tft.drawRGBBitmap(xbot1, ybot1, g1n2B, 30, 30);
        } else if (nbot1 == 3) {
          tft.drawRGBBitmap(xbot1, ybot1, g1n3B, 30, 30);
        } else if (nbot1 == 4) {
          tft.drawRGBBitmap(xbot1, ybot1, g1n4B, 30, 30);
        }
      } else {
        if (nbot1 == 1) {
          tft.drawRGBBitmap(xbot1, ybot1, g2n1B, 30, 30);
        } else if (nbot1 == 2) {
          tft.drawRGBBitmap(xbot1, ybot1, g2n2B, 30, 30);
        } else if (nbot1 == 3) {
          tft.drawRGBBitmap(xbot1, ybot1, g2n3B, 30, 30);
        } else if (nbot1 == 4) {
          tft.drawRGBBitmap(xbot1, ybot1, g2n4B, 30, 30);
        }
      }
    }
    if (xbotst1 == xbot1 && ybotst1 == ybot1) {
      nbot1 = random(1, 5);
    }
    xbotst1 = xbot1;
    ybotst1 = ybot1;
  } else {
    zvukbool = true;
    zv = 1;
    tmrzvuk = millis();
    zvuk();
    s = s + 8;
    kill1 = false;
    edapr1 = false;
    tft.fillRect(xbot1, ybot1, 30, 30, ST77XX_BLACK);
    for (int i = 0; i < 8; i++) {
      sy = i;
      for (int a = 0; a < 10; a++) {
        sx = a;
        if (fm == 0) {
          xys = map1PM[sy][sx];
        } else if (fm == 1) {
          xys = map2PM[sy][sx];
        } else {
          xys = map3PM[sy][sx];
        }
        if (xys == -2) {
          xbot1 = sx * 30;
          ybot1 = sy * 30;
          i = 8;
          a = 10;
        }
      }
    }
  }
}

void pacbot2() {
  if (kill2 == false) {
    if (nbot2 == 1) {
      xbot2 = xbot2 + 3;
    } else if (nbot2 == 2) {
      xbot2 = xbot2 - 3;
    } else if (nbot2 == 3) {
      ybot2 = ybot2 - 3;
    } else if (nbot2 == 4) {
      ybot2 = ybot2 + 3;
    }
    if (xbot2 > 295) {
      tft.fillRect(xbot2 - 2, ybot2, 34, 30, ST77XX_BLACK);
      xbot2 = 0;
    } else if (xbot2 < -5) {
      tft.fillRect(xbot2 - 2, ybot2, 34, 30, ST77XX_BLACK);
      xbot2 = 290;
    }
    if (ybot2 > 215) {
      tft.fillRect(xbot2, ybot2 - 2, 30, 34, ST77XX_BLACK);
      ybot2 = 0;
    } else if (ybot2 < -5) {
      tft.fillRect(xbot2, ybot2 - 2, 30, 34, ST77XX_BLACK);
      ybot2 = 210;
    }
    if (xbot2 + 30 > xbot1 && xbot2 < xbot1 + 30 && ybot2 + 30 > ybot1 && ybot2 < ybot1 + 30) {
      xbot2 = xbotst2;
      ybot2 = ybotst2;
    }
    if (xbot2 + 30 > xt + 10 && xbot2 < xt + 20 && ybot2 + 30 > yt + 10 && ybot2 < yt + 20) {
      tft.drawBitmap(xt, yt, tabletka, 30, 30, ST77XX_BLACK);
      tab = false;
      xt = 500;
      yt = 500;
    }
    if (xbot2 + 30 > xc + 10 && xbot2 < xc + 20 && ybot2 + 30 > yc + 10 && ybot2 < yc + 20) {
      tft.drawBitmap(xc, yc, coin, 30, 30, ST77XX_BLACK);
      c = false;
      xc = 500;
      yc = 500;
    }
    if (xbot2 + 30 > xv + 10 && xbot2 < xv + 20 && ybot2 + 30 > yv + 10 && ybot2 < yv + 20) {
      tft.drawBitmap(xv, yv, vishnya, 30, 30, ST77XX_BLACK);
      v = false;
      xv = 500;
      yv = 500;
    }
    if (xbot2 + 30 > xh + 10 && xbot2 < xh + 20 && ybot2 + 30 > yh + 10 && ybot2 < yh + 20) {
      tft.drawBitmap(xh, yh, plshp, 30, 30, ST77XX_BLACK);
      hep = false;
      xh = 500;
      yh = 500;
    }
    sy = 0;
    sx = 0;
    while (sy < 8) {
      while (sx < 10) {
        if (fm == 0) {
          SR = map1PM[sy][sx];
        } else if (fm == 1) {
          SR = map2PM[sy][sx];
        } else {
          SR = map3PM[sy][sx];
        }
        if (SR > 0) {
          xp = sx * 30;
          yp = sy * 30;
          //tft.fillRect(xp - 29, yp - 29, 60, 60, ST77XX_RED);
          if (xbot2 + 28 > xp + 3 && ybot2 + 28 > yp + 3 && xbot2 + 2 < xp + 26 && ybot2 + 2 < yp + 26) {
            xbot2 = xbotst2;
            ybot2 = ybotst2;
          }
          //tft.fillRect(xp - 29, yp - 29, 60, 60, ST77XX_BLACK);
        }
        sx = sx + 1;
      }
      sy = sy + 1;
      sx = 0;
    }
    if (edapr2 == false) {
      if (e2 == true) {
        if (nbot2 == 1) {
          tft.drawRGBBitmap(xbot2, ybot2, g1n1, 30, 30);
        } else if (nbot2 == 2) {
          tft.drawRGBBitmap(xbot2, ybot2, g1n2, 30, 30);
        } else if (nbot2 == 3) {
          tft.drawRGBBitmap(xbot2, ybot2, g1n3, 30, 30);
        } else if (nbot2 == 4) {
          tft.drawRGBBitmap(xbot2, ybot2, g1n4, 30, 30);
        }
      } else {
        if (nbot2 == 1) {
          tft.drawRGBBitmap(xbot2, ybot2, g2n1, 30, 30);
        } else if (nbot2 == 2) {
          tft.drawRGBBitmap(xbot2, ybot2, g2n2, 30, 30);
        } else if (nbot2 == 3) {
          tft.drawRGBBitmap(xbot2, ybot2, g2n3, 30, 30);
        } else if (nbot2 == 4) {
          tft.drawRGBBitmap(xbot2, ybot2, g2n4, 30, 30);
        }
      }
    } else {
      if (e2 == true) {
        if (nbot2 == 1) {
          tft.drawRGBBitmap(xbot2, ybot2, g1n1B, 30, 30);
        } else if (nbot2 == 2) {
          tft.drawRGBBitmap(xbot2, ybot2, g1n2B, 30, 30);
        } else if (nbot2 == 3) {
          tft.drawRGBBitmap(xbot2, ybot2, g1n3B, 30, 30);
        } else if (nbot2 == 4) {
          tft.drawRGBBitmap(xbot2, ybot2, g1n4B, 30, 30);
        }
      } else {
        if (nbot2 == 1) {
          tft.drawRGBBitmap(xbot2, ybot2, g2n1B, 30, 30);
        } else if (nbot2 == 2) {
          tft.drawRGBBitmap(xbot2, ybot2, g2n2B, 30, 30);
        } else if (nbot2 == 3) {
          tft.drawRGBBitmap(xbot2, ybot2, g2n3B, 30, 30);
        } else if (nbot2 == 4) {
          tft.drawRGBBitmap(xbot2, ybot2, g2n4B, 30, 30);
        }
      }
    }
    if (xbotst2 == xbot2 && ybotst2 == ybot2) {
      nbot2 = random(1, 5);
    }
    xbotst2 = xbot2;
    ybotst2 = ybot2;
  } else {
    zvukbool = true;
    zv = 1;
    tmrzvuk = millis();
    zvuk();
    s = s + 8;
    kill2 = false;
    edapr2 = false;
    tft.fillRect(xbot2, ybot2, 30, 30, ST77XX_BLACK);
    for (int i = 0; i < 8; i++) {
      sy = i;
      for (int a = 0; a < 10; a++) {
        sx = a;
        if (fm == 0) {
          xys = map1PM[sy][sx];
        } else if (fm == 1) {
          xys = map2PM[sy][sx];
        } else {
          xys = map3PM[sy][sx];
        }
        if (xys == -3) {
          xbot2 = sx * 30;
          ybot2 = sy * 30;
          i = 8;
          a = 10;
        }
      }
    }
  }
}

void stops() {
  tft.drawRoundRect(54, 60, 212, 120, 10, ST77XX_WHITE);
  tft.drawRoundRect(55, 61, 210, 118, 9, ST77XX_WHITE);
  tft.drawRoundRect(56, 62, 208, 116, 8, ST77XX_BLACK);
  tft.drawRoundRect(57, 63, 206, 114, 7, ST77XX_BLACK);
  tft.drawRoundRect(58, 64, 204, 112, 6, ST77XX_WHITE);
  tft.drawRoundRect(59, 65, 202, 110, 5, ST77XX_WHITE);
  tft.fillRoundRect(60, 66, 200, 108, 4, ST77XX_BLACK);
  tft.drawBitmap(62, 72, go, 196, 25, ST77XX_WHITE);
  tft.setCursor(100, 120);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);
  tft.print("skore = " + String(s));
  topskore = pref.getUInt("skorePM", 0);
  tft.setCursor(100, 140);
  tft.print("top skore = " + String(topskore));
  if (s > topskore) {
    pref.putUInt("skorePM", s);
  }
  delay(200);
  while (digitalRead(OK_BUTT) == 0) {
    if (digitalRead(HOME_BUTT) == 1) {
      zvukbool = true;
      zv = 2;
      tmrzvuk = millis();
      zvuk();
      igranach = false;
      break;
    }
  }
  if (igranach == false) {} else {
    igrapac();
  }
}

void nachalokart() {
  for (int i = 0; i < 24; i++) {
    sy = i;
    for (int a = 0; a < 32; a++) {
      sx = a;
      if (fm == 0) {
        forMapSB[sy][sx] = map1SB[sy][sx];
      } else if (fm == 1) {
        forMapSB[sy][sx] = map2SB[sy][sx];
      } else if (fm == 2) {
        forMapSB[sy][sx] = map3SB[sy][sx];
      }
    }
  }
  nachsboll();
}

void nachsboll() {
  tft.fillScreen(0x07FF);
  spase = 0;
  for (int i = 0; i < 24; i++) {
    sy = i;
    for (int a = 0; a < 32; a++) {
      sx = a;
      xys = forMapSB[sy][sx];
      if (xys == 0) {
        spase = spase + 1;
      } else if (xys == 1) {
        tft.drawBitmap(sx * 10, sy * 10, sbs21, 10, 10, 0xC618);
        tft.drawBitmap(sx * 10, sy * 10, sbs22, 10, 10, 0x7BEF);
      } else if (xys == 2) {
        tft.drawBitmap(sx * 10, sy * 10, sbs11, 10, 10, ST77XX_GREEN);
        tft.drawBitmap(sx * 10, sy * 10, sbs12, 10, 10, 0x03E0);
      } else if (xys == 3) {
        tft.drawBitmap(sx * 10, sy * 10, sbs41, 10, 10, 0x001F);
        tft.drawBitmap(sx * 10, sy * 10, sbs42, 10, 10, 0x000F);
      } else if (xys == 4) {
        tft.drawBitmap(sx * 10, sy * 10, sbs31, 10, 10, ST77XX_ORANGE);
        tft.drawBitmap(sx * 10, sy * 10, sbs32, 10, 10, 0x7800);
      } else if (xys == 5) {
        tft.drawBitmap(sx * 10, sy * 10, dlaeffekt, 10, 10, 0xC618);
      } else if (xys == 6) {
        tft.drawBitmap(sx * 10, sy * 10, sbs31, 10, 10, ST77XX_RED);
        tft.drawBitmap(sx * 10, sy * 10, sbs32, 10, 10, 0xA800);
      } else if (xys == 7) {
        tft.drawBitmap(sx * 10, sy * 10, sbs31, 10, 10, 0xFC30);
        tft.drawBitmap(sx * 10, sy * 10, sbs32, 10, 10, 0xCB4D);
      } else if (xys == 8) {
        tft.drawBitmap(sx * 10, sy * 10, sbs11, 10, 10, 0x0000);
        tft.drawBitmap(sx * 10, sy * 10, sbs12, 10, 10, 0x39C7);
      } else if (xys == 9) {
        tft.drawBitmap(sx * 10, sy * 10, sbs31, 10, 10, 0x4020);
        tft.drawBitmap(sx * 10, sy * 10, sbs32, 10, 10, 0x3020);
      } else if (xys == 10) {
        tft.drawBitmap(sx * 10, sy * 10, sbs31, 10, 10, 0x9A42);
        tft.drawBitmap(sx * 10, sy * 10, sbs32, 10, 10, 0x79C1);
      } else if (xys == 11) {
        tft.drawBitmap(sx * 10, sy * 10, sbs31, 10, 10, 0xCB63);
        tft.drawBitmap(sx * 10, sy * 10, sbs32, 10, 10, 0xB303);
      } else if (xys == 12) {
        tft.drawBitmap(sx * 10, sy * 10, sbs31, 10, 10, 0xE465);
        tft.drawBitmap(sx * 10, sy * 10, sbs32, 10, 10, 0xCC05);
      }
    }
  }
  tft.drawBitmap(290, 0, effekt2, 30, 30, ST77XX_WHITE);
  tft.drawBitmap(290, 30, effekt3, 30, 30, ST77XX_YELLOW);
  tft.drawBitmap(290, 60, effekt1, 30, 30, ST77XX_WHITE);
  tft.drawBitmap(290, 90, effekt4, 30, 30, ST77XX_WHITE);
  tft.drawBitmap(290, 120, effekt5, 30, 30, ST77XX_WHITE);
  tft.drawBitmap(290, 150, effekt6, 30, 30, ST77XX_WHITE);
  tft.drawBitmap(290, 180, coinsb, 10, 10, ST77XX_MAGENTA);
  tft.drawBitmap(300, 180, coinsb, 10, 10, ST77XX_MAGENTA);
  tft.drawBitmap(310, 180, coinsb, 10, 10, ST77XX_MAGENTA);
  tft.fillRect(290, 190, 30, 50, 0xC618);
  armoreffekt = false;
  hp = 3;
  roketsize = 1;
}

void boostsb() {
  if (rokbig == false) {
    kord = random(1, spase);
    /*while (kord == kordboostsb[5] || kord == kordboostsb[4] || kord == kordboostsb[3] || kord == kordboostsb[2] || kord == kordboostsb[1]) {
      kord = random(1, spase);
      }*/
    //kordboostsb[0] = kord;
    dlaspase = 0;
    for (int i = 0; i < 24; i++) {
      sy = i;
      for (int a = 0; a < 32; a++) {
        sx = a;
        xys = forMapSB[sy][sx];
        if (xys == 0) {
          dlaspase = dlaspase + 1;
        }
        if (dlaspase == kord - 1) {
          xboost = sx * 10;
          yboost = sy * 10;
          forMapSB[sy][sx] = 20;
          a = 32;
          i = 24;
        }
      }
    }
    tft.drawBitmap(xboost, yboost, lakyblok1, 10, 10, 0xFFFF);
    tft.drawBitmap(xboost, yboost, lakyblok2, 10, 10, 0xFEA0);
    rokbig = true;
  }
  if (rokmin == false) {
    kord = random(1, spase);
    /*while (kord == kordboostsb[5] || kord == kordboostsb[4] || kord == kordboostsb[3] || kord == kordboostsb[2] || kord == kordboostsb[0]) {
      kord = random(1, spase);
      }*/
    //kordboostsb[1] = kord;
    dlaspase = 0;
    for (int i = 0; i < 24; i++) {
      sy = i;
      for (int a = 0; a < 32; a++) {
        sx = a;
        xys = forMapSB[sy][sx];
        if (xys == 0) {
          dlaspase = dlaspase + 1;
        }
        if (dlaspase == kord - 1) {
          xboost = sx * 10;
          yboost = sy * 10;
          forMapSB[sy][sx] = 21;
          a = 32;
          i = 24;
        }
      }
    }
    tft.drawBitmap(xboost, yboost, lakyblok1, 10, 10, 0xFFFF);
    tft.drawBitmap(xboost, yboost, lakyblok2, 10, 10, 0xFEA0);
    rokmin = true;
  }
  if (bollfet == false) {
    kord = random(1, spase);
    /*while (kord == kordboostsb[5] || kord == kordboostsb[4] || kord == kordboostsb[3] || kord == kordboostsb[1] || kord == kordboostsb[2]) {
      kord = random(1, spase);
      }*/
    //kordboostsb[2] = kord;
    dlaspase = 0;
    for (int i = 0; i < 24; i++) {
      sy = i;
      for (int a = 0; a < 32; a++) {
        sx = a;
        xys = forMapSB[sy][sx];
        if (xys == 0) {
          dlaspase = dlaspase + 1;
        }
        if (dlaspase == kord - 1) {
          xboost = sx * 10;
          yboost = sy * 10;
          forMapSB[sy][sx] = 22;
          a = 32;
          i = 24;
        }
      }
    }
    tft.drawBitmap(xboost, yboost, lakyblok1, 10, 10, 0xFFFF);
    tft.drawBitmap(xboost, yboost, lakyblok2, 10, 10, 0xFEA0);
    bollfet = true;
  }
  if (bolllight == false) {
    kord = random(1, spase);
    /*while (kord == kordboostsb[5] || kord == kordboostsb[4] || kord == kordboostsb[2] || kord == kordboostsb[1] || kord == kordboostsb[0]) {
      kord = random(1, spase);
      }*/
    //kordboostsb[3] = kord;
    dlaspase = 0;
    for (int i = 0; i < 24; i++) {
      sy = i;
      for (int a = 0; a < 32; a++) {
        sx = a;
        xys = forMapSB[sy][sx];
        if (xys == 0) {
          dlaspase = dlaspase + 1;
        }
        if (dlaspase == kord - 1) {
          xboost = sx * 10;
          yboost = sy * 10;
          forMapSB[sy][sx] = 23;
          a = 32;
          i = 24;
        }
      }
    }
    tft.drawBitmap(xboost, yboost, lakyblok1, 10, 10, 0xFFFF);
    tft.drawBitmap(xboost, yboost, lakyblok2, 10, 10, 0xFEA0);
    bolllight = true;
  }
  if (armor == false) {
    kord = random(1, spase);
    /*while (kord == kordboostsb[5] || kord == kordboostsb[4] || kord == kordboostsb[2] || kord == kordboostsb[1] || kord == kordboostsb[0]) {
      kord = random(1, spase);
      }*/
    //kordboostsb[5] = kord;
    dlaspase = 0;
    for (int i = 0; i < 24; i++) {
      sy = i;
      for (int a = 0; a < 32; a++) {
        sx = a;
        xys = forMapSB[sy][sx];
        if (xys == 0) {
          dlaspase = dlaspase + 1;
        }
        if (dlaspase == kord - 1) {
          xboost = sx * 10;
          yboost = sy * 10;
          forMapSB[sy][sx] = 24;
          a = 32;
          i = 24;
        }
      }
    }
    tft.drawBitmap(xboost, yboost, lakyblok1, 10, 10, 0xFFFF);
    tft.drawBitmap(xboost, yboost, lakyblok2, 10, 10, 0xFEA0);
    armor = true;
  }
  if (many == false) {
    kord = random(1, spase);
    /*while (kord == kordboostsb[5] || kord == kordboostsb[4] || kord == kordboostsb[3] || kord == kordboostsb[1] || kord == kordboostsb[2]) {
      kord = random(1, spase);
      }*/
    //kordboostsb[3] = kord;
    dlaspase = 0;
    for (int i = 0; i < 24; i++) {
      sy = i;
      for (int a = 0; a < 32; a++) {
        sx = a;
        xys = forMapSB[sy][sx];
        if (xys == 0) {
          dlaspase = dlaspase + 1;
        }
        if (dlaspase == kord - 1) {
          xboost = sx * 10;
          yboost = sy * 10;
          forMapSB[sy][sx] = 25;
          a = 32;
          i = 24;
        }
      }
    }
    tft.drawBitmap(xboost, yboost, lakyblok1, 10, 10, 0xFFFF);
    tft.drawBitmap(xboost, yboost, lakyblok2, 10, 10, 0xFEA0);
    many = true;
  }
}

void roketsb() {
  if (digitalRead(DOWN_BUTT) == 0) {
    y = y + 4;
    if (y > 198) {
      y = y - 4;
    } else {
      if (armoreffekt == false) {
        if (roketsize == 0) {
          tft.drawBitmap(12, y, roketkamap2size2, 8, 44, 0x07FF);
        } else if (roketsize == 1) {
          tft.drawBitmap(12, y, roketkamap2size1, 8, 44, 0x07FF);
        } else if (roketsize == 2) {
          tft.drawBitmap(12, y, roketkamap2, 8, 44, 0x07FF);
        }
      } else {
        if (roketsize == 0) {
          tft.drawBitmap(12, y, roketkamap2armoronsize2, 8, 44, 0x07FF);
        } else if (roketsize == 1) {
          tft.drawBitmap(12, y, roketkamap2armoronsize1, 8, 44, 0x07FF);
        } else if (roketsize == 2) {
          tft.drawBitmap(12, y, roketkamap2armoron, 8, 44, 0x07FF);
        }
      }
    }
  } else if (digitalRead(UP_BUTT) == 0) {
    y = y - 4;
    if (y < -2) {
      y = y + 4;
    } else {
      if (armoreffekt == false) {
        if (roketsize == 0) {
          tft.drawBitmap(12, y, roketkamap2size2, 8, 44, 0x07FF);
        } else if (roketsize == 1) {
          tft.drawBitmap(12, y, roketkamap2size1, 8, 44, 0x07FF);
        } else if (roketsize == 2) {
          tft.drawBitmap(12, y, roketkamap2, 8, 44, 0x07FF);
        }
      } else {
        if (roketsize == 0) {
          tft.drawBitmap(12, y, roketkamap2armoronsize2, 8, 44, 0x07FF);
        } else if (roketsize == 1) {
          tft.drawBitmap(12, y, roketkamap2armoronsize1, 8, 44, 0x07FF);
        } else if (roketsize == 2) {
          tft.drawBitmap(12, y, roketkamap2armoron, 8, 44, 0x07FF);
        }
      }
    }
  }
  if (armoreffekt == false) {
    if (roketsize == 0) {
      tft.drawBitmap(12, y, roketkamapsize2, 8, 44, ST77XX_BLUE);
    } else if (roketsize == 1) {
      tft.drawBitmap(12, y, roketkamapsize1, 8, 44, ST77XX_BLUE);
    } else if (roketsize == 2) {
      tft.drawBitmap(12, y, roketkamap, 8, 44, ST77XX_BLUE);
    }
  } else {
    if (roketsize == 0) {
      tft.drawBitmap(12, y, roketkamaparmoronsize2, 8, 44, ST77XX_BLUE);
    } else if (roketsize == 1) {
      tft.drawBitmap(12, y, roketkamaparmoronsize1, 8, 44, ST77XX_BLUE);
    } else if (roketsize == 2) {
      tft.drawBitmap(12, y, roketkamaparmoron, 8, 44, ST77XX_BLUE);
    }
  }
}

void popalsb() {
  if (kontakt == false) {
    if (xb > 12 && xb < 26 && yb >= y - 10 && yb <= y + 40) {
      tmr2 = millis();
      kontakt = true;
      zvukbool = true;
      zv = 1;
      tmrzvuk = millis();
      zvuk();
      if (n == 4) {
        n = 1;
      } else if (n == 3) {
        n = 2;
      }
      ran = random(20, 71);
      delitel = 180 / ran;
      arad = PI / delitel;
      xcos = cos(arad);
      ysin = sin(arad);
      Serial.println(ran);
    }
  } else {
    if (millis() - tmr2 > 100) {
      kontakt = false;
    }
  }
}

void bollsb() {
  if (kill0 == false) {
    tft.drawBitmap(xb, yb, boll2, 18, 18, 0x07FF);
    if (e2 == true) {
      tft.drawBitmap(20, y + 11, boll1, 18, 18, 0x07FF);
      tft.drawBitmap(20, y + 11, boll2, 18, 18, 0x07FF);
      xb = 20;
      yb = y + 10;
      ran = random(1, 3);
      n = ran;
      ran = random(0, 45);
      delitel = 180 / ran;
      arad = PI / delitel;
      xcos = cos(arad);
      ysin = sin(arad);
      e2 = false;
    }
    if (n == 1 || n == 2) {
      xb = xb + spee * xcos;
    } else {
      xb = xb - spee * xcos;
    }
    if (n == 3 || n == 2) {
      yb = yb + spee * ysin;
    } else {
      yb = yb - spee * ysin;
    }
    if (xb + 14 > 320) {
      xb = 306;
      zvukbool = true;
      zv = 1;
      tmrzvuk = millis();
      zvuk();
      if (n == 1) {
        n = 4;
      } else if (n == 2) {
        n = 3;
      }
    } else if (armoreffekt == true && xb < 20) {
      xb = 20;
      zvukbool = true;
      zv = 1;
      tmrzvuk = millis();
      zvuk();
      if (n == 4) {
        n = 1;
      } else if (n == 3) {
        n = 2;
      }
    } else if (xb < -14) {
      kill0 = true;
      hp = hp - 1;
    }
    if (yb + 14 > 240) {
      zvukbool = true;
      zv = 1;
      tmrzvuk = millis();
      zvuk();
      yb = 226;
      if (n == 2) {
        n = 1;
      } else if (n == 3) {
        n = 4;
      }
    } else if (yb < -4) {
      zvukbool = true;
      zv = 1;
      tmrzvuk = millis();
      zvuk();
      yb = -4;
      if (n == 1) {
        n = 2;
      } else if (n == 4) {
        n = 3;
      }
    }
    sy = 0;
    sx = 4;
    while (sy < 24) {
      while (sx < 32) {
        xys = forMapSB[sy][sx];
        if (xys > 0) {
          xp = sx * 10;
          yp = sy * 10;
          if (xb + 14 >= xp && yb + 14 >= yp && xb <= xp + 10 && yb <= yp + 10) {
            zvukbool = true;
            zv = 1;
            tmrzvuk = millis();
            zvuk();
            xp = sx;
            yp = sy;
            if ((xys != 1 || xys != 5) && light == false) {
              if (xys == 2 || xys == 4 || xys == 6 || xys == 7 || xys == 8 || xys == 9 || xys == 10 || xys == 11 || xys == 12) {
                tft.fillRect(xp * 10, yp * 10, 10, 10, 0x07FF);
                forMapSB[sy][sx] = -1;
                s = s + 2;
              } else if (xys == 3) {
                tft.drawBitmap(sx * 10, sy * 10, sbs11, 10, 10, ST77XX_GREEN);
                tft.drawBitmap(sx * 10, sy * 10, sbs12, 10, 10, 0x03E0);
                forMapSB[sy][sx] = 2;
                s = s + 2;
              } else if (xys == 20 || xys == 21 || xys == 22 || xys == 23 || xys == 24 || xys == 25) {
                if (xys == 20) {
                  roketsize = roketsize + 1;
                  if (roketsize > 2) {
                    roketsize = 2;
                  }
                  rokbig = false;
                  if (roketsize == 1) {
                    tft.drawBitmap(290, 0, effekt2, 30, 30, ST77XX_WHITE);
                    tft.drawBitmap(290, 30, effekt3, 30, 30, ST77XX_YELLOW);
                  } else if (roketsize == 2) {
                    tft.drawBitmap(290, 30, effekt3, 30, 30, ST77XX_WHITE);
                    tft.drawBitmap(290, 60, effekt1, 30, 30, ST77XX_GREEN);
                  }
                  s = s + 2;
                } else if (xys == 21) {
                  roketsize = roketsize - 1;
                  if (roketsize < 0) {
                    roketsize = 0;
                  }
                  rokmin = false;
                  if (roketsize == 0) {
                    tft.drawBitmap(290, 0, effekt2, 30, 30, ST77XX_RED);
                    tft.drawBitmap(290, 30, effekt3, 30, 30, ST77XX_WHITE);
                  } else if (roketsize == 1) {
                    tft.drawBitmap(290, 30, effekt3, 30, 30, ST77XX_YELLOW);
                    tft.drawBitmap(290, 60, effekt1, 30, 30, ST77XX_WHITE);
                  }
                  s = s + 2;
                } else if (xys == 22) {
                  tmr3 = millis();
                  fet = false;
                  light = true;
                  bollfet = false;
                  tft.drawBitmap(290, 120, effekt5, 30, 30, ST77XX_RED);
                  tft.drawBitmap(290, 150, effekt6, 30, 30, ST77XX_WHITE);
                  s = s + 5;
                } else if (xys == 23) {
                  tmr3 = millis();
                  fet = true;
                  light = false;
                  bolllight = false;
                  tft.drawBitmap(290, 120, effekt5, 30, 30, ST77XX_WHITE);
                  tft.drawBitmap(290, 150, effekt6, 30, 30, ST77XX_GREEN);
                  s = s + 2;
                } else if (xys == 25) {
                  many = false;
                  s = s + 10;
                } else if (xys == 24) {
                  tmr4 = millis();
                  armor = false;
                  armoreffekt = true;
                  tft.fillRect(16, 0, 4, 240, ST77XX_BLUE);
                  tft.drawBitmap(290, 90, effekt4, 30, 30, ST77XX_GREEN);
                  s = s + 2;
                }
                tft.fillRect(xp * 10, yp * 10, 10, 10, 0x07FF);
                forMapSB[sy][sx] = 0;
              }
            }
            xp = sx * 10;
            yp = sy * 10;
            xb = xbst;
            yb = ybst;
            if (fet == false) {
              if (n == 1) {
                if (xb + 14 >= xp && yb >= yp + 10) {
                  n = 2;
                } else if (xb + 14 <= xp && yb <= yp + 10) {
                  n = 4;
                } else if (xb + 14 < xp && yb > yp + 10) {
                  n = 3;
                }
              } else if (n == 2) {
                if (xb + 14 >= xp && yb + 14 <= yp) {
                  n = 1;
                } else if (xb + 14 <= xp && yb + 14 >= yp) {
                  n = 3;
                } else if ( xb + 14 < xp && yb + 14 < yp) {
                  n = 4;
                }
              } else if (n == 3) {
                if (xb <= xp + 10 && yb + 14 <= yp) {
                  n = 4;
                } else if (xb >= xp + 10 && yb + 14 >= yp) {
                  n = 2;
                } else if (xb > xp + 10 && yb + 14 < yp) {
                  n = 1;
                }
              } else if (n == 4) {
                if (xb <= xp + 10 && yb >= yp + 10) {
                  n = 3;
                } else if (xb >= xp + 10 && yb <= yp + 10) {
                  n = 1;
                } else if (xb > yp + 10 && yb > yp + 10) {
                  n = 2;
                }
              }
            } else if (fet == true && (xys == 1 || xys == 5)) {
              if (n == 1) {
                if (xb + 14 >= xp && yb >= yp + 10) {
                  n = 2;
                } else if (xb + 14 <= xp && yb <= yp + 10) {
                  n = 4;
                } else if (xb + 14 < xp && yb > yp + 10) {
                  n = 3;
                }
              } else if (n == 2) {
                if (xb + 14 >= xp && yb + 14 <= yp) {
                  n = 1;
                } else if (xb + 14 <= xp && yb + 14 >= yp) {
                  n = 3;
                } else if ( xb + 14 < xp && yb + 14 < yp) {
                  n = 4;
                }
              } else if (n == 3) {
                if (xb <= xp + 10 && yb + 14 <= yp) {
                  n = 4;
                }
              } else if (n == 4) {
                if (xb <= xp + 10 && yb >= yp + 10) {
                  n = 3;
                } else if (xb >= xp + 10 && yb <= yp + 10) {
                  n = 1;
                } else if (xb > yp + 10 && yb > yp + 10) {
                  n = 2;
                }
              }
            }
            sy = 24;
            sx = 32;
          }
        }
        sx = sx + 1;
      }
      sy = sy + 1;
      sx = 4;
    }
    xbst = xb;
    ybst = yb;
    if (fet == true) {
      tft.drawBitmap(xb, yb, boll2, 18, 18, 0x0000);
    } else if (light == true) {
      tft.drawBitmap(xb, yb, boll2, 18, 18, 0xB7E0);
    } else {
      tft.drawBitmap(xb, yb, boll2, 18, 18, ST77XX_MAGENTA);
    }
  } else {
    tft.drawBitmap(20, y + 11, boll1, 18, 18, 0x07FF);
    tft.drawBitmap(20, y + 11, boll2, 18, 18, ST77XX_MAGENTA);
    if (digitalRead(OK_BUTT) == 1) {
      zvukbool = true;
      zv = 2;
      tmrzvuk = millis();
      zvuk();
      while (digitalRead(OK_BUTT) == 1) {}
      kill0 = false;
      e2 = true;
    }
  }
}

void stopsb() {
  tft.drawRoundRect(54, 60, 212, 120, 10, ST77XX_WHITE);
  tft.drawRoundRect(55, 61, 210, 118, 9, ST77XX_WHITE);
  tft.drawRoundRect(56, 62, 208, 116, 8, ST77XX_BLACK);
  tft.drawRoundRect(57, 63, 206, 114, 7, ST77XX_BLACK);
  tft.drawRoundRect(58, 64, 204, 112, 6, ST77XX_WHITE);
  tft.drawRoundRect(59, 65, 202, 110, 5, ST77XX_WHITE);
  tft.fillRoundRect(60, 66, 200, 108, 4, ST77XX_BLACK);
  tft.drawBitmap(62, 72, go, 196, 25, ST77XX_WHITE);
  tft.setCursor(100, 120);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);
  tft.print("skore = " + String(s));
  tft.setCursor(100, 140);
  topskore = pref.getUInt("skoreSB", 0);
  tft.print("top skore = " + String(topskore));
  if (s > topskore) {
    pref.putUInt("skoreSB", s);
  }
  delay(200);
  while (digitalRead(OK_BUTT) == 0) {
    if (digitalRead(HOME_BUTT) == 1) {
      zvukbool = true;
      zv = 2;
      tmrzvuk = millis();
      zvuk();
      igranach = false;
      break;
    }
  }
  if (igranach == false) {} else {
    igrasb();
  }
}

void duddela() {
  if (digitalRead(RIGHT_BUTT) == 0) {
    x = x + 4;
    n = 1;
    if (x > 294) {
      tft.fillRect(x, y + 18, 46, 36, 0xFF16);
      x = -31;
    }
  } else if (digitalRead(LEFT_BUTT) == 0) {
    x = x - 4;
    n = 2;
    if (x < -31) {
      tft.fillRect(x, y + 18, 46, 36, 0xFF16);
      x = 293;
    }
  }
  if ((x + 46 > xp1 && x < xp1 + 38 && y < yp1 + 18 && y + 46 > yp1) || (x + 46 > xp2 && x < xp2 + 38 && y < yp2 + 18 && y + 46 > yp2) || (x + 46 > xp3 && x < xp3 + 38 && y < yp3 + 18 && y + 46 > yp3) || (x + 46 > xp4 && x < xp4 + 38 && y < yp4 + 18 && y + 46 > yp4)) {
    e2 = true;
  } else {
    if (e2 == true) {
      e2 = false;
      tft.drawRGBBitmap(xp1, yp1, plot, 38, 18);
      tft.drawRGBBitmap(xp2, yp2, plot, 38, 18);
      tft.drawRGBBitmap(xp3, yp3, plot, 38, 18);
      tft.drawRGBBitmap(xp4, yp4, plot, 38, 18);
    }
  }
  if (pad == true) {
    if (y > 160 && ((x + 4 < xp1 + 38 && x + 42 > xp1 && y + 46 < yp1 + 10) || (x + 4 < xp2 + 38 && x + 42 > xp2 && y + 46 < yp2 + 10) || (x + 4 < xp3 + 38 && x + 42 > xp3 && y + 46 < yp3 + 10) || (x + 4 < xp4 + 38 && x + 42 > xp4 && y + 46 < yp4 + 10))) {
      y = 160;
      pad = false;
      prig = true;
      tft.fillRect(x, y + 36, 50, 31, 0xFF16);
      g = 10;
    } else if (y > 160) {
      while (y < 240) {
        y = y + 10;
        if (n == 1) {
          tft.drawRGBBitmap(x, y + 8, DJ_n_R, 50, 38);
        } else if (n == 2) {
          tft.drawRGBBitmap(x, y + 8, DJ_n_L, 50, 38);
        }
      }
      stopD();
    }
    y = y + 1 * g;
    g = g + 1;
    if (g > 10) {
      g = 10;
    }
    if (n == 1) {
      tft.drawRGBBitmap(x, y + 8, DJ_n_R, 50, 38);
    } else if (n == 2) {
      tft.drawRGBBitmap(x, y + 8, DJ_n_L, 50, 38);
    }
    if ((x + 42 > xp1 && x + 4 < xp1 + 38 && y + 46 < yp1 + 10 && y + 46 > yp1) || (x + 42 > xp2 && x + 4 < xp2 + 38 && y + 46 < yp2 + 10 && y + 46 > yp2) || (x + 42 > xp3 && x + 4 < xp3 + 38 && y + 46 < yp3 + 10 && y + 46 > yp3) || (x + 42 > xp4 && x + 4 < xp4 + 38 && y + 46 < yp4 + 10 && y + 46 > yp4)) {
      vsenadno();
      prig = true;
      pad = false;
      g = 10;
    }
  } else if (prig == true) {
    y = y - 1 * g;
    g = g - 0.5;
    if (g < 0) {
      g = 0;
      prig = false;
      pad = true;
    }
    if (n == 1) {
      tft.drawRGBBitmap(x, y + 18, DJ_v_R, 50, 38);
    } else if (n == 2) {
      tft.drawRGBBitmap(x, y + 18, DJ_v_L, 50, 38);
    }
  }
}

void vsenadno() {
  while (y < 160) {
    y = y + 10;
    if (n == 1) {
      tft.drawRGBBitmap(x, y + 8, DJ_n_R, 50, 38);
    } else if (n == 2) {
      tft.drawRGBBitmap(x, y + 8, DJ_n_L, 50, 38);
    }
    yp1 = yp1 + 10;
    tft.drawRGBBitmap(xp1, yp1, plot, 38, 18);
    yp2 = yp2 + 10;
    tft.drawRGBBitmap(xp2, yp2, plot, 38, 18);
    yp3 = yp3 + 10;
    tft.drawRGBBitmap(xp3, yp3, plot, 38, 18);
    yp4 = yp4 + 10;
    tft.drawRGBBitmap(xp4, yp4, plot, 38, 18);
  }
  zvukbool = true;
  zv = 1;
  tmrzvuk = millis();
  zvuk();
  if (yp1 > 240) {
    tft.setCursor(0, 230);
    tft.setTextSize(1);
    tft.setTextColor(0xFF16);
    tft.print(s);
    s = s + 1;
    tft.setCursor(0, 230);
    tft.setTextColor(0x0000);
    tft.print(s);
    xp1 = random(0, 27) * 10;
    yp1 = -20;
    tft.drawRGBBitmap(xp1, yp1, plot, 38, 18);
  }
  if (yp2 > 240) {
    tft.setCursor(0, 230);
    tft.setTextSize(1);
    tft.setTextColor(0xFF16);
    tft.print(s);
    s = s + 1;
    tft.setCursor(0, 230);
    tft.setTextColor(0x0000);
    tft.print(s);
    xp2 = random(0, 27) * 10;
    yp2 = -20;
    tft.drawRGBBitmap(xp2, yp2, plot, 38, 18);
  }
  if (yp3 > 240) {
    tft.setCursor(0, 230);
    tft.setTextSize(1);
    tft.setTextColor(0xFF16);
    tft.print(s);
    s = s + 1;
    tft.setCursor(0, 230);
    tft.setTextColor(0x0000);
    tft.print(s);
    xp3 = random(0, 27) * 10;
    yp3 = -20;
    tft.drawRGBBitmap(xp3, yp3, plot, 38, 18);
  }
  if (yp4 > 240) {
    tft.setCursor(0, 230);
    tft.setTextSize(1);
    tft.setTextColor(0xFF16);
    tft.print(s);
    s = s + 1;
    tft.setCursor(0, 230);
    tft.setTextColor(0x0000);
    tft.print(s);
    xp4 = random(0, 27) * 10;
    yp4 = -20;
    tft.drawRGBBitmap(xp4, yp4, plot, 38, 18);
  }
  y = 160;
  tft.fillRect(x, y + 46, 46, 14, 0xFF16);
  if (n == 1) {
    tft.drawRGBBitmap(x, y + 8, DJ_v_R, 50, 38);
  } else if (n == 2) {
    tft.drawRGBBitmap(x, y + 8, DJ_v_L, 50, 38);
  }
}

void spawnplot() {
  xp4 = 140;
  yp4 = 220;
  tft.drawRGBBitmap(xp4, yp4, plot, 38, 18);
  xp2 = random(0, 27) * 10;
  yp2 = 60;
  tft.drawRGBBitmap(xp2, yp2, plot, 38, 18);
  xp3 = random(0, 27) * 10;
  yp3 = 140;
  tft.drawRGBBitmap(xp3, yp3, plot, 38, 18);
  xp1 = random(0, 27) * 10;
  yp1 = -20;
  tft.drawRGBBitmap(xp1, yp1, plot, 38, 18);
}

void stopD() {
  tft.drawRoundRect(54, 60, 212, 120, 10, ST77XX_WHITE);
  tft.drawRoundRect(55, 61, 210, 118, 9, ST77XX_WHITE);
  tft.drawRoundRect(56, 62, 208, 116, 8, ST77XX_BLACK);
  tft.drawRoundRect(57, 63, 206, 114, 7, ST77XX_BLACK);
  tft.drawRoundRect(58, 64, 204, 112, 6, ST77XX_WHITE);
  tft.drawRoundRect(59, 65, 202, 110, 5, ST77XX_WHITE);
  tft.fillRoundRect(60, 66, 200, 108, 4, ST77XX_BLACK);
  tft.drawBitmap(62, 72, go, 196, 25, ST77XX_WHITE);
  tft.setCursor(100, 120);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);
  tft.print("skore = " + String(s));
  tft.setCursor(100, 140);
  topskore = pref.getUInt("skoreDJ", 0);
  tft.print("top skore = " + String(topskore));
  if (s > topskore) {
    pref.putUInt("skoreDJ", s);
  }
  delay(200);
  while (digitalRead(OK_BUTT) == 0) {
    if (digitalRead(HOME_BUTT) == 1) {
      zvukbool = true;
      zv = 2;
      tmrzvuk = millis();
      zvuk();
      igranach = false;
      break;
    }
  }
  if (igranach == false) {} else {
    y = 160;
    x = 140;
    n = 1;
    prig = true;
    g = 10;
    pad = false;
    s = 0;
    tft.fillScreen(0xFF16);
    spawnplot();
  }
}

void mapSnake() {
  spase = 0;
  for (int i = 0; i < 24; i++) {
    sy = i;
    for (int a = 0; a < 32; a++) {
      sx = a;
      if (fm == 1) {
        xys = map2S[sy][sx];
      } else if (fm == 2) {
        xys = map3S[sy][sx];
      } else if (fm == 0) {
        spase = 768;
      }
      if (xys == 0) {
        spase = spase + 1;
      } else if (xys == 1) {
        tft.fillRect(sx * 10, sy * 10, 10, 10, 0x049F);
      }
    }
  }
}

void delasnake() {
  if (millis() - tmr1 > 100) {
    if (digitalRead(UP_BUTT) == 0) {
      if (n == 4) {} else {
        n = 3;
      }
    } else if (digitalRead(DOWN_BUTT) == 0) {
      if (n == 3) {} else {
        n = 4;
      }
    } else if (digitalRead(RIGHT_BUTT) == 0) {
      if (n == 2) {} else {
        n = 1;
      }
    } else if (digitalRead(LEFT_BUTT) == 0) {
      if (n == 1) {} else {
        n = 2;
      }
    }
    if (n == 1) {
      x = x + 10;
    } else if (n == 2) {
      x = x - 10;
    } else if (n == 3) {
      y = y - 10;
    } else if (n == 4) {
      y = y + 10;
    }
    if (x == 320 || x == -10 || y == 240 || y == -10) {
      stopS();
    }
    i = dlinatela;
    while (i > 0) {
      if (x == xs[i] && y == ys[i]) {
        stopS();
      }
      i = i - 1;
    }
    for (int i = 0; i < 24; i++) {
      sy = i;
      for (int a = 0; a < 32; a++) {
        sx = a;
        if (fm == 1) {
          xys = map2S[sy][sx];
        } else if (fm == 2) {
          xys = map3S[sy][sx];
        }
        if (xys == 1) {
          if (x == sx * 10 && y == sy * 10) {
            stopS();
          }
        }
      }
    }
    if (xst != x || yst != y) {
      tft.drawBitmap(x, y, telo, 10, 10, 0xFFE0);
    }
    i = dlinatela;
    while (i > 0) {
      xs[i] = xs[i - 1];
      ys[i] = ys[i - 1];
      i = i - 1;
    }
    xs[0] = x;
    ys[0] = y;
    if (xst != x || yst != y) {
      tft.drawBitmap(xs[dlinatela], ys[dlinatela], telo, 10, 10, 0x0000);
      xst = x;
      yst = y;
    }
    if (edapr1 == false) {
      edapr1 = true;
      per = random(0, spase);
      ybot1 = 0;
      while (per > 30) {
        per = per - 32;
        ybot1 = ybot1 + 1;
      }
      xbot1 = 0;
      while (per > 0) {
        per = per - 1;
        xbot1 = xbot1 + 1;
      }
      ybot1 = ybot1 * 10;
      xbot1 = xbot1 * 10;
      i = dlinatela;
      while (i > 0) {
        if (xbot1 == xs[i]) {
          edapr1 = false;
        }
        i = i - 1;
      }
      i = dlinatela;
      while (i > 0) {
        if (ybot1 == ys[i]) {
          edapr1 = false;
        }
        i = i - 1;
      }
      for (int i = 0; i < 24; i++) {
        sy = i;
        for (int a = 0; a < 32; a++) {
          sx = a;
          if (fm == 1) {
            xys = map2S[sy][sx];
          } else if (fm == 2) {
            xys = map3S[sy][sx];
          }
          if (xys == 1) {
            if (xbot1 == sx * 10 && ybot1 == sy * 10) {
              edapr1 = false;
            }
          }
        }
      }
      if (edapr1 == true) {
        tft.drawBitmap(xbot1, ybot1, telo, 10, 10, ST77XX_RED);
      }
    }
    if (x == xbot1 && y == ybot1) {
      edapr1 = false;
      dlinatela = dlinatela + 1;
      tft.setTextSize(1);
      tft.setTextColor(ST77XX_BLACK);
      tft.setCursor(0, 10);
      tft.print(s);
      s = s + 1;
      tft.setTextColor(ST77XX_WHITE);
      tft.setCursor(0, 10);
      tft.print(s);
      zvukbool = true;
      zv = 1;
      tmrzvuk = millis();
      zvuk();
    }
    tmr1 = millis();
  }
}

void stopS() {
  tft.drawRoundRect(54, 60, 212, 120, 10, ST77XX_WHITE);
  tft.drawRoundRect(55, 61, 210, 118, 9, ST77XX_WHITE);
  tft.drawRoundRect(56, 62, 208, 116, 8, ST77XX_BLACK);
  tft.drawRoundRect(57, 63, 206, 114, 7, ST77XX_BLACK);
  tft.drawRoundRect(58, 64, 204, 112, 6, ST77XX_WHITE);
  tft.drawRoundRect(59, 65, 202, 110, 5, ST77XX_WHITE);
  tft.fillRoundRect(60, 66, 200, 108, 4, ST77XX_BLACK);
  tft.drawBitmap(62, 72, go, 196, 25, ST77XX_WHITE);
  tft.setCursor(100, 120);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);
  tft.print("skore = " + String(s));
  tft.setCursor(100, 140);
  topskore = pref.getUInt("skoreS", 0);
  tft.print("top skore = " + String(topskore));
  if (s > topskore) {
    pref.putUInt("skoreS", s);
  }
  delay(200);
  while (digitalRead(OK_BUTT) == 0) {
    if (digitalRead(HOME_BUTT) == 1) {
      zvukbool = true;
      zv = 2;
      tmrzvuk = millis();
      zvuk();
      igranach = false;
      break;
    }
  }
  if (igranach == false) {} else {
    y = 100;
    x = 100;
    n = 1;
    edapr1 = false;
    s = 0;
    dlinatela = 4;
    while (i < 100) {
      xs[i] = 0;
      ys[i] = 0;
      i = i + 1;
    }
    i = 0;
    tft.fillScreen(0x0000);
    mapSnake();
  }
}

void preprex() {
  if (kill1 == true) {
    se = random(1, 6);
    xbot1 = 320;
    kill1 = false;
  }
  if (kill2 == true) {
    sd = random(1, 6);
    xbot2 = 320;
    kill2 = false;
  }
  xbot1 = xbot1 - 4;
  xbot2 = xbot2 - 4;
  if (xbot1 < -50) {
    kill1 = true;
    s = s + 1;
  }
  if (xbot2 < -50) {
    kill2 = true;
    s = s + 1;
  }
  if (millis() - tmr3 > 100) {
    e3 = !e3;
    tmr3 = millis();
  }
  if (se == 1) {
    tft.drawRGBBitmap(xbot1, 165, p_k_1, 20, 35);
  } else if (se == 2) {
    tft.drawRGBBitmap(xbot1, 170, p_k_2, 47, 30);
  } else if (se == 3) {
    tft.drawRGBBitmap(xbot1, 150, p_k_3, 27, 50);
  } else if (se == 4) {
    if (e3 == true) {
      tft.drawRGBBitmap(xbot1, 146, p_b_1, 27, 23);
    } else {
      tft.drawRGBBitmap(xbot1, 146, p_b_2, 27, 23);
    }
  } else if (se == 5) {
    if (e3 == true) {
      tft.drawRGBBitmap(xbot1, 176, p_b_1, 27, 23);
    } else {
      tft.drawRGBBitmap(xbot1, 176, p_b_2, 27, 23);
    }
  }
  if (sd == 1) {
    tft.drawRGBBitmap(xbot2, 165, p_k_1, 20, 35);
  } else if (sd == 2) {
    tft.drawRGBBitmap(xbot2, 170, p_k_2, 47, 30);
  } else if (sd == 3) {
    tft.drawRGBBitmap(xbot2, 150, p_k_3, 27, 50);
  } else if (sd == 4) {
    if (e3 == true) {
      tft.drawRGBBitmap(xbot2, 146, p_b_1, 27, 23);
    } else {
      tft.drawRGBBitmap(xbot2, 146, p_b_2, 27, 23);
    }
  } else if (sd == 5) {
    if (e3 == true) {
      tft.drawRGBBitmap(xbot2, 176, p_b_1, 27, 23);
    } else {
      tft.drawRGBBitmap(xbot2, 176, p_b_2, 27, 23);
    }
  }
}

void begrex() {
  if (digitalRead(UP_BUTT) == 0 && prig == false) {
    //tft.fillRect(20,190,46,10,ST77XX_CYAN);
    prig = true;
    sel = false;
    g = 10;
  }
  if (digitalRead(DOWN_BUTT) == 0) {
    if (sel == false) {
      tft.fillRect(20, 168, 46, 8, ST77XX_CYAN);
    }
    sel = true;
  }
  if (digitalRead(DOWN_BUTT) == 1 && sel == true) {
    sel = false;
  }
  if (sel == true && prig == true) {
    y = y + 10;
    g = -10;
    if (y > 200) {
      y = 200;
      prig = false;
      tft.fillRect(20, 138, 46, 30, ST77XX_CYAN);
      tft.fillRect(20, 168, 46, 8, ST77XX_CYAN);
    }
  } else if (prig == true) {
    g = g - 0.6;
    if (g < -10) {
      g = -10;
    }
    y = y - g;
    if (y > 200) {
      y = 200;
      prig = false;
      tft.fillRect(20, 138, 46, 30, ST77XX_CYAN);
    }
  }
  if (prig == true && sel == false) {
    if (g > 0) {
      if (e2 == true) {
        tft.fillRect(20, 200, 46, 10, 0x03E0);
        tft.drawRGBBitmap(20, y - 32, rax1_v, 46, 42);
      } else {
        tft.drawRGBBitmap(20, y - 32, rax2_v, 46, 42);
      }
    } else {
      if (e2 == true) {
        tft.drawRGBBitmap(20, y - 42, rax1_n, 46, 42);
      } else {
        tft.drawRGBBitmap(20, y - 42, rax2_n, 46, 42);
      }
    }
  } else if (sel == true && prig == false) {
    if (e2 == true) {
      tft.drawRGBBitmap(20, y - 24, rax1_s, 46, 24);
    } else {
      tft.drawRGBBitmap(20, y - 24, rax2_s, 46, 24);
    }
  } else if (sel == true && prig == true) {
    if (e2 == true) {
      tft.drawRGBBitmap(20, y - 42, rax1_n, 46, 42);
    } else {
      tft.drawRGBBitmap(20, y - 42, rax2_n, 46, 42);
    }
  } else if (sel == false && prig == false) {
    if (e2 == true) {
      tft.drawRGBBitmap(20, y - 32, rax1, 46, 32);
    } else {
      tft.drawRGBBitmap(20, y - 32, rax2, 46, 32);
    }
  }
  if (millis() - tmr1 > 100 && prig == false) {
    e2 = !e2;
    tmr1 = millis();
  }
  if (se == 1) {
    if (xbot1 < 66 && y > 165 && xbot1 + 16 > 20) {
      stopRax();
    }
  } else if (se == 2) {
    if (xbot1 < 66 && y > 170 && xbot1 + 43 > 20) {
      stopRax();
    }
  } else if (se == 3) {
    if (xbot1 < 66 && y > 150 && xbot1 + 23 > 20) {
      stopRax();
    }
  } else if (se == 4) {
    if (sel == true && prig == false) {} else if (xbot1 < 66 && y > 154 && xbot1 + 23 > 20) {
      stopRax();
    }
  } else if (se == 5) {
    if (xbot1 < 66 && y > 177 && xbot1 + 23 > 20) {
      stopRax();
    }
  }
  if (sd == 1) {
    if (xbot2 < 66 && y > 165 && xbot2 + 16 > 20) {
      stopRax();
    }
  } else if (sd == 2) {
    if (xbot2 < 66 && y > 170 && xbot2 + 43 > 20) {
      stopRax();
    }
  } else if (sd == 3) {
    if (xbot2 < 66 && y > 150 && xbot2 + 23 > 20) {
      stopRax();
    }
  } else if (sd == 4) {
    if (sel == true && prig == false) {} else if (xbot2 < 66 && y > 154 && xbot2 + 23 > 20) {
      stopRax();
    }
  } else if (sd == 5) {
    if (xbot2 < 66 && y > 177 && xbot2 + 23 > 20) {
      stopRax();
    }
  }
}

void stopRax() {
  tft.drawRoundRect(54, 60, 212, 120, 10, ST77XX_WHITE);
  tft.drawRoundRect(55, 61, 210, 118, 9, ST77XX_WHITE);
  tft.drawRoundRect(56, 62, 208, 116, 8, ST77XX_BLACK);
  tft.drawRoundRect(57, 63, 206, 114, 7, ST77XX_BLACK);
  tft.drawRoundRect(58, 64, 204, 112, 6, ST77XX_WHITE);
  tft.drawRoundRect(59, 65, 202, 110, 5, ST77XX_WHITE);
  tft.fillRoundRect(60, 66, 200, 108, 4, ST77XX_BLACK);
  tft.drawBitmap(62, 72, go, 196, 25, ST77XX_WHITE);
  tft.setCursor(100, 120);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);
  tft.print("skore = " + String(s));
  tft.setCursor(100, 140);
  topskore = pref.getUInt("skoreDJ2", 0);
  tft.print("top skore = " + String(topskore));
  if (s > topskore) {
    pref.putUInt("skoreDJ2", s);
  }
  delay(200);
  while (digitalRead(OK_BUTT) == 0) {
    if (digitalRead(HOME_BUTT) == 1) {
      zvukbool = true;
      zv = 2;
      tmrzvuk = millis();
      zvuk();
      igranach = false;
      break;
    }
  }
  if (igranach == false) {} else {
    tft.fillScreen(ST77XX_CYAN);
    tft.fillRect(0, 200, 320, 40, 0x03E0);
    igranach = true;
    y = 200;
    xbot1 = 160;
    xbot2 = 320;
    kill1 = false;
    kill2 = false;
    se = 2;
    sd = 3;
    o = 320;
    s = 0;
    sst = 1;
    prig = false;
  }
}

void delatetris() {
  if (kill0 == true) {
    if (y == -48) {
      stopT();
    }
    s = s + 1;
    x = x / 12;
    y = y / 12;
    if (i == 0) {
      if (n == 1) {
        tetfon[y][x] = 1;
        tetfon[y][x + 1] = 1;
        tetfon[y + 1][x + 1] = 1;
        tetfon[y + 1][x + 2] = 1;
      } else if (n == 2) {
        tetfon[y][x + 2] = 1;
        tetfon[y + 1][x + 1] = 1;
        tetfon[y + 1][x + 2] = 1;
        tetfon[y + 2][x + 1] = 1;
      } else if (n == 3) {
        tetfon[y + 1][x] = 1;
        tetfon[y + 1][x + 1] = 1;
        tetfon[y + 2][x + 1] = 1;
        tetfon[y + 2][x + 2] = 1;
      } else if (n == 4) {
        tetfon[y][x + 1] = 1;
        tetfon[y + 1][x] = 1;
        tetfon[y + 1][x + 1] = 1;
        tetfon[y + 2][x] = 1;
      }
    } else if (i == 1) {
      if (n == 1) {
        tetfon[y][x + 1] = 2;
        tetfon[y + 1][x + 1] = 2;
        tetfon[y + 2][x + 1] = 2;
        tetfon[y + 2][x + 2] = 2;
      } else if (n == 2) {
        tetfon[y + 1][x] = 2;
        tetfon[y + 1][x + 1] = 2;
        tetfon[y + 1][x + 2] = 2;
        tetfon[y + 2][x] = 2;
      } else if (n == 3) {
        tetfon[y][x] = 2;
        tetfon[y][x + 1] = 2;
        tetfon[y + 1][x + 1] = 2;
        tetfon[y + 2][x + 1] = 2;
      } else if (n == 4) {
        tetfon[y][x + 2] = 2;
        tetfon[y + 1][x] = 2;
        tetfon[y + 1][x + 1] = 2;
        tetfon[y + 1][x + 2] = 2;
      }
    } else if (i == 2) {
      if (n == 1) {
        tetfon[y][x + 1] = 3;
        tetfon[y + 1][x + 1] = 3;
        tetfon[y + 2][x + 1] = 3;
        tetfon[y + 3][x + 1] = 3;
      } else if (n == 2) {
        tetfon[y + 1][x] = 3;
        tetfon[y + 1][x + 1] = 3;
        tetfon[y + 1][x + 2] = 3;
        tetfon[y + 1][x + 3] = 3;
      } else if (n == 3) {
        tetfon[y][x + 2] = 3;
        tetfon[y + 1][x + 2] = 3;
        tetfon[y + 2][x + 2] = 3;
        tetfon[y + 3][x + 2] = 3;
      } else if (n == 4) {
        tetfon[y + 2][x] = 3;
        tetfon[y + 2][x + 1] = 3;
        tetfon[y + 2][x + 2] = 3;
        tetfon[y + 2][x + 3] = 3;
      }
    } else if (i == 3) {
      tetfon[y][x] = 4;
      tetfon[y][x + 1] = 4;
      tetfon[y + 1][x] = 4;
      tetfon[y + 1][x + 1] = 4;
    } else if (i == 4) {
      if (n == 1) {
        tetfon[y][x + 1] = 5;
        tetfon[y + 1][x + 1] = 5;
        tetfon[y + 1][x + 2] = 5;
        tetfon[y + 2][x + 1] = 5;
      } else if (n == 2) {
        tetfon[y + 1][x] = 5;
        tetfon[y + 1][x + 1] = 5;
        tetfon[y + 1][x + 2] = 5;
        tetfon[y + 2][x + 1] = 5;
      } else if (n == 3) {
        tetfon[y][x + 1] = 5;
        tetfon[y + 1][x] = 5;
        tetfon[y + 1][x + 1] = 5;
        tetfon[y + 2][x + 1] = 5;
      } else if (n == 4) {
        tetfon[y][x + 1] = 5;
        tetfon[y + 1][x] = 5;
        tetfon[y + 1][x + 1] = 5;
        tetfon[y + 1][x + 2] = 5;
      }
    }
    i = ist;
    ist = random(0, 5);
    tft.setTextSize(1);
    tft.setTextColor(0xFFFF);
    tft.setCursor(144, 2);
    tft.print("NEXT:");
    tft.setCursor(144, 70);
    tft.fillRect(144, 70, 100, 10, 0x0000);
    tft.print("SKORE: " + String(s));
    tft.fillRect(144, 12, 48, 48, 0x0000);
    if (ist == 0) {
      tft.drawBitmap(144, 12, tetB0n1_1, 36, 36, 0xD801);
      tft.drawBitmap(144, 12, tetB0n1_2, 36, 36, 0xFB40);
    } else if (ist == 1) {
      tft.drawBitmap(144, 12, tetB1n1_1, 36, 36, 0x7B40);
      tft.drawBitmap(144, 12, tetB1n1_2, 36, 36, 0xFEA0);
    } else if (ist == 2) {
      tft.drawBitmap(144, 12, tetB2n1_1, 48, 48, 0x23E0);
      tft.drawBitmap(144, 12, tetB2n1_2, 48, 48, 0x07E4);
    } else if (ist == 3) {
      tft.drawBitmap(156, 12, tetB3_1, 24, 24, 0xA81F);
      tft.drawBitmap(156, 12, tetB3_2, 24, 24, 0xF81A);
    } else if (ist == 4) {
      tft.drawBitmap(144, 12, tetB4n1_1, 36, 36, 0x013F);
      tft.drawBitmap(144, 12, tetB4n1_2, 36, 36, 0x049F);
    }
    x = 48;
    y = -48;
    n = 1;
    kill0 = false;
  }
  xst = x;
  yst = y;
  nst = n;
  if (millis() - tmr1 > 370) {
    y = y + 12;
    if (i == 0) {
      if (n == 1) {
        xc = 0;
        yc = 0;
        while (yc < 20 && xc < 10) {
          if (tetfon[yc][xc] > 0) {
            if (x == xc * 12 && y == yc * 12) {
              kill0 = true;
              y = y - 12;
            } else if (x + 12 == xc * 12 && y + 12 == yc * 12) {
              kill0 = true;
              y = y - 12;
            } else if (x + 24 == xc * 12 && y + 12 == yc * 12) {
              kill0 = true;
              y = y - 12;
            }
          }
          xc = xc + 1;
          if (xc > 9) {
            xc = 0;
            yc = yc + 1;
          }
        }
      } else if (n == 2) {
        xc = 0;
        yc = 0;
        while (yc < 20 && xc < 10) {
          if (tetfon[yc][xc] > 0) {
            if (x + 12 == xc * 12 && y + 24 == yc * 12) {
              kill0 = true;
              y = y - 12;
            } else if (x + 24 == xc * 12 && y + 12 == yc * 12) {
              kill0 = true;
              y = y - 12;
            }
          }
          xc = xc + 1;
          if (xc > 9) {
            xc = 0;
            yc = yc + 1;
          }
        }
      } else if (n == 3) {
        xc = 0;
        yc = 0;
        while (yc < 20 && xc < 10) {
          if (tetfon[yc][xc] > 0) {
            if (x == xc * 12 && y + 12 == yc * 12) {
              kill0 = true;
              y = y - 12;
            } else if (x + 12 == xc * 12 && y + 24 == yc * 12) {
              kill0 = true;
              y = y - 12;
            } else if (x + 24 == xc * 12 && y + 24 == yc * 12) {
              kill0 = true;
              y = y - 12;
            }
          }
          xc = xc + 1;
          if (xc > 9) {
            xc = 0;
            yc = yc + 1;
          }
        }
      } else if (n == 4) {
        xc = 0;
        yc = 0;
        while (yc < 20 && xc < 10) {
          if (tetfon[yc][xc] > 0) {
            if (x == xc * 12 && y + 24 == yc * 12) {
              kill0 = true;
              y = y - 12;
            } else if (x + 12 == xc * 12 && y + 12 == yc * 12) {
              kill0 = true;
              y = y - 12;
            }
          }
          xc = xc + 1;
          if (xc > 9) {
            xc = 0;
            yc = yc + 1;
          }
        }
      }
    } else if (i == 1) {
      if (n == 1) {
        xc = 0;
        yc = 0;
        while (yc < 20 && xc < 10) {
          if (tetfon[yc][xc] > 0) {
            if (x + 12 == xc * 12 && y + 24 == yc * 12) {
              kill0 = true;
              y = y - 12;
            } else if (x + 24 == xc * 12 && y + 24 == yc * 12) {
              kill0 = true;
              y = y - 12;
            }
          }
          xc = xc + 1;
          if (xc > 9) {
            xc = 0;
            yc = yc + 1;
          }
        }
      } else if (n == 2) {
        xc = 0;
        yc = 0;
        while (yc < 20 && xc < 10) {
          if (tetfon[yc][xc] > 0) {
            if (x + 12 == xc * 12 && y + 12 == yc * 12) {
              kill0 = true;
              y = y - 12;
            } else if (x + 24 == xc * 12 && y + 12 == yc * 12) {
              kill0 = true;
              y = y - 12;
            } else if (x == xc * 12 && y + 24 == yc * 12) {
              kill0 = true;
              y = y - 12;
            }
          }
          xc = xc + 1;
          if (xc > 9) {
            xc = 0;
            yc = yc + 1;
          }
        }
      } else if (n == 3) {
        xc = 0;
        yc = 0;
        while (yc < 20 && xc < 10) {
          if (tetfon[yc][xc] > 0) {
            if (x == xc * 12 && y == yc * 12) {
              kill0 = true;
              y = y - 12;
            } else if (x + 12 == xc * 12 && y + 24 == yc * 12) {
              kill0 = true;
              y = y - 12;
            }
          }
          xc = xc + 1;
          if (xc > 9) {
            xc = 0;
            yc = yc + 1;
          }
        }
      } else if (n == 4) {
        xc = 0;
        yc = 0;
        while (yc < 19 && xc < 10) {
          if (tetfon[yc][xc] > 0) {
            if (x == xc * 12 && y + 12 == yc * 12) {
              kill0 = true;
              y = y - 12;
            } else if (x + 12 == xc * 12 && y + 12 == yc * 12) {
              kill0 = true;
              y = y - 12;
            } else if (x + 24 == xc * 12 && y + 12 == yc * 12) {
              kill0 = true;
              y = y - 12;
            }
          }
          xc = xc + 1;
          if (xc > 9) {
            xc = 0;
            yc = yc + 1;
          }
        }
      }
    } else if (i == 2) {
      if (n == 1) {
        xc = 0;
        yc = 0;
        while (yc < 20 && xc < 10) {
          if (tetfon[yc][xc] > 0) {
            if (x + 12 == xc * 12 && y + 36 == yc * 12) {
              kill0 = true;
              y = y - 12;
            }
          }
          xc = xc + 1;
          if (xc > 9) {
            xc = 0;
            yc = yc + 1;
          }
        }
      } else if (n == 2) {
        xc = 0;
        yc = 0;
        while (yc < 20 && xc < 10) {
          if (tetfon[yc][xc] > 0) {
            if (x == xc * 12 && y + 12 == yc * 12) {
              kill0 = true;
              y = y - 12;
            } else if (x + 12 == xc * 12 && y + 12 == yc * 12) {
              kill0 = true;
              y = y - 12;
            } else if (x + 24 == xc * 12 && y + 12 == yc * 12) {
              kill0 = true;
              y = y - 12;
            } else if (x + 36 == xc * 12 && y + 12 == yc * 12) {
              kill0 = true;
              y = y - 12;
            }
          }
          xc = xc + 1;
          if (xc > 9) {
            xc = 0;
            yc = yc + 1;
          }
        }
      } else if (n == 3) {
        xc = 0;
        yc = 0;
        while (yc < 20 && xc < 10) {
          if (tetfon[yc][xc] > 0) {
            if (x + 24 == xc * 12 && y + 36 == yc * 12) {
              kill0 = true;
              y = y - 12;
            }
          }
          xc = xc + 1;
          if (xc > 9) {
            xc = 0;
            yc = yc + 1;
          }
        }
      } else if (n == 4) {
        xc = 0;
        yc = 0;
        while (yc < 19 && xc < 10) {
          if (tetfon[yc][xc] > 0) {
            if (x == xc * 12 && y + 24 == yc * 12) {
              kill0 = true;
              y = y - 12;
            } else if (x + 12 == xc * 12 && y + 24 == yc * 12) {
              kill0 = true;
              y = y - 12;
            } else if (x + 24 == xc * 12 && y + 24 == yc * 12) {
              kill0 = true;
              y = y - 12;
            } else if (x + 36 == xc * 12 && y + 24 == yc * 12) {
              kill0 = true;
              y = y - 12;
            }
          }
          xc = xc + 1;
          if (xc > 9) {
            xc = 0;
            yc = yc + 1;
          }
        }
      }
    } else if (i == 3) {
      xc = 0;
      yc = 0;
      while (yc < 19 && xc < 10) {
        if (tetfon[yc][xc] > 0) {
          if (x == xc * 12 && y + 12 == yc * 12) {
            kill0 = true;
            y = y - 12;
          } else if (x + 12 == xc * 12 && y + 12 == yc * 12) {
            kill0 = true;
            y = y - 12;
          }
        }
        xc = xc + 1;
        if (xc > 9) {
          xc = 0;
          yc = yc + 1;
        }
      }
    } else if (i == 4) {
      if (n == 1) {
        xc = 0;
        yc = 0;
        while (yc < 19 && xc < 10) {
          if (tetfon[yc][xc] > 0) {
            if (x + 12 == xc * 12 && y + 24 == yc * 12) {
              kill0 = true;
              y = y - 12;
            } else if (x + 24 == xc * 12 && y + 12 == yc * 12) {
              kill0 = true;
              y = y - 12;
            }
          }
          xc = xc + 1;
          if (xc > 9) {
            xc = 0;
            yc = yc + 1;
          }
        }
      } else if (n == 2) {
        xc = 0;
        yc = 0;
        while (yc < 19 && xc < 10) {
          if (tetfon[yc][xc] > 0) {
            if (x == xc * 12 && y + 12 == yc * 12) {
              kill0 = true;
              y = y - 12;
            } else if (x + 12 == xc * 12 && y + 24 == yc * 12) {
              kill0 = true;
              y = y - 12;
            } else if (x + 24 == xc * 12 && y + 12 == yc * 12) {
              kill0 = true;
              y = y - 12;
            }
          }
          xc = xc + 1;
          if (xc > 9) {
            xc = 0;
            yc = yc + 1;
          }
        }
      } else if (n == 3) {
        xc = 0;
        yc = 0;
        while (yc < 19 && xc < 10) {
          if (tetfon[yc][xc] > 0) {
            if (x == xc * 12 && y + 12 == yc * 12) {
              kill0 = true;
              y = y - 12;
            } else if (x + 12 == xc * 12 && y + 24 == yc * 12) {
              kill0 = true;
              y = y - 12;
            }
          }
          xc = xc + 1;
          if (xc > 9) {
            xc = 0;
            yc = yc + 1;
          }
        }
      } else if (n == 4) {
        xc = 0;
        yc = 0;
        while (yc < 19 && xc < 10) {
          if (tetfon[yc][xc] > 0) {
            if (x == xc * 12 && y + 12 == yc * 12) {
              kill0 = true;
              y = y - 12;
            } else if (x + 12 == xc * 12 && y + 12 == yc * 12) {
              kill0 = true;
              y = y - 12;
            } else if (x + 24 == xc * 12 && y + 12 == yc * 12) {
              kill0 = true;
              y = y - 12;
            }
          }
          xc = xc + 1;
          if (xc > 9) {
            xc = 0;
            yc = yc + 1;
          }
        }
      }
    }
    if (i == 0) {
      if (n == 1) {
        if (y > 216) {
          y = 216;
          kill0 = true;
        }
      } else if (n == 2) {
        if (y > 204) {
          y = 204;
          kill0 = true;
        }
      } else if (n == 3) {
        if (y > 204) {
          y = 204;
          kill0 = true;
        }
      } else if (n == 4) {
        if (y > 204) {
          y = 204;
          kill0 = true;
        }
      }
    } else if (i == 1) {
      if (n == 1) {
        if (y > 204) {
          y = 204;
          kill0 = true;
        }
      } else if (n == 2) {
        if (y > 204) {
          y = 204;
          kill0 = true;
        }
      } else if (n == 3) {
        if (y > 204) {
          y = 204;
          kill0 = true;
        }
      } else if (n == 4) {
        if (y > 216) {
          y = 216;
          kill0 = true;
        }
      }
    } else if (i == 2) {
      if (n == 1 || n == 3) {
        if (y > 192) {
          y = 192;
          kill0 = true;
        }
      } else if (n == 2) {
        if (y > 216) {
          y = 216;
          kill0 = true;
        }
      } else {
        if (y > 204) {
          y = 204;
          kill0 = true;
        }
      }
    } else if (i == 3) {
      if (y > 216) {
        y = 216;
        kill0 = true;
      }
    } else if (i == 4) {
      if (n == 1) {
        if (y > 204) {
          y = 204;
          kill0 = true;
        }
      } else if (n == 2) {
        if (y > 204) {
          y = 204;
          kill0 = true;
        }
      } else if (n == 3) {
        if (y > 204) {
          y = 204;
          kill0 = true;
        }
      } else if (n == 4) {
        if (y > 216) {
          y = 216;
          kill0 = true;
        }
      }
    }
    if (digitalRead(DOWN_BUTT) == 1) {
      tmr1 = millis();
    } else {
      tmr1 = millis() - 320;
    }
  }
  if (digitalRead(OK_BUTT) == 1 && millis() - tmrkill1 > 300) {
    zvukbool = true;
    zv = 2;
    tmrzvuk = millis();
    zvuk();
    flag = false;
    if (i == 0) {
      if (n == 2 && x == -12) {
        x = 0;
        xc = 0;
        yc = 0;
        while (yc < 19 && xc < 10) {
          if (tetfon[yc][xc] > 0) {
            if (x == xc * 12 && y + 12 == yc * 12) {
              x = -12;
              flag = true;
            } else if (x + 12 == xc * 12 && y + 12 == yc * 12) {
              x = -12;
              flag = true;
            } else if (x + 24 == xc * 12 && y + 12 == yc * 12) {
              x = -12;
              flag = true;
            } else if (x + 24 == xc * 12 && y + 24 == yc * 12) {
              x = -12;
              flag = true;
            }
          }
          xc = xc + 1;
          if (xc > 9) {
            xc = 0;
            yc = yc + 1;
          }
        }
      } else if (n == 4 && x == 96) {
        x = 84;
        xc = 0;
        yc = 0;
        while (yc < 19 && xc < 10) {
          if (tetfon[yc][xc] > 0) {
            if (x == xc * 12 && y == yc * 12) {
              x = 96;
              flag = true;
            } else if (x + 12 == xc * 12 && y == yc * 12) {
              x = 96;
              flag = true;
            } else if (x + 12 == xc * 12 && y + 12 == yc * 12) {
              x = 96;
              flag = true;
            } else if (x + 24 == xc * 12 && y + 12 == yc * 12) {
              x = 96;
              flag = true;
            }
          }
          xc = xc + 1;
          if (xc > 9) {
            xc = 0;
            yc = yc + 1;
          }
        }
      }
    } else if (i == 1) {
      if (n == 1 && x == -12) {
        x = 0;
        xc = 0;
        yc = 0;
        while (yc < 19 && xc < 10) {
          if (tetfon[yc][xc] > 0) {
            if (x == xc * 12 && y + 12 == yc * 12) {
              x = -12;
              flag = true;
            } else if (x + 12 == xc * 12 && y + 12 == yc * 12) {
              x = -12;
              flag = true;
            } else if (x + 24 == xc * 12 && y + 12 == yc * 12) {
              x = -12;
              flag = true;
            } else if (x == xc * 12 && y + 24 == yc * 12) {
              x = -12;
              flag = true;
            }
          }
          xc = xc + 1;
          if (xc > 9) {
            xc = 0;
            yc = yc + 1;
          }
        }
      } else if (n == 3 && x == 96) {
        x = 84;
        xc = 0;
        yc = 0;
        while (yc < 19 && xc < 10) {
          if (tetfon[yc][xc] > 0) {
            if (x == xc * 12 && y + 12 == yc * 12) {
              x = 96;
              flag = true;
            } else if (x + 12 == xc * 12 && y + 12 == yc * 12) {
              x = 96;
              flag = true;
            } else if (x + 24 == xc * 12 && y == yc * 12) {
              x = 96;
              flag = true;
            } else if (x + 24 == xc * 12 && y + 12 == yc * 12) {
              x = 96;
              flag = true;
            }
          }
          xc = xc + 1;
          if (xc > 9) {
            xc = 0;
            yc = yc + 1;
          }
        }
      }
    } else if (i == 2) {
      if (n == 1 && x == -12) {
        x = 0;
        xc = 0;
        yc = 0;
        while (yc < 19 && xc < 10) {
          if (tetfon[yc][xc] > 0) {
            if (x == xc * 12 && y + 12 == yc * 12) {
              x = -12;
              flag = true;
            } else if (x + 12 == xc * 12 && y + 12 == yc * 12) {
              x = -12;
              flag = true;
            } else if (x + 24 == xc * 12 && y + 12 == yc * 12) {
              x = -12;
              flag = true;
            } else if (x + 36 == xc * 12 && y + 12 == yc * 12) {
              x = -12;
              flag = true;
            }
          }
          xc = xc + 1;
          if (xc > 9) {
            xc = 0;
            yc = yc + 1;
          }
        }
      } else if (n == 3 && x == 84) {
        x = 72;
        xc = 0;
        yc = 0;
        while (yc < 19 && xc < 10) {
          if (tetfon[yc][xc] > 0) {
            if (x == xc * 12 && y + 24 == yc * 12) {
              x = 84;
              flag = true;
            } else if (x + 12 == xc * 12 && y + 24 == yc * 12) {
              x = 84;
              flag = true;
            } else if (x + 24 == xc * 12 && y + 24 == yc * 12) {
              x = 84;
              flag = true;
            } else if (x + 36 == xc * 12 && y + 24 == yc * 12) {
              x = 84;
              flag = true;
            }
          }
          xc = xc + 1;
          if (xc > 9) {
            xc = 0;
            yc = yc + 1;
          }
        }
      } else if (n == 1 && x == 96) {
        x = 72;
        xc = 0;
        yc = 0;
        while (yc < 19 && xc < 10) {
          if (tetfon[yc][xc] > 0) {
            if (x == xc * 12 && y + 12 == yc * 12) {
              x = 92;
              flag = true;
            } else if (x + 12 == xc * 12 && y + 12 == yc * 12) {
              x = 92;
              flag = true;
            } else if (x + 24 == xc * 12 && y + 12 == yc * 12) {
              x = 92;
              flag = true;
            } else if (x + 36 == xc * 12 && y + 12 == yc * 12) {
              x = 92;
              flag = true;
            }
          }
          xc = xc + 1;
          if (xc > 9) {
            xc = 0;
            yc = yc + 1;
          }
        }
      } else if (n == 3 && x == -24) {
        x = 0;
        xc = 0;
        yc = 0;
        while (yc < 19 && xc < 10) {
          if (tetfon[yc][xc] > 0) {
            if (x == xc * 12 && y + 24 == yc * 12) {
              x = -24;
              flag = true;
            } else if (x + 12 == xc * 12 && y + 24 == yc * 12) {
              x = -24;
              flag = true;
            } else if (x + 24 == xc * 12 && y + 24 == yc * 12) {
              x = -24;
              flag = true;
            } else if (x + 36 == xc * 12 && y + 24 == yc * 12) {
              x = -24;
              flag = true;
            }
          }
          xc = xc + 1;
          if (xc > 9) {
            xc = 0;
            yc = yc + 1;
          }
        }
      }
    } else if (i == 4) {
      if (n == 1 && x == -12) {
        x = 0;
        xc = 0;
        yc = 0;
        while (yc < 19 && xc < 10) {
          if (tetfon[yc][xc] > 0) {
            if (x == xc * 12 && y + 12 == yc * 12) {
              x = -12;
              flag = true;
            } else if (x + 12 == xc * 12 && y + 12 == yc * 12) {
              x = -12;
              flag = true;
            } else if (x + 24 == xc * 12 && y + 12 == yc * 12) {
              x = -12;
              flag = true;
            } else if (x + 12 == xc * 12 && y + 24 == yc * 12) {
              x = -12;
              flag = true;
            }
          }
          xc = xc + 1;
          if (xc > 9) {
            xc = 0;
            yc = yc + 1;
          }
        }
      } else if (n == 3 && x == 96) {
        x = 84;
        xc = 0;
        yc = 0;
        while (yc < 19 && xc < 10) {
          if (tetfon[yc][xc] > 0) {
            if (x + 12 == xc * 12 && y == yc * 12) {
              x = 96;
              flag = true;
            } else if (x == xc * 12 && y + 12 == yc * 12) {
              x = 96;
              flag = true;
            } else if (x + 12 == xc * 12 && y + 12 == yc * 12) {
              x = 96;
              flag = true;
            } else if (x + 24 == xc * 12 && y + 12 == yc * 12) {
              x = 96;
              flag = true;
            }
          }
          xc = xc + 1;
          if (xc > 9) {
            xc = 0;
            yc = yc + 1;
          }
        }
      }
    }
    if (flag == false) {
      if (i == 0) {
        if (n == 1) {
          if (y < 216) {
            n = 2;
            xc = 0;
            yc = 0;
            while (yc < 19 && xc < 10) {
              if (tetfon[yc][xc] > 0) {
                if (x + 12 == xc * 12 && y + 24 == yc * 12) {
                  n = 1;
                } else if (x + 24 == xc * 12 && y == yc * 12) {
                  n = 1;
                }
              }
              xc = xc + 1;
              if (xc > 9) {
                xc = 0;
                yc = yc + 1;
              }
            }
          }
        } else if (n == 2) {
          if (x > -12) {
            n = 3;
            xc = 0;
            yc = 0;
            while (yc < 19 && xc < 10) {
              if (tetfon[yc][xc] > 0) {
                if (x == xc * 12 && y + 12 == yc * 12) {
                  n = 2;
                } else if (x + 24 == xc * 12 && y + 24 == yc * 12) {
                  n = 2;
                }
              }
              xc = xc + 1;
              if (xc > 9) {
                xc = 0;
                yc = yc + 1;
              }
            }
          }
        } else if (n == 3) {
          n = 4;
          xc = 0;
          yc = 0;
          while (yc < 19 && xc < 10) {
            if (tetfon[yc][xc] > 0) {
              if (x == xc * 12 && y + 24 == yc * 12) {
                n = 3;
              } else if (x + 12 == xc * 12 && y == yc * 12) {
                n = 3;
              }
            }
            xc = xc + 1;
            if (xc > 9) {
              xc = 0;
              yc = yc + 1;
            }
          }
        } else if (n == 4) {
          if (x < 96) {
            n = 1;
            xc = 0;
            yc = 0;
            while (yc < 19 && xc < 10) {
              if (tetfon[yc][xc] > 0) {
                if (x + 24 == xc * 12 && y + 12 == yc * 12) {
                  n = 4;
                } else if (x == xc * 12 && y == yc * 12) {
                  n = 4;
                }
              }
              xc = xc + 1;
              if (xc > 9) {
                xc = 0;
                yc = yc + 1;
              }
            }
          }
        }
      } else if (i == 1) {
        if (n == 1) {
          if (x > -12) {
            n = 2;
            xc = 0;
            yc = 0;
            while (yc < 19 && xc < 10) {
              if (tetfon[yc][xc] > 0) {
                if (x == xc * 12 && y + 12 == yc * 12) {
                  n = 1;
                } else if (x == xc * 12 && y + 24 == yc * 12) {
                  n = 1;
                } else if (x + 24 == xc * 12 && y + 12 == yc * 12) {
                  n = 1;
                }
              }
              xc = xc + 1;
              if (xc > 9) {
                xc = 0;
                yc = yc + 1;
              }
            }
          }
        } else if (n == 2) {
          n = 3;
          xc = 0;
          yc = 0;
          while (yc < 19 && xc < 10) {
            if (tetfon[yc][xc] > 0) {
              if (x == xc * 12 && y == yc * 12) {
                n = 2;
              } else if (x == xc * 12 && y + 12 == yc * 12) {
                n = 2;
              } else if (x + 12 == xc * 12 && y + 24 == yc * 12) {
                n = 2;
              }
            }
            xc = xc + 1;
            if (xc > 9) {
              xc = 0;
              yc = yc + 1;
            }
          }
        } else if (n == 3) {
          if (x < 96) {
            n = 4;
            xc = 0;
            yc = 0;
            while (yc < 19 && xc < 10) {
              if (tetfon[yc][xc] > 0) {
                if (x + 24 == xc * 12 && y == yc * 12) {
                  n = 3;
                } else if (x + 24 == xc * 12 && y + 12 == yc * 12) {
                  n = 3;
                } else if (x == xc * 12 && y + 12 == yc * 12) {
                  n = 3;
                }
              }
              xc = xc + 1;
              if (xc > 9) {
                xc = 0;
                yc = yc + 1;
              }
            }
          }
        } else if (n == 4) {
          if (y < 216) {
            n = 1;
            xc = 0;
            yc = 0;
            while (yc < 19 && xc < 10) {
              if (tetfon[yc][xc] > 0) {
                if (x + 12 == xc * 12 && y == yc * 12) {
                  n = 4;
                } else if (x + 12 == xc * 12 && y + 24 == yc * 12) {
                  n = 4;
                } else if (x + 24 == xc * 12 && y + 24 == yc * 12) {
                  n = 4;
                }
              }
              xc = xc + 1;
              if (xc > 9) {
                xc = 0;
                yc = yc + 1;
              }
            }
          }
        }
      } else if (i == 2) {
        if (n == 1) {
          if (x < 84 && x > -12) {
            n = 2;
            xc = 0;
            yc = 0;
            while (yc < 19 && xc < 10) {
              if (tetfon[yc][xc] > 0) {
                if (x == xc * 12 && y + 12 == yc * 12) {
                  n = 1;
                } else if (x + 24 == xc * 12 && y + 12 == yc * 12) {
                  n = 1;
                } else if (x + 36 == xc * 12 && y + 12 == yc * 12) {
                  n = 1;
                }
              }
              xc = xc + 1;
              if (xc > 9) {
                xc = 0;
                yc = yc + 1;
              }
            }
          }
        } else if (n == 2) {
          if (y < 204) {
            n = 3;
            xc = 0;
            yc = 0;
            while (yc < 19 && xc < 10) {
              if (tetfon[yc][xc] > 0) {
                if (x + 24 == xc * 12 && y == yc * 12) {
                  n = 2;
                } else if (x + 24 == xc * 12 && y + 24 == yc * 12) {
                  n = 2;
                } else if (x + 24 == xc * 12 && y + 36 == yc * 12) {
                  n = 2;
                }
              }
              xc = xc + 1;
              if (xc > 9) {
                xc = 0;
                yc = yc + 1;
              }
            }
          }
        } else if (n == 3) {
          if (x < 84 && x > -12) {
            n = 4;
            xc = 0;
            yc = 0;
            while (yc < 19 && xc < 10) {
              if (tetfon[yc][xc] > 0) {
                if (x == xc * 12 && y + 24 == yc * 12) {
                  n = 3;
                } else if (x + 12 == xc * 12 && y + 24 == yc * 12) {
                  n = 3;
                } else if (x + 36 == xc * 12 && y + 24 == yc * 12) {
                  n = 3;
                }
              }
              xc = xc + 1;
              if (xc > 9) {
                xc = 0;
                yc = yc + 1;
              }
            }
          }
        } else if (n == 4) {
          if (y < 204) {
            n = 1;
            xc = 0;
            yc = 0;
            while (yc < 19 && xc < 10) {
              if (tetfon[yc][xc] > 0) {
                if (x + 12 == xc * 12 && y == yc * 12) {
                  n = 4;
                } else if (x + 12 == xc * 12 && y + 12 == yc * 12) {
                  n = 4;
                } else if (x + 12 == xc * 12 && y + 36 == yc * 12) {
                  n = 4;
                }
              }
              xc = xc + 1;
              if (xc > 9) {
                xc = 0;
                yc = yc + 1;
              }
            }
          }
        }
      } else if (i == 4) {
        if (n == 1) {
          if (x > -12) {
            n = 2;
            xc = 0;
            yc = 0;
            while (yc < 19 && xc < 10) {
              if (tetfon[yc][xc] > 0) {
                if (x == xc * 12 && y + 12 == yc * 12) {
                  n = 1;
                }
              }
              xc = xc + 1;
              if (xc > 9) {
                xc = 0;
                yc = yc + 1;
              }
            }
          }
        } else if (n == 2) {
          n = 3;
          xc = 0;
          yc = 0;
          while (yc < 19 && xc < 10) {
            if (tetfon[yc][xc] > 0) {
              if (x + 12 == xc * 12 && y == yc * 12) {
                n = 2;
              }
            }
            xc = xc + 1;
            if (xc > 9) {
              xc = 0;
              yc = yc + 1;
            }
          }
        } else if (n == 3) {
          if (x < 96) {
            n = 4;
            xc = 0;
            yc = 0;
            while (yc < 19 && xc < 10) {
              if (tetfon[yc][xc] > 0) {
                if (x + 24 == xc * 12 && y + 12 == yc * 12) {
                  n = 3;
                }
              }
              xc = xc + 1;
              if (xc > 9) {
                xc = 0;
                yc = yc + 1;
              }
            }
          }
        } else if (n == 4) {
          if (y < 216) {
            n = 1;
            xc = 0;
            yc = 0;
            while (yc < 19 && xc < 10) {
              if (tetfon[yc][xc] > 0) {
                if (x + 12 == xc * 12 && y + 24 == yc * 12) {
                  n = 4;
                }
              }
              xc = xc + 1;
              if (xc > 9) {
                xc = 0;
                yc = yc + 1;
              }
            }
          }
        }
      }
    }
    flag = false;
    tmrkill1 = millis();
  }
  if (digitalRead(LEFT_BUTT) == 0 && millis() - tmrkill2 > 150) {
    x = x - 12;
    if (i == 0) {
      if (n == 1) {
        xc = 0;
        yc = 0;
        while (yc < 20 && xc < 10) {
          if (tetfon[yc][xc] > 0) {
            if (x == xc * 12 && y == yc * 12) {
              x = x + 12;
            } else if (x + 12 == xc * 12 && y + 12 == yc * 12) {
              x = x + 12;
            }
          }
          xc = xc + 1;
          if (xc > 9) {
            xc = 0;
            yc = yc + 1;
          }
        }
        if (x < 0) {
          x = x + 12;
        }
      } else if (n == 2) {
        xc = 0;
        yc = 0;
        while (yc < 19 && xc < 10) {
          if (tetfon[yc][xc] > 0) {
            if (x + 24 == xc * 12 && y == yc * 12) {
              x = x + 12;
            } else if (x + 12 == xc * 12 && y + 12 == yc * 12) {
              x = x + 12;
            } else if (x + 12 == xc * 12 && y + 24 == yc * 12) {
              x = x + 12;
            }
          }
          xc = xc + 1;
          if (xc > 9) {
            xc = 0;
            yc = yc + 1;
          }
        }
        if (x < -12) {
          x = x + 12;
        }
      } else if (n == 3) {
        xc = 0;
        yc = 0;
        while (yc < 19 && xc < 10) {
          if (tetfon[yc][xc] > 0) {
            if (x == xc * 12 && y + 12 == yc * 12) {
              x = x + 12;
            } else if (x + 12 == xc * 12 && y + 24 == yc * 12) {
              x = x + 12;
            }
          }
          xc = xc + 1;
          if (xc > 9) {
            xc = 0;
            yc = yc + 1;
          }
        }
        if (x < 0) {
          x = x + 12;
        }
      } else if (n == 4) {
        xc = 0;
        yc = 0;
        while (yc < 19 && xc < 10) {
          if (tetfon[yc][xc] > 0) {
            if (x + 12 == xc * 12 && y == yc * 12) {
              x = x + 12;
            } else if (x == xc * 12 && y + 12 == yc * 12) {
              x = x + 12;
            } else if (x == xc * 12 && y + 24 == yc * 12) {
              x = x + 12;
            }
          }
          xc = xc + 1;
          if (xc > 9) {
            xc = 0;
            yc = yc + 1;
          }
        }
        if (x < 0) {
          x = x + 12;
        }
      }
    } else if (i == 1) {
      if (n == 1) {
        xc = 0;
        yc = 0;
        while (yc < 19 && xc < 10) {
          if (tetfon[yc][xc] > 0) {
            if (x + 12 == xc * 12 && y == yc * 12) {
              x = x + 12;
            } else if (x + 12 == xc * 12 && y + 12 == yc * 12) {
              x = x + 12;
            } else if (x + 12 == xc * 12 && y + 24 == yc * 12) {
              x = x + 12;
            }
          }
          xc = xc + 1;
          if (xc > 9) {
            xc = 0;
            yc = yc + 1;
          }
        }
        if (x < -12) {
          x = x + 12;
        }
      } else if (n == 2) {
        xc = 0;
        yc = 0;
        while (yc < 19 && xc < 10) {
          if (tetfon[yc][xc] > 0) {
            if (x == xc * 12 && y + 12 == yc * 12) {
              x = x + 12;
            } else if (x == xc * 12 && y + 24 == yc * 12) {
              x = x + 12;
            }
          }
          xc = xc + 1;
          if (xc > 9) {
            xc = 0;
            yc = yc + 1;
          }
        }
        if (x < 0) {
          x = x + 12;
        }
      } else if (n == 3) {
        xc = 0;
        yc = 0;
        while (yc < 19 && xc < 10) {
          if (tetfon[yc][xc] > 0) {
            if (x == xc * 12 && y == yc * 12) {
              x = x + 12;
            } else if (x + 12 == xc * 12 && y + 12 == yc * 12) {
              x = x + 12;
            } else if (x + 12 == xc * 12 && y + 24 == yc * 12) {
              x = x + 12;
            }
          }
          xc = xc + 1;
          if (xc > 9) {
            xc = 0;
            yc = yc + 1;
          }
        }
        if (x < 0) {
          x = x + 12;
        }
      } else if (n == 4) {
        xc = 0;
        yc = 0;
        while (yc < 19 && xc < 10) {
          if (tetfon[yc][xc] > 0) {
            if (x + 24 == xc * 12 && y == yc * 12) {
              x = x + 12;
            } else if (x == xc * 12 && y + 12 == yc * 12) {
              x = x + 12;
            }
          }
          xc = xc + 1;
          if (xc > 9) {
            xc = 0;
            yc = yc + 1;
          }
        }
        if (x < 0) {
          x = x + 12;
        }
      }
    } else if (i == 2) {
      if (n == 1) {
        xc = 0;
        yc = 0;
        while (yc < 19 && xc < 10) {
          if (tetfon[yc][xc] > 0) {
            if (x + 12 == xc * 12 && y == yc * 12) {
              x = x + 12;
            } else if (x + 12 == xc * 12 && y + 12 == yc * 12) {
              x = x + 12;
            } else if (x + 12 == xc * 12 && y + 24 == yc * 12) {
              x = x + 12;
            } else if (x + 12 == xc * 12 && y + 36 == yc * 12) {
              x = x + 12;
            }
          }
          xc = xc + 1;
          if (xc > 9) {
            xc = 0;
            yc = yc + 1;
          }
        }
        if (x < -12) {
          x = x + 12;
        }
      } else if (n == 2) {
        xc = 0;
        yc = 0;
        while (yc < 19 && xc < 10) {
          if (tetfon[yc][xc] > 0) {
            if (x == xc * 12 && y + 12 == yc * 12) {
              x = x + 12;
            }
          }
          xc = xc + 1;
          if (xc > 9) {
            xc = 0;
            yc = yc + 1;
          }
          if (x < 0) {
            x = x + 12;
          }
        }
      } else if (n == 3) {
        xc = 0;
        yc = 0;
        while (yc < 19 && xc < 10) {
          if (tetfon[yc][xc] > 0) {
            if (x + 24 == xc * 12 && y == yc * 12) {
              x = x + 12;
            } else if (x + 24 == xc * 12 && y + 12 == yc * 12) {
              x = x + 12;
            } else if (x + 24 == xc * 12 && y + 24 == yc * 12) {
              x = x + 12;
            } else if (x + 24 == xc * 12 && y + 36 == yc * 12) {
              x = x + 12;
            }
          }
          xc = xc + 1;
          if (xc > 9) {
            xc = 0;
            yc = yc + 1;
          }
        }
        if (x < -24) {
          x = x + 12;
        }
      } else if (n == 4) {
        xc = 0;
        yc = 0;
        while (yc < 19 && xc < 10) {
          if (tetfon[yc][xc] > 0) {
            if (x == xc * 12 && y + 24 == yc * 12) {
              x = x + 12;
            }
          }
          xc = xc + 1;
          if (xc > 9) {
            xc = 0;
            yc = yc + 1;
          }
        }
        if (x < 0) {
          x = x + 12;
        }
      }
    } else if (i == 3) {
      xc = 0;
      yc = 0;
      while (yc < 19 && xc < 10) {
        if (tetfon[yc][xc] > 0) {
          if (x == xc * 12 && y == yc * 12) {
            x = x + 12;
          } else if (x == xc * 12 && y + 12 == yc * 12) {
            x = x + 12;
          }
        }
        xc = xc + 1;
        if (xc > 9) {
          xc = 0;
          yc = yc + 1;
        }
      }
      if (x < 0) {
        x = x + 12;
      }
    } else if (i == 4) {
      if (n == 1) {
        xc = 0;
        yc = 0;
        while (yc < 19 && xc < 10) {
          if (tetfon[yc][xc] > 0) {
            if (x + 12 == xc * 12 && y == yc * 12) {
              x = x + 12;
            } else if (x + 12 == xc * 12 && y + 12 == yc * 12) {
              x = x + 12;
            } else if (x + 12 == xc * 12 && y + 24 == yc * 12) {
              x = x + 12;
            }
          }
          xc = xc + 1;
          if (xc > 9) {
            xc = 0;
            yc = yc + 1;
          }
        }
        if (x < -12) {
          x = x + 12;
        }
      } else if (n == 2) {
        xc = 0;
        yc = 0;
        while (yc < 19 && xc < 10) {
          if (tetfon[yc][xc] > 0) {
            if (x == xc * 12 && y + 12 == yc * 12) {
              x = x + 12;
            } else if (x + 12 == xc * 12 && y + 24 == yc * 12) {
              x = x + 12;
            }
          }
          xc = xc + 1;
          if (xc > 9) {
            xc = 0;
            yc = yc + 1;
          }
        }
        if (x < 0) {
          x = x + 12;
        }
      } else if (n == 3) {
        xc = 0;
        yc = 0;
        while (yc < 19 && xc < 10) {
          if (tetfon[yc][xc] > 0) {
            if (x + 12 == xc * 12 && y == yc * 12) {
              x = x + 12;
            } else if (x == xc * 12 && y + 12 == yc * 12) {
              x = x + 12;
            } else if (x + 12 == xc * 12 && y + 24 == yc * 12) {
              x = x + 12;
            }
          }
          xc = xc + 1;
          if (xc > 9) {
            xc = 0;
            yc = yc + 1;
          }
        }
        if (x < 0) {
          x = x + 12;
        }
      } else if (n == 4) {
        xc = 0;
        yc = 0;
        while (yc < 19 && xc < 10) {
          if (tetfon[yc][xc] > 0) {
            if (x + 12 == xc * 12 && y == yc * 12) {
              x = x + 12;
            } else if (x == xc * 12 && y + 12 == yc * 12) {
              x = x + 12;
            }
          }
          xc = xc + 1;
          if (xc > 9) {
            xc = 0;
            yc = yc + 1;
          }
        }
        if (x < 0) {
          x = x + 12;
        }
      }
    }
    tmrkill2 = millis();
  } else if (digitalRead(RIGHT_BUTT) == 0 && millis() - tmrkill3 > 150) {
    x = x + 12;
    if (i == 0) {
      if (n == 1) {
        xc = 0;
        yc = 0;
        while (yc < 20 && xc < 10) {
          if (tetfon[yc][xc] > 0) {
            if (x + 12 == xc * 12 && y == yc * 12) {
              x = x - 12;
            } else if (x + 24 == xc * 12 && y + 12 == yc * 12) {
              x = x - 12;
            }
          }
          xc = xc + 1;
          if (xc > 9) {
            xc = 0;
            yc = yc + 1;
          }
        }
        if (x > 84) {
          x = x - 12;
        }
      } else if (n == 2) {
        xc = 0;
        yc = 0;
        while (yc < 19 && xc < 10) {
          if (tetfon[yc][xc] > 0) {
            if (x + 24 == xc * 12 && y == yc * 12) {
              x = x - 12;
            } else if (x + 24 == xc * 12 && y + 12 == yc * 12) {
              x = x - 12;
            } else if (x + 12 == xc * 12 && y + 24 == yc * 12) {
              x = x - 12;
            }
          }
          xc = xc + 1;
          if (xc > 9) {
            xc = 0;
            yc = yc + 1;
          }
        }
        if (x > 84) {
          x = x - 12;
        }
      } else if (n == 3) {
        xc = 0;
        yc = 0;
        while (yc < 19 && xc < 10) {
          if (tetfon[yc][xc] > 0) {
            if (x + 12 == xc * 12 && y + 12 == yc * 12) {
              x = x - 12;
            } else if (x + 24 == xc * 12 && y + 24 == yc * 12) {
              x = x - 12;
            }
          }
          xc = xc + 1;
          if (xc > 9) {
            xc = 0;
            yc = yc + 1;
          }
        }
        if (x > 84) {
          x = x - 12;
        }
      } else if (n == 4) {
        xc = 0;
        yc = 0;
        while (yc < 19 && xc < 10) {
          if (tetfon[yc][xc] > 0) {
            if (x + 12 == xc * 12 && y == yc * 12) {
              x = x - 12;
            } else if (x + 12 == xc * 12 && y + 12 == yc * 12) {
              x = x - 12;
            } else if (x == xc * 12 && y + 24 == yc * 12) {
              x = x - 12;
            }
          }
          xc = xc + 1;
          if (xc > 9) {
            xc = 0;
            yc = yc + 1;
          }
        }
        if (x > 96) {
          x = x - 12;
        }
      }
    } else if (i == 1) {
      if (n == 1) {
        xc = 0;
        yc = 0;
        while (yc < 19 && xc < 10) {
          if (tetfon[yc][xc] > 0) {
            if (x + 12 == xc * 12 && y == yc * 12) {
              x = x - 12;
            } else if (x + 12 == xc * 12 && y + 12 == yc * 12) {
              x = x - 12;
            } else if (x + 24 == xc * 12 && y + 24 == yc * 12) {
              x = x - 12;
            }
          }
          xc = xc + 1;
          if (xc > 9) {
            xc = 0;
            yc = yc + 1;
          }
        }
        if (x > 84) {
          x = x - 12;
        }
      } else if (n == 2) {
        xc = 0;
        yc = 0;
        while (yc < 19 && xc < 10) {
          if (tetfon[yc][xc] > 0) {
            if (x + 24 == xc * 12 && y + 12 == yc * 12) {
              x = x - 12;
            } else if (x == xc * 12 && y + 24 == yc * 12) {
              x = x - 12;
            }
          }
          xc = xc + 1;
          if (xc > 9) {
            xc = 0;
            yc = yc + 1;
          }
        }
        if (x > 84) {
          x = x - 12;
        }
      } else if (n == 3) {
        xc = 0;
        yc = 0;
        while (yc < 19 && xc < 10) {
          if (tetfon[yc][xc] > 0) {
            if (x + 12 == xc * 12 && y == yc * 12) {
              x = x - 12;
            } else if (x + 12 == xc * 12 && y + 12 == yc * 12) {
              x = x - 12;
            } else if (x + 12 == xc * 12 && y + 24 == yc * 12) {
              x = x - 12;
            }
          }
          xc = xc + 1;
          if (xc > 9) {
            xc = 0;
            yc = yc + 1;
          }
        }
        if (x > 96) {
          x = x - 12;
        }
      } else if (n == 4) {
        xc = 0;
        yc = 0;
        while (yc < 19 && xc < 10) {
          if (tetfon[yc][xc] > 0) {
            if (x + 24 == xc * 12 && y == yc * 12) {
              x = x - 12;
            } else if (x + 24 == xc * 12 && y + 12 == yc * 12) {
              x = x - 12;
            }
          }
          xc = xc + 1;
          if (xc > 9) {
            xc = 0;
            yc = yc + 1;
          }
        }
        if (x > 84) {
          x = x - 12;
        }
      }
    } else if (i == 2) {
      if (n == 1) {
        xc = 0;
        yc = 0;
        while (yc < 19 && xc < 10) {
          if (tetfon[yc][xc] > 0) {
            if (x + 12 == xc * 12 && y == yc * 12) {
              x = x - 12;
            } else if (x + 12 == xc * 12 && y + 12 == yc * 12) {
              x = x - 12;
            } else if (x + 12 == xc * 12 && y + 24 == yc * 12) {
              x = x - 12;
            } else if (x + 12 == xc * 12 && y + 36 == yc * 12) {
              x = x - 12;
            }
          }
          xc = xc + 1;
          if (xc > 9) {
            xc = 0;
            yc = yc + 1;
          }
        }
        if (x > 96) {
          x = x - 12;
        }
      } else if (n == 2) {
        xc = 0;
        yc = 0;
        while (yc < 19 && xc < 10) {
          if (tetfon[yc][xc] > 0) {
            if (x + 36 == xc * 12 && y + 12 == yc * 12) {
              x = x - 12;
            }
          }
          xc = xc + 1;
          if (xc > 9) {
            xc = 0;
            yc = yc + 1;
          }
          if (x > 72) {
            x = x - 12;
          }
        }
      } else if (n == 3) {
        xc = 0;
        yc = 0;
        while (yc < 19 && xc < 10) {
          if (tetfon[yc][xc] > 0) {
            if (x + 24 == xc * 12 && y == yc * 12) {
              x = x - 12;
            } else if (x + 24 == xc * 12 && y + 12 == yc * 12) {
              x = x - 12;
            } else if (x + 24 == xc * 12 && y + 24 == yc * 12) {
              x = x - 12;
            } else if (x + 24 == xc * 12 && y + 36 == yc * 12) {
              x = x - 12;
            }
          }
          xc = xc + 1;
          if (xc > 9) {
            xc = 0;
            yc = yc + 1;
          }
        }
        if (x > 84) {
          x = x - 12;
        }
      } else if (n == 4) {
        xc = 0;
        yc = 0;
        while (yc < 19 && xc < 10) {
          if (tetfon[yc][xc] > 0) {
            if (x + 36 == xc * 12 && y + 24 == yc * 12) {
              x = x - 12;
            }
          }
          xc = xc + 1;
          if (xc > 9) {
            xc = 0;
            yc = yc + 1;
          }
        }
        if (x > 72) {
          x = x - 12;
        }
      }
    } else if (i == 3) {
      xc = 0;
      yc = 0;
      while (yc < 19 && xc < 10) {
        if (tetfon[yc][xc] > 0) {
          if (x + 12 == xc * 12 && y == yc * 12) {
            x = x - 12;
          } else if (x + 12 == xc * 12 && y + 12 == yc * 12) {
            x = x - 12;
          }
        }
        xc = xc + 1;
        if (xc > 9) {
          xc = 0;
          yc = yc + 1;
        }
      }
      if (x > 96) {
        x = x - 12;
      }
    } else if (i == 4) {
      if (n == 1) {
        xc = 0;
        yc = 0;
        while (yc < 19 && xc < 10) {
          if (tetfon[yc][xc] > 0) {
            if (x + 12 == xc * 12 && y == yc * 12) {
              x = x - 12;
            } else if (x + 24 == xc * 12 && y + 12 == yc * 12) {
              x = x - 12;
            } else if (x + 12 == xc * 12 && y + 24 == yc * 12) {
              x = x - 12;
            }
          }
          xc = xc + 1;
          if (xc > 9) {
            xc = 0;
            yc = yc + 1;
          }
        }
        if (x > 84) {
          x = x - 12;
        }
      } else if (n == 2) {
        xc = 0;
        yc = 0;
        while (yc < 19 && xc < 10) {
          if (tetfon[yc][xc] > 0) {
            if (x + 24 == xc * 12 && y + 12 == yc * 12) {
              x = x - 12;
            } else if (x + 12 == xc * 12 && y + 24 == yc * 12) {
              x = x - 12;
            }
          }
          xc = xc + 1;
          if (xc > 9) {
            xc = 0;
            yc = yc + 1;
          }
        }
        if (x > 84) {
          x = x - 12;
        }
      } else if (n == 3) {
        xc = 0;
        yc = 0;
        while (yc < 19 && xc < 10) {
          if (tetfon[yc][xc] > 0) {
            if (x + 12 == xc * 12 && y == yc * 12) {
              x = x - 12;
            } else if (x + 12 == xc * 12 && y + 12 == yc * 12) {
              x = x - 12;
            } else if (x + 12 == xc * 12 && y + 24 == yc * 12) {
              x = x - 12;
            }
          }
          xc = xc + 1;
          if (xc > 9) {
            xc = 0;
            yc = yc + 1;
          }
        }
        if (x > 96) {
          x = x - 12;
        }
      } else if (n == 4) {
        xc = 0;
        yc = 0;
        while (yc < 19 && xc < 10) {
          if (tetfon[yc][xc] > 0) {
            if (x + 12 == xc * 12 && y == yc * 12) {
              x = x - 12;
            } else if (x + 24 == xc * 12 && y + 12 == yc * 12) {
              x = x - 12;
            }
          }
          xc = xc + 1;
          if (xc > 9) {
            xc = 0;
            yc = yc + 1;
          }
        }
        if (x > 84) {
          x = x - 12;
        }
      }
    }
    tmrkill3 = millis();
  }
  p = 0;
  hp = 0;
  while (p < 20) {
    if (tetfon[p][0] > 0 && tetfon[p][1] > 0 && tetfon[p][2] > 0 && tetfon[p][3] > 0 && tetfon[p][4] > 0 && tetfon[p][5] > 0 && tetfon[p][6] > 0 && tetfon[p][7] > 0 && tetfon[p][8] > 0 && tetfon[p][9] > 0) {
      tft.fillRect(0, p * 12, 120, 12, 0x0000);
      q = p;
      while (q > 0) {
        tetfon[q][0] = tetfon[q - 1][0];
        tetfon[q][1] = tetfon[q - 1][1];
        tetfon[q][2] = tetfon[q - 1][2];
        tetfon[q][3] = tetfon[q - 1][3];
        tetfon[q][4] = tetfon[q - 1][4];
        tetfon[q][5] = tetfon[q - 1][5];
        tetfon[q][6] = tetfon[q - 1][6];
        tetfon[q][7] = tetfon[q - 1][7];
        tetfon[q][8] = tetfon[q - 1][8];
        tetfon[q][9] = tetfon[q - 1][9];
        q = q - 1;
      }
      tetfon[0][0] = 0;
      tetfon[0][1] = 0;
      tetfon[0][2] = 0;
      tetfon[0][3] = 0;
      tetfon[0][4] = 0;
      tetfon[0][5] = 0;
      tetfon[0][6] = 0;
      tetfon[0][7] = 0;
      tetfon[0][8] = 0;
      tetfon[0][9] = 0;
      e2 = true;
      hp = hp + 1;
    }
    p = p + 1;
  }
  if (e2 == true) {
    if (hp == 1) {
      s = s + 10;
    } else if (hp == 2) {
      s = s + 25;
    } else if (hp == 3) {
      s = s + 50;
    } else if (hp == 4) {
      s = s + 100;
    }
    e2 = false;
    xh = 0;
    yh = 0;
    while (yh < 20) {
      if (tetfon[yh][xh] == 0) {
        tft.fillRect(xh * 12, yh * 12, 12, 12, 0x0000);
      } else if (tetfon[yh][xh] == 1) {
        tft.drawRGBBitmap(xh * 12, yh * 12, partR, 12, 12);
      } else if (tetfon[yh][xh] == 2) {
        tft.drawRGBBitmap(xh * 12, yh * 12, partY, 12, 12);
      } else if (tetfon[yh][xh] == 3) {
        tft.drawRGBBitmap(xh * 12, yh * 12, partG, 12, 12);
      } else if (tetfon[yh][xh] == 4) {
        tft.drawRGBBitmap(xh * 12, yh * 12, partP, 12, 12);
      } else if (tetfon[yh][xh] == 5) {
        tft.drawRGBBitmap(xh * 12, yh * 12, partB, 12, 12);
      }
      xh = xh + 1;
      if (xh > 9) {
        xh = 0;
        yh = yh + 1;
      }
    }
  }
  if (x != xst || y != yst || nst != n) {
    if (i == 0) {
      if (nst == 1) {
        tft.drawBitmap(xst, yst, tetB0n1, 36, 36, 0x0000);
      } else if (nst == 2) {
        tft.drawBitmap(xst, yst, tetB0n2, 36, 36, 0x0000);
      } else if (nst == 3) {
        tft.drawBitmap(xst, yst, tetB0n3, 36, 36, 0x0000);
      } else if (nst == 4) {
        tft.drawBitmap(xst, yst, tetB0n4, 36, 36, 0x0000);
      }
    } else if (i == 1) {
      if (nst == 1) {
        tft.drawBitmap(xst, yst, tetB1n1, 36, 36, 0x0000);
      } else if (nst == 2) {
        tft.drawBitmap(xst, yst, tetB1n2, 36, 36, 0x0000);
      } else if (nst == 3) {
        tft.drawBitmap(xst, yst, tetB1n3, 36, 36, 0x0000);
      } else if (nst == 4) {
        tft.drawBitmap(xst, yst, tetB1n4, 36, 36, 0x0000);
      }
    } else if (i == 2) {
      if (nst == 1) {
        tft.drawBitmap(xst, yst, tetB2n1, 48, 48, 0x0000);
      } else if (nst == 2) {
        tft.drawBitmap(xst, yst, tetB2n2, 48, 48, 0x0000);
      } else if (nst == 3) {
        tft.drawBitmap(xst, yst, tetB2n3, 48, 48, 0x0000);
      } else if (nst == 4) {
        tft.drawBitmap(xst, yst, tetB2n4, 48, 48, 0x0000);
      }
    } else if (i == 3) {
      tft.drawBitmap(xst, yst, tetB3, 24, 24, 0x0000);

    } else if (i == 4) {
      if (nst == 1) {
        tft.drawBitmap(xst, yst, tetB4n1, 36, 36, 0x0000);
      } else if (nst == 2) {
        tft.drawBitmap(xst, yst, tetB4n2, 36, 36, 0x0000);
      } else if (nst == 3) {
        tft.drawBitmap(xst, yst, tetB4n3, 36, 36, 0x0000);
      } else if (nst == 4) {
        tft.drawBitmap(xst, yst, tetB4n4, 36, 36, 0x0000);
      }
    }
    if (i == 0) {
      if (n == 1) {
        tft.drawBitmap(x, y, tetB0n1_1, 36, 36, 0xD801);
        tft.drawBitmap(x, y, tetB0n1_2, 36, 36, 0xFB40);
      } else if (n == 2) {
        tft.drawBitmap(x, y, tetB0n2_1, 36, 36, 0xD801);
        tft.drawBitmap(x, y, tetB0n2_2, 36, 36, 0xFB40);
      } else if (n == 3) {
        tft.drawBitmap(x, y, tetB0n3_1, 36, 36, 0xD801);
        tft.drawBitmap(x, y, tetB0n3_2, 36, 36, 0xFB40);
      } else if (n == 4) {
        tft.drawBitmap(x, y, tetB0n4_1, 36, 36, 0xD801);
        tft.drawBitmap(x, y, tetB0n4_2, 36, 36, 0xFB40);
      }
    } else if (i == 1) {
      if (n == 1) {
        tft.drawBitmap(x, y, tetB1n1_1, 36, 36, 0x7B40);
        tft.drawBitmap(x, y, tetB1n1_2, 36, 36, 0xFEA0);
      } else if (n == 2) {
        tft.drawBitmap(x, y, tetB1n2_1, 36, 36, 0x7B40);
        tft.drawBitmap(x, y, tetB1n2_2, 36, 36, 0xFEA0);
      } else if (n == 3) {
        tft.drawBitmap(x, y, tetB1n3_1, 36, 36, 0x7B40);
        tft.drawBitmap(x, y, tetB1n3_2, 36, 36, 0xFEA0);
      } else if (n == 4) {
        tft.drawBitmap(x, y, tetB1n4_1, 36, 36, 0x7B40);
        tft.drawBitmap(x, y, tetB1n4_2, 36, 36, 0xFEA0);
      }
    } else if (i == 2) {
      if (n == 1) {
        tft.drawBitmap(x, y, tetB2n1_1, 48, 48, 0x23E0);
        tft.drawBitmap(x, y, tetB2n1_2, 48, 48, 0x07E4);
      } else if (n == 2) {
        tft.drawBitmap(x, y, tetB2n2_1, 48, 48, 0x23E0);
        tft.drawBitmap(x, y, tetB2n2_2, 48, 48, 0x07E4);
      } else if (n == 3) {
        tft.drawBitmap(x, y, tetB2n3_1, 48, 48, 0x23E0);
        tft.drawBitmap(x, y, tetB2n3_2, 48, 48, 0x07E4);
      } else if (n == 4) {
        tft.drawBitmap(x, y, tetB2n4_1, 48, 48, 0x23E0);
        tft.drawBitmap(x, y, tetB2n4_2, 48, 48, 0x07E4);
      }
    } else if (i == 3) {
      tft.drawBitmap(x, y, tetB3_1, 24, 24, 0xA81F);
      tft.drawBitmap(x, y, tetB3_2, 24, 24, 0xF81A);
    } else if (i == 4) {
      if (n == 1) {
        tft.drawBitmap(x, y, tetB4n1_1, 36, 36, 0x013F);
        tft.drawBitmap(x, y, tetB4n1_2, 36, 36, 0x049F);
      } else if (n == 2) {
        tft.drawBitmap(x, y, tetB4n2_1, 36, 36, 0x013F);
        tft.drawBitmap(x, y, tetB4n2_2, 36, 36, 0x049F);
      } else if (n == 3) {
        tft.drawBitmap(x, y, tetB4n3_1, 36, 36, 0x013F);
        tft.drawBitmap(x, y, tetB4n3_2, 36, 36, 0x049F);
      } else if (n == 4) {
        tft.drawBitmap(x, y, tetB4n4_1, 36, 36, 0x013F);
        tft.drawBitmap(x, y, tetB4n4_2, 36, 36, 0x049F);
      }
    }
  }
}

void stopT() {
  tft.drawRoundRect(54, 60, 212, 120, 10, ST77XX_WHITE);
  tft.drawRoundRect(55, 61, 210, 118, 9, ST77XX_WHITE);
  tft.drawRoundRect(56, 62, 208, 116, 8, ST77XX_BLACK);
  tft.drawRoundRect(57, 63, 206, 114, 7, ST77XX_BLACK);
  tft.drawRoundRect(58, 64, 204, 112, 6, ST77XX_WHITE);
  tft.drawRoundRect(59, 65, 202, 110, 5, ST77XX_WHITE);
  tft.fillRoundRect(60, 66, 200, 108, 4, ST77XX_BLACK);
  tft.drawBitmap(62, 72, go, 196, 25, ST77XX_WHITE);
  tft.setCursor(100, 120);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);
  tft.print("skore = " + String(s));
  tft.setCursor(100, 140);
  topskore = pref.getUInt("skoreT", 0);
  tft.print("top skore = " + String(topskore));
  if (s > topskore) {
    pref.putUInt("skoreT", s);
  }
  delay(200);
  while (digitalRead(OK_BUTT) == 0) {
    if (digitalRead(HOME_BUTT) == 1) {
      zvukbool = true;
      zv = 2;
      tmrzvuk = millis();
      zvuk();
      igranach = false;
      break;
    }
  }
  x = 0;
  y = 0;
  while (y < 20) {
    while (x < 10) {
      tetfon[y][x] = 0;
      x = x + 1;
    }
    x = 0;
    y = y + 1;
  }
  if (igranach == false) {} else {
    tft.fillScreen(0x0000);
    tft.drawLine(121, 0, 121, 240, ST77XX_GREEN);
    igranach = true;
    kill0 = true;
    n = 0;
    x = 0;
    y = 0;
    i = 0;
    s = -1;
    e2 = true;
  }
}

void mob0() {
  p = p + 1;
  if (millis() - tmr2 > 1000) {
    Serial.println (p);
    p = 0;
    tmr2 = millis();
  }
  if (millis() - tmr3 > 200) {
    sel = true;
  }
  if (digitalRead(LEFT_BUTT) == 0) {
    x = x - 1;
    flag = true;
    sel = false;
    tmr3 = millis();
    if (mapM[y + 7][x + 9] > 0) {
      x = x + 1;
      flag = false;
      sel = true;
    }
    t = 1;
    tmr3 = millis();
  } else if (digitalRead(RIGHT_BUTT) == 0) {
    x = x + 1;
    flag = true;
    sel = false;
    if (mapM[y + 7][x + 9] > 0) {
      x = x - 1;
      flag = false;
      sel = true;
    }
    t = 2;
    tmr3 = millis();
  } else if (digitalRead(UP_BUTT) == 0) {
    y = y - 1;
    flag = true;
    sel = false;
    if (mapM[y + 7][x + 9] > 0) {
      y = y + 1;
      flag = false;
      sel = true;
    }
    t = 3;
    tmr3 = millis();
  } else if (digitalRead(DOWN_BUTT) == 0) {
    y = y + 1;
    flag = true;
    sel = false;
    if (mapM[y + 7][x + 9] > 0) {
      y = y - 1;
      flag = false;
      sel = true;
    }
    t = 4;
    tmr3 = millis();
  }
  if (sel == true) {
    if (t == 1) {
      tft.drawRGBBitmap(144, 112, m0_l_s, 16, 16);
    } else if (t == 2) {
      tft.drawRGBBitmap(144, 112, m0_r_s, 16, 16);
    } else if (t == 3) {
      tft.drawRGBBitmap(144, 112, m0_u_s, 16, 16);
    } else if (t == 4) {
      tft.drawRGBBitmap(144, 112, m0_d_s, 16, 16);
    }
    sel = false;
    tmr3 = millis();
  }
  if (flag == true) {
    if (t == 4 || t == 3) {
      for (int i = 0; i < 15; i++) {
        sy = i;
        for (int a = 0; a < 19; a++) {
          sx = a;
          xys = mapM[sy + y][sx + x];
          c = true;
          if (t == 1 && xys == mapM[y + sy][x + sx + 1]) {
            c = false;
          } else if (t == 2 && xys == mapM[y + sy][x + sx - 1]) {
            c = false;
          } else if (t == 3 && xys == mapM[y + sy + 1][x + sx]) {
            c = false;
          } else if (t == 4 && xys == mapM[y + sy - 1][x + sx]) {
            c = false;
          }
          if (sy == 7 && sx == 9) {
            c = false;
          }
          if (c == true) {
            if (xys == 0) {
              tft.fillRect(sx * 16, sy * 16, 16, 16, 0x740E);
            } else if (xys == 1) {
              tft.fillRect(sx * 16, sy * 16, 16, 16, 0x39E7);
            } else if (xys == 2) {
              tft.fillRect(sx * 16, sy * 16, 16, 16, 0x39E7);
            } else if (xys == -1) {
              tft.fillRect(sx * 16, sy * 16, 16, 16, 0xF800);
            }
          }
        }
      }
    } else {
      for (int i = 0; i < 19; i++) {
        sx = i;
        for (int a = 0; a < 15; a++) {
          sy = a;
          xys = mapM[sy + y][sx + x];
          c = true;
          if (t == 1 && xys == mapM[y + sy][x + sx + 1]) {
            c = false;
          } else if (t == 2 && xys == mapM[y + sy][x + sx - 1]) {
            c = false;
          } else if (t == 3 && xys == mapM[y + sy + 1][x + sx]) {
            c = false;
          } else if (t == 4 && xys == mapM[y + sy - 1][x + sx]) {
            c = false;
          }
          if (sy == 7 && sx == 9) {
            c = false;
          }
          if (c == true) {
            if (xys == 0) {
              tft.fillRect(sx * 16, sy * 16, 16, 16, 0x740E);
            } else if (xys == 1) {
              tft.fillRect(sx * 16, sy * 16, 16, 16, 0x39E7);
            } else if (xys == 2) {
              tft.fillRect(sx * 16, sy * 16, 16, 16, 0x39E7);
            } else if (xys == -1) {
              tft.fillRect(sx * 16, sy * 16, 16, 16, 0xF800);
            }
          }
        }
      }
    }
    e2 = !e2;
    if (t == 1) {
      if (e2 == true) {
        tft.drawRGBBitmap(144, 112, m0_l_1, 16, 16);
      } else {
        tft.drawRGBBitmap(144, 112, m0_l_2, 16, 16);
      }
    } else if (t == 2) {
      if (e2 == true) {
        tft.drawRGBBitmap(144, 112, m0_r_1, 16, 16);
      } else {
        tft.drawRGBBitmap(144, 112, m0_r_2, 16, 16);
      }
    } else if (t == 3) {
      if (e2 == true) {
        tft.drawRGBBitmap(144, 112, m0_u_1, 16, 16);
      } else {
        tft.drawRGBBitmap(144, 112, m0_u_2, 16, 16);
      }
    } else if (t == 4) {
      if (e2 == true) {
        tft.drawRGBBitmap(144, 112, m0_d_1, 16, 16);
      } else {
        tft.drawRGBBitmap(144, 112, m0_d_2, 16, 16);
      }
    }
    flag = false;
  }
  tmr1 = millis();
}

void delamaze() {
  s = s + 1;
  xbot1 = 80;
  ybot1 = 80;
  for (int i = 0; i < 120; i++) {
    sy = i;
    for (int m = 0; m < 120; m++) {
      sx = m;
      mapM[sy][sx] = 2;
    }
  }
  for (int i = 0; i < ybot1; i++) {
    sy = i;
    for (int m = 0; m < xbot1; m++) {
      sx = m;
      mapM[sy + 10][sx + 10] = 1;
    }
  }
  ran1 = random(1, 5);
  if (ran1 == 1) {
    ran1 = random(12, 8 + xbot1);
    while (ran1 % 2 == 1) {
      ran1 = random(12, 8 + xbot1);
    }
    x = ran1 - 9;
    y = 2;
    mapM[9][ran1] = 0;
    mapM[10][ran1] = 0;
    mapM[11][ran1] = 0;
    mapM[12][ran1] = 0;
    mapM[13][ran1] = 0;
    mapM[14][ran1] = 0;
    mapM[14][ran1 + 1] = 0;
    mapM[14][ran1 - 1] = 0;
    mapM[14][ran1 + 2] = 0;
    mapM[14][ran1 - 2] = 0;
    mapM[15][ran1 + 2] = 0;
    mapM[15][ran1 - 2] = 0;
    mapM[16][ran1 + 2] = 0;
    mapM[16][ran1 - 2] = 0;
    mapM[16][ran1 - 1] = 0;
    mapM[16][ran1 + 1] = 0;
    mapM[16][ran1] = 0;
    ran2 = ran1;
    ran1 = 16;
  } else if (ran1 == 2) {
    ran1 = random(12, 8 + ybot1);
    while (ran1 % 2 == 1) {
      ran1 = random(12, 8 + ybot1);
    }
    y = ran1 - 7;
    x = xbot1 + 2;
    mapM[ran1][xbot1 + 11] = 0;
    mapM[ran1][xbot1 + 10] = 0;
    mapM[ran1][xbot1 + 9] = 0;
    mapM[ran1][xbot1 + 8] = 0;
    mapM[ran1][xbot1 + 7] = 0;
    mapM[ran1][xbot1 + 6] = 0;
    mapM[ran1 + 1][xbot1 + 6] = 0;
    mapM[ran1 - 1][xbot1 + 6] = 0;
    mapM[ran1 + 2][xbot1 + 6] = 0;
    mapM[ran1 - 2][xbot1 + 6] = 0;
    mapM[ran1 + 2][xbot1 + 5] = 0;
    mapM[ran1 - 2][xbot1 + 5] = 0;
    mapM[ran1 + 2][xbot1 + 4] = 0;
    mapM[ran1 - 2][xbot1 + 4] = 0;
    mapM[ran1 - 1][xbot1 + 4] = 0;
    mapM[ran1 + 1][xbot1 + 4] = 0;
    mapM[ran1][xbot1 + 4] = 0;
    ran2 = ran1;
    ran1 = xbot1 + 4;
  } else if (ran1 == 3) {
    ran1 = random(12, 8 + xbot1);
    while (ran1 % 2 == 1) {
      ran1 = random(12, 8 + xbot1);
    }
    x = ran1 - 9;
    y = 3 + ybot1;
    mapM[11 + ybot1][ran1] = 0;
    mapM[10 + ybot1][ran1] = 0;
    mapM[9 + ybot1][ran1] = 0;
    mapM[8 + ybot1][ran1] = 0;
    mapM[7 + ybot1][ran1] = 0;
    mapM[6 + ybot1][ran1] = 0;
    mapM[6 + ybot1][ran1 + 1] = 0;
    mapM[6 + ybot1][ran1 - 1] = 0;
    mapM[6 + ybot1][ran1 + 2] = 0;
    mapM[6 + ybot1][ran1 - 2] = 0;
    mapM[5 + ybot1][ran1 + 2] = 0;
    mapM[5 + ybot1][ran1 - 2] = 0;
    mapM[4 + ybot1][ran1 + 2] = 0;
    mapM[4 + ybot1][ran1 - 2] = 0;
    mapM[4 + ybot1][ran1 - 1] = 0;
    mapM[4 + ybot1][ran1 + 1] = 0;
    mapM[4 + ybot1][ran1] = 0;
    ran2 = 4 + ybot1;
  } else if (ran1 == 4) {
    ran1 = random(12, 8 + ybot1);
    while (ran1 % 2 == 1) {
      ran1 = random(12, 8 + ybot1);
    }
    y = ran1 - 7;
    x = 1;
    mapM[ran1][9] = 0;
    mapM[ran1][10] = 0;
    mapM[ran1][11] = 0;
    mapM[ran1][12] = 0;
    mapM[ran1][13] = 0;
    mapM[ran1][14] = 0;
    mapM[ran1 + 1][14] = 0;
    mapM[ran1 - 1][14] = 0;
    mapM[ran1 + 2][14] = 0;
    mapM[ran1 - 2][14] = 0;
    mapM[ran1 + 2][15] = 0;
    mapM[ran1 - 2][15] = 0;
    mapM[ran1 + 2][16] = 0;
    mapM[ran1 - 2][16] = 0;
    mapM[ran1 - 1][16] = 0;
    mapM[ran1 + 1][16] = 0;
    mapM[ran1][16] = 0;
    ran2 = 16;
  }
  for (int i = 0; i < (xbot1 * 20); i++) {
    n = random(1, 5);
    if (n == 1) {
      if (mapM[ran1 - 3][ran2] == 1) {
        mapM[ran1 - 2][ran2] = 0;
        mapM[ran1 - 1][ran2] = 0;
        ran1 = ran1 - 2;
      }
    } else if (n == 2) {
      if (mapM[ran1][ran2 + 3] == 1) {
        mapM[ran1][ran2 + 2] = 0;
        mapM[ran1][ran2 + 1] = 0;
        ran2 = ran2 + 2;
      }
    } else if (n == 3) {
      if (mapM[ran1 + 3][ran2] == 1) {
        mapM[ran1 + 2][ran2] = 0;
        mapM[ran1 + 1][ran2] = 0;
        ran1 = ran1 + 2;
      }
    } else if (n == 4) {
      if (mapM[ran1][ran2 - 3] == 1) {
        mapM[ran1][ran2 - 2] = 0;
        mapM[ran1][ran2 - 1] = 0;
        ran2 = ran2 - 2;
      }
    }
  }
  ran1 = random(1, 5);
  if (ran1 == 1) {
    ran1 = random(12, 8 + xbot1);
    while (ran1 % 2 == 1) {
      ran1 = random(12, 8 + xbot1);
    }
    xbot2 = ran1 - 9;
    ybot2 = 2;
    mapM[9][ran1] = -1;
    mapM[10][ran1] = 0;
    mapM[11][ran1] = 0;
    mapM[12][ran1] = 0;
    mapM[13][ran1] = 0;
    mapM[14][ran1] = 0;
    mapM[14][ran1 + 1] = 0;
    mapM[14][ran1 - 1] = 0;
    mapM[14][ran1 + 2] = 0;
    mapM[14][ran1 - 2] = 0;
    mapM[15][ran1 + 2] = 0;
    mapM[15][ran1 - 2] = 0;
    mapM[16][ran1 + 2] = 0;
    mapM[16][ran1 - 2] = 0;
    mapM[16][ran1 - 1] = 0;
    mapM[16][ran1 + 1] = 0;
    mapM[16][ran1] = 0;
    ran2 = ran1;
    ran1 = 16;
  } else if (ran1 == 2) {
    ran1 = random(12, 8 + ybot1);
    while (ran1 % 2 == 1) {
      ran1 = random(12, 8 + ybot1);
    }
    ybot2 = ran1 - 7;
    xbot2 = xbot1 + 2;
    mapM[ran1][xbot1 + 11] = -1;
    mapM[ran1][xbot1 + 10] = 0;
    mapM[ran1][xbot1 + 9] = 0;
    mapM[ran1][xbot1 + 8] = 0;
    mapM[ran1][xbot1 + 7] = 0;
    mapM[ran1][xbot1 + 6] = 0;
    mapM[ran1 + 1][xbot1 + 6] = 0;
    mapM[ran1 - 1][xbot1 + 6] = 0;
    mapM[ran1 + 2][xbot1 + 6] = 0;
    mapM[ran1 - 2][xbot1 + 6] = 0;
    mapM[ran1 + 2][xbot1 + 5] = 0;
    mapM[ran1 - 2][xbot1 + 5] = 0;
    mapM[ran1 + 2][xbot1 + 4] = 0;
    mapM[ran1 - 2][xbot1 + 4] = 0;
    mapM[ran1 - 1][xbot1 + 4] = 0;
    mapM[ran1 + 1][xbot1 + 4] = 0;
    mapM[ran1][xbot1 + 4] = 0;
    ran2 = ran1;
    ran1 = xbot1 + 4;
  } else if (ran1 == 3) {
    ran1 = random(12, 8 + xbot1);
    while (ran1 % 2 == 1) {
      ran1 = random(12, 8 + xbot1);
    }
    xbot2 = ran1 - 9;
    ybot2 = 3 + ybot1;
    mapM[11 + ybot1][ran1] = -1;
    mapM[10 + ybot1][ran1] = 0;
    mapM[9 + ybot1][ran1] = 0;
    mapM[8 + ybot1][ran1] = 0;
    mapM[7 + ybot1][ran1] = 0;
    mapM[6 + ybot1][ran1] = 0;
    mapM[6 + ybot1][ran1 + 1] = 0;
    mapM[6 + ybot1][ran1 - 1] = 0;
    mapM[6 + ybot1][ran1 + 2] = 0;
    mapM[6 + ybot1][ran1 - 2] = 0;
    mapM[5 + ybot1][ran1 + 2] = 0;
    mapM[5 + ybot1][ran1 - 2] = 0;
    mapM[4 + ybot1][ran1 + 2] = 0;
    mapM[4 + ybot1][ran1 - 2] = 0;
    mapM[4 + ybot1][ran1 - 1] = 0;
    mapM[4 + ybot1][ran1 + 1] = 0;
    mapM[4 + ybot1][ran1] = 0;
    ran2 = 4 + ybot1;
  } else if (ran1 == 4) {
    ran1 = random(12, 8 + ybot1);
    while (ran1 % 2 == 1) {
      ran1 = random(12, 8 + ybot1);
    }
    ybot2 = ran1 - 7;
    xbot2 = 1;
    mapM[ran1][9] = -1;
    mapM[ran1][10] = 0;
    mapM[ran1][11] = 0;
    mapM[ran1][12] = 0;
    mapM[ran1][13] = 0;
    mapM[ran1][14] = 0;
    mapM[ran1 + 1][14] = 0;
    mapM[ran1 - 1][14] = 0;
    mapM[ran1 + 2][14] = 0;
    mapM[ran1 - 2][14] = 0;
    mapM[ran1 + 2][15] = 0;
    mapM[ran1 - 2][15] = 0;
    mapM[ran1 + 2][16] = 0;
    mapM[ran1 - 2][16] = 0;
    mapM[ran1 - 1][16] = 0;
    mapM[ran1 + 1][16] = 0;
    mapM[ran1][16] = 0;
    ran2 = 16;
  }
  for (int i = 0; i < (xbot1 * 20); i++) {
    n = random(1, 5);
    if (n == 1) {
      if (mapM[ran1 - 3][ran2] == 1) {
        mapM[ran1 - 2][ran2] = 0;
        mapM[ran1 - 1][ran2] = 0;
        ran1 = ran1 - 2;
      }
    } else if (n == 2) {
      if (mapM[ran1][ran2 + 3] == 1) {
        mapM[ran1][ran2 + 2] = 0;
        mapM[ran1][ran2 + 1] = 0;
        ran2 = ran2 + 2;
      }
    } else if (n == 3) {
      if (mapM[ran1 + 3][ran2] == 1) {
        mapM[ran1 + 2][ran2] = 0;
        mapM[ran1 + 1][ran2] = 0;
        ran1 = ran1 + 2;
      }
    } else if (n == 4) {
      if (mapM[ran1][ran2 - 3] == 1) {
        mapM[ran1][ran2 - 2] = 0;
        mapM[ran1][ran2 - 1] = 0;
        ran2 = ran2 - 2;
      }
    }
  }
  for (int m = 0; m < (xbot1 / 2) + (xbot1 / 4); m++) {
    ran2 = random(10, xbot1 + 10);
    ran1 = random(10, ybot1 + 10);
    while (ran1 % 2 == 1) {
      ran1 = random(10, ybot1 + 10);
    }
    while (ran2 % 2 == 1) {
      ran2 = random(10, xbot1 + 10);
    }
    mapM[ran1][ran2] = 0;
    for (int i = 0; i < (xbot1 * 20); i++) {
      n = random(1, 5);
      if (n == 1) {
        if (mapM[ran1 - 3][ran2] == 1) {
          mapM[ran1 - 2][ran2] = 0;
          mapM[ran1 - 1][ran2] = 0;
          ran1 = ran1 - 2;
        }
      } else if (n == 2) {
        if (mapM[ran1][ran2 + 3] == 1) {
          mapM[ran1][ran2 + 2] = 0;
          mapM[ran1][ran2 + 1] = 0;
          ran2 = ran2 + 2;
        }
      } else if (n == 3) {
        if (mapM[ran1 + 3][ran2] == 1) {
          mapM[ran1 + 2][ran2] = 0;
          mapM[ran1 + 1][ran2] = 0;
          ran1 = ran1 + 2;
        }
      } else if (n == 4) {
        if (mapM[ran1][ran2 - 3] == 1) {
          mapM[ran1][ran2 - 2] = 0;
          mapM[ran1][ran2 - 1] = 0;
          ran2 = ran2 - 2;
        }
      }
    }
  }
  xbot3 = x;
  ybot3 = y;

  tft.fillScreen(0x0000);
  t = 4;
  for (int i = 0; i < 15; i++) {
    sy = i;
    for (int a = 0; a < 19; a++) {
      sx = a;
      xys = mapM[sy + y][sx + x];
      if (xys == 0) {
        tft.fillRect(sx * 16, sy * 16, 16, 16, 0x740E);
      } else if (xys == 1) {
        tft.fillRect(sx * 16, sy * 16, 16, 16, 0x39E7);
      } else if (xys == 2) {
        tft.fillRect(sx * 16, sy * 16, 16, 16, 0x39E7);
      }
    }
  }
  tft.drawBitmap(304, 0, infoL, 16, 47, 0xFFFF);
  tft.setTextSize(2);
  tft.setTextColor(0xFFFF);
  tft.setCursor(307, 48);
  if (s < 10) {
    tft.print(s);
  } else if (s < 100) {
    tft.print(s / 10);
    tft.setCursor(307, 63);
    tft.print(s % 10);
  }
  tft.setCursor(307, 225);
  tft.setTextSize(2);
  tft.setTextColor(0x0000);
  if (SH == true) {
    tft.print("S");
    tft.setTextColor(0xF800);
    tft.setCursor(307, 225);
    tft.print("F");
    timeSH = 50;
  } else {
    tft.print("F");
    tft.setTextColor(0xF800);
    tft.setCursor(307, 225);
    tft.print("S");
    timeSH = 100;
  }
  if (t == 1) {
    tft.drawRGBBitmap(144, 112, m0_l_s, 16, 16);
  } else if (t == 2) {
    tft.drawRGBBitmap(144, 112, m0_r_s, 16, 16);
  } else if (t == 3) {
    tft.drawRGBBitmap(144, 112, m0_u_s, 16, 16);
  } else if (t == 4) {
    tft.drawRGBBitmap(144, 112, m0_d_s, 16, 16);
  }
  e3 = true;
}

void ranLab() {
  if (digitalRead(OK_BUTT) == 1 && millis() - tmrkill1 > 500) {
    SH = !SH;
    tft.setCursor(307, 225);
    tft.setTextSize(2);
    tft.setTextColor(0x0000);
    if (SH == true) {
      tft.print("S");
      tft.setTextColor(0xF800);
      tft.setCursor(307, 225);
      tft.print("F");
      timeSH = 50;
    } else {
      tft.print("F");
      tft.setTextColor(0xF800);
      tft.setCursor(307, 225);
      tft.print("S");
      timeSH = 100;
    }
    tmrkill1 = millis();
  }
}

void zvuk() {
  if (zvukon == true) {
    if (zvukbool == true) {
      if (zv == 1) {
        tone(PEZOPIN, 1500, 10);
        if (millis() - tmrzvuk > 10) {
          zvukbool = false;
          zv = 0;
        }
      } else if (zv == 2) {
        tone(PEZOPIN, 1800, 3);
        if (millis() - tmrzvuk > 3) {
          zvukbool = false;
          zv = 0;
        }
      }
    }
  }
}

void pausa() {
  if (igranach == true) {
    if (digitalRead(HOME_BUTT) == 1) {
      tft.drawRoundRect(54, 60, 212, 120, 10, ST77XX_WHITE);
      tft.drawRoundRect(55, 61, 210, 118, 9, ST77XX_WHITE);
      tft.drawRoundRect(56, 62, 208, 116, 8, ST77XX_BLACK);
      tft.drawRoundRect(57, 63, 206, 114, 7, ST77XX_BLACK);
      tft.drawRoundRect(58, 64, 204, 112, 6, ST77XX_WHITE);
      tft.drawRoundRect(59, 65, 202, 110, 5, ST77XX_WHITE);
      tft.fillRoundRect(60, 66, 200, 108, 4, ST77XX_BLACK);
      tft.drawBitmap(115, 72, paus, 90, 25, ST77XX_WHITE);
      tft.drawBitmap(112, 117, pause1, 25, 25, COLOR2);
      tft.drawBitmap(147, 117, pause2, 25, 25, ST77XX_WHITE);
      tft.drawBitmap(182, 117, pause3, 25, 25, ST77XX_WHITE);
      sx = 0;
      while (digitalRead(HOME_BUTT) == 1) {}
      while (digitalRead(OK_BUTT) == 0) {
        if (digitalRead(RIGHT_BUTT) == 0) {
          sx = sx + 1;
          if (sx > 2) {
            sx = 2;
          } else {
            if (sx == 1) {
              tft.drawBitmap(112, 117, pause1, 25, 25, ST77XX_WHITE);
              tft.drawBitmap(147, 117, pause2, 25, 25, COLOR2);
              tft.drawBitmap(182, 117, pause3, 25, 25, ST77XX_WHITE);
            } else {
              tft.drawBitmap(112, 117, pause1, 25, 25, ST77XX_WHITE);
              tft.drawBitmap(147, 117, pause2, 25, 25, ST77XX_WHITE);
              tft.drawBitmap(182, 117, pause3, 25, 25, COLOR2);
            }
          }
          while (digitalRead(RIGHT_BUTT) == 0) {}
          delay(70);
        } else if (digitalRead(LEFT_BUTT) == 0) {
          sx = sx - 1;
          if (sx < 0) {
            sx = 0;
          } else {
            if (sx == 1) {
              tft.drawBitmap(112, 117, pause1, 25, 25, ST77XX_WHITE);
              tft.drawBitmap(147, 117, pause2, 25, 25, COLOR2);
              tft.drawBitmap(182, 117, pause3, 25, 25, ST77XX_WHITE);
            } else {
              tft.drawBitmap(112, 117, pause1, 25, 25, COLOR2);
              tft.drawBitmap(147, 117, pause2, 25, 25, ST77XX_WHITE);
              tft.drawBitmap(182, 117, pause3, 25, 25, ST77XX_WHITE);
            }
          }
          while (digitalRead(LEFT_BUTT) == 0) {}
          delay(70);
        }
      }
      if (sx == 0) {
        if (NG == 1) {
          tft.fillRect(54, 60, 212, 120, 0x0000);
          for (int i = 0; i < 10; i++) {
            sy = i;
            for (int a = 0; a < 12; a++) {
              sx = a;
              xys = forMapBS[sy][sx];
              if (xys == 0) {} else if (xys == 1) {
                tft.fillRect(sx * 24, sy * 24, 24, 24, 0xC618);
                tft.drawBitmap(sx * 24, sy * 24, b_1, 24, 24, 0x7BEF);
              } else if (xys == 2) {
                tft.fillRect(sx * 24, sy * 24, 24, 24, ST77XX_ORANGE);
                tft.drawBitmap(sx * 24, sy * 24, b_2, 24, 24, 0xC618);
              } else if (xys == 3) {
                tft.fillRect(sx * 24, sy * 24, 24, 24, 0x03EF);
                tft.drawBitmap(sx * 24, sy * 24, b_3, 24, 24, 0xFFFF);
              } else if (xys == 4) {
                tft.fillRect(sx * 24, sy * 24, 24, 24, ST77XX_ORANGE);
                tft.drawBitmap(sx * 24, sy * 24, b_2, 24, 24, 0xC618);
                tft.drawBitmap(sx * 24, sy * 24, r_1, 24, 24, ST77XX_BLACK);
              } else if (xys == 5) {
                tft.fillRect(sx * 24, sy * 24, 24, 24, ST77XX_ORANGE);
                tft.drawBitmap(sx * 24, sy * 24, b_2, 24, 24, 0xC618);
                tft.drawBitmap(sx * 24, sy * 24, r_2, 24, 24, ST77XX_BLACK);
              } else if (xys == 6) {
                tft.fillRect(sx * 24, sy * 24, 24, 24, ST77XX_ORANGE);
                tft.drawBitmap(sx * 24, sy * 24, b_2, 24, 24, 0xC618);
                tft.drawBitmap(sx * 24, sy * 24, r_3, 24, 24, ST77XX_BLACK);
              }
            }
          }
        } else if (NG == 2) {
          tft.fillScreen(0x0000);
          for (int i = 0; i < 154; i = i + 2) {
            tft.fillRect(157, i * 16, 6, 16, 0x31A6);
          }
          tft.drawBitmap(10, y, roketMap, 10, 40, 0xFF80);
          tft.setTextSize(7);
          tft.setTextColor(0x31A6);
          if (rotat == 3) {
            tft.setRotation(4);
          } else {
            tft.setRotation(2);
          }
          tft.setCursor(91, 221);
          tft.print(String(int(p / 100)) + String(int((p / 10) % 10)) + String(int((p % 100) % 10)));
          if (rotat == 3) {
            tft.setRotation(2);
          } else {
            tft.setRotation(4);
          }
          tft.setCursor(91, 221);
          tft.print(String(int(q / 100)) + String(int((q / 10) % 10)) + String(int((q % 100) % 10)));
          tft.setRotation(rotat);
        } else if (NG == 3) {
          tft.fillRoundRect(54, 60, 212, 120, 10, 0xFF16);
          tft.drawRGBBitmap(xp4, yp4, plot, 38, 18);
          tft.drawRGBBitmap(xp2, yp2, plot, 38, 18);
          tft.drawRGBBitmap(xp3, yp3, plot, 38, 18);
          tft.drawRGBBitmap(xp1, yp1, plot, 38, 18);
        } else if (NG == 4) {
          tft.fillScreen(ST77XX_CYAN);
        } else if (NG == 5) {
          tft.fillRoundRect(54, 60, 212, 120, 10, 0x0000);
          tft.drawBitmap(xbot1, ybot1, telo, 10, 10, ST77XX_RED);
          for (int i = 0; i < 24; i++) {
            sy = i;
            for (int a = 0; a < 32; a++) {
              sx = a;
              if (fm == 1) {
                xys = map2S[sy][sx];
              } else if (fm == 2) {
                xys = map3S[sy][sx];
              } else if (fm == 0) {
                spase = 768;
              }
              if (xys == 0) {
                spase = spase + 1;
              } else if (xys == 1) {
                tft.fillRect(sx * 10, sy * 10, 10, 10, 0x049F);
              }
            }
          }
        } else if (NG == 6) {
          tft.fillRoundRect(54, 60, 212, 120, 10, 0x0000);
          for (int i = 0; i < 8; i++) {
            sy = i;
            for (int a = 0; a < 11; a++) {
              sx = a;
              if (fm == 0) {
                xys = map1PM[sy][sx];
              } else if (fm == 1) {
                xys = map2PM[sy][sx];
              } else {
                xys = map3PM[sy][sx];
              }
              if (xys == 0) {
                spase = spase + 1;
              } else if (xys == 1) {
                tft.drawBitmap(sx * 30, sy * 30, stena1, 30, 30, 0x915C);
              } else if (xys == 2) {
                tft.drawBitmap(sx * 30, sy * 30, stena2, 30, 30, 0x915C);
              } else if (xys == 3) {
                tft.drawBitmap(sx * 30, sy * 30, stena3, 30, 30, 0x915C);
              } else if (xys == 4) {
                tft.drawBitmap(sx * 30, sy * 30, stena4, 30, 30, 0x915C);
              } else if (xys == 5) {
                tft.drawBitmap(sx * 30, sy * 30, stena5, 30, 30, 0x915C);
              } else if (xys == 6) {
                tft.drawBitmap(sx * 30, sy * 30, stena6, 30, 30, 0x915C);
              } else if (xys == 7) {
                tft.drawBitmap(sx * 30, sy * 30, stena7, 30, 30, 0x915C);
              } else if (xys == 8) {
                tft.drawBitmap(sx * 30, sy * 30, stena8, 30, 30, 0x915C);
              } else if (xys == 9) {
                tft.drawBitmap(sx * 30, sy * 30, stena9, 30, 30, 0x915C);
              } else if (xys == 10) {
                tft.drawBitmap(sx * 30, sy * 30, stena10, 30, 30, 0x915C);
              } else if (xys == 11) {
                tft.drawBitmap(sx * 30, sy * 30, stena11, 30, 30, 0x915C);
              } else if (xys == 12) {
                tft.drawBitmap(sx * 30, sy * 30, stena12, 30, 30, 0x915C);
              } else if (xys == 13) {
                tft.drawBitmap(sx * 30, sy * 30, stena13, 30, 30, 0x915C);
              } else if (xys == 14) {
                tft.drawBitmap(sx * 30, sy * 30, stena14, 30, 30, 0x915C);
              } else if (xys == 15) {
                tft.drawBitmap(sx * 30, sy * 30, stena15, 30, 30, 0x915C);
              } else if (xys == 16) {
                tft.drawBitmap(sx * 30, sy * 30, stena16, 30, 30, 0x915C);
              }
            }
          }
          tft.drawBitmap(x, y, pac1n3, 30, 30, ST77XX_YELLOW);
          tft.drawBitmap(xv, yv, vishnya, 30, 30, ST77XX_RED);
          tft.drawBitmap(xt, yt, tabletka, 30, 30, 0xFE19);
          tft.drawBitmap(xc, yc, coin, 30, 30, ST77XX_YELLOW);
          tft.drawBitmap(xh, yh, plshp, 30, 30, ST77XX_RED);
        } else if (NG == 7) {
          tft.fillRoundRect(54, 60, 212, 120, 10, 0x07FF);
          for (int i = 0; i < 24; i++) {
            sy = i;
            for (int a = 0; a < 32; a++) {
              sx = a;
              xys = forMapSB[sy][sx];
              if (xys == 0) {
              } else if (xys == 1) {
                tft.drawBitmap(sx * 10, sy * 10, sbs21, 10, 10, 0xC618);
                tft.drawBitmap(sx * 10, sy * 10, sbs22, 10, 10, 0x7BEF);
              } else if (xys == 2) {
                tft.drawBitmap(sx * 10, sy * 10, sbs11, 10, 10, ST77XX_GREEN);
                tft.drawBitmap(sx * 10, sy * 10, sbs12, 10, 10, 0x03E0);
              } else if (xys == 3) {
                tft.drawBitmap(sx * 10, sy * 10, sbs41, 10, 10, 0x001F);
                tft.drawBitmap(sx * 10, sy * 10, sbs42, 10, 10, 0x000F);
              } else if (xys == 4) {
                tft.drawBitmap(sx * 10, sy * 10, sbs31, 10, 10, ST77XX_ORANGE);
                tft.drawBitmap(sx * 10, sy * 10, sbs32, 10, 10, 0x7800);
              } else if (xys == 6) {
                tft.drawBitmap(sx * 10, sy * 10, sbs31, 10, 10, ST77XX_RED);
                tft.drawBitmap(sx * 10, sy * 10, sbs32, 10, 10, 0xA800);
              } else if (xys == 7) {
                tft.drawBitmap(sx * 10, sy * 10, sbs31, 10, 10, 0xFC30);
                tft.drawBitmap(sx * 10, sy * 10, sbs32, 10, 10, 0xCB4D);
              } else if (xys == 8) {
                tft.drawBitmap(sx * 10, sy * 10, sbs11, 10, 10, 0x0000);
                tft.drawBitmap(sx * 10, sy * 10, sbs12, 10, 10, 0x39C7);
              } else if (xys == 9) {
                tft.drawBitmap(sx * 10, sy * 10, sbs31, 10, 10, 0x4020);
                tft.drawBitmap(sx * 10, sy * 10, sbs32, 10, 10, 0x3020);
              } else if (xys == 10) {
                tft.drawBitmap(sx * 10, sy * 10, sbs31, 10, 10, 0x9A42);
                tft.drawBitmap(sx * 10, sy * 10, sbs32, 10, 10, 0x79C1);
              } else if (xys == 11) {
                tft.drawBitmap(sx * 10, sy * 10, sbs31, 10, 10, 0xCB63);
                tft.drawBitmap(sx * 10, sy * 10, sbs32, 10, 10, 0xB303);
              } else if (xys == 12) {
                tft.drawBitmap(sx * 10, sy * 10, sbs31, 10, 10, 0xE465);
                tft.drawBitmap(sx * 10, sy * 10, sbs32, 10, 10, 0xCC05);
              }
            }
          }
        } else if (NG == 8) {
          tft.fillRoundRect(54, 60, 212, 120, 10, ST77XX_CYAN);
          tft.fillRect(0, 200, 320, 40, 0x03E0);
        } else if (NG == 9) {
          tft.fillRoundRect(54, 60, 212, 120, 10, 0x0000);
          tft.drawLine(121, 0, 121, 240, ST77XX_GREEN);
          xh = 0;
          yh = 0;
          while (yh < 20) {
            if (tetfon[yh][xh] == 0) {
              tft.fillRect(xh * 12, yh * 12, 12, 12, 0x0000);
            } else if (tetfon[yh][xh] == 1) {
              tft.drawRGBBitmap(xh * 12, yh * 12, partR, 12, 12);
            } else if (tetfon[yh][xh] == 2) {
              tft.drawRGBBitmap(xh * 12, yh * 12, partY, 12, 12);
            } else if (tetfon[yh][xh] == 3) {
              tft.drawRGBBitmap(xh * 12, yh * 12, partG, 12, 12);
            } else if (tetfon[yh][xh] == 4) {
              tft.drawRGBBitmap(xh * 12, yh * 12, partP, 12, 12);
            } else if (tetfon[yh][xh] == 5) {
              tft.drawRGBBitmap(xh * 12, yh * 12, partB, 12, 12);
            }
            xh = xh + 1;
            if (xh > 9) {
              xh = 0;
              yh = yh + 1;
            }
          }
          if (n < 1) {
            n = 4;
          }
        } else if (NG == 10) {
          for (int i = 0; i < 15; i++) {
            sy = i;
            for (int a = 0; a < 19; a++) {
              sx = a;
              xys = mapM[sy + y][sx + x];
              if (xys == 0) {
                tft.fillRect(sx * 16, sy * 16, 16, 16, 0x740E);
              } else if (xys == 1) {
                tft.fillRect(sx * 16, sy * 16, 16, 16, 0x39E7);
              } else if (xys == 2) {
                tft.fillRect(sx * 16, sy * 16, 16, 16, 0x39E7);
              }
            }
          }
          if (t == 1) {
            tft.drawRGBBitmap(144, 112, m0_l_s, 16, 16);
          } else if (t == 2) {
            tft.drawRGBBitmap(144, 112, m0_r_s, 16, 16);
          } else if (t == 3) {
            tft.drawRGBBitmap(144, 112, m0_u_s, 16, 16);
          } else if (t == 4) {
            tft.drawRGBBitmap(144, 112, m0_d_s, 16, 16);
          }
        }
      } else if (sx == 1) {
        if (NG == 1) {
          s = 0;
          se = 0;
          sd = 0;
          tft.setTextSize(1);
          nachalo();
        } else if (NG == 2) {
          y = 0;
          e2 = true;
          n = 0;
          o = 0;
          q = 0;
          p = 0;
          xbot1 = 152;
          ybot1 = 112;
          kill1 = true;
          xbot2 = 152;
          ybot2 = 112;
          kill2 = true;
          xbot3 = 152;
          ybot3 = 112;
          kill3 = true;
          tft.fillScreen(0x0000);
          for (int i = 0; i < 154; i = i + 2) {
            tft.fillRect(157, i * 16, 6, 16, 0x31A6);
          }
          tft.drawBitmap(10, y, roketMap, 10, 40, 0xFF80);
          tft.setTextSize(7);
          tft.setTextColor(0x31A6);
          if (rotat == 3) {
            tft.setRotation(4);
          } else {
            tft.setRotation(2);
          }
          tft.setCursor(91, 221);
          tft.print(String(int(p / 100)) + String(int((p / 10) % 10)) + String(int((p % 100) % 10)));
          if (rotat == 3) {
            tft.setRotation(2);
          } else {
            tft.setRotation(4);
          }
          tft.setCursor(91, 221);
          tft.print(String(int(q / 100)) + String(int((q / 10) % 10)) + String(int((q % 100) % 10)));
          tft.setRotation(rotat);
        } else if (NG == 3) {
          tft.fillScreen(0xFF16);
          s = 0;
          y = 160;
          x = 140;
          n = 1;
          prig = true;
          g = 10;
          pad = false;
          spawnplot();
        } else if (NG == 4) {
          tft.fillScreen(ST77XX_CYAN);
          tft.setTextSize(1);
          xbot1 = 320;
          xbot2 = 520;
          g = 0;
          y = 120;
          ran1 = random(1, 8) * 22;
          ran2 = random(1, 8) * 22;
        } else if (NG == 5) {
          tft.fillScreen(0x0000);
          mapSnake();
          dlinatela = 4;
          n = 1;
          s = 0;
          edapr1 = false;
          igranach = true;
          x = 0;
          while (x < 200) {
            xs[x] = 0;
            x = x + 1;
          }
          y = 0;
          while (y < 200) {
            ys[y] = 0;
            y = y + 1;
          }
          x = 100;
          y = 100;
        } else if (NG == 6) {
          n = 1;
          nbot1 = 1;
          nbot2 = 1;
          hp = 3;
          tab = false;
          s = 0;
          nachpac();
        } else if (NG == 7) {
          y = 0;
          tft.setTextSize(1);
          nachalokart();
          kill0 = true;
          s = 0;
          bollfet = false;
          bolllight = false;
          rokmin = false;
          armor = false;
          rokbig = false;
          many = false;
        } else if (NG == 8) {
          tft.fillScreen(ST77XX_CYAN);
          tft.fillRect(0, 200, 320, 40, 0x03E0);
          igranach = true;
          y = 200;
          xbot1 = 160;
          xbot2 = 320;
          kill1 = false;
          kill2 = false;
          se = 2;
          sd = 3;
          o = 320;
          s = 0;
          sst = 1;
          prig = false;
        } else if (NG == 9) {
          tft.fillScreen(0x0000);
          tft.drawLine(121, 0, 121, 240, ST77XX_GREEN);
          x = 0;
          y = 0;
          while (y < 20) {
            while (x < 10) {
              tetfon[y][x] = 0;
              x = x + 1;
            }
            x = 0;
            y = y + 1;
          }
          kill0 = true;
          n = 0;
          x = 0;
          y = 0;
          i = 0;
          s = -1;
          e2 = true;
          igranach = true;
        } else if (NG == 10) {
          e3 = false;
          s = -1;
        }
      } else if (sx == 2) {
        zvukbool = true;
        zv = 2;
        tmrzvuk = millis();
        zvuk();
        igranach = false;
        e3 = false;
      }
    }
  }
}
