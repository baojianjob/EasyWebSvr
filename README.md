### 软件名称： 
EasyWebServer网站服务器 V1.9


### 软件功能：
这是一款小型的Web服务器软件。它可以很快速地在您的PC上创建一个站点，而无需IIS等庞大复杂的工具。
它的主要功能特点如下：

- 遵循HTTP/1.1协议标准
-  支持CGI、ISAPI，支持PHP、ASP、Perl
-  支持自定义端口、最大连接数、网络超时
-  支持自定义缺省文档列表，支持目录浏览
-  支持中文UTF-8编码
-  支持Cookie，支持SOAP等
-  支持多线程文件下载、断点续传
-  支持访问日志记录
-  极少的资源占用，极快的访问速度，极简单的设置
-  绿色软件，无须安装，不写注册表
-  VC++ SDK 编写，体积超小


### 运行环境：
Windows 98/Me/2000/XP


### 使用说明：
1. 设置好主目录、端口号等参数后，启动服务器
   点右键菜单--“浏览主页”，就可以在浏览器中看到效果了
   （作为演示，可以把主目录设置为本目录下的demo子目录）

2. 如果要执行CGI/ISAPI或PHP等脚本，注意要设置相应权限，并设置相应的脚本映射

3. 如何设置以支持PHP？

  > 1. 到http://www.php.net网站下载PHP的安装包，如php-5.2.4-Win32.zip（建议用php5）
  >
  > 2. 解压到硬盘某个目录，如c:\php5
  >
  > 3. 把目录下的php.ini-recommended改名为php.ini
  >
  > 4. 在桌面--我的电脑--右键--属性--高级--环境变量，在系统变量中找到Path行，点编辑，在原来的变量值最后增加c:\php5，注意用分号分隔，确定。
  >
  > 5. 在EasyWebServer中点右键菜单--设置--映射，点添加按钮，在“添加/编辑脚本映射”对话框中，扩展名栏输入php，并指定可执行文件（即解释器）
  >
  >       在c:\php5中有2个解释器文件，一个是CGI版的：c:\php5\php-cgi.exe，另一个是ISAPI版的：c:\php5\php5isapi.dll
  >
  >       这两个随便选择一个都可以，但推荐使用ISAPI版的。确定。
  >
  > 6. 访问demo目录下的hello.php，应该就可以看到效果了

4. 如何设置以支持ASP？

  > EasyWebServer对ASP的支持，目前是通过安装第三方ActiveHTML解释器来实现的，这也是在
  > Apache等非IIS服务器上支持ASP的方法，具体步骤如下：
  >
  > 1. 到网上（如华军、天空等软件下载站）下载ActiveHTML
  >
  > 2. 安装ActiveHTML，比如安装到C:\Program Files\seliSoft\ActiveHTML
  >
  > 3. 在EasyWebServer中点右键菜单--设置--映射，点添加按钮，在“添加/编辑脚本映射”对话框中，扩展名栏输入asp，并指定可执行文件（即解释器）为 
  >
  >    C:\Program Files\seliSoft\ActiveHTML\AHTML.exe 点确定。
  >
  > 4. 访问demo目录下的hello.asp，应该就可以看到效果了

5. 如何设置以支持Perl？

  > 1. 到网上（如华军、天空等软件下载站）下载ActivePerl
  > 2. 安装ActivePerl，比如安装到C:\Perl目录
  > 3. 在EasyWebServer中点右键菜单--设置--映射，点添加按钮，
  >    ​    在“添加/编辑脚本映射”对话框中，扩展名栏输入pl，并指定可执行文件（即解释器）
  >    ​    在c:\perl\bin目录下有2个解释器文件，
  >    ​    一个是CGI版的：c:\perl\bin\perl.exe
  >    ​    另一个是ISAPI版的：c:\perl\bin\PerlEx30.dll
  >    ​    目前(1.9版)只支持CGI版的perl.exe，注意要在后面加上一个参数"%s"，如下
  >    ​    C:\Perl\bin\perl.exe "%s"
  >    ​    点确定。
  > 4. 访问demo目录下的hello.pl，应该就可以看到效果了

在Demo子目录下有CGI、ISAPI、PHP等的演示，详见Demo\readme.txt

### 常见问题：

1. 为什么执行我的PHP（或ASP）脚本时，浏览器中看到的却是PHP（或ASP）的代码？
  
  > 答：要想正常执行PHP脚本，必须满足2个条件：
  >
> 1. 在服务器设置--权限中，选中“允许执行脚本”
  > 2. 设置好相应的脚本映射，具体见使用说明中“如何设置以支持PHP”
  
2. 我从哪里可以得到该软件的最新版本？

  > 答：请到这里下载最新版：http://www.onlinedown.net/soft/47720.htm


欢迎测试，并提出您的宝贵意见和建议！

​                                                                 baojianjob@tom.com
​                                                                           2007.11.14
