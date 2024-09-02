﻿// BaseDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "BaseDialog.h"
#include "IniHelper.h"
#include "MusicPlayer2.h"

// CBaseDialog 对话框
std::map<CString, HWND> CBaseDialog::m_unique_hwnd;

IMPLEMENT_DYNAMIC(CBaseDialog, CDialog)

CBaseDialog::CBaseDialog(UINT nIDTemplate, CWnd* pParent /*=NULL*/)
    : CDialog(nIDTemplate, pParent)
{
    m_nDialogID = nIDTemplate;
}

CBaseDialog::~CBaseDialog()
{
}

void CBaseDialog::SetBackgroundColor(COLORREF color, BOOL bRepaint)
{
    if (m_brBkgr.GetSafeHandle() != NULL)
    {
        m_brBkgr.DeleteObject();
    }

    if (color != (COLORREF)-1)
    {
        m_brBkgr.CreateSolidBrush(color);
    }

    if (bRepaint && GetSafeHwnd() != NULL)
    {
        Invalidate();
        UpdateWindow();
    }
}

HWND CBaseDialog::GetUniqueHandel(LPCTSTR dlg_name)
{
    return m_unique_hwnd[dlg_name];
}

const std::map<CString, HWND>& CBaseDialog::AllUniqueHandels()
{
    return m_unique_hwnd;
}

void CBaseDialog::CloseAllWindow()
{
    //确保在退出前关闭所有窗口
    for (const auto& item : AllUniqueHandels())
    {
        ::SendMessage(item.second, WM_COMMAND, IDCANCEL, 0);
    }
}

void CBaseDialog::LoadConfig()
{
    ASSERT(!GetDialogName().IsEmpty());
    ASSERT(IsRememberDialogSizeEnable());
    CIniHelper ini{ theApp.m_config_path };
    //载入窗口大小设置
    m_window_size.cx = ini.GetInt(_T("window_size"), GetDialogName() + _T("_width"), -1);
    m_window_size.cy = ini.GetInt(_T("window_size"), GetDialogName() + _T("_height"), -1);
}

void CBaseDialog::SaveConfig() const
{
    ASSERT(!GetDialogName().IsEmpty());
    ASSERT(IsRememberDialogSizeEnable());
    CIniHelper ini{ theApp.m_config_path };
    //保存窗口大小设置
    ini.WriteInt(_T("window_size"), GetDialogName() + _T("_width"), m_window_size.cx);
    ini.WriteInt(_T("window_size"), GetDialogName() + _T("_height"), m_window_size.cy);
    ini.Save();
}

void CBaseDialog::ReLoadLayoutResource()
{
    ASSERT(m_nDialogID);
    CMFCDynamicLayout* pDynamicLayout = GetDynamicLayout();
    if (pDynamicLayout)
    {
        HRSRC layoutRes = ::FindResourceW(NULL, MAKEINTRESOURCEW(m_nDialogID), RT_DIALOG_LAYOUT);
        if (layoutRes)
        {
            HGLOBAL hResData = ::LoadResource(NULL, layoutRes);
            if (hResData)
            {
                LPVOID layoutResData = ::LockResource(hResData);
                DWORD layoutResSize = ::SizeofResource(NULL, layoutRes);
                // std::wstring data(static_cast<wchar_t*>(layoutResData), layoutResSize / sizeof(wchar_t));
                pDynamicLayout->LoadResource(this, layoutResData, layoutResSize);
            }
        }
    }
}

void CBaseDialog::SetIcon(IconMgr::IconType type, BOOL bBigIcon)
{
    if (bBigIcon)
        CDialog::SetIcon(theApp.m_icon_mgr.GetHICON(type, IconMgr::IconStyle::IS_OutlinedDark, IconMgr::IconSize::IS_ALL), TRUE);
    else
        CDialog::SetIcon(theApp.m_icon_mgr.GetHICON(type, IconMgr::IconStyle::IS_OutlinedDark, IconMgr::IconSize::IS_DPI_16), FALSE);
}

void CBaseDialog::SetButtonIcon(UINT id, IconMgr::IconType type)
{
    CWnd* dlgItem = GetDlgItem(id);
    CButton* close_btn = static_cast<CButton*>(dlgItem);
    if (close_btn != nullptr)
        close_btn->SetIcon(theApp.m_icon_mgr.GetHICON(type, IconMgr::IconStyle::IS_OutlinedDark, IconMgr::IconSize::IS_DPI_16));
}

void CBaseDialog::ShowDlgCtrl(UINT id, bool show)
{
    CWnd* pCtrl = GetDlgItem(id);
    if (pCtrl != nullptr)
        pCtrl->ShowWindow(show);
}

void CBaseDialog::EnableDlgCtrl(UINT id, bool enable)
{
    CWnd* pCtrl = GetDlgItem(id);
    if (pCtrl != nullptr)
        pCtrl->EnableWindow(enable);
}

CRect CBaseDialog::GetTextExtent(const CString& text)
{
    ASSERT(m_pDC != nullptr);   // m_pDC由OnInitDialog负责申请释放
    if (m_pDC == nullptr)
        return CRect();
    if (text.IsEmpty())
        return CRect();
    CRect text_size;
    m_pDC->DrawTextW(text, &text_size, DT_CALCRECT);    // 使用CDC::DrawTextW测量文本宽度（CDC::GetTextExtent是理论宽度，不准确）
    return text_size;
}

void CBaseDialog::RepositionTextBasedControls(const vector<CtrlTextInfo>& items, CtrlTextInfo::Width center_min_width)
{
    ASSERT(m_pDC != nullptr);   // 此方法仅在InitializeControls期间可用
    if (m_pDC == nullptr)
        return;
    int center_width = theApp.DPI(center_min_width);
    std::map<int, pair<int, int>> col_info;
    struct itemInfo
    {
        int col_index;
        CWnd* p;
        CRect rect;
    };
    vector<itemInfo> items_info;
    int center_left{ 0 }, center_right{ INT_MAX };

    for (const auto& item : items)
    {
        ASSERT(item.col_index != CtrlTextInfo::UN_USE);
        ASSERT(item.id != 0);
        // 获取所有列所需dx，以及左贴靠元素的右边缘center_left，右贴靠元素的左边缘center_right
        CWnd* pItem = GetDlgItem(item.id);
        if (pItem == nullptr)
            continue;
        CRect rect{};
        pItem->GetWindowRect(&rect);
        ScreenToClient(&rect);
        CString text;
        pItem->GetWindowTextW(text);
        int dx = GetTextExtent(text).Width() + theApp.DPI(item.ext_width) - rect.Width();
        if (dx < 0) dx = 0;                                     // 文字只增加控件宽度
        if (col_info[item.col_index].first < dx)                // 取此列元素中宽度增长最多的
            col_info[item.col_index].first = dx;
        if (item.col_index < 0 && center_left < rect.right)
            center_left = rect.right;
        if (item.col_index > 0 && center_right > rect.left)
            center_right = rect.left;
        if (item.col_index == 0)    // col_index为0的控件可能有多个
        {
            if(center_left < rect.left)
                center_left = rect.left;
            if(center_right > rect.right)
                center_right = rect.right;
        }
        items_info.emplace_back(itemInfo{ item.col_index, pItem, std::move(rect) });
    }

    if (center_right == INT_MAX)    // 如果控件全部都是左贴靠的那么以窗口右边缘作为剩余空间的右边缘
    {
        CRect dlg_rect{};
        GetClientRect(&dlg_rect);
        center_right = dlg_rect.Width();
    }
    // 此断言触发说明资源文件中的原始布局没有给中间控件/空闲空间留够宽度
    ASSERT(center_right - center_left >= center_width);
    int dx_sum_left{}, dx_sum_right{};
    // 因为同一col_index可以有多个控件&没有要求顺序所以控件的最终位置必须可以无状态的计算出来
    for (auto& a : col_info)
    {
        if (a.first < 0)
        {
            a.second.second = dx_sum_left;    // 存储此列之前控件的总dx，即此列控件的右移距离
            dx_sum_left += a.second.first;
        }
        else if (a.first > 0)
        {
            a.second.second = dx_sum_right;
            dx_sum_right += a.second.first;
        }
    }
    float scale{ 1.0f };
    if (center_right - center_left - dx_sum_left - dx_sum_right < center_width)
    {
        // ASSERT(false);
        // 现在加载的文本使此行的中间控件/空闲空间被挤压的太小
        // 这需要重新设计窗口控件排布以适应当前翻译长度，这里先简单的把缺少的空间分摊给各dx
        scale = static_cast<float>(center_right - center_left - center_width) / (dx_sum_left + dx_sum_right);
        dx_sum_left = static_cast<int>(dx_sum_left * scale + 0.5f);
        dx_sum_right = static_cast<int>(dx_sum_right * scale + 0.5f);
    }
    for (const auto& item : items_info)
    {
        const auto& rect = item.rect;
        int dx = static_cast<int>(col_info[item.col_index].first * scale + 0.5f);
        int sum_dx = static_cast<int>(col_info[item.col_index].second * scale + 0.5f);
        if (item.col_index < 0)
            item.p->SetWindowPos(nullptr, rect.left + sum_dx, rect.top, rect.Width() + dx, rect.Height(), SWP_NOZORDER);
        else if (item.col_index > 0)
            item.p->SetWindowPos(nullptr, rect.left + sum_dx - dx_sum_right, rect.top, rect.Width() + dx, rect.Height(), SWP_NOZORDER);
        else if (item.col_index == 0)
            item.p->SetWindowPos(nullptr, rect.left + dx_sum_left, rect.top, rect.Width() - dx_sum_left - dx_sum_right, rect.Height(), SWP_NOZORDER);
    }
}

void CBaseDialog::SetMinSize(int cx, int cy)
{
    m_min_size.cx = cx;
    m_min_size.cy = cy;
}

void CBaseDialog::SetDlgControlText(int id, const wchar_t* key)
{
    const wstring& temp = theApp.m_str_table.LoadText(key);
    SetDlgItemTextW(id, temp.c_str());
}

void CBaseDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CBaseDialog, CDialog)
    ON_WM_DESTROY()
    ON_WM_GETMINMAXINFO()
    ON_WM_SIZE()
    ON_WM_ERASEBKGND()
    ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CBaseDialog 消息处理程序


BOOL CBaseDialog::OnInitDialog()
{
    if (!GetDialogName().IsEmpty())
    {
        m_unique_hwnd[GetDialogName()] = m_hWnd;
        if (IsRememberDialogSizeEnable())
            CBaseDialog::LoadConfig();          // 载入设置
    }

    CDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化

    //获取初始时窗口的大小
    if (m_min_size.cx <= 0 || m_min_size.cy <= 0)
    {
        CRect rect;
        GetWindowRect(rect);
        m_min_size.cx = rect.Width();
        m_min_size.cy = rect.Height();
    }

    // 通用窗口/控件初始化操作

    // 设置窗口字体
    CCommon::SetDialogFont(this, &theApp.m_font_set.dlg.GetFont());
    //为按钮添加图标
    SetButtonIcon(IDCANCEL, IconMgr::IconType::IT_Cancel);
    SetButtonIcon(IDOK, IconMgr::IconType::IT_Ok);
    // 设置按钮文本
    wstring temp;
    temp = theApp.m_str_table.LoadText(L"TXT_OK");
    SetDlgItemTextW(IDOK, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_CANCEL");
    SetDlgItemTextW(IDCANCEL, temp.c_str());

    // 在还原窗口大小之前（当前窗口状态与资源一致），派生类执行控件文本初始化及调整控件排布
    // 与实际窗口大小相关的初始化（比如表格列宽）应在派生类的OnInitDialog进行
    m_pDC = GetDC();
    m_pDC->SelectObject(&theApp.m_font_set.dlg.GetFont());
    bool rtn = InitializeControls();
    ReleaseDC(m_pDC);
    m_pDC = nullptr;
    // 如果更改了控件排布那么应当返回true以向布局管理器应用控件调整（重新加载动态布局设置）
    if (rtn)
        ReLoadLayoutResource();

    //初始化窗口大小
    if (m_window_size.cx > 0 && m_window_size.cy > 0)
    {
        SetWindowPos(nullptr, 0, 0, m_window_size.cx, m_window_size.cy, SWP_NOZORDER | SWP_NOMOVE);
    }

    return TRUE;    // return TRUE unless you set the focus to a control
                    // 异常: OCX 属性页应返回 FALSE
}


void CBaseDialog::OnDestroy()
{
    CDialog::OnDestroy();

    // TODO: 在此处添加消息处理程序代码
    if (!GetDialogName().IsEmpty())
    {
        m_unique_hwnd[GetDialogName()] = NULL;
        if (IsRememberDialogSizeEnable())
            CBaseDialog::SaveConfig();
    }
}


void CBaseDialog::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    //限制窗口最小大小
    lpMMI->ptMinTrackSize.x = m_min_size.cx;        //设置最小宽度
    lpMMI->ptMinTrackSize.y = m_min_size.cy;        //设置最小高度

    CDialog::OnGetMinMaxInfo(lpMMI);
}


void CBaseDialog::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);

    // TODO: 在此处添加消息处理程序代码
    if (nType != SIZE_MAXIMIZED && nType != SIZE_MINIMIZED)
    {
        CRect rect;
        GetWindowRect(&rect);
        m_window_size.cx = rect.Width();
        m_window_size.cy = rect.Height();
    }

}


INT_PTR CBaseDialog::DoModal()
{
    if (!GetDialogName().IsEmpty())
    {
        HWND unique_hwnd{ m_unique_hwnd[GetDialogName()] };
        if (unique_hwnd != NULL)      ///如果对话框已存在，则显示已存在的对话框
        {
            ::ShowWindow(unique_hwnd, SW_RESTORE);
            ::SetForegroundWindow(unique_hwnd);
            return 0;
        }
    }
    return CDialog::DoModal();
}


BOOL CBaseDialog::OnEraseBkgnd(CDC* pDC)
{
    // 修改窗口背景（CDialogEx）
    if (m_brBkgr.GetSafeHandle() != NULL)
    {
        ASSERT_VALID(pDC);
        CRect rectClient;
        GetClientRect(rectClient);
        pDC->FillRect(rectClient, &m_brBkgr);
        return TRUE;
    }
    return CDialog::OnEraseBkgnd(pDC);
}


HBRUSH CBaseDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    // 修改窗口背景（CDialogEx）
    if (m_brBkgr.GetSafeHandle() != NULL)
    {
#define AFX_MAX_CLASS_NAME 255
#define AFX_STATIC_CLASS _T("Static")
#define AFX_BUTTON_CLASS _T("Button")
// 上面两个是CDialogEx原有的(因为这两种比较原生，类名不在rc文件中直接出现？)，下面是我自己加的
#define AFX_SLIDER_CLASS _T("msctls_trackbar32")    // 滑动条控件CSliderCtrl及其派生类
#define AFX_SYSLINK_CLASS _T("SysLink")             // 超链接控件CSysLink及其派生类

        if (nCtlColor == CTLCOLOR_STATIC)
        {
            TCHAR lpszClassName[AFX_MAX_CLASS_NAME + 1];

            ::GetClassName(pWnd->GetSafeHwnd(), lpszClassName, AFX_MAX_CLASS_NAME);
            CString strClass = lpszClassName;

            if (strClass == AFX_BUTTON_CLASS || strClass == AFX_STATIC_CLASS || strClass == AFX_SLIDER_CLASS || strClass == AFX_SYSLINK_CLASS)
            {
                pDC->SetBkMode(TRANSPARENT);

                if (m_brBkgr.GetSafeHandle() != NULL && IsAppThemed())
                {
                    return (HBRUSH)m_brBkgr.GetSafeHandle();
                }
                else
                {
                    return (HBRUSH)::GetStockObject(HOLLOW_BRUSH);
                }
            }
        }
    }

    return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
}
