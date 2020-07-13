#include "stdafx.h"
#include "cgilib.h"
#include "cgihtml.h"
#include "mtstring.h"
#include "textmsg.h"

#define ARRAY_SIZE(x)   (sizeof(x) / sizeof(x[0]))
#define ISEMPTY(x)      ((x) == NULL || *(x) == 0)

using namespace minilib;

static char RecordFileName[300];

static const TCHAR *pViewTextBegin = 
  _T("<html>\n")
  _T("<head>\n")
  _T("<title>留言本</title>\n")
  _T("<style>\n")
  _T("<!-- \n")
  _T("p\n")
  _T("{\n")
  _T("  size:6pt;\n")
  _T("}\n")
  _T(".title\n")
  _T("{\n")
  _T("  color:#FFB6C1;\n")
  _T("}\n")
  _T("</style>\n")
  _T("</head>\n")
  _T("<body>\n")
  _T("<p><span class='title'><center><h1>留  言  本</h1></center></span>\n")
  _T("</p>\n")
  _T("<center>\n")
  _T("<table border=\"1\" cellpadding=\"0\" cellspacing=\"0\" bordercolorlight=\"#C0C0C0\" bordercolordark=\"#FFFFFF\" style=\"font-size: 9pt\" width=650>\n");


static const TCHAR *pViewTextEndFormat = 
  _T("</table>\n")
  _T("</center>\n")
  _T("<form action=\"%s?func=add\" method=\"post\">\n")
  _T("<center><p>姓名：\n")
  _T("<input type=\"text\" name=\"name\">\n")
  _T("<p>性别：<input type=\"radio\" name=\"sex\" value=\"0\">女性\n")
  _T("<input type=\"radio\" name=\"sex\" value=\"1\">男性\n")
  _T("<p>EMail：<input type＝\"Text\" name=\"email\" value=\"\">\n")
  _T("<p>我要留言：</p>\n")
  _T("<textarea name=\"content\" rows=15 cols=95%%></textarea>\n")
  _T("<p><input type=submit value=\"发 表\">\n")
  _T("<input type=reset value=\"清除重写\"></p>\n")
  _T("</form>\n")
  _T("</center>\n")
  _T("</body>\n")
  _T("</html>\n");

void FuncHtmlEncode(MtSTRING &HtmlStr, BOOL bRepRet = TRUE, BOOL bRepSpace = TRUE)
{
  HtmlStr.Replace(_T("&"), _T("&amp;"));
  HtmlStr.Replace(_T("<"), _T("&lt;"));
  HtmlStr.Replace(_T(">"), _T("&gt;"));
  HtmlStr.Replace(_T("\""), _T("&quot;"));
  HtmlStr.Replace(_T("\'"), _T("&#0039"));
  if(bRepRet)
    HtmlStr.Replace(_T("\r\n"), _T("<br>\r\n"));
  if(bRepSpace)
    HtmlStr.Replace(_T(" "), _T("&nbsp;"));
}

void ViewTextMsg(CgiDATA &Cgi)
{
  printf(pViewTextBegin);

  FILE *pFile = _tfopen(RecordFileName, _T("rb"));
  if(pFile != NULL)
  {
    TextMSG TextMsg;

    TCHAR TimeStr[256];
    while(TextMsg.ReadFile(pFile))
    {
      printf("<tr height=20  bgcolor=\"#DADADA\">\n");
      printf("<td>作者: %s </td>\n", TextMsg.Name);
      printf("<td>性别: %s </td>\n", TextMsg.bSex ? _T("男") : _T("女"));
      printf("<td>邮件: %s </td>\n", TextMsg.Mail);

      _stprintf(TimeStr, _T("%4d-%d-%d %02d:%02d:%02d"),
        TextMsg.Time.wYear, TextMsg.Time.wMonth, TextMsg.Time.wDay, 
        TextMsg.Time.wHour, TextMsg.Time.wMinute, TextMsg.Time.wSecond);
      printf("<td>%s</td>\n", TimeStr);
      printf("</tr>\n");
      printf("<tr height=80>\n");

      MtSTRING Text(TextMsg.pText);
      FuncHtmlEncode(Text);
      printf("<td colspan=\"4\">%s</td>\n", (LPCTSTR)Text);
      printf("</tr>\n");
    }
    fclose(pFile);
  }

  printf(pViewTextEndFormat, getenv("SCRIPT_NAME"));
}
/*
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<html>
<head>
<meta http-equiv="content-type" content="text/html; charset=utf-8">
<meta http-equiv="refresh" content="0; url=frame.dll?Lib=netoa&funcid=%%V:FuncID%%">
<title>%%V:Title%%</title>
</head>
</html>
*/

void JumpNewPage(const TCHAR *pUrl, const TCHAR *pContent, int TimeSecond = 0)
{
  printf(_T("<html>\n"));
  printf(_T("<head>"));
  printf(_T("<meta http-equiv=\"refresh\" content=\"%d; url=%s\">"), TimeSecond, pUrl);
  printf(_T("<title>正在跳转...</title>\n"));
  printf(_T("</head>\n"));
  printf(_T("<body>%s</body></html>\n"), pContent);
}

void AddTextMsg(CgiDATA &Cgi)
{
  const char *pScriptName = getenv("SCRIPT_NAME");

  TextMSG NewMsg;
  
  char *pName = Cgi.GetValue(_T("name"));
  char *pSex = Cgi.GetValue(_T("sex"));
  char *pMail = Cgi.GetValue(_T("email"));
  char *pText = Cgi.GetValue(_T("content"));
  if(ISEMPTY(pName) || ISEMPTY(pMail) || ISEMPTY(pText))
  {
    JumpNewPage(pScriptName, "姓名、邮件、内容不能为空", 2);
    return;
  }
  lstrcpyn(NewMsg.Name, pName, ARRAY_SIZE(NewMsg.Name) - 1);
  lstrcpyn(NewMsg.Mail, pMail, ARRAY_SIZE(NewMsg.Mail) - 1);
  NewMsg.bSex = Cgi.GetValueInt(_T("sex"), 0) ? TRUE : FALSE;
  int TextLen = _tcslen(pText);
  NewMsg.pText = new TCHAR[TextLen + 1];
  _tcscpy(NewMsg.pText, pText);

  FILE *pFile = _tfopen(RecordFileName, _T("ab"));
  if(pFile == NULL)
  {
    printf("写数据文件失败！");
    return;
  }
  NewMsg.WriteFile(pFile);
  fclose(pFile);

  JumpNewPage(pScriptName, "留言成功，正在跳转...", 1);
}

void main(int argc,char *argv[])
{
/*
  int i = 1;
  while(i == 1)
  {
    Sleep(100);
  }
*/ 
  strcpy(RecordFileName, argv[0]);
  strcpy(strrchr(RecordFileName, '\\') + 1, "msg.dat");

  printf("Content-Type:text/html\n\n");

  CgiDATA Cgi;
  char *pFunc = Cgi.GetValue("func");
  if(pFunc == NULL || strcmp(pFunc, "add") != 0)
  {
    ViewTextMsg(Cgi);
  }
  else
  {
    AddTextMsg(Cgi);
  }
}

/*
如何调试CGI程序？
答：可以通过设置死循环的方法。具体如下：
比如有CGI程序cgi.exe
1). 在cgi.exe的代码最开始加上如下代码
    int i = 1;
    while(i == 1)
    {
      Sleep(100);
    }
2). 在while处设置断点
3). 通过网页执行此cgi程序。则cgi程序必定会在此陷入死循环
4). 到Vc中调试进程，选择cgi.exe，就会在while断点处停下
5). 然后在局部变量监视器中，手工把i的值改为0，这样就可以跳出循环，接着向下跟踪了
*/