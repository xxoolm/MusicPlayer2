﻿// AddToPlaylistDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "AddToPlaylistDlg.h"
#include "PlaylistMgr.h"
#include "FilePathHelper.h"


// CAddToPlaylistDlg 对话框

IMPLEMENT_DYNAMIC(CAddToPlaylistDlg, CBaseDialog)

CAddToPlaylistDlg::CAddToPlaylistDlg(CWnd* pParent /*=nullptr*/)
	: CBaseDialog(IDD_ADD_TO_PLAYLIST_DIALOG, pParent)
{

}

CAddToPlaylistDlg::~CAddToPlaylistDlg()
{
}

CString CAddToPlaylistDlg::GetDialogName() const
{
    return _T("AddToPlaylistDlg");
}

bool CAddToPlaylistDlg::InitializeControls()
{
    wstring temp;
    temp = theApp.m_str_table.LoadText(L"TITLE_ADD_TO_PLAYLIST");
    SetWindowTextW(temp.c_str());

    ShowDlgCtrl(IDC_DELETE_BUTTON, false);  //隐藏“删除”按钮

    RepositionTextBasedControls({
        { CtrlTextInfo::R1, IDOK, CtrlTextInfo::W32 },
        { CtrlTextInfo::R2, IDCANCEL, CtrlTextInfo::W32 }
        });
    return true;
}

void CAddToPlaylistDlg::ShowList()
{
    m_playlist_list_ctrl.DeleteAllItems();
    auto& data_list{ m_searched ? m_search_result : m_list };
    for (const auto& item : data_list)
    {
        m_playlist_list_ctrl.AddString(item.c_str());
    }
}

void CAddToPlaylistDlg::DoDataExchange(CDataExchange* pDX)
{
    CBaseDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST1, m_playlist_list_ctrl);
    DDX_Control(pDX, IDC_SEARCH_EDIT, m_search_edit);
}


BEGIN_MESSAGE_MAP(CAddToPlaylistDlg, CBaseDialog)
    ON_WM_GETMINMAXINFO()
    ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CAddToPlaylistDlg::OnNMDblclkList1)
    ON_EN_CHANGE(IDC_SEARCH_EDIT, &CAddToPlaylistDlg::OnEnChangeSearchEdit)
    ON_MESSAGE(WM_SEARCH_EDIT_BTN_CLICKED, &CAddToPlaylistDlg::OnSearchEditBtnClicked)
END_MESSAGE_MAP()


// CAddToPlaylistDlg 消息处理程序


BOOL CAddToPlaylistDlg::OnInitDialog()
{
    CBaseDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化

    SetIcon(IconMgr::IconType::IT_Playlist, FALSE);     // 设置小图标

    m_search_edit.SetCueBanner(theApp.m_str_table.LoadText(L"TXT_SEARCH_PROMPT").c_str(), TRUE);

    //初始化列表
    CPlaylistMgr::Instance().IterateItemsWithoutSpecialPlaylist([&](PlaylistInfo& item) {
        CFilePathHelper playlist_path{ item.path };
        m_list.push_back(playlist_path.GetFileNameWithoutExtension());
    });
    ShowList();

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void CAddToPlaylistDlg::OnOK()
{
    // TODO: 在此添加专用代码和/或调用基类
    int index = m_playlist_list_ctrl.GetCurSel();
    m_playlist_selected = m_playlist_list_ctrl.GetItemText(index);

    CBaseDialog::OnOK();
}


void CAddToPlaylistDlg::OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    m_playlist_selected = m_playlist_list_ctrl.GetItemText(pNMItemActivate->iItem);
    CBaseDialog::OnOK();

    *pResult = 0;
}


void CAddToPlaylistDlg::OnEnChangeSearchEdit()
{
    // TODO:  如果该控件是 RICHEDIT 控件，它将不
    // 发送此通知，除非重写 CBaseDialog::OnInitDialog()
    // 函数并调用 CRichEditCtrl().SetEventMask()，
    // 同时将 ENM_CHANGE 标志“或”运算到掩码中。

    // TODO:  在此添加控件通知处理程序代码
    CString str;
    m_search_edit.GetWindowText(str);
    QuickSearch(wstring(str));
    m_searched = !str.IsEmpty();
    ShowList();
}


afx_msg LRESULT CAddToPlaylistDlg::OnSearchEditBtnClicked(WPARAM wParam, LPARAM lParam)
{
    CSearchEditCtrl* pEdit = (CSearchEditCtrl*)wParam;
    if (pEdit == &m_search_edit)
    {
        if (m_searched)
        {
            //清除搜索结果
            m_searched = false;
            m_search_edit.SetWindowText(_T(""));
            ShowList();
        }
    }
    return 0;
}

void CAddToPlaylistDlg::QuickSearch(const wstring& key_word)
{
    m_search_result.clear();
    for (const auto& str : m_list)
    {
        if (theApp.m_chinese_pingyin_res.IsStringMatchWithPingyin(key_word, str))
        {
            m_search_result.push_back(str);
        }
    }
}
