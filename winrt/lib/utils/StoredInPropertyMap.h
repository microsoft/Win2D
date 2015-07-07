// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    //
    // CoreApplication.Properties is used to store backlinks to our active ResourceManager
    // instances. When the manager is destroyed, it must also clean up these backlinks.
    // This type provides an inheritable behavior to do that. When destroyed,
    // it removes the specified key from the specified map.
    //
    class StoredInPropertyMap
    {
        ComPtr<ABI::Windows::Foundation::Collections::IMap<HSTRING, IInspectable*>> m_propertyMap;
        WinString m_key;

    protected:
        ~StoredInPropertyMap()
        {
            if (m_propertyMap)
                (void)m_propertyMap->Remove(m_key);
        }

    public:
        void WhenDestroyedRemoveFromPropertyMap(ABI::Windows::Foundation::Collections::IMap<HSTRING, IInspectable*>* propertyMap, HSTRING key)
        {
            assert(!m_propertyMap);

            m_propertyMap = propertyMap;
            m_key = key;
        }
    };

}}}}
