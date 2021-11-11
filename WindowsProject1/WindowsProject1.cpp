// WindowsProject1.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "framework.h"
#include "WindowsProject1.h"
#include <exception>
// バージョン情報ボックスのメッセージ ハンドラーです。
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


#include <string>
namespace Win32Wrap {
	ATOM RegisterClassExW(const WNDCLASSEXW& r)
	{
		return ::RegisterClassExW(&r);
	}
	std::wstring LoadStringW
	( _In_opt_ HINSTANCE hInstance
	, _In_     UINT      uID      
	)
	{
		LPCWSTR lpBuffer;
		if (const auto cchTextW = ::LoadStringW
		( /*_In_opt_                                 HINSTANCE hInstance   */hInstance   
		, /*_In_                                     UINT      uID         */uID         
		, /*_Out_writes_to_(cchBufferMax, return +1) LPWSTR    lpBuffer    */LPWSTR(&lpBuffer)
		, /*_In_                                     int       cchBufferMax*/0
		))
			return { lpBuffer, static_cast<size_t>(cchTextW) };
		else
			return {};
	}
}
class MainWindow {
	LRESULT Handle_WM_COMMAND(HWND hWnd, WPARAM wParam, LPARAM lParam)
	{
		switch (const int wmId = LOWORD(wParam))
		{
		case IDM_ABOUT:
			DialogBox(HINSTANCE(GetWindowLongPtrW(hWnd, GWLP_HINSTANCE)), MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			return 0;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			return 0;
		default:
			return DefWindowProc(hWnd, WM_COMMAND, wParam, lParam);
		}		
	}
	void Handle_WM_PAINT(HWND hWnd, WPARAM wParam, LPARAM lParam)
	{
		PAINTSTRUCT ps;
		const HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: HDC を使用する描画コードをここに追加してください...
		EndPaint(hWnd, &ps);
	}
public:
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message) {
		case WM_CREATE:
			try {
				SetWindowLongPtr(hWnd, GWLP_USERDATA, LONG_PTR(new MainWindow));
				return 0;
			}
			catch (const std::exception& e) {
				OutputDebugStringA(e.what());;
				OutputDebugStringA("\n");
				return -1;
			}
		case WM_NCDESTROY:
			delete reinterpret_cast<MainWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
			PostQuitMessage(0/*終了コード*/);
			return 0;
		case WM_COMMAND:
			if (const auto p = reinterpret_cast<MainWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA)))
				return p->Handle_WM_COMMAND(hWnd, wParam, lParam);
			else
				return DefWindowProc(hWnd, message, wParam, lParam);
		case WM_PAINT:
			if (const auto p = reinterpret_cast<MainWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA)))
				p->Handle_WM_PAINT(hWnd, wParam, lParam);
			return 0;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
};

int APIENTRY wWinMain
( _In_     HINSTANCE hInstance
, _In_opt_ HINSTANCE //hPrevInstance
, _In_     LPWSTR    //lpCmdLine    
, _In_     int       nCmdShow
)
{
	const auto sClass = Win32Wrap::LoadStringW(hInstance, IDC_WINDOWSPROJECT1);
	const auto atom = Win32Wrap::RegisterClassExW(
	{ /*UINT        cbSize        */sizeof (WNDCLASSEXW)
	, /*UINT        style         */CS_HREDRAW | CS_VREDRAW
	, /*WNDPROC     lpfnWndProc   */MainWindow::WindowProc
	, /*int         cbClsExtra    */0
	, /*int         cbWndExtra    */0
	, /*HINSTANCE   hInstance     */hInstance
	, /*HICON       hIcon         */LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT1))
	, /*HCURSOR     hCursor       */LoadCursor(nullptr, IDC_ARROW)
	, /*HBRUSH      hbrBackground */HBRUSH(COLOR_WINDOW + 1)
	, /*LPCWSTR     lpszMenuName  */MAKEINTRESOURCEW(IDC_WINDOWSPROJECT1)
	, /*LPCWSTR     lpszClassName */sClass.c_str()
	, /*HICON       hIconSm       */LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SMALL))
	});
	if (const HWND hWnd = CreateWindowW
	( LPCWSTR(atom)
	, Win32Wrap::LoadStringW(hInstance, IDS_APP_TITLE).c_str()
	, WS_OVERLAPPEDWINDOW
	, CW_USEDEFAULT
	, 0
	, CW_USEDEFAULT
	, 0
	, nullptr
	, nullptr
	, hInstance
	, nullptr
	)) {
		ShowWindow(hWnd, nCmdShow);
		UpdateWindow(hWnd);
		const HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT1));
		MSG msg;
		// メイン メッセージ ループ:
		while (GetMessage(&msg, nullptr, 0, 0))
		{
			if (TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
				continue;
			else {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		return static_cast<int>(msg.wParam);//PostQuitMessageから渡って来た終了コード
	}
	else
		return EXIT_FAILURE;
}
