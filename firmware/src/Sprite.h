#include <Arduino.h>

class Sprite
{

public:
    Sprite();
    void SetGrid(byte *grid, int cols, int rows);
    bool Tick();
    int GetCurCol();
    int GetCurRow();
    int GetPrevCol();
    int GetPrevRow();
    uint32_t GetColor();

    void SetSpeedMode(int speedMode);
    void SetColorMode(int colorMode);
    bool IsBlockAvailable(int x, int y);
    int PositionsToGo;

private:
    enum Direction
    {
        up,
        down,
        left,
        right
    };
    const Direction AllDirections[4] = {up, down, left, right};

    byte *grid;
    int cols;
    int rows;
    int speedMode;
    int colorMode;
    int speedDelayMS;    

    int curCol;
    int curRow;
    int prevCol;
    int prevRow;
    Direction curDirection;
    Direction prevDirection;

    unsigned long tickMillis;

    int GetNumDirections();
    bool IncrementPosition(Direction direction);
    bool IsDirectionAvailable(Direction direction);
    Direction GetReverseDirection(Direction direction);

    void SetSpeed();

    uint32_t Wheel(byte WheelPos);
    uint32_t Color(uint8_t r, uint8_t g, uint8_t b);


};
