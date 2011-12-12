#pragma once

#include "D3DRenderer.h"

ref class D3DView
{
public:
    D3DView(Windows::UI::Core::CoreWindow^ window, Windows::ApplicationModel::Core::CoreApplicationView^ applicationView);
    ~D3DView();

    void Run();

    void OnWindowSizeChanged(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::WindowSizeChangedEventArgs^ args);
    void OnLogicalDpiChanged(Platform::Object^ sender);

private:

    // Basics
    Windows::UI::Core::CoreWindow^ m_window;
    Windows::ApplicationModel::Core::CoreApplicationView^ m_view;

    // DirectX
    D3DRenderer m_renderer;
};