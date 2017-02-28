#include <Adafruit_TFTLCD.h>
#include <TouchScreen.h>

enum {
  DirUp,
  DirDown,
  DirLeft,
  DirRight
};

class GameBtns {
  public:
    unsigned short m_uRadius;
    unsigned short m_x, m_y;
    Adafruit_TFTLCD& m_tft;
    TouchScreen& m_ts;
    GameBtns(Adafruit_TFTLCD& tft, TouchScreen& ts, unsigned short x, unsigned short y): m_tft(tft), m_ts(ts) {
      m_x = x;
      m_y = y;
    };
    GameBtns(Adafruit_TFTLCD& tft, TouchScreen& ts): m_tft(tft), m_ts(ts) {
      m_x = 120;
      //m_y = 60;
      m_y = 275;
    };
    void draw(void);
    int getDir(int direction);
};
