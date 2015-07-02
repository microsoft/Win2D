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

using System.Collections.Generic;
using System.Linq;
using Windows.ApplicationModel.Background;
using Windows.Foundation;
using Windows.Storage;

namespace ExampleGallery.BackgroundTask
{
    //
    // This manages starting / stopping the live tile updating background task and 
    // extracting information about the current state of the tile.
    //
    public sealed class LiveTileUpdater
    {
        object lockable = new object();
        IBackgroundTaskRegistration backgroundTaskRegistration;

        public event BackgroundTaskCompletedEventHandler BackgroundTaskCompleted;
        public event TypedEventHandler<LiveTileUpdater, object> NewLiveTileGenerated;
        public string[] MostRecentTileImages { get; set; }

        static string TaskName = "Update Live Tile";
        static string LatestTileKey = "LatestTile";

        public LiveTileUpdater()
        {
            UpdateTask();
            UpdateTileImages();
        }

        public bool IsStarted
        {
            get
            {
                UpdateTask();
                return backgroundTaskRegistration != null;
            }

            set
            {
                if (value)
                    Start();
                else
                    Stop();
            }
        }

        void Start()
        {
            ScheduleTask();
            UpdateTask();
        }


        void Stop()
        {
            if (backgroundTaskRegistration != null)
            {
                backgroundTaskRegistration.Unregister(true);
            }
            UpdateTask();
        }

        void UpdateTask()
        {
            lock (lockable)
            {
                var task = FindTask();

                if (task == backgroundTaskRegistration)
                    return;

                if (backgroundTaskRegistration != null)
                    backgroundTaskRegistration.Completed -= OnCompleted;

                backgroundTaskRegistration = task;

                if (backgroundTaskRegistration != null)
                    backgroundTaskRegistration.Completed += OnCompleted;
            }
        }

        void UpdateTileImages()
        {
            string[] newImages = GetLatestTileImagesFromSettings();

            if (newImages == null)
                return;

            if (MostRecentTileImages != null && Enumerable.SequenceEqual(newImages, MostRecentTileImages))
                return;

            MostRecentTileImages = newImages;
            if (NewLiveTileGenerated != null)
                NewLiveTileGenerated(this, null);
        }

        void OnCompleted(BackgroundTaskRegistration sender, BackgroundTaskCompletedEventArgs args)
        {
            if (BackgroundTaskCompleted != null)
                BackgroundTaskCompleted(sender, args);

            UpdateTask();
            UpdateTileImages();
        }

        static string[] GetLatestTileImagesFromSettings()
        {
            object value;
            if (ApplicationData.Current.LocalSettings.Values.TryGetValue(LatestTileKey, out value))
            {
                return value as string[];
            }

            return null;
        }

        internal static void SetLatestTileImagesInSettings(IEnumerable<string> filenames)
        {
            ApplicationData.Current.LocalSettings.Values[LatestTileKey] = filenames.ToArray();
        }

        static IBackgroundTaskRegistration ScheduleTask()
        {
            var existingTask = FindTask();
            if (existingTask != null)
                return existingTask;

            var trigger = new TimeTrigger(15, false);

            var builder = new BackgroundTaskBuilder();
            builder.Name = TaskName;
            builder.TaskEntryPoint = typeof(LiveTileUpdaterTask).ToString();
            builder.SetTrigger(trigger);

            return builder.Register();
        }

        static IBackgroundTaskRegistration FindTask()
        {
            foreach (var entry in BackgroundTaskRegistration.AllTasks)
            {
                if (entry.Value.Name == TaskName)
                {
                    return entry.Value;
                }
            }

            return null;
        }
    }
}
