// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#include "CanvasTypography.h"

CanvasTypography::CanvasTypography(
    IDWriteTypography* typography)
    : ResourceWrapper(typography)
{
}

IFACEMETHODIMP CanvasTypography::AddFeature(CanvasTypographyFeature feature)
{
    return AddFeatureWithNameAndParameter(feature.Name, feature.Parameter);
}

IFACEMETHODIMP CanvasTypography::AddFeatureWithNameAndParameter(CanvasTypographyFeatureName name, uint32_t parameter)
{
    return ExceptionBoundary(
        [&]
        {
            DWRITE_FONT_FEATURE dwriteFontFeature{};
            dwriteFontFeature.nameTag = ToDWriteFontFeatureTag(name);
            dwriteFontFeature.parameter = parameter;
            ThrowIfFailed(GetResource()->AddFontFeature(dwriteFontFeature));
        });
}

IFACEMETHODIMP CanvasTypography::GetFeatures(
    uint32_t* valueCount,
    CanvasTypographyFeature** valueElements)
{
    return ExceptionBoundary(
        [&]
        {
            auto& resource = GetResource();

            uint32_t featureCount = resource->GetFontFeatureCount();

            std::vector<DWRITE_FONT_FEATURE> dwriteFontFeatures(featureCount);

            for (uint32_t i = 0; i < featureCount; ++i)
                ThrowIfFailed(resource->GetFontFeature(i, &dwriteFontFeatures[i]));
            
            auto returnedFeatures = TransformToComArray<CanvasTypographyFeature>(
                dwriteFontFeatures.begin(),
                dwriteFontFeatures.end(),
                [](DWRITE_FONT_FEATURE const& dwriteFeature)
                {                
                    CanvasTypographyFeature feature{};
                    feature.Name = ToCanvasTypographyFeatureName(dwriteFeature.nameTag);
                    feature.Parameter = dwriteFeature.parameter;

                    return feature;
                });

            returnedFeatures.Detach(valueCount, valueElements);
        });
}


std::vector<DWRITE_FONT_FEATURE> CanvasTypography::GetFeatureData()
{
    auto& resource = GetResource();

    uint32_t featureCount = resource->GetFontFeatureCount();

    std::vector<DWRITE_FONT_FEATURE> featureData;
    featureData.reserve(featureCount);

    for (uint32_t i = 0; i < featureCount; ++i)
    {
        DWRITE_FONT_FEATURE dwriteFontFeature;
        ThrowIfFailed(resource->GetFontFeature(i, &dwriteFontFeature));
        featureData.push_back(dwriteFontFeature);
    }

    return featureData;
}


_Use_decl_annotations_
IFACEMETHODIMP CanvasTypographyFactory::ActivateInstance(IInspectable** object)
{
    return ExceptionBoundary(
        [&]
        {
            ComPtr<IDWriteTypography> dwriteTypography;
            ThrowIfFailed(CustomFontManager::GetInstance()->GetSharedFactory()->CreateTypography(&dwriteTypography));

            auto newCanvasTypography = Make<CanvasTypography>(dwriteTypography.Get());
            CheckMakeResult(newCanvasTypography);

            ThrowIfFailed(newCanvasTypography.CopyTo(object));
        });
}

ActivatableClassWithFactory(CanvasTypography, CanvasTypographyFactory);
