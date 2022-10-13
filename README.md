# Software_safety
软安课设
测试样本：APPQt.exe
控制台程序：FCInject_Qt.exe
注入DLL文件：Dll.dll

# 编译说明

#### 编译环境：

    IDE：Microsoft Visual Studio Community 2022 (64 位) - 17.3.4 
    编译器：5.14.2_msvc2017 
    拓展：Qt Visual Studio Tools - 2.9.1.6 
    包含库：Detours Version 3.0 Build_343


#### 配置属性 

    项目配置平台：Win32 Windows SDK 
    版本：10.0 
    平台工具集：Visual Studio 2022 (v143) 
    语言标准：默认(ISO C++14 标准)

### 编译步骤：

※已打包为一个解决方案，其中共含三个项目：APPQt、Dll、FCInject_Qt


#### 1.修改项目dotours库

在Visual Studio中配置项目属性，增添detours环境路径

#### 2.可执行exe

编译生成的dll文件以及exe可执行文件会出现在debug文件夹中。也可以将debug清空，再次尝试编译。

# 

##### PS：由于程序使用相对路径，只要不将解决方案拆开，则可以不用修改dll路径

项目 FCInject_Qt 中 FCInject_Qt.cpp 文件 96-97 行的dll路径

	wcscpy_s(DirPath, MAX_PATH, L".\\");// dll path
	CHAR DllPath[MAX_PATH + 1] = ".\\Dll.dll";	//dll name
  
#运行说明
#### 1. 点击FCInject_Qt.exe运行，选择测试样本文件APPQt.exe路径，点击开始运行
#### 2. 依次点击测试样本，触发API行为，截获信息会在APPQt.exe显示出来


PS：若显示   "应用程序无法正常启动(0xc000007b）"
        可能是qt环境出了问题，我总结问题如下：
        https://blog.csdn.net/m0_54129327/article/details/126978063?spm=1001.2014.3001.5501

※应该是可以直接运行的，若出现其他报错可以重新编译一下源码，在Debug目录下重新运行
