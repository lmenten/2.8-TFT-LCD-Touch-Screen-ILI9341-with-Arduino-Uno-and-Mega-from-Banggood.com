
#include "LedSimulation.h"
#include <SPI.h>
#include <Wire.h>
#include <TouchScreen.h>


#define YP A2  // must be an analog pin, use "An" notation!
#define XM A3  // must be an analog pin, use "An" notation!
#define YM 8   // can be a digital pin
#define XP 9   // can be a digital pin
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

// Assign human-readable names to some common 16-bit color values:
#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

// The control pins for the LCD can be assigned to any digital or
// analog pins...but we'll use the analog pins as this allows us to
// double up the pins with the touch screen (see the TFT paint example).
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin
Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

LedSimulation *led = new LedSimulation(tft);

#define LCD_CS A3 
#define LCD_CD A2 
#define LCD_WR A1 
#define LCD_RD A0 
#define LCD_RESET A4 
unsigned long LastTime;
unsigned long LastTimeMoved;

void setup(void) 
{
  Serial.begin(9600);
  Serial.println(F("Bit Race"));
  
  tft.begin(0x9341);
  tft.setRotation(2);
  
  Serial.println(F("Game!"));
  tft.fillScreen(BLACK);
  led->clear();
  
  tft.fillCircle(60, 320 - 40, 20, BLUE);
  tft.fillCircle(40 * 4 + 20, 320 - 40, 20, BLUE);

  long LastTime = 0;
  long LastTimeMoved = 0;
}

bool isHome = true; // display the homescreen
unsigned long deadTime = millis(); // time that the player dies
int heroX = 5; //start the hero in the center of the screen
float vSpeed = 0.3;
float borderPos = 0;
// positions of the enemies
int enemyX[2] = {5, 10};
float enemyY[2] = {0, -8};

/**
   Updates the player position
   @param  {int} int d             player direction. 0 keep, -1 left, 1 right
*/
void goDirection(int d) {
  if (d > 0 && heroX < 12) {
    Serial.println("GoLeft");
    heroX += d;
  } else if (d < 0 && heroX > 3) {
    Serial.println("GoRight");
    heroX += d;
  }
}

/**
   Draws the hero car in the screen
   Hero looks like:
 *         *
 *        ***
 *         *
 *        ***
    HeroX indicates the middle px in the bottom of the car
*/
void drawHero(void)
{
  led->setLedOn(15, heroX);
  led->setLedOn(15, heroX + 1);
  led->setLedOn(15, heroX - 1);
  led->setLedOn(14, heroX);
  led->setLedOn(13, heroX);
  led->setLedOn(13, heroX + 1);
  led->setLedOn(13, heroX - 1);
  led->setLedOn(12, heroX);
}

/**
   Draw road edges
*/
void drawBorder(void)
{
  byte i;
  if (borderPos > 1) {
    i = 1;
  } else {
    i = 0;
  }
  if (borderPos > 2)
    borderPos = 0;
  for (i; i < 16; i += 2) {
    led->setLedOn(i, 1);
    led->setLedOn(i, 14);
  }
}

/**
   Update entities positions in the scene
*/
void updatePos(void)
{
  borderPos += vSpeed;

  for (byte e = 0; e < 2; e++) {
    enemyY[e] += vSpeed;
    if (enemyY[e] > 18) {
      enemyY[e] = -1;
      if (random(2))
        enemyX[e] = 5;
      else
        enemyX[e] = 10;
    }
  }
}

/**
   restore default values
*/
void restartGame(void)
{
  heroX = 5;
  borderPos = 0;
  enemyX[0] = 5;
  enemyX[1] = 10;
  enemyY[0] = 0;
  enemyY[1] = -8;
}

/** Dies */
void heroDie(void)
{
  deadTime = millis();
  isHome = true;
  restartGame();
}

void checkCollisions() {
  for (byte e = 0; e < 2; e++) {
    int ex = enemyX[e], ey = (int)enemyY[e];
    int diffX = abs(heroX - ex);

    if (ey >= 13 && diffX <= 2) {
      heroDie();
    }
  }
}

/** Draw enemies in the scene
  Enemy looks like:

         ***
         ***

  The position of the enemy indicates the * in the bottom
*/
void drawEnemies() {
  for (byte e = 0; e < 2; e++) {
    byte y = (byte) enemyY[e];
    byte x = (byte) enemyX[e];

    if (y >= 0 && y <= 15)
      led->setLedOn(y, x);
    if (y - 1 >= 0 && y - 1 <= 15) {
      led->setLedOn(y - 1, x);
      led->setLedOn(y - 1, x - 1);
      led->setLedOn(y - 1, x + 1);
    }
    if (y - 2 >= 0 && y - 2 <= 15) {
      led->setLedOn(y - 2, x - 1);
      led->setLedOn(y - 2, x + 1);
      led->setLedOn(y - 2, x);
    }
  }
}

/**
   Check collisions and update game entities
*/
void gameUpdate() {
  //Serial.println("Update");
  updatePos();
  checkCollisions();
  drawHero();
  drawEnemies();
  drawBorder();
  led->draw();
}

unsigned char  Logo[16][16] = {
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
  {1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1},
  {1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1},
  {1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1},
  {1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1},
  {1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1},
  {1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1},
  {1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1},
  {1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1},
  {1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1},
  {1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1},
  {1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1},
  {1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1},
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
};

unsigned char  Home[16][16] = {
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
  {1, 1, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 1, 1},
  {1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1},
  {1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1},
  {1, 1, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1},
  {1, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1},
  {1, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1},
  {1, 1, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1},
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
  {1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0},
  {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1},
  {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1},
  {1, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0},
  {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1},
  {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0},
};

/**
   Arduino loop. If game is running, just keep the game loop running.
   Otherwise shows the homescene
*/
// game variables
bool isLogo = true; // display the homescreen


#define MINPRESSURE 10
#define MAXPRESSURE 1000
// This is calibration data for the raw touch data to the screen coordinates
#define TS_MINX 130
#define TS_MINY 110
#define TS_MAXX 940
#define TS_MAXY 920

int touch(int direction)
{
  // See if there's any  touch data for us
  // See if there's any  touch data for us
  TSPoint p = ts.getPoint();
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  if (p.z > MINPRESSURE) {
    Serial.print("("); Serial.print(p.x);
    Serial.print(", "); Serial.print(p.y); Serial.println(")");
    uint16_t x, y;
    y = map(p.x, TS_MINX, TS_MAXX, 0, 240); //tft.width());
    x = map(p.y, TS_MINY, TS_MAXY, 0, 320); //tft.height());
    Serial.print("mapped -> ("); Serial.print(x);
    Serial.print(", "); Serial.print(y); Serial.println(")");
    if (y < 50) {
      //Serial.println("A palpable hit!");
      if (x < 160) {
        Serial.print("Left! x: ");
        Serial.println(x);
        direction = -1;
      } else {
        Serial.println("Right! x: ");
        Serial.println(x);
        direction = 1;
      }
    }
  }
  return direction; //player direction. 0 keep pos, -1 go left, +1 go right
}

int direction = 0;
void loop(void)
{
  unsigned long CurTime = millis();
  direction = touch(direction);

  if (isLogo) {
    led->setScene(Logo);
    led->draw();
    if (millis() - deadTime > 5000)
      isLogo = false;
  } else {
    if (isHome) {
      led->setScene(Home);
      led->draw();
      //if (direction != 0 && millis() - deadTime > 1000)
      if (millis() - deadTime > 1000)
        isHome = false;
    } else {
      if ((CurTime - LastTimeMoved) > 5) {
        goDirection(direction);
        direction = 0;
        LastTimeMoved = CurTime;
      }
      if ((CurTime - LastTime) > 30) {
        gameUpdate();
        LastTime = CurTime;
      }
    }
  }
}
