#pragma once

#include <QtWidgets/QWidget>
#include "ui_AppQt.h"
#include<Windows.h>
#include <QTextBrowser>
#include <iostream>
#include<stdio.h>
#include<QPushButton>


class AppQt : public QWidget
{
	Q_OBJECT

public:
	AppQt(QWidget* parent = nullptr);
	~AppQt();

public slots:
	void MsgBoxA();
	void MsgBoxW();
	void Heap();
	void File();
	void Reg();
	void HeapAbnormalAnalyse();
	void FileAbnormalAnalyse();
	void RegAbnormalAnalyse();
	void recvData();
	void Socket();
	void ClearText();
	
private:
	Ui::AppQtClass ui;
	
	QPushButton ButtonBoxA;
	QPushButton ButtonBoxW;
	QPushButton ButtonHeap;
	QPushButton ButtonFile;
	QPushButton ButtonReg;
	QPushButton ButtonAbHeap;
	QPushButton ButtonAbFile;
	QPushButton ButtonAbReg;
	QPushButton ButtonTelecomm;
	QPushButton ButtonClearText;
	
};

