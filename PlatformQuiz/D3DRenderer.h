#pragma once

#include <wrl.h>
#include <d3d11_1.h>
#include <dxgi1_2.h>

class D3DRenderer
{
public:
    D3DRenderer(Windows::UI::Core::CoreWindow^ window);
    ~D3DRenderer();

    void Initialize();
    void CreateDeviceIndependentResources();
    void CreateDeviceResources();
    void CreateWindowSizeDependentResources();
    void UpdateForWindowSizeChange();
    void SetDpi(float dpi);
    void DiscardDeviceResources();

    void Update(float timeTotal, float timeDelta);
    void Render();
    void Present();
    void Clear();

private:
    Windows::UI::Core::CoreWindow^ m_window;
    Microsoft::WRL::ComPtr<ID3D11Device> m_d3dDevice;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_d3dContext;
    Microsoft::WRL::ComPtr<IDXGISwapChain1> m_swapChain;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_depthStencil;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthStencilView;
    D3D_FEATURE_LEVEL m_featureLevel;
    float m_dpi;
};