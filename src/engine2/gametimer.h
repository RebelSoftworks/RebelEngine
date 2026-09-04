//========= Copyright (c) 2026 Mohamed Ashraf, All rights reserved. ============//
//
// This software is proprietary and confidential.
//
// You are granted a limited, non-exclusive, non-transferable license to use and
// modify this software for personal or internal purposes only. You may distribute
// compiled binary builds of the software provided that such distribution does not
// expose, include, or facilitate access to the source code.
//
// You may not distribute, publish, disclose, or otherwise make available the
// source code, in whole or in part, whether original or modified. All rights
// not expressly granted herein are reserved by the copyright holder.
//
// This software is provided "as is", without warranty of any kind, express or
// implied.
//
//=============================================================================//
#ifndef GAMETIMER_H
#define GAMETIMER_H
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <profileapi.h>
#else
#include <time.h>
#include <tier0/platform.h>
#endif

class CGameTimer
{
public:
    void Init()
    {
#ifndef POSIX
        QueryPerformanceFrequency(&m_Frequency);
        QueryPerformanceCounter(&m_LastTime);
#else
        clock_gettime(CLOCK_MONOTONIC, &m_LastTime);
#endif
    }

    void BeginFrame()
    {
#ifdef POSIX
        timespec current;
        clock_gettime(CLOCK_MONOTONIC, &current);

        m_DeltaTime =
            static_cast<float>(current.tv_sec - m_LastTime.tv_sec) +
            static_cast<float>(current.tv_nsec - m_LastTime.tv_nsec) /
            1'000'000'000.0f;

        m_LastTime = current;

        if (m_DeltaTime > 0.1f)
            m_DeltaTime = 0.1f;
#else
        LARGE_INTEGER current;
        QueryPerformanceCounter(&current);

        m_DeltaTime = static_cast<float>(current.QuadPart - m_LastTime.QuadPart) /
            static_cast<float>(m_Frequency.QuadPart);

        m_LastTime = current;

        if (m_DeltaTime > 0.1f)
            m_DeltaTime = 0.1f;
#endif // POSIX
    }

    float GetDeltaTime() const
    {
        return m_DeltaTime;
    }

private:
    LARGE_INTEGER m_Frequency{};
    LARGE_INTEGER m_LastTime{};
    float m_DeltaTime = 0.0f;
};

extern CGameTimer g_GameTimer;

#endif // !GAMETIMER_H
