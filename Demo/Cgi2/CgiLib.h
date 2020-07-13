#if !defined(CGIDATA_H____INCLUDED_)
#define CGIDATA_H____INCLUDED_

#include <stdio.h>

char *GetInputString();
int   htoi(char *s);

class CgiDATA
{
protected:
  struct FormUNIT
  {
    char                  *Name;
    char                  *Val;
    FormUNIT              *pNext;
    FormUNIT(){ Name = Val = NULL;  pNext = NULL; }    
  };

  char                    *pQueryString;
  char                    *pPostString;
  FormUNIT                *pHead;
  void                    DivideValuePairs(char *pString);
  bool                    DecodeString(char **Input);     
  //char                   *GetInput(){ return pString; }

public:   
  CgiDATA();
  ~CgiDATA();
  char                   *GetValue(const char *Name);   
  int                     GetValueInt(const char *pName, int DefaultValue = 0);
};

#endif // #if !defined(CGIDATA_H____INCLUDED_)