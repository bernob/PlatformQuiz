#include "pch.h"

#include "ViewProvider.h"
#include "D3DView.h"

using namespace Windows::ApplicationModel::Core;
using namespace Windows::UI::Core;
using namespace Windows::ApplicationModel::Activation;

ViewProvider::ViewProvider()
{
    m_activationEntryPoint = ActivationEntryPoint::Unknown;
}

void ViewProvider::Initialize(Windows::UI::Core::CoreWindow^ window, Windows::ApplicationModel::Core::CoreApplicationView^ applicationView)
{
    m_window = window;
    m_applicationView = applicationView;
}

// this method is called after Initialize
void ViewProvider::Load(Platform::String^ entryPoint )
{
    if (entryPoint == "PlatformQuiz.App")
    {
        m_activationEntryPoint = ActivationEntryPoint::PlatformQuiz;
    }
}

// this method is called after Load
void ViewProvider::Run()
{
    if (m_activationEntryPoint == ActivationEntryPoint::PlatformQuiz)
    {
        auto view = ref new D3DView(m_window, m_applicationView);
        view->Run();

        // Must delete the view explicitly in order to break a circular dependency
        // between View and CoreWindow. View holds on to a CoreWindow reference most
        // typically for window activation, while CoreWindow refers back to View when
        // event handlers are hooked up. Without breaking this circular dependency,
        // neither View nor CoreWindow object gets to clean up. It's also important
        // to note that a 'delete' call on a ref class instance simply means calling
        // into a class destructor in order to explicitly break a cycle. It doesn't
        // actually deallocate any memory.
        delete view;
    }
}

void ViewProvider::Uninitialize()
{
}
