#pragma once

class TextMSG
{
public:
  TCHAR         Name[8];      // 姓名
  BOOL          bSex;         // 性别
  TCHAR         Mail[32];     // 邮件
  SYSTEMTIME    Time;         // 时间
  int           TextLen;      // 内容长度
  TCHAR        *pText;        // 内容

public:
  TextMSG();
  ~TextMSG();

  BOOL          ReadFile(FILE *pFile, int MaxTextLen = 1024 * 8);
  BOOL          WriteFile(FILE *pFile);
};
