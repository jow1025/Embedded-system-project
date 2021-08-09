#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <QMessageBox>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/io.h>
#include <signal.h>
#include <time.h>
#include <string>
#include <QVariant>
#define LED_DEVICE "/dev/fpga_led"
#define FPGA_TEXT_LCD_DEVICE "/dev/fpga_text_lcd"
#define LCD_MAX_BUFF 32
#define LCD_LINE_BUFF 16

using namespace std;
enum Direction
{
    DIR_NONE,
    DIR_LEFT,
    DIR_RIGHT
};

enum BlockType
{
    BLOCK_NONE = 0,
    BLOCK_I,
    BLOCK_J,
    BLOCK_L,
    BLOCK_O,
    BLOCK_S,
    BLOCK_T,
    BLOCK_Z
};

class GameEngine
{
public:
    static const int MAX_LEVEL;

private:
    BlockType **map; //for colors
    bool block[4][4];

    bool gameOver;
    unsigned int rows, cols;
    int blockPosX, blockPosY;
    int level, score, clearedLines;
    int str_size, str_size2;
    int chk_size;
    BlockType blockType, nextBlockType;


    void clearMap();

    void clearBlock();
    void createNewBlock();
    void joinBlockToMap();

    void pullBoardDown(int line);
    void clearFullLines();

    BlockType randomBlock() const;
    BlockType **getMapWithBlock() const;
    bool isBlockOutside(bool block[4][4], Direction direction = DIR_NONE) const;
    bool shouldBlockStop(bool block[4][4]) const;
    unsigned char led_data;
    int led_dev;
    int lcd_dev;
    char lcd_text[32];
    QMessageBox msgBox;
    clock_t led_1second = clock();
public:
    GameEngine(int rows, int columns, int level = 1);
    ~GameEngine();


    void moveBlockToTheSide(Direction dir);
    void moveBlockDown();
    void rotateBlock();


    void getBlockAppearance(BlockType blockType, bool block[4][4]) const;

    bool isGameOver();

    BlockType **getBoard();

    BlockType getBlockType() const;
    BlockType getNextBlockType() const;

    void nextLevel();
    void setLevel(int level);
    int getLevel() const;

    void addScore(int points);
    int getScore() const;

    unsigned int getRows() const;
    unsigned int getColumns() const;
};

#endif // GAMEENGINE_H
