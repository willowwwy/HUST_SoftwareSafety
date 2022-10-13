#pragma once

#include <QtWidgets/QMainWindow>
#include "qthread.h"
#include "ui_FCInject_Qt.h"
#include<iostream>
#include<Windows.h>
#include<qfiledialog.h>
#include<string.h>
#include<QSet.h>
#include <QStyleOption>
#include <QPainter>

#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"detours.lib")
using namespace std;
void sendData();

class exQT :public QThread
{
    Q_OBJECT

public:
    void startThread();
    void Check();
private:
    void run() override;

signals:
    void updateTextBrowser(QString);

private slots:
};

class FCInject_Qt : public QMainWindow
{
    Q_OBJECT

public:
    FCInject_Qt(QWidget* parent = Q_NULLPTR);

private:
    Ui::FCInject_QtClass ui;
    exQT* exqt;
    //void myShow();
public:
    void paintEvent(QPaintEvent* event);
    void setExQt(exQT* e) { exqt = e; }

    exQT* getExQt() { return exqt; }
private slots:
    void on_startButton_clicked();
    void on_stopButton_clicked();
    void on_clearButton_clicked();
    void on_viewButton_clicked();
    void on_updateTextBrowser(QString);
};

