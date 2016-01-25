// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

class MockXamlSolidColorBrush : public RuntimeClass<ISolidColorBrush, IBrush>
{
public:

    // ISolidColorBrush
    
    MOCK_METHOD1(get_Color, HRESULT(ABI::Windows::UI::Color *));
    MOCK_METHOD1(put_Color, HRESULT(ABI::Windows::UI::Color));

    // IBrush

    MOCK_METHOD1(get_Opacity           , HRESULT(double *));
    MOCK_METHOD1(put_Opacity           , HRESULT(double));
    MOCK_METHOD1(get_Transform         , HRESULT(ABI::Windows::UI::Xaml::Media::ITransform **));
    MOCK_METHOD1(put_Transform         , HRESULT(ABI::Windows::UI::Xaml::Media::ITransform *));
    MOCK_METHOD1(get_RelativeTransform , HRESULT(ABI::Windows::UI::Xaml::Media::ITransform **));
    MOCK_METHOD1(put_RelativeTransform , HRESULT(ABI::Windows::UI::Xaml::Media::ITransform *));
};
