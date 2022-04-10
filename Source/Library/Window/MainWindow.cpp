#include "Window/MainWindow.h"
#include "wchar.h"

namespace library
{
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   MainWindow::Initialize

      Summary:  Initializes main window

      Args:     HINSTANCE hInstance
                  Handle to the instance
                INT nCmdShow
                    Is a flag that says whether the main application window
                    will be minimized, maximized, or shown normally
                PCWSTR pszWindowName
                    The window name

      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: MainWindow::Initialize definition (remove the comment)
    --------------------------------------------------------------------*/
    HRESULT MainWindow::Initialize(_In_ HINSTANCE hInstance, _In_ INT nCmdShow, _In_ PCWSTR pszWindowName) {

        static bool didInitRawInput = false;
        if (!didInitRawInput)
        {
            RAWINPUTDEVICE rid =
            {
                .usUsagePage = 0x01,
                .usUsage = 0x02,
                .dwFlags = 0,
                .hwndTarget = nullptr
            };

            if (!RegisterRawInputDevices(&rid, 1, sizeof(rid))) return E_FAIL;
            didInitRawInput = true;
        }
        RECT rc;
        POINT p1, p2;

        GetClientRect(m_hWnd, &rc);

        p1.x = rc.left;
        p1.y = rc.top;
        p2.x = rc.right;
        p2.y = rc.bottom;

        ClientToScreen(m_hWnd, &p1);
        ClientToScreen(m_hWnd, &p2);

        rc.left = p1.x;
        rc.top = p1.y;
        rc.right = p2.x;
        rc.bottom = p2.y;

        ClipCursor(&rc);

        return BaseWindow::initialize(hInstance,
            nCmdShow,
            pszWindowName,
            WS_VISIBLE | WS_SYSMENU, 0, 0, 800, 600);

    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   MainWindow::GetWindowClassName

      Summary:  Returns the name of the window class

      Returns:  PCWSTR
                  Name of the window class
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: MainWindow::GetWindowClassName definition (remove the comment)
    --------------------------------------------------------------------*/
    PCWSTR MainWindow::GetWindowClassName() const {
        return L"windowClassName";
    }


    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   MainWindow::HandleMessage

      Summary:  Handles the messages

      Args:     UINT uMessage
                  Message code
                WPARAM wParam
                    Additional data the pertains to the message
                LPARAM lParam
                    Additional data the pertains to the message

      Returns:  LRESULT
                  Integer value that your program returns to Windows
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: MainWindow::HandleMessage definition (remove the comment)
    --------------------------------------------------------------------*/
    LRESULT MainWindow::HandleMessage(_In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam) {
        
        switch (uMsg)
        {

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(m_hWnd, &ps);
            FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
            EndPaint(m_hWnd, &ps);
            return 0;
        }
        // ���콺 �Է�ó��
        case WM_INPUT:
        {
            // check
            UINT dwSize;
            static BYTE lpb[sizeof(RAWINPUT)];

            GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));

            RAWINPUT* raw = (RAWINPUT*)lpb;

            if (raw->header.dwType == RIM_TYPEMOUSE)
            {
                m_mouseRelativeMovement.X = raw->data.mouse.lLastX;
                m_mouseRelativeMovement.Y = raw->data.mouse.lLastY;
            }
            //break;
            return 0;
        }
        case WM_KEYDOWN:
        {
            switch (wParam) {
            case 0x57:
                m_directions.bFront = true;
                
                break; //w
            case 0x53:
                m_directions.bBack = true;
                break; //s
            case 0x41:
                m_directions.bLeft = true;
                break;// a
            case 0x44:
                m_directions.bRight = true;
                break;
            case VK_SHIFT:
                m_directions.bDown = true;
                break;
            case VK_SPACE:
                m_directions.bUp = true;
                break;
            }
            return 0;
        }
        case WM_KEYUP:
        {
            switch (wParam) {
            case 0x57: 
                m_directions.bFront = false;
                break; //w
            case 0x53: 
                m_directions.bBack = false; 
                break; //s
            case 0x41: 
                m_directions.bLeft = false;
                break;// a
            case 0x44: 
                m_directions.bRight = false;
                break;
            case VK_SHIFT: 
                m_directions.bDown = false;
                break;
            case VK_SPACE: 
                m_directions.bUp = false;
                break;
            }
            return 0;
        }
        

        
        default:
            return DefWindowProc(m_hWnd, uMsg, wParam, lParam);
        }
        return TRUE;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
     Method:   MainWindow::GetDirections

     Summary:  Returns the keyboard direction input

     Returns:  const DirectionsInput&
                 Keyboard direction input
   M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
   /*--------------------------------------------------------------------
     TODO: MainWindow::GetDirections definition (remove the comment)
   --------------------------------------------------------------------*/
    const DirectionsInput& MainWindow::GetDirections() const
    {
        
        return m_directions;
    }

   /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
     Method:   MainWindow::GetMouseRelativeMovement

     Summary:  Returns the mouse relative movement

     Returns:  const MouseRelativeMovement&
                 Mouse relative movement
   M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
   /*--------------------------------------------------------------------
     TODO: MainWindow::GetMouseRelativeMovement definition (remove the comment)
   --------------------------------------------------------------------*/
    const MouseRelativeMovement& MainWindow::GetMouseRelativeMovement() const
    {
        return m_mouseRelativeMovement;
    }
   /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
     Method:   MainWindow::ResetMouseMovement

     Summary:  Reset the mouse relative movement to zero
   M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
   /*--------------------------------------------------------------------
     TODO: MainWindow::ResetMouseMovement definition (remove the comment)
   --------------------------------------------------------------------*/
    void MainWindow::ResetMouseMovement()
    {
        m_mouseRelativeMovement = { 0,0 };
    }

}
