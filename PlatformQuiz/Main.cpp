#include "pch.h"
#include "ViewProvider.h"

[Platform::MTAThread]
int main(array<Platform::String^>^)
{
    auto viewProvider = ref new ViewProviderFactory();
    Windows::ApplicationModel::Core::CoreApplication::Run(viewProvider);
}
