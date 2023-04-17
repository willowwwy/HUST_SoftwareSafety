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

//�ع�������,��ʾ����
void FCInject_Qt::paintEvent(QPaintEvent* event)
{
	QStyleOption styleOption;
	styleOption.initFrom(this);
	QPainter painter(this);
	style()->drawPrimitive(QStyle::PE_Widget, &styleOption, &painter, this);
	QWidget::paintEvent(event);
}

//���캯��
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

//ѡ��Ŀ���testApp
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

//������Ϣ��
void FCInject_Qt::on_clearButton_clicked()
{
	ui.output->clear();
}

//��ʼ�ػ�
void FCInject_Qt::on_startButton_clicked()
{
	runflag = 1;
	this->exqt->startThread();
}

//��ʼ�ػ��߳�
void exQT::startThread()
{
	this->start();
}

//��ͣ�ػ��߳�
void FCInject_Qt::on_stopButton_clicked()
{
	runflag = 0;
}

//��ʼ����
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

		//flag�����жϴ�ʱ��������״̬
		while (runflag)
		{
			WaitForSingleObject(hSemaphore, 10);
			memcpy(&RecvInfo, lpBase, sizeof(Info));//���п����洢
			if (st.wMilliseconds == RecvInfo.st.wMilliseconds && st.wSecond == RecvInfo.st.wSecond)//�ж�ʱ���Ƿ�һ��
				continue;
			else
			{
				st = RecvInfo.st;
				QString string;
				//�ַ���ת��
				for (int i = 0; i < RecvInfo.ArgumentNum; i++)
				{
					string += QString(RecvInfo.ArgumentName[i]);
					string += ":";
					string += RecvInfo.ArgumentValue[i];
					string += '\n';
				}
				//����������е���Ϣ���������
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

	//�Ѳ�������
	if (!strcmp(RecvInfo.ArgumentValue[0], "HeapCreate")) {//�ػ�HeapCreate
		for (i = 0; i < 20; i++) {
			if (myheap[i] == 0) {
				//��¼��ʱ�����Ķ���Ϣ
				myheap[i] = strtoul(RecvInfo.ArgumentValue[1], NULL, 16);
				break;
			}
		}
	}
	i = 0;
	if (!strcmp(RecvInfo.ArgumentValue[0], "HeapFree")) {//�ػ�HeapFree
		unsigned temp;
		temp = strtoul(RecvInfo.ArgumentValue[1], NULL, 16);//��¼��ʱ��������Ϣ
		for (i = 0; i < 20; i++) {
			if (myheap[i] == temp) {//�������Ķ���Ϣ���ͷŵĶ���Ϣ����ƥ��
				//���ڴ����Ķ�Ӧ��Ϣ
				myheap[i] = 0;
				QString string = "heapfree balancedly\n";//�����ͷ�
				emit updateTextBrowser(string);
				break;
			}
		}
		if (i == 20) {
			//���ڴ����Ķ�Ӧ��Ϣ,Double Free
			QString string = "heapfree abnormally!\n";
			emit updateTextBrowser(string);
		}
	}

	//�ļ�����
	if (!strcmp(RecvInfo.ArgumentValue[0], "CreateFile")) {
		char fileName[50] = "";
		char indexName[200] = "";
		char* filePath = RecvInfo.ArgumentValue[1];

		//��ȡ�ļ���
		int len = strlen(RecvInfo.ArgumentValue[1]);
		int len_temp = len;
		while (filePath[len_temp - 1] != '\\' && len_temp != 0) {
			--len_temp;
		}
		if (len_temp) {
			//�Ǳ��ļ���
			/*len = len_temp;
			while (filePath[len - 1] == '\\') {
				len--;
			}*/
			strncpy(indexName, filePath, len_temp);
			strcpy(fileName, filePath + len_temp);
		}
		else {
			//�ڱ�·����
			strcpy(indexName, filePath);
			strcpy(fileName, filePath);
		}

		////��ȡ�ļ���
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

		unsigned temp;	//����ģʽ
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

//������Ϣ������
void sendData() {
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	SOCKET servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);//�����׽���
	sockaddr_in sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.sin_family = PF_INET;
	sockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");//����Ŀ�ĵ�ַ
	sockAddr.sin_port = htons(1234);
	bind(servSock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));//�󶨱��ص�ַ���׽ӿ�
	listen(servSock, 20);//���м���
	SOCKADDR clntAddr;
	int nSize = sizeof(SOCKADDR);
	SOCKET clntSock = accept(servSock, (SOCKADDR*)&clntAddr, &nSize);
	char str[32] = "Hello World!";
	send(clntSock, str, strlen(str) + sizeof(char), NULL);//������ϢHello World
	closesocket(clntSock);//�ر��׽���
	closesocket(servSock);
	WSACleanup();
}