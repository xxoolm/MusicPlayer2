﻿// LyricDownloadDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "Player.h"
#include "LyricDownloadDlg.h"
#include "MessageDlg.h"
#include "SongInfoHelper.h"
#include "SongDataManager.h"
#include "FilterHelper.h"
#include "IniHelper.h"


// CLyricDownloadDlg 对话框

IMPLEMENT_DYNAMIC(CLyricDownloadDlg, CBaseDialog)

CLyricDownloadDlg::CLyricDownloadDlg(CWnd* pParent /*=NULL*/)
	: CBaseDialog(IDD_LYRIC_DOWNLOAD_DIALOG, pParent)
{

}

CLyricDownloadDlg::~CLyricDownloadDlg()
{
}


void CLyricDownloadDlg::ShowDownloadList()
{
	m_down_list_ctrl.DeleteAllItems();
	for (size_t i{}; i < m_down_list.size(); i++)
	{
		CString tmp;
		tmp.Format(_T("%d"), i + 1);
		m_down_list_ctrl.InsertItem(i, tmp);
		m_down_list_ctrl.SetItemText(i, 1, m_down_list[i].title.c_str());
		m_down_list_ctrl.SetItemText(i, 2, m_down_list[i].artist.c_str());
		m_down_list_ctrl.SetItemText(i, 3, m_down_list[i].album.c_str());
		m_down_list_ctrl.SetItemText(i, 4, Time(m_down_list[i].duration).toString().c_str());
	}
}

bool CLyricDownloadDlg::SaveLyric(const wchar_t * path, CodeType code_type)
{
	bool char_connot_convert;
	string lyric_str = CCommon::UnicodeToStr(m_lyric_str, code_type, &char_connot_convert);
	if (char_connot_convert)	//当文件中包含Unicode字符时，询问用户是否要选择一个Unicode编码格式再保存
	{
        const wstring& info = theApp.m_str_table.LoadText(L"MSG_UNICODE_WARNING"); // 从string table载入字符串
        if (MessageBox(info.c_str(), NULL, MB_OKCANCEL | MB_ICONWARNING) != IDOK)
            return false;        // 如果用户点击了取消按钮，则返回false
	}

	ofstream out_put{ path, std::ios::binary };
    if (!out_put.is_open())
    {
        out_put.close();
        const wstring& info = theApp.m_str_table.LoadText(L"MSG_LYRIC_SAVE_FAILED");
        MessageBox(info.c_str(), NULL, MB_ICONWARNING | MB_OK);
        return false;
	}
	out_put << lyric_str;
    out_put.close();
	return true;
}

void CLyricDownloadDlg::SetID(wstring id)
{
    m_song.SetSongId(id);
    CSongDataManager::GetInstance().SetSongID(m_song, m_song.song_id);
}

void CLyricDownloadDlg::SaveConfig() const
{
	CIniHelper ini(theApp.m_config_path);
	ini.WriteBool(L"lyric_download", L"download_translate", m_download_translate);
	ini.WriteInt(L"lyric_download", L"save_as_utf8", static_cast<int>(m_save_code));
	ini.WriteBool(L"lyric_download", L"save_to_song_folder", m_save_to_song_folder);
	ini.WriteInt(L"lyric_download", L"search_max_item", m_search_max_item);
	ini.Save();
}

void CLyricDownloadDlg::LoadConfig()
{
	CIniHelper ini(theApp.m_config_path);
	m_download_translate = ini.GetBool(L"lyric_download", L"download_translate", true);
	m_save_code = static_cast<CodeType>(ini.GetInt(L"lyric_download", L"save_as_utf8", 1));
	m_save_to_song_folder = ini.GetBool(L"lyric_download", L"save_to_song_folder", true);
	m_search_max_item = ini.GetInt(L"lyric_download", L"search_max_item", 30);
}

wstring CLyricDownloadDlg::GetSavedDir()
{
    if (m_save_to_song_folder || !CCommon::FolderExist(theApp.m_lyric_setting_data.lyric_path))
        return CFilePathHelper(m_song.file_path).GetDir();
    else
        return theApp.m_lyric_setting_data.lyric_path;
}

wstring CLyricDownloadDlg::GetSavedPath()
{
    return GetSavedDir() + m_lyric_name + L".lrc";
}

CString CLyricDownloadDlg::GetDialogName() const
{
    return _T("LyricDownloadDlg");
}

bool CLyricDownloadDlg::InitializeControls()
{
    wstring temp;
    temp = theApp.m_str_table.LoadText(L"TITLE_LYRIC_DL");
    SetWindowTextW(temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_LYRIC_DL_TITLE");
    SetDlgItemTextW(IDC_TXT_LYRIC_DL_TITLE_STATIC, temp.c_str());
    // IDC_TITLE_EDIT1
    temp = theApp.m_str_table.LoadText(L"TXT_LYRIC_DL_SEARCH");
    SetDlgItemTextW(IDC_SEARCH_BUTTON2, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_LYRIC_DL_ARTIST");
    SetDlgItemTextW(IDC_TXT_LYRIC_DL_ARTIST_STATIC, temp.c_str());
    // IDC_ARTIST_EDIT1
    temp = theApp.m_str_table.LoadText(L"TXT_LYRIC_DL_INFO");
    SetDlgItemTextW(IDC_STATIC_INFO, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_LYRIC_DL_UNLINK");
    SetDlgItemTextW(IDC_UNASSOCIATE_LINK, temp.c_str());
    // IDC_LYRIC_DOWN_LIST1
    temp = theApp.m_str_table.LoadText(L"TXT_LYRIC_DL_OPT");
    SetDlgItemTextW(IDC_TXT_LYRIC_DL_OPT_STATIC, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_LYRIC_DL_WITH_TRANSLATION");
    SetDlgItemTextW(IDC_DOWNLOAD_TRANSLATE_CHECK1, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_LYRIC_DL_SAVE_ENCODE_SEL");
    SetDlgItemTextW(IDC_TXT_LYRIC_DL_SAVE_ENCODE_SEL_STATIC, temp.c_str());
    // IDC_COMBO2
    temp = theApp.m_str_table.LoadText(L"TXT_LYRIC_DL_SAVE_DIR_SEL");
    SetDlgItemTextW(IDC_TXT_LYRIC_DL_SAVE_DIR_SEL_STATIC, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_LYRIC_DL_SAVE_DIR_LYRIC");
    SetDlgItemTextW(IDC_SAVE_TO_LYRIC_FOLDER1, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_LYRIC_DL_SAVE_DIR_SONG");
    SetDlgItemTextW(IDC_SAVE_TO_SONG_FOLDER1, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_LYRIC_DL_SEL_DL");
    SetDlgItemTextW(IDC_DOWNLOAD_SELECTED, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_LYRIC_DL_SEL_SAVE_AS");
    SetDlgItemTextW(IDC_SELECTED_SAVE_AS, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_CLOSE");
    SetDlgItemTextW(IDCANCEL, temp.c_str());

    RepositionTextBasedControls({
        { CtrlTextInfo::L1, IDC_TXT_LYRIC_DL_TITLE_STATIC },
        { CtrlTextInfo::C0, IDC_TITLE_EDIT1 },
        { CtrlTextInfo::R1, IDC_SEARCH_BUTTON2, CtrlTextInfo::W32 },
        { CtrlTextInfo::L1, IDC_TXT_LYRIC_DL_ARTIST_STATIC },
        { CtrlTextInfo::C0, IDC_ARTIST_EDIT1 }
        }, CtrlTextInfo::W64);
    RepositionTextBasedControls({
        { CtrlTextInfo::R1, IDC_DOWNLOAD_SELECTED, CtrlTextInfo::W32 },
        { CtrlTextInfo::R2, IDC_SELECTED_SAVE_AS, CtrlTextInfo::W32 },
        { CtrlTextInfo::R3, IDCANCEL, CtrlTextInfo::W32 }
        });
    return true;
}

void CLyricDownloadDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LYRIC_DOWN_LIST1, m_down_list_ctrl);
	DDX_Control(pDX, IDC_DOWNLOAD_TRANSLATE_CHECK1, m_download_translate_chk);
	DDX_Control(pDX, IDC_COMBO2, m_save_code_combo);
	DDX_Control(pDX, IDC_UNASSOCIATE_LINK, m_unassciate_lnk);
}


BEGIN_MESSAGE_MAP(CLyricDownloadDlg, CBaseDialog)
	ON_BN_CLICKED(IDC_SEARCH_BUTTON2, &CLyricDownloadDlg::OnBnClickedSearchButton2)
	ON_EN_CHANGE(IDC_TITLE_EDIT1, &CLyricDownloadDlg::OnEnChangeTitleEdit1)
	ON_EN_CHANGE(IDC_ARTIST_EDIT1, &CLyricDownloadDlg::OnEnChangeArtistEdit1)
	ON_NOTIFY(NM_CLICK, IDC_LYRIC_DOWN_LIST1, &CLyricDownloadDlg::OnNMClickLyricDownList1)
	ON_NOTIFY(NM_RCLICK, IDC_LYRIC_DOWN_LIST1, &CLyricDownloadDlg::OnNMRClickLyricDownList1)
	ON_BN_CLICKED(IDC_DOWNLOAD_SELECTED, &CLyricDownloadDlg::OnBnClickedDownloadSelected)
	ON_BN_CLICKED(IDC_DOWNLOAD_TRANSLATE_CHECK1, &CLyricDownloadDlg::OnBnClickedDownloadTranslateCheck1)
	ON_WM_DESTROY()
	ON_MESSAGE(WM_SEARCH_COMPLATE, &CLyricDownloadDlg::OnSearchComplate)
	ON_MESSAGE(WM_DOWNLOAD_COMPLATE, &CLyricDownloadDlg::OnDownloadComplate)
	ON_BN_CLICKED(IDC_SAVE_TO_SONG_FOLDER1, &CLyricDownloadDlg::OnBnClickedSaveToSongFolder1)
	ON_BN_CLICKED(IDC_SAVE_TO_LYRIC_FOLDER1, &CLyricDownloadDlg::OnBnClickedSaveToLyricFolder1)
	ON_BN_CLICKED(IDC_SELECTED_SAVE_AS, &CLyricDownloadDlg::OnBnClickedSelectedSaveAs)
	ON_CBN_SELCHANGE(IDC_COMBO2, &CLyricDownloadDlg::OnCbnSelchangeCombo2)
	ON_COMMAND(ID_LD_LYRIC_DOWNLOAD, &CLyricDownloadDlg::OnLdLyricDownload)
	ON_COMMAND(ID_LD_LYRIC_SAVEAS, &CLyricDownloadDlg::OnLdLyricSaveas)
	ON_COMMAND(ID_LD_COPY_TITLE, &CLyricDownloadDlg::OnLdCopyTitle)
	ON_COMMAND(ID_LD_COPY_ARTIST, &CLyricDownloadDlg::OnLdCopyArtist)
	ON_COMMAND(ID_LD_COPY_ALBUM, &CLyricDownloadDlg::OnLdCopyAlbum)
	ON_COMMAND(ID_LD_COPY_ID, &CLyricDownloadDlg::OnLdCopyId)
	ON_COMMAND(ID_LD_VIEW_ONLINE, &CLyricDownloadDlg::OnLdViewOnline)
	ON_NOTIFY(NM_DBLCLK, IDC_LYRIC_DOWN_LIST1, &CLyricDownloadDlg::OnNMDblclkLyricDownList1)
	ON_NOTIFY(NM_CLICK, IDC_UNASSOCIATE_LINK, &CLyricDownloadDlg::OnNMClickUnassociateLink)
	ON_COMMAND(ID_LD_PREVIEW, &CLyricDownloadDlg::OnLdPreview)
    ON_COMMAND(ID_LD_RELATE, &CLyricDownloadDlg::OnLdRelate)
END_MESSAGE_MAP()


// CLyricDownloadDlg 消息处理程序


bool CLyricDownloadDlg::IsItemSelectedValid() const
{
	return (m_item_selected >= 0 && m_item_selected < static_cast<int>(m_down_list.size()));
}

BOOL CLyricDownloadDlg::OnInitDialog()
{
	CBaseDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	LoadConfig();

    SetIcon(IconMgr::IconType::IT_Download, FALSE);
    SetButtonIcon(IDC_SEARCH_BUTTON2, IconMgr::IconType::IT_Find);
    SetButtonIcon(IDC_DOWNLOAD_SELECTED, IconMgr::IconType::IT_Download);
    SetButtonIcon(IDC_SELECTED_SAVE_AS, IconMgr::IconType::IT_Save_As);

    m_song = CPlayer::GetInstance().GetCurrentSongInfo();

	if (m_song.IsTitleEmpty())		//如果没有标题信息，就把文件名设为标题
	{
        m_song.title = m_song.GetFileName();
		size_t index = m_song.title.rfind(L'.');
		m_song.title = m_song.title.substr(0, index);
	}
	if (m_song.IsArtistEmpty())	//没有艺术家信息，清空艺术家的文本
	{
		m_song.artist.clear();
	}
	if (m_song.IsAlbumEmpty())	//没有唱片集信息，清空唱片集的文本
	{
		m_song.album.clear();
	}

    if (m_song.is_cue || CPlayer::GetInstance().IsOsuFile())
    {
        m_lyric_name = CSongInfoHelper::GetDisplayStr(m_song, DF_ARTIST_TITLE);
        CCommon::FileNameNormalize(m_lyric_name);
    }
    else
    {
        m_lyric_name = m_song.GetFileName();
        m_lyric_name = CFilePathHelper(m_lyric_name).ReplaceFileExtension(nullptr);	//清除文件名的扩展名
    }
	//if (!song.is_cue)
	//else
	//	m_lyric_name = song.artist + L" - " + song.title + L".lrc";
	//m_file_path = CPlayer::GetInstance().GetCurrentDir() + m_lyric_name;
	//if (!song.is_cue)
	//{
	//}

	SetDlgItemText(IDC_TITLE_EDIT1, m_song.title.c_str());
	SetDlgItemText(IDC_ARTIST_EDIT1, m_song.artist.c_str());

	//设置列表控件主题颜色
	//m_down_list_ctrl.SetColor(theApp.m_app_setting_data.theme_color);

	//初始化搜索结果列表控件
	CRect rect;
	m_down_list_ctrl.GetWindowRect(rect);
	int width0, width1, width2, width3, width4;
	width0 = rect.Width() / 10;
	width1 = rect.Width() * 3 / 10;
	width2 = rect.Width() * 2 / 10;
    width4 = rect.Width() / 10;
	width3 = rect.Width() - theApp.DPI(20) - 1 - width0 - width1 - width2 - width4;

    m_down_list_ctrl.SetExtendedStyle(m_down_list_ctrl.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_LABELTIP);
	m_down_list_ctrl.InsertColumn(0, theApp.m_str_table.LoadText(L"TXT_SERIAL_NUMBER").c_str(), LVCFMT_LEFT, width0);
	m_down_list_ctrl.InsertColumn(1, theApp.m_str_table.LoadText(L"TXT_TITLE").c_str(), LVCFMT_LEFT, width1);
	m_down_list_ctrl.InsertColumn(2, theApp.m_str_table.LoadText(L"TXT_ARTIST").c_str(), LVCFMT_LEFT, width2);
	m_down_list_ctrl.InsertColumn(3, theApp.m_str_table.LoadText(L"TXT_ALBUM").c_str(), LVCFMT_LEFT, width3);
	m_down_list_ctrl.InsertColumn(4, theApp.m_str_table.LoadText(L"TXT_LENGTH").c_str(), LVCFMT_LEFT, width4);

	//设置列表控件的提示总是置顶，用于解决如果弹出此窗口的父窗口具有置顶属性时，提示信息在窗口下面的问题
	m_down_list_ctrl.GetToolTips()->SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	//m_tool_tip.Create(this, TTS_ALWAYSTIP);

	//初始化下载选项中控件的状态
	m_download_translate_chk.SetCheck(m_download_translate);
	m_save_code_combo.AddString(_T("ANSI"));
	m_save_code_combo.AddString(_T("UTF-8"));
	m_save_code_combo.SetCurSel(static_cast<int>(m_save_code));
	if (m_save_to_song_folder)
		((CButton*)GetDlgItem(IDC_SAVE_TO_SONG_FOLDER1))->SetCheck(TRUE);
	else
		((CButton*)GetDlgItem(IDC_SAVE_TO_LYRIC_FOLDER1))->SetCheck(TRUE);

	//判断歌词文件夹是否存在
	bool lyric_path_exist = CCommon::FolderExist(theApp.m_lyric_setting_data.lyric_path);
	if (!lyric_path_exist)		//如果歌词文件不存在，则禁用“保存到歌词文件夹”单选按钮，并强制选中“保存到歌曲所在目录”
	{
		((CButton*)GetDlgItem(IDC_SAVE_TO_LYRIC_FOLDER1))->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_SAVE_TO_LYRIC_FOLDER1))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_SAVE_TO_SONG_FOLDER1))->SetCheck(TRUE);
		m_save_to_song_folder = true;
	}

	m_unassciate_lnk.ShowWindow(SW_HIDE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CLyricDownloadDlg::OnBnClickedSearchButton2()
{
	// TODO: 在此添加控件通知处理程序代码
    SetDlgItemText(IDC_STATIC_INFO, theApp.m_str_table.LoadText(L"TXT_LYRIC_DL_INFO_SEARCHING").c_str());
	GetDlgItem(IDC_SEARCH_BUTTON2)->EnableWindow(FALSE);		//点击“搜索”后禁用该按钮
	wstring keyword = CInternetCommon::URLEncode(m_song.artist + L' ' + m_song.title);	//搜索关键字为“艺术家 标题”，并将其转换成URL编码
	CString url;
	url.Format(L"http://music.163.com/api/search/get/?s=%s&limit=%d&type=1&offset=0", keyword.c_str(), m_search_max_item);
	//int rtn = CLyricDownloadCommon::HttpPost(buff, m_search_result);		//向网易云音乐的歌曲搜索API发送http的POST请求
	m_search_thread_info.url = url;
	m_search_thread_info.hwnd = GetSafeHwnd();
	theApp.m_lyric_download_dialog_exit = false;
	m_pSearchThread = AfxBeginThread(LyricSearchThreadFunc, &m_search_thread_info);
}


void CLyricDownloadDlg::OnEnChangeTitleEdit1()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CBaseDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	CString tmp;
	GetDlgItemText(IDC_TITLE_EDIT1, tmp);
	m_song.title = tmp;
}


void CLyricDownloadDlg::OnEnChangeArtistEdit1()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CBaseDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	CString tmp;
	GetDlgItemText(IDC_ARTIST_EDIT1, tmp);
	m_song.artist = tmp;
}


void CLyricDownloadDlg::OnNMClickLyricDownList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	m_item_selected = pNMItemActivate->iItem;
	*pResult = 0;
}


void CLyricDownloadDlg::OnNMRClickLyricDownList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	m_item_selected = pNMItemActivate->iItem;

	if (IsItemSelectedValid())
	{
		//弹出右键菜单
        CMenu* pContextMenu = theApp.m_menu_mgr.GetMenu(MenuMgr::LdListMenu);
        m_down_list_ctrl.ShowPopupMenu(pContextMenu, pNMItemActivate->iItem, this);
    }

	*pResult = 0;
}


void CLyricDownloadDlg::OnBnClickedDownloadSelected()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!IsItemSelectedValid())
		return;

	GetDlgItem(IDC_DOWNLOAD_SELECTED)->EnableWindow(FALSE);		//点击“下载选中项”后禁用该按钮
	GetDlgItem(IDC_SELECTED_SAVE_AS)->EnableWindow(FALSE);		//点击“下载选中项”后禁用该按钮
    SetID(m_down_list[m_item_selected].id);                     // 将选中项目的歌曲ID关联到歌曲
	m_download_thread_info.hwnd = GetSafeHwnd();
	m_download_thread_info.download_translate = m_download_translate;
	m_download_thread_info.save_as = false;
	m_download_thread_info.song_id = m_down_list[m_item_selected].id;
	m_pDownThread = AfxBeginThread(LyricDownloadThreadFunc, &m_download_thread_info);
}


void CLyricDownloadDlg::OnBnClickedSelectedSaveAs()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!IsItemSelectedValid())
		return;

	GetDlgItem(IDC_DOWNLOAD_SELECTED)->EnableWindow(FALSE);		//点击“下载选中项”后禁用该按钮
	GetDlgItem(IDC_SELECTED_SAVE_AS)->EnableWindow(FALSE);		//点击“下载选中项”后禁用该按钮
	m_download_thread_info.hwnd = GetSafeHwnd();
	m_download_thread_info.download_translate = m_download_translate;
	m_download_thread_info.save_as = true;
	m_download_thread_info.song_id = m_down_list[m_item_selected].id;
	m_pDownThread = AfxBeginThread(LyricDownloadThreadFunc, &m_download_thread_info);
}


void CLyricDownloadDlg::OnBnClickedDownloadTranslateCheck1()
{
	// TODO: 在此添加控件通知处理程序代码
	m_download_translate = (m_download_translate_chk.GetCheck() != 0);
}


void CLyricDownloadDlg::OnDestroy()
{
	CBaseDialog::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	SaveConfig();
}

UINT CLyricDownloadDlg::LyricSearchThreadFunc(LPVOID lpParam)
{
    CCommon::SetThreadLanguageList(theApp.m_str_table.GetLanguageTag());
	SearchThreadInfo* pInfo = (SearchThreadInfo*)lpParam;
    wstring url = pInfo->url;
    wstring result;
    bool rtn = CInternetCommon::HttpPost(url, result);        //向网易云音乐的歌曲搜索API发送http的POST请求
    if (theApp.m_lyric_download_dialog_exit) return 0;
    // 此处（以及大部分网络相关）有线程安全问题，HttpPost可能卡30s网络超时，要解决此问题CInternetSession的封装应当提供退出flag参数
    // 此时如果歌词下载窗口关闭则pInfo会是野指针（比如关闭再打开此对话框会使得上面的检查无效）
    pInfo->rtn = rtn;
	pInfo->result = result;
	::PostMessage(pInfo->hwnd, WM_SEARCH_COMPLATE, 0, 0);		//搜索完成后发送一个搜索完成的消息

	return 0;
}

UINT CLyricDownloadDlg::LyricDownloadThreadFunc(LPVOID lpParam)
{
    CCommon::SetThreadLanguageList(theApp.m_str_table.GetLanguageTag());
	DownloadThreadInfo* pInfo = (DownloadThreadInfo*)lpParam;
    wstring song_id = pInfo->song_id;
    bool download_translate = pInfo->download_translate;
    wstring result;
    bool rtn = CLyricDownloadCommon::DownloadLyric(song_id, result, download_translate);  //下载歌词
    if (theApp.m_lyric_download_dialog_exit) return 0;
    pInfo->success = rtn;
	pInfo->result = result;
	::PostMessage(pInfo->hwnd, WM_DOWNLOAD_COMPLATE, (WPARAM)pInfo->save_as, 0);		//下载完成后发送一个下载完成消息（wParam用于传递是否弹出“另存为”对话框）
	return 0;
}


afx_msg LRESULT CLyricDownloadDlg::OnSearchComplate(WPARAM wParam, LPARAM lParam)
{
	//响应WM_SEARCH_CONPLATE消息
	GetDlgItem(IDC_SEARCH_BUTTON2)->EnableWindow(TRUE);	//搜索完成之后启用该按钮
	m_search_result = m_search_thread_info.result;
    if(m_search_thread_info.rtn != CInternetCommon::SUCCESS)
        SetDlgItemText(IDC_STATIC_INFO, theApp.m_str_table.LoadText(L"TXT_LYRIC_DL_INFO").c_str());

    switch (m_search_thread_info.rtn)
    {
    case CInternetCommon::FAILURE:
    {
        const wstring& info = theApp.m_str_table.LoadText(L"MSG_NETWORK_SEARCH_FAILED");
        MessageBox(info.c_str(), NULL, MB_ICONWARNING);
        return 0;
    }
    case CInternetCommon::OUTTIME:
    {
        const wstring& info = theApp.m_str_table.LoadText(L"MSG_NETWORK_SEARCH_TIME_OUT");
        MessageBox(info.c_str(), NULL, MB_ICONWARNING);
        return 0;
    }
    default: break;
    }
	//DEBUG模式下，将查找返回的结果保存到文件
#ifdef DEBUG
	ofstream out_put{ L".\\down.log", std::ios::binary };
	out_put << CCommon::UnicodeToStr(m_search_result, CodeType::UTF8);
#endif // DEBUG

	CInternetCommon::DisposeSearchResult(m_down_list, m_search_result);		//处理返回的结果
	ShowDownloadList();			//将搜索的结果显示在列表控件中

	//计算搜索结果中最佳匹配项目
	int best_matched;
	bool id_releated{ false };
    CSongDataManager::GetInstance().GetSongID(m_song, m_song.song_id);  // 从媒体库读取id
    if (m_song.song_id != 0)   //如果当前歌曲已经有关联的ID，则根据该ID在搜索结果列表中查找对应的项目
	{
		for (size_t i{}; i<m_down_list.size(); i++)
		{
            if (m_song.GetSongId() == m_down_list[i].id)
			{
				id_releated = true;
				best_matched = i;
				break;
			}
		}
	}
	if(!id_releated)
		best_matched = CInternetCommon::SelectMatchedItem(m_down_list, m_song.title, m_song.artist, m_song.album, m_lyric_name, true);
    wstring info;
	m_unassciate_lnk.ShowWindow(SW_HIDE);
    SongInfo song_info_ori{ CSongDataManager::GetInstance().GetSongInfo3(m_song) };
	if (m_down_list.empty())
    {
        song_info_ori.SetNoOnlineLyric(true);
        CSongDataManager::GetInstance().AddItem(song_info_ori);
        info = theApp.m_str_table.LoadText(L"TXT_LYRIC_DL_INFO_SEARCH_NO_SONG");
    }
	else if (best_matched == -1)
    {
        song_info_ori.SetNoOnlineLyric(true);
        CSongDataManager::GetInstance().AddItem(song_info_ori);
        info = theApp.m_str_table.LoadText(L"TXT_LYRIC_DL_INFO_SEARCH_NO_MATCHED");
    }
	else if(id_releated)
	{
        info = theApp.m_str_table.LoadTextFormat(L"TXT_LYRIC_DL_INFO_SEARCH_RELATED", { best_matched + 1 });
		m_unassciate_lnk.ShowWindow(SW_SHOW);
	}
	else
        info = theApp.m_str_table.LoadTextFormat(L"TXT_LYRIC_DL_INFO_SEARCH_BEST_MATCHED", { best_matched + 1 });

    SetDlgItemText(IDC_STATIC_INFO, info.c_str());
	//自动选中列表中最佳匹配的项目
	m_down_list_ctrl.SetFocus();
	m_down_list_ctrl.SetItemState(best_matched, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);	//选中行
	m_down_list_ctrl.EnsureVisible(best_matched, FALSE);		//使选中行保持可见
	m_item_selected = best_matched;
	return 0;
}


void CLyricDownloadDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类
	theApp.m_lyric_download_dialog_exit = true;
	if (m_pSearchThread != nullptr)
		WaitForSingleObject(m_pSearchThread->m_hThread, 1000);	//等待线程退出
	if (m_pDownThread != nullptr)
		WaitForSingleObject(m_pDownThread->m_hThread, 1000);	//等待线程退出
	CBaseDialog::OnCancel();
}


void CLyricDownloadDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
	theApp.m_lyric_download_dialog_exit = true;
	if (m_pSearchThread != nullptr)
		WaitForSingleObject(m_pSearchThread->m_hThread, 1000);	//等待线程退出
	if (m_pDownThread != nullptr)
		WaitForSingleObject(m_pDownThread->m_hThread, 1000);	//等待线程退出
	CBaseDialog::OnOK();
}


afx_msg LRESULT CLyricDownloadDlg::OnDownloadComplate(WPARAM wParam, LPARAM lParam)
{
	m_lyric_str = m_download_thread_info.result;
	GetDlgItem(IDC_DOWNLOAD_SELECTED)->EnableWindow(TRUE);		//下载完成后启用该按钮
	GetDlgItem(IDC_SELECTED_SAVE_AS)->EnableWindow(TRUE);		//下载完成后启用该按钮
	if (!m_download_thread_info.success || m_lyric_str.empty())
	{
        const wstring& info = theApp.m_str_table.LoadText(L"MSG_NETWORK_LYRIC_DOWNLOAD_FAILED");
        MessageBox(info.c_str(), NULL, MB_ICONWARNING);
		return 0;
	}
	if (!CLyricDownloadCommon::DisposeLryic(m_lyric_str))
	{
        const wstring& info = theApp.m_str_table.LoadText(L"MSG_NETWORK_SONG_NO_LYRIC");
        MessageBox(info.c_str(), NULL, MB_ICONWARNING);
		return 0;
	}

	CLyricDownloadCommon::AddLyricTag(m_lyric_str, m_down_list[m_item_selected].id, m_down_list[m_item_selected].title, m_down_list[m_item_selected].artist, m_down_list[m_item_selected].album);

	//保存歌词
	if (wParam == 0)		//wParam为0时不弹出“另存为对话框”
	{
        if (!m_save_to_song_folder && !CCommon::FolderExist(theApp.m_lyric_setting_data.lyric_path))
        {
            const wstring& info = theApp.m_str_table.LoadText(L"MSG_LYRIC_FOLDER_NOT_EXIST");
            MessageBox(info.c_str(), NULL, MB_ICONWARNING | MB_OK);
            return 0;
        }
        wstring saved_path = GetSavedPath();
		if (CCommon::FileExist(saved_path))
		{
            const wstring& info = theApp.m_str_table.LoadText(L"MSG_LYRIC_OVERWRITE_INQUIRY");
            if (MessageBox(info.c_str(), NULL, MB_ICONWARNING | MB_OKCANCEL) == IDCANCEL)
				return 0;
		}
		if (!SaveLyric(saved_path.c_str(), m_save_code))	//保存歌词
			return 0;
		if (m_download_translate)
		{
			CLyrics lyrics{ saved_path, CLyrics::LyricType::LY_LRC_NETEASE };		//打开保存过的歌词
			lyrics.SaveLyric2(theApp.m_general_setting_data.download_lyric_text_and_translation_in_same_line);
		}

		if (m_song.IsSameSong(CPlayer::GetInstance().GetCurrentSongInfo()))		//如果正在播放的歌曲还是当前下载歌词的歌曲，才更新歌词显示
			CPlayer::GetInstance().IniLyrics(saved_path);
        wstring info = theApp.m_str_table.LoadTextFormat(L"MSG_LYRIC_DOWNLOAD_COMPLETE_SAVED", { saved_path });
        MessageBox(info.c_str(), NULL, MB_ICONINFORMATION);
	}
	else
	{
		//设置过滤器
        wstring filter = FilterHelper::GetLyricFileFilter();
		//构造保存文件对话框
        CFileDialog fileDlg(FALSE, _T("lrc"), GetSavedPath().c_str(), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, filter.c_str(), this);
		//为“另存为”对话框添加一个组合选择框
		fileDlg.AddComboBox(IDC_SAVE_COMBO_BOX);
		//为组合选择框添加项目
        fileDlg.AddControlItem(IDC_SAVE_COMBO_BOX, 0, theApp.m_str_table.LoadText(L"TXT_SAVE_AS_ANSI").c_str());
        fileDlg.AddControlItem(IDC_SAVE_COMBO_BOX, 1, theApp.m_str_table.LoadText(L"TXT_SAVE_AS_UTF8").c_str());
		//为组合选择框设置默认选中的项目
		fileDlg.SetSelectedControlItem(IDC_SAVE_COMBO_BOX, static_cast<int>(m_save_code));

		//显示保存文件对话框
		if (IDOK == fileDlg.DoModal())
		{
			DWORD selected_item;
			fileDlg.GetSelectedControlItem(IDC_SAVE_COMBO_BOX, selected_item);	//获取“编码格式”中选中的项目
			CodeType save_code{};
			switch (selected_item)
			{
			case 0: save_code = CodeType::ANSI; break;
			case 1: save_code = CodeType::UTF8; break;
			default: break;
			}
			wstring saved_path{ fileDlg.GetPathName().GetString() };
			if (!SaveLyric(saved_path.c_str(), save_code))	//保存歌词
				return 0;

			if (m_download_translate)
			{
				CLyrics lyrics{ saved_path, CLyrics::LyricType::LY_LRC_NETEASE };		//打开保存过的歌词
				lyrics.SaveLyric2(theApp.m_general_setting_data.download_lyric_text_and_translation_in_same_line);
			}

            if (m_song.IsSameSong(CPlayer::GetInstance().GetCurrentSongInfo()))		//如果正在播放的歌曲还是当前下载歌词的歌曲，才更新歌词显示
                CPlayer::GetInstance().IniLyrics(saved_path);
		}
	}
	return 0;
}


void CLyricDownloadDlg::OnBnClickedSaveToSongFolder1()
{
	// TODO: 在此添加控件通知处理程序代码
	m_save_to_song_folder = true;
}


void CLyricDownloadDlg::OnBnClickedSaveToLyricFolder1()
{
	// TODO: 在此添加控件通知处理程序代码
	m_save_to_song_folder = false;
}


void CLyricDownloadDlg::OnCbnSelchangeCombo2()
{
	// TODO: 在此添加控件通知处理程序代码
	//获取组合框中选中的编码格式
	switch (m_save_code_combo.GetCurSel())
	{
	case 1: m_save_code = CodeType::UTF8; break;
	default: m_save_code = CodeType::ANSI; break;
	}
}


void CLyricDownloadDlg::OnLdLyricDownload()
{
	// TODO: 在此添加命令处理程序代码
	OnBnClickedDownloadSelected();
}


void CLyricDownloadDlg::OnLdLyricSaveas()
{
	// TODO: 在此添加命令处理程序代码
	OnBnClickedSelectedSaveAs();
}


void CLyricDownloadDlg::OnLdCopyTitle()
{
	// TODO: 在此添加命令处理程序代码
	if (IsItemSelectedValid())
	{
		if(!CCommon::CopyStringToClipboard(m_down_list[m_item_selected].title))
            MessageBox(theApp.m_str_table.LoadText(L"MSG_COPY_CLIPBOARD_FAILED").c_str(), NULL, MB_ICONWARNING);
	}
}


void CLyricDownloadDlg::OnLdCopyArtist()
{
	// TODO: 在此添加命令处理程序代码
	if (IsItemSelectedValid())
	{
		if (!CCommon::CopyStringToClipboard(m_down_list[m_item_selected].artist))
            MessageBox(theApp.m_str_table.LoadText(L"MSG_COPY_CLIPBOARD_FAILED").c_str(), NULL, MB_ICONWARNING);
	}
}


void CLyricDownloadDlg::OnLdCopyAlbum()
{
	// TODO: 在此添加命令处理程序代码
	if (IsItemSelectedValid())
	{
		if (!CCommon::CopyStringToClipboard(m_down_list[m_item_selected].album))
            MessageBox(theApp.m_str_table.LoadText(L"MSG_COPY_CLIPBOARD_FAILED").c_str(), NULL, MB_ICONWARNING);
	}
}


void CLyricDownloadDlg::OnLdCopyId()
{
	// TODO: 在此添加命令处理程序代码
	if (IsItemSelectedValid())
	{
		if (!CCommon::CopyStringToClipboard(m_down_list[m_item_selected].id))
            MessageBox(theApp.m_str_table.LoadText(L"MSG_COPY_CLIPBOARD_FAILED").c_str(), NULL, MB_ICONWARNING);
	}
}


void CLyricDownloadDlg::OnLdViewOnline()
{
	// TODO: 在此添加命令处理程序代码
	if (IsItemSelectedValid())
	{
		//获取网易云音乐中该歌曲的在线接听网址
		wstring song_url{ L"http://music.163.com/#/song?id=" + m_down_list[m_item_selected].id };
		//打开超链接
		ShellExecute(NULL, _T("open"), song_url.c_str(), NULL, NULL, SW_SHOW);
	}
}

//双击列表项目后下载选中项目
void CLyricDownloadDlg::OnNMDblclkLyricDownList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	m_item_selected = pNMItemActivate->iItem;
	if (IsItemSelectedValid())
	{
		OnBnClickedDownloadSelected();
	}
	*pResult = 0;
}


BOOL CLyricDownloadDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	//if (pMsg->message == WM_MOUSEMOVE)
	//	m_tool_tip.RelayEvent(pMsg);

	return CBaseDialog::PreTranslateMessage(pMsg);
}


void CLyricDownloadDlg::OnNMClickUnassociateLink(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
    SetID(wstring());
	m_unassciate_lnk.ShowWindow(SW_HIDE);

	*pResult = 0;
}


void CLyricDownloadDlg::OnLdPreview()
{
	// TODO: 在此添加命令处理程序代码

	//下载歌词
	const CInternetCommon::ItemInfo& item{ m_down_list[m_item_selected] };
	bool success{ false };
	wstring result;
	{
		CWaitCursor wait_cursor;
		success = CLyricDownloadCommon::DownloadLyric(item.id, result, m_download_translate);		//下载歌词
	}

	//如果不成功弹出消息对话框
	if (!success || result.empty())
	{
        const wstring& info = theApp.m_str_table.LoadText(L"MSG_NETWORK_LYRIC_DOWNLOAD_FAILED");
        MessageBox(info.c_str(), NULL, MB_ICONWARNING);
		return;
	}
	if (!CLyricDownloadCommon::DisposeLryic(result))
	{
        const wstring& info = theApp.m_str_table.LoadText(L"MSG_NETWORK_SONG_NO_LYRIC");
        MessageBox(info.c_str(), NULL, MB_ICONWARNING);
		return;
	}

	//添加歌词标签
	CLyricDownloadCommon::AddLyricTag(result, item.id, item.title, item.artist, item.album);

	CLyrics lyrics;
	lyrics.LyricsFromRowString(result, CLyrics::LyricType::LY_LRC_NETEASE);
	result = lyrics.GetLyricsString2(theApp.m_general_setting_data.download_lyric_text_and_translation_in_same_line);

    // 显示预览窗口
    CMessageDlg dlg(L"LrcPreviewDlg");
    dlg.SetWindowTitle(theApp.m_str_table.LoadText(L"TITLE_LYRIC_PREVIEW"));
    wstring info = item.artist + L" - " + item.title;
    dlg.SetInfoText(info);
    dlg.SetMessageText(result);
    dlg.DoModal();
}


void CLyricDownloadDlg::OnLdRelate()
{
    // TODO: 在此添加命令处理程序代码
    if (m_item_selected >= 0 && m_item_selected < static_cast<int>(m_down_list.size()))
    {
        SetID(m_down_list[m_item_selected].id);     // 将选中项目的歌曲ID关联到歌曲
    }
}
