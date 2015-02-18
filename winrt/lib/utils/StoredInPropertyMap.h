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
