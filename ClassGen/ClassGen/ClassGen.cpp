// ClassGen.cpp : Defines the entry point for the console application.
//

// TODO: add namespace option (2nd argument)

#include "stdafx.h"

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

#define BASE_DIALOG "CDialog"
#define DIVIDER "/*----------------------------------------------------------*/"

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
		ofs << "{" << endl << "}" << endl
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
		ofs << "public:" << endl
		<< "\tvoid presetFont(const char *fontFaceName, int size, int weight);" << endl
		<< "\tvirtual INT_PTR DoModal();" << endl
		<< "\t" << className << "(CWnd* pParent);" << endl
		<< "\t~" << className << "();" << endl
		<< "};" << endl
		;

	ofs.close();

	ofs.open(className + string(".cpp"), std::ofstream::binary | std::ofstream::trunc);

	ofs << "//Automatically generated by ClassGen" << endl
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

		if (isArgPresent("-t", argc, argv)) {
			ofs << "\tON_MESSAGE(DATA_READY, &" << className << "::OnDataReady)" << endl
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
		<< "\tm_fontFaceName = \"MS Shell Dlg\";" << endl
		<< "}" << endl
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
		<< "\tif (m_fontWeight) {" << endl
		<< "\t\tCFont * pFont = GetFont();" << endl
		<< "\t\tLOGFONT lf;" << endl
		<< "\t\tpFont->GetLogFont(&lf);" << endl
		<< "\t\tlf.lfWeight |= m_fontWeight;" << endl
		<< "\t\tm_font.CreateFontIndirect(&lf);" << endl
		<< "\t\tSendMessageToDescendants(WM_SETFONT, (WPARAM)m_font.m_hObject);" << endl
		<< "\t}" << endl;

		if (isArgPresent("-e", argc, argv)) {
			ofs << "\tINIT_EASYSIZE;" << endl;
			ofs << endl;
		}

		ofs << endl
		<< "\treturn true;" << endl
		<< "}" << endl
		<< endl;

		if (isArgPresent("-e", argc, argv)) {
			ofs << DIVIDER << endl
			<< "#pragma warning(disable: 4189)   // local variable is initialized but not referenced" << endl
			<< "BEGIN_EASYSIZE_MAP(" << className << ")" << endl
			<< "/*EASYSIZE(IDC_EDITOR, ES_BORDER, ES_BORDER, ES_BORDER, ES_BORDER, 0);*/" << endl
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
			<< "\t{" << endl
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
		cout << "Usage: ClassGen classname [-d] [-t] [-e] [-i IDD]" << endl;
		cout << "-d = dialog" << endl;
		cout << "-t = threaded" << endl;
		cout << "-e = add easysize" << endl;
		cout << "-i = set IDD" << endl;
		cout << "ClassGen DlgPrepareRework -d" << endl;
		return -1;
	}

	if (isArgPresent("-d", argc, argv)) generateDialog(argc, argv);
	else generateSimple(argv[1], argc, argv);

    return 0;
}

