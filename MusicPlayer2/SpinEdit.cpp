﻿// SpinEdit.cpp: 实现文件
//

#include "stdafx.h"
#include "SpinEdit.h"


// CSpinEdit

IMPLEMENT_DYNAMIC(CSpinEdit, CEdit)

CSpinEdit::CSpinEdit()
{

}

CSpinEdit::~CSpinEdit()
{
}

void CSpinEdit::SetRange(short lower, short upper)
{
    if (m_spin.GetSafeHwnd() != NULL)
        m_spin.SetRange(lower, upper);
}

void CSpinEdit::SetValue(int value)
{
    if (m_spin.GetSafeHwnd() != NULL)
        m_spin.SetPos(value);
}

int CSpinEdit::GetValue()
{
    if (m_spin.GetSafeHwnd() != NULL)
        return m_spin.GetPos();
    else
        return 0;
}


void CSpinEdit::SetMouseWheelEnable(bool enable)
{
    m_mouse_wheel_enable = enable;
}

void CSpinEdit::SetSpinRect()
{
    // 更新 m_spin 位置
    if (::IsWindow(m_spin.GetSafeHwnd()))
    {
        CRect editRect, spinRect;
        GetWindowRect(&editRect);
        GetParent()->ScreenToClient(&editRect);
        m_spin.GetClientRect(&spinRect);
        spinRect.OffsetRect(editRect.right, editRect.top);
        m_spin.MoveWindow(&spinRect);
    }
}

BEGIN_MESSAGE_MAP(CSpinEdit, CEdit)
    ON_MESSAGE(WM_TABLET_QUERYSYSTEMGESTURESTATUS, &CSpinEdit::OnTabletQuerysystemgesturestatus)
    ON_WM_SIZE()
    ON_WM_MOVE()
END_MESSAGE_MAP()



// CSpinEdit 消息处理程序

void CSpinEdit::PreSubclassWindow()
{
    // TODO: 在此添加专用代码和/或调用基类
    CWnd* pParent = GetParent();        //获取控件的父窗口
    m_spin.Create(UDS_ALIGNRIGHT | UDS_ARROWKEYS | UDS_HOTTRACK | UDS_SETBUDDYINT | WS_VISIBLE, CRect(), pParent, SPIN_ID); //创建微调控件
    m_spin.SetBuddy(this);              //设置Edit控件为关联控件
    m_spin.SetRange(0, 999);            //设置默认范围
    SetSpinRect();
    CEdit::PreSubclassWindow();
}


BOOL CSpinEdit::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类

    //如果m_mouse_wheel_enable为false，则不响应鼠标滚轮消息
    if (pMsg->message == WM_MOUSEWHEEL && !m_mouse_wheel_enable)
    {
        //将鼠标滚轮消息发送给父窗口
        CWnd* pParent = GetParent();
        pParent->SendMessage(WM_MOUSEWHEEL, pMsg->wParam, pMsg->lParam);
        return true;
    }

    return CEdit::PreTranslateMessage(pMsg);
}


afx_msg LRESULT CSpinEdit::OnTabletQuerysystemgesturestatus(WPARAM wParam, LPARAM lParam)
{
    return 0;
}


void CSpinEdit::OnSize(UINT nType, int cx, int cy)
{
    CEdit::OnSize(nType, cx, cy);
    SetSpinRect();
}


void CSpinEdit::OnMove(int x, int y)
{
    CEdit::OnMove(x, y);
    SetSpinRect();
}
