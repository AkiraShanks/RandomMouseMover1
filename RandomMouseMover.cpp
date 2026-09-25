#include <afxwin.h>
#include <afxdlgs.h>
#include <windows.h>
#include <random>
#include "resource.h"

#define ID_TIMER_MOVE 1001

class CMainDlg : public CDialog
{
public:
    CMainDlg() : CDialog(IDD_MAIN), m_running(false) {}

protected:
    bool m_running;

    BOOL OnInitDialog() override
    {
        CDialog::OnInitDialog();
        SetWindowTextW(L"Random Mouse Mover");
        return TRUE;
    }

    void SetDisplayAwake(bool awake)
    {
        if (awake)
        {
            SetThreadExecutionState(
                ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_DISPLAY_REQUIRED);
        }
        else
        {
            SetThreadExecutionState(ES_CONTINUOUS);
        }
    }

    void MoveMouseRandomly()
    {
        RECT workArea{};
        if (!SystemParametersInfoW(SPI_GETWORKAREA, 0, &workArea, 0))
            return;

        static std::mt19937 generator{ std::random_device{}() };

        const int left = workArea.left;
        const int right = workArea.right - 1;
        const int top = workArea.top;
        const int bottom = workArea.bottom - 1;

        if (right < left || bottom < top)
            return;

        std::uniform_int_distribution<int> x(left, right);
        std::uniform_int_distribution<int> y(top, bottom);

        SetCursorPos(x(generator), y(generator));
    }

    void StartMover()
    {
        if (m_running)
            return;

        m_running = true;
        SetDisplayAwake(true);
        MoveMouseRandomly();
        SetTimer(ID_TIMER_MOVE, 30000, nullptr);
    }

    void StopMover()
    {
        if (!m_running)
            return;

        KillTimer(ID_TIMER_MOVE);
        m_running = false;
        SetDisplayAwake(false);
    }

    afx_msg void OnTimer(UINT_PTR timerId)
    {
        if (timerId == ID_TIMER_MOVE && m_running)
            MoveMouseRandomly();

        CDialog::OnTimer(timerId);
    }

    afx_msg void OnTurnOn()
    {
        StartMover();
    }

    afx_msg void OnTurnOff()
    {
        StopMover();
    }

    void OnCancel() override
    {
        StopMover();
        CDialog::OnCancel();
    }

    DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CMainDlg, CDialog)
    ON_WM_TIMER()
    ON_COMMAND(ID_BUTTON_ON, &CMainDlg::OnTurnOn)
    ON_COMMAND(ID_BUTTON_OFF, &CMainDlg::OnTurnOff)
END_MESSAGE_MAP()

class CRandomMouseMoverApp : public CWinApp
{
public:
    BOOL InitInstance() override
    {
        CWinApp::InitInstance();
        CMainDlg dialog;
        m_pMainWnd = &dialog;
        dialog.DoModal();
        return FALSE;
    }
};

CRandomMouseMoverApp theApp;
