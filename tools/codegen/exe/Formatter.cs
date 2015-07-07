// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using System.Diagnostics;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Globalization;

namespace CodeGen
{
    // Please create Formatter object within using scope
    // Example: using (Formatter someFormatter = new Formatter("FilePath")){...}
    public class Formatter : System.IDisposable
    {
        // Provides indenting, where one indent equals four spaces.
        public Formatter(string fileName)
        {
            m_streamWriter = new StreamWriter(fileName);
            m_singleIndent = "    "; // Fixed to four spaces, according to our convention.
        }

        public void Dispose()
        {
            m_streamWriter.Dispose();
        }

        public void Indent()
        {
            m_indentAccumulator += m_singleIndent;
        }

        public void Unindent()
        {
            m_indentAccumulator = m_indentAccumulator.Substring(0, m_indentAccumulator.Length - m_singleIndent.Length);
        }

        public void WriteLine(string line)
        {
            m_streamWriter.WriteLine(m_indentAccumulator + line);
        }

        public void WriteLine()
        {
            m_streamWriter.WriteLine();
        }

        public void Write(string str)
        {
            m_streamWriter.Write(str);
        }

        public void WriteIndent()
        {
            m_streamWriter.Write(m_indentAccumulator);
        }

        // Member variables
        string m_singleIndent;
        string m_indentAccumulator;
        StreamWriter m_streamWriter;

        // Public utility functions
        public static string StylizeWithCapitalLeadingLetter(string name)
        {
            if (name == null)
            {
                return null;
            }

            if (name.Length == 0)
            {
                return "";
            }

            return char.ToUpper(name[0]) + name.Substring(1);
        }

        public static string StylizeNameFromUnderscoreSeparators(string name) 
        {
            //
            // For example, this changes ARC_SIZE to ArcSize, GEOMETRY_SIMPLIFICATION_OPTION to GeometrySimplificationOption.
            //
            // It would be straightforward to do textInfo.ToTitleCase to fix the casing here, however
            // it doesn't behave exactly the same way as D2D's casing for helper methods.
            //
            // For example, D2D's helper for D2D1_MATRIX_3X2_F is called Matrix3x2F. It would
            // be nice to just take each token, titlecase them, and concatinate them together.
            // But textInfo.ToTitleCase("3x2") is "3X2".
            //

            if (name == null)
            {
                return null;
            }

            if (name.Length == 0)
            {
                return "";
            }
            
            string[] tokens = name.Split('_');
            StringBuilder result = new StringBuilder();
            CultureInfo cultureInfo = new CultureInfo("en-us");
            TextInfo textInfo = cultureInfo.TextInfo;

            foreach (string token in tokens)
            {
                if(token.Length > 0)
                {
                    string r = char.ToUpper(token[0]) + token.Substring(1).ToLower();
                    result.Append(r);
                }
            }
            
            return result.ToString();
        }

        public static string Prefix;
    }

}
