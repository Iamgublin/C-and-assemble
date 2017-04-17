#include"Winproc.h"
INT_PTR CALLBACK MoreInformaiton(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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
	}
	return (INT_PTR)FALSE;
}
INT_PTR CALLBACK SelCard(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
	{
		FindCard(hDlg);
		return (INT_PTR)FALSE;
	}
	case WM_PAINT:
		break;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		int wmId = LOWORD(wParam);
		// 分析菜单选择: 
		switch (wmId)
		{
		case IDC_STOP:
			StopFilter(hDlg);
			EndDialog(hDlg, 0);
			break;
		case IDC_START:
			StartFilter(hDlg);
			EndDialog(hDlg, 0);
			break;
		default:
			break;
		}
		break;
	}
	return (INT_PTR)FALSE;
}



//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_SIZE:
	{
		Cxrect = LOWORD(lParam);
		Cyrect = HIWORD(lParam);
		UpdateWindow(hWnd);
		break;
	}
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
		case ID_SELNETCARD:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_SELNETCARD), hWnd, SelCard);
			break;
		case ID_CLEARLIST:
			DeleteAllListInfo();
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
		MoveWindow(ListView, 0, 0, Cxrect,
			Cyrect / 2, TRUE);
		MoveWindow(StaticWindow, 0, Cyrect / 2, Cxrect, Cyrect / 2, TRUE);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_NOTIFY:
	{
		LPNMHDR Hdr = (LPNMHDR)lParam;
		if (Hdr->code == NM_CLICK)
		{
			ShowOutput((LPNMLISTVIEW)Hdr);
		}
		if (Hdr->code == NM_DBLCLK)
		{
			DialogBox(hInst, MAKEINTRESOURCE(IDD_MOREEINFORMATION), HwndWinMain, MoreInformaiton);
			break;
		}
		break;
	}
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
