/*
 * (C) 2003-2006 Gabest
 * (C) 2006-2014 see Authors.txt
 *
 * This file is part of MPC-HC.
 *
 * MPC-HC is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * MPC-HC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#pragma once

#include <mutex>
#include <condition_variable>

#include "ISubPic.h"

class CSubPicQueueImpl : public CUnknown, public ISubPicQueue
{
    CCritSec m_csSubPicProvider;
    CComPtr<ISubPicProvider> m_pSubPicProvider;

protected:
    double m_fps;
    REFERENCE_TIME m_rtNow;

    bool m_bDisableAnim;

    CComPtr<ISubPicAllocator> m_pAllocator;

    HRESULT RenderTo(ISubPic* pSubPic, REFERENCE_TIME rtStart, REFERENCE_TIME rtStop, double fps, BOOL bIsAnimated);

public:
    CSubPicQueueImpl(bool bDisableAnim, ISubPicAllocator* pAllocator, HRESULT* phr);
    virtual ~CSubPicQueueImpl();

    DECLARE_IUNKNOWN;
    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void** ppv);

    // ISubPicQueue

    STDMETHODIMP SetSubPicProvider(ISubPicProvider* pSubPicProvider);
    STDMETHODIMP GetSubPicProvider(ISubPicProvider** pSubPicProvider);

    STDMETHODIMP SetFPS(double fps);
    STDMETHODIMP SetTime(REFERENCE_TIME rtNow);
    /*
    STDMETHODIMP Invalidate(REFERENCE_TIME rtInvalidate = -1) PURE;
    STDMETHODIMP_(bool) LookupSubPic(REFERENCE_TIME rtNow, ISubPic** ppSubPic) PURE;

    STDMETHODIMP GetStats(int& nSubPics, REFERENCE_TIME& rtNow, REFERENCE_TIME& rtStart, REFERENCE_TIME& rtStop) PURE;
    STDMETHODIMP GetStats(int nSubPics, REFERENCE_TIME& rtStart, REFERENCE_TIME& rtStop) PURE;
    */
};

class CSubPicQueue : public CSubPicQueueImpl, protected CAMThread
{
protected:
    bool m_bExitThread;

    int m_nMaxSubPic;

    CComPtr<ISubPic> m_pSubPic;
    CInterfaceList<ISubPic> m_queue;

    std::mutex m_mutexSubpic; // to protect m_pSubPic
    std::mutex m_mutexQueue; // to protect m_queue
    std::condition_variable m_condQueueFull;

    CAMEvent m_runQueueEvent;

    REFERENCE_TIME m_rtNowLast;

    bool m_bInvalidate;
    REFERENCE_TIME m_rtInvalidate;

    bool EnqueueSubPic(CComPtr<ISubPic>& pSubPic, bool bBlocking);
    REFERENCE_TIME GetCurrentRenderingTime();

    // CAMThread
    virtual DWORD ThreadProc();

public:
    CSubPicQueue(int nMaxSubPic, bool bDisableAnim, ISubPicAllocator* pAllocator, HRESULT* phr);
    virtual ~CSubPicQueue();

    // ISubPicQueue

    STDMETHODIMP SetFPS(double fps);
    STDMETHODIMP SetTime(REFERENCE_TIME rtNow);

    STDMETHODIMP Invalidate(REFERENCE_TIME rtInvalidate = -1);
    STDMETHODIMP_(bool) LookupSubPic(REFERENCE_TIME rtNow, CComPtr<ISubPic>& pSubPic);

    STDMETHODIMP GetStats(int& nSubPics, REFERENCE_TIME& rtNow, REFERENCE_TIME& rtStart, REFERENCE_TIME& rtStop);
    STDMETHODIMP GetStats(int nSubPic, REFERENCE_TIME& rtStart, REFERENCE_TIME& rtStop);
};

class CSubPicQueueNoThread : public CSubPicQueueImpl
{
protected:
    CCritSec m_csLock;
    CComPtr<ISubPic> m_pSubPic;

public:
    CSubPicQueueNoThread(bool bDisableAnim, ISubPicAllocator* pAllocator, HRESULT* phr);
    virtual ~CSubPicQueueNoThread();

    // ISubPicQueue

    STDMETHODIMP Invalidate(REFERENCE_TIME rtInvalidate = -1);
    STDMETHODIMP_(bool) LookupSubPic(REFERENCE_TIME rtNow, CComPtr<ISubPic>& pSubPic);

    STDMETHODIMP GetStats(int& nSubPics, REFERENCE_TIME& rtNow, REFERENCE_TIME& rtStart, REFERENCE_TIME& rtStop);
    STDMETHODIMP GetStats(int nSubPic, REFERENCE_TIME& rtStart, REFERENCE_TIME& rtStop);
};
