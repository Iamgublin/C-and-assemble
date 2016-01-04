// hookbyzlz.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "hookbyzlz.h"
#include<Windows.h>

// 这是导出函数的一个示例。
HOOKBYZLZ_API int fnhookbyzlz()
{
	MessageBox(NULL, TEXT("注入成功"), NULL, MB_OK);
	return 0;
}

