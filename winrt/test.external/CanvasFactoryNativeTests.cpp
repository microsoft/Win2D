// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"
#include <WeakReference.h>

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
        ABI::Windows::Graphics::Effects::IGraphicsEffectSource>
    {
        InspectableClass(L"EffectsAbi::IGraphicsEffectD2D1Interop::DummyWeakReferenceSource", TrustLevel::BaseTrust);

        ComPtr<ABI::Microsoft::Graphics::Canvas::ICanvasImage> m_canvasImage;

    public:
        DummyWeakReferenceSource()
        {
        }
    };

    // Dummy WinRT object not implementing IWeakReferenceSource
    class DummyNonWeakReferenceSource : public RuntimeClass<
        RuntimeClassFlags<WinRtClassicComMix | InhibitWeakReference>,
        ABI::Windows::Graphics::Effects::IGraphicsEffectSource>
    {
        InspectableClass(L"EffectsAbi::IGraphicsEffectD2D1Interop::DummyNonWeakReferenceSource", TrustLevel::BaseTrust);

        ComPtr<ABI::Microsoft::Graphics::Canvas::ICanvasImage> m_canvasImage;

    public:
        DummyNonWeakReferenceSource()
        {
        }
    };

    class ImageInteropProxy : public RuntimeClass<
        RuntimeClassFlags<WinRtClassicComMix>,
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

    TEST_METHOD(CanvasFactoryNative_AddRemove_ChecksNullInputs)
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
        Assert::AreEqual(E_INVALIDARG, factory->Add(nullptr, (IInspectable*)(void*)canvasBitmap));
        Assert::AreEqual(E_INVALIDARG, factory->Add(d2DImage.Get(), (IInspectable*)nullptr));
        Assert::AreEqual(E_INVALIDARG, factory->Add(nullptr, (IInspectable*)nullptr));
        Assert::AreEqual(E_INVALIDARG, factory->Remove(nullptr));

        // Ensure the C++/CX wrapper handles failures as expected
        Assert::ExpectException<Platform::InvalidArgumentException^>([&] { Add(nullptr, canvasBitmap); });
        Assert::ExpectException<Platform::InvalidArgumentException^>([&] { Add(d2DImage.Get(), (CanvasBitmap^)nullptr); });
        Assert::ExpectException<Platform::InvalidArgumentException^>([&] { Add(nullptr, (CanvasBitmap^)nullptr); });
        Assert::ExpectException<Platform::InvalidArgumentException^>([&] { Remove(nullptr); });
    }

    TEST_METHOD(CanvasFactoryNative_AddRemove_UpdateCacheCorrectly)
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
        Assert::IsFalse(Add(d2DImage.Get(), canvasBitmap));

        // The image can be removed, and it can be removed only once
        Assert::IsTrue(Remove(d2DImage.Get()));
        Assert::IsFalse(Remove(d2DImage.Get()));

        // After this, the image can now be added again, and only once
        Assert::IsTrue(Add(d2DImage.Get(), canvasBitmap));
        Assert::IsFalse(Add(d2DImage.Get(), canvasBitmap));

        // Using GetOrCreate will correctly get the original instance
        Platform::Object^ cachedResource1 = GetOrCreate<Platform::Object>(d2DImage.Get());

        Assert::IsTrue(As<IUnknown>(canvasBitmap).Get() == As<IUnknown>(cachedResource1).Get());

        ColorSourceEffect^ dummyEffect = ref new ColorSourceEffect();

        // Replace the tracked resource for the image with the dummy effect
        Assert::IsTrue(Remove(d2DImage.Get()));
        Assert::IsTrue(Add(d2DImage.Get(), dummyEffect));

        // Using GetOrCreate will now return the dummy effect
        Platform::Object^ cachedResource2 = GetOrCreate<Platform::Object>(d2DImage.Get());

        Assert::IsTrue(As<IUnknown>(dummyEffect).Get() == As<IUnknown>(cachedResource2).Get());

        // Create a dummy type (we want to check arbitrary types are also accepted by the cache)
        ComPtr<DummyWeakReferenceSource> dummyObject = Make<DummyWeakReferenceSource>();

        // Sanity check: the object implements IWeakReferenceSource
        Assert::IsNotNull(As<IWeakReferenceSource>(dummyObject.Get()).Get());

        Platform::Object^ dummyObject2 = reinterpret_cast<Platform::Object^>(As<ABI::Windows::Graphics::Effects::IGraphicsEffectSource>(dummyObject.Get()).Get());

        // Add the dummy object to the resource cache
        Assert::IsTrue(Remove(d2DImage.Get()));
        Assert::IsTrue(Add(d2DImage.Get(), dummyObject2));

        // GetOrCreate will correctly return the dummy object now
        Platform::Object^ cachedResource3 = GetOrCreate<Platform::Object>(d2DImage.Get());

        Assert::IsTrue(As<IUnknown>(dummyObject).Get() == As<IUnknown>(cachedResource3).Get());

        // Create a dummy type without weak reference support
        ComPtr<DummyNonWeakReferenceSource> dummyObjectWithNoWeakRef = Make<DummyNonWeakReferenceSource>();

        // Sanity check: the object doesn't implement IWeakReferenceSource
        Assert::IsNull(MaybeAs<IWeakReferenceSource>(dummyObjectWithNoWeakRef.Get()).Get());

        Platform::Object^ dummyObject3 = reinterpret_cast<Platform::Object^>(As<ABI::Windows::Graphics::Effects::IGraphicsEffectSource>(dummyObjectWithNoWeakRef.Get()).Get());

        // Try to add the dummy object to the cache
        Assert::AreEqual(E_NOINTERFACE, factory->Add(d2DImage.Get(), (IInspectable*)(void*)dummyObject3));
        Assert::ExpectException<Platform::InvalidCastException^>([&] { Add(d2DImage.Get(), dummyObject3); });

        // GetOrCreate will return the previous object (since adding the new one failed)
        Platform::Object^ cachedResource4 = GetOrCreate<Platform::Object>(d2DImage.Get());

        Assert::IsTrue(As<IUnknown>(dummyObject).Get() == As<IUnknown>(cachedResource4).Get());

        // Restore the original CanvasBitmap in the resource cache
        Assert::IsTrue(Remove(d2DImage.Get()));
        Assert::IsTrue(Add(d2DImage.Get(), canvasBitmap));

        // Sanity check: GetOrCreate will return the original bitmap now
        Platform::Object^ cachedResource5 = GetOrCreate<Platform::Object>(d2DImage.Get());

        Assert::IsTrue(As<IUnknown>(canvasBitmap).Get() == As<IUnknown>(cachedResource5).Get());
    }

    TEST_METHOD(CanvasFactoryNative_AddRemove_GetOrAddPerformsValidationCorrectly)
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
        Assert::IsTrue(Remove(d2DImage.Get()));

        // Get the ICanvasFactoryNative activation factory to test the ABI methods
        ComPtr<ABI::Microsoft::Graphics::Canvas::ICanvasFactoryNative> factory;
        ThrowIfFailed(Windows::Foundation::GetActivationFactory(
            reinterpret_cast<HSTRING>(CanvasDevice::typeid->FullName),
            &factory));

        // Create an effect with no device and WIN2D_GET_DEVICE_ASSOCIATION_TYPE_UNSPECIFIED
        ComPtr<ImageInteropProxy> proxyEffect1 = Make<ImageInteropProxy>(nullptr, WIN2D_GET_DEVICE_ASSOCIATION_TYPE::WIN2D_GET_DEVICE_ASSOCIATION_TYPE_UNSPECIFIED);
        Platform::Object^ proxyEffect1Object = reinterpret_cast<Platform::Object^>(As<ABI::Windows::Graphics::Effects::IGraphicsEffectSource>(proxyEffect1.Get()).Get());

        // Add the proxy object to the cache
        Assert::IsTrue(Add(d2DImage.Get(), proxyEffect1Object));

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
        Assert::IsTrue(Remove(d2DImage.Get()));
        Assert::IsTrue(Add(d2DImage.Get(), proxyEffect2Object));

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
        Assert::IsTrue(Remove(d2DImage.Get()));
        Assert::IsTrue(Add(d2DImage.Get(), proxyEffect3Object));

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
        Assert::IsTrue(Remove(d2DImage.Get()));
        Assert::IsTrue(Add(d2DImage.Get(), proxyEffect4Object));

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
        Assert::IsTrue(Remove(d2DImage.Get()));
        Assert::IsTrue(Add(d2DImage.Get(), colorEffect));
    }
};
