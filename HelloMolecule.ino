#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ESP8266HTTPClient.h>
#include <time.h>
#include <sys/time.h>
#include <coredecls.h>
#include <WiFiClient.h>
//#include "SH1106Wire.h" //1.3寸用这个
#include "SSD1306Wire.h"    //0.96寸用这个
#include "OLEDDisplayUi.h"
//#include "font.c"
#include <stdint.h>
#include "ui_animation.h"

#define SCREEN_HEIGHT 64
#define SCREEN_WIDTH 128

#if defined(ESP8266)    //OLED引脚定义
const int SDA_PIN = D2; //对应nodemcu接D5或者D1，，wemosD1mini的D2
const int SDC_PIN = D1; //对应nodemcu接D6或者D2，，wemosD1mini的D5
#else
const int SDA_PIN = D1; //对应nodemcu接D5或者D1
const int SCL = D2;     //对应nodemcu接D6或者D2
#endif

const int I2C_DISPLAY_ADDRESS = 0x3c;                      //I2c地址默认

//SH1106Wire display(I2C_DISPLAY_ADDRESS, SDA_PIN, SDC_PIN); // 1.3寸用这个
SSD1306Wire     display(I2C_DISPLAY_ADDRESS, SDA_PIN, SDC_PIN);   // 0.96寸用这个

const char *ssid     = "CMCC-DyVv";
const char *password = "pba5ayzk";

long int Time;
String Time_api = "http://api.m.taobao.com/rest/api3.do?api=mtop.common.getTimestamp";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "ntp2.aliyun.com", 60 * 60 * 8, 30 * 60 * 1000);


#define TZ              8      // 中国时区为8
#define DST_MN          0      // 默认为0

#define TZ_MN           ((TZ)*60)   //时间换算
#define TZ_SEC          ((TZ)*3600)
#define DST_SEC         ((DST_MN)*60)


time_t now; //实例化时间

//uint8_t* screen_photo = NULL;

//创建一个对象ui,并关联屏幕显示
 OLEDDisplayUi ui   ( &display );


 /*这两个函数都只是对图像进行操作，不是对屏幕*/
//void screen_clear(uint16_t color);//清屏函数声明
//void screen_draw_pixel(int32_t x, int32_t y, uint16_t color);//描点函数声明
/**********************************/

void gfx_draw_pixel(int x, int y, unsigned int rgb)//指定GUI库的描点函数
{
  //  screen_draw_pixel(x, y, rgb);
display.setPixel(x, y);

}

struct EXTERNAL_GFX_OP
{
  void (*draw_pixel)(int x, int y, unsigned int rgb);
  void (*fill_rect)(int x0, int y0, int x1, int y1, unsigned int rgb);
} my_gfx_op;

void UpdateTime_RTC(void);

 void setup(){
  Serial.begin(115200);
  WiFi.begin(ssid, password);
    while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }
    timeClient.begin(); 
Serial.printf("WiFi name:%s\n IP Address: %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());    
  randomSeed(analogRead(0));
  display.init();
  display.clear();
  display.display();
  //    display.flipScreenVertically(); //屏幕翻转
  // display.mirrorScreen();//使用分光棱镜显示需要用到此函数
  display.setContrast(240); //屏幕亮度
 // delay(1000);
 ui.setTargetFPS(80);                   //刷新频率
  ui.disableAllIndicators();             //不显示页码小点。
 ui.enableAutoTransition();
  ui.setFrameAnimation(SLIDE_LEFT);           //切屏方向
  // ui.disableAutoTransition();
 ui.enableAutoTransition();
  ui.setAutoTransitionForwards();//设置自动过渡方向,
  
 // ui.setFrames(frames, numberOfFrames);       // 设置框架和显示屏幕内容数
  ui.setTimePerFrame(5000);                   //设置切换时间
  ui.setTimePerTransition(500);//设置转场大约所需要时间
  // ui.setOverlays(overlays, numberOfOverlays); //设置覆盖的画面数  
  ui.init();// UI负责初始化显示
//screen_photo = (uint8_t*)malloc(SCREEN_HEIGHT * SCREEN_WIDTH);//动态分配一块屏幕分辨率大小的空间
 //Link your LCD driver & start UI:
    my_gfx_op.draw_pixel = gfx_draw_pixel;//指定GuiLite库的描点函数
    my_gfx_op.fill_rect = NULL;//gfx_fill_rect;
    create_ui(NULL, SCREEN_HEIGHT, SCREEN_WIDTH, 2, &my_gfx_op);//ui初始化
    
//configTime(TZ_SEC, DST_SEC, "ntp.ntsc.ac.cn", "ntp5.aliyun.com"); //ntp获取时间，你也可用其他"pool.ntp.org","0.cn.pool.ntp.org","1.cn.pool.ntp.org","ntp1.aliyun.com"


}


void loop()
{

display.clear();
 ui_update();//ui更新//最终所有的特效调用都在这里面
 display.display();
// display.drawXbm(0, 0, 128, 64, screen_photo);
//delay(6000);
//  Time = getTimestamp(Time_api); //nowapi时间API
//  Serial.println(Time);
}

long int getTimestamp(String url)
{
  String Time = "";
  long long Time1;
  WiFiClient client;
  HTTPClient http;
  http.begin(client,url);

  // start connection and send HTTP headerFFF
  int httpCode = http.GET();

  // httpCode will be negative on error
  if (httpCode > 0)
  {
    // file found at server
    if (httpCode == HTTP_CODE_OK)
    {
      String payload = http.getString();
      int pos = (payload.indexOf("data"));
      Time = payload.substring(pos + 12, payload.length() - 6);
      Serial.println(payload.substring(pos + 12, payload.length() - 6));
    }
  }
  else
  {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
  
  return atoll(Time.c_str());
}
