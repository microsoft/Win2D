// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Printing
{
    class CanvasPrintTaskOptionsChangedEventArgs
        : public RuntimeClass<ICanvasPrintTaskOptionsChangedEventArgs>
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_Printing_CanvasPrintTaskOptionsChangedEventArgs, BaseTrust);
        
        uint32_t m_currentPreviewPageNumber;
        uint32_t m_newPreviewPageNumber;
        ComPtr<IPrintTaskOptionsCore> m_printTaskOptions;
        
    public:
        CanvasPrintTaskOptionsChangedEventArgs(
            uint32_t currentPreviewPageNumber,
            ComPtr<IPrintTaskOptionsCore> const& printTaskOptions)
            : m_currentPreviewPageNumber(currentPreviewPageNumber)
            , m_newPreviewPageNumber(1)
            , m_printTaskOptions(printTaskOptions)
        {
        }

        uint32_t GetNewPreviewPageNumber() const
        {
            return m_newPreviewPageNumber;
        }
        
        IFACEMETHODIMP get_CurrentPreviewPageNumber(uint32_t* value) override
        {
            return ExceptionBoundary(
                [&]
                {
                    CheckInPointer(value);
                    *value = m_currentPreviewPageNumber;
                });
        }
        
        IFACEMETHODIMP put_NewPreviewPageNumber(uint32_t value) override
        {
            return ExceptionBoundary(
                [&]
                {
                    if (value < 1)
                        ThrowHR(E_INVALIDARG);

                    m_newPreviewPageNumber = value;
                });                
        }
        
        IFACEMETHODIMP get_NewPreviewPageNumber(uint32_t* value) override
        {
            return ExceptionBoundary(
                [&]
                {
                    CheckInPointer(value);
                    *value = m_newPreviewPageNumber;
                });
        }

        IFACEMETHODIMP GetDeferral(ICanvasPrintDeferral** value) override
        {
            return ExceptionBoundary(
                [&]
                {
                    CheckAndClearOutPointer(value);
                    ThrowHR(E_NOTIMPL);
                });
        }
        
        IFACEMETHODIMP get_PrintTaskOptions(IPrintTaskOptionsCore** value) override
        {
            return ExceptionBoundary(
                [&]
                {
                    CheckAndClearOutPointer(value);
                    ThrowIfFailed(m_printTaskOptions.CopyTo(value));
                });
        }
    };

} } } } }
