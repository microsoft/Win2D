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

namespace canvas
{
    using namespace ABI::Windows::Foundation::Collections;

    class MockPropertyValue : public RuntimeClass<IPropertyValue>
    {
        IFACEMETHODIMP get_Type(
            ABI::Windows::Foundation::PropertyType *)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP get_IsNumericScalar(
            boolean *)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP GetUInt8(
            BYTE *)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP GetInt16(
            INT16 *)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP GetUInt16(
            UINT16 *)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP GetInt32(
            INT32 *)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP GetUInt32(
            UINT32 *)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP GetInt64(
            INT64 *)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP GetUInt64(
            UINT64 *)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP GetSingle(
            FLOAT *)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP GetDouble(
            DOUBLE *)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP GetChar16(
            WCHAR *)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP GetBoolean(
            boolean *)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP GetString(
            HSTRING *)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP GetGuid(
            GUID *)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP GetDateTime(
            ABI::Windows::Foundation::DateTime *)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP GetTimeSpan(
            ABI::Windows::Foundation::TimeSpan *)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP GetPoint(
            ABI::Windows::Foundation::Point *)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP GetSize(
            ABI::Windows::Foundation::Size *)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP GetRect(
            ABI::Windows::Foundation::Rect *)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP GetUInt8Array(
            UINT32 *,
            BYTE **)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP GetInt16Array(
            UINT32 *,
            INT16 **)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP GetUInt16Array(
            UINT32 *,
            UINT16 **)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP GetInt32Array(
            UINT32 *,
            INT32 **)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP GetUInt32Array(
            UINT32 *,
            UINT32 **)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP GetInt64Array(
            UINT32 *,
            INT64 **)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP GetUInt64Array(
            UINT32 *,
            UINT64 **)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP GetSingleArray(
            UINT32 *,
            FLOAT **)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP GetDoubleArray(
            UINT32 *,
            DOUBLE **)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP GetChar16Array(
            UINT32 *,
            WCHAR **)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP GetBooleanArray(
            UINT32 *,
            boolean **)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP GetStringArray(
            UINT32 *,
            HSTRING **)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP GetInspectableArray(
            UINT32 *,
            IInspectable ***)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP GetGuidArray(
            UINT32 *,
            GUID **)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP GetDateTimeArray(
            UINT32 *,
            ABI::Windows::Foundation::DateTime **)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP GetTimeSpanArray(
            UINT32 *,
            ABI::Windows::Foundation::TimeSpan **)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP GetPointArray(
            UINT32 *,
            ABI::Windows::Foundation::Point **)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP GetSizeArray(
            UINT32 *,
            ABI::Windows::Foundation::Size **)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP GetRectArray(
            UINT32 *,
            ABI::Windows::Foundation::Rect **)
		{
			return E_NOTIMPL;
		}

    };

    class MockPropertyValueStatics : public RuntimeClass<IPropertyValueStatics>
    {
        IFACEMETHODIMP CreateEmpty(
            IInspectable** propertyValue)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP CreateUInt8(
            BYTE,
            IInspectable** propertyValue)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP CreateInt16(
            INT16,
            IInspectable** propertyValue)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP CreateUInt16(
            UINT16,
            IInspectable** propertyValue)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP CreateInt32(
            INT32,
            IInspectable** propertyValue)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP CreateUInt32(
            UINT32,
            IInspectable** propertyValue)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP CreateInt64(
            INT64,
            IInspectable** propertyValue)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP CreateUInt64(
            UINT64,
            IInspectable** propertyValue)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP CreateSingle(
            FLOAT,
            IInspectable** propertyValue)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP CreateDouble(
            DOUBLE,
            IInspectable** propertyValue)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP CreateChar16(
            WCHAR,
            IInspectable** propertyValue)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP CreateBoolean(
            boolean,
            IInspectable** propertyValue)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP CreateString(
             HSTRING,
            IInspectable** propertyValue)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP CreateInspectable(
            IInspectable *,
            IInspectable** propertyValue)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP CreateGuid(
            GUID,
            IInspectable** propertyValue)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP CreateDateTime(
            ABI::Windows::Foundation::DateTime,
            IInspectable** propertyValue)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP CreateTimeSpan(
            ABI::Windows::Foundation::TimeSpan,
            IInspectable** propertyValue)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP CreatePoint(
            ABI::Windows::Foundation::Point,
            IInspectable** propertyValue)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP CreateSize(
            ABI::Windows::Foundation::Size,
            IInspectable** propertyValue)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP CreateRect(
            ABI::Windows::Foundation::Rect,
            IInspectable** propertyValue)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP CreateUInt8Array(
            UINT32,
            BYTE *,
            IInspectable** propertyValue)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP CreateInt16Array(
            UINT32,
            INT16 *,
            IInspectable** propertyValue)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP CreateUInt16Array(
            UINT32,
            UINT16 *,
            IInspectable** propertyValue)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP CreateInt32Array(
            UINT32,
            INT32 *,
            IInspectable** propertyValue)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP CreateUInt32Array(
            UINT32,
            UINT32 *,
            IInspectable** propertyValue)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP CreateInt64Array(
            UINT32,
            INT64 *,
            IInspectable** propertyValue)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP CreateUInt64Array(
            UINT32,
            UINT64 *,
            IInspectable** propertyValue)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP CreateSingleArray(
            UINT32,
            FLOAT *,
            IInspectable** propertyValue)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP CreateDoubleArray(
            UINT32,
            DOUBLE *,
            IInspectable** propertyValue)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP CreateChar16Array(
            UINT32,
            WCHAR *,
            IInspectable** propertyValue)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP CreateBooleanArray(
            UINT32,
            boolean *,
            IInspectable** propertyValue)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP CreateStringArray(
            UINT32,
            HSTRING *,
            IInspectable** propertyValue)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP CreateInspectableArray(
            UINT32,
            IInspectable**,
            IInspectable** propertyValue)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP CreateGuidArray(
            UINT32,
            GUID *,
            IInspectable** propertyValue)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP CreateDateTimeArray(
            UINT32,
            ABI::Windows::Foundation::DateTime *,
            IInspectable** propertyValue)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP CreateTimeSpanArray(
            UINT32,
            ABI::Windows::Foundation::TimeSpan *,
            IInspectable** propertyValue)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP CreatePointArray(
            UINT32,
            ABI::Windows::Foundation::Point *,
            IInspectable** propertyValue)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP CreateSizeArray(
            UINT32,
            ABI::Windows::Foundation::Size *,
            IInspectable** propertyValue)
		{
			return E_NOTIMPL;
		}

        IFACEMETHODIMP CreateRectArray(
            UINT32,
            ABI::Windows::Foundation::Rect *,
            IInspectable** propertyValue)
		{
			return E_NOTIMPL;
		}
    };

    class MockProperties : public RuntimeClass<
        IMap<HSTRING, IInspectable*>, IMap_impl<HSTRING, IInspectable*>, IPropertySet>
    {
    public:
        CALL_COUNTER_WITH_MOCK(LookupMethod, HRESULT(HSTRING, IInspectable**));
        CALL_COUNTER_WITH_MOCK(InsertMethod, HRESULT(HSTRING, IInspectable*, boolean *));

        IFACEMETHODIMP add_MapChanged(MapChangedEventHandler<HSTRING, IInspectable*>*, EventRegistrationToken*)
        {
            Assert::Fail(L"Unexpected call to add_MapChanged");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP remove_MapChanged(EventRegistrationToken)
        {
            Assert::Fail(L"Unexpected call to remove_MapChanged");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP Lookup(HSTRING key, IInspectable** value)
        {
            return LookupMethod.WasCalled(key, value);
        }

        IFACEMETHODIMP get_Size(unsigned int *)
        {
            Assert::Fail(L"Unexpected call to get_Size");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP HasKey(HSTRING, boolean *)
        {
            Assert::Fail(L"Unexpected call to HasKey");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP GetView(IMapView<HSTRING, IInspectable*> **view)
        {
            Assert::Fail(L"Unexpected call to GetView");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP Insert(HSTRING key, IInspectable* value, boolean * inserted)
        {
            return InsertMethod.WasCalled(key, value, inserted);
        }
        IFACEMETHODIMP Remove(HSTRING)
        {
            Assert::Fail(L"Unexpected call to Remove");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP Clear()
        {
            Assert::Fail(L"Unexpected call to Clear");
            return E_NOTIMPL;
        }
    };
}
