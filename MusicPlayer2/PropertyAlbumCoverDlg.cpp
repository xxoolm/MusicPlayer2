﻿// PropertyAlbumCover.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "Player.h"
#include "PropertyAlbumCoverDlg.h"
#include "COSUPlayerHelper.h"
#include "DrawCommon.h"
#include "MusicPlayerCmdHelper.h"
#include "PropertyDlgHelper.h"
#include "CommonDialogMgr.h"
#include "FilterHelper.h"

#define PROPERTY_COVER_IMG_FILE_NAME L"PropertyCoverImg_U6V19HODcJ2p11FM"

// CPropertyAlbumCoverDlg 对话框

IMPLEMENT_DYNAMIC(CPropertyAlbumCoverDlg, CTabDlg)

CPropertyAlbumCoverDlg::CPropertyAlbumCoverDlg(vector<SongInfo>& all_song_info, int& index, bool show_out_album_cover, bool read_only /*= false*/, CWnd* pParent /*=nullptr*/)
    : CTabDlg(IDD_PROPERTY_ALBUM_COVER_DIALOG, pParent), m_all_song_info{ all_song_info }, m_index{ index },
    m_show_out_album_cover{ show_out_album_cover }, m_read_only{ read_only }, m_batch_edit{ false }
{

}

CPropertyAlbumCoverDlg::CPropertyAlbumCoverDlg(vector<SongInfo>& all_song_info, CWnd* pParent /*= nullptr*/)
    : CTabDlg(IDD_PROPERTY_ALBUM_COVER_DIALOG, pParent), m_all_song_info{ all_song_info }, m_index{ m_no_use }, m_batch_edit{ true }
{

}

CPropertyAlbumCoverDlg::~CPropertyAlbumCoverDlg()
{
}

void CPropertyAlbumCoverDlg::PagePrevious()
{
    m_cover_changed = false;
    m_cover_deleted = false;
    m_index--;
    if (m_index < 0) m_index = static_cast<int>(m_all_song_info.size()) - 1;
    if (m_index < 0) m_index = 0;
    SetWreteEnable();
    ShowInfo();
}

void CPropertyAlbumCoverDlg::PageNext()
{
    m_cover_changed = false;
    m_cover_deleted = false;
    m_index++;
    if (m_index >= static_cast<int>(m_all_song_info.size())) m_index = 0;
    SetWreteEnable();
    ShowInfo();
}

int CPropertyAlbumCoverDlg::SaveModified()
{
    if (m_cover_deleted || m_cover_changed || IsDlgButtonChecked(IDC_SAVE_ALBUM_COVER_BUTTON))
    {
        CWaitCursor wait_cursor;
        int current_position{};
        bool is_playing{};
        // 调用SaveModified的属性主窗口已进行ReOpen操作，此时程序没有打开任何歌曲

        int saved_count{};
        if (m_cover_changed)
        {
            //保存手动浏览的专辑封面
            saved_count += SaveAlbumCover(m_out_img_path);
        }
        else if (m_cover_deleted)
        {
            //删除内嵌专辑封面
            saved_count += SaveAlbumCover(wstring());
        }
        else if (IsDlgButtonChecked(IDC_SAVE_ALBUM_COVER_BUTTON))
        {
            //将关联的外部专辑封面嵌入到音频文件
            if (m_batch_edit)
                saved_count += SaveEnbedLinkedCoverForBatchEdit();
            else
                saved_count += SaveAlbumCover(m_out_img_path, true);
        }

        return saved_count;
    }
    return 1;
}

void CPropertyAlbumCoverDlg::AdjustColumnWidth()
{
    CRect rect;
    m_list_ctrl.GetWindowRect(rect);
    int width0 = theApp.DPI(85);
    int width1 = rect.Width() - width0 - theApp.DPI(20) - 1;
    m_list_ctrl.SetColumnWidth(0, width0);
    m_list_ctrl.SetColumnWidth(1, width1);
}

void CPropertyAlbumCoverDlg::ShowInfo()
{
    m_cover_img.Destroy();
    m_cover_out_img.Destroy();
    int cover_type{};
    size_t cover_size{};

    //载入内嵌专辑封面
    if (!IsShowOutAlbumCover() && !m_batch_edit)
    {
        CAudioTag audio_tag(CurrentSong().file_path);
        wstring cover_img_path = audio_tag.GetAlbumCover(cover_type, PROPERTY_COVER_IMG_FILE_NAME, &cover_size);

        //载入图片
        if (cover_size > 0)
            m_cover_img.Load(cover_img_path.c_str());
    }

    //载入外部专辑封面
    if (IsShowOutAlbumCover())
    {
        if (m_cover_changed)
        {
            m_cover_out_img.Load(m_out_img_path.c_str());
            cover_size = CCommon::GetFileSize(m_out_img_path);
        }
        else if (!m_batch_edit)
        {
            CMusicPlayerCmdHelper helper;
            m_out_img_path = helper.SearchAlbumCover(CurrentSong());
            m_cover_out_img.Load(m_out_img_path.c_str());
        }
    }

    //显示列表信息
    //文件路径
    m_list_ctrl.SetItemText(RI_FILE_PATH, 1, CurrentSong().file_path.c_str());
    if (HasAlbumCover())
    {
        //路径
        wstring str_path;
        if (IsShowOutAlbumCover())
            str_path = m_out_img_path;
        else
            str_path = theApp.m_str_table.LoadText(L"TXT_INNER_COVER");
        m_list_ctrl.SetItemText(RI_COVER_PATH, 1, str_path.c_str());

        //文件类型
        CString str_type;
        if (IsShowOutAlbumCover())
            str_type = CFilePathHelper(m_out_img_path).GetFileExtension().c_str();
        else
        {
            switch (cover_type)
            {
            case 0:
                str_type = L"jpg";
                break;
            case 1:
                str_type = L"png";
                break;
            case 2:
                str_type = L"gif";
                break;
            case 3:
                str_type = L"bmp";
                break;
            default:
                break;
            }
        }
        m_list_ctrl.SetItemText(RI_FORMAT, 1, str_type);

        //宽度、高度、BPP
        int cover_width{};
        int cover_height{};
        int cover_bpp;
        cover_width = GetCoverImage().GetWidth();
        cover_height = GetCoverImage().GetHeight();
        cover_bpp = GetCoverImage().GetBPP();
        m_list_ctrl.SetItemText(RI_WIDTH, 1, std::to_wstring(cover_width).c_str());
        m_list_ctrl.SetItemText(RI_HEIGHT, 1, std::to_wstring(cover_height).c_str());
        m_list_ctrl.SetItemText(RI_BPP, 1, std::to_wstring(cover_bpp).c_str());

        //文件大小
        if (IsShowOutAlbumCover())
        {
            size_t file_size = CCommon::GetFileSize(m_out_img_path);
            m_list_ctrl.SetItemText(RI_SIZE, 1, CCommon::DataSizeToString(file_size));
        }
        else
        {
            m_list_ctrl.SetItemText(RI_SIZE, 1, CCommon::DataSizeToString(cover_size));
        }
    }
    else
    {
        //m_list_ctrl.SetItemText(RI_COMPRESSED, 0, _T(""));
        for (int i = 0; i < RI_MAX; i++)
        {
            if (i != RI_FILE_PATH)
                m_list_ctrl.SetItemText(i, 1, _T(""));
        }
    }


    SetWreteEnable();
    CheckDlgButton(IDC_SAVE_ALBUM_COVER_BUTTON, FALSE);
    Invalidate();
}

const SongInfo& CPropertyAlbumCoverDlg::CurrentSong()
{
    if (m_index >= 0 && m_index < static_cast<int>(m_all_song_info.size()) && !m_batch_edit)
    {
        return m_all_song_info[m_index];
    }
    else
    {
        static SongInfo song;
        return song;
    }
}

CImage& CPropertyAlbumCoverDlg::GetCoverImage()
{
    if (IsShowOutAlbumCover())
        return m_cover_out_img;
    else
        return m_cover_img;
}

bool CPropertyAlbumCoverDlg::IsCurrentSong()
{
    if (m_batch_edit)
    {
        for (const auto& song : m_all_song_info)
        {
            if (song.file_path == CPlayer::GetInstance().GetCurrentFilePath())
                return true;
        }
        return false;
    }
    else
    {
        const SongInfo song{ CurrentSong() };
        bool is_current_song{ song.file_path == CPlayer::GetInstance().GetCurrentFilePath() };
        return is_current_song;
    }
}

bool CPropertyAlbumCoverDlg::IsShowOutAlbumCover()
{
    return m_show_out_album_cover || m_cover_changed || COSUPlayerHelper::IsOsuFile(CurrentSong().file_path) || m_batch_edit;
}

bool CPropertyAlbumCoverDlg::IsDeleteEnable()
{
    if (m_batch_edit)
    {
        return m_write_enable;
    }
    else
    {
        if (IsShowOutAlbumCover())
            return (CCommon::FileExist(m_out_img_path) && !COSUPlayerHelper::IsOsuFile(CurrentSong().file_path));
        else
            return m_write_enable && HasAlbumCover();
    }
}

bool CPropertyAlbumCoverDlg::HasAlbumCover()
{
    return !GetCoverImage().IsNull();
}

void CPropertyAlbumCoverDlg::SetWreteEnable()
{
    if (m_batch_edit)
    {
        CPropertyDlgHelper helper(m_all_song_info);
        m_write_enable = helper.IsMultiCoverWritable();
    }
    else
    {
        CFilePathHelper file_path{ m_all_song_info[m_index].file_path };
        m_write_enable = (!m_all_song_info[m_index].is_cue && !COSUPlayerHelper::IsOsuFile(file_path.GetFilePath()) && CAudioTag::IsFileTypeCoverWriteSupport(file_path.GetFileExtension()));
    }
    m_write_enable &= !m_read_only;
    EnableControls();
    SetSaveBtnEnable();
}

void CPropertyAlbumCoverDlg::EnableControls()
{

    EnableDlgCtrl(IDC_BROWSE_BUTTON, m_write_enable);
    EnableDlgCtrl(IDC_DELETE_BUTTON, IsDeleteEnable());
    if (m_batch_edit)
    {
        EnableDlgCtrl(IDC_SAVE_ALBUM_COVER_BUTTON, m_write_enable);
    }
    else
    {
        EnableDlgCtrl(IDC_SAVE_ALBUM_COVER_BUTTON, m_write_enable && IsShowOutAlbumCover() && !m_cover_out_img.IsNull());
    }
    EnableDlgCtrl(IDC_SHOW_OUT_ALBUM_COVER_CHK, (!m_batch_edit && !COSUPlayerHelper::IsOsuFile(CurrentSong().file_path)));
}

void CPropertyAlbumCoverDlg::SetSaveBtnEnable()
{
    bool enable = m_write_enable && (IsDlgButtonChecked(IDC_SAVE_ALBUM_COVER_BUTTON) || m_cover_changed || m_cover_deleted);
    CWnd* pParent = GetParentWindow();
    if (pParent != nullptr)
        pParent->SendMessage(WM_PROPERTY_DIALOG_MODIFIED, enable);
}

int CPropertyAlbumCoverDlg::SaveAlbumCover(const wstring& album_cover_path, bool delete_file)
{
    if (m_batch_edit)
    {
        int saved_count{};
        for (const auto& song : m_all_song_info)
        {
            CAudioTag audio_tag(song.file_path);
            if (audio_tag.WriteAlbumCover(album_cover_path))
            {
                saved_count++;
                if (delete_file)
                {
                    DeleteLinkedPic(song.file_path, album_cover_path);
                }
            }
        }
        return saved_count;
    }
    else
    {
        CAudioTag audio_tag(CurrentSong().file_path);
        bool rtn = audio_tag.WriteAlbumCover(album_cover_path);
        if (rtn && delete_file)
        {
            DeleteLinkedPic(CurrentSong().file_path, album_cover_path);
        }
        return rtn;
    }
}

int CPropertyAlbumCoverDlg::SaveEnbedLinkedCoverForBatchEdit()
{
    int saved_count{};
    for (const auto& song : m_all_song_info)
    {
        CMusicPlayerCmdHelper helper;
        wstring album_cover_path = helper.SearchAlbumCover(song);
        CAudioTag audio_tag(song.file_path);
        if (!album_cover_path.empty() && audio_tag.WriteAlbumCover(album_cover_path))
        {
            saved_count++;
            DeleteLinkedPic(song.file_path, album_cover_path);
        }
    }
    return saved_count;

}

void CPropertyAlbumCoverDlg::DeleteLinkedPic(const wstring& file_path, const wstring& album_cover_path)
{
    //将外部专辑封面嵌入到音频文件后，如果图片的文件名和音频文件的文件名相同，则删除此外部专辑封面图片，因此这个图片已经没有作用了
    wstring album_cover_file_name = CFilePathHelper(album_cover_path).GetFilePathWithoutExtension();
    wstring file_name = CFilePathHelper(file_path).GetFilePathWithoutExtension();
    if (file_name == album_cover_file_name)
    {
        CommonDialogMgr::DeleteAFile(theApp.m_pMainWnd->GetSafeHwnd(), album_cover_path);
    }
}

void CPropertyAlbumCoverDlg::OnTabEntered()
{
    ShowInfo();
}

bool CPropertyAlbumCoverDlg::InitializeControls()
{
    wstring temp;
    // IDC_LIST1
    temp = theApp.m_str_table.LoadText(L"TXT_COVER_PROPERTY_BROWSE");
    SetDlgItemTextW(IDC_BROWSE_BUTTON, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_COVER_PROPERTY_DELETE");
    SetDlgItemTextW(IDC_DELETE_BUTTON, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_COVER_PROPERTY_SHOW_OUT_COVER");
    SetDlgItemTextW(IDC_SHOW_OUT_ALBUM_COVER_CHK, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_COVER_PROPERTY_DO_INNER_COVER");
    SetDlgItemTextW(IDC_SAVE_ALBUM_COVER_BUTTON, temp.c_str());

    RepositionTextBasedControls({
        { CtrlTextInfo::L1, IDC_BROWSE_BUTTON, CtrlTextInfo::W32 },
        { CtrlTextInfo::L2, IDC_DELETE_BUTTON, CtrlTextInfo::W32 }
        });
    return true;
}

void CPropertyAlbumCoverDlg::DoDataExchange(CDataExchange* pDX)
{
    CTabDlg::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST1, m_list_ctrl);
}


BEGIN_MESSAGE_MAP(CPropertyAlbumCoverDlg, CTabDlg)
    ON_WM_MOUSEWHEEL()
    ON_WM_PAINT()
    ON_BN_CLICKED(IDC_SAVE_ALBUM_COVER_BUTTON, &CPropertyAlbumCoverDlg::OnBnClickedSaveAlbumCoverButton)
    ON_BN_CLICKED(IDC_DELETE_BUTTON, &CPropertyAlbumCoverDlg::OnBnClickedDeleteButton)
    ON_BN_CLICKED(IDC_BROWSE_BUTTON, &CPropertyAlbumCoverDlg::OnBnClickedBrowseButton)
    ON_WM_RBUTTONUP()
    ON_COMMAND(ID_COVER_BROWSE, &CPropertyAlbumCoverDlg::OnCoverBrowse)
    ON_COMMAND(ID_COVER_DELETE, &CPropertyAlbumCoverDlg::OnCoverDelete)
    ON_COMMAND(ID_COVER_SAVE_AS, &CPropertyAlbumCoverDlg::OnCoverSaveAs)
    ON_WM_INITMENU()
    ON_BN_CLICKED(IDC_SHOW_OUT_ALBUM_COVER_CHK, &CPropertyAlbumCoverDlg::OnBnClickedShowOutAlbumCoverChk)
    ON_COMMAND(ID_COMPRESS_SIZE, &CPropertyAlbumCoverDlg::OnCompressSize)
    ON_MESSAGE(WM_TABLET_QUERYSYSTEMGESTURESTATUS, &CPropertyAlbumCoverDlg::OnTabletQuerysystemgesturestatus)
END_MESSAGE_MAP()


// CPropertyAlbumCoverDlg 消息处理程序


BOOL CPropertyAlbumCoverDlg::OnInitDialog()
{
    CTabDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化
    SetButtonIcon(IDC_BROWSE_BUTTON, IconMgr::IconType::IT_Folder_Explore);
    SetButtonIcon(IDC_DELETE_BUTTON, IconMgr::IconType::IT_Cancel);

    //初始化列表
    //初始化列表
    CRect rect;
    m_list_ctrl.GetWindowRect(rect);
    m_list_ctrl.SetExtendedStyle(m_list_ctrl.GetExtendedStyle() | LVS_EX_GRIDLINES);
    int width0 = theApp.DPI(85);
    int width1 = rect.Width() - width0 - theApp.DPI(20) - 1;
    m_list_ctrl.InsertColumn(0, theApp.m_str_table.LoadText(L"TXT_ITEM").c_str(), LVCFMT_LEFT, width0);
    m_list_ctrl.InsertColumn(1, theApp.m_str_table.LoadText(L"TXT_VALUE").c_str(), LVCFMT_LEFT, width1);


    m_list_ctrl.InsertItem(RI_FILE_PATH, theApp.m_str_table.LoadText(L"TXT_FILE_PATH").c_str());                   //文件路径
    m_list_ctrl.InsertItem(RI_COVER_PATH, theApp.m_str_table.LoadText(L"TXT_PATH").c_str());                       //封面路径
    m_list_ctrl.InsertItem(RI_FORMAT, theApp.m_str_table.LoadText(L"TXT_COVER_PROPERTY_FORMAT").c_str());          //封面类型
    m_list_ctrl.InsertItem(RI_WIDTH, theApp.m_str_table.LoadText(L"TXT_COVER_PROPERTY_WIDTH").c_str());            //宽度
    m_list_ctrl.InsertItem(RI_HEIGHT, theApp.m_str_table.LoadText(L"TXT_COVER_PROPERTY_HEIGHT").c_str());          //高度
    m_list_ctrl.InsertItem(RI_BPP, theApp.m_str_table.LoadText(L"TXT_COVER_PROPERTY_BPP").c_str());                //每像素位数
    m_list_ctrl.InsertItem(RI_SIZE, theApp.m_str_table.LoadText(L"TXT_COVER_PROPERTY_FILE_SIZE").c_str());         //文件大小

    CheckDlgButton(IDC_SHOW_OUT_ALBUM_COVER_CHK, m_show_out_album_cover);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


BOOL CPropertyAlbumCoverDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值

    //return CTabDlg::OnMouseWheel(nFlags, zDelta, pt);
    return TRUE;
}


void CPropertyAlbumCoverDlg::OnPaint()
{
    CPaintDC dc(this); // device context for painting
                       // TODO: 在此处添加消息处理程序代码
                       // 不为绘图消息调用 CTabDlg::OnPaint()

    //计算专辑封面的绘图区域
    CRect rect;
    GetClientRect(rect);

    CRect rect_list;
    m_list_ctrl.GetWindowRect(rect_list);
    ScreenToClient(rect_list);

    rect.right = rect_list.left;
    rect.DeflateRect(theApp.DPI(16), theApp.DPI(16));

    if (HasAlbumCover())        //有专辑封面时绘制专辑封面
    {
        CImage& img{ GetCoverImage() };
        if (img.GetWidth() < rect.Width() && img.GetHeight() < rect.Height())       //如果专辑封面图片大小小于绘图区域，则将绘图区域改为图片大小
        {
            CRect rect_img;
            rect_img.left = rect.left + (rect.Width() - img.GetWidth()) / 2;
            rect_img.top = rect.top + (rect.Height() - img.GetHeight()) / 2;
            rect_img.right = rect_img.left + img.GetWidth();
            rect_img.bottom = rect_img.top + img.GetHeight();
            rect = rect_img;
        }
        CDrawCommon draw;
        draw.Create(&dc);
        draw.DrawImage(img, rect.TopLeft(), rect.Size(), CDrawCommon::StretchMode::FIT);
    }
    else        //否则以灰色填充
    {
        dc.FillSolidRect(rect, RGB(210, 210, 210));
    }
}


void CPropertyAlbumCoverDlg::OnBnClickedSaveAlbumCoverButton()
{
    // TODO: 在此添加控件通知处理程序代码
    SetSaveBtnEnable();
}


void CPropertyAlbumCoverDlg::OnBnClickedDeleteButton()
{
    // TODO: 在此添加控件通知处理程序代码
    if (IsShowOutAlbumCover() && !m_batch_edit)
    {
        wstring inquiry_info = theApp.m_str_table.LoadTextFormat(L"MSG_DELETE_SINGLE_FILE_INQUIRY", { m_out_img_path });
        if (MessageBox(inquiry_info.c_str(), NULL, MB_ICONQUESTION | MB_OKCANCEL) == IDOK)
        {
            if (CommonDialogMgr::DeleteAFile(theApp.m_pMainWnd->GetSafeHwnd(), m_out_img_path) != 0)
            {
                const wstring& info = theApp.m_str_table.LoadText(L"MSG_DELETE_FILE_FAILED");
                MessageBox(info.c_str(), NULL, MB_ICONWARNING);
            }
        }
    }
    else
    {
        m_cover_deleted = true;
        SetSaveBtnEnable();
    }
}


void CPropertyAlbumCoverDlg::OnBnClickedBrowseButton()
{
    // TODO: 在此添加控件通知处理程序代码

    wstring filter = FilterHelper::GetImageFileFilter();
    CFileDialog fileDlg(TRUE, NULL, NULL, 0, filter.c_str(), this);
    if (IDOK == fileDlg.DoModal())
    {
        m_out_img_path = fileDlg.GetPathName().GetString();
        m_cover_changed = true;
        ShowInfo();
    }
}


void CPropertyAlbumCoverDlg::OnRButtonUp(UINT nFlags, CPoint point)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    CPoint point1;
    GetCursorPos(&point1);
    CMenu* pMenu = theApp.m_menu_mgr.GetMenu(MenuMgr::PropertyCoverMenu);
    if (pMenu != NULL)
        pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point1.x, point1.y, this);

    CTabDlg::OnRButtonUp(nFlags, point);
}


void CPropertyAlbumCoverDlg::OnCoverBrowse()
{
    // TODO: 在此添加命令处理程序代码
    OnBnClickedBrowseButton();
}


void CPropertyAlbumCoverDlg::OnCoverDelete()
{
    // TODO: 在此添加命令处理程序代码
    OnBnClickedDeleteButton();
}


void CPropertyAlbumCoverDlg::OnCoverSaveAs()
{
    // TODO: 在此添加命令处理程序代码
        //设置过滤器
    wstring szFilter = theApp.m_str_table.LoadText(L"TXT_FILTER_ALL_FILES") + L"(*.*)|*.*||";
    //设置另存为时的默认文件名
    CString file_name;
    CString extension = m_list_ctrl.GetItemText(RI_FORMAT, 1);
    file_name.Format(_T("AlbumCover - %s - %s.%s"), CurrentSong().artist.c_str(), CurrentSong().album.c_str(), extension);
    wstring file_name_wcs{ file_name };
    CCommon::FileNameNormalize(file_name_wcs);		//替换掉文件名中的无效字符
    //构造保存文件对话框
    CFileDialog fileDlg(FALSE, NULL, file_name_wcs.c_str(), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter.c_str(), this);
    //显示保存文件对话框
    if (IDOK == fileDlg.DoModal())
    {
        CString dest_file = fileDlg.GetPathName();
        CString src_file;

        if (IsShowOutAlbumCover())
            src_file = m_out_img_path.c_str();
        else
            src_file = (CCommon::GetTemplatePath() + PROPERTY_COVER_IMG_FILE_NAME).c_str();

        ::CopyFile(src_file, dest_file, FALSE);
        SetFileAttributes(dest_file, FILE_ATTRIBUTE_NORMAL);		//取消文件的隐藏属性
    }

}


void CPropertyAlbumCoverDlg::OnInitMenu(CMenu* pMenu)
{
    CTabDlg::OnInitMenu(pMenu);

    // TODO: 在此处添加消息处理程序代码
    pMenu->EnableMenuItem(ID_COVER_BROWSE, MF_BYCOMMAND | (m_write_enable ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_COVER_DELETE, MF_BYCOMMAND | (IsDeleteEnable() ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_COVER_SAVE_AS, MF_BYCOMMAND | (HasAlbumCover() ? MF_ENABLED : MF_GRAYED));
    CImage& cover_image{ m_cover_img };
    int cover_size{};
    if (!cover_image.IsNull())
        cover_size = max(cover_image.GetWidth(), cover_image.GetHeight());
    pMenu->EnableMenuItem(ID_COMPRESS_SIZE, MF_BYCOMMAND | (!IsShowOutAlbumCover() && !m_batch_edit && cover_size > theApp.m_nc_setting_data.max_album_cover_size ? MF_ENABLED : MF_GRAYED));
}


void CPropertyAlbumCoverDlg::OnBnClickedShowOutAlbumCoverChk()
{
    // TODO: 在此添加控件通知处理程序代码
    m_show_out_album_cover = (IsDlgButtonChecked(IDC_SHOW_OUT_ALBUM_COVER_CHK) != 0);
    ShowInfo();
}


void CPropertyAlbumCoverDlg::OnCompressSize()
{
    CImage& album_cover{ GetCoverImage() };
    if (!album_cover.IsNull() && theApp.m_nc_setting_data.max_album_cover_size > 0)
    {
        CSize image_size;
        image_size.cx = album_cover.GetWidth();
        image_size.cy = album_cover.GetHeight();
        if (max(image_size.cx, image_size.cy) > theApp.m_nc_setting_data.max_album_cover_size)      //如果专辑封面的尺寸大于设定的最大值，则将其缩小
        {
            wstring temp_img_path{ CCommon::GetTemplatePath() + ALBUM_COVER_TEMP_NAME_FOR_PROPERTIES };
            temp_img_path += L".jpg";
            //缩小图片大小并保存到临时目录
            CDrawCommon::ImageResize(album_cover, temp_img_path, theApp.m_nc_setting_data.max_album_cover_size, IT_JPG);
            m_out_img_path = temp_img_path;
            m_cover_changed = true;
            SetSaveBtnEnable();
        }
    }

}


afx_msg LRESULT CPropertyAlbumCoverDlg::OnTabletQuerysystemgesturestatus(WPARAM wParam, LPARAM lParam)
{
    return 0;
}
