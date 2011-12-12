#pragma once

enum class ActivationEntryPoint
{
    Unknown,
    PlatformQuiz
};

ref class ViewProvider : public Windows::ApplicationModel::Infrastructure::IViewProvider
{
public:
    ViewProvider();

    void Initialize(Windows::UI::Core::CoreWindow^ window, Windows::ApplicationModel::Core::CoreApplicationView^ applicationView);
    void Load(Platform::String^ entryPoint);
    void Run();
    void Uninitialize();

private:
    ActivationEntryPoint m_activationEntryPoint;
    Windows::UI::Core::CoreWindow^ m_window;
    Windows::ApplicationModel::Core::CoreApplicationView^ m_applicationView;
};

ref class ViewProviderFactory : Windows::ApplicationModel::Infrastructure::IViewProviderFactory 
{
public:
    ViewProviderFactory() {}
    Windows::ApplicationModel::Infrastructure::IViewProvider^ CreateViewProvider()
    {
        return ref new ViewProvider();
    }
};
