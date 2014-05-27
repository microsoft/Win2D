// Copyright (c) Microsoft Corporation.  All rights reserved

#pragma once

namespace canvas
{
    using namespace Microsoft::WRL;

    //
    // This internal interface is exposed by the CanvasDevice runtime class and
    // allows for internal access to non-WinRT methods.
    //
    // Since this is a native C++ (ie not COM) object and these methods never
    // cross DLL boundaries these use exceptions for error handling and can
    // take/return C++ types.
    //
    [uuid(E2C64DD1-4126-4C7A-A5D2-D5E8B2C7235C)]
    class ICanvasDeviceInternal : public IUnknown
    {
    public:
        virtual ComPtr<ID2D1Device1> GetD2DDevice() = 0;
    };
}
