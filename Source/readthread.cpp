#include "readthread.h"
#include "mainwindow.h"
#include <QtCore/QDebug>

static MainWindow* m_MainWindow;
extern StructUart Uart;

readThread::readThread(QObject* parent) :
    QThread(parent)
{
    RxD.Step = 0;
    runFlag = false;
}

void readThread::setMain(void* parent)
{
    m_MainWindow = static_cast<MainWindow*>(parent);
}

void readThread::Action_Command_Process(uint8_t cmdType, int action_length,
                                        uint8_t* action_data)
{
    uint8_t Act_Process;
    Act_Process = *action_data++;
    
    //qDebug("Act_Process : 0x%02x", Act_Process);
    
    switch(Act_Process)
    {
//    case TM_PRINT_MESSAGE :
//        if(cmdType == COMM_CONTROL)
//        {
//            QByteArray byteArray = QByteArray::fromRawData((char*)action_data, action_length - 1);
//            QString string(byteArray);
//            emit consolePrint(QByteArray(byteArray));
//        }
//        
        break;
    

    default:
        break;
    }
}


void readThread::Uart_Rx_Process(void)
{
    uint8_t aTempChar = 0/*a, TempArry[2], aTempCnt = 0*/;
    
    while(m_MainWindow->UART_GetChar(&aTempChar))
    {
        //qDebug("Rx[%c] : 0x%02x",aTempChar, aTempChar);
        switch(RxD.Step)
        {
        case 0:                 // Protocol 검사
            if(aTempChar == PROTOCOL_START_FIRST)
            {
                RxD.Step = 1;
                //qDebug("PT:step 0");
            }
            else
            {
                RxD.ErrCode = 1;
            }
            
            break;
            
        case 1:                 // Protocol 검사
            if(aTempChar == PROTOCOL_START_SECOND)
            {
                RxD.Step = 2;
                //qDebug("PT:step 1");
            }
            else
            {
                RxD.ErrCode = 2;
            }
            
            break;
            
        case 2:  // Target MCU ID   이 data 필드가 나에게 온것이 맞는지를 검사.
            if(aTempChar == ID_MAIN_MCU_ID)
            {
                RxD.Step = 3;
                //qDebug("PT:step 2");
            }
            else
            {
                RxD.ErrCode = 3;
            }
            
            break;
            
        case 3:                 // Device ID    이 data 필드를 보낸 주체
            SourceDeviceId = aTempChar;
            RxD.Step = 4;
            //qDebug("PT:step 3");
            break;
            
        case 4:                 // Data Length High byte
            RxD.Lenth = 0xff00 & (aTempChar << 8);
            RxD.CheckSum = 0x73 + ID_MAIN_MCU_ID + SourceDeviceId + aTempChar;
            RxD.Step = 5;
            //qDebug("PT:step 4");
            break;
            
        case 5:                 // Data Length Low byte
            RxD.Lenth = RxD.Lenth | aTempChar;
            RxD.CheckSum = RxD.CheckSum + aTempChar;
            RxD.Step = 6;
            //qDebug("PT:step 5");
            break;
            
        case 6:                 // Control Command
            RxD.CheckSum = RxD.CheckSum + aTempChar;
            RxD.DataCnt = 0;
            RxD.Step = aTempChar;
            //qDebug("PT:Control cmd[%02x]", aTempChar);
            //qDebug("PT:step 6");
            break;
            
        //////////////////////////////////// COMM_GET_INFO ///////////////////////////////////////
        case COMM_GET_INFO:
            RxD.DataBuf[RxD.DataCnt++] = aTempChar;
            RxD.CheckSum = RxD.CheckSum + aTempChar;
            
            if((RxD.Lenth - 8) <= RxD.DataCnt)
            {
                RxD.Step = COMM_GET_INFO + 300;
            }
            
            //qDebug("PT:COMM_CONTROL 1");
            break;
            
        case COMM_GET_INFO + 300:
        
            //qDebug("PT:COMM_CONTROL 2");
            if(aTempChar == RxD.CheckSum)
            {
                RxD.Step = COMM_GET_INFO + 400;
            }
            else
            {
                RxD.ErrCode = 4;
            }
            
            break;
            
        case COMM_GET_INFO + 400:
            qDebug("PT:COMM_GET_INFO 3");
            
            if(aTempChar == ID_PROTOCOL_END)
            {
                //qDebug("count : %d", RxD.DataCnt);
                Action_Command_Process(COMM_GET_INFO, RxD.DataCnt, RxD.DataBuf);
                RxD.Step = 0;
            }
            else
            {
                RxD.ErrCode = 5;
            }
            
            break;
            
        //////////////////////////////////// Control command ///////////////////////////////////////
        case COMM_CONTROL:
            RxD.DataBuf[RxD.DataCnt++] = aTempChar;
            RxD.CheckSum = RxD.CheckSum + aTempChar;
            
            if((RxD.Lenth - 8) <= RxD.DataCnt)
            {
                RxD.Step = COMM_CONTROL + 300;
            }
            
            //qDebug("PT:COMM_CONTROL 1");
            break;
            
        case COMM_CONTROL + 300:
        
            //qDebug("PT:COMM_CONTROL 2");
            if(aTempChar == RxD.CheckSum)
            {
                RxD.Step = COMM_CONTROL + 400;
            }
            else
            {
                RxD.ErrCode = 4;
            }
            
            break;
            
        case COMM_CONTROL + 400:
        
            //qDebug("PT:COMM_CONTROL 3");
            if(aTempChar == ID_PROTOCOL_END)
            {
                //qDebug("count : %d", RxD.DataCnt);
                Action_Command_Process(COMM_CONTROL, RxD.DataCnt, RxD.DataBuf);
                RxD.Step = 0;
            }
            else
            {
                RxD.ErrCode = 5;
            }
            
            break;
            
        default:
            RxD.ErrCode = 6;
            break;
        }   // end switch(RxD.Step){
        
        if(RxD.ErrCode > 0)
        {
            //qDebug("Rx Err:%d", RxD.ErrCode);
            //qDebug("Rx[%c] : 0x%02x",aTempChar, aTempChar);
            RxD.ErrCode = 0;
            RxD.Step = 0;
        }
    }   // end while(mUART1RxDataIsExist()){
}

void readThread::run()
{
    qDebug("run : readThread");
    runFlag = true;
    
    while(runFlag)
    {
        Uart_Rx_Process();
        msleep(1);
    }
}
