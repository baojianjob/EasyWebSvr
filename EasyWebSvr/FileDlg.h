#ifndef __FILEDLG_H__
#define __FILEDLG_H__

// 打开/保存文件对话框
// wbj 2002.12.10
// wbj 2003.6.12
// wbj 2003.8.7

namespace minilib
{

class CFileDlg
{
public:
  CFileDlg(bool bOpenFile, HWND hParentWnd, const TCHAR *pFilter, 
    const TCHAR *pDefExt, DWORD FilterIndex = 1, const TCHAR *pInitFileName = (TCHAR *)_T(""), 
    DWORD Flags = 0);
  ~CFileDlg();

  bool          DoModal();
  const TCHAR  *GetFileName() const;
  const TCHAR  *GetPathFileName() const;

protected:
  bool          bOpenFileDlg;            // TRUE for file open, FALSE for file save
  TCHAR         FileTitle[64];           // contains file title after return
  TCHAR         FilePathName[_MAX_PATH]; // contains full path name after return

  OPENFILENAME  Ofn;
};

}
/* Example:

  const char *pFilter = "文本文件(*.txt)\0*.txt\0图片文件\0*.bmp;*.jpg\0所有文件(*.*)\0*.*\0"
  CFileDlg FileDlg(false, m_hWnd, pFilter, "txt");
  if(FileDlg.DoModal())
  {
    FileDlg.GetPathFileName();
  }
*/
#endif // __FILEDLG_H__