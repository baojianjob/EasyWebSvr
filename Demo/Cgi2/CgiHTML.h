
#if !defined(CGIHTML_H____INCLUDED_)
#define CGIHTML_H____INCLUDED_

class CgiHTML  
{
public:
    void TableData(char *Content = "&nbsp", char *Align = "Center", char *VAlign = "Center",int Height = 1);
    void EndTableRow();
    void BeginTableRow(char *BGColor = "#B03A00" ,int Height = 20);
    void EndTable();
    void BeginTable(char *Width = "100%", char *BGColor = "#FFFFFF", char *BorderColor = "#FFFFFF");
    void TextArea(char *Name, int Rows, int Cols, char *Wrap = "VIRTUAL");
    void File(char *Name, char *Value, int Size);
    void Hidden(char *Name, char *Value);
    void NewLine();
    void Reset(char *Name, char *Value, char *Fun, int Size);
    void EndForm();
    void BeginForm(char *Method, char *Action, char *Name);
    void Submit(char *Name, char *Value, char *Fun, int Size);
    void CheckBox(char *Name, char *Value, bool bCheck);
    void Radio(char *Name, char *Title, bool bCheck);
    void Edit(char *Name, char *Value, char *Title, int Size);
    void Password(char *Name, char *Value, char *Title, int Size);
    void Button(char *Name, char *Value, char *Fun, int Size);
    void EndHtml();
    void BeginHtml(const char *Title );
    CgiHTML();
    virtual ~CgiHTML();
};
#endif // !defined(CGIHTML_H____INCLUDED_)
