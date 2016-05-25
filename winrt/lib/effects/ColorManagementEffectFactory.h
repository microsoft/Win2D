// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects 
{
    class ColorManagementEffectFactory
        : public AgileActivationFactory<IColorManagementEffectStatics>
        , private LifespanTracker<ColorManagementEffectFactory>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_Effects_ColorManagementEffect, BaseTrust);

    public:
        IFACEMETHODIMP ActivateInstance(IInspectable**) override;
        IFACEMETHODIMP IsBestQualitySupported(ICanvasDevice* device, boolean* result) override;
    };

}}}}}
