#include <windows.h>
#include <iostream>
#include <string>
#include "other_functions.h"

#pragma comment(lib, "winmm.lib")

#ifndef UNICODE
#error Please enable UNICODE for your compiler! VS: Project Properties -> General -> \
Character Set -> Use Unicode.
#endif

class ConsoleEngine
{
protected:
	int m_nScreenWidth;
	int m_nScreenHeight;
	CHAR_INFO *m_bufScreen;
	std::wstring m_sAppName;
	HANDLE m_hOriginalConsole;
	CONSOLE_SCREEN_BUFFER_INFO m_OriginalConsoleInfo;
	HANDLE m_hConsole;
	HANDLE m_hConsoleIn;
	SMALL_RECT m_rectWindow;

	bool m_bConsoleInFocus = true;

	int descriptionStartColunmNum, nEffects;


	int Error(const wchar_t *msg)
	{
		wchar_t buf[256];
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buf, 256, NULL);
		SetConsoleActiveScreenBuffer(m_hOriginalConsole);
		wprintf(L"ERROR: %s\n\t%s\n", msg, buf);
		return 0;
	}

	// These need to be static because of the OnDestroy call the OS may make. The OS
	// spawns a special thread just for that


public:
	ConsoleEngine()
	{
		m_nScreenWidth = 80;
		m_nScreenHeight = 30;

		m_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		m_hConsoleIn = GetStdHandle(STD_INPUT_HANDLE);

		m_sAppName = L"Default";
	}

	int ConstructConsole(int nrows, int ncols, int fontw, int fonth)
	{
		if (m_hConsole == INVALID_HANDLE_VALUE)
			return Error(L"Bad Handle");

		m_nScreenWidth = ncols;
		m_nScreenHeight = nrows;

		// Update 13/09/2017 - It seems that the console behaves differently on some systems
		// and I'm unsure why this is. It could be to do with windows default settings, or
		// screen resolutions, or system languages. Unfortunately, MSDN does not offer much
		// by way of useful information, and so the resulting sequence is the reult of experiment
		// that seems to work in multiple cases.
		//
		// The problem seems to be that the SetConsoleXXX functions are somewhat circular and 
		// fail depending on the state of the current console properties, i.e. you can't set
		// the buffer size until you set the screen size, but you can't change the screen size
		// until the buffer size is correct. This coupled with a precise ordering of calls
		// makes this procedure seem a little mystical :-P. Thanks to wowLinh for helping - Jx9

		// Change console visual size to a minimum so ScreenBuffer can shrink
		// below the actual visual size
		m_rectWindow = { 0, 0, 1, 1 };
		SetConsoleWindowInfo(m_hConsole, TRUE, &m_rectWindow);

		// Set the size of the screen buffer
		COORD coord = { (short)m_nScreenWidth, (short)m_nScreenHeight };
		if (!SetConsoleScreenBufferSize(m_hConsole, coord))
			Error(L"SetConsoleScreenBufferSize");

		// Assign screen buffer to the console
		if (!SetConsoleActiveScreenBuffer(m_hConsole))
			return Error(L"SetConsoleActiveScreenBuffer");

		// Set the font size now that the screen buffer has been assigned to the console
		CONSOLE_FONT_INFOEX cfi;
		cfi.cbSize = sizeof(cfi);
		cfi.nFont = 0;
		cfi.dwFontSize.X = fontw;
		cfi.dwFontSize.Y = fonth;
		cfi.FontFamily = FF_DONTCARE;
		cfi.FontWeight = FW_NORMAL;

		/*	DWORD version = GetVersion();
			DWORD major = (DWORD)(LOBYTE(LOWORD(version)));
			DWORD minor = (DWORD)(HIBYTE(LOWORD(version)));*/

			//if ((major > 6) || ((major == 6) && (minor >= 2) && (minor < 4)))		
			//	wcscpy_s(cfi.FaceName, L"Raster"); // Windows 8 :(
			//else
			//	wcscpy_s(cfi.FaceName, L"Lucida Console"); // Everything else :P

			//wcscpy_s(cfi.FaceName, L"Liberation Mono");
		wcscpy_s(cfi.FaceName, L"Consolas");
		if (!SetCurrentConsoleFontEx(m_hConsole, false, &cfi))
			return Error(L"SetCurrentConsoleFontEx");

		// Get screen buffer info and check the maximum allowed window size. Return
		// error if exceeded, so user knows their dimensions/fontsize are too large
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		if (!GetConsoleScreenBufferInfo(m_hConsole, &csbi))
			return Error(L"GetConsoleScreenBufferInfo");
		if (m_nScreenHeight > csbi.dwMaximumWindowSize.Y)
			return Error(L"Screen Height / Font Height Too Big");
		if (m_nScreenWidth > csbi.dwMaximumWindowSize.X)
			return Error(L"Screen Width / Font Width Too Big");

		// Set Physical Console Window Size
		m_rectWindow = { 0, 0, (short)m_nScreenWidth - 1, (short)m_nScreenHeight - 1 };
		if (!SetConsoleWindowInfo(m_hConsole, TRUE, &m_rectWindow))
			return Error(L"SetConsoleWindowInfo");

		// Set flags to allow mouse input		
		if (!SetConsoleMode(m_hConsoleIn, ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT))
			return Error(L"SetConsoleMode");

		// Allocate memory for screen buffer
		m_bufScreen = new CHAR_INFO[m_nScreenWidth*m_nScreenHeight];
		memset(m_bufScreen, 0, sizeof(CHAR_INFO) * m_nScreenWidth * m_nScreenHeight);

		//SetConsoleCtrlHandler((PHANDLER_ROUTINE)CloseHandler, TRUE);
		return 1;
	}

	/*void Draw(int x, int y, short c = 0x2588, short col = 0x000F)
	{
		if (x >= 0 && x < m_nScreenWidth && y >= 0 && y < m_nScreenHeight)
		{
			m_bufScreen[y * m_nScreenWidth + x].Char.UnicodeChar = c;
			m_bufScreen[y * m_nScreenWidth + x].Attributes = col;
		}
	}*/

	/*void Fill(int x1, int y1, int x2, int y2, short c = 0x2588, short col = 0x000F)
	{
		Clip(x1, y1);
		Clip(x2, y2);
		for (int x = x1; x < x2; x++)
			for (int y = y1; y < y2; y++)
				Draw(x, y, c, col);
	}*/

	void DrawString(int row, int column, std::wstring c, short col = 0x000F)
	{
		for (size_t i = 0; i < c.size(); i++)
		{
			m_bufScreen[row * m_nScreenWidth + column + i].Char.UnicodeChar = c[i];
			m_bufScreen[row * m_nScreenWidth + column + i].Attributes = col;
		}
	}

	void Clip(int &x, int &y)
	{
		if (x < 0) x = 0;
		if (x >= m_nScreenWidth) x = m_nScreenWidth-1;
		if (y < 0) y = 0;
		if (y >= m_nScreenHeight) y = m_nScreenHeight-1;
	}

	void add_description(std::wstring* text, int columnNum, int _nEffects, short col = 0x000f) {
		descriptionStartColunmNum = columnNum;
		nEffects = _nEffects;
		for (int i = 0; i < nEffects; ++i) {
			DrawString(columnNum,0 , text[i], col);
			columnNum += 2;
		}

	}
	void set_color(int row, int column, short col = 0x000F) {
		m_bufScreen[row * m_nScreenWidth + column].Attributes = col;
	}

	void set_color(int rowStart, int rowEnd, int colStart, int colEnd, short col = 0x000F) {
		for (int i = rowStart; i < rowEnd; ++i)
			for (int j = colStart; j < colEnd; ++j)
				m_bufScreen[i * m_nScreenWidth + j].Attributes = col;
	}
	
	void update(int id, float value, int nVertices ,float blurScale, float gamma, short col = 0x0030) {
		set_color(descriptionStartColunmNum, descriptionStartColunmNum+nEffects*2, 0, m_nScreenWidth);
		int columnNum = descriptionStartColunmNum + id * 2;
		for (int i = 0; i < m_nScreenWidth; ++i) {
			m_bufScreen[columnNum * m_nScreenWidth + i].Attributes = col;
			m_bufScreen[(columnNum + 1) * m_nScreenWidth + i].Attributes = col;

		}
		int sliderPositionX = (int)scale(value, -1.f, 1.f, 0.f, (float)(m_nScreenWidth - 1));
		int sliderPositionY = columnNum + 1;
		Clip(sliderPositionX, sliderPositionY);
		set_color(sliderPositionY, sliderPositionX);

		std::wstring text;
		text += L"id="+std::to_wstring(id)+L"  ";
		text += L"value=" + std::to_wstring(value).substr(0, 6) + L"  ";
		text += L"nVertices=" + std::to_wstring(nVertices) + L"  ";
		text += L"blurScale=" + std::to_wstring(blurScale).substr(0, 6) + L"  ";
		text += L"gamma=" + std::to_wstring(gamma).substr(0, 6) + L"  ";

		DrawString(m_nScreenHeight - 1, 0, text, 0x0008);
		}

	void show() {
		WriteConsoleOutput(m_hConsole, m_bufScreen, { (short)m_nScreenWidth, (short)m_nScreenHeight }, { 0,0 }, &m_rectWindow);
	}

	~ConsoleEngine()
	{
		SetConsoleActiveScreenBuffer(m_hOriginalConsole);
		delete[] m_bufScreen;
	}




};