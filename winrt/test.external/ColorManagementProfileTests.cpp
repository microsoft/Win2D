// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

using namespace Microsoft::Graphics::Canvas::Effects;
using namespace Platform;


// Arbitrary but valid ICC profile data for testing.
static uint8_t someIccProfile[] =
{
    0, 0, 3, 196, 0, 0, 0, 0, 4, 48, 0, 0, 109, 110, 116, 114, 82, 71, 66, 32, 88, 89, 90, 32, 7, 218, 0, 9, 0, 1, 0, 10,
    0, 53, 0, 12, 97, 99, 115, 112, 77, 83, 70, 84, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 246, 214, 0, 1, 0, 0, 0, 0, 211, 45, 77, 83, 70, 84, 176, 212, 199, 133, 174, 126, 32, 208, 198, 67, 156, 18,
    161, 8, 116, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 18, 119, 116, 112, 116, 0, 0, 1, 92, 0, 0, 0, 20, 114, 88, 89, 90, 0, 0, 1, 112, 0, 0, 0, 20, 103, 88, 89, 90,
    0, 0, 1, 132, 0, 0, 0, 20, 98, 88, 89, 90, 0, 0, 1, 152, 0, 0, 0, 20, 114, 84, 82, 67, 0, 0, 1, 172, 0, 0, 0, 16,
    103, 84, 82, 67, 0, 0, 1, 172, 0, 0, 0, 16, 98, 84, 82, 67, 0, 0, 1, 172, 0, 0, 0, 16, 68, 50, 66, 48, 0, 0, 1, 188,
    0, 0, 0, 84, 68, 50, 66, 49, 0, 0, 1, 188, 0, 0, 0, 84, 68, 50, 66, 50, 0, 0, 1, 188, 0, 0, 0, 84, 68, 50, 66, 51,
    0, 0, 2, 16, 0, 0, 0, 84, 66, 50, 68, 48, 0, 0, 2, 100, 0, 0, 0, 84, 66, 50, 68, 49, 0, 0, 2, 100, 0, 0, 0, 84,
    66, 50, 68, 50, 0, 0, 2, 100, 0, 0, 0, 84, 66, 50, 68, 51, 0, 0, 2, 184, 0, 0, 0, 84, 99, 104, 97, 100, 0, 0, 3, 12,
    0, 0, 0, 44, 99, 112, 114, 116, 0, 0, 3, 56, 0, 0, 0, 100, 100, 101, 115, 99, 0, 0, 3, 156, 0, 0, 0, 40, 88, 89, 90, 32,
    0, 0, 0, 0, 0, 0, 243, 81, 0, 1, 0, 0, 0, 1, 22, 204, 88, 89, 90, 32, 0, 0, 0, 0, 0, 0, 111, 162, 0, 0, 56, 245,
    0, 0, 3, 144, 88, 89, 90, 32, 0, 0, 0, 0, 0, 0, 98, 153, 0, 0, 183, 133, 0, 0, 24, 218, 88, 89, 90, 32, 0, 0, 0, 0,
    0, 0, 36, 160, 0, 0, 15, 132, 0, 0, 182, 207, 99, 117, 114, 118, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 255, 255, 109, 112, 101, 116,
    0, 0, 0, 0, 0, 3, 0, 3, 0, 0, 0, 1, 0, 0, 0, 24, 0, 0, 0, 60, 109, 97, 116, 102, 0, 0, 0, 0, 0, 3, 0, 3,
    62, 223, 69, 47, 62, 197, 39, 58, 62, 18, 131, 171, 62, 99, 216, 56, 63, 55, 133, 91, 61, 120, 73, 109, 60, 100, 67, 226, 61, 198, 222, 185,
    63, 54, 212, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 109, 112, 101, 116, 0, 0, 0, 0, 0, 3, 0, 3, 0, 0, 0, 1,
    0, 0, 0, 24, 0, 0, 0, 60, 109, 97, 116, 102, 0, 0, 0, 0, 0, 3, 0, 3, 62, 211, 45, 124, 62, 183, 20, 55, 62, 56, 196, 156,
    62, 89, 198, 237, 63, 55, 20, 55, 61, 147, 208, 125, 60, 158, 98, 33, 61, 244, 26, 239, 63, 115, 71, 33, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 109, 112, 101, 116, 0, 0, 0, 0, 0, 3, 0, 3, 0, 0, 0, 1, 0, 0, 0, 24, 0, 0, 0, 60, 109, 97, 116, 102,
    0, 0, 0, 0, 0, 3, 0, 3, 64, 72, 145, 25, 191, 206, 245, 125, 190, 251, 49, 214, 191, 122, 144, 145, 63, 245, 68, 32, 61, 9, 7, 16,
    61, 147, 88, 15, 190, 106, 124, 185, 63, 179, 222, 254, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 109, 112, 101, 116, 0, 0, 0, 0,
    0, 3, 0, 3, 0, 0, 0, 1, 0, 0, 0, 24, 0, 0, 0, 60, 109, 97, 116, 102, 0, 0, 0, 0, 0, 3, 0, 3, 64, 79, 99, 154,
    191, 196, 192, 244, 190, 255, 63, 130, 191, 120, 33, 209, 63, 240, 33, 31, 61, 42, 54, 160, 61, 99, 234, 85, 190, 80, 236, 42, 63, 135, 83, 40,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 115, 102, 51, 50, 0, 0, 0, 0, 0, 1, 12, 61, 0, 0, 5, 220, 255, 255, 243, 44,
    0, 0, 7, 144, 0, 0, 253, 144, 255, 255, 251, 164, 255, 255, 253, 164, 0, 0, 3, 218, 0, 0, 192, 140, 109, 108, 117, 99, 0, 0, 0, 0,
    0, 0, 0, 1, 0, 0, 0, 12, 101, 110, 85, 83, 0, 0, 0, 72, 0, 0, 0, 28, 0, 67, 0, 111, 0, 112, 0, 121, 0, 114, 0, 105,
    0, 103, 0, 104, 0, 116, 0, 32, 0, 40, 0, 99, 0, 41, 0, 32, 0, 77, 0, 105, 0, 99, 0, 114, 0, 111, 0, 115, 0, 111, 0, 102,
    0, 116, 0, 32, 0, 67, 0, 111, 0, 114, 0, 112, 0, 111, 0, 114, 0, 97, 0, 116, 0, 105, 0, 111, 0, 110, 0, 0, 109, 108, 117, 99,
    0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 12, 101, 110, 85, 83, 0, 0, 0, 12, 0, 0, 0, 28, 0, 115, 0, 99, 0, 82, 0, 71, 0, 66, 0, 0
};


#if WINVER > _WIN32_WINNT_WINBLUE

static ColorManagementSimpleProfile someSimpleProfile =
{
    { 1, 2 },
    { 3, 4 },
    { 5, 6 },
    { 7, 8 },
    ColorManagementGamma::G2084
};

#endif


TEST_CLASS(ColorManagementProfileTests)
{
    TEST_METHOD(ColorManagementProfile_Constructor)
    {
        auto srgb = ref new ColorManagementProfile(CanvasColorSpace::Srgb);
        auto scrgb = ref new ColorManagementProfile(CanvasColorSpace::ScRgb);

        Assert::AreEqual(CanvasColorSpace::Srgb, srgb->ColorSpace);
        Assert::AreEqual(CanvasColorSpace::ScRgb, scrgb->ColorSpace);

        Assert::AreEqual(0u, srgb->IccProfile->Length);
        Assert::AreEqual(0u, scrgb->IccProfile->Length);

#if WINVER > _WIN32_WINNT_WINBLUE
        Assert::AreEqual(ColorManagementProfileType::Icc, srgb->Type);
        Assert::AreEqual(ColorManagementProfileType::Icc, scrgb->Type);

        Assert::IsNull(srgb->SimpleProfile);
        Assert::IsNull(scrgb->SimpleProfile);

        Assert::AreEqual(ExtendedColorSpace::FullG22P709, srgb->ExtendedColorSpace);
        Assert::AreEqual(ExtendedColorSpace::FullG10P709, scrgb->ExtendedColorSpace);
#endif

        ExpectCOMException(E_INVALIDARG, []
        {
            ref new ColorManagementProfile(CanvasColorSpace::Custom);
        });
    }


    TEST_METHOD(ColorManagementProfile_CreateCustom)
    {
        auto custom = ColorManagementProfile::CreateCustom(ref new Array<uint8_t>(someIccProfile, sizeof(someIccProfile)));

        Assert::AreEqual(CanvasColorSpace::Custom, custom->ColorSpace);

        AssertExpectedIccProfile(custom->IccProfile);

#if WINVER > _WIN32_WINNT_WINBLUE
        Assert::AreEqual(ColorManagementProfileType::Icc, custom->Type);
        Assert::IsNull(custom->SimpleProfile);
        Assert::AreEqual(ExtendedColorSpace::Custom, custom->ExtendedColorSpace);
#endif

        ExpectCOMException(E_INVALIDARG, []
        {
            ColorManagementProfile::CreateCustom(nullptr);
        });
    }


#if WINVER > _WIN32_WINNT_WINBLUE


    TEST_METHOD(ColorManagementProfile_CreateSimple)
    {
        auto simple = ColorManagementProfile::CreateSimple(someSimpleProfile);

        Assert::AreEqual(ColorManagementProfileType::Simple, simple->Type);

        AssertExpectedSimpleProfile(simple->SimpleProfile->Value);

        Assert::AreEqual(CanvasColorSpace::Custom, simple->ColorSpace);
        Assert::AreEqual(ExtendedColorSpace::Custom, simple->ExtendedColorSpace);
        Assert::AreEqual(0u, simple->IccProfile->Length);
    }


    TEST_METHOD(ColorManagementProfile_CreateExtended)
    {
        auto extendedSrgb = ColorManagementProfile::CreateExtended(ExtendedColorSpace::FullG22P709);
        auto extendedScrgb = ColorManagementProfile::CreateExtended(ExtendedColorSpace::FullG10P709);
        auto extendedStudio = ColorManagementProfile::CreateExtended(ExtendedColorSpace::StudioG22P709);

        Assert::AreEqual(ColorManagementProfileType::Extended, extendedSrgb->Type);
        Assert::AreEqual(ColorManagementProfileType::Extended, extendedScrgb->Type);
        Assert::AreEqual(ColorManagementProfileType::Extended, extendedStudio->Type);

        Assert::AreEqual(ExtendedColorSpace::FullG22P709, extendedSrgb->ExtendedColorSpace);
        Assert::AreEqual(ExtendedColorSpace::FullG10P709, extendedScrgb->ExtendedColorSpace);
        Assert::AreEqual(ExtendedColorSpace::StudioG22P709, extendedStudio->ExtendedColorSpace);

        Assert::AreEqual(CanvasColorSpace::Srgb, extendedSrgb->ColorSpace);
        Assert::AreEqual(CanvasColorSpace::ScRgb, extendedScrgb->ColorSpace);
        Assert::AreEqual(CanvasColorSpace::Custom, extendedStudio->ColorSpace);

        Assert::AreEqual(0u, extendedSrgb->IccProfile->Length);
        Assert::AreEqual(0u, extendedScrgb->IccProfile->Length);
        Assert::AreEqual(0u, extendedStudio->IccProfile->Length);

        Assert::IsNull(extendedSrgb->SimpleProfile);
        Assert::IsNull(extendedScrgb->SimpleProfile);
        Assert::IsNull(extendedStudio->SimpleProfile);

        ExpectCOMException(E_INVALIDARG, []
        {
            ColorManagementProfile::CreateExtended(ExtendedColorSpace::Custom);
        });
    }


    TEST_METHOD(ColorManagementProfile_IsSupported)
    {
        auto device = ref new CanvasDevice();

        // ICC profiles are always supported.
        Assert::IsTrue(ColorManagementProfile::IsSupported(ColorManagementProfileType::Icc, device));

        // Invalid profile types are never supported.
        Assert::IsFalse(ColorManagementProfile::IsSupported((ColorManagementProfileType)123, device));

        // Simple and Extended types depend on the OS version.
        bool isSimpleSupported = ColorManagementProfile::IsSupported(ColorManagementProfileType::Simple, device);
        bool isExtendedSupported = ColorManagementProfile::IsSupported(ColorManagementProfileType::Extended, device);

        Assert::AreEqual(isSimpleSupported, isExtendedSupported);

        if (!isSimpleSupported)
        {
            auto simple = ColorManagementProfile::CreateSimple(someSimpleProfile);

            ExpectCOMException(E_NOTIMPL, L"This type of ColorManagementProfile is not supported on this version of Windows. Use ColorManagementProfile.IsSupported to determine which types are available.", [&]
            {
                GetWrappedResource<ID2D1ColorContext>(device, simple);
            });
        }
    }


#endif  // WINVER > _WIN32_WINNT_WINBLUE


    TEST_METHOD(ColorManagementProfile_CreateViaInterop)
    {
        auto device = ref new CanvasDevice();
        auto commandList = ref new CanvasCommandList(device);
        auto drawingSession = commandList->CreateDrawingSession();
        auto d2dContext = GetWrappedResource<ID2D1DeviceContext>(drawingSession);

        ComPtr<ID2D1ColorContext> d2dSrgb, d2dScrgb, d2dCustom;
        
        ThrowIfFailed(d2dContext->CreateColorContext(D2D1_COLOR_SPACE_SRGB, nullptr, 0, &d2dSrgb));
        ThrowIfFailed(d2dContext->CreateColorContext(D2D1_COLOR_SPACE_SCRGB, nullptr, 0, &d2dScrgb));
        ThrowIfFailed(d2dContext->CreateColorContext(D2D1_COLOR_SPACE_CUSTOM, someIccProfile, sizeof(someIccProfile), &d2dCustom));

        auto srgb = GetOrCreate<ColorManagementProfile>(device, d2dSrgb.Get());
        auto scrgb = GetOrCreate<ColorManagementProfile>(device, d2dScrgb.Get());
        auto custom = GetOrCreate<ColorManagementProfile>(device, d2dCustom.Get());

        Assert::AreEqual(CanvasColorSpace::Srgb, srgb->ColorSpace);
        Assert::AreEqual(CanvasColorSpace::ScRgb, scrgb->ColorSpace);
        Assert::AreEqual(CanvasColorSpace::Custom, custom->ColorSpace);

        Assert::AreEqual(0u, srgb->IccProfile->Length);
        Assert::AreEqual(0u, scrgb->IccProfile->Length);

        AssertExpectedIccProfile(custom->IccProfile);

#if WINVER > _WIN32_WINNT_WINBLUE
        Assert::AreEqual(ColorManagementProfileType::Icc, srgb->Type);
        Assert::AreEqual(ColorManagementProfileType::Icc, scrgb->Type);
        Assert::AreEqual(ColorManagementProfileType::Icc, custom->Type);

        Assert::IsNull(srgb->SimpleProfile);
        Assert::IsNull(scrgb->SimpleProfile);
        Assert::IsNull(custom->SimpleProfile);

        Assert::AreEqual(ExtendedColorSpace::FullG22P709, srgb->ExtendedColorSpace);
        Assert::AreEqual(ExtendedColorSpace::FullG10P709, scrgb->ExtendedColorSpace);
        Assert::AreEqual(ExtendedColorSpace::Custom, custom->ExtendedColorSpace);

        if (ColorManagementProfile::IsSupported(ColorManagementProfileType::Extended, device))
        {
            ComPtr<ID2D1ColorContext1> d2dSimple, d2dExtendedSrgb, d2dExtendedScrgb, d2dExtendedStudio;

            auto d2dContext5 = As<ID2D1DeviceContext5>(d2dContext);

            ThrowIfFailed(d2dContext5->CreateColorContextFromSimpleColorProfile(reinterpret_cast<D2D1_SIMPLE_COLOR_PROFILE*>(&someSimpleProfile), &d2dSimple));
            ThrowIfFailed(d2dContext5->CreateColorContextFromDxgiColorSpace(DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709, &d2dExtendedSrgb));
            ThrowIfFailed(d2dContext5->CreateColorContextFromDxgiColorSpace(DXGI_COLOR_SPACE_RGB_FULL_G10_NONE_P709, &d2dExtendedScrgb));
            ThrowIfFailed(d2dContext5->CreateColorContextFromDxgiColorSpace(DXGI_COLOR_SPACE_RGB_STUDIO_G22_NONE_P709, &d2dExtendedStudio));

            auto simple = GetOrCreate<ColorManagementProfile>(device, d2dSimple.Get());
            auto extendedSrgb = GetOrCreate<ColorManagementProfile>(device, d2dExtendedSrgb.Get());
            auto extendedScrgb = GetOrCreate<ColorManagementProfile>(device, d2dExtendedScrgb.Get());
            auto extendedStudio = GetOrCreate<ColorManagementProfile>(device, d2dExtendedStudio.Get());

            Assert::AreEqual(ColorManagementProfileType::Simple, simple->Type);
            Assert::AreEqual(CanvasColorSpace::Custom, simple->ColorSpace);
            Assert::AreEqual(ExtendedColorSpace::Custom, simple->ExtendedColorSpace);
            Assert::AreEqual(0u, simple->IccProfile->Length);
            AssertExpectedSimpleProfile(simple->SimpleProfile->Value);

            Assert::AreEqual(ColorManagementProfileType::Extended, extendedSrgb->Type);
            Assert::AreEqual(CanvasColorSpace::Srgb, extendedSrgb->ColorSpace);
            Assert::AreEqual(ExtendedColorSpace::FullG22P709, extendedSrgb->ExtendedColorSpace);
            Assert::AreEqual(0u, extendedSrgb->IccProfile->Length);
            Assert::IsNull(extendedSrgb->SimpleProfile);

            Assert::AreEqual(ColorManagementProfileType::Extended, extendedScrgb->Type);
            Assert::AreEqual(CanvasColorSpace::ScRgb, extendedScrgb->ColorSpace);
            Assert::AreEqual(ExtendedColorSpace::FullG10P709, extendedScrgb->ExtendedColorSpace);
            Assert::AreEqual(0u, extendedScrgb->IccProfile->Length);
            Assert::IsNull(extendedScrgb->SimpleProfile);

            Assert::AreEqual(ColorManagementProfileType::Extended, extendedStudio->Type);
            Assert::AreEqual(CanvasColorSpace::Custom, extendedStudio->ColorSpace);
            Assert::AreEqual(ExtendedColorSpace::StudioG22P709, extendedStudio->ExtendedColorSpace);
            Assert::AreEqual(0u, extendedStudio->IccProfile->Length);
            Assert::IsNull(extendedStudio->SimpleProfile);
        }
#endif
    }


    TEST_METHOD(ColorManagementProfile_GetPropertiesWhileRealized)
    {
        auto device = ref new CanvasDevice();

        auto srgb = ref new ColorManagementProfile(CanvasColorSpace::Srgb);
        auto custom = ColorManagementProfile::CreateCustom(ref new Array<uint8_t>(someIccProfile, sizeof(someIccProfile)));

        GetWrappedResource<ID2D1ColorContext>(device, srgb);
        GetWrappedResource<ID2D1ColorContext>(device, custom);

        Assert::AreEqual(CanvasColorSpace::Srgb, srgb->ColorSpace);
        Assert::AreEqual(CanvasColorSpace::Custom, custom->ColorSpace);

        Assert::AreEqual(0u, srgb->IccProfile->Length);

        AssertExpectedIccProfile(custom->IccProfile);

#if WINVER > _WIN32_WINNT_WINBLUE
        Assert::AreEqual(ColorManagementProfileType::Icc, srgb->Type);
        Assert::AreEqual(ColorManagementProfileType::Icc, custom->Type);

        Assert::IsNull(srgb->SimpleProfile);
        Assert::IsNull(custom->SimpleProfile);

        Assert::AreEqual(ExtendedColorSpace::FullG22P709, srgb->ExtendedColorSpace);
        Assert::AreEqual(ExtendedColorSpace::Custom, custom->ExtendedColorSpace);

        if (ColorManagementProfile::IsSupported(ColorManagementProfileType::Extended, device))
        {
            auto simple = ColorManagementProfile::CreateSimple(someSimpleProfile);
            auto extended = ColorManagementProfile::CreateExtended(ExtendedColorSpace::StudioG2084P2020);

            GetWrappedResource<ID2D1ColorContext>(device, simple);
            GetWrappedResource<ID2D1ColorContext>(device, extended);

            Assert::AreEqual(CanvasColorSpace::Custom, simple->ColorSpace);
            Assert::AreEqual(CanvasColorSpace::Custom, extended->ColorSpace);

            Assert::AreEqual(0u, simple->IccProfile->Length);
            Assert::AreEqual(0u, extended->IccProfile->Length);

            Assert::AreEqual(ColorManagementProfileType::Simple, simple->Type);
            Assert::AreEqual(ColorManagementProfileType::Extended, extended->Type);

            AssertExpectedSimpleProfile(simple->SimpleProfile->Value);
            Assert::IsNull(extended->SimpleProfile);

            Assert::AreEqual(ExtendedColorSpace::Custom, simple->ExtendedColorSpace);
            Assert::AreEqual(ExtendedColorSpace::StudioG2084P2020, extended->ExtendedColorSpace);
        }
#endif
    }


    TEST_METHOD(ColorManagementProfile_Closed)
    {
        auto profile = ref new ColorManagementProfile(CanvasColorSpace::Srgb);

        delete profile;

        Assert::ExpectException<Platform::ObjectDisposedException^>([&]
        {
            profile->ColorSpace;
        });

        Assert::ExpectException<Platform::ObjectDisposedException^>([&]
        {
            profile->IccProfile;
        });

#if WINVER > _WIN32_WINNT_WINBLUE
        Assert::ExpectException<Platform::ObjectDisposedException^>([&]
        {
            profile->Type;
        });

        Assert::ExpectException<Platform::ObjectDisposedException^>([&]
        {
            profile->SimpleProfile;
        });

        Assert::ExpectException<Platform::ObjectDisposedException^>([&]
        {
            profile->ExtendedColorSpace;
        });
#endif

        Assert::ExpectException<Platform::ObjectDisposedException^>([&]
        {
            GetWrappedResource<ID2D1ColorContext>(ref new CanvasDevice(), profile);
        });
    }


    TEST_METHOD(ColorManagementProfile_Interop)
    {
        auto device = ref new CanvasDevice();
        auto device2 = ref new CanvasDevice();

        auto srgb = ref new ColorManagementProfile(CanvasColorSpace::Srgb);
        auto custom = ColorManagementProfile::CreateCustom(ref new Array<uint8_t>(someIccProfile, sizeof(someIccProfile)));

        auto d2dSrgb = GetWrappedResource<ID2D1ColorContext>(device, srgb);
        auto d2dCustom = GetWrappedResource<ID2D1ColorContext>(device, custom);

        // Repeated realization on the same device returns the same D2D object.
        auto d2dSrgb2 = GetWrappedResource<ID2D1ColorContext>(device, srgb);
        auto d2dCustom2 = GetWrappedResource<ID2D1ColorContext>(device, custom);

        Assert::AreEqual<void*>(d2dSrgb.Get(), d2dSrgb2.Get());
        Assert::AreEqual<void*>(d2dCustom.Get(), d2dCustom2.Get());

        // Realizing onto a different device returns different objects.
        d2dSrgb2 = GetWrappedResource<ID2D1ColorContext>(device2, srgb);
        d2dCustom2 = GetWrappedResource<ID2D1ColorContext>(device2, custom);

        Assert::AreNotEqual<void*>(d2dSrgb.Get(), d2dSrgb2.Get());
        Assert::AreNotEqual<void*>(d2dCustom.Get(), d2dCustom2.Get());

        // Win2D wrapper state is preserved.
        auto srgb2 = GetOrCreate<ColorManagementProfile>(d2dSrgb2.Get());
        auto custom2 = GetOrCreate<ColorManagementProfile>(d2dCustom2.Get());

        Assert::AreEqual(srgb, srgb2);
        Assert::AreEqual(custom, custom2);

        Assert::AreEqual(CanvasColorSpace::Srgb, srgb->ColorSpace);
        Assert::AreEqual(CanvasColorSpace::Custom, custom->ColorSpace);

        Assert::AreEqual(0u, srgb->IccProfile->Length);

        AssertExpectedIccProfile(custom->IccProfile);

#if WINVER > _WIN32_WINNT_WINBLUE
        // Make sure realization does the right thing with the various ID2D1ColorContext1 variants.
        if (ColorManagementProfile::IsSupported(ColorManagementProfileType::Extended, device))
        {
            D2D1_SIMPLE_COLOR_PROFILE simpleProfile{};

            auto d2dSrgb1 = As<ID2D1ColorContext1>(d2dSrgb);
            Assert::AreEqual<int>(D2D1_COLOR_CONTEXT_TYPE_ICC, d2dSrgb1->GetColorContextType());
            Assert::AreEqual<int>(DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709, d2dSrgb1->GetDXGIColorSpace());
            Assert::AreEqual(E_INVALIDARG, d2dSrgb1->GetSimpleColorProfile(&simpleProfile));

            auto d2dCustom1 = As<ID2D1ColorContext1>(d2dCustom);
            Assert::AreEqual<int>(D2D1_COLOR_CONTEXT_TYPE_ICC, d2dCustom1->GetColorContextType());
            Assert::AreEqual<int>(DXGI_COLOR_SPACE_CUSTOM, d2dCustom1->GetDXGIColorSpace());
            Assert::AreEqual(E_INVALIDARG, d2dCustom1->GetSimpleColorProfile(&simpleProfile));

            auto simple = ColorManagementProfile::CreateSimple(someSimpleProfile);
            auto d2dSimple1 = GetWrappedResource<ID2D1ColorContext1>(device, simple);
            Assert::AreEqual<int>(D2D1_COLOR_CONTEXT_TYPE_SIMPLE, d2dSimple1->GetColorContextType());
            Assert::AreEqual<int>(DXGI_COLOR_SPACE_CUSTOM, d2dSimple1->GetDXGIColorSpace());
            Assert::AreEqual(S_OK, d2dSimple1->GetSimpleColorProfile(&simpleProfile));
            AssertExpectedSimpleProfile(*reinterpret_cast<ColorManagementSimpleProfile*>(&simpleProfile));

            auto extended = ColorManagementProfile::CreateExtended(ExtendedColorSpace::StudioG2084P2020);
            auto d2dExtended1 = GetWrappedResource<ID2D1ColorContext1>(device, extended);
            Assert::AreEqual<int>(D2D1_COLOR_CONTEXT_TYPE_DXGI, d2dExtended1->GetColorContextType());
            Assert::AreEqual<int>(DXGI_COLOR_SPACE_RGB_STUDIO_G2084_NONE_P2020, d2dExtended1->GetDXGIColorSpace());
            Assert::AreEqual(E_INVALIDARG, d2dExtended1->GetSimpleColorProfile(&simpleProfile));
        }
#endif

        // Cannot interop without a device.
        ExpectCOMException(E_INVALIDARG, L"To unwrap this resource type, a device parameter must be passed to GetWrappedResource.", [&]
        {
            GetWrappedResource<ID2D1ColorContext>(srgb);
        });

        ExpectCOMException(E_INVALIDARG, L"To wrap this resource type, a device parameter must be passed to GetOrCreate.", [&]
        {
            GetOrCreate<ColorManagementProfile>(d2dSrgb.Get());
        });

        ExpectCOMException(E_INVALIDARG, L"Existing resource wrapper is associated with a different device.", [&]
        {
            GetOrCreate<ColorManagementProfile>(device, d2dSrgb2.Get());
        });
    }


    static void AssertExpectedIccProfile(Array<uint8_t>^ data)
    {
        Assert::AreEqual<size_t>(sizeof(someIccProfile), data->Length);

        for (int i = 0; i < sizeof(someIccProfile); i++)
        {
            Assert::AreEqual(someIccProfile[i], data[i]);
        }
    }


#if WINVER > _WIN32_WINNT_WINBLUE

    static void AssertExpectedSimpleProfile(ColorManagementSimpleProfile const& profile)
    {
        Assert::AreEqual(someSimpleProfile.RedPrimary,   profile.RedPrimary);
        Assert::AreEqual(someSimpleProfile.GreenPrimary, profile.GreenPrimary);
        Assert::AreEqual(someSimpleProfile.BluePrimary,  profile.BluePrimary);
        Assert::AreEqual(someSimpleProfile.WhitePointXZ, profile.WhitePointXZ);
        Assert::AreEqual(someSimpleProfile.Gamma,        profile.Gamma);
    }

#endif
};
