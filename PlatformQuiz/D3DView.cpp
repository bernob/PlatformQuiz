#include "pch.h"
#include "D3DView.h"

using namespace Windows::ApplicationModel::Core;
using namespace Windows::UI::Core;
using namespace Windows::System;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;

namespace Timing {
    long long Counter()
    {
        LARGE_INTEGER li;
        if (!QueryPerformanceCounter(&li))
            throw ref new Platform::FailureException();
        return li.QuadPart;
    }

    long long Frequency()
    {
        LARGE_INTEGER li;
        if (!QueryPerformanceFrequency(&li))
            throw ref new Platform::FailureException();
        return li.QuadPart;
    }
}

D3DView::D3DView(CoreWindow^ window, CoreApplicationView^ applicationView) 
    : m_window(window), m_view(applicationView), m_renderer(window)
{
    m_window->PointerCursor = ref new CoreCursor(CoreCursorType::Arrow, 0);
    m_window->SizeChanged += 
        ref new TypedEventHandler<CoreWindow^, WindowSizeChangedEventArgs^>(this, &D3DView::OnWindowSizeChanged);

    DisplayProperties::LogicalDpiChanged += 
        ref new DisplayPropertiesEventHandler(this, &D3DView::OnLogicalDpiChanged);
}

void D3DView::Run()
{
    m_window->Activate();
    m_renderer.SetDpi(static_cast<float>(DisplayProperties::LogicalDpi));

    long long frequency = Timing::Frequency();
    long long startTime = Timing::Counter();
    long long lastTime = startTime;

    while (true)
    {
        long long currentTime = Timing::Counter();
        float timeTotal = (currentTime - startTime) / (double)frequency;
        float timeDelta = (currentTime - lastTime) / (double)frequency;
        lastTime = currentTime;

        m_window->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);
        m_renderer.Update(timeTotal, timeDelta);
        m_renderer.Render();
    }
}

void D3DView::OnWindowSizeChanged(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::WindowSizeChangedEventArgs^ args)
{
    m_renderer.UpdateForWindowSizeChange();
}

void D3DView::OnLogicalDpiChanged(Platform::Object^ sender)
{
    m_renderer.SetDpi(static_cast<float>(DisplayProperties::LogicalDpi));
}

D3DView::~D3DView()
{
    m_window = nullptr;
    m_view = nullptr;
}