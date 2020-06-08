#include <Windows.h>
#include <iostream>
#include <tchar.h>

HWND hWnd, hButtonCreate;
HINSTANCE hInstance;
OPENFILENAME ofn;

void Only_Dir(wchar_t* dir)
{
	for (int i = wcslen(dir); i > 0 ; i--)
	{
		if (dir[i] == L'\\')
		{
			break;
		}
		dir[i] = L'\0';
	}
}

LRESULT CALLBACK WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_COMMAND:
		{
		
			switch (LOWORD(wParam))
			{
				case 101:
				{
					wchar_t szFile[100] = {0}; // Buffer for selected file name.
					ZeroMemory(&ofn, sizeof(ofn));
					ofn.lStructSize = sizeof(ofn);
					ofn.hwndOwner = hWnd;
					ofn.lpstrFile = szFile;
					ofn.nMaxFile = sizeof(szFile);
					ofn.lpstrFilter = L"Картинки (*.bmp, *.png, *.jpg)\0*.bmp; *.png; *.jpg\0";
					ofn.nFilterIndex = 2;
					ofn.lpstrFileTitle = NULL;
					ofn.nMaxFileTitle = 0;
					ofn.lpstrInitialDir = NULL;
					ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
					
					if (!GetOpenFileName(&ofn))
					{
						return 0;
					}

					HANDLE hFile;
					DWORD i = 0;
					
					wchar_t dir[100];

					GetModuleFileName(NULL, dir, 100);

					Only_Dir(dir);
					SetCurrentDirectory(dir);
					
					hFile = CreateFile(L"list.txt", GENERIC_WRITE, NULL, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_HIDDEN, NULL);

					SetFilePointer(hFile, 0, NULL, FILE_END);

					if (!WriteFile(hFile, szFile, wcslen(szFile) * 2, &i, NULL))
					{
						MessageBox(hWnd, L"ERROR1", L"ERROR", MB_OK);
						return 0;
					}
					WriteFile(hFile, L"\n", wcslen(szFile) * 2, &i, NULL);
					wchar_t szPath[] = L"Control Panel\\Desktop\\";
					HKEY hKey;

					if (RegOpenKeyEx(HKEY_CURRENT_USER, szPath, 0, KEY_ALL_ACCESS, &hKey) != ERROR_SUCCESS)
					{
						MessageBox(hWnd, L"ERROR2", L"ERROE", MB_OK);
						return 0;
					}
					else
					{
						if (!SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, (PVOID*)ofn.lpstrFile, SPIF_UPDATEINIFILE | SPIF_SENDCHANGE))
						{
							MessageBox(hWnd, L"ERROR3", L"ERROE", MB_OK);
							return 0;
						}
					}

					if (RegSetValueEx(hKey, L"Wallpaper", 0, REG_SZ, (BYTE*)ofn.lpstrFile, ofn.nMaxFile) != ERROR_SUCCESS)
					{
						MessageBox(hWnd, L"ERROR", L"ERROE", MB_OK);
						return 0;
					}
					CloseHandle(hFile);

				}
				return 0;

			default:
				break;
			}

		}
		return 0;

		case WM_CREATE:
		{
			hButtonCreate = CreateWindowEx(0, L"BUTTON", L"Сменить картинку рабочего стола", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 5, 10, 265, 20, hWnd, (HMENU)(101), hInstance, nullptr);
			
		}
		return 0;

		case WM_DESTROY:
		{
			PostQuitMessage(0);
		}
		return 0;

		default:

			return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInsrance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg{};

	WNDCLASSEX wc;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH)(GetStockObject(WHITE_BRUSH));
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hInstance = hInstance;
	wc.lpszClassName = L"MyClass";
	wc.lpszMenuName = 0;
	wc.style = CS_VREDRAW | CS_HREDRAW;
	wc.lpfnWndProc = WinProc;

	if (!(RegisterClassEx(&wc)))
		return FALSE;

	if ((hWnd = CreateWindowEx(0, wc.lpszClassName, L"Window", (WS_OVERLAPPEDWINDOW&~WS_THICKFRAME) | WS_VISIBLE, 200, 200, 300, 80, nullptr, nullptr, wc.hInstance, nullptr)) == INVALID_HANDLE_VALUE)
		return FALSE;


	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&msg, nullptr, 0, 0))
	{

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}


	return static_cast<int>(msg.wParam);
}