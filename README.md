ClassGen
--------

Simple class generator for Visual C++.
Generate normal, threaded, dialog or dialog with trayicon classes easily.

Usage
-----

1) Decide to try it out :)
2) Create new MFC project Test (dialog based document), dialog ID = IDD_TEST 
3) Go to Resource View -> Test.rc -> Dialog
4) Create new dialog DlgDummy IDD_DUMMY
5) Issue command cg DlgDummy -t -d -e -i IDD_DUMMY
   in the target directory (Test\Test).
6) Add newly generated DlgDummy.h and DlgDummy.cpp to the Test project
7) Copy easysize.h to Test\Test and also add it to the project
8) Add IDC_EDITOR to IDD_DUMMY. Set it to Multiline = true, Want Return = true.
   Also expand its size so it almost matches the size of the IDD_DUMMY.
9) Set the character set in project to NOT SET.
10) Add a button to IDD_TEST
11) Add these lines to its Clicked handler:

	DlgDummy dlg(this);
	dlg.presetFont("Courier New", 10, 0);
	dlg.DoModal();

12) Set Border of IDD_DUMMY to Resizing so the easysize can handle resizing of IDC_EDITOR
    Also uncomment the line:
    EASYSIZE(IDC_EDITOR, ES_BORDER, ES_BORDER, ES_BORDER, ES_BORDER, 0);

13) Compile
14) Run

Note: The Test project is included here.

License
-------

Freeware

Copyright
---------
(c) 2020 Michal Vanka
