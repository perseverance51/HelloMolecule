#define GUILITE_ON  //Do not define this macro once more!!!
#include "GuiLite.h"//GUI库
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <Wire.h>
#include "ui_animation.h"
#include <Arduino.h>



/*********时钟相关的参数**********/
//#define CLOCK_POS_X 64//时钟中心点x/y坐标//像素点为单位
//#define CLOCK_POS_Y 32
//#define CLOCK_RADIUS 31//时钟半径//像素点为单位
//#define CLOCK_ANIMATION_MAX_RADIUS 31//放动画时时钟最大半径
//#define CLOCK_SACLE_RGB GL_RGB(0, 175, 235)//时钟外框颜色
//#define HOUR_HAND_LEN 28//时针长度
//#define HOUR_HAND_RGB GL_RGB(255, 255, 255)//时针颜色
//#define HOUR_HAND_ANIMATION_ANGLE 2//放动画时时针起始及终值角度。弧度为单位
//#define MINUTE_HAND_LEN 27//与时针参数描述相同
//#define MINUTE_HAND_RGB GL_RGB(255, 255, 255)
//#define MINUTE_HAND_ANIMATION_ANGLE 4
//#define SECOND_HAND_LEN 30
//#define SECOND_HAND_RGB GL_RGB(237, 125, 124)
//#define SECOND_HAND_ANIMATION_ANGLE 6
/*************************/

/*思维点和连线相关的参数*/
#define MOLECULE_RADIUS 2//思维点半径
#define MOLECULE_TOTAL 9//运动思维点的总数
#define BOND_TOTAL 36 //BOND_TOTAL = 1 + 2 + ... + (MOLECULE_TOTAL - 1)//思维点连线的总数，最多是等差数列求和7-28
const int VELOCITY = 2;//思维点运动速度
const int BOND_MAX_DISTANCE = 36;//思维点之间连线的阈值
/***************/

/***星空穿越特效相关参数*/
//#define FACTOR_SIZE 2//星空粒子大小//方形时为方形的边长//圆形时为圆形半径
//#define FACTOR_SHAPE 1//星空粒子形状//0 方形//1 圆形
/*****************/

const int UI_WIDTH = 128;
const int UI_HEIGHT = 64;
const int FRAME_COUNT = 16;//播放时钟动画的帧数

/************这两个不能动*****/
static c_surface* s_surface;
static c_display* s_display;
/*****************/

unsigned short int randomColor();//产生随机颜色函数声明
void screen_fill_circle(int32_t x0, int32_t y0, int32_t r,uint16_t color);//绘制填充圆形函数声明


/**星空穿越特效类*/
/**
class c_star {
public:
  c_star(){
    initialize();
  }
  void initialize() {
    m_x = m_start_x = rand() % UI_WIDTH;
    m_y = m_start_y = rand() % UI_HEIGHT;
    m_size = FACTOR_SIZE;
    m_x_factor = UI_WIDTH;
    m_y_factor = UI_HEIGHT;
    m_size_factor = 1;
  }

  void move() {
    s_surface->fill_rect(m_x, m_y, m_x + m_size - 1, m_y + m_size - 1, 0, Z_ORDER_LEVEL_0);//clear star footprint

    m_x_factor -= 6;
    m_y_factor -= 6;
    m_size += m_size / 20;
    if (m_x_factor < 1 || m_y_factor < 1)
    {
      return initialize();
    }
    if (m_start_x > (UI_WIDTH / 2) && m_start_y > (UI_HEIGHT / 2))
    {
      m_x = (UI_WIDTH / 2) + (UI_WIDTH * (m_start_x - (UI_WIDTH / 2)) / m_x_factor);
      m_y = (UI_HEIGHT / 2) + (UI_HEIGHT * (m_start_y - (UI_HEIGHT / 2)) / m_y_factor);
    }
    else if (m_start_x <= (UI_WIDTH / 2) && m_start_y > (UI_HEIGHT / 2))
    {
      m_x = (UI_WIDTH / 2) - (UI_WIDTH * ((UI_WIDTH / 2) - m_start_x) / m_x_factor);
      m_y = (UI_HEIGHT / 2) + (UI_HEIGHT * (m_start_y - (UI_HEIGHT / 2)) / m_y_factor);
    }
    else if (m_start_x > (UI_WIDTH / 2) && m_start_y <= (UI_HEIGHT / 2))
    {
      m_x = (UI_WIDTH / 2) + (UI_WIDTH * (m_start_x - (UI_WIDTH / 2)) / m_x_factor);
      m_y = (UI_HEIGHT / 2) - (UI_HEIGHT * ((UI_HEIGHT / 2) - m_start_y) / m_y_factor);
    }
    else if (m_start_x <= (UI_WIDTH / 2) && m_start_y <= (UI_HEIGHT / 2))
    {
      m_x = (UI_WIDTH / 2) - (UI_WIDTH * ((UI_WIDTH / 2) - m_start_x) / m_x_factor);
      m_y = (UI_HEIGHT / 2) - (UI_HEIGHT * ((UI_HEIGHT / 2) - m_start_y) / m_y_factor);
    }

    if (m_x < 0 || (m_x + m_size - 1) >= UI_WIDTH ||
      m_y < 0 || (m_y + m_size - 1) >= UI_HEIGHT)
    {
      return initialize();
    }
    if(FACTOR_SHAPE == 0)
    {
      s_surface->fill_rect(m_x, m_y, m_x + m_size - 1, m_y + m_size - 1, randomColor(), Z_ORDER_LEVEL_0);//draw star
  
    }
    else if(FACTOR_SHAPE == 1)
    {
      screen_fill_circle(m_x, m_y, m_size, randomColor());

    }
  }
  int m_start_x, m_start_y;
  float m_x, m_y, m_x_factor, m_y_factor, m_size_factor, m_size;
};
******/
/*****************/








/**********思维点类*******/
class Molecule
{
public:
  Molecule()
  {
    x = rand() % UI_WIDTH;
    y = rand() % UI_HEIGHT;
    vx = VELOCITY * ((0 == rand() % 2) ? -1 : 1);
    vy = VELOCITY * ((0 == rand() % 2) ? -1 : 1);
    //color = GL_RGB(rand() % 5 * 32 + 127, rand() % 5 * 32 + 127, rand() % 5 * 32 + 127);
    color = randomColor();
  }

  void move()
  {
    //draw(0);
    if (x <= 0 || x >= UI_WIDTH)
    {
      vx = (0 - vx);
      color = randomColor();
    }
    if (y < 0 || y >= UI_HEIGHT)
    {
      vy = (0 - vy);
      color = randomColor();
    }
    x += vx;
    y += vy;
    //draw(color);
    
    screen_fill_circle(x, y, MOLECULE_RADIUS, color);
  }

  void draw(unsigned int color)
  {
    s_surface->draw_pixel(x - 2, y - 2, color, Z_ORDER_LEVEL_0);
    s_surface->draw_pixel(x - 1, y - 2, color, Z_ORDER_LEVEL_0);
    s_surface->draw_pixel(x, y - 2, color, Z_ORDER_LEVEL_0);
    s_surface->draw_pixel(x + 1, y - 2, color, Z_ORDER_LEVEL_0);
    s_surface->draw_pixel(x + 2, y - 2, color, Z_ORDER_LEVEL_0);

    s_surface->draw_pixel(x - 3, y - 1, color, Z_ORDER_LEVEL_0);
    s_surface->draw_pixel(x + 3, y - 1, color, Z_ORDER_LEVEL_0);

    s_surface->draw_pixel(x - 3, y, color, Z_ORDER_LEVEL_0);
    s_surface->draw_pixel(x + 3, y, color, Z_ORDER_LEVEL_0);

    s_surface->draw_pixel(x - 3, y + 1, color, Z_ORDER_LEVEL_0);
    s_surface->draw_pixel(x + 3, y + 1, color, Z_ORDER_LEVEL_0);

    s_surface->draw_pixel(x - 2, y + 2, color, Z_ORDER_LEVEL_0);
    s_surface->draw_pixel(x - 1, y + 2, color, Z_ORDER_LEVEL_0);
    s_surface->draw_pixel(x, y + 2, color, Z_ORDER_LEVEL_0);
    s_surface->draw_pixel(x + 1, y + 2, color, Z_ORDER_LEVEL_0);
    s_surface->draw_pixel(x + 2, y + 2, color, Z_ORDER_LEVEL_0);
  }

  float x, y, vx, vy;
  unsigned int color;
};
/*****************/

/*******思维点的连线类*********/
class Bond
{
public:
  Bond()
  {
    m0 = m1 = 0;
    x0 = y0 = x1 = y1 = 0;
  }

  static void createBond(Molecule* m0, Molecule* m1)
  {
    float distance = sqrtf((m0->x - m1->x) * (m0->x - m1->x) + (m0->y - m1->y) * (m0->y - m1->y));
    static unsigned int _color = randomColor();

    int index = -1;
    for (int i = 0; i < BOND_TOTAL; i++)
    {
      if ((bonds[i].m0 == m0 && bonds[i].m1 == m1) || (bonds[i].m0 == m1 && bonds[i].m1 == m0))
      {
        index = i;
        break;
      }
    }

    if (index >= 0)
    {//has been registered
      if (distance > BOND_MAX_DISTANCE)
      {//unregister
        s_surface->draw_line(bonds[index].x0, bonds[index].y0, bonds[index].x1, bonds[index].y1, GL_RGB(0, 0, 0), Z_ORDER_LEVEL_0);
        bonds[index].m0 = bonds[index].m1 = 0;
        _color = randomColor();
        return;
      }
      else
      {//update bond & draw
        s_surface->draw_line(bonds[index].x0, bonds[index].y0, bonds[index].x1, bonds[index].y1, GL_RGB(0, 0, 0), Z_ORDER_LEVEL_0);
        s_surface->draw_line(m0->x, m0->y, m1->x, m1->y, _color, Z_ORDER_LEVEL_0);
        bonds[index].x0 = m0->x;
        bonds[index].y0 = m0->y;
        bonds[index].x1 = m1->x;
        bonds[index].y1 = m1->y;
      }
      return;
    }

    if (distance > BOND_MAX_DISTANCE)
    {
      return;
    }
    //register new bond
    index = -1;
    for (int i = 0; i < BOND_TOTAL; i++)
    {
      if (bonds[i].m0 == 0 && bonds[i].m1 == 0)
      {
        index = i;
        break;
      }
    }
    if (index < 0)
    {//bonds full
      ASSERT(false);
      return;
    }

    //register
    bonds[index].m0 = m0;
    bonds[index].m1 = m1;
    bonds[index].x0 = m0->x;
    bonds[index].y0 = m0->y;
    bonds[index].x1 = m1->x;
    bonds[index].y1 = m1->y;
    s_surface->draw_line(m0->x, m0->y, m1->x, m1->y, color, Z_ORDER_LEVEL_0);
  }

  Molecule *m0, *m1;
  float x0, y0, x1, y1;
  static const unsigned int color = GL_RGB(0, 162, 232);
  static Bond bonds[BOND_TOTAL];
};
/*****************/

//c_star stars[100];//星空穿越特效
Molecule molecules[MOLECULE_TOTAL];//思维点
Bond Bond::bonds[BOND_TOTAL];//思维点的连线

    
void create_ui(void* phy_fb, int screen_width, int screen_height, int color_bytes, struct EXTERNAL_GFX_OP* gfx_op)//ui的初始化函数
{
  /**********这部分使用时几乎不需要修改*******/
  if (phy_fb)
  {
    static c_surface surface(UI_WIDTH, UI_HEIGHT, color_bytes, Z_ORDER_LEVEL_0);
    static c_display display(phy_fb, screen_width, screen_height, &surface);
    s_surface = &surface;
    s_display = &display;
  }
  else
  {//for MCU without framebuffer
    static c_surface_no_fb surface_no_fb(UI_WIDTH, UI_HEIGHT, color_bytes, gfx_op, Z_ORDER_LEVEL_0);
    static c_display display(phy_fb, screen_width, screen_height, &surface_no_fb);
    s_surface = &surface_no_fb;
    s_display = &display;
  }

  //background
  s_surface->fill_rect(0, 0, UI_WIDTH, UI_HEIGHT, 0, Z_ORDER_LEVEL_0);
  /*****************/

  /***********这里是自定义需要的初始化******/
//  time_clock.set_hands(CLOCK_RADIUS, HOUR_HAND_LEN, 2, HOUR_HAND_RGB, MINUTE_HAND_LEN, 4, MINUTE_HAND_RGB, SECOND_HAND_LEN, 6, SECOND_HAND_RGB);
  /*****************/
}

void ui_update(void)
{

    /*思维点及连线使用范例 */
    for (int i = 0; i < MOLECULE_TOTAL; i++)
    {
      molecules[i].move();
    }
    
    for (int i = 0; i < MOLECULE_TOTAL; i++)
    {
      for (int sub_i = i + 1; sub_i < MOLECULE_TOTAL; sub_i++)
      {
        Bond::createBond(&molecules[i], &molecules[sub_i]);
      }
    }
  
    delay(50);//改变这个延时函数就能改变特效播放的快慢
}

unsigned short int randomColor()
{
    unsigned char red,green,blue;
    red = rand();
    //__ASM("NOP");
    green = rand();
    //__ASM("NOP");
    blue = rand();
    return (red << 11 | green << 5 | blue);
}

/*********填充圆用到的工具函数********/
void screen_draw_fastVLine(int32_t x, int32_t y, int32_t length,uint16_t color)  
{
  // Bounds check
    int32_t y0=y;
    do
    {
        s_surface->draw_pixel(x, y,color, Z_ORDER_LEVEL_0);   // 逐点显示，描出垂直线
        y++;
    }
    while(y0+length>=y);
}

void screen_fillCircle_helper(int32_t x0, int32_t y0, int32_t r, uint8_t corner, int32_t delta,uint16_t color)  {

  int32_t f     = 1 - r;
  int32_t ddF_x = 1;
  int32_t ddF_y = -2 * r;
  int32_t x     = 0;
  int32_t y     = r;

  while(x<y)  {
    if(f >= 0)  {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    if(corner & 0x1)  {
      screen_draw_fastVLine(x0+x, y0-y, 2*y+delta, color);
      screen_draw_fastVLine(x0+y, y0-x, 2*x+delta, color);
    }
    if(corner & 0x2)  {
      screen_draw_fastVLine(x0-x, y0-y, 2*y+delta, color);
      screen_draw_fastVLine(x0-y, y0-x, 2*x+delta, color);
    }
  }
}
/*****************/

void screen_fill_circle(int32_t x0, int32_t y0, int32_t r,uint16_t color) 
{
    screen_draw_fastVLine(x0, y0-r, 2*r, color);
    screen_fillCircle_helper(x0, y0, r, 3, 0, color);
}
