#include "AppQt.h"
#pragma comment(lib,"ws2_32.lib")
#include<QPushButton>

QTextBrowser* browser;
AppQt::AppQt(QWidget* parent)
	: QWidget(parent)
{
	resize(QSize(980, 590));
	browser = new QTextBrowser;
	browser->setParent(this);
	browser->setGeometry(100, 500, 1024, 384);

	/* &ButtonBoxA:信号发出者，指针类型
	 * &QPushButton:: released：处理的信号， &发送者的类名::信号名字
	 * this：信号接收者
	 * &AppQt::MsgBoxA： 槽函数，信号处理函数，&接收者类名::槽函数名字
	*/
	ButtonBoxA.setParent(this);
	ButtonBoxA.setText("MessageBoxA");
	ButtonBoxA.move(100, 100);
	connect(&ButtonBoxA, &QPushButton::released, this, &AppQt::MsgBoxA);

	ButtonBoxW.setParent(this);
	ButtonBoxW.setText("MessageBoxW");
	ButtonBoxW.move(400, 100);
	connect(&ButtonBoxW, &QPushButton::released, this, &AppQt::MsgBoxW);

	ButtonHeap.setParent(this);
	ButtonHeap.setText("Heap");
	ButtonHeap.move(100, 200);
	connect(&ButtonHeap, &QPushButton::released, this, &AppQt::Heap);

	ButtonFile.setParent(this);
	ButtonFile.setText("File");
	ButtonFile.move(400, 200);
	connect(&ButtonFile, &QPushButton::released, this, &AppQt::File);

	ButtonReg.setParent(this);
	ButtonReg.setText("Reg");
	ButtonReg.move(700, 200);
	connect(&ButtonReg, &QPushButton::released, this, &AppQt::Reg);

	ButtonAbHeap.setParent(this);
	ButtonAbHeap.setText("HeapAbnormalAnalyse");
	ButtonAbHeap.move(100, 300);
	connect(&ButtonAbHeap, &QPushButton::released, this, &AppQt::HeapAbnormalAnalyse);

	ButtonAbFile.setParent(this);
	ButtonAbFile.setText("FileAbnormalAnalyse");
	ButtonAbFile.move(400, 300);
	connect(&ButtonAbFile, &QPushButton::released, this, &AppQt::FileAbnormalAnalyse);

	ButtonAbReg.setParent(this);
	ButtonAbReg.setText("RegAbnormalAnalyse");
	ButtonAbReg.move(700, 300);
	connect(&ButtonAbReg, &QPushButton::released, this, &AppQt::RegAbnormalAnalyse);

	ButtonTelecomm.setParent(this);
	ButtonTelecomm.setText("Telecommunication");
	ButtonTelecomm.move(100, 400);
	connect(&ButtonTelecomm, &QPushButton::released, this, &AppQt::Socket);

	ButtonClearText.setParent(this);
	ButtonClearText.setText("Clear");
	ButtonClearText.move(900, 900);
	connect(&ButtonClearText, &QPushButton::released, this, &AppQt::ClearText);

	ui.setupUi(this);
}

void AppQt::ClearText()
{
	browser->clear();
}

void AppQt::MsgBoxA()
{
	browser->append("MessageBoxA Test Start:");
	MessageBoxA(NULL, "OldMessageBoxA", "MessageBoxA", NULL);

	browser->append("\n");
}

void AppQt::MsgBoxW()
{
	browser->append("MessageBoxW Test Start:");
	MessageBoxW(NULL, L"OldMessageBoxW", L"MessageBoxW", NULL);
	browser->append("\n");
}

void AppQt::Heap()
{
	browser->append("HeapTest Start:");
	// creates a private heap
	Sleep(500);
	HANDLE heap = HeapCreate(0, 20, 20);
	if (heap)
		browser->append("HeapCreate Success!");
	else
	{
		browser->append("HeapCreate Failed!");
		return;
	}

	int* ptmp = (int*)HeapAlloc(heap, 0, sizeof(int) * 30);
	if (heap != NULL)
	{
		//Free Heap
		Sleep(500);
		if (HeapFree(heap, 0, ptmp))
			browser->append("HeapFree Success!");
		else
		{
			browser->append("HeapFree Failed!");
			return;
		}

		//Destroy Heap
		Sleep(500);
		if (HeapDestroy(heap))
			browser->append("HeapDestroy Success!");
		else
		{
			browser->append("HeapDestroy Failed!");
			return;
		}
	}
	browser->append("\n");
}

void AppQt::File()
{
	browser->append("FileTest Start:");
	Sleep(1000);

	//CreateFile
	HANDLE file = (HANDLE)CreateFile(L"test.txt", GENERIC_ALL | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, NULL, NULL);
	Sleep(1000);
	if (file == INVALID_HANDLE_VALUE)
	{
		browser->append("CreateFile Failed!");
		return;
	}
	else
		browser->append("CreateFile Success!");

	//WriteFile
	unsigned long num = 0;
	BOOL flag;
	Sleep(500);
	flag = WriteFile(file, "WriteFile Hooked!", 15, &num, NULL);
	Sleep(500);
	if (flag)
		browser->append("WriteFile success!");
	else
	{
		browser->append("WriteFile Failed!");
		CloseHandle(file);
		return;
	}

	//ReadFile
	char buffer[20];
	flag = ReadFile(file, buffer, 15, &num, NULL);
	buffer[num] = '\0';
	Sleep(500);

	if (flag)
		browser->append("Readfile success!");
	else
	{
		browser->append("Readfile Failed!");
		CloseHandle(file);
		return;
	}

	//DeleteFile
	char error[50] = { 0 };
	CloseHandle(file);
	Sleep(500);
	flag = DeleteFile(L"test.txt");
	Sleep(500);
	if (flag)
		browser->append("Deletefile success!");
	else
	{
		sprintf_s(error, "%d", GetLastError());
		browser->append("DeleteFile Failed!");
		return;
	}
	browser->append("\n");
}

void AppQt::Reg()
{
	HKEY pReg = NULL;
	TCHAR Data[20] = L"willow";
	Sleep(500);
	browser->append("RegTest Start:");
	//RegCreate
	size_t flag;
	flag = RegCreateKeyEx(HKEY_CURRENT_USER, L"key", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &pReg, NULL);
	if (flag == ERROR_SUCCESS)
		browser->append("RegCreate success!");
	else
	{
		browser->append("RegCreate Failed!");
		return;
	}

	//修改注册表
	size_t iLen = wcslen(Data);
	Sleep(500);
	flag = RegSetValueEx(pReg, L"SetByWwy", 0, REG_SZ, (CONST BYTE*)Data, sizeof(TCHAR) * iLen);
	if (flag == ERROR_SUCCESS)
		browser->append("RegSetValue success!");
	else
	{
		browser->append("RegSetValue Failed!");
		return;
	}
	Sleep(500);
	RegCloseKey(pReg);
	Sleep(500);

	//Open Reg
	flag = RegOpenKeyEx(HKEY_CURRENT_USER, L"key", 0, KEY_ALL_ACCESS, &pReg);
	Sleep(500);
	if (flag == ERROR_SUCCESS)
		browser->append("RegOpenKey success!");
	else
	{
		browser->append("RegOpenKey Failed!");
		return;
	}

	//删除注册表
	flag = RegDeleteValue(pReg, L"SetByWwy");
	Sleep(500);
	if (flag == ERROR_SUCCESS)
		browser->append("RegDeleteValue success!");
	else
	{
		browser->append("RegDeleteValue Failed!");
		return;
	}
	RegCloseKey(pReg);
	browser->append("\n");
}

void AppQt::Socket()
{
	recvData();
}

void AppQt::recvData()
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	SOCKET sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);//创建tcp套接字
	Sleep(500);
	sockaddr_in sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));//设置套接字地址
	sockAddr.sin_family = PF_INET;
	sockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");//本地的ip地址
	sockAddr.sin_port = htons(1234);
	::connect(sock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));//进行connect操作，加上作用域防止与qt内部函数矛盾
	Sleep(500);
	char szBuffer[MAXBYTE] = { 0 };
	recv(sock, szBuffer, MAXBYTE, NULL);//设置缓冲区接受信息
	Sleep(500);

	QString temp;
	temp.sprintf("Message from server: %s\n", szBuffer);
	browser->append(temp);//打印出信息

	closesocket(sock);//关闭套接字
	Sleep(500);
	WSACleanup();
}

void AppQt::HeapAbnormalAnalyse()
{
	browser->append("start HeapAbnormalTest: ");

	HANDLE heap = HeapCreate(0, 20, 20);
	if (heap)
		browser->append("HeapCreate success!");

	int* ptmp = (int*)HeapAlloc(heap, 0, sizeof(int) * 30);

	Sleep(500);
	if (heap != NULL) {
		browser->append("Starting first HeapFree:");
		if (HeapFree(heap, 0, ptmp))//判断第一次Free是否成功
			browser->append("First HeapFree success!");
		else
			browser->append("First HeapFree Failed!");
		Sleep(500);

		printf("Starting second HeapFree:");
		if (HeapFree(heap, 0, ptmp))//判断第二次Free是否成功
			browser->append("Second HeapFree success!");
		else
			browser->append("Error");
		Sleep(500);

		printf("Starting HeapDestroy:");

		if (HeapDestroy(heap))
			browser->append("HeapDestrpy success!");
		else
			browser->append("Second HeapFree Failed!");
	}
	browser->append("\n");
}

void AppQt::FileAbnormalAnalyse()
{
	browser->append("start FileAbnormalTest: ");

	//判断文件夹
	HANDLE hOpenFile1 = (HANDLE)CreateFile(L".\\FileTest.txt", GENERIC_ALL | GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, NULL, NULL);
	browser->append("Create FileTest.txt Success!");
	CloseHandle(hOpenFile1);

	//自我复制
	HANDLE hOpenFile2 = (HANDLE)CreateFile(L".\\CopyAPP.exe", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_ALWAYS, NULL, NULL);
	browser->append("Copy APP.exe Success!");
	CloseHandle(hOpenFile2);

	//修改
	HANDLE hOpenFile3 = (HANDLE)CreateFile(L".\\ModifyAPP.exe", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, NULL, NULL);
	browser->append("Modify APP.exe Success!");
	CloseHandle(hOpenFile3);

	browser->append("\n");
}

//注册表异常分析
void AppQt::RegAbnormalAnalyse()
{
	Sleep(500);
	HKEY hKey = NULL;
	TCHAR Data[20] = L"U202012056";
	browser->append("start RegAbnormalTest: ");
	size_t lRet = RegCreateKeyEx(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL);
	if (lRet == ERROR_SUCCESS)
		browser->append("RegCreate success!");
	else
	{
		browser->append("RegCreate Failed!");
		return;
	}

	Sleep(500);
	size_t iLen = wcslen(Data);
	lRet = RegSetValueEx(hKey, L"SetByWwy", 0, REG_SZ, (CONST BYTE*)Data, sizeof(TCHAR) * iLen);

	if (lRet == ERROR_SUCCESS)
		browser->append("RegSetValue success!");
	else
	{
		browser->append(" RegSetValue Failed!");
		return;
	}

	Sleep(500);
	lRet = RegDeleteValue(hKey, L"SetByWwy");
	if (lRet == ERROR_SUCCESS)
		browser->append("RegDeleteValue success!");
	else
	{
		browser->append("RegDeleteValue Failed!");
		return;
	}
	RegCloseKey(hKey);

	browser->append("\n");
}
AppQt::~AppQt()
{}