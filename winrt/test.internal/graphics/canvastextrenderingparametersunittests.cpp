// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#include <lib/text/CanvasTextRenderingParameters.h>

namespace canvas
{
    using namespace ABI::Windows::UI::Text;

    TEST_CLASS(CanvasTextRenderingParametersTests)
    {
        TEST_METHOD_EX(CanvasTextRenderingParametersTests_Implements_Expected_Interfaces)
        {
            auto textRenderingParameters = CanvasTextRenderingParameters::CreateNew(CanvasTextRenderingMode::Default, CanvasTextGridFit::Default);

            ASSERT_IMPLEMENTS_INTERFACE(textRenderingParameters, ICanvasTextRenderingParameters);
            ASSERT_IMPLEMENTS_INTERFACE(textRenderingParameters, ABI::Windows::Foundation::IClosable);
            ASSERT_IMPLEMENTS_INTERFACE(textRenderingParameters, ICanvasResourceWrapperNative);
        }

        TEST_METHOD_EX(CanvasTextRenderingParametersTests_Closure)
        {
            auto textRenderingParameters = CanvasTextRenderingParameters::CreateNew(CanvasTextRenderingMode::Default, CanvasTextGridFit::Default);

            Assert::AreEqual(S_OK, textRenderingParameters->Close());

            CanvasTextRenderingMode renderingMode;
            Assert::AreEqual(RO_E_CLOSED, textRenderingParameters->get_RenderingMode(&renderingMode));

            CanvasTextGridFit gridFit;
            Assert::AreEqual(RO_E_CLOSED, textRenderingParameters->get_GridFit(&gridFit));
        }

        TEST_METHOD_EX(CanvasTextRenderingParametersTests_NullArgs)
        {
            auto textRenderingParameters = CanvasTextRenderingParameters::CreateNew(CanvasTextRenderingMode::Default, CanvasTextGridFit::Default);

            Assert::AreEqual(E_INVALIDARG, textRenderingParameters->get_RenderingMode(nullptr));

            Assert::AreEqual(E_INVALIDARG, textRenderingParameters->get_GridFit(nullptr));
        }

        TEST_METHOD_EX(CanvasTextRenderingParametersTests_get_RenderingMode)
        {
            static CanvasTextRenderingMode renderingModes[] =
            {
                CanvasTextRenderingMode::Default,
                CanvasTextRenderingMode::Aliased,
                CanvasTextRenderingMode::GdiClassic,
                CanvasTextRenderingMode::GdiNatural,
                CanvasTextRenderingMode::Natural,
                CanvasTextRenderingMode::NaturalSymmetric,
                CanvasTextRenderingMode::Outline,
#if WINVER > _WIN32_WINNT_WINBLUE
                CanvasTextRenderingMode::NaturalSymmetricDownsampled
#endif
            };

            for (auto expectedRenderingMode : renderingModes)
            {
                auto textRenderingParameters = CanvasTextRenderingParameters::CreateNew(expectedRenderingMode, CanvasTextGridFit::Default);

                CanvasTextRenderingMode actualRenderingMode;
                Assert::AreEqual(S_OK, textRenderingParameters->get_RenderingMode(&actualRenderingMode));
                Assert::AreEqual(expectedRenderingMode, actualRenderingMode);
            }
        }

        TEST_METHOD_EX(CanvasTextRenderingParametersTests_get_GridFit)
        {
            static CanvasTextGridFit gridFits[] =
            {
                CanvasTextGridFit::Default,
                CanvasTextGridFit::Disable,
                CanvasTextGridFit::Enable
            };

            for (auto expectedGridFit : gridFits)
            {
                auto textRenderingParameters = CanvasTextRenderingParameters::CreateNew(CanvasTextRenderingMode::Default, expectedGridFit);

                CanvasTextGridFit actualGridFit;
                Assert::AreEqual(S_OK, textRenderingParameters->get_GridFit(&actualGridFit));
                Assert::AreEqual(expectedGridFit, actualGridFit);
            }
        }
    };
}