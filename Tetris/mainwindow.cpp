#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "fpga_dot_font.h"
#include <QDesktopWidget>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()),this, SLOT(keyEvent()));
    timer->start(100);

    timer2 = new QTimer(this);
    connect(timer2, SIGNAL(timeout()),this,SLOT(dip_update()));
    timer2->start(100);

    dip_dev = open(FPGA_DIP_SWITCH_DEVICE,O_RDWR);
    if(dip_dev<0){
        printf("Device Open Error : %s\n",FPGA_DIP_SWITCH_DEVICE);
        msgBox.setText("Device Open Error!");
        msgBox.setInformativeText("Check Module & Device node");
        msgBox.exec();
        exit(1);
    }
    push_dev = open(FPGA_PUSH_SWITCH_DEVICE,O_RDWR);
    if(push_dev<0){
        printf("Device open error:%s\n",FPGA_PUSH_SWITCH_DEVICE);
        msgBox.setText("Device Open Error!");
        msgBox.setInformativeText("Check Module & DEVICE node");
        msgBox.exec();
        exit(1);
    }
    buff_size=sizeof(push_sw_buff);
    fnd_dev = open(FND_DEVICE,O_RDWR);
    if (fnd_dev<0){
        printf("Device open error : %s\n",FND_DEVICE);
        msgBox.setText("Device Open Error!");
        msgBox.setInformativeText("Check Module & Device node");
        msgBox.exec();
        exit(1);
    }
    dot_dev = open(FPGA_DOT_DEVICE,O_RDWR);
    if (dot_dev<0){
        printf("Device open error : %s\n",FPGA_DOT_DEVICE);
        msgBox.setText("Device Open Error!");
        msgBox.setInformativeText("Check Module & Device node");
        msgBox.exec();
        exit(1);
    }
    motor_dev = open(FPGA_STEP_MOTOR_DEVICE,O_RDWR);
    if(motor_dev<0){
        printf("Device open error:%s\n",FPGA_STEP_MOTOR_DEVICE);
        msgBox.setText("Device Open Error!");
        msgBox.setInformativeText("Check Module & DEVICE node");
        msgBox.exec();
        exit(1);
    }


    initGameOverLabel();
    for(i = 0;i < 4; i++){
        fnd_data[i] = 0;
    }
    write(fnd_dev,&fnd_data,4);
    write(dot_dev,fpga_number[1],level_size);
    motor_action=0;
    motor_direction=0;
    motor_speed=250;
    memset(motor_state,0,sizeof(motor_state));
    motor_action=1;
    motor_state[0]=(unsigned char)motor_action;
    motor_state[1]=(unsigned char)motor_direction;
    motor_state[2]=(unsigned char)motor_speed;
    write(motor_dev,motor_state,3);
    setWindowFlags(Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint);

    QIntValidator *miniValidator = new QIntValidator(0, 9);
    ui->startingLevelLineEdit->setValidator(miniValidator);

    QIntValidator *validator = new QIntValidator(0, 99);
    ui->columnsLineEdit->setValidator(validator);
    ui->rowsLineEdit->setValidator(validator);

    timer3 = new QTimer(this);

    direction = DIR_NONE;
    immediatelyMoveBlockDown = false;
    rotate = false;

    keyLeftPressed = false;
    keyRightPressed = false;
    keyDownPressed = false;
    keySpacePressed = false;
}

MainWindow::~MainWindow()
{
    delete ui;
    delete gameOverLabel;
    delete engine;
    delete timer;
    delete timer2;
    delete timer3;
}

void MainWindow::mainLoop()
{
    if (!engine->isGameOver())
    {
        BlockType **board = engine->getBoard();

        engine->moveBlockToTheSide(direction);

        moveBlockDownTime--;
        if (moveBlockDownTime <= 0 || immediatelyMoveBlockDown)
        {
            immediatelyMoveBlockDown = false;

            engine->moveBlockDown();
            addMoveBlockDownTime(engine->getLevel());

            refreshStats();
            refreshNextBlockTable();
        }
        direction = DIR_NONE;

        if (rotate)
        {
            rotate = false;
            engine->rotateBlock();
        }

        for (int i = 0; i < static_cast<int>(engine->getRows()); i++)
        {
            for (int j = 0; j < static_cast<int>(engine->getColumns()); j++)
            {
                ui->gameTable->item(i, j)->setBackground(getBlockColor(board[j][i]));
            }
        }

        for (unsigned int i = 0; i < engine->getColumns(); i++)
            delete[] board[i];
        delete[] board;

        if (engine->isGameOver()){
            memset(motor_state,0,sizeof(motor_state));
            motor_action=0;
            motor_state[0]=(unsigned char)motor_action;
            motor_state[1]=(unsigned char)motor_direction;
            motor_state[2]=(unsigned char)motor_speed;
            write(motor_dev,motor_state,3);
            showGameOverLabel();
        }
    }
}

void MainWindow::initGameTableItems(int rows, int columns)
{
    ui->gameTable->setRowCount(rows);
    ui->gameTable->setColumnCount(columns);

    for (int i = 0; i < rows; i++)
        for (int j = 0; j < columns; j++)
            ui->gameTable->setItem(i, j, new QTableWidgetItem);

    adjustGameTableItemsSize();
}

void MainWindow::adjustGameTableItemsSize()
{
    int rows = ui->gameTable->rowCount();
    int columns = ui->gameTable->columnCount();

    if (rows > 0 && columns > 0)
    {
        QSize maxSize;
        if (ui->adjustWinSizeCheckBox->isChecked())
            maxSize = QSize(QApplication::desktop()->availableGeometry().width() - 156,
                            QApplication::desktop()->availableGeometry().height() - 64);
        else
            maxSize = QSize(ui->gamePage->width() - 156, ui->gamePage->height() - 20);

        if (ui->gridCheckBox->isChecked())
            ui->gameTable->setShowGrid(true);
        else
            ui->gameTable->setShowGrid(false);

        int newSize = (maxSize.width() / columns < maxSize.height() / rows
                       ? maxSize.width() / columns : maxSize.height() / rows);

        for (int i = 0; i < columns; i++)
            ui->gameTable->setColumnWidth(i, newSize);
        for (int i = 0; i < rows; i++)
            ui->gameTable->setRowHeight(i, newSize);
    }
}

void MainWindow::initGameTable(int rows, int columns)
{
    initGameTableItems(rows, columns);
    adjustGameTableSize();
}

void MainWindow::adjustGameTableSize()
{
    int rows = ui->gameTable->rowCount();
    int cols = ui->gameTable->columnCount();

    if (rows > 0 && cols > 0)
        ui->gameTable->setMaximumSize(ui->gameTable->columnWidth(0) * cols,
                                      ui->gameTable->rowHeight(0) * rows);
}

void MainWindow::initNextBlockTableItems()
{
    int cols = ui->nextBlockTable->columnCount();
    int rows = ui->nextBlockTable->rowCount();

    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            ui->nextBlockTable->setItem(i, j, new QTableWidgetItem);

    for (int i = 0; i < rows; i++)
        ui->nextBlockTable->setColumnWidth(i, ui->nextBlockTable->width() / 4);
    for (int i = 0; i < cols; i++)
        ui->nextBlockTable->setRowHeight(i, ui->nextBlockTable->height() / 4);
}

void MainWindow::adjustGameWindowSize()
{
    this->setGeometry(0, 0, ui->gameTable->maximumWidth() + 160,
                      ui->gameTable->maximumHeight() + 20);
    this->move(0,0);
}

void MainWindow::initGameOverLabel()
{
    gameOverLabel = new QLabel(this);
    gameOverLabel->setText("GAME OVER");
    gameOverLabel->setFont(QFont("Arial", 32));
    gameOverLabel->setAlignment(Qt::AlignCenter);

    QPalette palette = gameOverLabel->palette();
    palette.setColor(gameOverLabel->foregroundRole(), QColor(196, 0, 0));
    gameOverLabel->setPalette(palette);

    gameOverLabel->hide();
}

void MainWindow::showGameOverLabel()
{
    gameOverLabel->setFixedSize(size());
    gameOverLabel->show();
}

void MainWindow::hideGameOverLabel()
{
    gameOverLabel->hide();
}

void MainWindow::refreshStats()
{
    ui->levelLabel->setText("Level: " + QString::number(engine->getLevel()));
    level_size = sizeof(fpga_number[engine->getLevel()]);
    write(dot_dev,fpga_number[engine->getLevel()],level_size);
    ui->scoreLabel->setText("Score: " + QString::number(engine->getScore()));
    if (engine->getScore() >= 10){
        if(engine->getScore() >= 100){
            if(engine->getScore()>=1000){
                fnd_data[0] = engine->getScore()/1000;
                fnd_data[1] = (engine->getScore()-(fnd_data[0]*1000))/100;
                fnd_data[2] = (engine->getScore()-(fnd_data[1]*100))/10;
                fnd_data[3] = engine->getScore()%10;
            }
            else{
                fnd_data[1] = engine->getScore()/100;
                fnd_data[2] = (engine->getScore()-(fnd_data[1]*100))/10;
                fnd_data[3] = engine->getScore()%10;
            }
        }
        else{
            fnd_data[2] = engine->getScore()/10;
            fnd_data[3] = engine->getScore()%10;
        }
    }
    else
        fnd_data[3] = engine->getScore();
    write(fnd_dev,&fnd_data,4);
}

void MainWindow::refreshNextBlockTable()
{
    bool nextBlock[4][4];
    engine->getBlockAppearance(engine->getNextBlockType(), nextBlock);
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            ui->nextBlockTable->item(i, j)->setBackground(nextBlock[j][i] ? getBlockColor(engine->getNextBlockType())
                                                                          : Qt::white);
}

void MainWindow::addMoveBlockDownTime(int level)
{
    moveBlockDownTime = static_cast<int>(((engine->MAX_LEVEL * 6) / (level * 0.8)) - 6);
}

QBrush MainWindow::getBlockColor(BlockType blockType)
{
    switch(blockType)
    {
    case BLOCK_NONE: return Qt::gray;
    case BLOCK_I: return Qt::red;
    case BLOCK_L: return Qt::magenta;
    case BLOCK_J: return Qt::yellow;
    case BLOCK_O: return Qt::blue;
    case BLOCK_S: return Qt::cyan;
    case BLOCK_T: return Qt::green;
    case BLOCK_Z: return QColor(255,128,0); //Orange
    }
}

void MainWindow::on_columnsLineEdit_editingFinished()
{
    if (ui->columnsLineEdit->text().toInt() < 5)
        ui->columnsLineEdit->setText("5");
}

void MainWindow::on_rowsLineEdit_editingFinished()
{
    if (ui->rowsLineEdit->text().toInt() < 5)
        ui->rowsLineEdit->setText("5");
}

void MainWindow::on_startButton_clicked()
{
    QString ApplicationPath=QApplication::applicationDirPath();
        QFile file(ApplicationPath+"/auth.txt");
        //QString filename = "auth.txt";
        //file->setFileName("/home/pi/build-tetris-Desktop-Debug/" + filename);

        if(!file.open(QFile::ReadOnly|QFile::Text))
        {
            if(!file.exists())
            {
                printf("file do not exist");

            }
            else{
                printf("another error");
            }
        }
        QTextStream OpenFile(&file);
        QString str;
        QString auth = "1";
        while(!OpenFile.atEnd())
        {
            str = OpenFile.readLine();
        }
        file.close();
        if(QString::compare(str,auth) == 0 && pwd == 84){
            int rows = ui->rowsLineEdit->text().toInt();
            int cols = ui->columnsLineEdit->text().toInt();
            if (rows > 0 && cols > 0)
            {
                ui->stackedWidget->setCurrentWidget(ui->gamePage);

                initNextBlockTableItems();
                initGameTable(rows, cols);

                if (ui->adjustWinSizeCheckBox->isChecked())
                    adjustGameWindowSize();

                engine = new GameEngine(rows, cols, ui->startingLevelLineEdit->text().toInt());
                refreshStats();
                refreshNextBlockTable();

                connect(timer3, SIGNAL(timeout()), this, SLOT(mainLoop()));
                timer3->start(60);
                addMoveBlockDownTime(engine->getLevel());
            }
        } else if(QString::compare(str,auth) != 0 && pwd != 84){
            QMessageBox MsgBox;
            MsgBox.setText("Face recognition and secondary password miss");
            MsgBox.setDefaultButton(QMessageBox::Ok);
            if(MsgBox.exec() == QMessageBox::Ok){
                this->close();
            }
        }
        else if(QString::compare(str,auth) != 0){
            QMessageBox MsgBox;
            MsgBox.setText("Face recognition miss");
            MsgBox.setDefaultButton(QMessageBox::Ok);
            if(MsgBox.exec() == QMessageBox::Ok){
                this->close();
            }
        }else{
            QMessageBox MsgBox;
            MsgBox.setText("secondary password miss");
            MsgBox.setDefaultButton(QMessageBox::Ok);
            if(MsgBox.exec() == QMessageBox::Ok){
                this->close();
            }
        }
}

void MainWindow::keyEvent()
{
    read(push_dev, &push_sw_buff,buff_size);
    for(i=0;i<MAX_BUTTON;i++){
        if (push_sw_buff[3] == 1)
        {
            keyLeftPressed = true;
            direction = DIR_LEFT;
        }
        if (push_sw_buff[5] == 1)
        {
            keyRightPressed = true;
            if (direction == DIR_LEFT)
                direction = DIR_NONE;
            else
                direction = DIR_RIGHT;
        }
        if (push_sw_buff[7] == 1)
        {
            keyDownPressed = true;
            immediatelyMoveBlockDown = true;
        }
        if (push_sw_buff[4] == 1)
        {
            keySpacePressed = true;
            rotate = true;
        }

        if (push_sw_buff[3] == 0)
            keyLeftPressed = false;
        if (push_sw_buff[5] == 0)
            keyRightPressed = false;
        if (push_sw_buff[7] == 0)
            keyDownPressed = false;
        if (push_sw_buff[4] == 0)
            keySpacePressed = false;
    }
}

void MainWindow::strreverse(char* begin, char* end)
{
    char aux;
    while(end>begin)
        aux=* end, *end --=* begin, *begin ++=aux;
}

void MainWindow::itoa(int value,char* str, int base)
{
    static char num[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    char* wstr = str;
    int sign;

    if(base<2 || base>35)
    {
        *wstr = '\0';
        return;
    }

    if((sign=value) < 0)
        value = -value;
    do
        *wstr++ = num[value%base];

    while(value /= base);

    if(sign < 0)
        *wstr ++='_';
    *wstr = '\0';

    strreverse(str,wstr-1);
}
void MainWindow::dip_update()
{
    int dip[8];
    int i;
    pwd = 0;
    read(dip_dev,&dip_sw_buff,1);

    for(i=0;i<8;i++){
        dip[i] = dip_sw_buff%2;
        dip_sw_buff = dip_sw_buff/2;
    }
    if(dip[0]==1)
        pass_set[0] = 1;
    else
        pass_set[0] = 0;
    if(dip[1]==1)
        pass_set[1] = 2;
    else
        pass_set[1]= 0;
    if(dip[2]==1)
        pass_set[2] = 4;
    else
        pass_set[2] = 0;
    if(dip[3]==1)
        pass_set[3] = 8;
    else
        pass_set[3] = 0;
    if(dip[4]==1)
        pass_set[4] = 16;
    else
        pass_set[4] = 0;
    if(dip[5]==1)
        pass_set[5] = 32;
    else
        pass_set[5] = 0;
    if(dip[6]==1)
        pass_set[6] = 64;
    else
        pass_set[6] = 0;
    if(dip[7]==1)
        pass_set[7] = 128;
    else
        pass_set[7] = 0;
    for(i = 0; i < 8; i++){
        pwd +=pass_set[i];
    }
    // 2^8 = 256
}
