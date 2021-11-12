// WindowsProject1.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "framework.h"
#include "WindowsProject1.h"
#include <exception>
#include <CommCtrl.h>
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
	HWND hListView;
	MainWindow(HWND hListView)
		: hListView(hListView)
	{
		LVCOLUMNW col =
		{ /*UINT   mask       */LVCF_TEXT | LVCF_WIDTH
		, /*int    fmt        */0
		, /*int    cx         */100//列幅の初期値
		, /*LPWSTR pszText    */const_cast<LPWSTR>(L"第１列")
		, /*int    cchTextMax */0
		, /*int    iSubItem   */0
		, /*int    iImage     */0
		, /*int    iOrder     */0
		};
		ListView_InsertColumn(hListView, 0, &col);
		col.pszText = const_cast<LPWSTR>(L"第２列");
		ListView_InsertColumn(hListView, 1, &col);

		HKEY hKey;
		if (RegOpenKeyW(HKEY_CURRENT_USER, szRegKey, &hKey) == ERROR_SUCCESS) {
			const auto cCols = Header_GetItemCount(ListView_GetHeader(hListView));
			for (int iCol = 0; iCol < cCols; ++iCol) {
				DWORD cx;
				DWORD dwType;
				DWORD cbData = sizeof cx;
				if (RegQueryValueExW
				( /*_In_       HKEY    hKey       */hKey
				, /*_In_opt_   LPCWSTR lpValueName*/RegValNameOfColumn(iCol).c_str()
				, /*_Reserved_ LPDWORD lpReserved */nullptr
				, /*_Out_opt_  LPDWORD lpType     */&dwType
				, /*           LPBYTE  lpData     */LPBYTE(&cx)
				, /*           LPDWORD lpcbData   */&cbData
				) == ERROR_SUCCESS) {
					if (dwType == REG_DWORD) {
						ListView_SetColumnWidth(hListView, iCol, cx);
					}
				}
			}
			(void)RegCloseKey(hKey);
		}


	}
	static const wchar_t szRegKey[];

	static std::wstring RegValNameOfColumn(int iCol)
	{
		return L"cx#" + std::to_wstring(iCol);
	}

	void SaveColumnWidths()
	{
		HKEY hKey;
		if (RegCreateKeyW(HKEY_CURRENT_USER, szRegKey, &hKey) == ERROR_SUCCESS) {
			const auto cCols = Header_GetItemCount(ListView_GetHeader(hListView));
			for (int iCol = 0;iCol < cCols; ++iCol) {
				const DWORD cx = ListView_GetColumnWidth(hListView, iCol);
				(void)RegSetValueExW
				( hKey
				, RegValNameOfColumn(iCol).c_str()
				, 0
				, REG_DWORD
				, LPBYTE(&cx)
				, sizeof cx
				);
			}
			(void)RegCloseKey(hKey);
		}
	}

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
				SetWindowLongPtr(hWnd, GWLP_USERDATA, LONG_PTR(new MainWindow(CreateWindowW
				( /*lpClassName */WC_LISTVIEWW
				, /*lpWindowName*/L""
				, /*dwStyle     */WS_CHILD | WS_VISIBLE | LVS_REPORT
				, /*x           */0
				, /*y           */0
				, /*nWidth      */100
				, /*nHeight     */100
				, /*hWndParent  */hWnd
				, /*hMenu       */nullptr
				, /*hInstance   */nullptr
				, /*lpParam     */nullptr
				))));
				return 0;
			}
			catch (const std::exception& e) {
				OutputDebugStringA(e.what());;
				OutputDebugStringA("\n");
				return -1;
			}
		case WM_DESTROY:
			if (const auto p = reinterpret_cast<MainWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA))) {
				p->SaveColumnWidths();
			}
			return 0;
		case WM_NCDESTROY:
			delete reinterpret_cast<MainWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
			PostQuitMessage(0/*終了コード*/);
			return 0;
		case WM_SIZE:
			if (const auto p = reinterpret_cast<MainWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA))) {
				MoveWindow
				( /*_In_ HWND hWnd    */p->hListView
				, /*_In_ int  X       */0
				, /*_In_ int  Y       */0
				, /*_In_ int  nWidth  */LOWORD(lParam)//幅  
				, /*_In_ int  nHeight */HIWORD(lParam)//高さ
				, /*_In_ BOOL bRepaint*/true
				);					
			}
			return 0;

		case WM_COMMAND:
			if (const auto p = reinterpret_cast<MainWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA)))
				return p->Handle_WM_COMMAND(hWnd, wParam, lParam);
			else
				return DefWindowProc(hWnd, message, wParam, lParam);
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
};
const wchar_t MainWindow::szRegKey[] = LR"(Software\youtube-programmercpp\WindowsProject1)";

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
