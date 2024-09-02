﻿#pragma once
#include"Player.h"
#include "PropertyDlg.h"
#include "PlayListCtrl.h"
#include "ColorConvert.h"
#include "DrawCommon.h"
#include "CPlayerUIBase.h"

// CMiniModeDlg 对话框

class CMiniModeDlg : public CDialogEx
{
    DECLARE_DYNAMIC(CMiniModeDlg)

public:
    CMiniModeDlg(int& item_selected, vector<int>& items_selected, CWnd* pParent = NULL);   // 标准构造函数
    virtual ~CMiniModeDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_MINI_DIALOG };
#endif

    void Init();
    void UpdatePlayPauseButton();
    void ShowPlaylist();
    void SetPlayListColor();
    //void SetDefaultBackGround(CImage* pImage);
    //void SetDisplayFormat(DisplayFormat* pDisplayFormat);

    void MoveWindowPos();

    void SetVolume(int step);
    void SetTransparency();

    void SetDragEnable();
    CPlayListCtrl& GetPlaylistCtrl() { return m_playlist_ctrl; }

    void GetPlaylistItemSelected();

    void DrawInfo();

    CPlayerUIBase* GetCurUi();
    bool IsUseUiPlaylist() { return m_use_ui_playlist; }
    bool IsShowPlaylist() { return m_show_playlist; }

protected:

    int m_position_x;
    int m_position_y;

    int m_ui_width{};
    int m_ui_height{};

    bool m_show_playlist{ false };		//是否显示播放列表
    LONG m_playlist_y_offset{};         //播放列表收起时窗口需要进行的y坐标偏移量

    int& m_item_selected;		//播放列表中鼠标选中的项目，引用MusicPlayerDlg类中的同名变量，当迷你窗口中播放列表选中的项目变化时，同步到主窗口中选中的项目
    vector<int>& m_items_selected;

    std::vector<std::shared_ptr<CPlayerUIBase>> m_ui_list;      //保存每个界面类的指针
    int m_ui_index{};

    CDC* m_pDC;

    CPlayListCtrl m_playlist_ctrl{ CPlayer::GetInstance().GetPlayList() };

    bool m_first_start{ true };
    bool m_always_on_top{ true };
    bool m_draw_reset{ false };
    bool m_use_ui_playlist{ true };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

    void SaveConfig() const;
    void LoadConfig();

    void SetTitle();
    void SetAlwaysOnTop();
    void AdjustWindowSize();
    bool CalculateWindowSize(int& width, int& height, int& height_with_playlist);

    DECLARE_MESSAGE_MAP()


protected:

    virtual BOOL OnInitDialog();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnTimer(UINT_PTR nIDEvent);

    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnDestroy();
    //afx_msg void OnMove(int x, int y);
    afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMiniModeExit();
    afx_msg void OnInitMenu(CMenu* pMenu);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg void OnNMDblclkList2(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMRClickList2(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnPaint();
public:
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnShowPlayList();
    afx_msg void OnMouseLeave();
    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
    afx_msg void OnMiniMideMinimize();
protected:
    afx_msg LRESULT OnListItemDragged(WPARAM wParam, LPARAM lParam);
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
public:
    afx_msg void OnMiniModeAlwaysOnTop();
protected:
    //afx_msg LRESULT OnTimerIntervalChanged(WPARAM wParam, LPARAM lParam);
public:
    afx_msg void OnExitSizeMove();
protected:
    afx_msg LRESULT OnTabletQuerysystemgesturestatus(WPARAM wParam, LPARAM lParam);
public:
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
};
