#include "GameBtns.h"

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
#define YP A2  // must be an analog pin, use "An" notation!
#define XM A3  // must be an analog pin, use "An" notation!
#define YM 8   // can be a digital pin
#define XP 9   // can be a digital pin

// Assign human-readable names to some common 16-bit color values:
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

// This is calibration data for the raw touch data to the screen coordinates
#define TS_MINX 0
#define TS_MINY 0
#define TS_MAXX 920
#define TS_MAXY 935

void GameBtns::draw(void) {
  m_tft.fillCircle(m_x + 25, m_y, 10, YELLOW);
  m_tft.fillCircle(m_x - 25, m_y, 10, YELLOW);
  m_tft.fillCircle(m_x, m_y + 25, 10, BLUE);
  m_tft.fillCircle(m_x, m_y - 25, 10, BLUE);
}

#define MINPRESSURE 10
#define MAXPRESSURE 1000
#define TS_MINX 127
#define TS_MINY 112
#define TS_MAXX 920
#define TS_MAXY 940

int GameBtns::getDir(int direction)
{
  uint16_t x, y;
  uint8_t z;
  // See if there's any  touch data for us
  TSPoint p = m_ts.getPoint();
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  if (p.z > MINPRESSURE) {
    uint16_t x, y;
    y = p.x;
    x = p.y;
    /*
    Serial.print("->(");
    Serial.print(x); Serial.print(", ");
    Serial.print(y); Serial.print(", ");
    Serial.print(z);
    Serial.println(")");
    */
    x = map(x, TS_MINX, TS_MAXX, 0, m_tft.width());
    y = map(y, TS_MINY, TS_MAXY, 0, m_tft.height());
    Serial.print("mapped->(");
    Serial.print(x); Serial.print(", ");
    Serial.print(y);
    Serial.println(")");
    if (y < 120) {
      //Serial.println("A palpable hit!");
      if (y > 87) {
        //Serial.print("Up! y: ");
        //Serial.println(y);
        direction = DirUp;
      } else if (y < 35) {
        //Serial.print("Down! y: ");
        //Serial.println(y);
        direction = DirDown;
      } else if (x < 135) {
        //Serial.print("Left! x: ");
        //Serial.println(x);
        direction = DirLeft;
      } else if (x > 190) {
        //Serial.println("Right! y: ");
        //Serial.println(y);
        direction = DirRight;
      }
    }
  }
  return direction;
}

