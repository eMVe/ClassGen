// ClassGen.cpp (c) Michal Vanka
//

/*
 Note: for -m option:
  - add something like this to .rc file:

/////////////////////////////////////////////////////////////////////////////
//
// Menu
//

IDR_MENU1 MENU
BEGIN
    POPUP "TrayMenu"
    BEGIN
        MENUITEM "Config",                      ID_TRAYMENU_CONFIG
        MENUITEM "Exit",                        ID_TRAYMENU_EXIT
    END
END

	- add appropriate #defines to resource.h, for instance:

#define IDR_MENU1						32000

#define ID_TRAYMENU_CONFIG              32771
#define ID_TRAYMENU_EXIT                32772
#define ID_TRAYMENU_ITEM3               32773

 */

#include "stdafx.h"

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

#define BASE_DIALOG "CDialog"
#define DIVIDER "/*----------------------------------------------------------*/"


string getCommandLine(int argc, char *argv[])
{
	std::string rv;
	for (int i = 0; i < argc; i++) {
		if (i) rv += " ";
		rv += argv[i];
	}
	return rv;
}
void generateThreadH(ofstream &ofs, std::string & className)
{
	ofs << "\tHANDLE m_mutex;" << endl
		<< "\tunsigned int m_threadHandle;" << endl
		<< "\tvolatile bool m_bThreadRunning;" << endl
		<< endl
		<< "\tvoid threadStart();" << endl
		<< "\tstatic unsigned int __stdcall threadLauncher(void *p_this);" << endl
		<< "\tvoid thread();" << endl
		;
}

void generateThreadCPP(ofstream &ofs, std::string & className)
{
	ofs << DIVIDER << endl
		<< "void " << className << "::threadStart()" << endl
		<< "{" << endl
		<< "\tif (!m_bThreadRunning) {" << endl
		<< "\t\tm_bThreadRunning = true;" << endl
		<< "\t\tm_threadHandle = _beginthreadex(0, 0, &" << className << "::threadLauncher, this, 0, 0);" << endl
		<< "\t}" << endl
		<< "}" << endl
		<< endl

		<< DIVIDER << endl
		<< "unsigned int " << className << "::threadLauncher(void *p_this)" << endl
		<< "{" << endl
		<< "\t" << className << " *p = static_cast<" << className << "*>(p_this);" << endl
		<< "\tp->thread();" << endl
		<< "\treturn 0;" << endl
		<< "}" << endl
		<< endl

		<< DIVIDER << endl
		<< "void " << className << "::thread()" << endl
		<< "{" << endl
		<< "//\tSendMessage(DATA_READY, SYS_MESSAGE, (LPARAM)m_replyList[1].c_str());" << endl
		<< "\tm_bThreadRunning=false;" << endl
		<< "}" << endl
		<< endl
		;
}

bool isArgPresent(const char *arg, int argc, char* argv[])
{
	for (int i = 1; i < argc; i++) {
		if (!strcmp(arg, argv[i])) return true;
	}
	return false;
}

int getArgIndex(const char *arg, int argc, char* argv[])
{
	int i;
	for (int i = 1; i < argc; i++) {
		if (!strcmp(arg, argv[i])) return i;
	}
	return -1;
}

void generateSimple(const char *classNameL, int argc, char *argv[])
{
	std::string className(classNameL);
	
	ofstream ofs(className + string(".h"),std::ofstream::binary | std::ofstream::trunc);
	if (!ofs.is_open()) return;

	ofs << "#pragma once" << endl
		<< endl;
		ofs<< "class " << className << endl
		<< "{" << endl;
		if (isArgPresent("-t", argc, argv)) {
			generateThreadH(ofs, className);
		}
		ofs << "public:" << endl
		<< "\t" << className << "();" << endl
		<< "\t~" << className << "();" << endl
		<< "};" << endl;

	ofs.close();

	ofs.open(className + string(".cpp"), std::ofstream::binary | std::ofstream::trunc);

	ofs << "#include \"stdafx.h\"" << endl
		<< "#include \"" << className << ".h\"" << endl
		<< endl
		<< DIVIDER << endl
		<< className << "::" << className << "()" << endl;
		if (isArgPresent("-t", argc, argv)) {
			ofs << "\t: m_bThreadRunning(false)" << endl;
		}
		ofs << "{" << endl;
		ofs << "}" << endl
		<< endl
		<< DIVIDER << endl
		<< className << "::~" << className << "()" << endl
		<< "{" << endl << "}" << endl
		<< endl;
		if (isArgPresent("-t", argc, argv)) {
			generateThreadCPP(ofs, className);
		}
	ofs.close();

}

void generateDialog(int argc, char* argv[])
{
	const char *classNameL = argv[1];

	std::string className(classNameL);
	std::string dialogIdd = "IDD_TODO";

	ofstream ofs(className + string(".h"),std::ofstream::binary | std::ofstream::trunc);
	if (!ofs.is_open()) return;

	if (isArgPresent("-i", argc, argv)) {
		int idx = getArgIndex("-i", argc, argv);
		if (idx + 1 < argc) {
			dialogIdd = ((char*)argv[idx + 1]);
		}
	}

	ofs << "#pragma once" << endl
		<< endl
		;
	if (isArgPresent("-e", argc, argv)) {
		ofs << "#include \"easysize.h\"" << endl;
		ofs << endl;
	}
	if (isArgPresent("-t", argc, argv)) {

		ofs << "#define DATA_READY (WM_USER + 110)" << endl
			<< endl
			;
	}

	if (isArgPresent("-m", argc, argv)) {
		ofs << "#define WM_TRAY_ICON_NOTIFY_MESSAGE (WM_USER + 111)" << endl
			<< endl
			;
	}

	ofs << "class " << className << " : public " << BASE_DIALOG << endl
		<< "{" << endl
		;

	if (isArgPresent("-t", argc, argv)) {

		ofs << "//\tenum {" << endl
			<< "//\t\tSYS_MESSAGE," << endl
			<< "//\t\tDATA_MESSAGE" << endl
			<< "//\t};" << endl
			;
	}

	if (isArgPresent("-m", argc, argv)) {

		ofs << "\tBOOL		m_bMinimizeToTray;" << endl
			<< endl
	
			<< "\tBOOL			m_bTrayIconVisible;" << endl
			<< "\tNOTIFYICONDATA	m_nidIconData;" << endl
			<< "\tCMenu			m_mnuTrayMenu;" << endl
			<< "\tUINT			m_nDefaultMenuItem;" << endl
			;
	}

	ofs	<< "\tCString m_fontFaceName;" << endl
		<< "\tint	m_fontSize;" << endl
		<< "\tLONG m_fontWeight;" << endl
		<< "\tCFont m_font;" << endl;

	if (isArgPresent("-e", argc, argv)) {
		ofs << endl;
		ofs << "\tDECLARE_EASYSIZE;" << endl;
		ofs << "\tafx_msg void OnSize(UINT nType, int cx, int cy);" << endl;
	}

	if (isArgPresent("-t", argc, argv)) {
		ofs << endl;
		generateThreadH(ofs, className);
	}

	if (isArgPresent("-m", argc, argv)) {

		ofs << "\tvoid TraySetMinimizeToTray(BOOL bMinimizeToTray = TRUE);" << endl
			<< "\tBOOL TraySetMenu(UINT nResourceID, UINT nDefaultPos = 0);" << endl
			<< "\tBOOL TraySetMenu(HMENU hMenu, UINT nDefaultPos = 0);" << endl
			<< "\tBOOL TraySetMenu(LPCTSTR lpszMenuName, UINT nDefaultPos = 0);" << endl
			<< "\tBOOL TrayUpdate();" << endl
			<< "\tBOOL TrayShow();" << endl
			<< "\tBOOL TrayHide();" << endl
			<< "\tvoid TraySetToolTip(LPCTSTR lpszToolTip);" << endl
			<< "\tvoid TraySetIcon(HICON hIcon);" << endl
			<< "\tvoid TraySetIcon(UINT nResourceID);" << endl
			<< "\tvoid TraySetIcon(LPCTSTR lpszResourceName);" << endl
			<< "\tBOOL TrayIsVisible();" << endl
			;
	}

	ofs << endl
		<< "protected:" << endl
		<< "\tvoid DoDataExchange(CDataExchange* pDX);" << endl
		<< "\tBOOL PreTranslateMessage (MSG* pMsg);" << endl
		<< "\tBOOL OnInitDialog();" << endl
		<< "\tvoid OnCancel();" << endl
		<< endl
		<< "\tDECLARE_MESSAGE_MAP()" << endl
		<< "\tafx_msg void OnClose();" << endl
		<< "\tafx_msg void OnOK();" << endl
		;
	if (isArgPresent("-t", argc, argv)) {
		ofs << "\tafx_msg LRESULT OnDataReady(WPARAM wParam, LPARAM lParam);" << endl
			;
	}

	if (isArgPresent("-m", argc, argv)) {
		ofs << "\tafx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);" << endl
			<< "\tafx_msg void OnDestroy();" << endl
			<< "\tafx_msg void OnSysCommand(UINT nID, LPARAM lParam);" << endl
			<< "\tafx_msg LRESULT OnTrayNotify(WPARAM wParam, LPARAM lParam);" << endl
			<< "\tafx_msg void OnTraymenuConfig();" << endl
			<< "\tafx_msg void OnTraymenuExit();" << endl
			;
	}
	ofs << "public:" << endl

		<< "// Dialog Data" << endl
		<< "#ifdef AFX_DESIGN_TIME" << endl
		<< "\tenum { IDD = " << dialogIdd << " };" << endl
		<< "#endif" << endl
		;

	if (isArgPresent("-m", argc, argv)) {
		ofs << "\tvirtual void OnTrayLButtonDown(CPoint pt);" << endl
			<< "\tvirtual void OnTrayLButtonUp(CPoint pt);" << endl
			<< "\tvirtual void OnTrayLButtonDblClk(CPoint pt);" << endl
			<< "\tvirtual void OnTrayRButtonDown(CPoint pt);" << endl
			<< "\tvirtual void OnTrayRButtonDblClk(CPoint pt);" << endl
			<< "\tvirtual void OnTrayMouseMove(CPoint pt);" << endl
			<< endl
			;
	}

	ofs << "\tvoid presetFont(const char *fontFaceName, int size, int weight);" << endl
		<< "\tvirtual INT_PTR DoModal();" << endl
		<< "\t" << className << "(CWnd* pParent = NULL);" << endl
		<< "\t~" << className << "();" << endl
		<< "};" << endl
		;

	ofs.close();

	ofs.open(className + string(".cpp"), std::ofstream::binary | std::ofstream::trunc);

	ofs << "//Automatically generated by ClassGen [" << getCommandLine(argc,argv) << "]" << endl
		<< endl
		<< "#include \"stdafx.h\"" << endl
		<< "#include <afxpriv.h>" << endl
		<< "#include \"" << className << ".h\"" << endl
		<< "#include \"resource.h\"" << endl
		<< endl

		<< "BEGIN_MESSAGE_MAP(" << className << ", " << BASE_DIALOG << ")" << endl
		<< "\tON_WM_CLOSE()" << endl;

		if (isArgPresent("-e", argc, argv)) {
			ofs	<< "\tON_WM_SIZE()" << endl;				
		}

		if (isArgPresent("-m", argc, argv)) {
			ofs << "\tON_WM_CREATE()" << endl
				<< "\tON_WM_DESTROY()" << endl
				<< "\tON_WM_SYSCOMMAND()" << endl
				;
		}

		if (isArgPresent("-t", argc, argv)) {
			ofs << "\tON_MESSAGE(DATA_READY, &" << className << "::OnDataReady)" << endl
			;
		}
		if (isArgPresent("-m", argc, argv)) {

			ofs << "\tON_MESSAGE(WM_TRAY_ICON_NOTIFY_MESSAGE, OnTrayNotify)" << endl
				<< "\tON_COMMAND(ID_TRAYMENU_CONFIG, OnTraymenuConfig)" << endl
				<< "\tON_COMMAND(ID_TRAYMENU_EXIT, OnTraymenuExit)" << endl
				;
		}

		ofs << "END_MESSAGE_MAP()" << endl
			<< endl

		<< DIVIDER << endl
		<< className << "::" << className << "(CWnd* pParent) : " << BASE_DIALOG << "(" << dialogIdd << ", pParent)" << endl;
		if (isArgPresent("-t", argc, argv)) {
			ofs << "\t, m_bThreadRunning(false)" << endl;
		}
		ofs << "\t,m_fontWeight(0)" << endl
			<< "\t,m_fontSize(10)" << endl
			<< "{" << endl
			<< "\tm_fontFaceName = \"MS Shell Dlg\";" << endl;

		if (isArgPresent("-m", argc, argv)) {

		ofs << "\tm_nidIconData.cbSize = sizeof(NOTIFYICONDATA);" << endl

		<< "\tm_nidIconData.hWnd = 0;" << endl
		<< "\tm_nidIconData.uID = 1;" << endl

		<< "\tm_nidIconData.uCallbackMessage = WM_TRAY_ICON_NOTIFY_MESSAGE;" << endl

		<< "\tm_nidIconData.hIcon = 0;" << endl
		<< "\tm_nidIconData.szTip[0] = 0;" << endl
		<< "\tm_nidIconData.uFlags = NIF_MESSAGE;" << endl

		<< "\tm_bTrayIconVisible = FALSE;" << endl

		<< "\tm_nDefaultMenuItem = 0;" << endl
		<< "#ifndef _DEBUG" << endl
		<< "\tm_bMinimizeToTray = TRUE;" << endl
		<< "#endif" << endl;

		}

		ofs << "}" << endl
		<< endl

		<< DIVIDER << endl
		<< className << "::~" << className << "()" << endl
		<< "{" << endl << "}" << endl
		<< endl

		<< DIVIDER << endl
		<< "void " << className << "::DoDataExchange(CDataExchange* pDX)" << endl
		<< "{" << endl
		<< "\t" << BASE_DIALOG << "::DoDataExchange(pDX);" << endl
		<< "}" << endl
		<< endl

		<< DIVIDER << endl
		<< "BOOL " << className << "::OnInitDialog()" << endl
		<< "{" << endl
		<< "\t" << BASE_DIALOG << "::OnInitDialog();" << endl
		<< endl
		;

		if (isArgPresent("-m", argc, argv)) {
			ofs << "\tTraySetIcon(IDR_MAINFRAME);" << endl
				<< "\tTraySetToolTip(\"THE PROGRAM\");" << endl
				<< "\tTraySetMenu(IDR_MENU1);" << endl
				;
		}

		ofs << "\tif (m_fontWeight) {" << endl
			<< "\t\tCFont * pFont = GetFont();" << endl
			<< "\t\tLOGFONT lf;" << endl
			<< "\t\tpFont->GetLogFont(&lf);" << endl
			<< "\t\tlf.lfWeight |= m_fontWeight;" << endl
			<< "\t\tm_font.CreateFontIndirect(&lf);" << endl
			<< "\t\tSendMessageToDescendants(WM_SETFONT, (WPARAM)m_font.m_hObject);" << endl
			<< "\t}" << endl
			<< endl
			;

		if (isArgPresent("-e", argc, argv)) {
			ofs << "\tINIT_EASYSIZE;" << endl;
			ofs << endl;
		}

		if (isArgPresent("-m", argc, argv)) {
			ofs << "\t#ifndef _DEBUG" << endl
				<< "\tPostMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);" << endl
				<< "\t#endif" << endl
				;
		}

		ofs << endl
			<< "\treturn true;" << endl
			<< "}" << endl
			<< endl;


		if (isArgPresent("-m", argc, argv)) {
			ofs << DIVIDER << endl
				<< "int " << className << "::OnCreate(LPCREATESTRUCT lpCreateStruct)" << endl
				<< "{" << endl
				<< "\tif (" << BASE_DIALOG << "::OnCreate(lpCreateStruct) == -1)" << endl
				<< "\t\treturn -1;" << endl
				<< endl
				<< "\tm_nidIconData.hWnd = this->m_hWnd;" << endl
				<< "\tm_nidIconData.uID = 1;" << endl

				<< "\treturn 0;" << endl
				<< "}" << endl
				<< endl

				<< DIVIDER << endl
				<< "void " << className << "::OnDestroy()" << endl
				<< "{" << endl
				<< "\t" << BASE_DIALOG << "::OnDestroy();" << endl
				<< "\tif (m_nidIconData.hWnd && m_nidIconData.uID > 0 && TrayIsVisible()) {" << endl
					<< "\t\tShell_NotifyIcon(NIM_DELETE, &m_nidIconData);" << endl
				<< "\t}" << endl
				<< "}" << endl
				<< endl
				<< DIVIDER << endl

			<< "LRESULT DlgMain::OnTrayNotify(WPARAM wParam, LPARAM lParam)" << endl
			<< "{" << endl
				<< "\tUINT uID;" << endl
				<< "\tUINT uMsg;" << endl
				<< endl
				<< "\tuID = (UINT)wParam;" << endl
				<< "\tuMsg = (UINT)lParam;" << endl
				<< endl
				<< "\tif (uID != 1)" << endl
					<< "\t\treturn NULL;" << endl
				<< endl
				<< "\tCPoint pt;" << endl
				<< endl
				<< "\tswitch (uMsg) {" << endl
				<< "\tcase WM_MOUSEMOVE:" << endl
					<< "\t\tGetCursorPos(&pt);" << endl
					<< "\t\tClientToScreen(&pt);" << endl
					<< "\t\tOnTrayMouseMove(pt);" << endl
					<< "\t\tbreak;" << endl
				<< "\tcase WM_LBUTTONDOWN:" << endl
					<< "\t\tGetCursorPos(&pt);" << endl
					<< "\t\tClientToScreen(&pt);" << endl
					<< "\t\tOnTrayLButtonDown(pt);" << endl
					<< "\t\tbreak;" << endl
				<< "\tcase WM_LBUTTONUP:" << endl
					<< "\t\tGetCursorPos(&pt);" << endl
					<< "\t\tClientToScreen(&pt);" << endl
					<< "\t\tOnTrayLButtonUp(pt);" << endl
					<< "\t\tbreak;" << endl
				<< "\tcase WM_LBUTTONDBLCLK:" << endl
					<< "\t\tGetCursorPos(&pt);" << endl
					<< "\t\tClientToScreen(&pt);" << endl
					<< "\t\tOnTrayLButtonDblClk(pt);" << endl
					<< "\t\tbreak;" << endl
				<< endl
				<< "\tcase WM_RBUTTONDOWN:" << endl
				<< "\tcase WM_CONTEXTMENU:" << endl
					<< "\t\tGetCursorPos(&pt);" << endl
					<< "\t\t//ClientToScreen(&pt);" << endl
					<< "\t\tOnTrayRButtonDown(pt);" << endl
					<< "\t\tbreak;" << endl
				<< "\tcase WM_RBUTTONDBLCLK:" << endl
					<< "\t\tGetCursorPos(&pt);" << endl
					<< "\t\tClientToScreen(&pt);" << endl
					<< "\t\tOnTrayRButtonDblClk(pt);" << endl
					<< "\t\tbreak;" << endl
				<< "\t}" << endl
				<< "\treturn NULL;" << endl
			<< "}" << endl
			<< endl
			<< DIVIDER << endl
			<< "void " << className << "::OnSysCommand(UINT nID, LPARAM lParam)" << endl
			<< "{" << endl
				<< "\tif (m_bMinimizeToTray) {" << endl
					<< "\t\tif ((nID & 0xFFF0) == SC_MINIMIZE) {" << endl
						<< "\t\t\tif (TrayShow()) {" << endl
							<< "\t\t\t\tthis->ShowWindow(SW_HIDE);" << endl
					<< "\t\t\t}" << endl
					<< "\t\t}" << endl
					<< "\t\telse {" << endl
						<< "\t\t\t" << BASE_DIALOG << "::OnSysCommand(nID, lParam);" << endl
					<< "\t\t}" << endl
				<< "\t}" << endl
				<< "\telse {" << endl
					<< "\t\t" << BASE_DIALOG << "::OnSysCommand(nID, lParam);" << endl
			<< "\t}" << endl
			<< "}" << endl

			<< endl
			<< DIVIDER << endl
			<< "void " << className << "::OnTrayRButtonDown(CPoint pt)" << endl
			<< "{" << endl
			<< "\tm_mnuTrayMenu.GetSubMenu(0)->TrackPopupMenu(TPM_BOTTOMALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, pt.x, pt.y, this);" << endl
			<< "\tm_mnuTrayMenu.GetSubMenu(0)->SetDefaultItem(m_nDefaultMenuItem, TRUE);" << endl
			<< "}" << endl
			<< endl
			<< DIVIDER << endl
			<< "void " << className << "::OnTrayLButtonUp(CPoint pt)" << endl
			<< "{" << endl
			<< "\tpt = pt;" << endl
			<< "\tif (m_bMinimizeToTray)" << endl
			<< "\t\tif (TrayHide())" << endl
			<< "\t\t\tthis->ShowWindow(SW_SHOW);" << endl
			<< "}" << endl
			<< endl
			<< DIVIDER << endl
			<< "void " << className << "::OnTrayLButtonDblClk(CPoint pt) {" << endl
			<< "\tpt = pt;	//for debug" << endl
			<< "}" << endl
			<< endl
			<< DIVIDER << endl
			<< "void " << className << "::OnTrayRButtonDblClk(CPoint pt) {" << endl
			<< "\tpt = pt;	//for debug" << endl
			<< "}" << endl
			<< endl
			<< DIVIDER << endl
			<< "void " << className << "::OnTrayMouseMove(CPoint pt) {" << endl
			<< "\tpt = pt;	//for debug" << endl
			<< "}" << endl
			<< endl
			<< DIVIDER << endl
			<< "void " << className << "::OnTrayLButtonDown(CPoint pt) {" << endl
			<< "\tpt = pt;	//for debug" << endl
			<< "}" << endl
			<< endl
			<< DIVIDER << endl
			<< "void DlgMain::OnTraymenuConfig()" << endl
			<< "{" << endl
				<< "\tTrayHide();" << endl
				<< "\tShowWindow(SW_RESTORE);" << endl
			<< "}" << endl
			<< endl
			<< DIVIDER << endl
			<< "void DlgMain::OnTraymenuExit()" << endl
			<< "{" << endl
				<< "\tPostMessage(WM_SYSCOMMAND, SC_CLOSE, 0);" << endl
			<< "}" << endl
			;
		}
		
		


		if (isArgPresent("-e", argc, argv)) {
			ofs << DIVIDER << endl
			<< "#pragma warning(disable: 4189)   // local variable is initialized but not referenced" << endl
			<< "BEGIN_EASYSIZE_MAP(" << className << ")" << endl
			<< "\t/* EASYSIZE(IDC_EDITOR, ES_BORDER, ES_BORDER, ES_BORDER, ES_BORDER, 0); */" << endl
			<< "\tEASYSIZE(IDOK, ES_KEEPSIZE, ES_KEEPSIZE, ES_BORDER, ES_BORDER, 0);" << endl
			<< "\tEASYSIZE(IDCANCEL, ES_KEEPSIZE, ES_KEEPSIZE, ES_BORDER, ES_BORDER, 0);" << endl
			<< "\END_EASYSIZE_MAP" << endl
			<< "#pragma warning(default: 4189)" << endl
			<< endl;
		}

		ofs << DIVIDER << endl
			<< "void " << className << "::OnClose()" << endl
			<< "{" << endl
			<< "\t" << BASE_DIALOG << "::OnClose();" << endl
			<< "}" << endl
			<< endl

		<< DIVIDER << endl
			<< "void " << className << "::OnCancel()" << endl
			<< "{" << endl
			<< "\t" << BASE_DIALOG << "::OnCancel();" << endl
			<< "}" << endl
			<< endl

		<< DIVIDER << endl
			<< "void " << className << "::OnOK()" << endl
			<< "{" << endl
			<< "\t" << BASE_DIALOG << "::OnOK();" << endl
			<< "}" << endl
			<< endl
			;

		if (isArgPresent("-e", argc, argv)) {
			ofs << DIVIDER << endl
			<< "void " << className << "::OnSize(UINT nType, int cx, int cy)" << endl
			<< "{" << endl
			<< "\t" << BASE_DIALOG << "::OnSize(nType, cx, cy);" << endl
			<< "\tUPDATE_EASYSIZE;" << endl
			<< "}" << endl;
		}

		if (isArgPresent("-t", argc, argv)) {
			ofs	<< DIVIDER << endl
				<< "LRESULT " << className << "::OnDataReady(WPARAM wParam, LPARAM lParam)" << endl
				<< "{" << endl
				<< "//\tstring s;" << endl
				<< "//\tswitch (wParam) {" << endl
				<< "//\tcase SYS_MESSAGE:" << endl
				<< "//\t\tm_listView.AddString((char*)lParam);" << endl
				<< "//\t\tbreak;" << endl
				<< "//\tcase DATA_MESSAGE:" << endl
				<< "//\t\tm_listView.ResetContent();" << endl
				<< "//\t\ts = string(\"Lot \") + m_lotid.GetString() + string(\" can be processed on:\");" << endl
				<< "//\t\tm_listView.AddString(s.c_str());" << endl
				<< "//\t\tm_listView.AddString((char*)lParam);" << endl
				<< "//\t\tbreak;" << endl
				<< "//\t}" << endl
				<< "//\tGetDlgItem(IDC_ELI_ED_LOTID)->SetFocus();" << endl
				<< "\treturn 0;" << endl
				<< "}" << endl
				<< endl
				;
		}

		if (isArgPresent("-m", argc, argv)) {

			ofs << DIVIDER << endl 
			<< "void " << className << "::TraySetMinimizeToTray(BOOL bMinimizeToTray)" << endl
			<< "{" << endl
			<< "\tm_bMinimizeToTray = bMinimizeToTray;" << endl
			<< "}" << endl
			<< endl

			<< DIVIDER << endl
			<< "BOOL " << className << "::TrayIsVisible()" << endl
			<< "{" << endl
			<< "\treturn m_bTrayIconVisible;" << endl
			<< "}" << endl
			<< endl

			<< DIVIDER << endl
			<< "void " << className << "::TraySetIcon(HICON hIcon)" << endl
			<< "{" << endl
			<< "\tASSERT(hIcon);" << endl
				<< endl
			<< "\tm_nidIconData.hIcon = hIcon;" << endl
			<< "\tm_nidIconData.uFlags |= NIF_ICON;" << endl
			<< "}" << endl
			<< endl

			<< DIVIDER << endl
			<< "void " << className << "::TraySetIcon(UINT nResourceID)" << endl
			<< "{" << endl
				<< "\tASSERT(nResourceID > 0);" << endl
				<< "\tHICON hIcon = 0;" << endl
				<< "\thIcon = AfxGetApp()->LoadIcon(nResourceID);" << endl
				<< "\tif (hIcon) {" << endl
					<< "\t\tm_nidIconData.hIcon = hIcon;" << endl
					<< "\t\tm_nidIconData.uFlags |= NIF_ICON;" << endl
				<< "\t}" << endl
				<< "\telse {" << endl
					<< "\t\tTRACE0(\"FAILED TO LOAD ICON\\n\");" << endl
				<< "\t}" << endl
			<< "}" << endl
			<< endl

			<< DIVIDER << endl
			<< "void " << className << "::TraySetIcon(LPCTSTR lpszResourceName)" << endl
			<< "{" << endl
				<< "\tHICON hIcon = 0;" << endl
				<< "\thIcon = AfxGetApp()->LoadIcon(lpszResourceName);" << endl
				<< "\tif (hIcon) {" << endl
					<< "\t\tm_nidIconData.hIcon = hIcon;" << endl
					<< "\t\tm_nidIconData.uFlags |= NIF_ICON;" << endl
				<< "\t}" << endl
				<< "\telse {" << endl
					<< "\t\tTRACE0(\"FAILED TO LOAD ICON\\n\");" << endl
				<< "\t}" << endl
			<< "}" << endl
			<< endl

			<< DIVIDER << endl
			<< "void " << className << "::TraySetToolTip(LPCTSTR lpszToolTip)" << endl
			<< "{" << endl
				<< "\tASSERT(strlen(lpszToolTip) > 0 && strlen(lpszToolTip) < 64);" << endl

				<< "\tstrcpy(m_nidIconData.szTip, lpszToolTip);" << endl
				<< "\tm_nidIconData.uFlags |= NIF_TIP;" << endl
			<< "}" << endl
			<< endl

			<< DIVIDER << endl
			<< "BOOL " << className << "::TrayShow()" << endl
			<< "{" << endl
				<< "\tBOOL bSuccess = FALSE;" << endl
				<< "\tif (!m_bTrayIconVisible) {" << endl
					<< "\t\tbSuccess = Shell_NotifyIcon(NIM_ADD, &m_nidIconData);" << endl
					<< "\t\tif (bSuccess)" << endl
						<< "\t\t\tm_bTrayIconVisible = TRUE;" << endl
				<< "\t}" << endl
				<< "\telse {" << endl
					<< "\t\tTRACE0(\"ICON ALREADY VISIBLE\");" << endl
				<< "\t}" << endl
				<< "\treturn bSuccess;" << endl
			<< "}" << endl
			<< endl

			<< DIVIDER << endl
			<< "BOOL " << className << "::TrayHide()" << endl
			<< "{" << endl
				<< "\tBOOL bSuccess = FALSE;" << endl
				<< "\tif (m_bTrayIconVisible) {" << endl
					<< "\t\tbSuccess = Shell_NotifyIcon(NIM_DELETE, &m_nidIconData);" << endl
					<< "\t\tif (bSuccess)" << endl
						<< "\t\t\tm_bTrayIconVisible = FALSE;" << endl
				<< "\t}" << endl
				<< "\telse {" << endl
					<< "\t\tTRACE0(\"ICON ALREADY HIDDEN\");" << endl
				<< "\t}" << endl
				<< "\treturn bSuccess;" << endl
			<< "}" << endl
			<< endl

			<< DIVIDER << endl
			<< "BOOL " << className << "::TrayUpdate()" << endl
			<< "{" << endl
				<< "\tBOOL bSuccess = FALSE;" << endl
				<< "\tif (m_bTrayIconVisible) {" << endl
					<< "\t\tbSuccess = Shell_NotifyIcon(NIM_MODIFY, &m_nidIconData);" << endl
				<< "\t}" << endl
				<< "\telse {" << endl
					<< "\t\tTRACE0(\"ICON NOT VISIBLE\");" << endl
				<< "\t}" << endl
				<< "\treturn bSuccess;" << endl
			<< "}" << endl
			<< endl
			<< DIVIDER << endl
			<< "BOOL " << className << "::TraySetMenu(UINT nResourceID, UINT nDefaultPos)" << endl
			<< "{" << endl
				<< "\tnDefaultPos = nDefaultPos;" << endl
				<< "\tBOOL bSuccess;" << endl
				<< "\tbSuccess = m_mnuTrayMenu.LoadMenu(nResourceID);" << endl
				<< "\treturn bSuccess;" << endl
			<< "}" << endl
			<< endl

			<< DIVIDER << endl
			<< "BOOL " << className << "::TraySetMenu(LPCTSTR lpszMenuName, UINT nDefaultPos)" << endl
			<< "{" << endl
				<< "\tnDefaultPos = nDefaultPos;" << endl
				<< "\tBOOL bSuccess;" << endl
				<< "\tbSuccess = m_mnuTrayMenu.LoadMenu(lpszMenuName);" << endl
				<< "\treturn bSuccess;" << endl
			<< "}" << endl
			<< endl

			<< DIVIDER << endl
			<< "BOOL " << className << "::TraySetMenu(HMENU hMenu, UINT nDefaultPos)" << endl
			<< "{" << endl
				<< "\tnDefaultPos = nDefaultPos;" << endl
				<< "\tm_mnuTrayMenu.Attach(hMenu);" << endl
				<< "\treturn TRUE;" << endl
			<< "}" << endl
			<< endl
					;
		}
		ofs << DIVIDER << endl
			<< "BOOL " << className << "::PreTranslateMessage(MSG* pMsg)" << endl
			<< "{" << endl
			<< "\treturn " << BASE_DIALOG << "::PreTranslateMessage(pMsg);" << endl
			<< "}" << endl
			<< endl

			<< DIVIDER << endl
			<< "INT_PTR " << className << "::DoModal()" << endl
			<< "{" << endl
			<< "\tCDialogTemplate dlgTemplate; CString fontFaceName; WORD fontSize; INT_PTR result;" << endl
			<< endl
			<< "\tdlgTemplate.Load(MAKEINTRESOURCE(m_nIDHelp));" << endl
			<< "\tdlgTemplate.GetFont(fontFaceName, fontSize);" << endl
			<< "\tdlgTemplate.SetFont(m_fontFaceName, m_fontSize);" << endl
			<< endl
			<< "\tLPSTR pdata = (LPSTR)GlobalLock(dlgTemplate.m_hTemplate);" << endl
			<< endl
			<< "\tm_lpszTemplateName = NULL;" << endl
			<< "\tInitModalIndirect(pdata);" << endl
			<< endl
			<< "\tresult = " << BASE_DIALOG << "::DoModal();" << endl
			<< endl
			<< "\tGlobalUnlock(dlgTemplate.m_hTemplate);" << endl
			<< endl
			<< "\treturn result;" << endl
			<< "}" << endl
			<< endl
			<< DIVIDER << endl
			<< "void " << className << "::presetFont(const char *fontFaceName, int fontSize, int fontWeight)" << endl
			<< "{" << endl
			<< "\tif (fontFaceName && *fontFaceName) m_fontFaceName = fontFaceName;" << endl
			<< "\tif (fontSize) m_fontSize = fontSize;" << endl
			<< "\tif (fontWeight) m_fontWeight = fontWeight;" << endl
			<< "}" << endl
			<< endl
			;

	if (isArgPresent("-t", argc, argv)) {
		generateThreadCPP(ofs, className);
	}

	ofs.close();

}

int main(int argc, char *argv[])
{
	if (argc < 2) {
		cout << "Usage: ClassGen classname [-d] [-t] [-e] [-m] [-i IDD]" << endl;
		cout << "-d = dialog" << endl;
		cout << "-t = threaded" << endl;
		cout << "-e = add easysize" << endl;
		cout << "-m = trayicon" << endl;
		cout << "-i = set IDD" << endl;
		cout << "ClassGen DlgPrepareRework -d" << endl;
		return -1;
	}

	if (isArgPresent("-d", argc, argv)) generateDialog(argc, argv);
	else generateSimple(argv[1], argc, argv);

    return 0;
}

