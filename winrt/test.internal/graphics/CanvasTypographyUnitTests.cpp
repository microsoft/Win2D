// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#include <lib/text/CanvasTypography.h>

#include "stubs/StubCanvasTextLayoutAdapter.h"

#include "mocks/MockDWriteTypography.h"

namespace canvas
{
    using namespace ABI::Windows::UI::Text;

    TEST_CLASS(CanvasTypographyTests)
    {
    public:

        struct Fixture
        {
            std::shared_ptr<StubCanvasTextLayoutAdapter> Adapter;
            ComPtr<CanvasTypographyFactory> TypographyFactory;
            ComPtr<MockDWriteTypography> DWriteTypography;

            Fixture()
                : Adapter(std::make_shared<StubCanvasTextLayoutAdapter>())
            {
                DWriteTypography = Make<MockDWriteTypography>();

                Adapter->GetMockDWriteFactory()->CreateTypographyMethod.AllowAnyCall(
                    [=](IDWriteTypography** typography)
                    {
                        return DWriteTypography.CopyTo(typography);
                    });

                CustomFontManagerAdapter::SetInstance(Adapter);

                TypographyFactory = Make<CanvasTypographyFactory>();
            }

            ComPtr<ICanvasTypography> CreateTypography()
            {
                ComPtr<IInspectable> inspectable;
                ThrowIfFailed(TypographyFactory->ActivateInstance(&inspectable));

                return As<ICanvasTypography>(inspectable);
            }

            void ExpectOneCreateTypography()
            {
                Adapter->GetMockDWriteFactory()->CreateTypographyMethod.SetExpectedCalls(1, 
                    [=](IDWriteTypography** typography)
                    {
                        return DWriteTypography.CopyTo(typography);
                    });
            }
        };

        TEST_METHOD_EX(CanvasTypographyTests_Implements_Expected_Interfaces)
        {
            Fixture f;

            auto typography = f.CreateTypography();

            ASSERT_IMPLEMENTS_INTERFACE(typography, ICanvasTypography);
            ASSERT_IMPLEMENTS_INTERFACE(typography, ICanvasResourceWrapperNative);
        }

        TEST_METHOD_EX(CanvasTypographyTests_CreationCallsThroughToDWrite)
        {
            Fixture f;

            f.ExpectOneCreateTypography();

            auto typography = f.CreateTypography();
        }

        TEST_METHOD_EX(CanvasTypographyTests_AddFeature)
        {
            for (int i = 0; i < 2; ++i)
            {
                Fixture f;

                auto typography = f.CreateTypography();

                f.DWriteTypography->AddFontFeatureMethod.SetExpectedCalls(1,
                    [&](DWRITE_FONT_FEATURE feature)
                    {
                        Assert::AreEqual(DWRITE_FONT_FEATURE_TAG_HISTORICAL_LIGATURES, feature.nameTag);
                        Assert::AreEqual(123u, feature.parameter);
                        return S_OK;
                    });

                if (i == 0)
                    Assert::AreEqual(S_OK, typography->AddFeature(CanvasTypographyFeature{ CanvasTypographyFeatureName::HistoricalLigatures, 123u }));
                else
                    Assert::AreEqual(S_OK, typography->AddFeatureWithNameAndParameter(CanvasTypographyFeatureName::HistoricalLigatures, 123u));

            }
        }

        TEST_METHOD_EX(CanvasTypographyTests_GetFeatures)
        {
            Fixture f;

            f.DWriteTypography->GetFontFeatureCountMethod.SetExpectedCalls(1, [&] { return 3; });

            f.DWriteTypography->GetFontFeatureMethod.SetExpectedCalls(3,
                [&](UINT32 index, DWRITE_FONT_FEATURE* out)
                {
                    static const DWRITE_FONT_FEATURE features[]
                    {
                        { DWRITE_FONT_FEATURE_TAG_CAPITAL_SPACING, 4u },
                        { DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_19, 5u },
                        { DWRITE_FONT_FEATURE_TAG_SUPERSCRIPT, 6u }
                    };
                    *out = features[index];
                    return S_OK;
                });

            auto typography = f.CreateTypography();

            uint32_t retrievedFeatureCount;
            CanvasTypographyFeature* retrievedFeatures;

            Assert::AreEqual(S_OK, typography->GetFeatures(&retrievedFeatureCount, &retrievedFeatures));
            Assert::AreEqual(3u, retrievedFeatureCount);

            Assert::AreEqual(CanvasTypographyFeatureName::CapitalSpacing, retrievedFeatures[0].Name);
            Assert::AreEqual(4u, retrievedFeatures[0].Parameter);

            Assert::AreEqual(CanvasTypographyFeatureName::StylisticSet19, retrievedFeatures[1].Name);
            Assert::AreEqual(5u, retrievedFeatures[1].Parameter);

            Assert::AreEqual(CanvasTypographyFeatureName::Superscript, retrievedFeatures[2].Name);
            Assert::AreEqual(6u, retrievedFeatures[2].Parameter);
        }

        TEST_METHOD_EX(CanvasTypographyTests_UnknownFeature)
        {
            for (int i = 0; i < 2; ++i)
            {
                Fixture f;

                auto typography = f.CreateTypography();

                if (i == 0)
                    Assert::AreEqual(E_INVALIDARG, typography->AddFeature(CanvasTypographyFeature{ CanvasTypographyFeatureName::None, 123u }));
                else
                    Assert::AreEqual(E_INVALIDARG, typography->AddFeatureWithNameAndParameter(CanvasTypographyFeatureName::None, 123u));

                ValidateStoredErrorState(E_INVALIDARG, Strings::InvalidTypographyFeatureName);

            }
        }
    };
}
