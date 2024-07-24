// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

using namespace Microsoft::Graphics::Canvas;
using namespace Microsoft::Graphics::Canvas::Effects;
using namespace Platform;
using namespace Microsoft::UI;

TEST_CLASS(EffectTransferTable3DTests)
{
    TEST_METHOD(EffectTransferTable3D_NativeInterop)
    {
        auto canvasDevice = ref new CanvasDevice();
        auto transferTable = EffectTransferTable3D::CreateFromColors(canvasDevice, ref new Array<Windows::UI::Color>(8), 2, 2, 2);

        auto d2dLookupTable = GetWrappedResource<ID2D1LookupTable3D>(transferTable);

        auto newTransferTable = GetOrCreate<EffectTransferTable3D>(canvasDevice, d2dLookupTable.Get());
        auto newD2dLookupTable = GetWrappedResource<ID2D1LookupTable3D>(newTransferTable);

        Assert::AreEqual(transferTable, newTransferTable);
        Assert::AreEqual<void*>(d2dLookupTable.Get(), newD2dLookupTable.Get());

        auto getResourceWithExplicitDevice = GetWrappedResource<ID2D1LookupTable3D>(canvasDevice, transferTable);
        Assert::AreEqual<void*>(d2dLookupTable.Get(), getResourceWithExplicitDevice.Get());

        ExpectCOMException(E_INVALIDARG, L"Existing resource wrapper is associated with a different device.", [&]
        {
            GetWrappedResource<ID2D1LookupTable3D>(ref new CanvasDevice(), transferTable);
        });

        newTransferTable = GetOrCreate<EffectTransferTable3D>(d2dLookupTable.Get());

        delete transferTable;
        delete newTransferTable;

        ExpectCOMException(E_INVALIDARG, L"To wrap this resource type, a device parameter must be passed to GetOrCreate.", [&]
        {
            transferTable = GetOrCreate<EffectTransferTable3D>(d2dLookupTable.Get());
        });

        transferTable = GetOrCreate<EffectTransferTable3D>(canvasDevice, d2dLookupTable.Get());
        Assert::AreEqual(canvasDevice, transferTable->Device);
    }


    TEST_METHOD(EffectTransferTable3D_Closed)
    {
        auto canvasDevice = ref new CanvasDevice();
        auto transferTable = EffectTransferTable3D::CreateFromColors(canvasDevice, ref new Array<Windows::UI::Color>(8), 2, 2, 2);

        delete transferTable;

        ExpectCOMException(RO_E_CLOSED, [&]
        {
            auto device = transferTable->Device;
        });

        ExpectCOMException(RO_E_CLOSED, [&]
        {
            GetWrappedResource<ID2D1LookupTable3D>(transferTable);
        });
    }


    TEST_METHOD(EffectTransferTable3D_DrawWrongDevice)
    {
        auto canvasDevice = ref new CanvasDevice();
        auto transferTable = EffectTransferTable3D::CreateFromColors(canvasDevice, ref new Array<Windows::UI::Color>(8), 2, 2, 2);

        auto effect = ref new TableTransfer3DEffect();
        effect->Source = ref new ColorSourceEffect();
        effect->Table = transferTable;

        // Works.
        auto commandList = ref new CanvasCommandList(canvasDevice);
        auto drawingSession = commandList->CreateDrawingSession();
        
        drawingSession->DrawImage(effect);

        // Fails.
        auto otherDevice = ref new CanvasDevice();
        auto otherCommandList = ref new CanvasCommandList(otherDevice);
        auto otherDrawingSession = otherCommandList->CreateDrawingSession();

        ExpectCOMException(E_INVALIDARG, L"Existing resource wrapper is associated with a different device.", [&]
        {
            otherDrawingSession->DrawImage(effect);
        });
    }
};
