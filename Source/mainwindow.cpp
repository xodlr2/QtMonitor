#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QThread>
#include <QDebug>
#include <QProcess>
#include <QTime>

static QMutex lock;
static StructUart Uart;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->BtnConnect->setText("Connect");

    serialPortSearch();
    serialAddBaud();
    console = new Console(ui->consoleWidget);
    console->resize(381, 451);
    console->setEnabled(true);
    ui->consoleWidget = console;
    ui->consoleWidget->setEnabled(true);
    serialPort = new QSerialPort(this);

    QTime time = QTime::currentTime();
    QLabel *lbl_toString = new QLabel(time.toString("AP hh:mm:ss:zzz"), this);
    lbl_toString->setGeometry(500,30,150,30);

    connect(serialPort, SIGNAL(error(QSerialPort::SerialPortError)), this,
            SLOT(handleError(QSerialPort::SerialPortError)));
    connect(serialPort, SIGNAL(readyRead()), this, SLOT(readData()));
    //    connect(console, SIGNAL(getData(QByteArray)), this, SLOT(writeData(QByteArray)));
    //    process = new QProcess(this);
    //    connect(process , SIGNAL(finished(int, QProcess::ExitStatus)), this,
    //            SLOT(processFinished(int, QProcess::ExitStatus)));
    //    connect(process , SIGNAL(readyReadStandardOutput()), this, SLOT(processRead()));
    //    connect(process , SIGNAL(readyReadStandardError()), this, SLOT(processErrorRead()));
    //    connect(process , SIGNAL(started()), this, SLOT(processStart()));
    rxThread = new readThread(this);
    rxThread->setMain(this);
    connect(rxThread , SIGNAL(consolePrint(QByteArray)), this, SLOT(putPrint(QByteArray)),
            Qt::BlockingQueuedConnection);
    rxThread->start();
}

MainWindow::~MainWindow()
{
    disconnect(serialPort, SIGNAL(error(QSerialPort::SerialPortError)), this,
               SLOT(handleError(QSerialPort::SerialPortError)));
    disconnect(serialPort, SIGNAL(readyRead()), this, SLOT(readData()));
    //    disconnect(console, SIGNAL(getData(QByteArray)), this, SLOT(writeData(QByteArray)));
    //    disconnect(process , SIGNAL(finished(int, QProcess::ExitStatus)), this,
    //               SLOT(processFinished(int, QProcess::ExitStatus)));
    //    disconnect(process , SIGNAL(readyReadStandardOutput()), this, SLOT(processRead()));
    //    disconnect(process , SIGNAL(readyReadStandardError()), this, SLOT(processErrorRead()));
    //    disconnect(process , SIGNAL(started()), this, SLOT(processStart()));
    //    disconnect(rxThread , SIGNAL(consolePrint(QByteArray)), this,
    //               SLOT(putPrint(QByteArray))/*, Qt::BlockingQueuedConnection*/);
    if(rxThread->isRunning())
    {
        rxThread->runFlag = false;
        
        while(rxThread->isRunning());
    }
    
    delete rxThread;
    delete process;
    delete serialPort;
    delete console;
    delete ui;
}

void MainWindow::serialPortSearch()
{
    ui->ComPort->clear();
    foreach(const QSerialPortInfo& info, QSerialPortInfo::availablePorts())
    {
        qDebug() << "Name : " << info.portName();
        qDebug() << "Description : " << info.description();
        qDebug() << "Manufacturer: " << info.manufacturer();
        ui->ComPort->addItem(info.portName());
    }
}

void MainWindow::serialAddBaud()
{
    ui->ComBaud->addItem("9600");
    ui->ComBaud->addItem("115200");
    ui->ComBaud->addItem("19200");
    ui->ComBaud->addItem("38400");
    ui->ComBaud->addItem("57600");
}

void MainWindow::on_BtnConnect_clicked()
{
    if(ui->BtnConnect->text() == QString("Connect"))
    {
        serialPort->setPortName(ui->ComPort->currentText());
        serialPort->setBaudRate(ui->ComBaud->currentText().toInt());
        serialPort->setDataBits(QSerialPort::Data8);
        serialPort->setParity(QSerialPort::NoParity);
        serialPort->setStopBits(QSerialPort::OneStop);
        serialPort->setFlowControl(QSerialPort::NoFlowControl);
        if(serialPort->open(QIODevice::ReadWrite))
        {
            ui->BtnConnect->setText("DisConnect");
            console->clear();
        }
        else
        {
            qDebug() << "error : " << serialPort->errorString();
        }
    }
    else
    {
        serialPort->close();
        ui->BtnConnect->setText("Connect");
    }
}

void MainWindow::handleError(QSerialPort::SerialPortError error)
{
    if(error == QSerialPort::ResourceError)
    {
        QMessageBox::critical(this, tr("Critical Error"), serialPort->errorString());
        serialPort->close();
        //ui->SendButton->setEnabled(false);
        ui->BtnConnect->setText("Connect");
    }
}

void MainWindow::writeData(const QByteArray& data)
{
    serialPort->write(data);
    
    if(process->state() == QProcess::Running)
    {
        process->write(data);
    }
    else
    {
        qDebug("process not running!!!");
    }
}

uint8_t MainWindow::UART_GetChar(uint8_t* pData)
{
    lock.lock();
    
    if(Uart.RxInCnt == Uart.RxOutCnt)
    {
        lock.unlock();
        return 0;
    }
    else
    {
        *pData = Uart.RxBuf[Uart.RxOutCnt];
    }
    
    if(Uart.RxOutCnt < UART_RBUF_SIZE - 1)
    {
        Uart.RxOutCnt++;
    }
    else
    {
        Uart.RxOutCnt = 0;
    }
    
    lock.unlock();
    return 1;
}

void MainWindow::readData()
{
    QByteArray data = serialPort->readAll();
    lock.lock();
    qDebug() << QTime::currentTime() << data;
    putPrint("["+QTime::currentTime().toString().toUtf8()+"]-");
    putPrint(data);
    lock.unlock();
}

void MainWindow::putPrint(QByteArray data)
{
    //qDebug() << "processRead 3";
    console->putData(data);
}
