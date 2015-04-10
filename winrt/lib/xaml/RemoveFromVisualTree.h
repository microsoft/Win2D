// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may
// not use these files except in compliance with the License. You may obtain
// a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations
// under the License.

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
    }

}}}}}}