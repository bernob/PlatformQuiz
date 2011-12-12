#include "pch.h"
#include "ViewProvider.h"

[Platform::MTAThread]
int main(array<Platform::String^>^)
{
    auto viewProviderFactory = ref new Direct3DTutorialViewProviderFactory();
    Windows::ApplicationModel::Core::CoreApplication::Run(viewProviderFactory);
    return 0;
}