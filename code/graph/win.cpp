#include "private.h"

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

win_t* win_create(graphics_t* graphics) {

    win_t* win = (win_t*)malloc(sizeof(win_t));
    win->engine = engine_create(graphics);

    static int number = 0;
    number++;

    const wchar_t* name = L"Graph";
    wchar_t buffer[20];
    wsprintf(buffer, L"%s%i", name, number);

    WNDCLASSEXW wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = NULL;
    wcex.hIcon = NULL;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = buffer;
    wcex.hIconSm = NULL; 

    RegisterClassExW(&wcex);

    HWND hWnd = CreateWindowW(
        buffer,  // classname
        buffer,  // windowname
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,    // x-pos 
        CW_USEDEFAULT,    // y-pos 
        (int) graphics->size.width,
        (int) graphics->size.height,
        nullptr,
        nullptr,
        NULL,
        win);       // lparam WM_CREATE

    ShowWindow(hWnd, SW_SHOWNORMAL);
    UpdateWindow(hWnd);

    if (graphics->timerTimeMillis > 0) {
        SetTimer(hWnd, 0, graphics->timerTimeMillis, NULL);
    }

    win->hWnd = hWnd;
    return win;
}


void win_message_loop() {
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {

    if (message == WM_CREATE) {

        LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
        win_t* win = (win_t*)pcs->lpCreateParams;

        // save as user data
        ::SetWindowLongPtrW(
            hwnd,
            GWLP_USERDATA,
            reinterpret_cast<LONG_PTR>(win)
        );

        engine_create_target(win->engine, hwnd);
        engine_create_brush(win->engine);
        engine_create_text(win->engine);
        return 0;

    }

    // load user data
    win_t* win = reinterpret_cast<win_t*>(
        ::GetWindowLongPtrW(
            hwnd,
            GWLP_USERDATA
        ));



    switch (message) {

    case WM_CLOSE:  
        DestroyWindow(hwnd);
        return 0;

    case WM_SIZE:
    {  // bock is must
        UINT width = LOWORD(lParam);
        UINT height = HIWORD(lParam);
        engine_window_resized(win->engine, width, height);
    }
    return 0;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        BeginPaint(hwnd, &ps);   // without WM_PAINT is repeated (?)
        engine_paint(win->engine);
        EndPaint(hwnd, &ps);
    }
    return 0;

    case WM_TIMER:
    {
        InvalidateRect(hwnd, NULL, false);
    }
    return 0;

    case WM_DESTROY:
        engine_release(win->engine);
        PostQuitMessage(0);
        return 0;
    }

    // all other messages
    return DefWindowProc(hwnd, message, wParam, lParam);
}
