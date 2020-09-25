#ifndef READTHREAD_H
#define READTHREAD_H

#include <QThread>
#include <QVariant>
#include <QMutex>

typedef union
{
    uint8_t c[4];
    float f;
} union_data;


typedef struct
{
    uint8_t CheckSum;
    uint16_t ErrCode;
    uint16_t Step;
    uint16_t Lenth;
    uint8_t DataCnt;
    uint8_t DataBuf[256];
    
} PROTOCOL_STRUCT;

class readThread : public QThread {
    Q_OBJECT
public:
    explicit readThread(QObject* parent = nullptr);
    void setMain(void* parent);
    bool runFlag;
    
private:
    PROTOCOL_STRUCT RxD;
    uint8_t SourceDeviceId;
    
    void Action_Command_Process(uint8_t cmdType, int action_length,
                                uint8_t* action_data);
    void Uart_Rx_Process(void);
    void run();
    
signals:
    void consolePrint(QByteArray);

    
    
public slots:

};

#endif // READTHREAD_H
