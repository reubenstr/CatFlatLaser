/*
	Cat Flat Laser
	Reuben Strangelove
	Spring 2020
	
	A flat LED cat toy to simulate a laser pointer.
	
	MCU:
		Arduino Nano (AtMega328p)
		
	
	Notes: 
		



*/

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <Sprite.h>
#include <EEPROM.h>

// https://github.com/JChristensen/JC_Button
#include <JC_Button.h>

#define PIN_NEOPIXEL_ROUND_1 11
#define PIN_NEOPIXEL_ROUND_2 9
#define PIN_NEOPIXEL_ROUND_3 6
#define PIN_NEOPIXEL_ROUND_4 2

#define PIN_NEOPIXEL_CENTER_1 10
#define PIN_NEOPIXEL_CENTER_2 12

#define PIN_BUTTON_MODE 3
#define PIN_BUTTON_COLOR 4
#define PIN_BUTTON_SPEED 5

Adafruit_NeoPixel stripRound1 = Adafruit_NeoPixel(23, PIN_NEOPIXEL_ROUND_1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel stripRound2 = Adafruit_NeoPixel(25, PIN_NEOPIXEL_ROUND_2, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel stripRound3 = Adafruit_NeoPixel(23, PIN_NEOPIXEL_ROUND_3, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel stripRound4 = Adafruit_NeoPixel(25, PIN_NEOPIXEL_ROUND_4, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel stripCenter1 = Adafruit_NeoPixel(23, PIN_NEOPIXEL_CENTER_1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel stripCenter2 = Adafruit_NeoPixel(22, PIN_NEOPIXEL_CENTER_2, NEO_GRB + NEO_KHZ800);

int mode;
int color;
int speed;
const int numModes = 3;
const int numColors = 4;
const int numSpeeds = 3;

const int gridSize = 25;
byte grid[gridSize][gridSize];
const int gridXCrossIndex = 12;
const int gridYCrossIndex = 11;

const int maxSprites = 3;
Sprite sprites[maxSprites];

Button buttonMode(PIN_BUTTON_MODE);
Button buttonColor(PIN_BUTTON_COLOR);
Button buttonSpeed(PIN_BUTTON_SPEED);

void ShowLEDs()
{
  stripRound1.show();
  stripRound2.show();
  stripRound3.show();
  stripRound4.show();
  stripCenter1.show();
  stripCenter2.show();
}

void UpdatePixel(int x, int y, uint32_t color)
{

  // Stips are not physically laid out in a manner which is
  // condusive to logical/simple grid to strip conversion.
  // Next physical version will have better strip layout,
  // then the code may be simplified.

  const int physicalStripOffset = 1;

  // Process outer ring strips.
  if (x == 0 && y != 0)
  {
    stripRound1.setPixelColor(y - physicalStripOffset, color);
  }

  if (x == gridSize - 1)
  {
    stripRound3.setPixelColor((gridSize - 1) - y - physicalStripOffset, color);
  }

  if (y == 0)
  {
    stripRound4.setPixelColor((gridSize - 1) - x, color);
  }

  if (y == gridSize - 1)
  {
    stripRound2.setPixelColor(x, color);
  }

  // Process inner cross strips.
  if (y == gridYCrossIndex && x != 0 && x != (gridSize - 1))
  {
    stripCenter1.setPixelColor((gridSize - 1) - x, color);
  }

  if (x == gridXCrossIndex && y != 0 && y != (gridSize - 1) && y != gridYCrossIndex)
  {
    if (y >= gridYCrossIndex)
    {
      stripCenter2.setPixelColor(y - 1, color);
    }
    else
    {
      stripCenter2.setPixelColor(y, color);
    }
  }
}

void ProcessButtons()
{
  const int holdButtonDelay = 1000;

  buttonMode.read();
  buttonColor.read();
  buttonSpeed.read();

  if (buttonMode.wasPressed())
  {
    mode++;
    if (mode == numModes)
    {
      mode = 0;
    }
    EEPROM.write(0, mode);
  }

  if (buttonColor.wasPressed())
  {
    color++;
    if (color == numColors)
    {
      color = 0;
    }
    for (int i = 0; i < maxSprites; i++)
    {
      sprites[i].SetColorMode(color);
    }
    EEPROM.write(1, color);
  }

  if (buttonSpeed.wasPressed())
  {
    speed++;
    if (speed == numSpeeds)
    {
      speed = 0;
    }
    for (int i = 0; i < maxSprites; i++)
    {
      sprites[i].SetSpeedMode(speed);
    }
    EEPROM.write(2, speed);
  }

  // Check if button(s) are held.
  if (buttonMode.pressedFor(holdButtonDelay))
  {
    mode = 0;
    EEPROM.write(0, mode);
  }

  if (buttonColor.pressedFor(holdButtonDelay))
  {
    color = 0;
    EEPROM.write(1, color);
    for (int i = 0; i < maxSprites; i++)
    {
      sprites[i].SetColorMode(color);
    }
  }

  if (buttonSpeed.pressedFor(holdButtonDelay))
  {
    speed = 0;
    EEPROM.write(2, speed);
    for (int i = 0; i < maxSprites; i++)
    {
      sprites[i].SetSpeedMode(speed);
    }
  }
}

void LoadEEPROM()
{
  mode = EEPROM.read(0);
  color = EEPROM.read(1);
  speed = EEPROM.read(2);

  if (mode == 255)
  {
    mode = 0;
    EEPROM.write(0, mode);
  }

  if (color == 255)
  {
    color = 0;
    EEPROM.write(1, color);
  }

  if (speed == 255)
  {
    speed = 0;
    EEPROM.write(2, speed);
  }
}

void setup()
{
  Serial.begin(115200);
  Serial.println("Cat Flat Laser Startup...");

  buttonMode.begin();
  buttonColor.begin();
  buttonSpeed.begin();

  stripRound1.begin();
  stripRound1.show();
  stripRound2.begin();
  stripRound2.show();
  stripRound3.begin();
  stripRound3.show();
  stripRound4.begin();
  stripRound4.show();
  stripCenter1.begin();
  stripCenter1.show();
  stripCenter2.begin();
  stripCenter2.show();

  // Fill grid with non-allowable blocks.
  for (int x = 0; x < gridSize; x++)
  {
    for (int y = 0; y < gridSize; y++)
    {
      grid[x][y] = 0;
    }
  }

  // Fill grid will allowable blocks.
  for (int i = 0; i < gridSize; i++)
  {
    // Outer edges.
    grid[i][0] = 1;
    grid[i][gridSize - 1] = 1;
    grid[0][i] = 1;
    grid[gridSize - 1][i] = 1;

    // Center cross.
    grid[i][gridYCrossIndex] = 1;
    grid[gridXCrossIndex][i] = 1;
  }

  // Init sprites.
  for (int i = 0; i < maxSprites; i++)
  {
    sprites[i].SetGrid((byte *)grid, gridSize, gridSize);
    sprites[i].SetColorMode(color);
    sprites[i].SetSpeedMode(speed);
  }

  LoadEEPROM();
}

void loop()
{

  ProcessButtons();

  bool updateFlag = false;
  for (int i = 0; i < maxSprites; i++)
  {

    if (sprites[i].Tick())
    {
      UpdatePixel(sprites[i].GetPrevCol(), sprites[i].GetPrevRow(), 0); // Turn off previous pixel.

      // Only display X amount of sprites depending on mode value.
      if (i < (mode + 1))
      {
        UpdatePixel(sprites[i].GetCurCol(), sprites[i].GetCurRow(), sprites[i].GetColor());
      }

      updateFlag = true;
    }

    if (updateFlag)
    {
      ShowLEDs();
    }
  }
}