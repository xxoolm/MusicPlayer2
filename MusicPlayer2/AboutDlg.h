﻿#pragma once
#include "BaseDialog.h"

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CBaseDialog
{
public:
    CAboutDlg();
    virtual ~CAboutDlg();

    // 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_ABOUTBOX };
#endif

protected:
    CToolTipCtrl m_tool_tip;        //鼠标指向时的工具提示
    CRect m_rc_pic;
    CImage m_about_pic;
    int m_white_height{};           // 背景白色区域的高度

    virtual CString GetDialogName() const;
    virtual bool IsRememberDialogSizeEnable() const override { return false; };
    virtual bool InitializeControls() override;
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnNMClickSyslink1(NMHDR *pNMHDR, LRESULT *pResult);
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnNMClickSyslink2(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMClickGithubSyslink(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMClickDonateSyslink(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnPaint();
    afx_msg void OnNMClickLicenseSyslink(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg void OnNMClickSyslinkBass(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnNMClickSyslinkTaglib(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnNMClickSyslinkTinyxml2(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnNMClickSyslinkScintilla(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnNMClickSyslinkTrafficmonitor(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnNMClickSyslinkSimplenotepad(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnNMClickAcknowledgementSyslink(NMHDR* pNMHDR, LRESULT* pResult);
};
