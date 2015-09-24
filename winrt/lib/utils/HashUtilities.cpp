// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"
#include "HashUtilities.h"

using namespace ABI::Windows::Security::Cryptography;
using namespace ABI::Windows::Security::Cryptography::Core;
using namespace ABI::Windows::Storage::Streams;

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    ComArray<BYTE> GetSha1Hash(BYTE const* data, size_t dataSize)
    {
        // Look up WinRT activation factories.
        ComPtr<ICryptographicBufferStatics> bufferStatics;
        ThrowIfFailed(GetActivationFactory(Wrappers::HStringReference(RuntimeClass_Windows_Security_Cryptography_CryptographicBuffer).Get(), &bufferStatics));

        ComPtr<IHashAlgorithmNamesStatics> algorithmNames;
        ThrowIfFailed(GetActivationFactory(Wrappers::HStringReference(RuntimeClass_Windows_Security_Cryptography_Core_HashAlgorithmNames).Get(), &algorithmNames));

        ComPtr<IHashAlgorithmProviderStatics> algorithmStatics;
        ThrowIfFailed(GetActivationFactory(Wrappers::HStringReference(RuntimeClass_Windows_Security_Cryptography_Core_HashAlgorithmProvider).Get(), &algorithmStatics));

        // Convert the input data to an IBuffer.
        ComPtr<IBuffer> dataBuffer;
        ThrowIfFailed(bufferStatics->CreateFromByteArray(static_cast<UINT32>(dataSize), const_cast<BYTE*>(data), &dataBuffer));

        // Look up the name of the SHA1 hashing algorithm.
        WinString sha1Name;
        ThrowIfFailed(algorithmNames->get_Sha1(sha1Name.GetAddressOf()));

        // Instantiate the hashing algorithm.
        ComPtr<IHashAlgorithmProvider> algorithm;
        ThrowIfFailed(algorithmStatics->OpenAlgorithm(sha1Name, &algorithm));

        // Hash our data.
        ComPtr<IBuffer> resultBuffer;
        ThrowIfFailed(algorithm->HashData(dataBuffer.Get(), &resultBuffer));

        // Convert IBuffer -> ComArray.
        ComArray<BYTE> result;
        ThrowIfFailed(bufferStatics->CopyToByteArray(resultBuffer.Get(), result.GetAddressOfSize(), result.GetAddressOfData()));

        return result;
    }


    // Swaps a UUID between local and network byte ordering.
    static void SwapUuidByteOrder(BYTE* uuid)
    {
        // htonl(time_low)
        std::swap(uuid[0], uuid[3]);
        std::swap(uuid[1], uuid[2]);

        // htons(time_mid)
        std::swap(uuid[4], uuid[5]);

        // htons(time_hi_and_version)
        std::swap(uuid[6], uuid[7]);
    }


    // Creates a version 5 (SHA-1) UUID following section 4.3 of RFC 4122.
    // Unlike the more common version 4 UUID algorithm, this uses a stable hash
    // based on an input name, so the same name always produces the same UUID .
    IID GetVersion5Uuid(IID const& namespaceId, BYTE const* name, size_t nameSize)
    {
        // Convert the namespace to a byte array.
        auto namespaceBytes = reinterpret_cast<BYTE const*>(&namespaceId);

        std::vector<BYTE> buffer(namespaceBytes, namespaceBytes + sizeof(IID));

        // Convert to network byte ordering.
        SwapUuidByteOrder(buffer.data());

        // Concatenate the name onto the namespace.
        buffer.insert(buffer.end(), name, name + nameSize);

        // Take the first 16 bytes of the SHA-1 hash.
        auto result = GetSha1Hash(buffer.data(), buffer.size());
    
        assert(result.GetSize() >= sizeof(IID));

        // Set the variant bits (MSB0-1 = 2 means standard RFC 4122 UUID).
        result[8] &= 0x3F;
        result[8] |= 2 << 6;

        // Set the version bits (MSB0-3 = 5 means SHA-1 name-based UUID).
        result[6] &= 0x0F;
        result[6] |= 5 << 4;

        // Convert to local byte ordering.
        SwapUuidByteOrder(result.GetData());

        return *reinterpret_cast<IID*>(result.GetData());
    }

}}}}
