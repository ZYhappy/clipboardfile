// TestClipboard.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "TestClipboard.h"
#include "DataObject.h"
#include "VirtualFileSrc.h"
#include "VirtualFileSrcStream.h"
#include "DragDrop.h"

#ifndef GET_X_LPARAM
#define GET_X_LPARAM(lp)                        ((int)(short)LOWORD(lp))
#endif
#ifndef GET_Y_LPARAM
#define GET_Y_LPARAM(lp)                        ((int)(short)HIWORD(lp))
#endif

#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此处放置代码。

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_TESTCLIPBOARD, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TESTCLIPBOARD));

    MSG msg;

    // 主消息循环:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TESTCLIPBOARD));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_TESTCLIPBOARD);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   ::OleInitialize(nullptr);
#if 0
   IDataObject *data_obj = nullptr;
   HRESULT hr = clipboard::CDataObject_CreateInstance(IID_IDataObject, (void**)&data_obj);
   if (SUCCEEDED(hr)) {
	   ::OleSetClipboard(data_obj);
	   data_obj->Release();
   }
#elif 0
   IDataObject *data_obj = nullptr;
   HRESULT hr = clipboard::VirtualFileSrc_CreateInstance(IID_IDataObject, (void**)&data_obj);
   if (SUCCEEDED(hr)) {
	   ::OleSetClipboard(data_obj);
	   data_obj->Release();
   }
#elif 1
   IDataObject *data_obj = nullptr;
   HRESULT hr = clipboard::VirtualFileSrcStream_CreateInstance(IID_IDataObject, (void**)&data_obj);
   if (SUCCEEDED(hr)) {
	   ::OleSetClipboard(data_obj);
	   data_obj->Release();
   }

#endif

   return TRUE;
}

class CDrawTextRestorer
{
public:
	CDrawTextRestorer(HDC hdc, HFONT hfont, COLORREF clrText, int nBackMode = TRANSPARENT)
		: m_hdc(hdc)
		, m_hfontOld(NULL)
	{
		if (hfont) {
			m_hfontOld = ::SelectObject(m_hdc, hfont);
		}
		m_clrOld = ::SetTextColor(m_hdc, clrText);
		m_nModeOld = ::SetBkMode(m_hdc, nBackMode);
	}

	~CDrawTextRestorer()
	{
		if (m_hfontOld) {
			::SelectObject(m_hdc, m_hfontOld);
		}
		::SetTextColor(m_hdc, m_clrOld);
		m_nModeOld = ::SetBkMode(m_hdc, m_nModeOld);
	}

protected:
	HDC		m_hdc;
	HGDIOBJ m_hfontOld;
	COLORREF m_clrOld;
	int		 m_nModeOld;

};

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static RECT s_rc = { 100, 50, 400, 70 };
	static bool s_pushdown = false;
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 分析菜单选择:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 在此处添加使用 hdc 的任何绘图代码...
			CDrawTextRestorer rest(hdc, (HFONT)::GetStockObject(DEVICE_DEFAULT_FONT), RGB(0, 0, 0));
			RECT rc = s_rc;
			if (s_pushdown) {
				::OffsetRect(&rc, 1, 1);
			}
			::DrawText(hdc, L"TestFakeFile.txt(按住我拖动)", -1, &rc, DT_TOP);
            EndPaint(hWnd, &ps);
        }
        break;
	case WM_MOUSEMOVE:
		if (wParam & MK_LBUTTON) {
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			if (PtInRect(&s_rc, pt)) {
				HRESULT hr = clipboard::DoDragDrop(DROPEFFECT_COPY);
				break;
			}		
		}
		break;
	case WM_LBUTTONDOWN:
	{
		POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		if (PtInRect(&s_rc, pt)) {
			s_pushdown = true;
			::InvalidateRect(hWnd, &s_rc, true);
		}
	}
		break;
	case WM_LBUTTONUP:
		if (s_pushdown) {
			s_pushdown = false;
			::InvalidateRect(hWnd, &s_rc, true);
		}
		break;
	case WM_CAPTURECHANGED:
		if (s_pushdown) {
			s_pushdown = false;
			::InvalidateRect(hWnd, &s_rc, true);
		}
		break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
