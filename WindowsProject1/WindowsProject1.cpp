﻿// WindowsProject1.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "framework.h"
#include "WindowsProject1.h"

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


//
//  関数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的: メイン ウィンドウのメッセージを処理します。
//
//  WM_COMMAND  - アプリケーション メニューの処理
//  WM_PAINT    - メイン ウィンドウを描画する
//  WM_DESTROY  - 中止メッセージを表示して戻る
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// 選択されたメニューの解析:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(HINSTANCE(GetWindowLongPtrW(hWnd, GWLP_HINSTANCE)), MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
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
		// TODO: HDC を使用する描画コードをここに追加してください...
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0/*終了コード*/);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
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
	, /*WNDPROC     lpfnWndProc   */WndProc
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
