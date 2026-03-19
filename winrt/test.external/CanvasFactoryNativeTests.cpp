// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"
#include <WeakReference.h>

using ABI::Microsoft::Graphics::Canvas::ICanvasEffectFactoryNative;
using ABI::Microsoft::Graphics::Canvas::ICanvasImageInterop;
using ABI::Microsoft::Graphics::Canvas::WIN2D_GET_DEVICE_ASSOCIATION_TYPE;
using ABI::Microsoft::Graphics::Canvas::WIN2D_GET_D2D_IMAGE_FLAGS;
using namespace Microsoft::Graphics::Canvas;
using namespace Microsoft::Graphics::Canvas::Effects;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI;
using namespace Platform;

TEST_CLASS(CanvasFactoryNativeTests)
{
    // Dummy WinRT object implementing IWeakReferenceSource (the C++/CX tooling adds the interface automatically)
    class DummyWeakReferenceSource : public RuntimeClass<
        RuntimeClassFlags<WinRtClassicComMix>,
        ABI::Microsoft::Graphics::Canvas::ICanvasImage,
        ABI::Windows::Graphics::Effects::IGraphicsEffectSource>
    {
        InspectableClass(L"EffectsAbi::IGraphicsEffectD2D1Interop::DummyWeakReferenceSource", TrustLevel::BaseTrust);

        ComPtr<ABI::Microsoft::Graphics::Canvas::ICanvasImage> m_canvasImage;

    public:
        DummyWeakReferenceSource()
        {
        }

        // ICanvasImage

        IFACEMETHODIMP GetBounds(
            ABI::Microsoft::Graphics::Canvas::ICanvasResourceCreator* resourceCreator,
            ABI::Windows::Foundation::Rect* bounds
        ) override
        {
            Assert::Fail(L"Unexpected call to GetBounds");
        }

        IFACEMETHODIMP GetBoundsWithTransform(
            ABI::Microsoft::Graphics::Canvas::ICanvasResourceCreator* resourceCreator,
            ABI::Windows::Foundation::Numerics::Matrix3x2 transform,
            ABI::Windows::Foundation::Rect* bounds
        ) override
        {
            Assert::Fail(L"Unexpected call to GetBoundsWithTransform");
        }
    };

    // Dummy WinRT object not implementing IWeakReferenceSource
    class DummyNonWeakReferenceSource : public RuntimeClass<
        RuntimeClassFlags<WinRtClassicComMix | InhibitWeakReference>,
        ABI::Microsoft::Graphics::Canvas::ICanvasImage,
        ABI::Windows::Graphics::Effects::IGraphicsEffectSource>
    {
        InspectableClass(L"EffectsAbi::IGraphicsEffectD2D1Interop::DummyNonWeakReferenceSource", TrustLevel::BaseTrust);

        ComPtr<ABI::Microsoft::Graphics::Canvas::ICanvasImage> m_canvasImage;

    public:
        DummyNonWeakReferenceSource()
        {
        }

        // ICanvasImage

        IFACEMETHODIMP GetBounds(
            ABI::Microsoft::Graphics::Canvas::ICanvasResourceCreator* resourceCreator,
            ABI::Windows::Foundation::Rect* bounds
        ) override
        {
            Assert::Fail(L"Unexpected call to GetBounds");
        }

        IFACEMETHODIMP GetBoundsWithTransform(
            ABI::Microsoft::Graphics::Canvas::ICanvasResourceCreator* resourceCreator,
            ABI::Windows::Foundation::Numerics::Matrix3x2 transform,
            ABI::Windows::Foundation::Rect* bounds
        ) override
        {
            Assert::Fail(L"Unexpected call to GetBoundsWithTransform");
        }
    };

    class ImageInteropProxy : public RuntimeClass<
        RuntimeClassFlags<WinRtClassicComMix>,
        ABI::Microsoft::Graphics::Canvas::ICanvasImage,
        ABI::Windows::Graphics::Effects::IGraphicsEffectSource,
        ICanvasImageInterop>
    {
        InspectableClass(L"EffectsAbi::IGraphicsEffectD2D1Interop::ImageInteropProxy", TrustLevel::BaseTrust);

        ComPtr<ABI::Microsoft::Graphics::Canvas::ICanvasDevice> m_device;
        WIN2D_GET_DEVICE_ASSOCIATION_TYPE m_type;

    public:
        ImageInteropProxy(ABI::Microsoft::Graphics::Canvas::ICanvasDevice* device, WIN2D_GET_DEVICE_ASSOCIATION_TYPE type)
        {
            m_device = device;
            m_type = type;

        }

        // ICanvasImage

        IFACEMETHODIMP GetBounds(
            ABI::Microsoft::Graphics::Canvas::ICanvasResourceCreator* resourceCreator,
            ABI::Windows::Foundation::Rect* bounds
        ) override
        {
            Assert::Fail(L"Unexpected call to GetBounds");
        }

        IFACEMETHODIMP GetBoundsWithTransform(
            ABI::Microsoft::Graphics::Canvas::ICanvasResourceCreator* resourceCreator,
            ABI::Windows::Foundation::Numerics::Matrix3x2 transform,
            ABI::Windows::Foundation::Rect* bounds
        ) override
        {
            Assert::Fail(L"Unexpected call to GetBoundsWithTransform");
        }

        // ICanvasImageInterop

        IFACEMETHODIMP GetDevice(ABI::Microsoft::Graphics::Canvas::ICanvasDevice** device, WIN2D_GET_DEVICE_ASSOCIATION_TYPE* type) override
        {
            *type = m_type;

            return m_device.CopyTo(device);
        }

        IFACEMETHODIMP GetD2DImage(
            ABI::Microsoft::Graphics::Canvas::ICanvasDevice* device,
            ID2D1DeviceContext* deviceContext,
            WIN2D_GET_D2D_IMAGE_FLAGS flags,
            float targetDpi,
            float* realizeDpi,
            ID2D1Image** ppImage) override
        {
            *ppImage = nullptr;

            return E_NOTIMPL;
        }
    };

    class CanvasImageFactory : public RuntimeClass<
        RuntimeClassFlags<WinRtClassicComMix>,
        ICanvasEffectFactoryNative>
    {
        InspectableClass(L"EffectsAbi::IGraphicsEffectD2D1Interop::CanvasImageFactory", TrustLevel::BaseTrust);

        ComPtr<IInspectable> m_object;

    public:
        CanvasImageFactory()
        {
        }

        // Updates the wrapper to return from CreateWrapper
        void SetWrapper(IInspectable* wrapper)
        {
            m_object = wrapper;
        }

        // ICanvasEffectFactoryNative

        IFACEMETHODIMP CreateWrapper(
            ABI::Microsoft::Graphics::Canvas::ICanvasDevice* device,
            ID2D1Effect* resource,
            float dpi,
            IInspectable** wrapper) override
        {
            m_object.CopyTo(wrapper);

            return S_OK;
        }
    };

    // Mock effect to validate the test factory. We're not actually creating a proper D2D effect because it's not
    // really needed for what we need here (this is enough), plus that's just a ridiculous amount of extra work.
    class MockD2DEffect : public RuntimeClass<
        RuntimeClassFlags<WinRtClassicComMix>,
        ID2D1Effect>
    {
        InspectableClass(L"EffectsAbi::IGraphicsEffectD2D1Interop::MockD2DEffect", TrustLevel::BaseTrust);

        IID m_effectId;

    public:
        MockD2DEffect(REFIID effectId)
        {
            m_effectId = effectId;
        }

        //
        // ID2D1Effect
        //

        STDMETHOD_(void, SetInput)(UINT32 index, ID2D1Image* input, BOOL invalidate = TRUE)
        {
            Assert::Fail(L"Unexpected call to SetInput");
        }

        STDMETHOD(SetInputCount)(UINT32 inputCount)
        {
            Assert::Fail(L"Unexpected call to SetInputCount");
        }

        STDMETHOD_(void, GetInput)(UINT32 index, ID2D1Image** input) CONST
        {
            Assert::Fail(L"Unexpected call to GetInput");
        }

        STDMETHOD_(UINT32, GetInputCount)() CONST
        {
            Assert::Fail(L"Unexpected call to GetInputCount");
        }

        STDMETHOD_(void, GetOutput)(ID2D1Image** outputImage) CONST
        {
            Assert::Fail(L"Unexpected call to GetOutput");
        }

        //
        // ID2D1Properties
        //

        STDMETHOD_(UINT32, GetPropertyCount)() CONST
        {
            Assert::Fail(L"Unexpected call to GetPropertyCount");
        }

        STDMETHOD(GetPropertyName)(UINT32 index, PWSTR name,UINT32 nameCount) CONST
        {
            Assert::Fail(L"Unexpected call to GetPropertyName");
        }

        STDMETHOD_(UINT32, GetPropertyNameLength)(UINT32 index) CONST
        {
            Assert::Fail(L"Unexpected call to GetPropertyNameLength");
        }

        STDMETHOD_(D2D1_PROPERTY_TYPE, GetType)(UINT32 index) CONST
        {
            Assert::Fail(L"Unexpected call to GetType");
        }

        STDMETHOD_(UINT32, GetPropertyIndex)(PCWSTR name) CONST
        {
            Assert::Fail(L"Unexpected call to GetPropertyIndex");
        }

        STDMETHOD(SetValueByName)(PCWSTR name, D2D1_PROPERTY_TYPE type, CONST BYTE* data, UINT32 dataSize)
        {
            Assert::Fail(L"Unexpected call to SetValueByName");
        }

        STDMETHOD(SetValue)(UINT32 index, D2D1_PROPERTY_TYPE type, CONST BYTE* data, UINT32 dataSize)
        {
            Assert::Fail(L"Unexpected call to SetValue");
        }

        STDMETHOD(GetValueByName)(PCWSTR name, D2D1_PROPERTY_TYPE type, BYTE* data, UINT32 dataSize) CONST
        {
            Assert::Fail(L"Unexpected call to GetValueByName");
        }

        STDMETHOD(GetValue)(UINT32 index, D2D1_PROPERTY_TYPE type, BYTE* data, UINT32 dataSize) CONST
        {
            // Only support getting the effect id (don't bother validating the other parameters here).
            // This is called from within Win2D with D2D helpers and those will correctly match an IID.
            if (index == static_cast<UINT32>(D2D1_PROPERTY_CLSID))
            {
                *reinterpret_cast<IID*>(data) = m_effectId;
            }
            else
            {
                Assert::Fail(L"Unexpected call to GetValue");
            }

            return S_OK;
        }

        STDMETHOD_(UINT32, GetValueSize)(UINT32 index) CONST
        {
            Assert::Fail(L"Unexpected call to GetValueSize");
        }

        STDMETHOD(GetSubProperties)(UINT32 index, ID2D1Properties** subProperties) CONST
        {
            Assert::Fail(L"Unexpected call to GetSubProperties");
        }
    };

    class DummyCanvasImage : public RuntimeClass<
        RuntimeClassFlags<WinRtClassicComMix>,
        ABI::Microsoft::Graphics::Canvas::ICanvasImage,
        ABI::Windows::Graphics::Effects::IGraphicsEffectSource,
        ABI::Windows::Foundation::IClosable,
        ICanvasImageInterop>
    {
        InspectableClass(L"EffectsAbi::IGraphicsEffectD2D1Interop::DummyCanvasImage", TrustLevel::BaseTrust);

    public:
        DummyCanvasImage()
        {
        }

        // ICanvasImage

        IFACEMETHODIMP GetBounds(
            ABI::Microsoft::Graphics::Canvas::ICanvasResourceCreator* resourceCreator,
            ABI::Windows::Foundation::Rect* bounds
        ) override
        {
            Assert::Fail(L"Unexpected call to GetBounds");
        }

        IFACEMETHODIMP GetBoundsWithTransform(
            ABI::Microsoft::Graphics::Canvas::ICanvasResourceCreator* resourceCreator,
            ABI::Windows::Foundation::Numerics::Matrix3x2 transform,
            ABI::Windows::Foundation::Rect* bounds
        ) override
        {
            Assert::Fail(L"Unexpected call to GetBoundsWithTransform");
        }

        // IClosable

        IFACEMETHODIMP Close() override
        {
            Assert::Fail(L"Unexpected call to Close");
        }

        // ICanvasImageInterop

        IFACEMETHODIMP GetDevice(ABI::Microsoft::Graphics::Canvas::ICanvasDevice** device, WIN2D_GET_DEVICE_ASSOCIATION_TYPE* type) override
        {
            *device = nullptr;
            *type = WIN2D_GET_DEVICE_ASSOCIATION_TYPE::WIN2D_GET_DEVICE_ASSOCIATION_TYPE_UNSPECIFIED;

            return S_OK;
        }

        IFACEMETHODIMP GetD2DImage(
            ABI::Microsoft::Graphics::Canvas::ICanvasDevice* device,
            ID2D1DeviceContext* deviceContext,
            WIN2D_GET_D2D_IMAGE_FLAGS flags,
            float targetDpi,
            float* realizeDpi,
            ID2D1Image** ppImage) override
        {
            Assert::Fail(L"Unexpected call to GetD2DImage");
        }
    };

    TEST_METHOD(CanvasFactoryNative_RegisterUnregisterWrapper_ChecksNullInputs)
    {
        auto canvasDevice = ref new CanvasDevice();

        auto canvasBitmap = CanvasBitmap::CreateFromColors(
            canvasDevice,
            ref new Platform::Array<Color>(256 * 256),
            256,
            256,
            96.0f,
            CanvasAlphaMode::Ignore);

        // Get the underlying image
        ComPtr<ID2D1Image> d2DImage;
        ThrowIfFailed(As<ABI::Microsoft::Graphics::Canvas::ICanvasResourceWrapperNative>(canvasBitmap)->GetNativeResource(
            As<ABI::Microsoft::Graphics::Canvas::ICanvasDevice>(canvasDevice).Get(),
            96.0f,
            IID_PPV_ARGS(&d2DImage)));

        // Get the ICanvasFactoryNative activation factory to test the ABI methods
        ComPtr<ABI::Microsoft::Graphics::Canvas::ICanvasFactoryNative> factory;
        ThrowIfFailed(Windows::Foundation::GetActivationFactory(
            reinterpret_cast<HSTRING>(CanvasDevice::typeid->FullName),
            &factory));

        // Ensure the methods perform null checks correctly
        Assert::AreEqual(E_INVALIDARG, factory->RegisterWrapper(nullptr, (IInspectable*)(void*)canvasBitmap));
        Assert::AreEqual(E_INVALIDARG, factory->RegisterWrapper(d2DImage.Get(), (IInspectable*)nullptr));
        Assert::AreEqual(E_INVALIDARG, factory->RegisterWrapper(nullptr, (IInspectable*)nullptr));
        Assert::AreEqual(E_INVALIDARG, factory->UnregisterWrapper(nullptr));

        // Ensure the C++/CX wrapper handles failures as expected
        Assert::ExpectException<Platform::InvalidArgumentException^>([&] { RegisterWrapper(nullptr, canvasBitmap); });
        Assert::ExpectException<Platform::InvalidArgumentException^>([&] { RegisterWrapper(d2DImage.Get(), (CanvasBitmap^)nullptr); });
        Assert::ExpectException<Platform::InvalidArgumentException^>([&] { RegisterWrapper(nullptr, (CanvasBitmap^)nullptr); });
        Assert::ExpectException<Platform::InvalidArgumentException^>([&] { UnregisterWrapper(nullptr); });
    }

    TEST_METHOD(CanvasFactoryNative_RegisterUnregisterWrapper_ChecksInvalidInputs)
    {
        // Just create a handful of unsupported objects to test against
        auto canvasDevice = ref new CanvasDevice();
        auto canvasBrush = ref new CanvasSolidColorBrush(canvasDevice, Windows::UI::Colors::Red);

        // Also create a bitmap image
        auto canvasBitmap = CanvasBitmap::CreateFromColors(
            canvasDevice,
            ref new Platform::Array<Color>(256 * 256),
            256,
            256,
            96.0f,
            CanvasAlphaMode::Ignore);

        // Get the underlying image
        ComPtr<ID2D1Image> d2DImage;
        ThrowIfFailed(As<ABI::Microsoft::Graphics::Canvas::ICanvasResourceWrapperNative>(canvasBitmap)->GetNativeResource(
            As<ABI::Microsoft::Graphics::Canvas::ICanvasDevice>(canvasDevice).Get(),
            96.0f,
            IID_PPV_ARGS(&d2DImage)));

        // Get the ICanvasFactoryNative activation factory to test the ABI methods
        ComPtr<ABI::Microsoft::Graphics::Canvas::ICanvasFactoryNative> factory;
        ThrowIfFailed(Windows::Foundation::GetActivationFactory(
            reinterpret_cast<HSTRING>(CanvasDevice::typeid->FullName),
            &factory));        

        // Passing invalid D2D types (only ID2D1Image is allowed)
        Assert::AreEqual(E_INVALIDARG, factory->RegisterWrapper(GetWrappedResource<IUnknown, CanvasDevice>(canvasDevice).Get(), reinterpret_cast<IInspectable*>(canvasBitmap)));
        Assert::AreEqual(E_INVALIDARG, factory->RegisterWrapper(GetWrappedResource<IUnknown, CanvasSolidColorBrush>(canvasBrush).Get(), reinterpret_cast<IInspectable*>(canvasBitmap)));
       
        // Passing invalid Win2D types (only ICanvasImage is allowed)
        Assert::AreEqual(E_INVALIDARG, factory->RegisterWrapper(d2DImage.Get(), reinterpret_cast<IInspectable*>(canvasDevice)));
        Assert::AreEqual(E_INVALIDARG, factory->RegisterWrapper(d2DImage.Get(), reinterpret_cast<IInspectable*>(canvasBrush)));

        // Passing invalid D2D types again, but this time removing wrappers
        Assert::AreEqual(E_INVALIDARG, factory->UnregisterWrapper(GetWrappedResource<IUnknown, CanvasDevice>(canvasDevice).Get()));
        Assert::AreEqual(E_INVALIDARG, factory->UnregisterWrapper(GetWrappedResource<IUnknown, CanvasSolidColorBrush>(canvasBrush).Get()));
    }

    TEST_METHOD(CanvasFactoryNative_RegisterUnregisterWrapper_UpdateCacheCorrectly)
    {
        auto canvasDevice = ref new CanvasDevice();

        auto canvasBitmap = CanvasBitmap::CreateFromColors(
            canvasDevice,
            ref new Platform::Array<Color>(256 * 256),
            256,
            256,
            96.0f,
            CanvasAlphaMode::Ignore);

        // Get the underlying image
        ComPtr<ID2D1Image> d2DImage;
        ThrowIfFailed(As<ABI::Microsoft::Graphics::Canvas::ICanvasResourceWrapperNative>(canvasBitmap)->GetNativeResource(
            As<ABI::Microsoft::Graphics::Canvas::ICanvasDevice>(canvasDevice).Get(),
            96.0f,
            IID_PPV_ARGS(&d2DImage)));

        // Get the ICanvasFactoryNative activation factory to test the ABI methods
        ComPtr<ABI::Microsoft::Graphics::Canvas::ICanvasFactoryNative> factory;
        ThrowIfFailed(Windows::Foundation::GetActivationFactory(
            reinterpret_cast<HSTRING>(CanvasDevice::typeid->FullName),
            &factory));

        // The effect is already in the cache
        Assert::IsFalse(RegisterWrapper(d2DImage.Get(), canvasBitmap));

        // The image can be removed, and it can be removed only once
        Assert::IsTrue(UnregisterWrapper(d2DImage.Get()));
        Assert::IsFalse(UnregisterWrapper(d2DImage.Get()));

        // After this, the image can now be added again, and only once
        Assert::IsTrue(RegisterWrapper(d2DImage.Get(), canvasBitmap));
        Assert::IsFalse(RegisterWrapper(d2DImage.Get(), canvasBitmap));

        // Using GetOrCreate will correctly get the original instance
        Platform::Object^ cachedResource1 = GetOrCreate<Platform::Object>(d2DImage.Get());

        Assert::IsTrue(As<IUnknown>(canvasBitmap).Get() == As<IUnknown>(cachedResource1).Get());

        ColorSourceEffect^ dummyEffect = ref new ColorSourceEffect();

        // Replace the tracked resource for the image with the dummy effect
        Assert::IsTrue(UnregisterWrapper(d2DImage.Get()));
        Assert::IsTrue(RegisterWrapper(d2DImage.Get(), dummyEffect));

        // Using GetOrCreate will now return the dummy effect
        Platform::Object^ cachedResource2 = GetOrCreate<Platform::Object>(d2DImage.Get());

        Assert::IsTrue(As<IUnknown>(dummyEffect).Get() == As<IUnknown>(cachedResource2).Get());

        // Create a dummy type (we want to check arbitrary types are also accepted by the cache)
        ComPtr<DummyWeakReferenceSource> dummyObject = Make<DummyWeakReferenceSource>();

        // Sanity check: the object implements IWeakReferenceSource
        Assert::IsNotNull(As<IWeakReferenceSource>(dummyObject.Get()).Get());

        Platform::Object^ dummyObject2 = reinterpret_cast<Platform::Object^>(As<ABI::Windows::Graphics::Effects::IGraphicsEffectSource>(dummyObject.Get()).Get());

        // Add the dummy object to the resource cache
        Assert::IsTrue(UnregisterWrapper(d2DImage.Get()));
        Assert::IsTrue(RegisterWrapper(d2DImage.Get(), dummyObject2));

        // GetOrCreate will correctly return the dummy object now
        Platform::Object^ cachedResource3 = GetOrCreate<Platform::Object>(d2DImage.Get());

        Assert::IsTrue(As<IUnknown>(dummyObject).Get() == As<IUnknown>(cachedResource3).Get());

        // Create a dummy type without weak reference support
        ComPtr<DummyNonWeakReferenceSource> dummyObjectWithNoWeakRef = Make<DummyNonWeakReferenceSource>();

        // Sanity check: the object doesn't implement IWeakReferenceSource
        Assert::IsNull(MaybeAs<IWeakReferenceSource>(dummyObjectWithNoWeakRef.Get()).Get());

        Platform::Object^ dummyObject3 = reinterpret_cast<Platform::Object^>(As<ABI::Windows::Graphics::Effects::IGraphicsEffectSource>(dummyObjectWithNoWeakRef.Get()).Get());

        // Try to add the dummy object to the cache
        Assert::AreEqual(E_NOINTERFACE, factory->RegisterWrapper(d2DImage.Get(), (IInspectable*)(void*)dummyObject3));
        Assert::ExpectException<Platform::InvalidCastException^>([&] { RegisterWrapper(d2DImage.Get(), dummyObject3); });

        // GetOrCreate will return the previous object (since adding the new one failed)
        Platform::Object^ cachedResource4 = GetOrCreate<Platform::Object>(d2DImage.Get());

        Assert::IsTrue(As<IUnknown>(dummyObject).Get() == As<IUnknown>(cachedResource4).Get());

        // Restore the original CanvasBitmap in the resource cache
        Assert::IsTrue(UnregisterWrapper(d2DImage.Get()));
        Assert::IsTrue(RegisterWrapper(d2DImage.Get(), canvasBitmap));

        // Sanity check: GetOrCreate will return the original bitmap now
        Platform::Object^ cachedResource5 = GetOrCreate<Platform::Object>(d2DImage.Get());

        Assert::IsTrue(As<IUnknown>(canvasBitmap).Get() == As<IUnknown>(cachedResource5).Get());
    }

    TEST_METHOD(CanvasFactoryNative_RegisterUnregisterWrapper_GetOrAddPerformsValidationCorrectly)
    {
        // Get a device and also retrieve its ABI, as it's needed later to create dummy effects
        auto canvasDevice1 = ref new CanvasDevice();
        auto canvasDevice1Abi = As<ABI::Microsoft::Graphics::Canvas::ICanvasDevice>(canvasDevice1);

        // Also get a different device, used for tests later
        auto canvasDevice2 = ref new CanvasDevice();
        auto canvasDevice2Abi = As<ABI::Microsoft::Graphics::Canvas::ICanvasDevice>(canvasDevice2);

        ColorSourceEffect^ colorEffect = ref new ColorSourceEffect();

        // Get the underlying image
        ComPtr<ID2D1Image> d2DImage;
        ThrowIfFailed(As<ABI::Microsoft::Graphics::Canvas::ICanvasResourceWrapperNative>(colorEffect)->GetNativeResource(
            As<ABI::Microsoft::Graphics::Canvas::ICanvasDevice>(canvasDevice1).Get(),
            96.0f,
            IID_PPV_ARGS(&d2DImage)));

        // Remove it from the cache, so we can reuse the native resource
        Assert::IsTrue(UnregisterWrapper(d2DImage.Get()));

        // Get the ICanvasFactoryNative activation factory to test the ABI methods
        ComPtr<ABI::Microsoft::Graphics::Canvas::ICanvasFactoryNative> factory;
        ThrowIfFailed(Windows::Foundation::GetActivationFactory(
            reinterpret_cast<HSTRING>(CanvasDevice::typeid->FullName),
            &factory));

        // Create an effect with no device and WIN2D_GET_DEVICE_ASSOCIATION_TYPE_UNSPECIFIED
        ComPtr<ImageInteropProxy> proxyEffect1 = Make<ImageInteropProxy>(nullptr, WIN2D_GET_DEVICE_ASSOCIATION_TYPE::WIN2D_GET_DEVICE_ASSOCIATION_TYPE_UNSPECIFIED);
        Platform::Object^ proxyEffect1Object = reinterpret_cast<Platform::Object^>(As<ABI::Windows::Graphics::Effects::IGraphicsEffectSource>(proxyEffect1.Get()).Get());

        // Add the proxy object to the cache
        Assert::IsTrue(RegisterWrapper(d2DImage.Get(), proxyEffect1Object));

        ComPtr<IInspectable> resultInspectable;

        // The effect can always be retrieved regardless of device being passed
        Assert::AreEqual(S_OK, factory->GetOrCreate(nullptr, d2DImage.Get(), 0, resultInspectable.ReleaseAndGetAddressOf()));
        Assert::AreEqual(S_OK, factory->GetOrCreate(canvasDevice1Abi.Get(), d2DImage.Get(), 0, resultInspectable.ReleaseAndGetAddressOf()));
        Assert::AreEqual(S_OK, factory->GetOrCreate(canvasDevice2Abi.Get(), d2DImage.Get(), 0, resultInspectable.ReleaseAndGetAddressOf()));

        // Sanity check, we did get our wrapper back
        Assert::IsTrue(As<IUnknown>(proxyEffect1).Get() == As<IUnknown>(resultInspectable).Get());

        // Create an effect with no device and WIN2D_GET_DEVICE_ASSOCIATION_TYPE_REALIZATION_DEVICE
        ComPtr<ImageInteropProxy> proxyEffect2 = Make<ImageInteropProxy>(nullptr, WIN2D_GET_DEVICE_ASSOCIATION_TYPE::WIN2D_GET_DEVICE_ASSOCIATION_TYPE_REALIZATION_DEVICE);
        Platform::Object^ proxyEffect2Object = reinterpret_cast<Platform::Object^>(As<ABI::Windows::Graphics::Effects::IGraphicsEffectSource>(proxyEffect2.Get()).Get());

        // Update the cache
        Assert::IsTrue(UnregisterWrapper(d2DImage.Get()));
        Assert::IsTrue(RegisterWrapper(d2DImage.Get(), proxyEffect2Object));

        // Same as before, we can always get the effect back
        Assert::AreEqual(S_OK, factory->GetOrCreate(nullptr, d2DImage.Get(), 0, resultInspectable.ReleaseAndGetAddressOf()));
        Assert::AreEqual(S_OK, factory->GetOrCreate(canvasDevice1Abi.Get(), d2DImage.Get(), 0, resultInspectable.ReleaseAndGetAddressOf()));
        Assert::AreEqual(S_OK, factory->GetOrCreate(canvasDevice2Abi.Get(), d2DImage.Get(), 0, resultInspectable.ReleaseAndGetAddressOf()));

        // Sanity check, we did get our wrapper back
        Assert::IsTrue(As<IUnknown>(proxyEffect2).Get() == As<IUnknown>(resultInspectable).Get());

        // Create an effect with an existing device and WIN2D_GET_DEVICE_ASSOCIATION_TYPE_REALIZATION_DEVICE
        ComPtr<ImageInteropProxy> proxyEffect3 = Make<ImageInteropProxy>(canvasDevice1Abi.Get(), WIN2D_GET_DEVICE_ASSOCIATION_TYPE::WIN2D_GET_DEVICE_ASSOCIATION_TYPE_REALIZATION_DEVICE);
        Platform::Object^ proxyEffect3Object = reinterpret_cast<Platform::Object^>(As<ABI::Windows::Graphics::Effects::IGraphicsEffectSource>(proxyEffect3.Get()).Get());

        // Update the cache
        Assert::IsTrue(UnregisterWrapper(d2DImage.Get()));
        Assert::IsTrue(RegisterWrapper(d2DImage.Get(), proxyEffect3Object));

        // Once again, we can always get the effect back
        Assert::AreEqual(S_OK, factory->GetOrCreate(nullptr, d2DImage.Get(), 0, resultInspectable.ReleaseAndGetAddressOf()));
        Assert::AreEqual(S_OK, factory->GetOrCreate(canvasDevice1Abi.Get(), d2DImage.Get(), 0, resultInspectable.ReleaseAndGetAddressOf()));
        Assert::AreEqual(S_OK, factory->GetOrCreate(canvasDevice2Abi.Get(), d2DImage.Get(), 0, resultInspectable.ReleaseAndGetAddressOf()));

        // Sanity check, same as before, we should get the latest wrapper we added
        Assert::IsTrue(As<IUnknown>(proxyEffect3).Get() == As<IUnknown>(resultInspectable).Get());

        // Create an effect with an existing device and WIN2D_GET_DEVICE_ASSOCIATION_TYPE_CREATION_DEVICE
        ComPtr<ImageInteropProxy> proxyEffect4 = Make<ImageInteropProxy>(canvasDevice1Abi.Get(), WIN2D_GET_DEVICE_ASSOCIATION_TYPE::WIN2D_GET_DEVICE_ASSOCIATION_TYPE_CREATION_DEVICE);
        Platform::Object^ proxyEffect4Object = reinterpret_cast<Platform::Object^>(As<ABI::Windows::Graphics::Effects::IGraphicsEffectSource>(proxyEffect4.Get()).Get());

        // Update the cache
        Assert::IsTrue(UnregisterWrapper(d2DImage.Get()));
        Assert::IsTrue(RegisterWrapper(d2DImage.Get(), proxyEffect4Object));

        // We can only get the effect if we pass either null, or the correct device
        Assert::AreEqual(S_OK, factory->GetOrCreate(nullptr, d2DImage.Get(), 0, resultInspectable.ReleaseAndGetAddressOf()));
        Assert::AreEqual(S_OK, factory->GetOrCreate(canvasDevice1Abi.Get(), d2DImage.Get(), 0, resultInspectable.ReleaseAndGetAddressOf()));

        // Sanity check, while we could get the wrapper back
        Assert::IsTrue(As<IUnknown>(proxyEffect4).Get() == As<IUnknown>(resultInspectable).Get());

        // When passing the wrong device, this should fail
        Assert::AreEqual(E_INVALIDARG, factory->GetOrCreate(canvasDevice2Abi.Get(), d2DImage.Get(), 0, resultInspectable.ReleaseAndGetAddressOf()));

        // The resulting wrapper should be null here
        Assert::IsNull(resultInspectable.Get());

        // Restore the original wrapper in the cache to cleanup
        Assert::IsTrue(UnregisterWrapper(d2DImage.Get()));
        Assert::IsTrue(RegisterWrapper(d2DImage.Get(), colorEffect));
    }

    TEST_METHOD(CanvasFactoryNative_RegisterUnregisterEffectFactory_CheckNullOrInvalidInputs)
    {
        // Create the dummy factory
        ComPtr<CanvasImageFactory> imageFactory = Make<CanvasImageFactory>();

        // Get the ICanvasFactoryNative activation factory to test the ABI methods
        ComPtr<ABI::Microsoft::Graphics::Canvas::ICanvasFactoryNative> factory;
        ThrowIfFailed(Windows::Foundation::GetActivationFactory(
            reinterpret_cast<HSTRING>(CanvasDevice::typeid->FullName),
            &factory));

        // The empty GUID isn't allowed
        Assert::AreEqual(E_INVALIDARG, factory->RegisterEffectFactory(GUID_NULL, imageFactory.Get()));

        // Test some D2D effects (wrapped by Win2D) to validate they're not allowed
        Assert::AreEqual(E_INVALIDARG, factory->RegisterEffectFactory(CLSID_D2D1ArithmeticComposite, imageFactory.Get()));
        Assert::AreEqual(E_INVALIDARG, factory->RegisterEffectFactory(CLSID_D2D1Crop, imageFactory.Get()));
        Assert::AreEqual(E_INVALIDARG, factory->RegisterEffectFactory(CLSID_D2D12DAffineTransform, imageFactory.Get()));
        Assert::AreEqual(E_INVALIDARG, factory->RegisterEffectFactory(CLSID_D2D1Sharpen, imageFactory.Get()));

        GUID pixelShaderEffectId = { 0x8db3047a, 0x84cc, 0x4152, 0xaf, 0x92, 0x50, 0xe4, 0xac, 0xb9, 0xd1, 0xfc };

        // Win2D's PixelShaderEffect isn't allowed either (this is defined in PixelShaderEffectImpl.h, but it isn't public)
        Assert::AreEqual(E_INVALIDARG, factory->RegisterEffectFactory(pixelShaderEffectId, imageFactory.Get()));

        // Same as above, but unregistering factories as well
        Assert::AreEqual(E_INVALIDARG, factory->UnregisterEffectFactory(GUID_NULL));
        Assert::AreEqual(E_INVALIDARG, factory->UnregisterEffectFactory(CLSID_D2D1ArithmeticComposite));
        Assert::AreEqual(E_INVALIDARG, factory->UnregisterEffectFactory(CLSID_D2D1Crop));
        Assert::AreEqual(E_INVALIDARG, factory->UnregisterEffectFactory(CLSID_D2D12DAffineTransform));
        Assert::AreEqual(E_INVALIDARG, factory->UnregisterEffectFactory(CLSID_D2D1Sharpen));
        Assert::AreEqual(E_INVALIDARG, factory->UnregisterEffectFactory(pixelShaderEffectId));

        GUID randomGuid;
        ThrowIfFailed(CoCreateGuid(&randomGuid));

        // Also check the factory isn't null
        Assert::AreEqual(E_INVALIDARG, factory->RegisterEffectFactory(randomGuid, nullptr));

        // Also test the C++/CX helper
        Assert::ExpectException<Platform::InvalidArgumentException^>([&] { RegisterEffectFactory(CLSID_D2D1ArithmeticComposite, imageFactory.Get()); });
        Assert::ExpectException<Platform::InvalidArgumentException^>([&] { RegisterEffectFactory(pixelShaderEffectId, imageFactory.Get()); });
        Assert::ExpectException<Platform::InvalidArgumentException^>([&] { RegisterEffectFactory(CLSID_D2D1ArithmeticComposite, imageFactory.Get()); });
        Assert::ExpectException<Platform::InvalidArgumentException^>([&] { UnregisterEffectFactory(GUID_NULL); });
        Assert::ExpectException<Platform::InvalidArgumentException^>([&] { UnregisterEffectFactory(CLSID_D2D1ArithmeticComposite); });
        Assert::ExpectException<Platform::InvalidArgumentException^>([&] { UnregisterEffectFactory(pixelShaderEffectId); });
        Assert::ExpectException<Platform::InvalidArgumentException^>([&] { RegisterEffectFactory(randomGuid, nullptr); });
    }

    TEST_METHOD(CanvasFactoryNative_RegisterUnregisterEffectFactory_WorksAsExpected)
    {
        // Create the dummy factory
        ComPtr<CanvasImageFactory> imageFactory = Make<CanvasImageFactory>();

        // Get the ICanvasFactoryNative activation factory to test the ABI methods
        ComPtr<ABI::Microsoft::Graphics::Canvas::ICanvasFactoryNative> factory;
        ThrowIfFailed(Windows::Foundation::GetActivationFactory(
            reinterpret_cast<HSTRING>(CanvasDevice::typeid->FullName),
            &factory));

        GUID randomGuid;
        ThrowIfFailed(CoCreateGuid(&randomGuid));

        // Unregistering a GUID never seen before will return false
        Assert::AreEqual(S_FALSE, factory->UnregisterEffectFactory(randomGuid));

        // Registering a factory the first time should succeed
        Assert::AreEqual(S_OK, factory->RegisterEffectFactory(randomGuid, imageFactory.Get()));

        // The second time it should return false
        Assert::AreEqual(S_FALSE, factory->RegisterEffectFactory(randomGuid, imageFactory.Get()));

        // We can now correctly unregister it
        Assert::AreEqual(S_OK, factory->UnregisterEffectFactory(randomGuid));

        // Sanity check: we can't unregister it again
        Assert::AreEqual(S_FALSE, factory->UnregisterEffectFactory(randomGuid));

        // And for good measure, we can add it back again and remove it once more
        Assert::AreEqual(S_OK, factory->RegisterEffectFactory(randomGuid, imageFactory.Get()));
        Assert::AreEqual(S_FALSE, factory->RegisterEffectFactory(randomGuid, imageFactory.Get()));
        Assert::AreEqual(S_OK, factory->UnregisterEffectFactory(randomGuid));
        Assert::AreEqual(S_FALSE, factory->UnregisterEffectFactory(randomGuid));
    }

    TEST_METHOD(CanvasFactoryNative_RegisterUnregisterEffectFactory_WorksAsExpected_WithCppCxHelpers)
    {
        // Create the dummy factory
        ComPtr<CanvasImageFactory> imageFactory = Make<CanvasImageFactory>();

        GUID randomGuid;
        ThrowIfFailed(CoCreateGuid(&randomGuid));

        // Unregistering a GUID never seen before will return false
        Assert::IsFalse(UnregisterEffectFactory(randomGuid));

        // Registering a factory the first time should succeed
        Assert::IsTrue(RegisterEffectFactory(randomGuid, imageFactory.Get()));

        // The second time it should return false
        Assert::IsFalse(RegisterEffectFactory(randomGuid, imageFactory.Get()));

        // We can now correctly unregister it
        Assert::IsTrue(UnregisterEffectFactory(randomGuid));

        // Sanity check: we can't unregister it again
        Assert::IsFalse(UnregisterEffectFactory(randomGuid));

        // And for good measure, we can add it back again and remove it once more
        Assert::IsTrue(RegisterEffectFactory(randomGuid, imageFactory.Get()));
        Assert::IsFalse(RegisterEffectFactory(randomGuid, imageFactory.Get()));
        Assert::IsTrue(UnregisterEffectFactory(randomGuid));
        Assert::IsFalse(UnregisterEffectFactory(randomGuid));
    }

    TEST_METHOD(CanvasFactoryNative_RegisterUnregisterEffectFactory_InvalidFactoryIsDetected)
    {
        // Get a canvas device
        auto canvasDevice = ref new CanvasDevice();
        auto canvasDeviceAbi = As<ABI::Microsoft::Graphics::Canvas::ICanvasDevice>(canvasDevice);

        // Create the dummy factory
        ComPtr<CanvasImageFactory> imageFactory = Make<CanvasImageFactory>();

        GUID randomGuid;
        ThrowIfFailed(CoCreateGuid(&randomGuid));

        // Also create a dummy effect
        ComPtr<MockD2DEffect> mockEffect = Make<MockD2DEffect>(randomGuid);

        // Get the ICanvasFactoryNative activation factory to test the ABI methods
        ComPtr<ABI::Microsoft::Graphics::Canvas::ICanvasFactoryNative> factory;
        ThrowIfFailed(Windows::Foundation::GetActivationFactory(
            reinterpret_cast<HSTRING>(CanvasDevice::typeid->FullName),
            &factory));

        // Resulting wrapper to check whenever needed
        ComPtr<IInspectable> wrapper;

        // Register the factory
        Assert::AreEqual(S_OK, factory->RegisterEffectFactory(randomGuid, imageFactory.Get()));

        // A factory returning a null wrapper is not valid, and Win2D will throw E_UNEXPECTED
        imageFactory->SetWrapper(nullptr);
        Assert::AreEqual(E_UNEXPECTED, factory->GetOrCreate(canvasDeviceAbi.Get(), As<IUnknown>(mockEffect).Get(), 0, wrapper.ReleaseAndGetAddressOf()));

        // A factory returning an object that's not an ICanvasImage is also invalid
        imageFactory->SetWrapper(canvasDeviceAbi.Get());
        Assert::AreEqual(E_NOINTERFACE, factory->GetOrCreate(canvasDeviceAbi.Get(), As<IUnknown>(mockEffect).Get(), 0, wrapper.ReleaseAndGetAddressOf()));
    }

    TEST_METHOD(CanvasFactoryNative_RegisterUnregisterEffectFactory_ReturnsCustomWrapper)
    {
        // Get a canvas device
        auto canvasDevice = ref new CanvasDevice();
        auto canvasDeviceAbi = As<ABI::Microsoft::Graphics::Canvas::ICanvasDevice>(canvasDevice);

        // Create the dummy factory and an effect, just like in the test above
        ComPtr<CanvasImageFactory> imageFactory = Make<CanvasImageFactory>();

        GUID randomGuid;
        ThrowIfFailed(CoCreateGuid(&randomGuid));

        ComPtr<MockD2DEffect> mockEffect = Make<MockD2DEffect>(randomGuid);

        // Get the ICanvasFactoryNative activation factory to test the ABI methods
        ComPtr<ABI::Microsoft::Graphics::Canvas::ICanvasFactoryNative> factory;
        ThrowIfFailed(Windows::Foundation::GetActivationFactory(
            reinterpret_cast<HSTRING>(CanvasDevice::typeid->FullName),
            &factory));

        // Create dummy canvas image
        auto dummyCanvasImage = Make<DummyCanvasImage>();

        // Set the factory to return this dummy canvas image
        imageFactory->SetWrapper(As<IInspectable>(dummyCanvasImage).Get());

        // Register the factory
        Assert::AreEqual(S_OK, factory->RegisterEffectFactory(randomGuid, imageFactory.Get()));

        // Get a wrapper for the effect, triggering the factory to be invoked
        ComPtr<IInspectable> wrapper;
        Assert::AreEqual(S_OK, factory->GetOrCreate(canvasDeviceAbi.Get(), As<IUnknown>(mockEffect).Get(), 0, &wrapper));

        // The returned object should be our dummy image
        Assert::IsTrue(As<IUnknown>(dummyCanvasImage).Get() == As<IUnknown>(wrapper).Get());
    }
};
