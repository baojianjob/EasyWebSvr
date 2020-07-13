#include "stdafx.h"
#include "cgilib.h"

char *GetQueryString()
{
  char *Temp = getenv("QUERY_STRING");
  if(Temp == NULL)
    return NULL;
  int Length = strlen(Temp);
  char *pString = new char[Length + 2];
  strcpy(pString, Temp);
  return pString;
}

char *GetPostString()
{
  char *Method = getenv("REQUEST_METHOD"); 
  if(Method != NULL && stricmp(Method, "POST") == 0)
  {
    int Length=atoi(getenv("CONTENT_LENGTH"));
    char *pString = new char[Length + 2];
    for(int i=0; i < Length; i++)
      pString[i] = fgetc(stdin);
    pString[Length] = '\0';
    return pString;
  }

#if 0
  //char * Debug = "func=add";//Name=ShenYaoQin&Account=sqlemail&Password=11111111";
  char * Debug = "func=add";
                 
  pString = new char[strlen(Debug) + 2];
  strcpy(pString, Debug);
  return pString;
#endif 

  return NULL;
}

int htoi(char *s)
{
  char *digits = "0123456789ABCDEF";
  strupr(s);
  return 16 * (strchr(digits, s[0]) - strchr(digits, '0')) +
         (strchr(digits, s[1]) - strchr(digits, '0'));         
}

CgiDATA::CgiDATA()
{
  pHead = NULL;
  pQueryString = GetQueryString();
  if(pQueryString != NULL)
    strcat(pQueryString, "&");
  DivideValuePairs(pQueryString);

  pPostString = GetPostString();
  if(pPostString != NULL)
    strcat(pPostString, "&");
  DivideValuePairs(pPostString);
} 
   
CgiDATA::~CgiDATA()
{
  delete[] pQueryString;
  delete[] pPostString;
  FormUNIT *pUint = pHead;
  while(pUint != NULL)
  {
    pHead = pUint->pNext;
    delete pUint;
    pUint = pHead;
  }
}

void CgiDATA::DivideValuePairs(char *pString)
{
  if(pString == NULL)
    return;

  char *pChr ,  *Target = pString;
  FormUNIT *pUnit;

  while((pChr = strchr(Target, '&')) != NULL)
  {
    pUnit = new FormUNIT;
    pUnit->Name = Target;
    pUnit->pNext = pHead;
    pHead = pUnit;
    *pChr = '\0';
    Target = pChr + 1;
    
    if((pChr = strchr(pUnit->Name, '=')) != NULL)
    {
      *pChr = '\0';
      pUnit->Val = pChr + 1;     
      DecodeString(&pUnit->Name);
      DecodeString(&pUnit->Val);
    }
  }
}

bool CgiDATA::DecodeString(char **Input)
{
  char *pString = *Input;
  if(pString == NULL)
    return false;
    
  int Length = strlen(pString);  
  char *pChr = pString;

  for(int i = 0; i < Length; i++)
  {
    switch(pString[i])
    {     
      case '+':
      {
        *pChr = ' ';
         break;
      }
      case '%':
      {
        char Str[3];
        Str[0] = pString[i + 1];
        Str[1] = pString[i + 2];
        Str[2] = '\0';        
        *pChr = htoi(Str);
        i += 2;
        break;
      }
      default:
       *pChr = pString[i];
    }    
    pChr++; 
  }    
  *pChr = '\0';     
  return true;
}

char *CgiDATA::GetValue(const char *Name)
{
  FormUNIT *pUnit = pHead;
  while(pUnit != NULL)
  {
    if(strcmp(Name, pUnit->Name) ==  0)
      return pUnit->Val;
   
    pUnit = pUnit->pNext;
  }
  return NULL;
}

int CgiDATA::GetValueInt(const char *pName, int DefaultValue)
{
  char *pStr = GetValue(pName);
  if(pStr == NULL)
    return DefaultValue;
  return atoi(pStr);
}