// dllmain.cpp : Определяет точку входа для приложения DLL.
#include "pch.h"
#include "K1804BC2.h"

#define model_key 0x00000000

extern "C" IDSIMMODEL __declspec(dllexport) * createdsimmodel(CHAR * device, ILICENCESERVER * ils)
{
	if (ils->authorize(model_key))
	{
		return new K1804BC2;
	}
	else
	{
		MessageBox(0, L"Авторизация модели не прошла: [NULL]!", L"Внимание!", MB_ICONERROR);
		return NULL;
	}
}

extern "C" VOID __declspec(dllexport) deletedsimmodel(IDSIMMODEL * model)
{
	delete (K1804BC2*)model;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fwdreason, LPVOID lpvReserved)
{
	return 1;
}
