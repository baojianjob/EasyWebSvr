#include "stdafx.h"
#include "textmsg.h"


TextMSG::TextMSG()
{
  memset(this, 0, sizeof(*this));
}

TextMSG::~TextMSG()
{
  if(pText != NULL)
    delete []pText;
}

// 读数据文件
BOOL TextMSG::ReadFile(FILE *pFile, int MaxTextLen)
{
  int Len = sizeof(*this) - sizeof(TCHAR *);
  if(fread(this, 1, Len, pFile) != Len)
    return FALSE;
  if(TextLen <= 0 || TextLen > MaxTextLen)
    return FALSE;

  if(pText != NULL)
    delete []pText;

  pText = new TCHAR[TextLen + 1];

  if(fread(pText, sizeof(TCHAR), TextLen, pFile) != TextLen)
  {
    delete []pText;
    pText = NULL;
    return FALSE;
  }
  pText[TextLen] = 0;
  return TRUE;
}

// 写数据文件
BOOL TextMSG::WriteFile(FILE *pFile)
{
  assert(pText != NULL && *pText);
  TextLen = (int)_tcslen(pText);

  ::GetLocalTime(&Time);

  int Len = sizeof(*this) - sizeof(TCHAR *);
  if(fwrite(this, 1, Len, pFile) != Len)
    return FALSE;

  if(fwrite(pText, sizeof(TCHAR), TextLen, pFile) != TextLen)
    return FALSE;
  return TRUE;
}
