﻿#pragma once
#include "BaseDialog.h"

// CDonateDlg 对话框

class CDonateDlg : public CBaseDialog
{
    DECLARE_DYNAMIC(CDonateDlg)

public:
    CDonateDlg(CWnd* pParent = nullptr);   // 标准构造函数
    virtual ~CDonateDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_DONATE_DIALOG };
#endif

private:
    CRect m_pic1_rect;
    CRect m_pic2_rect;

protected:
    virtual CString GetDialogName() const override;
    virtual bool IsRememberDialogSizeEnable() const { return false; };
    virtual bool InitializeControls() override;
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
};
