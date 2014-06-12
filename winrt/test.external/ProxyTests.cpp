#include "pch.h"

using namespace Microsoft::WRL::Wrappers;
using namespace ABI::Microsoft::Graphics::Canvas;

class TestResourceImpl : public RuntimeClass<ICanvasResource>
{
public:

    IFACEMETHOD(get_Device)(_In_ ICanvasDevice** value)
    {
        Assert::Fail(L"Unexpected call to get_Device");
        return E_NOTIMPL;
    }
};

TEST_CLASS(ProxyTests)
{
public:

    //
    // This exercises the generated proxy/stub for one of our interfaces to make
    // sure they're getting linked properly.
    //
    // Note that in order for this to work the interfaces and proxies need to be
    // listed in the Package.appxmanifest.  It is unclear if this is necessary
    // in practice since we expect all of our types to be free-threaded and
    // inproc.  
    //
    // This test exercises everything end-to-end and so if it does turn
    // out that we need proxies/stubs then we have them on hand.
    //
    TEST_METHOD(MarshalInterfaceTest)
    {
        RoInitializeWrapper init(RO_INIT_MULTITHREADED);

        //
        // Create a memory stream; this will have the interface explicitly
        // marshaled to it.
        //
        ComPtr<IStream> stream;
        ThrowIfFailed(CreateStreamOnHGlobal(NULL, TRUE, &stream));

        //
        // Now create an instance of something that implements the interface
        // we're testing.  Explicitly marshal this as out-of-proc (MSHCTX_LOCAL)
        // to ensure that we use standard marshaling (ie no FTM).
        //
        {
            ComPtr<ICanvasResource> original;
            ThrowIfFailed(MakeAndInitialize<TestResourceImpl>(&original));

            ThrowIfFailed(CoMarshalInterface(stream.Get(), __uuidof(ICanvasResource), original.Get(), MSHCTX_LOCAL, nullptr, MSHLFLAGS_NORMAL));
        }

        //
        // Rewind the stream so that unmarshal reads from the beginning.
        //
        LARGE_INTEGER zero = {};

        ThrowIfFailed(stream->Seek(zero, SEEK_SET, nullptr));

        //
        // Now attempt to unmarshal.
        //
        ComPtr<ICanvasResource> unmarshaled;
        ThrowIfFailed(CoGetInterfaceAndReleaseStream(stream.Detach(), IID_PPV_ARGS(&unmarshaled)));

        //
        // No errors is good enough for success here.  REGDB_E_CLASSNOTREG
        // indicates that we're missing entries in the Appxmanifest.xml.
        // E_NOINTERFACE indicates that we have a proxy class registered but,
        // for some reason, it doesn't know how to handle our interface.
        //
    }
};
