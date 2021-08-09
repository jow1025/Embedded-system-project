#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "User32.lib")
#include <QFile>
#include <QTextStream>
#include <QMainWindow>
#include <QLabel>
#include <QMessageBox>
#include "gameengine.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/io.h>
#include <signal.h>
#define MAX_BUTTON 9
#define FPGA_PUSH_SWITCH_DEVICE "/dev/fpga_push_switch"
#define FND_DEVICE "/dev/fpga_fnd"
#define FPGA_DOT_DEVICE "/dev/fpga_dot"
#define FPGA_STEP_MOTOR_DEVICE "/dev/fpga_step_motor"
#define FPGA_DIP_SWITCH_DEVICE "/dev/fpga_dip_switch"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    Ui::MainWindow *ui;
    QLabel *gameOverLabel;

    GameEngine *engine;
    QTimer *timer;
    QTimer *timer2;
    QTimer *timer3;
    Direction direction;
    int moveBlockDownTime;
    bool immediatelyMoveBlockDown;
    bool rotate;
    bool keyLeftPressed, keyRightPressed, keyDownPressed, keySpacePressed;

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void itoa(int value, char* str, int base);
    void strreverse(char* begin, char* end);

private slots:
    void mainLoop();

    void on_columnsLineEdit_editingFinished();

    void on_rowsLineEdit_editingFinished();

    void on_startButton_clicked();

    void keyEvent();

    void dip_update();
private:
    void initGameTableItems(int rows, int columns);
    void adjustGameTableItemsSize();

    void initGameTable(int rows, int columns);
    void adjustGameTableSize();

    void initNextBlockTableItems();

    void adjustGameWindowSize();

    void initGameOverLabel();
    void showGameOverLabel();
    void hideGameOverLabel();

    void refreshStats();
    void refreshNextBlockTable();

    void addMoveBlockDownTime(int level);

    QBrush getBlockColor(BlockType blockType);
    int i;
    int push_dev;
    int fnd_dev;
    int dot_dev;

    int dip_dev;
    int motor_dev;
    int motor_action;
    int motor_direction;
    int motor_speed;
    int level_size;
    int buff_size;
    int pass_set[8];
    int pwd;

    unsigned char dip_sw_buff;
    unsigned char motor_state[3];
    unsigned char fnd_data[4];
    unsigned char push_sw_buff[MAX_BUTTON];
    QMessageBox msgBox;
};

#endif // MAINWINDOW_H
