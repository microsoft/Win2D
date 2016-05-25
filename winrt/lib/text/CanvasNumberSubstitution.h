// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Text
{
    class CanvasNumberSubstitution : RESOURCE_WRAPPER_RUNTIME_CLASS(
        IDWriteNumberSubstitution,
        CanvasNumberSubstitution,
        ICanvasNumberSubstitution)
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_Text_CanvasNumberSubstitution, BaseTrust);

    public:
        CanvasNumberSubstitution(IDWriteNumberSubstitution* numberSubstitution);
    };


    //
    // CanvasTextNumberSubstitutionFactory
    //

    class CanvasNumberSubstitutionFactory
        : public AgileActivationFactory<ICanvasNumberSubstitutionFactory>
        , private LifespanTracker<CanvasNumberSubstitutionFactory>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_Text_CanvasNumberSubstitution, BaseTrust);

    public:

        IFACEMETHOD(Create)(
            CanvasNumberSubstitutionMethod method,
            ICanvasNumberSubstitution** numberSubstitution);

        IFACEMETHOD(CreateWithLocaleAndIgnoreOverrides)(
            CanvasNumberSubstitutionMethod method,
            HSTRING localeName,
            boolean ignoreOverrides,
            ICanvasNumberSubstitution** numberSubstitution);
    };
    
}}}}}
