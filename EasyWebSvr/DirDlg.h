// 选择目录对话框
// wbj 2002.12.16
// wbj 2003.10.9
// wbj 2004.2.6

namespace minilib
{

class CDirdlg
{
public:
  CDirdlg();
  virtual ~CDirdlg();

  BOOL DoModal(HWND hWndParent,                 // 父窗口句柄
               const TCHAR *pIniSelDir = NULL,  // 初始选中的目录(绝对路径)
               const TCHAR *pDlgTitle = NULL    // 提示标题
              );

  const TCHAR *GetDirPathName() { return PathName; }

protected:
  TCHAR  PathName[MAX_PATH];  // 存放返回的路径信息
  TCHAR  IniSelDir[MAX_PATH]; // 初始选中的目录名

  static int CALLBACK BrowseCtrlCallback(HWND, UINT, LPARAM, LPARAM);
};

}