#include "FCInject_Qt.h"
#include <detours.h>

SYSTEMTIME st;
class Info {
public:
	int ArgumentNum = 0;
	char ArgumentName[10][50] = { 0 };
	char ArgumentValue[10][256] = { 0 };
	SYSTEMTIME st;
}RecvInfo;

int runflag = 1;
QString AppFilePath;
unsigned myheap[20] = { 0 };
QSet<QString> folderindex;
QSet<QString> fileindex;

//重构窗口类,显示背景
void FCInject_Qt::paintEvent(QPaintEvent* event)
{
	QStyleOption styleOption;
	styleOption.initFrom(this);
	QPainter painter(this);
	style()->drawPrimitive(QStyle::PE_Widget, &styleOption, &painter, this);
	QWidget::paintEvent(event);
}

//构造函数
FCInject_Qt::FCInject_Qt(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	this->setExQt(new exQT());
	//this->setStyleSheet("QMainWindow {background-image:url(:/background.png)}");
	exQT* exqt = this->getExQt();
	connect(exqt, SIGNAL(updateTextBrowser(QString)), this, SLOT(on_updateTextBrowser(QString)));
}

HANDLE hSemaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS, NULL, L"HookSemaphore");
HANDLE hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(Info), L"ShareMemory");
LPVOID lpBase = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);

//选择目标的testApp
char filepath[100];
void FCInject_Qt::on_viewButton_clicked()
{
	QString path = QFileDialog::getOpenFileName(this, "Choose a file", ".", "*.exe");
	QByteArray ba = path.toLatin1();
	char* c = ba.data();
	strcpy(filepath, c);
	AppFilePath = filepath;
	if (!path.isEmpty())
	{
		ui.textBrowser->clear();
		ui.textBrowser->setText(path);
	}
}

//清理信息流
void FCInject_Qt::on_clearButton_clicked()
{
	ui.output->clear();
}

//开始截获
void FCInject_Qt::on_startButton_clicked()
{
	runflag = 1;
	this->exqt->startThread();
}

//开始截获本线程
void exQT::startThread()
{
	this->start();
}

//暂停截获本线程
void FCInject_Qt::on_stopButton_clicked()
{
	runflag = 0;
}

//开始运行
void exQT::run()
{
	fileindex.insert("APP.exe");
	//exQt1.startThread();
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(STARTUPINFO));
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
	si.cb = sizeof(STARTUPINFO);
	WCHAR DirPath[MAX_PATH + 1];
	wcscpy_s(DirPath, MAX_PATH, L".\\");
	CHAR DllPath[MAX_PATH + 1] = ".\\Dll.dll";	//dll name
	//wcscpy_s(DirPath, MAX_PATH, L"D:\\Source_code\\Software_safety\\Keshe_Curriculum\\Debug");
	//CHAR DllPath[MAX_PATH + 1] = "D:\\Source_code\\Software_safety\\Keshe_Curriculum\\Debug\\Dll.dll";	//dll name
	WCHAR EXE[MAX_PATH + 1] = { 0 };
	wcscpy_s(EXE, MAX_PATH, AppFilePath.toStdWString().c_str());

	if (DetourCreateProcessWithDllEx(EXE, NULL, NULL, NULL, TRUE,
		CREATE_DEFAULT_ERROR_MODE | CREATE_SUSPENDED | CREATE_NEW_CONSOLE, NULL, DirPath, &si, &pi,
		DllPath, NULL))
	{
		ResumeThread(pi.hThread);
		//sendData();
		GetLocalTime(&st);

		//flag用来判断此时程序运行状态
		while (runflag)
		{
			WaitForSingleObject(hSemaphore, 10);
			memcpy(&RecvInfo, lpBase, sizeof(Info));//进行拷贝存储
			if (st.wMilliseconds == RecvInfo.st.wMilliseconds && st.wSecond == RecvInfo.st.wSecond)//判断时间是否一致
				continue;
			else
			{
				st = RecvInfo.st;
				QString string;
				//字符串转换
				for (int i = 0; i < RecvInfo.ArgumentNum; i++)
				{
					string += QString(RecvInfo.ArgumentName[i]);
					string += ":";
					string += RecvInfo.ArgumentValue[i];
					string += '\n';
				}
				//更新输出框中的信息，进行输出
				emit updateTextBrowser(string);
				Check();
			}
		}
	}
	else
	{
		char error[100];
		sprintf_s(error, "%d", GetLastError());
	}
}

void FCInject_Qt::on_updateTextBrowser(QString string)
{
	ui.output->append(string);
}

void exQT::Check() {
	int i = 0;
	if (!strcmp(RecvInfo.ArgumentValue[0], "Socket")) {
		sendData();
	}

	//堆操作分析
	if (!strcmp(RecvInfo.ArgumentValue[0], "HeapCreate")) {//截获到HeapCreate
		for (i = 0; i < 20; i++) {
			if (myheap[i] == 0) {
				//记录此时创建的堆信息
				myheap[i] = strtoul(RecvInfo.ArgumentValue[1], NULL, 16);
				break;
			}
		}
	}
	i = 0;
	if (!strcmp(RecvInfo.ArgumentValue[0], "HeapFree")) {//截获到HeapFree
		unsigned temp;
		temp = strtoul(RecvInfo.ArgumentValue[1], NULL, 16);//记录此时参数的信息
		for (i = 0; i < 20; i++) {
			if (myheap[i] == temp) {//将创建的堆信息与释放的堆信息进行匹配
				//存在创建的对应信息
				myheap[i] = 0;
				QString string = "heapfree balancedly\n";//正常释放
				emit updateTextBrowser(string);
				break;
			}
		}
		if (i == 20) {
			//存在创建的对应信息,Double Free
			QString string = "heapfree abnormally!\n";
			emit updateTextBrowser(string);
		}
	}

	//文件操作
	if (!strcmp(RecvInfo.ArgumentValue[0], "CreateFile")) {
		char fileName[50] = "";
		char indexName[200] = "";
		char* filePath = RecvInfo.ArgumentValue[1];

		//获取文件名
		int len = strlen(RecvInfo.ArgumentValue[1]);
		int len_temp = len;
		while (filePath[len_temp - 1] != '\\' && len_temp != 0) {
			--len_temp;
		}
		if (len_temp) {
			//非本文件夹
			/*len = len_temp;
			while (filePath[len - 1] == '\\') {
				len--;
			}*/
			strncpy(indexName, filePath, len_temp);
			strcpy(fileName, filePath + len_temp);
		}
		else {
			//在本路径下
			strcpy(indexName, filePath);
			strcpy(fileName, filePath);
		}

		////获取文件夹
		//len = strlen(RecvInfo.ArgumentValue[1]);
		//while (filePath[len - 1] != '\\') {
		//	len--;
		//}
		//while (filePath[len - 1] == '\\') {
		//	len--;
		//}
		//strncpy(indexName, filePath, len);
		//QString index = QString::fromStdString(indexName);

		if (!folderindex.contains(indexName)) {
			folderindex.insert(indexName);
		}

		if (folderindex.size() >= 2) {
			QString string = "WARNING! ! ! Edited multiple folders!\n";
			emit updateTextBrowser(string);
		}

		unsigned temp;	//访问模式
		temp = strtoul(RecvInfo.ArgumentValue[2], NULL, 16);

		if (temp & GENERIC_READ) {
			if (!fileindex.contains(fileName)) {
				fileindex.insert(fileName);
			}
			else {
				QString string = "WARNING! ! ! Might exists self-replication!\n";
				emit updateTextBrowser(string);
			}
		}

		if (temp & GENERIC_WRITE) {
			if (strstr(fileName, ".ocx") || strstr(fileName, ".dll") || strstr(fileName, ".exe")) {
				QString string = "WARNING! ! ! Might modify other executable code!\n";
				emit updateTextBrowser(string);
			}
		}
	}
	if (!strcmp(RecvInfo.ArgumentValue[0], "RegCreateKey")) {
		QString string = "WARNING! ! !RegCreatekey detected!\n";
		emit updateTextBrowser(string);
		if (strstr(RecvInfo.ArgumentValue[2], "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run")) {
			QString string = "WARNING! ! ! Modifiying the startup key!\n";
			emit updateTextBrowser(string);
		}
	}
	if (!strcmp(RecvInfo.ArgumentValue[0], "RegSetValue")) {
		QString string = "WARNING! ! ! RegSetValue detected!\n";
		emit updateTextBrowser(string);
	}
}

//发送信息给本机
void sendData() {
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	SOCKET servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);//创建套接字
	sockaddr_in sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.sin_family = PF_INET;
	sockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");//设置目的地址
	sockAddr.sin_port = htons(1234);
	bind(servSock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));//绑定本地地址与套接口
	listen(servSock, 20);//进行监听
	SOCKADDR clntAddr;
	int nSize = sizeof(SOCKADDR);
	SOCKET clntSock = accept(servSock, (SOCKADDR*)&clntAddr, &nSize);
	char str[32] = "Hello World!";
	send(clntSock, str, strlen(str) + sizeof(char), NULL);//发送信息Hello World
	closesocket(clntSock);//关闭套接字
	closesocket(servSock);
	WSACleanup();
}