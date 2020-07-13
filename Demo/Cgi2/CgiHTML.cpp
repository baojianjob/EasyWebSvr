#include "stdafx.h"
#include "CgiHTML.h"


CgiHTML::CgiHTML()
{

}

CgiHTML::~CgiHTML()
{

}

void CgiHTML::BeginHtml(const char *Title)
{
  printf("Content-Type:text/html\n\n");
  printf("<HTML>\n");
  printf("<HEAD>\n");
  printf("<TITLE>");
  printf("%s", Title);
  printf("</TITLE>\n");
  printf("</HEAD>\n");
  printf("<BODY>\n");
}

void CgiHTML::EndHtml()
{
  printf("</BODY>\n");
  printf("</HTML>\n");
}

void CgiHTML::Button(char *Name, char *Value, char *Fun, int Size)
{
  if(Fun == NULL)
    printf("<INPUT TYPE = BUTTON NAME = \"%s\" VALUE = \"%s\" SIZE = %d>\n",
          Name, Value,  Size); 
  else
    printf("<INPUT TYPE = BUTTON NAME = \"%s\" VALUE = \"%s\" onClick = \"%s\" SIZE = %d>\n",
          Name, Value, Fun, Size); 
}

void CgiHTML::Submit(char *Name, char *Value, char *Fun, int Size)
{
  if(Fun == NULL)
    printf("<INPUT TYPE = SUBMIT NAME = \"%s\" VALUE = \"%s\" SIZE = %d>\n",
          Name, Value,  Size); 
  else
    printf("<INPUT TYPE = SUBMIT NAME = \"%s\" VALUE = \"%s\" onClick = \"%s\" SIZE = %d>\n",
          Name, Value, Fun, Size); 
}

void CgiHTML::Reset(char *Name, char *Value, char *Fun, int Size)
{
  if(Fun == NULL)
    printf("<INPUT TYPE = RESET NAME = \"%s\" VALUE = \"%s\" SIZE = %d>\n",
          Name, Value,  Size); 
  else
    printf("<INPUT TYPE = RESET NAME = \"%s\" VALUE = \"%s\" onClick = \"%s\" SIZE = %d>\n",
          Name, Value, Fun, Size); 
}

void CgiHTML::Password(char *Name, char *Value, char *Title, int Size)
{
  printf("%s<INPUT TYPE = PASSWORD NAME = \"%s\" VALUE = \"%s\" SIZE = %d>\n",
          Title, Name, Value, Size);
}

void CgiHTML::Edit(char *Name, char *Value, char *Title, int Size)
{
  printf("%s<INPUT TYPE = TEXT NAME = \"%s\" VALUE = \"%s\" SIZE = %d>\n",
          Title, Name, Value, Size);
}

void CgiHTML::Radio(char *Name, char *Title, bool bCheck)
{
  printf("<INPUT TYPE = %s NAME = \"%s\"  %s> %s\n",
          "RADIO", Name, bCheck== true ? "CHECKED" : " ", Title);
            
}

void CgiHTML::CheckBox(char *Name, char *Title, bool bCheck)
{
  printf("<INPUT TYPE = %s NAME = \"%s\"  %s> %s\n",
          "CHECKBOX", Name, bCheck== true ? "CHECKED" : "", Title);
}


void CgiHTML::BeginForm(char *Method, char *Action, char *Name)
{
  printf("<FORM METHOD = \"%s\"", Method);
  if(Action != NULL)
    printf(" ACTION = \"%s\"", Action);
  if(Name != NULL)
    printf(" NAME = \"%s\"", Name);
  printf(">\n");
}

void CgiHTML::EndForm()
{
  printf("</FORM>\n");
}

void CgiHTML::NewLine()
{
  printf("<BR>\n");
}

void CgiHTML::Hidden(char *Name, char *Value)
{
  printf("<INPUT TYPE = HIDDEN NAME = \"%s\" VALUE = \"%s\">\n", Name, Value);
}

void CgiHTML::File(char *Name, char *Value, int Size)
{
  printf("<INPUT TYPE = FILE NAME = \"%s\" VALUE = \"%s\" SIZE = %d>\n",
         Name, Value, Size);
}

void CgiHTML::TextArea(char *Name, int Rows, int Cols, char *Wrap)
{
  printf("<TEXTAREA NAME = \"%s\" ROWS = %d COLS = %d WRAP = %s></TEXTAREA>\n",
         Name, Rows, Cols, Wrap);
}

void CgiHTML::BeginTable(char *Width, char *BGColor, char *BorderColor)
{
  printf("<TABLE BORDER= 0 CELLSPACING = 1 CELLPADDING = 2 BGCOLOR=\"%s\" BORDERCOLOR=\"%s\" WIDTH=\"%s\">\n",
         BGColor, BorderColor, Width);
}

void CgiHTML::EndTable()
{
  printf("</TABLE>\n");
}

void CgiHTML::BeginTableRow(char *BGColor, int Height)
{
  printf("<TR BGCOLOR = \"%s\" HEIGHT = %d>\n", BGColor, Height); 
}

void CgiHTML::EndTableRow()
{
  printf("</TR>\n");
}

void CgiHTML::TableData(char *Content, char *Align, char *VAlign, int Height)
{
   printf("<TD VALIGN=\"%s\" ALIGN=\"%s\" HEIGHT=%d>%s</TD>\n",
           VAlign, Align, Height, Content);
}
