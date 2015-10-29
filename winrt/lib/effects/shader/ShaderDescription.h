// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects 
{
    // Metadata describing a shader variable.
    struct ShaderVariable
    {
        ShaderVariable(D3D11_SHADER_VARIABLE_DESC const& desc, D3D11_SHADER_TYPE_DESC const& type)
            : Name(std::wstring(desc.Name, desc.Name + strlen(desc.Name)))
            , Class(type.Class)
            , Type(type.Type)
            , Rows(type.Rows)
            , Columns(type.Columns)
            , Elements(type.Elements)
            , Size(desc.Size)
            , Offset(desc.StartOffset)
        { }


        WinString Name;

        D3D_SHADER_VARIABLE_CLASS Class;
        D3D_SHADER_VARIABLE_TYPE Type;

        unsigned Rows;
        unsigned Columns;
        unsigned Elements;

        unsigned Size;
        unsigned Offset;


        unsigned ComponentCount() const
        {
            return Rows * Columns * std::max(Elements, 1u);
        }


        bool IsVector(unsigned columns) const
        {
            return Class == D3D_SVC_VECTOR && 
                   Rows == 1 && 
                   Columns == columns;
        }


        bool IsMatrix(unsigned rows, unsigned columns) const
        {
            return (Class == D3D_SVC_MATRIX_ROWS || Class == D3D_SVC_MATRIX_COLUMNS) &&
                   Rows == rows &&
                   Columns == columns;
        }
    };


    // Comparison operators enable sort and search.
    struct VariableNameComparison
    {
        MapKeyComparison<WinString> comparison;

        bool operator() (ShaderVariable const& value1, ShaderVariable const& value2) const { return comparison(value1.Name, value2.Name); }
        bool operator() (ShaderVariable const& value1, HSTRING               value2) const { return comparison(value1.Name, value2); }
        bool operator() (HSTRING               value1, ShaderVariable const& value2) const { return comparison(value1, value2.Name); }
    };


    // Copyable struct stores shader program code along with metadata describing how to use the shader.
    struct ShaderDescription
    {
        ShaderDescription()
            : Hash(GUID_NULL)
            , InputCount(0)
            , InstructionCount(0)
            , MinFeatureLevel(static_cast<D3D_FEATURE_LEVEL>(0))
        { }


        std::vector<BYTE> Code;

        // Generated from the shader code following RFC 4122 section 4.3 (version 5 UUID).
        IID Hash;

        unsigned InputCount;
        unsigned InstructionCount;
        D3D_FEATURE_LEVEL MinFeatureLevel;

        // Sorted by name.
        std::vector<ShaderVariable> Variables;
    };

}}}}}
