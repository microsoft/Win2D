// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace UI { namespace Xaml
{
    using namespace ABI::Windows::UI::Xaml;
    using namespace ABI::Windows::UI::Xaml::Controls;

    //
    // Removes the given element from a parent.  This needs to inspect the
    // parent to figure out what property might be holding a reference to the
    // element.
    //
    inline void RemoveFromVisualTreeImpl(IDependencyObject* parent, IUIElement* toRemove)
    {
        if (!parent)
            return;

        if (auto panel = MaybeAs<IPanel>(parent))
        {
            ComPtr<IVector<UIElement*>> children;
            ThrowIfFailed(panel->get_Children(&children));

            uint32_t index;
            boolean found;
            ThrowIfFailed(children->IndexOf(toRemove, &index, &found));

            if (found)
            {
                ThrowIfFailed(children->RemoveAt(index));
            }
        }
        else if (auto userControl = MaybeAs<IUserControl>(parent))
        {
            ComPtr<IUIElement> currentContent;
            ThrowIfFailed(userControl->get_Content(&currentContent));
            if (IsSameInstance(currentContent.Get(), toRemove))
            {
                ThrowIfFailed(userControl->put_Content(nullptr));
            }
        }
        else if (auto contentControl = MaybeAs<IContentControl>(parent))
        {
            ComPtr<IInspectable> currentContent;
            ThrowIfFailed(contentControl->get_Content(&currentContent));
            if (IsSameInstance(currentContent.Get(), toRemove))
            {
                ThrowIfFailed(contentControl->put_Content(nullptr));
            }
        }
    }

}}}}}}
