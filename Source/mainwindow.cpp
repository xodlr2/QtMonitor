#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->BtnConnect->setText("Connect");

    serialPortSearch();
    serialAddBaud();
    serialPort = new QSerialPort(this);

    QTime time = QTime::currentTime();
    QLabel *lbl_toString = new QLabel(time.toString("AP hh:mm:ss:zzz"), this);
    lbl_toString->setGeometry(500,30,150,30);

    connect(serialPort, SIGNAL(readyRead()), this, SLOT(readData()));
}

MainWindow::~MainWindow()
{
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

void MainWindow::readData()
{
    QByteArray data = serialPort->readAll();
    qDebug() << QTime::currentTime() << data;

}

