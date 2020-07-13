//***********************************************
// 文件名称：tlist.h
// 功　　能：链表模板
// 作　　者：wbj
// 创建日期：2002-12-10
// 更新日期：2007-02-01
//***********************************************
/*
更新历史:

2003.10.25 增加 RemoveValue 函数
2004.12.2  修改 SortQuick 函数的一个Bug
2004.12.2  增加 CuteNode 函数
2004.12.3  增加 RemoveSame 函数
2004.12.7  增加 SortInsert 函数
           增加 AddHead, InsertBefore, InsertAfter等protected类型函数
2006-05-29 修正了在Vc2003下编译不能通过的Bug
2006-12-12 增加回调方式排序插入InsertSorted函数
2007-02-01 把pNext、pPrev成员改为私有，提供GetNext()和GetPrev()公有函数
*/
#ifndef __TLIST_H__
#define __TLIST_H__

namespace minilib
{

template <class ARG_TYPE>
class TLIST
{
public:
  class NODE
  {
  public:
    ARG_TYPE    Data;

  private:
    NODE       *pNext;
    NODE       *pPrev;
    friend class TLIST<ARG_TYPE>;
  
  public:
    NODE(const ARG_TYPE &NodeData) : Data(NodeData), pNext(NULL), pPrev(NULL) {};
    NODE       *GetNext() { return pNext; }
    NODE       *GetPrev() { return pPrev; }
  };
  typedef NODE * POSITION;

protected:
  NODE         *pHead;     // 链表头节点指针
  NODE         *pTail;     // 链表尾节点指针
  int           NodeCount; // 链表中节点的个数

public:
  TLIST();
  TLIST(const TLIST<ARG_TYPE> &OtherList);
  virtual  ~TLIST();

  const TLIST<ARG_TYPE> &operator =(const TLIST<ARG_TYPE> &OtherList);
  bool                  operator ==(const TLIST<ARG_TYPE> &OtherList);
  
  POSITION              AddHead(const ARG_TYPE &NewElement);
  void                  AddHead(const TLIST &OtherList);
  POSITION              AddTail(const ARG_TYPE &NewElement);
  void                  AddTail(const TLIST &OtherList);
  int                   CopyList(const TLIST &OtherList, int BeginIndex, int Count);

  const ARG_TYPE       &GetNext(POSITION &Position) const;
  const ARG_TYPE       &GetPrev(POSITION &Position) const;

  const ARG_TYPE       &GetAt(POSITION Position) const;
  const ARG_TYPE       &GetAt(int Index) const;
  void                  SetAt(POSITION Position, const ARG_TYPE &NewElement);
  void                  SetAt(int Index, const ARG_TYPE &NewElement);

  POSITION              GetPosFromIndex(int Index) const;
  int                   GetIndexFromPos(POSITION Position) const;

  const ARG_TYPE       &GetHead() const;
  const ARG_TYPE       &GetTail() const;
  POSITION              GetHeadPosition() const;
  POSITION              GetTailPosition() const;

  POSITION              InsertAfter(POSITION Position, const ARG_TYPE &NewElement);
  void                  InsertAfter(POSITION Position, const TLIST &OtherList);
  POSITION              InsertBefore(POSITION Position, const ARG_TYPE &NewElement);
  void                  InsertBefore(POSITION Position, const TLIST &OtherList);
  POSITION              InsertSorted(const ARG_TYPE &NewElement, bool bAsc);
  POSITION              InsertSorted(const ARG_TYPE &NewElement, int (*CompareFunc)(const ARG_TYPE &Data1, const ARG_TYPE &Data2));

  int                   GetCount() const;
  bool                  IsEmpty() const;

  void                  RemoveAll();
  void                  RemoveAt(POSITION Position);
  void                  RemoveAt(POSITION &Position, int Count);
  int                   RemoveValue(const ARG_TYPE &Value);
  int                   RemoveSame();
  ARG_TYPE              RemoveHead();  
  ARG_TYPE              RemoveTail();

  void                  CutNode(TLIST<ARG_TYPE> &OtherList, POSITION &Position, int Count);

  POSITION              Find(const ARG_TYPE &SearchValue, POSITION StartAfter = NULL) const;
  POSITION              ReverseFind(const ARG_TYPE &SearchValue) const;

  int                   CutSameGroup(TLIST &GroupList, int (*CompareFunc)(const ARG_TYPE &Data1, const ARG_TYPE &Data2));
  void                  SortQuick(POSITION LowPos, POSITION HighPosint, int (*CompareFunc)(const ARG_TYPE &Data1, const ARG_TYPE &Data2));
  void                  SortQuick(int (*CompareFunc)(const ARG_TYPE &Data1, const ARG_TYPE &Data2));  // 快速排序
  void                  SortInsert(int (*CompareFunc)(const ARG_TYPE &Data1, const ARG_TYPE &Data2)); // 插入排序
  void                  Swap(POSITION Pos1, POSITION Pos2);

#ifdef __BASEIO_H__
  bool                  Save(BaseIO *pIoStream) const;
  bool                  Load(BaseIO *pIoStream);
#endif // __BASEIO_H__

#ifdef _DEBUG
  bool                  CheckList() const;
#endif // _DEBUG

protected:
  POSITION              CutAt(POSITION Position);
  POSITION              AddHead(NODE *pNewNode);
  POSITION              AddTail(NODE *pNewNode);
  POSITION              InsertBefore(POSITION Position, NODE *pNewNode);
  POSITION              InsertAfter(POSITION Position, NODE *pNewNode);
  POSITION              InsertSorted(NODE *pNewNode, int (*CompareFunc)(const ARG_TYPE &Data1, const ARG_TYPE &Data2));
};

template <class ARG_TYPE>
TLIST<ARG_TYPE>::TLIST()
{
  pHead = pTail = NULL;
  NodeCount = 0;
}

template <class ARG_TYPE>
TLIST<ARG_TYPE>::TLIST(const TLIST<ARG_TYPE> &OtherList)
{
  pHead = pTail = NULL;
  NodeCount = 0;
  *this = OtherList;
}

template <class ARG_TYPE>
TLIST<ARG_TYPE>::~TLIST()
{
  RemoveAll();
}

template <class ARG_TYPE>
const TLIST<ARG_TYPE> &TLIST<ARG_TYPE>::operator =(const TLIST<ARG_TYPE> &OtherList)
{
  if(&OtherList != this)
  {
    RemoveAll();
    AddTail(OtherList);
  }
  return *this;
}

template <class ARG_TYPE>
bool TLIST<ARG_TYPE>::operator ==(const TLIST<ARG_TYPE> &OtherList)
{
  if(GetCount() != OtherList.GetCount())
    return false;
  NODE *pNode1 = this->pHead;
  NODE *pNode2 = OtherList.pHead;
  for(; pNode1 != NULL; pNode1 = pNode1->pNext, pNode2 = pNode2->pNext)
  {
    assert(pNode2 != NULL);
    if(!(pNode1->Data == pNode2->Data))
      return false;
  }
  assert(pNode1 == NULL && pNode2 == NULL);
  return true;
}

template <class ARG_TYPE>
inline typename TLIST<ARG_TYPE>::POSITION TLIST<ARG_TYPE>::AddHead(const ARG_TYPE &NewElement)
{
  return AddHead(new NODE(NewElement));
}

template <class ARG_TYPE>
typename TLIST<ARG_TYPE>::POSITION TLIST<ARG_TYPE>::AddHead(NODE *pNewNode) // protected
{
  assert(pNewNode != NULL);
  pNewNode->pPrev = NULL;

  if(pHead == NULL)
  {
    pNewNode->pNext = NULL;
    pHead = pTail = pNewNode;
  }
  else
  {
    pNewNode->pNext = pHead;
    pHead->pPrev = pNewNode;
    pHead = pNewNode;
  }

  NodeCount++;
  return pNewNode;
}

template <class ARG_TYPE>
void TLIST<ARG_TYPE>::AddHead(const TLIST &OtherList)
{
  POSITION Pos = OtherList.GetTailPosition();
  while(Pos)
  {
    AddHead(Pos->Data);
    Pos = Pos->pPrev;
  }
}
  
template <class ARG_TYPE>
inline typename TLIST<ARG_TYPE>::POSITION TLIST<ARG_TYPE>::AddTail(const ARG_TYPE &NewElement)
{
  return AddTail(new NODE(NewElement));
}

template <class ARG_TYPE>
typename TLIST<ARG_TYPE>::POSITION TLIST<ARG_TYPE>::AddTail(NODE *pNewNode) // protected
{
  assert(pNewNode != NULL);
  pNewNode->pPrev = pNewNode->pNext = NULL;

  if(pTail == NULL)
    pHead = pTail = pNewNode;
  else
  {
    pNewNode->pPrev = pTail;
    pTail->pNext = pNewNode;
    pTail = pNewNode;
  }

  NodeCount++;
  return pNewNode;
}

template <class ARG_TYPE>
inline typename TLIST<ARG_TYPE>::POSITION TLIST<ARG_TYPE>::InsertSorted(const ARG_TYPE &NewElement, int (*CompareFunc)(const ARG_TYPE &Data1, const ARG_TYPE &Data2))
{
  return InsertSorted(new NODE(NewElement), CompareFunc);
}

template <class ARG_TYPE>
typename TLIST<ARG_TYPE>::POSITION TLIST<ARG_TYPE>::InsertSorted(NODE *pNewNode, int (*CompareFunc)(const ARG_TYPE &Data1, const ARG_TYPE &Data2)) // protected
{
  assert(pNewNode != NULL);

  POSITION Pos = pHead;
  for(; Pos != NULL; Pos = Pos->pNext)
  {
    if(CompareFunc(pNewNode->Data, Pos->Data) < 0)
      break;
  }
  if(Pos == NULL)
    return AddTail(pNewNode);
  return InsertBefore(Pos, pNewNode);
}

template <class ARG_TYPE>
void TLIST<ARG_TYPE>::AddTail(const TLIST &OtherList)
{
  POSITION Pos = OtherList.GetHeadPosition();
  while(Pos != NULL)
  {
    AddTail(Pos->Data);
    Pos = Pos->pNext;
  }
}

// 拷贝 OtherList 从下标 BeginIndex 开始的 Count 个节点
// 返回值: 成功拷贝的节点个数
template <class ARG_TYPE>
int TLIST<ARG_TYPE>::CopyList(const TLIST &OtherList, int BeginIndex, int Count)
{
  assert(Count > 0);
  RemoveAll();

  POSITION Pos = OtherList.GetPosFromIndex(BeginIndex);
  assert(Pos != NULL);
  for(int i = 0; Pos != NULL && i < Count; i++, Pos = Pos->pNext)
    AddTail(Pos->Data);

  return GetCount();
}

template <class ARG_TYPE>
inline const ARG_TYPE &TLIST<ARG_TYPE>::GetNext(POSITION &Position) const
{
  assert(Position != NULL);

  POSITION Pos = Position;
  Position = Pos->pNext;
  return Pos->Data;
}

template <class ARG_TYPE>
inline const ARG_TYPE &TLIST<ARG_TYPE>::GetPrev(POSITION &Position) const
{
  assert(Position != NULL);

  POSITION Pos = Position;
  Position = Pos->pPrev;
  return Pos->Data;
}

template <class ARG_TYPE>
const ARG_TYPE &TLIST<ARG_TYPE>::GetAt(POSITION Position) const
{
  assert(Position != NULL);
  return Position->Data;
}

template <class ARG_TYPE>
inline const ARG_TYPE &TLIST<ARG_TYPE>::GetAt(int Index) const
{
  assert(Index >= 0 && Index < NodeCount);
  
  NODE *pNode = GetPosFromIndex(Index);
  assert(pNode);
 
  return pNode->Data;
}

template <class ARG_TYPE>
void TLIST<ARG_TYPE>::SetAt(int Index, const ARG_TYPE &NewElement)
{
  assert(Index >= 0 && Index < NodeCount);
  
  NODE *pNode = GetPosFromIndex(Index);

  if(pNode == NULL)
    return;
  
  pNode->Data = NewElement;
}

template <class ARG_TYPE>
inline void TLIST<ARG_TYPE>::SetAt(POSITION Position, const ARG_TYPE &NewElement)
{
  if(pNode == NULL)
    return;

  Position->Data = NewElement;
}

template <class ARG_TYPE>
inline const ARG_TYPE &TLIST<ARG_TYPE>::GetHead() const
{
  assert(pHead);

  return pHead->Data;
}

template <class ARG_TYPE>
inline const ARG_TYPE &TLIST<ARG_TYPE>::GetTail() const
{
  assert(pTail);

  return pTail->Data;
}

template <class ARG_TYPE>
inline typename TLIST<ARG_TYPE>::POSITION TLIST<ARG_TYPE>::GetHeadPosition() const
{
  return pHead;
}

template <class ARG_TYPE>
inline typename TLIST<ARG_TYPE>::POSITION TLIST<ARG_TYPE>::GetTailPosition() const
{
  return pTail;
}

template <class ARG_TYPE>
inline typename TLIST<ARG_TYPE>::POSITION TLIST<ARG_TYPE>::InsertAfter(POSITION Position, const ARG_TYPE &NewElement)
{
  return InsertAfter(Position, new NODE(NewElement));
}

template <class ARG_TYPE>
typename TLIST<ARG_TYPE>::POSITION TLIST<ARG_TYPE>::InsertAfter(POSITION Position, NODE *pNewNode) // protected
{
  assert(pNewNode != NULL);
  if(Position == NULL || Position == pTail || IsEmpty())
    return AddTail(pNewNode);

  pNewNode->pPrev = Position;
  pNewNode->pNext = Position->pNext;
  Position->pNext->pPrev = pNewNode;
  Position->pNext = pNewNode;

  NodeCount++;
  return pNewNode;
}

template <class ARG_TYPE>
void TLIST<ARG_TYPE>::InsertAfter(POSITION Position, const TLIST &OtherList)
{
  if(Position == NULL || Position == pTail || IsEmpty())
  {
    AddTail(OtherList);
    return;
  }

  POSITION OtherPos = OtherList.GetHeadPosition();
  while(OtherPos != NULL)
  {
    Position = InsertAfter(Position, OtherPos->Data);
    OtherPos = OtherPos->pNext;
  }
}

template <class ARG_TYPE>
inline typename TLIST<ARG_TYPE>::POSITION TLIST<ARG_TYPE>::InsertBefore(POSITION Position, const ARG_TYPE &NewElement)
{
  return InsertBefore(Position, new NODE(NewElement));
}

template <class ARG_TYPE>
typename TLIST<ARG_TYPE>::POSITION TLIST<ARG_TYPE>::InsertBefore(POSITION Position, NODE *pNewNode) // protected
{
  assert(pNewNode != NULL);
  if(Position == NULL || Position == pHead || IsEmpty())
    return AddHead(pNewNode);

  pNewNode->pNext = Position;
  pNewNode->pPrev = Position->pPrev;
  Position->pPrev->pNext = pNewNode;
  Position->pPrev = pNewNode;

  NodeCount++;
  return pNewNode;
}

template <class ARG_TYPE>
void TLIST<ARG_TYPE>::InsertBefore(POSITION Position, const TLIST &OtherList)
{
  if(Position == NULL || Position == pHead || IsEmpty())
  {
    AddHead(OtherList);
    return;
  }

  POSITION OtherPos = OtherList.GetTailPosition();
  while(OtherPos != NULL)
  {
    Position = InsertBefore(Position, OtherPos->Data);
    OtherPos = OtherPos->pPrev;
  }
}

template <class ARG_TYPE>
typename TLIST<ARG_TYPE>::POSITION TLIST<ARG_TYPE>::InsertSorted(const ARG_TYPE &NewElement, bool bAsc)
{
  POSITION Pos = pHead;
  while(Pos)
  {
    if(bAsc == (NewElement < Pos->Data))
      break;
    Pos = Pos->pNext;
  }
  if(Pos == NULL)
    return AddTail(NewElement);
  return InsertBefore(Pos, NewElement);
}

template <class ARG_TYPE>
inline int TLIST<ARG_TYPE>::GetCount() const
{
  return NodeCount;
}

template <class ARG_TYPE>
inline bool TLIST<ARG_TYPE>::IsEmpty() const
{
  return NodeCount == 0 ? true : false;
}

template <class ARG_TYPE>
void TLIST<ARG_TYPE>::RemoveAll()
{
  while(pHead)
  {
    NODE *pNode = pHead->pNext;
    delete pHead;
    pHead = pNode;
  }

  pHead = pTail = NULL;
  NodeCount = 0;
}

template <class ARG_TYPE>
typename TLIST<ARG_TYPE>::POSITION TLIST<ARG_TYPE>::CutAt(POSITION Position)
{
  if(IsEmpty())     // 链表为空
    return NULL;

  if(Position == pHead || Position == pTail)
  {
    if(pHead == pTail)           //  只有一个节点
      pHead = pTail = NULL;
    else if(Position == pHead)   //   删除的是头节点
    {
      pHead = pHead->pNext;
      pHead->pPrev = NULL;
    }
    else                         //   删除的是尾节点
    {
      pTail = pTail->pPrev;
      pTail->pNext = NULL;
    }
  }
  else   //   删除的是中间节点
  {
    Position->pPrev->pNext = Position->pNext;
    Position->pNext->pPrev = Position->pPrev;
  }

  NodeCount--;
  assert(NodeCount >= 0);
  return Position;
}

template <class ARG_TYPE>
void TLIST<ARG_TYPE>::CutNode(TLIST<ARG_TYPE> &OtherList, POSITION &Position, int Count)
{
  OtherList.RemoveAll();
  assert(Position);

  for(int i = 0; i < Count && Position != NULL; i++)
  {
    POSITION CutPos = Position;
    Position = Position->pNext;
    OtherList.AddTail(CutAt(CutPos));
  }
}

template <class ARG_TYPE>
inline void TLIST<ARG_TYPE>::RemoveAt(POSITION Position)
{
  assert(Position);
  delete CutAt(Position);
}

template <class ARG_TYPE>
void TLIST<ARG_TYPE>::RemoveAt(POSITION &Position, int Count)
{
  assert(Position != NULL);

  for(int i = 0; i < Count && Position != NULL; i++)
  {
    POSITION DelPos = Position;
    Position = Position->pNext;
    RemoveAt(DelPos);
  }
}

template <class ARG_TYPE>
ARG_TYPE TLIST<ARG_TYPE>::RemoveHead()
{
  assert(pHead);

  ARG_TYPE HeadData = pHead->Data;
  RemoveAt(GetHeadPosition());
  return HeadData;
}

template <class ARG_TYPE>
ARG_TYPE TLIST<ARG_TYPE>::RemoveTail()
{
  assert(pTail);

  ARG_TYPE TailData = pTail->Data;
  RemoveAt(GetTailPosition());
  return TailData;
}

// 删除链表中所有值为 Value 的节点
// 返回成功删除的节点个数
template <class ARG_TYPE>
int TLIST<ARG_TYPE>::RemoveValue(const ARG_TYPE &Value)
{
  int RemoveCount = 0;
  POSITION ListPos = pHead;
  while(ListPos != NULL)
  {
    if(ListPos->Data == Value)
    {
      RemoveAt(ListPos, 1);
      RemoveCount++;
    }
    else
      ListPos = ListPos->pNext;
  }
  return RemoveCount;
}

template <class ARG_TYPE>
typename TLIST<ARG_TYPE>::POSITION TLIST<ARG_TYPE>::GetPosFromIndex(int Index) const
{
  assert(Index >= 0 && Index < NodeCount);
  if(Index < 0 || Index >= NodeCount)
    return NULL;

  NODE *pNode = pHead;
  for(int i = 0; i < Index; i++)
    pNode = pNode->pNext;
  return pNode;
}

template <class ARG_TYPE>
int TLIST<ARG_TYPE>::GetIndexFromPos(POSITION Position) const
{
  int Index = 0;
  for(POSITION Pos = GetHeadPosition(); Pos != NULL; Pos = Pos->pNext, Index++)
    if(Position == Pos)
      return Index;
  return -1;
}

template <class ARG_TYPE>
typename TLIST<ARG_TYPE>::POSITION TLIST<ARG_TYPE>::Find(const ARG_TYPE &SearchValue, POSITION StartAfter) const
{
  if(pHead == NULL)
    return NULL;

  POSITION Pos = (StartAfter == NULL ? pHead : StartAfter->pNext);
  for(; Pos != NULL; Pos = Pos->pNext)
    if(Pos->Data == SearchValue)
      break;

  return Pos;
}

template <class ARG_TYPE>
typename TLIST<ARG_TYPE>::POSITION TLIST<ARG_TYPE>::ReverseFind(const ARG_TYPE &SearchValue) const
{
  POSITION Pos = pTail;
  for(; Pos != NULL; Pos = Pos->pPrev)
    if(Pos->Data == SearchValue)
      break;

  return Pos;
}
/*
typedef TLIST<int> IntLIST;
typedef IntLIST::NODE * IntListPOS;
void PrintIntList(const IntLIST &IntList, TSTRING &ResultStr)
{
  char TempStr[50];

  IntListPOS ListPos = IntList.GetHeadPosition();
  for(; ListPos != NULL; ListPos = ListPos->pNext)
  //IntListPOS ListPos = IntList.GetTailPosition();
  //for(; ListPos != NULL; ListPos = ListPos->pPrev)
  {
    wsprintf(TempStr, "%d, ", ListPos->Data);
    ResultStr += TempStr;
  }
}
*/

// 删除链表中的重复节点
// 返回成功删除的节点个数
template <class ARG_TYPE>
int TLIST<ARG_TYPE>::RemoveSame()
{
  int RemoveCount = 0;
  POSITION ListPos = pHead;
  while(ListPos != NULL)
  {
    POSITION FindPos = ListPos->pNext;
    while(FindPos != NULL)
    {
      if(FindPos->Data == ListPos->Data)
      {
        RemoveAt(FindPos, 1);
        RemoveCount++;
      }
      else
        FindPos = FindPos->pNext;
    }
    ListPos = ListPos->pNext;
  }
  return RemoveCount;
}

// 剪切掉链表中的第一个节点及与第一个节点相同的所有节点，把剪切掉的节点组成一个新的链表 GroupList
template <class ARG_TYPE>
int TLIST<ARG_TYPE>::CutSameGroup(TLIST &GroupList, int (*CompareFunc)(const ARG_TYPE &Data1, const ARG_TYPE &Data2))
{
  GroupList.RemoveAll();
  if(IsEmpty())
    return 0;

  NODE *pFirst = CutAt(GetHeadPosition());
  GroupList.AddTail(pFirst);

  for(NODE *pNode = GetHeadPosition(); pNode != NULL;)
  {
    if(CompareFunc(pFirst->Data, pNode->Data) == 0)
    {
      NODE *pCutNode = pNode;
      pNode = pNode->pNext;
      GroupList.AddTail(CutAt(pCutNode));
    }
    else
     pNode = pNode->pNext;
  }
  assert(CheckList());
  assert(GroupList.CheckList());
  return GroupList.GetCount();
}

// 插入排序
// （插入排序是稳定的）
template <class ARG_TYPE>
inline void TLIST<ARG_TYPE>::SortInsert(int (*CompareFunc)(const ARG_TYPE &Data1, const ARG_TYPE &Data2))
{
  if(GetCount() <= 1)
    return;
  TLIST<ARG_TYPE> SortList;
  while(!IsEmpty())
  {
    NODE *pNode = CutAt(GetHeadPosition());
    SortList.InsertSorted(pNode, CompareFunc);
  }
  assert(pHead == NULL && pTail == NULL);
  pHead = SortList.pHead;
  pTail = SortList.pTail;
  NodeCount = SortList.NodeCount;
  SortList.pHead = SortList.pTail = NULL;
  SortList.NodeCount = 0;
}

// 快速排序
// （快速排序是不稳定的）
template <class ARG_TYPE>
inline void TLIST<ARG_TYPE>::SortQuick(int (*CompareFunc)(const ARG_TYPE &Data1, const ARG_TYPE &Data2))
{
  SortQuick(GetHeadPosition(), GetTailPosition(), CompareFunc);
}

// 快速排序
template <class ARG_TYPE>
void TLIST<ARG_TYPE>::SortQuick(POSITION LowPos, POSITION HighPos, int (*CompareFunc)(const ARG_TYPE &Data1, const ARG_TYPE &Data2))
{
  if(LowPos == HighPos)
    return;

  assert(LowPos != NULL && HighPos != NULL);

  NODE *NewLowPos = LowPos;
  NODE *NewHighPos = LowPos;
  assert(GetIndexFromPos(LowPos) < GetIndexFromPos(HighPos));

  POSITION Pos = LowPos;
  do
  {
    Pos = NewHighPos->pNext;
    if(CompareFunc(Pos->Data, LowPos->Data) < 0)
    {
      NODE *pCutNode = CutAt(Pos);

      pCutNode->pNext = NewLowPos;
      pCutNode->pPrev = NewLowPos->pPrev;
      (NewLowPos->pPrev == NULL ? pHead : NewLowPos->pPrev->pNext) = pCutNode;
      NewLowPos->pPrev = pCutNode;

      NewLowPos = pCutNode;

      NodeCount++;
    }
    else
      NewHighPos = Pos;
  } while(Pos != HighPos);

  if(NewLowPos != LowPos && NewLowPos != LowPos->pPrev)
    SortQuick(NewLowPos, LowPos->pPrev, CompareFunc);
  if(NewHighPos != LowPos && NewHighPos != LowPos->pNext)
    SortQuick(LowPos->pNext, NewHighPos, CompareFunc);
}

template <class ARG_TYPE>
void TLIST<ARG_TYPE>::Swap(POSITION Pos1, POSITION Pos2)
{
  if(Pos1 == Pos2)
    return;

  assert(Pos1 != NULL && Pos2 != NULL);

  if(Pos1->pNext != Pos2)
  {
    // 设置 Pos1 的后一个节点的 pPrev 值
    (Pos1->pNext == NULL ? pTail : Pos1->pNext->pPrev) = Pos2;

    // 设置 Pos2 的前一个节点的 pNext 值
    (Pos2->pPrev == NULL ? pHead : Pos2->pPrev->pNext) = Pos1;
  }

  if(Pos1->pPrev != Pos2)
  {
    // 设置 Pos1 的前一个节点的 pNext 值
    (Pos1->pPrev == NULL ? pHead : Pos1->pPrev->pNext) = Pos2;

    // 设置 Pos2 的后一个节点的 pPrev 值
    (Pos2->pNext == NULL ? pTail : Pos2->pNext->pPrev) = Pos1;
  }

  if(Pos1->pNext == Pos2)
  {
    Pos1->pNext = Pos2->pNext;
    Pos2->pPrev = Pos1->pPrev;

    Pos1->pPrev = Pos2;
    Pos2->pNext = Pos1;
  }
  else if(Pos1->pPrev == Pos2)
  {
    Pos2->pNext = Pos1->pNext;
    Pos1->pPrev = Pos2->pPrev;

    Pos2->pPrev = Pos1;
    Pos1->pNext = Pos2;
  }
  else
  {
    // 交换 Pos1, Pos2 的 pPrev 值
    POSITION TempPos = Pos1->pPrev;
    Pos1->pPrev = Pos2->pPrev;
    Pos2->pPrev = TempPos;

    // 交换 Pos1, Pos2 的 pNext 值
    TempPos = Pos1->pNext;
    Pos1->pNext = Pos2->pNext;
    Pos2->pNext = TempPos;
  }
}

/*
template <class ARG_TYPE>
void TLIST<ARG_TYPE>::Swap(POSITION Pos1, POSITION Pos2)
{
  if(Pos1 == Pos2)
    return;

  assert(Pos1 != NULL && Pos2 != NULL);

  if(Pos1->pNext == Pos2)
  {
    // 设置 Pos1 的前一个节点的 pNext 值
    if(Pos1->pPrev == NULL)
      pHead = Pos2;
    else
      Pos1->pPrev->pNext = Pos2;

    // 设置 Pos2 的后一个节点的 pPrev 值
    if(Pos2->pNext == NULL)
      pTail = Pos1;
    else
      Pos2->pNext->pPrev = Pos1;

    Pos1->pNext = Pos2->pNext;
    Pos2->pPrev = Pos1->pPrev;

    Pos1->pPrev = Pos2;
    Pos2->pNext = Pos1;
  }
  else if(Pos1->pPrev == Pos2)
  {
    // 设置 Pos1 的后一个节点的 pPrev 值
    if(Pos1->pNext == NULL)
      pTail = Pos2;
    else
      Pos1->pNext->pPrev = Pos2;

    // 设置 Pos2 的前一个节点的 pNext 值
    if(Pos2->pPrev == NULL)
      pHead = Pos1;
    else
      Pos2->pPrev->pNext = Pos1;

    Pos2->pNext = Pos1->pNext;
    Pos1->pPrev = Pos2->pPrev;

    Pos2->pPrev = Pos1;
    Pos1->pNext = Pos2;
  }
  else
  {
    // 设置 Pos1 的前一个节点的 pNext 值
    if(Pos1->pPrev == NULL)
      pHead = Pos2;
    else
      Pos1->pPrev->pNext = Pos2;

    // 设置 Pos2 的后一个节点的 pPrev 值
    if(Pos2->pNext == NULL)
      pTail = Pos1;
    else
      Pos2->pNext->pPrev = Pos1;

    // 设置 Pos1 的后一个节点的 pPrev 值
    if(Pos1->pNext == NULL)
      pTail = Pos2;
    else
      Pos1->pNext->pPrev = Pos2;

    // 设置 Pos2 的前一个节点的 pNext 值
    if(Pos2->pPrev == NULL)
      pHead = Pos1;
    else
      Pos2->pPrev->pNext = Pos1;

    // 交换 Pos1, Pos2 的 pPrev 值
    POSITION TempPos = Pos1->pPrev;
    Pos1->pPrev = Pos2->pPrev;
    Pos2->pPrev = TempPos;

    // 交换 Pos1, Pos2 的 pNext 值
    TempPos = Pos1->pNext;
    Pos1->pNext = Pos2->pNext;
    Pos2->pNext = TempPos;
  }
}
*/

#ifdef __BASEIO_H__

// 把整个链表保存到 pIoStream 数据流中
template <class ARG_TYPE>
bool TLIST<ARG_TYPE>::Save(BaseIO *pIoStream) const
{
  assert(pIoStream != NULL);
  if(pIoStream == NULL)
    return false;

  if(!pIoStream->WriteValue(NodeCount))
    return false;

  for(POSITION Pos = pHead; Pos != NULL; Pos = Pos->pNext)
    if(!pIoStream->WriteValue(Pos->Data))
      return false;
  return true;
}

// 从 pIoStream 数据流中加载整个链表
template <class ARG_TYPE>
bool TLIST<ARG_TYPE>::Load(BaseIO *pIoStream)
{
  assert(pIoStream != NULL);
  if(pIoStream == NULL)
    return false;

  RemoveAll();

  int Count = 0;
  if(!pIoStream->ReadValue(Count))
    return false;

  for(int i = 0; i < Count; i++)
  {
    NODE *pNewNode = new NODE(ARG_TYPE());
    if(!pIoStream->ReadValue(pNewNode->Data))
    {
      delete pNewNode;
      return false;
    }
    AddTail(pNewNode);
  }
  return true;
}
#endif // __BASEIO_H__

#ifdef _DEBUG

// 用于调试一个算法时, 判断链表的结构还是否正确
template <class ARG_TYPE>
bool TLIST<ARG_TYPE>::CheckList() const
{
  if(pHead == NULL)
    return (pTail == NULL && NodeCount == 0);

  if(pTail == NULL)
    return (pHead == NULL && NodeCount == 0);

  if(pHead->pPrev != NULL)
    return false;
  if(pTail->pNext != NULL)
    return false;

  int Count = 0;
  NODE *pNode;
  for(pNode = pHead; pNode != NULL; pNode = pNode->pNext)
    Count++;
  if(Count != GetCount())
    return false;

  Count = 0;
  for(pNode = pTail; pNode != NULL; pNode = pNode->pPrev)
    Count++;
  if(Count != GetCount())
    return false;
  return true;
}

#endif // _DEBUG

}
#endif // __TLIST_H__