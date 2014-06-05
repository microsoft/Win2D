These are tests for the internals of winrt.lib.

This statically links with the library to it has direct access to everything
within it.

These are non-windows store tests.  This means that all dependencies on WinRT
APIs must use test doubles instead.


