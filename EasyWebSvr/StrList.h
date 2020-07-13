//***********************************************
// 文件名称：strlist.h
// 功　　能：字符串链表
// 作　　者：wbj
// 创建日期：2002-12-22
// 更新日期：2003-11-26
//***********************************************
#ifndef __STRLIST_H__
#define __STRLIST_H__

#include "mtstring.h"
#include "tlist.h"

namespace minilib
{

class StrLIST : public TLIST<MtSTRING>
{
public:
  // 把字符串转换成字符串链表, 以 \r\n 做分隔
  void FromStringToList(const TCHAR *pString);

  // 把字符串转换成字符串链表, 以 pSeparator 中的字符做分隔
  void FromStringToList(const TCHAR *pString, const TCHAR *pSeparator);

  // 把字符串链表转换成字符串, 以 pSeparator 做分隔
  void FromListToString(MtSTRING &Str, const TCHAR *pSeparator = _T("\r\n")) const;

  // 读取文件到字符串链表
  bool LoadFromFile(const TCHAR *pFileName);

  // 把字符串链表的内容保存到文件
  bool SaveToFile(const TCHAR *pFileName) const;

  // 删除链表中所有的空字符串节点
  void RemoveNullString();

  // 自动换行.  注意：对 Unicode 版本可能有问题，有待改进
  void AutoBreakLineStr(const TCHAR *pString, int MaxLineLength);
};

typedef StrLIST::POSITION StrListPOS;

}
#endif // __STRLIST_H__