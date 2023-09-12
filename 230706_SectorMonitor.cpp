// 230706_SectorMonitor.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "230706_SectorMonitor.h"
#include "SectorClient.h"

#define MAX_LOADSTRING 100

HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

//==========================
//더블버퍼링용 전역 변수들
//==========================
HBITMAP g_hMemDCBitmap;
HBITMAP g_hMemDCBitmap_old;
HDC g_hMemDC;
RECT g_MemDCRect;
HPEN g_hGridPen;

//브러쉬
HBRUSH g_brush[6];

//윈도우 핸들
HWND g_Window;


//전역 클라이언트
server_baby::SectorClient g_SectorClient;

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void RenderGrid(HDC hdc);
void RenderSector(HDC hdc);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MY230706SECTORMONITOR, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    g_SectorClient.Start((char*)"127.0.0.1", 11907);


    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MY230706SECTORMONITOR));

    MSG msg;


    // 기본 메시지 루프입니다:
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

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MY230706SECTORMONITOR));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_MY230706SECTORMONITOR);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; 

   g_Window = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 800, 900, nullptr, nullptr, hInstance, nullptr);

   if (!g_Window)
   {
      return FALSE;
   }

   ShowWindow(g_Window, nCmdShow);
   UpdateWindow(g_Window);

   return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    switch (message)
    {
    case WM_CREATE:
    {
        HDC hdcC = GetDC(hWnd);
        GetClientRect(hWnd, &g_MemDCRect);
        g_hMemDCBitmap = CreateCompatibleBitmap(hdcC, g_MemDCRect.right, g_MemDCRect.bottom);
        g_hMemDC = CreateCompatibleDC(hdcC);
        ReleaseDC(hWnd, hdcC);
        g_hMemDCBitmap_old = (HBITMAP)SelectObject(g_hMemDC, g_hMemDCBitmap);

        g_hGridPen = CreatePen(PS_SOLID, 1, RGB(200, 200, 200));




        g_brush[0] = CreateSolidBrush(RGB(255, 255, 255));
        g_brush[1] = CreateSolidBrush(RGB(206, 251, 201));
        g_brush[2] = CreateSolidBrush(RGB(171, 242, 0));
        g_brush[3] = CreateSolidBrush(RGB(255, 187, 0));
        g_brush[4] = CreateSolidBrush(RGB(255, 94, 0));
        g_brush[5] = CreateSolidBrush(RGB(255, 0, 0));

        
    }
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다:
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
        //메모리 DC를 클리어하고
        PatBlt(g_hMemDC, 0, 0, g_MemDCRect.right, g_MemDCRect.bottom, WHITENESS);

        //RenderObstacle, RenderGrid를 메모리 DC에 출력
        RenderSector(g_hMemDC);
        RenderGrid(g_hMemDC);

        //메모리DC에 렌더링이 끝나면 메모리 DC -> 윈도우 DC로의 출력
        //DC에서 DC로의 출력 함수(대상DC, 출력좌표 x y, 출력크기 cx, cy, 소스 DC, 소스좌표 x y, 출력방법
        //를 인자로 넣어 출력가능하다.
        HDC hdc = BeginPaint(hWnd, &ps);
        BitBlt(hdc, 0, 0, g_MemDCRect.right, g_MemDCRect.bottom, g_hMemDC, 0, 0, SRCCOPY);
        EndPaint(hWnd, &ps);
        ReleaseDC(hWnd, hdc);
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

// 정보 대화 상자의 메시지 처리기입니다.
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


void RenderSector(HDC hdc)
{
    int iX = 0;
    int iY = 0;
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, g_brush[0]);
    SelectObject(hdc, GetStockObject(NULL_PEN));

    //사각형의 테두리를 보이지 않도록 하기 위해 NULL_PEN을 지정한다.
    //CreatePen으로 NULL_PEN을 생성해도 되지만
    //GetStockObject를 사용하여 이미 시스템에 만들어져있는 고정 GDI Object를 사용해본다.
    //GetStockObejct는 시스템의 고정적인 범용 GDI Object로서 삭제가 필요하지 않다.
    //시스템 전역적인 GDI Object를 얻어서 사용한다는 개념이다.

    int sectorDistribution[SECTOR_MONITOR_Y_MAX][SECTOR_MONITOR_X_MAX] = { 0 };



    for (int i = 0; i < SECTOR_MONITOR_Y_MAX; i++)
    {
        for (int j = 0; j < SECTOR_MONITOR_X_MAX; j++)
        {
            int* size = &sectorDistribution[i][j];

            for (int k = 0; k < SECTOR_MONITOR; k++)
            {
                for (int m = 0; m < SECTOR_MONITOR; m++)
                {
                    int sectorSize = g_SectorClient.sectorMap_[i * SECTOR_MONITOR + k][j * SECTOR_MONITOR + m];
                    *size += sectorSize;
                }

            }


        }
    }

    for (int iCntW = 0; iCntW < SECTOR_MONITOR_Y_MAX; iCntW++)
    {
        for (int iCntH = 0; iCntH < SECTOR_MONITOR_X_MAX; iCntH++)
        {
            int size = sectorDistribution[iCntW][iCntH];
            if (size > 25)
            {
                SelectObject(hdc, g_brush[5]);
                SetBkColor(hdc, RGB(255, 0, 0));

            }
            else if (size >= 20)
            {
                SelectObject(hdc, g_brush[4]);
                SetBkColor(hdc, RGB(255, 94, 0));
            }
            else if (size >= 15)
            {
                SelectObject(hdc, g_brush[3]);
                SetBkColor(hdc, RGB(255, 187, 0));
            }
            else if (size >= 10)
            {
                SelectObject(hdc, g_brush[2]);
                SetBkColor(hdc, RGB(171, 242, 0));
            }
            else if (size >= 5)
            {
                SelectObject(hdc, g_brush[1]);
                SetBkColor(hdc, RGB(206, 251, 201));
            }
            else
            {
                SelectObject(hdc, g_brush[0]);
                SetBkColor(hdc, RGB(255, 255, 255));
            }

            iY = iCntW * GRID_SIZE;
            iX = iCntH * GRID_SIZE;

            Rectangle(hdc, iY, iX, iY + GRID_SIZE, iX + GRID_SIZE);

            WCHAR BUF[16] = { 0 };
            _itow_s(size, BUF, 10);
            TextOut(hdc, iY + 10, iX + 10, BUF, wcslen(BUF));
        }
    }
    SelectObject(hdc, hOldBrush);
}

void RenderGrid(HDC hdc)
{
    int iX = 0;
    int iY = 0;
    HPEN hOldPen = (HPEN)SelectObject(hdc, g_hGridPen);

    //그리드의 마지막 선을 추가로 그리기 위해 <= 의 반복 조건
    for (int iCntW = 0; iCntW <= SECTOR_MONITOR_X_MAX; iCntW++)
    {
        MoveToEx(hdc, iX, 0, NULL);
        LineTo(hdc, iX, SECTOR_MONITOR_Y_MAX * GRID_SIZE);
        iX += GRID_SIZE;
    }
    for (int iCntH = 0; iCntH <= SECTOR_MONITOR_Y_MAX; iCntH++)
    {
        MoveToEx(hdc, 0, iY, NULL);
        LineTo(hdc, SECTOR_MONITOR_X_MAX * GRID_SIZE, iY);
        iY += GRID_SIZE;
    }

    SelectObject(hdc, hOldPen);
}

