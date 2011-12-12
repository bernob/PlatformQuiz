#include "pch.h"
#include <d3d11_1.h>
#include "PlatformQuiz.h"
using namespace Microsoft::WRL;
using namespace Windows::UI::Core;
using namespace Windows::Foundation;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Infrastructure;
// This class defines the application as a whole.
ref class Direct3DTutorialViewProvider : public IViewProvider
{
private:
    CoreWindow^ m_window;
    ComPtr<IDXGISwapChain1> m_swapChain;
    ComPtr<ID3D11Device1> m_d3dDevice;
    ComPtr<ID3D11DeviceContext1> m_d3dDeviceContext;
    ComPtr<ID3D11RenderTargetView> m_renderTargetView;

public:
    // This method is called on application launch.
    void Initialize(
        _In_ CoreWindow^ window,
        _In_ CoreApplicationView^ applicationView
        );

    // This method is called after Initialize.
    void Load(_In_ Platform::String^ entryPoint);

    // This method is called after Load.
    void Run();

    // This method is called before the application exits.
    void Uninitialize();

private:

    // This method is called whenever the application window size changes.
    void OnWindowSizeChanged(
        _In_ CoreWindow^ sender,
        _In_ WindowSizeChangedEventArgs^ args
        );

    // This method creates all application resources that depend on
    // the application window size.  It is called at app initialization,
    // and whenever the application window size changes.
    void CreateWindowSizeDependentResources();
};

// This class defines how to create the custom View Provider defined above.
ref class Direct3DTutorialViewProviderFactory : IViewProviderFactory 
{
public:
    IViewProvider^ CreateViewProvider();
};
