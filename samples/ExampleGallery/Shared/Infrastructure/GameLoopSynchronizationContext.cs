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

using Microsoft.Graphics.Canvas.UI.Xaml;
using System;
using System.Threading;
using System.Threading.Tasks;

namespace ExampleGallery
{
    // Pop quiz: what does this code do?
    //
    //      await animatedControl.RunOnGameLoopThreadAsync(async () =>
    //      {
    //          Debug.WriteLine("a");
    //          await Task.Delay(1000);
    //          Debug.WriteLine("b");
    //      });
    //      Debug.WriteLine("c");
    //
    // If you guessed that it prints "a", "b", "c", you'd be wrong.
    // In fact, the output is "a", "c", "b".
    //
    // You may also be surprised to learn that the Debug.WriteLine call which prints "b"
    // will run on an arbitrary threadpool thread, not the game loop thread at all!
    //
    // What gives?
    //
    // CanvasAnimatedControl.RunOnGameLoopThreadAsync runs a single atomic operation on
    // the game loop thread. But when you pass it an async lambda, this is actually a
    // chain of continuation methods:
    //
    //  - The part up to the await Task.Delay (which prints "a") runs on the game loop thread
    //  - It registers the second part of the lambda to run later as an async continuation
    //  - It exits
    //  - At this point the task returned by RunOnGameLoopThreadAsync is signalled as complete
    //  - This unblocks the calling thread to complete its outer await and print "c"
    //  - Later, when Task.Delay completes, the await continuation is invoked
    //  - Because the game loop thread does not have a specialized SynchronizationContext,
    //    Debug.WriteLine("b") defaults to running on the threadpool.
    //
    // To run async methods which contain await operations on the game loop thread and have
    // them work the way we'd expect, we must provide our own SynchronizationContext that
    // knows how to invoke each async continuation back to the proper thread. This must also
    // take care to return a task that will complete only when the entire async chain has
    // finished, rather than as soon as the first await is reached.
    //
    // This helper allows us to write:
    //
    //      await GameLoopSynchronizationContext.RunOnGameLoopThreadAsync(animatedControl, async () =>
    //      {
    //          Debug.WriteLine("a");
    //          await Task.Delay(1000);
    //          Debug.WriteLine("b");
    //      });
    //      Debug.WriteLine("c");
    //
    // The output is "a", "b", "c", with both "a" and "b" written from the game loop thread.
    
    class GameLoopSynchronizationContext : SynchronizationContext
    {
        ICanvasAnimatedControl control;


        // Constructor.
        public GameLoopSynchronizationContext(ICanvasAnimatedControl control)
        {
            this.control = control;
        }


        // Posts a single atomic action for asynchronous execution on the game loop thread.
        public override void Post(SendOrPostCallback callback, object state)
        {
            var action = control.RunOnGameLoopThreadAsync(() =>
            {
                // Re-register ourselves as the current synchronization context,
                // to work around CLR issues where this state can sometimes get nulled out.
                SynchronizationContext.SetSynchronizationContext(this);
                    
                callback(state);
            });
        }


        // Runs an action, which could contain an arbitrarily complex chain of async awaits,
        // on the game loop thread. This helper registers a custom synchronization context
        // to make sure every await continuation in the chain remains on the game loop
        // thread, regardless of which thread the lower level async operations complete on.
        // It wraps the entire chain with a TaskCompletionSource in order to return a single
        // Task that will be signalled only when the whole chain has completed.
        public static async Task RunOnGameLoopThreadAsync(ICanvasAnimatedControl control, Func<Task> callback)
        {
            var completedSignal = new TaskCompletionSource<object>();

            await control.RunOnGameLoopThreadAsync(async () =>
            {
                try
                {
                    SynchronizationContext.SetSynchronizationContext(new GameLoopSynchronizationContext(control));

                    await callback();

                    completedSignal.SetResult(null);
                }
                catch (Exception e)
                {
                    completedSignal.SetException(e);
                }
            });

            await completedSignal.Task;
        }
    };
}
