#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QProcess>
#include <QMutex>
#include <QTimer>
#include "console.h"
#include "readthread.h"


////////////////////////////////////////////////////////////////////
// PROTOCOL Part     0x27, 0x30~0x39,
////////////////////////////////////////////////////////////////////

#define PROTOCOL_START_FIRST  '#'       //0x23
#define PROTOCOL_START_SECOND 'P'       //0x50
#define ID_PROTOCOL_END       '!'       //0x21
/////////////////////////// Device ID /////////////////////////////////
#define ID_MAIN_MCU_ID        '$'       //0x24
#define ID_USER_PC_ID               0x25
#define ID_PC_GUI_ID                0x26
#define ID_COLLIMATOR_MCU_ID        0x27
#define ID_COMPRESSION_MCU_ID      0x28
#define ID_BUCKY_MCU_ID             0x29
#define ID_GENERATOR_MCU_ID         0x2A
#define ID_TOUCH_BOARD_ID           0x2B
#define ID_PADDLE_SEN_MCU_ID        0x2C
#define ID_PADDLE_DISPLAY_MCU_ID    0x2D

/////////////////////////// Control command ///////////////////////////
#define COMM_INITIAL            '<'     //0x3C
#define COMM_CONTROL            '>'     //0x3E
#define COMM_GET_INFO       '^'         //0x5E
#define COMM_STATUS_INFO        ','     //0x2C
#define COMM_EEPROM_WRITE     '.'     //0x2E
#define COMM_VERSION_INFO     '?'     //0x3F
#define COMM_SELF_TEST          '/'     //0x2F
#define COMM_CONFIGURATION       '"'     //0x22
#define COMM_SYS_CONTROL    0x23    // 남과장 확인 필요.


/////////////////////////// Action command ///////////////////////////////
#define MODE                            0x30
#define SYSTEM_STATUS                   0x31
#define MOVING_STATUS                   0x32

#define SHOT_BUTTON                     0x38
#define READY_BUTTON                    0x39
#define NEXT_BUTTON                     0x3A

#define KV_VALUE                        0x40
#define MA_VALUE                        0x41
#define SEC_VALUE                       0x42
#define KV_OFFSET                       0x43
#define MA_OFFSET                       0x44
#define SEC_OFFSET                      0x45
#define KV_MIN                          0x46
#define KV_MAX                          0x47
#define MA_MIN                          0x48
#define MA_MAX                          0x49
#define SEC_MAX                         0x4A
#define FOCUS                           0x4B
#define SERIAL_NO                       0x4C
#define GENERATOR_SERIAL_NO             0x4D

#define CARM_TUBE_DEG                   0x60
#define CARM_DET_DEG                    0x61
#define CARM_TUBE_DEG_OFFSET            0x62
#define CARM_DET__DEG_OFFSET            0x63
#define CARM_ANGLE_STOP_DEG             0x64
#define CARM_TUBE_ROTATE_LEFT_RIGHT     0x65
#define CARM_DET_ROTATE_LEFT_RIGHT      0x66
#define CARM_TUBE_ROTATE_INITIALIZE     0x67
#define STANDARD_ROTATE_SPEED           0x68
#define MAX_ROTATE_SPEED                0x69
#define CLUTCH_LOCK                     0x6A

#define TOMO_TRAJECTORY                 0x70
#define TOMO_ANGLE_RETURN               0x71

#define LIFT_HEIGHT                     0x80
#define LIFT_UP_DOWN                    0x81
#define LIFT_INITIALIZE                 0x82

#define COMP_HEIGHT                     0x90
#define COMP_SET_RELEASE_HEIGHT         0x91
#define COMP_FORCE                      0x92
#define COMP_SET_FORCE                  0x93
#define COMP_UP_DOWN                    0x94
#define COMP_INITIALIZE                 0x95

#define COMP_RELEASE_BUTTON             0x96
#define COMP_MANUAL_UP_DOWN             0x97

#define PADDLE_TYPE                     0x9A

#define COL_ABSOLUTE_MOVE               0xB0
#define COL_RELATIVE_MOVE               0xB1
#define COL_MAX_LIMIT                   0xB2
#define COL_INITIALIZE                  0xB3
#define COL_POWER_LED                   0xB4
#define COL_LED_TIME                    0xB5
#define TUBE_TEMPERATURE                0xB6

#define BUCKY_TYPE                      0xC0
#define BUCKY_START                     0xC1
#define BUCKY_MOVING_TIME               0xC2

#define PATIENT_INFO                    0xD0

#define T_ERROR_CODE                    0xE0
#define T_ERROR_DELETE                  0xE1
#define WARNING_CODE                    0xE2
#define T_WARNING_DELETE                0xE3

//for Colimator PC monitor
#define TM_PRINT_MESSAGE                0xE4
#define PM_GET_MOTOR_CONFIG             0xE5
#define PM_GET_TMC_REGISTER             0xE6
#define PM_GET_VELOCITY_POSITION        0xE7
#define PM_SET_LOOP_TEST_START          0xE8
#define PM_SET_LOOP_TEST_STOP           0xE9
#define PM_GET_LOOP_TEST_COUNT          0xEA
#define PM_X_HOMMING                    0xEB

#define UART_RBUF_SIZE  65535

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

typedef struct
{
    uint16_t RxInCnt, RxOutCnt;
    uint16_t TxInCnt, TxOutCnt;
    uint8_t RxBuf[UART_RBUF_SIZE];
    
} StructUart;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = nullptr);
    uint8_t UART_GetChar(uint8_t* pData);
    ~MainWindow();

private slots:
    void serialPortSearch();
    void serialAddBaud();


      void on_BtnConnect_clicked();  
    void handleError(QSerialPort::SerialPortError error);
	    void readData();
		
    void putPrint(QByteArray data);
    void writeData(const QByteArray& data);

	




public:
    readThread* rxThread;
    
private:
    Ui::MainWindow *ui;

    QProcess* process;
    
    QSerialPort* serialPort;
    Console* console;
  //  QSerialPort::BaudRate baudRate;

};
#endif // MAINWINDOW_H
