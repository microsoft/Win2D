// Copyright (c) Microsoft Corporation.  All rights reserved

#include "pch.h"

using namespace Microsoft::DirectX;

TEST_CLASS(GraphicsErrorTests)
{
public:
    TEST_METHOD(GraphicsErrorCodeMatchDXGICodes)
    {
#define CHECK(WINRT, HR) Assert::AreEqual(static_cast<HRESULT>(GraphicsErrorCode::WINRT), HR)

        CHECK(DeviceHung                              , DXGI_ERROR_DEVICE_HUNG);
        CHECK(DeviceRemoved                           , DXGI_ERROR_DEVICE_REMOVED);
        CHECK(DeviceReset                             , DXGI_ERROR_DEVICE_RESET);
        CHECK(DriverInternalError                     , DXGI_ERROR_DRIVER_INTERNAL_ERROR);
        CHECK(FrameStatisticsDisjoint                 , DXGI_ERROR_FRAME_STATISTICS_DISJOINT);
        CHECK(GraphicsVidpnSourceInUse                , DXGI_ERROR_GRAPHICS_VIDPN_SOURCE_IN_USE);
        CHECK(InvalidCall                             , DXGI_ERROR_INVALID_CALL);
        CHECK(MoreData                                , DXGI_ERROR_MORE_DATA);
        CHECK(Nonexclusive                            , DXGI_ERROR_NONEXCLUSIVE);
        CHECK(NotCurrentlyAvailable                   , DXGI_ERROR_NOT_CURRENTLY_AVAILABLE);
        CHECK(NotFound                                , DXGI_ERROR_NOT_FOUND);
        CHECK(RemoteClientDisconnected                , DXGI_ERROR_REMOTE_CLIENT_DISCONNECTED);
        CHECK(RemoteOutOfMemory                       , DXGI_ERROR_REMOTE_OUTOFMEMORY);
        CHECK(WasStillDrawing                         , DXGI_ERROR_WAS_STILL_DRAWING);
        CHECK(Unsupported                             , DXGI_ERROR_UNSUPPORTED);
        CHECK(AccessLost                              , DXGI_ERROR_ACCESS_LOST);
        CHECK(WaitTimeout                             , DXGI_ERROR_WAIT_TIMEOUT);
        CHECK(SessionDisconnected                     , DXGI_ERROR_SESSION_DISCONNECTED);
        CHECK(RestrictToOutputStale                   , DXGI_ERROR_RESTRICT_TO_OUTPUT_STALE);
        CHECK(CannotProtectContent                    , DXGI_ERROR_CANNOT_PROTECT_CONTENT);
        CHECK(AccessDenied                            , DXGI_ERROR_ACCESS_DENIED);
        CHECK(NameAlreadyExists                       , DXGI_ERROR_NAME_ALREADY_EXISTS);
        CHECK(TooManyUniqueStateObjects               , D3D11_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS);
        CHECK(FileNotFound                            , D3D11_ERROR_FILE_NOT_FOUND);
        CHECK(TooManyUniqueViewObjects                , D3D11_ERROR_TOO_MANY_UNIQUE_VIEW_OBJECTS);
        CHECK(DeferredContextMapWithoutInitialDiscard , D3D11_ERROR_DEFERRED_CONTEXT_MAP_WITHOUT_INITIAL_DISCARD);

#undef CHECK
    }
};

