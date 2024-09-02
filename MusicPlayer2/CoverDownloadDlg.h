﻿#pragma once
#include "CoverDownloadCommon.h"
#include "ListCtrlEx.h"
#include "SongInfo.h"
#include "BaseDialog.h"

// CCoverDownloadDlg 对话框

class CCoverDownloadDlg : public CBaseDialog
{
    DECLARE_DYNAMIC(CCoverDownloadDlg)

public:
    CCoverDownloadDlg(CWnd* pParent = nullptr);   // 标准构造函数
    virtual ~CCoverDownloadDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_COVER_DOWNLOAD_DIALOG };
#endif

#define WM_SEARCH_COMPLATE (WM_USER+101)        //歌曲搜索完成消息
#define WM_DOWNLOAD_COMPLATE (WM_USER+102)      //专辑封面下载完成消息

    //歌曲搜索线程函数
    static UINT SongSearchThreadFunc(LPVOID lpParam);

    //专辑封面下载线程函数
    static UINT CoverDownloadThreadFunc(LPVOID lpParam);

protected:
    CListCtrlEx m_down_list_ctrl;
    CLinkCtrl m_unassciate_lnk;

    SongInfo m_song;        // 此次下载封面的歌曲，窗口初始化时写入，之后不应再从播放实例获取
    wstring m_org_album_cover_path; // 歌曲原封面，初始化时写入，在CoverDownloadThreadFunc中应读取这里，不从播放实例获取
    wstring m_title;        //要查找歌词的歌曲的标题
    wstring m_artist;       //要查找歌词的歌曲的艺术家
    wstring m_album;        //要查找歌词的歌曲的唱片集
    wstring m_file_name;    //要查找歌词的歌曲的文件名

    bool m_save_to_song_folder;     // 是否将下载到的封面存储到歌曲目录

    vector<CInternetCommon::ItemInfo> m_down_list;  //搜索结果的列表
    int m_item_selected{ -1 };      //搜索结果列表中选中的项目

    wstring m_search_url;
    int m_search_rtn;
    wstring m_search_result;

    CWinThread* m_pSearchThread;        //搜索歌词的线程
    CWinThread* m_pDownThread;          //下载歌词的线程

    void SaveConfig() const;
    void LoadConfig();

    void SetID(wstring id);     // 保存指定id到媒体库内m_song.file_path对应条目

    wstring GetSavedDir();

    virtual SongInfo GetSongInfo() const;
    virtual CString GetDialogName() const override;
    virtual bool InitializeControls() override;

    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
    void ShowDownloadList();        //将搜索结果显示出来

    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedSearchButton();
protected:
    afx_msg LRESULT OnSearchComplate(WPARAM wParam, LPARAM lParam);
public:
    afx_msg virtual void OnBnClickedDownloadSelected();
    afx_msg void OnNMClickCoverDownList(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMDblclkCoverDownList(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMRClickCoverDownList(NMHDR *pNMHDR, LRESULT *pResult);
    virtual void OnOK();
    virtual void OnCancel();
    afx_msg void OnDestroy();
protected:
    afx_msg LRESULT OnDownloadComplate(WPARAM wParam, LPARAM lParam);
public:
    afx_msg void OnEnChangeTitleEdit();
    afx_msg void OnEnChangeArtistEdit();
    afx_msg void OnNMClickUnassociateLink(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnBnClickedSaveToSongFolder2();
    afx_msg void OnBnClickedSaveToAlbumFolder2();
};
