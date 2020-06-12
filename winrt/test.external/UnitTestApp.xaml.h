// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "UnitTestApp.g.h"

namespace test
{
    namespace external
    {
        ref class App sealed
        {
        protected:
            virtual void OnLaunched(Microsoft::UI::Xaml::LaunchActivatedEventArgs^ e) override;

        internal:
            App();

        private:
            void OnSuspending(Platform::Object^ sender, Windows::ApplicationModel::SuspendingEventArgs^ e);
            void OnNavigationFailed(Platform::Object ^sender, Microsoft::UI::Xaml::Navigation::NavigationFailedEventArgs ^e);
        };
    }
}
