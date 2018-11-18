#pragma once

#include <wrl.h>
#include <utility>

struct __declspec(uuid("{BD8D14DC-8F27-4947-B159-A47A72643129}")) IUnkAction : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE Invoke() = 0;
};

template<class Callable>
class UnkGenericAction : public Microsoft::WRL::RuntimeClass<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>, IUnkAction> {
public:
    UnkGenericAction(Callable callable)
        : callable(std::move(callable))
    {}

    HRESULT STDMETHODCALLTYPE Invoke() override {
        HRESULT hr = S_OK;

        this->callable();

        return hr;
    }

private:
    Callable callable;
};

template<class Callable>
Microsoft::WRL::ComPtr<UnkGenericAction<Callable>> MakeUnkGenericAction(Callable callable) {
    auto tmp = Microsoft::WRL::Make<UnkGenericAction<Callable>>(std::move(callable));
    return tmp;
}