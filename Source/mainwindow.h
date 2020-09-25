#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTime>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void serialPortSearch();
    void serialAddBaud();
    void readData();

    void on_BtnConnect_clicked();

private:
    Ui::MainWindow *ui;

    QSerialPort* serialPort;
  //  QSerialPort::BaudRate baudRate;

};
#endif // MAINWINDOW_H
