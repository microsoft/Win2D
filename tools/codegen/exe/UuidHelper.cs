// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using System;
using System.Diagnostics;
using System.Linq;
using System.Security.Cryptography;
using System.Text;

namespace CodeGen
{
    public static class UuidHelper
    {
        // Creates a version 5 (SHA-1) UUID following section 4.3 of RFC 4122.
        // Unlike the more common version 4 UUID algorithm, this uses a stable hash
        // based on an input name, so the same name always produces the same UUID.
        public static Guid GetVersion5Uuid(Guid namespaceId, string name)
        {
            // Convert inputs to byte arrays.
            var namespaceBytes = namespaceId.ToByteArray();
            var nameBytes = Encoding.UTF8.GetBytes(name);

            // Convert to network byte ordering.
            SwapByteOrder(namespaceBytes);

            // Concatenate the namespace and name.
            var inputBytes = namespaceBytes.Concat(nameBytes).ToArray();

            // Take the first 16 bytes of the SHA-1 hash.
            byte[] result;

            using (var sha1 = SHA1.Create())
            {
                result = sha1.ComputeHash(inputBytes).Take(16).ToArray();
            }

            // Set the variant bits (MSB0-1 = 2 means standard RFC 4122 UUID).
            result[8] &= 0x3F;
            result[8] |= 2 << 6;

            // Set the version bits (MSB0-3 = 5 means SHA-1 name-based UUID).
            result[6] &= 0x0F;
            result[6] |= 5 << 4;

            // Convert to local byte ordering.
            SwapByteOrder(result);

            return new Guid(result);
        }


        // Swaps a UUID between local and network byte ordering.
        static void SwapByteOrder(byte[] uuid)
        {
            Debug.Assert(uuid.Length == 16);

            if (BitConverter.IsLittleEndian)
            {
                // htonl(time_low)
                Swap(ref uuid[0], ref uuid[3]);
                Swap(ref uuid[1], ref uuid[2]);

                // htons(time_mid)
                Swap(ref uuid[4], ref uuid[5]);

                // htons(time_hi_and_version)
                Swap(ref uuid[6], ref uuid[7]);
            }
        }


        static void Swap(ref byte a, ref byte b)
        {
            byte temp = a;
            a = b;
            b = temp;
        }
    }
}
