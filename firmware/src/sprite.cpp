#include <Sprite.h>
#include <Arduino.h>

Sprite::Sprite()
{
}

void Sprite::SetGrid(byte *grid, int cols, int rows)
{
    this->grid = grid;
    this->cols = cols;
    this->rows = rows;

    curCol = 0;
    curRow = 0;

    curDirection = up;

    SetSpeed();
}

int Sprite::GetCurCol()
{
    return curCol;
}

int Sprite::GetCurRow()
{
    return curRow;
}

int Sprite::GetPrevCol()
{
    return prevCol;
}

int Sprite::GetPrevRow()
{
    return prevRow;
}

void Sprite::SetSpeedMode(int speedMode)
{
    this->speedMode = speedMode;
    SetSpeed();
}

void Sprite::SetColorMode(int colorMode)
{
    this->colorMode = colorMode;
}

// Return true if position has changed.
bool Sprite::Tick()
{

    if ((tickMillis + speedDelayMS) > millis())
    {
        return false;
    }

    tickMillis = millis();

    prevCol = curCol;
    prevRow = curRow;
    prevDirection = curDirection;

    // Check if at a T or + interection.
    if (GetNumDirections() > 2)
    {
        // Randomly select direction that is not reverse direction.
        while (1)
        {
            curDirection = AllDirections[random(4)];

            if (curDirection != GetReverseDirection(prevDirection))
            {
                break;
            }
        }
        SetSpeed();
    }
    else
    {
        // Check if at a corner.
        if (!IsDirectionAvailable(curDirection))
        {
            // Find next direction that is not reverse direction.
            for (const auto dir : AllDirections)
            {
                if (IsDirectionAvailable(dir))
                {
                    if (dir != GetReverseDirection(curDirection))
                    {
                        curDirection = dir;
                        break;
                    }
                }
            }
            SetSpeed();
        }
    }

    IncrementPosition(curDirection);

    PositionsToGo--;

    return true;
}

bool Sprite::IsDirectionAvailable(Direction direction)
{
    int xOffset = 0;
    int yOffset = 0;

    if (direction == up)
    {
        yOffset = 1;
    }
    else if (direction == down)
    {
        yOffset = -1;
    }
    else if (direction == left)
    {
        xOffset = -1;
    }
    else if (direction == right)
    {
        xOffset = 1;
    }

    return IsBlockAvailable(curCol + xOffset, curRow + yOffset);
}

int Sprite::GetNumDirections()
{
    int numDirections = 0;

    for (const auto dir : AllDirections)
    {
        if (IsDirectionAvailable(dir))
        {
            numDirections++;
        }
    }

    return numDirections;
}

bool Sprite::IsBlockAvailable(int x, int y)
{
    if (x < 0 || x > (cols - 1))
    {
        return false;
    }

    if (y < 0 || y > (rows - 1))
    {
        return false;
    }

    if (*(grid + (x * rows) + y) == 1)
    {
        return true;
    }

    return false;
}

bool Sprite::IncrementPosition(Direction direction)
{

    int xOffset = 0;
    int yOffset = 0;

    if (direction == up)
    {
        yOffset = 1;
    }
    else if (direction == down)
    {
        yOffset = -1;
    }
    else if (direction == left)
    {
        xOffset = -1;
    }
    else if (direction == right)
    {
        xOffset = 1;
    }

    if (!IsBlockAvailable(curCol + xOffset, curRow + yOffset))
    {
        return false;
    }

    curCol += xOffset;
    curRow += yOffset;

    return true;
}

Sprite::Direction Sprite::GetReverseDirection(Direction direction)
{
    if (direction == up)
    {
        return down;
    }
    else if (direction == down)
    {
        return up;
    }
    else if (direction == left)
    {
        return right;
    }
    return left;
}

uint32_t Sprite::Color(uint8_t r, uint8_t g, uint8_t b)
{
    return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
}

uint32_t Sprite::Wheel(byte WheelPos)
{
    WheelPos = 255 - WheelPos;
    if (WheelPos < 85)
    {
        return Color(255 - WheelPos * 3, 0, WheelPos * 3);
    }
    if (WheelPos < 170)
    {
        WheelPos -= 85;
        return Color(0, WheelPos * 3, 255 - WheelPos * 3);
    }
    WheelPos -= 170;
    return Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

uint32_t Sprite::GetColor()
{
    static unsigned int rainbowColor = 0;

    if (colorMode == 0)
    {
        return Color(255, 0, 0);
    }
    else if (colorMode == 1)
    {
        return Color(0, 255, 0);
    }
    else if (colorMode == 2)
    {
        return Color(0, 0, 255);
    }

    rainbowColor += 5;
    return Wheel((byte)rainbowColor);
}

void Sprite::SetSpeed()
{
    speedDelayMS = random(5 * (speedMode + 1), + 50 * (speedMode + 1));
}