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

using System;
using System.IO;

namespace ExtractAPISurface
{
    class CodeWriter : IDisposable
    {
        TextWriter output;

        bool wantSeparator;
        int indentation;

        const int TabSize = 4;


        public CodeWriter(string directory, string filename)
        {
            // Make sure the output directory exists.
            if (!string.IsNullOrEmpty(directory))
            {
                Directory.CreateDirectory(directory);

                filename = Path.Combine(directory, filename);
            }

            // Open the output file.
            Console.WriteLine("Writing {0}", filename);

            output = new StreamWriter(filename);
        }


        public void Dispose()
        {
            output.Dispose();
        }


        public void WriteLine(string value)
        {
            if (wantSeparator)
            {
                output.WriteLine();
                wantSeparator = false;
            }

            output.Write(new string(' ', indentation * TabSize));

            output.WriteLine(value);
        }


        public void WriteLine(string format, params object[] args)
        {
            WriteLine(string.Format(format, args));
        }


        // Inserts a blank line between two blocks of code, but lazily so it can avoid writing
        // blanks if there isn't actually any more code output after this gets called.
        public void WriteSeparator()
        {
            wantSeparator = true;
        }


        // Placeholder comment to be picked up by the C# compiler /doc feature.
        public void WriteDocComment(string summary = null)
        {
            WriteLine("/// <summary>{0}</summary>", summary);
        }


        // Indents a block of code - C# 'using' statement ends the indentation.
        public IDisposable Indent()
        {
            indentation++;

            return new DisposeAction(() =>
            {
                indentation--;
                wantSeparator = false;
            });
        }


        // Writes indented code inside { and } braces - C# 'using' statement ends the block.
        public IDisposable WriteBraces()
        {
            WriteLine("{");

            var indentedBlock = Indent();

            return new DisposeAction(() =>
            {
                indentedBlock.Dispose();

                WriteLine("}");
            });
        }


        // Helper class calls a user specified action when it is disposed.
        class DisposeAction : IDisposable
        {
            Action disposeAction;

            public DisposeAction(Action disposeAction)
            {
                this.disposeAction = disposeAction;
            }

            public void Dispose()
            {
                disposeAction();
            }
        }
    }
}
