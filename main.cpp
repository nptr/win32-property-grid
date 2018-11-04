#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <tchar.h>
#include <stdlib.h>
#include <stdio.h>
#include "main.h"
#include "propertyGrid.h"

// Begin MSVC++ Support

#ifdef _UNICODE
#define _tmemcpy  wmemcpy
#define _tmemmove  wmemmove
#define _tmemset  wmemset
#else
#define _tmemcpy  memcpy
#define _tmemmove  memmove
#define _tmemset  memset
#endif

// end MSVC++ Support

#define NELEMS(a)  (sizeof(a) / sizeof((a)[0]))
#define HEIGHT(rect) (LONG)(rect.bottom - rect.top)
#define WIDTH(rect) (LONG)(rect.right - rect.left)

#define Refresh(hwnd) RedrawWindow((hwnd), NULL, NULL, \
    RDW_ERASE|RDW_INVALIDATE|RDW_UPDATENOW)

#define CopyLF(lfDest, lfSource) \
    lfDest.lfHeight = lfSource.lfHeight; \
    lfDest.lfWidth = lfSource.lfWidth; \
    lfDest.lfEscapement = lfSource.lfEscapement; \
    lfDest.lfOrientation = lfSource.lfOrientation; \
    lfDest.lfWeight = lfSource.lfWeight; \
    lfDest.lfItalic = lfSource.lfItalic; \
    lfDest.lfUnderline = lfSource.lfUnderline; \
    lfDest.lfStrikeOut = lfSource.lfStrikeOut; \
    lfDest.lfCharSet = lfSource.lfCharSet; \
    lfDest.lfOutPrecision = lfSource.lfOutPrecision; \
    lfDest.lfClipPrecision = lfSource.lfClipPrecision; \
    lfDest.lfQuality = lfSource.lfQuality; \
    lfDest.lfPitchAndFamily = lfSource.lfPitchAndFamily; \
    _tmemmove(lfDest.lfFaceName,lfSource.lfFaceName,LF_FACESIZE)

typedef struct tagDemoData {
    TCHAR strProp1 [MAX_PATH];
    TCHAR strProp2 [MAX_PATH];
    DWORD dwChoicesCount;
    LPTSTR szzChoices;
    TCHAR strProp3 [MAX_PATH];
    TCHAR sczzChoices [MAX_PATH];
    TCHAR strProp4 [MAX_PATH];
    BYTE bProp5;
    COLORREF crProp6;
    LOGFONT lfProp7;
    COLORREF crProp7;
    TCHAR strProp8 [MAX_PATH];
    TCHAR strProp9 [MAX_PATH];
    DWORD dwProp10;
    SYSTEMTIME stProp11;
    SYSTEMTIME stProp12;
    SYSTEMTIME stProp13;
} DEMODATA, *LPDEMODATA;

static DEMODATA gDemoData;

static HANDLE ghInstance;

void Main_OnClose(HWND hwnd)
{
    free(gDemoData.szzChoices);
    EndDialog(hwnd, 0);
}

void Main_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    switch (id)
    {
        case IDOK:
        {
            static BOOL fToggle = TRUE;
            PropGrid_ShowPropertyDescriptions(GetDlgItem(hwnd, IDC_PG), fToggle);
            Button_SetText(hwndCtl, fToggle ? _T("Hide Descriptions") : _T("Show Desciptions")); 
            fToggle = !fToggle;
        }
            break;
        /*
         * TODO: Add more control ID's, when needed.
         */
    }
}

LPCTSTR byte_to_binary(int x)
{
    static TCHAR b[9] = { 0 };
    for (int z = 0x80, y = 0; z > 0; z >>= 1, y++)
    {
        b[y] = (((x & z) == z) ? _T('1') : _T('0'));
    }
    return b;
}

int AllocatedStringArray_Append(LPTSTR *lpszzTarget, int iCharCount, LPTSTR szSource)
{
    int count = iCharCount + _tcslen(szSource) + 1;
    *lpszzTarget = (LPTSTR) realloc(*lpszzTarget, (count + 1) * sizeof(TCHAR));
    _tmemmove(*lpszzTarget + iCharCount - 1, szSource, _tcslen(szSource) + 1);
    _tmemset(*lpszzTarget + count - 1, _T('\0'), 1); // Ensure array terminator
    return count;
}

static BOOL Main_OnNotify(HWND hwnd, INT id, LPNMHDR pnm)
{
    if(IDC_PG == id)
    {
        LPNMPROPGRID lpnmp = (LPNMPROPGRID)pnm;
		LPPROPGRIDITEM Item = PropGrid_GetItemData(pnm->hwndFrom, lpnmp->iIndex);
        switch(lpnmp->iIndex)
        {
            case 0:
                _sntprintf(gDemoData.strProp1, NELEMS(gDemoData.strProp1),
#ifdef _UNICODE
                    _T("%ls"),
#else
                    _T("%s"),
#endif
                    (LPTSTR)Item->lpCurValue);
                break;
            case 1:
                _sntprintf(gDemoData.strProp2, NELEMS(gDemoData.strProp2),
#ifdef _UNICODE
                    _T("%ls"),
#else
                    _T("%s"),
#endif
                    (LPTSTR)Item->lpCurValue);
                break;
            case 2:
            {
                _sntprintf(gDemoData.strProp3, NELEMS(gDemoData.strProp3),
#ifdef _UNICODE
                    _T("%ls"),
#else
                    _T("%s"),
#endif
                    (LPTSTR)Item->lpCurValue);

                // Add item to choices
                gDemoData.dwChoicesCount =
                    AllocatedStringArray_Append(&gDemoData.szzChoices,
                        gDemoData.dwChoicesCount, gDemoData.strProp3);

                Item->lpszzCmbItems = gDemoData.szzChoices;
                PropGrid_SetItemData(pnm->hwndFrom,lpnmp->iIndex, Item);
            }
                break;
            case 3: //Static No Edit
                break;
            case 4:
                // Toggle flat style
				PropGrid_SetFlatStyleChecks(pnm->hwndFrom, (BOOL)Item->lpCurValue);
				Refresh(pnm->hwndFrom);
                break;
            case 5:
                // flip bit
                if((BOOL)Item->lpCurValue)
                    gDemoData.bProp5 |= 0x01;
                else
                    gDemoData.bProp5 &= ~0x01;
                break;
            case 6:
                if((BOOL)Item->lpCurValue)
                    gDemoData.bProp5 |= 0x02;
                else
                    gDemoData.bProp5 &= ~0x02;
                break;
            case 7:
                if((BOOL)Item->lpCurValue)
                    gDemoData.bProp5 |= 0x04;
                else
                    gDemoData.bProp5 &= ~0x04;
                break;
            case 8:
                if((BOOL)Item->lpCurValue)
                    gDemoData.bProp5 |= 0x08;
                else
                    gDemoData.bProp5 &= ~0x08;
                break;
            case 9:
                if((BOOL)Item->lpCurValue)
                    gDemoData.bProp5 |= 0x10;
                else
                    gDemoData.bProp5 &= ~0x10;
                break;
            case 10:
                if((BOOL)Item->lpCurValue)
                    gDemoData.bProp5 |= 0x20;
                else
                    gDemoData.bProp5 &= ~0x20;
                break;
            case 11:
                if((BOOL)Item->lpCurValue)
                    gDemoData.bProp5 |= 0x40;
                else
                    gDemoData.bProp5 &= ~0x40;
                break;
            case 12:
                if((BOOL)Item->lpCurValue)
                    gDemoData.bProp5 |= 0x80;
                else
                    gDemoData.bProp5 &= ~0x80;
                break;
            case 13:
                gDemoData.crProp6 = (COLORREF)Item->lpCurValue;
                break;
            case 14:
            {
                gDemoData.crProp7 = ((LPPROPGRIDFONTITEM)Item->lpCurValue)->crFont;
                CopyLF(gDemoData.lfProp7,((LPPROPGRIDFONTITEM)Item->lpCurValue)->logFont);
            }
                break;
            case 15:
                _sntprintf(gDemoData.strProp8, NELEMS(gDemoData.strProp8),
                    ((LPPROPGRIDFDITEM)Item->lpCurValue)->lpszFilePath);
                break;
            case 16:
                _sntprintf(gDemoData.strProp9, NELEMS(gDemoData.strProp9),
                    (LPTSTR)Item->lpCurValue);
                break;
            case 17:
                gDemoData.dwProp10 = (DWORD)Item->lpCurValue;
                break;
            case 18:
            {
                LPSYSTEMTIME lpst = (LPSYSTEMTIME)Item->lpCurValue;
                memmove(&gDemoData.stProp11, lpst, sizeof(SYSTEMTIME));
            }
                break;
            case 19:
            {
                LPSYSTEMTIME lpst = (LPSYSTEMTIME)Item->lpCurValue;
                memmove(&gDemoData.stProp12, lpst, sizeof(SYSTEMTIME));
            }
                break;
            case 20:
            {
                LPSYSTEMTIME lpst = (LPSYSTEMTIME)Item->lpCurValue;
                memmove(&gDemoData.stProp13, lpst, sizeof(SYSTEMTIME));
            }
                break;
        }
        if(4 < lpnmp->iIndex && lpnmp->iIndex < 13)
        {
            TCHAR buf[MAX_PATH];
            _sntprintf(buf, MAX_PATH, _T("Check Boxes: %#lx = %ls"),
                gDemoData.bProp5,byte_to_binary(gDemoData.bProp5));
            Static_SetText(GetDlgItem(hwnd, IDC_LBL),buf);
        }
    }
    return TRUE;
}

BOOL Main_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
    HWND hPropGrid = GetDlgItem(hwnd,IDC_PG);

    // Initialize DemoData
    _sntprintf(gDemoData.strProp1, NELEMS(gDemoData.strProp1),_T("Property Grid demo"));
    
    _tmemset(gDemoData.strProp2,_T('\0'),NELEMS(gDemoData.strProp2));
    _tmemmove(gDemoData.sczzChoices, _T("Item1\0Item2\0Item3\0Item4\0Item5\0Item6\0Item7\0Item8\0Item9\0Item10\0Item11\0"), NELEMS(gDemoData.sczzChoices));

    _tmemset(gDemoData.strProp3,_T('\0'),NELEMS(gDemoData.strProp3));
    TCHAR szzChoices[] = _T("Robert\0Wally\0Mike\0Vickie\0Leah\0Arthur\0");
    gDemoData.dwChoicesCount = NELEMS(szzChoices);
    gDemoData.szzChoices = (LPTSTR) malloc(gDemoData.dwChoicesCount * sizeof(TCHAR));
    _tmemmove(gDemoData.szzChoices, szzChoices, gDemoData.dwChoicesCount);

    _sntprintf(gDemoData.strProp4, NELEMS(gDemoData.strProp4),_T("Static, no edit"));
    gDemoData.bProp5 = 0xff;
    gDemoData.crProp6 = RGB(128,64,64);
    GetObject((HFONT)GetStockObject(DEFAULT_GUI_FONT),
        sizeof(LOGFONT), &gDemoData.lfProp7);
    gDemoData.crProp7 = RGB(0,0,255);
    _sntprintf(gDemoData.strProp8, NELEMS(gDemoData.strProp8),_T("C:\\test.txt"));
    _sntprintf(gDemoData.strProp9, NELEMS(gDemoData.strProp9),_T("C:\\"));
    gDemoData.dwProp10 = MAKEIPADDRESS(127,0,0,1);
    GetLocalTime(&gDemoData.stProp11); 
    GetLocalTime(&gDemoData.stProp12);  
    GetLocalTime(&gDemoData.stProp13);

    // Load IDC_Label
    TCHAR buf[MAX_PATH];
    _sntprintf(buf, MAX_PATH, _T("Check Boxes: %#lx = %s"),
        gDemoData.bProp5,byte_to_binary(gDemoData.bProp5));
    Static_SetText(GetDlgItem(hwnd, IDC_LBL),buf);
 
    // Load the grid
    PROPGRIDITEM Item;
    PropGrid_ItemInit(Item);

    Item.lpszCatalog = _T("Edit, Static, and Combos");
    Item.lpszPropName = _T("Edit Field");
    Item.lpCurValue = (LPARAM) gDemoData.strProp1;
    Item.lpszPropDesc = _T("This field is editable");
    Item.iItemType = PIT_EDIT;
    PropGrid_AddItem(hPropGrid, &Item);

    Item.lpszPropName = _T("Static Combo Field");
    Item.lpCurValue = (LPARAM) gDemoData.strProp2;
    Item.lpszzCmbItems = gDemoData.sczzChoices;
    Item.lpszPropDesc = _T("Press F4 to view drop down.");
    Item.iItemType = PIT_COMBO;
    PropGrid_AddItem(hPropGrid, &Item);

    Item.lpszPropName = _T("Editable Combo Field");
    Item.lpCurValue = (LPARAM) gDemoData.strProp3;
    Item.lpszzCmbItems = gDemoData.szzChoices;
    Item.lpszPropDesc = _T("Press F4 to view drop down.");
    Item.iItemType = PIT_EDITCOMBO;
    PropGrid_AddItem(hPropGrid, &Item);

    Item.lpszPropName = _T("Static Field");
    Item.lpCurValue = (LPARAM) gDemoData.strProp4;
    Item.lpszPropDesc = _T("This field is not editable.");
    Item.iItemType = PIT_STATIC;
    PropGrid_AddItem(hPropGrid, &Item);

    Item.lpszCatalog = _T("Check Boxes");
    Item.lpszPropName = _T("Draw Flat Style boxes");
    Item.lpCurValue = (LPARAM) FALSE;
    Item.lpszPropDesc = _T("Toggle flat style check boxes.");
    Item.iItemType = PIT_CHECK;
    PropGrid_AddItem(hPropGrid, &Item);

    Item.lpszPropName = _T("Bit 0");
    Item.lpCurValue = (LPARAM) gDemoData.bProp5 & 0x01;
    Item.lpszPropDesc = _T("Bit flag.");
    Item.iItemType = PIT_CHECK;
    PropGrid_AddItem(hPropGrid, &Item);

    Item.lpszPropName = _T("Bit 1");
    Item.lpCurValue = (LPARAM) gDemoData.bProp5 & 0x02;
    PropGrid_AddItem(hPropGrid, &Item);

    Item.lpszPropName = _T("Bit 2");
    Item.lpCurValue = (LPARAM) gDemoData.bProp5 & 0x04;
    PropGrid_AddItem(hPropGrid, &Item);

    Item.lpszPropName = _T("Bit 3");
    Item.lpCurValue = (LPARAM) gDemoData.bProp5 & 0x08;
    PropGrid_AddItem(hPropGrid, &Item);

    Item.lpszPropName = _T("Bit 4");
    Item.lpCurValue = (LPARAM) gDemoData.bProp5 & 0x10;
    PropGrid_AddItem(hPropGrid, &Item);

    Item.lpszPropName = _T("Bit 5");
    Item.lpCurValue = (LPARAM) gDemoData.bProp5 & 0x20;
    PropGrid_AddItem(hPropGrid, &Item);

    Item.lpszPropName = _T("Bit 6");
    Item.lpCurValue = (LPARAM) gDemoData.bProp5 & 0x40;
    PropGrid_AddItem(hPropGrid, &Item);

    Item.lpszPropName = _T("Bit 7");
    Item.lpCurValue = (LPARAM) gDemoData.bProp5 & 0x80;
    PropGrid_AddItem(hPropGrid, &Item);

    Item.lpszCatalog = _T("Dialogs");
    Item.lpszPropName = _T("Choose color");
    Item.lpCurValue = (LPARAM) gDemoData.crProp6;
    Item.lpszPropDesc = _T("Click button to launch dialog or <shift TAB> + Enter.");
    Item.iItemType = PIT_COLOR;
    PropGrid_AddItem(hPropGrid, &Item);

    PROPGRIDFONTITEM ItemLf;
    ItemLf.logFont = gDemoData.lfProp7;
    ItemLf.crFont = gDemoData.crProp7;
    Item.lpszPropName = _T("Choose font");
    Item.lpCurValue = (LPARAM) &ItemLf;
    Item.iItemType = PIT_FONT;
    PropGrid_AddItem(hPropGrid, &Item);

    PROPGRIDFDITEM ItemFd = {0};
    ItemFd.lpszDlgTitle = _T("Choose File");
    ItemFd.lpszFilePath = gDemoData.strProp8;
    ItemFd.lpszFilter = _T("Text files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0");
    ItemFd.lpszDefExt = _T("txt");
    Item.lpszPropName = _T("Choose file");
    Item.lpCurValue = (LPARAM) &ItemFd;
    Item.iItemType = PIT_FILE;
    PropGrid_AddItem(hPropGrid, &Item);

    Item.lpszPropName = _T("Choose folder");
    Item.lpCurValue = (LPARAM) gDemoData.strProp9;
    Item.iItemType = PIT_FOLDER;
    PropGrid_AddItem(hPropGrid, &Item);

    Item.lpszCatalog = _T("IpEdit, Date, and Time");
    Item.lpszPropName = _T("IpEdit Field");
    Item.lpCurValue = (LPARAM) gDemoData.dwProp10;
    Item.lpszPropDesc = _T("Enter valid IP addresses");
    Item.iItemType = PIT_IP;
    PropGrid_AddItem(hPropGrid, &Item);

    Item.lpszPropName = _T("Date Field");
    Item.lpCurValue = (LPARAM) &gDemoData.stProp11;
    Item.lpszPropDesc = _T("Press F4 to view drop down.");
    Item.iItemType = PIT_DATE;
    PropGrid_AddItem(hPropGrid, &Item);

    Item.lpszPropName = _T("Time Field");
    Item.lpCurValue = (LPARAM) &gDemoData.stProp12;
    Item.lpszPropDesc = _T("Pick a time.");
    Item.iItemType = PIT_TIME;
    PropGrid_AddItem(hPropGrid, &Item);

    Item.lpszPropName = _T("Date and Time Field");
    Item.lpCurValue = (LPARAM) &gDemoData.stProp13;
    Item.lpszPropDesc = _T("Pick a time.");
    Item.iItemType = PIT_DATETIME;
    PropGrid_AddItem(hPropGrid, &Item);

    PropGrid_ShowToolTips(hPropGrid,TRUE);
    PropGrid_ExpandAllCatalogs(hPropGrid);
	PropGrid_SetCurSel(hPropGrid,0);

    return TRUE;
}

void Main_OnSize(HWND hwnd, UINT state, int cx, int cy)
{
    RECT rcOk = {0};
    GetWindowRect(GetDlgItem(hwnd,IDOK),&rcOk);

    RECT rcLb = {0};
    GetWindowRect(GetDlgItem(hwnd,IDC_LBL),&rcLb);

    MoveWindow(GetDlgItem(hwnd,IDC_PG),0,0,
        cx, cy - (HEIGHT(rcOk) + 2),TRUE);
    MoveWindow(GetDlgItem(hwnd,IDOK), cx - WIDTH(rcOk),
        cy - HEIGHT(rcOk), WIDTH(rcOk),HEIGHT(rcOk),TRUE);

    //Refresh label
    MoveWindow(GetDlgItem(hwnd,IDC_LBL), 1,
        cy - HEIGHT(rcOk) + 1, cx - WIDTH(rcOk) - 4, HEIGHT(rcOk) - 2,TRUE);
    Refresh(GetDlgItem(hwnd,IDC_LBL)); //And label contents
}

BOOL CALLBACK Main_DlgProc (HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
        HANDLE_MSG (hwndDlg, WM_CLOSE, Main_OnClose);
        HANDLE_MSG (hwndDlg, WM_COMMAND, Main_OnCommand);
        HANDLE_MSG (hwndDlg, WM_INITDIALOG, Main_OnInitDialog);
        HANDLE_MSG (hwndDlg, WM_NOTIFY, Main_OnNotify);
        HANDLE_MSG (hwndDlg, WM_SIZE, Main_OnSize);

        //// TODO: Add dialog message crackers here...

        default: return FALSE;
    }
}

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
    INITCOMMONCONTROLSEX icc;
    WNDCLASSEX wcx;

    ghInstance = hInstance;

    icc.dwSize = sizeof(icc);
    icc.dwICC = ICC_WIN95_CLASSES /*|ICC_COOL_CLASSES|ICC_DATE_CLASSES|ICC_PAGESCROLLER_CLASS|ICC_USEREX_CLASSES*/;
    InitCommonControlsEx(&icc);

    InitPropertyGrid(hInstance);

    /* Get system dialog information */
    wcx.cbSize = sizeof(wcx);
    if (!GetClassInfoEx(NULL, MAKEINTRESOURCE(32770), &wcx))
        return 0;

    /* Add our own stuff */
    wcx.hInstance = hInstance;
    wcx.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDR_ICO_MAIN));
    wcx.lpszClassName = _T("PropertyClass");
    if (!RegisterClassEx(&wcx))
        return 0;

    /* The user interface is a modal dialog box */
    return DialogBox(hInstance, MAKEINTRESOURCE(DLG_MAIN), NULL, (DLGPROC)Main_DlgProc);
}
