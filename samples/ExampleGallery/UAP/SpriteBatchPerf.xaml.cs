// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using ExampleGallery.Direct3DInterop.SpriteBatchPerformance;
using Microsoft.Graphics.Canvas;
using Microsoft.Graphics.Canvas.Text;
using Microsoft.Graphics.Canvas.UI.Xaml;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Linq;
using System.Numerics;
using System.Threading;
using System.Threading.Tasks;
using Windows.ApplicationModel;
using Windows.Foundation;
using Windows.UI;
using Windows.UI.Popups;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;

namespace ExampleGallery
{
    public sealed partial class SpriteBatchPerf : UserControl, INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;

        public enum GraphMode
        {
            DrawCount,
            BitmapCount,
        }

        public List<GraphMode> GraphModes { get { return Utils.GetEnumAsList<GraphMode>(); } }

        GraphMode currentGraphMode = GraphMode.DrawCount;

        public GraphMode CurrentGraphMode
        {
            get
            {
                return currentGraphMode;
            }
            set
            {
                if (currentGraphMode != value)
                {
                    ResetGraph();
                }
                currentGraphMode = value;
                if (PropertyChanged != null)
                {
                    PropertyChanged(this, new PropertyChangedEventArgs("IsDrawCountEnabled"));
                    PropertyChanged(this, new PropertyChangedEventArgs("IsBitmapCountEnabled"));
                }
            }
        }

        public bool IsDrawCountEnabled { get { return CurrentGraphMode == GraphMode.BitmapCount; } }
        public bool IsBitmapCountEnabled { get { return CurrentGraphMode == GraphMode.DrawCount; } }

        public int DrawCount { get; set; }
        public int BitmapCount { get; set; }

        public List<Scenario> Scenarios { get; private set; }

        // This is used when the page unloads to stop any pending profiling work.
        CancellationTokenSource currentProfilingTaskCancellation = new CancellationTokenSource();

        public SpriteBatchPerf()
        {
            Scenarios = new List<Scenario>();

            bool spriteBatchSupported = CanvasSpriteBatch.IsSupported(CanvasDevice.GetSharedDevice());

            Scenarios.Add(new Scenario(Scenario.DrawMethod.DrawImage, CanvasSpriteSortMode.None));

            if (spriteBatchSupported) 
            {
                Scenarios.Add(new Scenario(Scenario.DrawMethod.Win2DSpriteBatch, CanvasSpriteSortMode.None));
                Scenarios.Add(new Scenario(Scenario.DrawMethod.Win2DSpriteBatch, CanvasSpriteSortMode.Bitmap));
                Scenarios.Add(new Scenario(Scenario.DrawMethod.CppWin2DSpriteBatch, CanvasSpriteSortMode.None));
                Scenarios.Add(new Scenario(Scenario.DrawMethod.CppWin2DSpriteBatch, CanvasSpriteSortMode.Bitmap));
                Scenarios.Add(new Scenario(Scenario.DrawMethod.D2DSpriteBatch, CanvasSpriteSortMode.None));
            }

            DrawCount = 0;
            BitmapCount = 1;

            if (!DesignMode.DesignModeEnabled)
                DataContext = this;

            if (ThumbnailGenerator.IsDrawingThumbnail)
            {
                foreach (var scenario in Scenarios)
                {
                    scenario.PopulateWithThumbnailData();
                }
            }

            this.InitializeComponent();

            if (!spriteBatchSupported)
            {
                this.SpriteBatchNotSupportedText.Visibility = Visibility.Visible;
            }
        }

        void control_Unloaded(object sender, RoutedEventArgs e)
        {
            currentProfilingTaskCancellation.Cancel();

            // Explicitly remove references to allow the Win2D controls to get garbage collected.
            GraphCanvas.RemoveFromVisualTree();
            GraphCanvas = null;
        }

        async void OnGoButtonClicked(object sender, RoutedEventArgs e)
        {
            var button = (Button)sender;

            button.IsEnabled = false;

            try
            {
                await RunScenarios();
            }
            catch (TaskCanceledException)
            {
                // ignore
            }

            button.IsEnabled = true;
        }

        async Task RunScenarios()
        {
            var device = CanvasDevice.GetSharedDevice();

            int maxValue;

            switch (CurrentGraphMode)
            {
                case GraphMode.DrawCount:
                    maxValue = (int)DrawCountSlider.Maximum;
                    break;

                case GraphMode.BitmapCount:
                    maxValue = (int)BitmapCountSlider.Maximum;
                    break;

                default:
                    throw new Exception();
            }

            for (int value = 0; value <= maxValue; value += (maxValue / 100))
            {
                switch (CurrentGraphMode)
                {
                    case GraphMode.DrawCount:
                        DrawCount = value;
                        break;

                    case GraphMode.BitmapCount:
                        value = Math.Max(1, value);
                        BitmapCount = value;
                        break;
                }

                if (PropertyChanged != null)
                {
                    PropertyChanged(this, new PropertyChangedEventArgs("DrawCount"));
                    PropertyChanged(this, new PropertyChangedEventArgs("BitmapCount"));
                }

                using (var scenarioData = new ScenarioData(device, DrawCount, BitmapCount, value))
                {
                    foreach (var scenario in Scenarios)
                    {
                        await scenario.Go(currentProfilingTaskCancellation.Token, device, scenarioData);
                    }
                }

                GraphCanvas.Invalidate();
            }
        }

        void OnDrawGraph(CanvasControl sender, CanvasDrawEventArgs args)
        {
            var graphDrawer = new GraphDrawer((float)sender.ActualWidth, (float)sender.ActualHeight, Scenarios);
            graphDrawer.Draw(args.DrawingSession);
        }

        void OnResetGraphClicked(object sender, RoutedEventArgs e)
        {
            ResetGraph();
        }

        void ResetGraph()
        {
            foreach (var scenario in Scenarios)
            {
                scenario.Reset();
            }

            GraphCanvas.Invalidate();
        }


        public sealed class Scenario : INotifyPropertyChanged
        {
            public enum DrawMethod
            {
                Win2DSpriteBatch,
                CppWin2DSpriteBatch,
                D2DSpriteBatch,
                DrawImage
            }

            public DrawMethod Method { get; private set; }
            public CanvasSpriteSortMode SortMode { get; private set; }

            public CanvasImageSource Image { get; private set; }

            static Color[] colors =
            {
                Colors.Red,
                Colors.Green,
                Colors.Blue,
                Colors.DarkGoldenrod,
                Colors.Purple,
                Colors.DarkOliveGreen
            };

            public SolidColorBrush Color
            {
                get
                {
                    int index = (int)Method + (int)SortMode * Enum.GetValues(typeof(DrawMethod)).Length;
                    return new SolidColorBrush(colors[index % colors.Length]);
                }
            }

            public string Name
            {
                get
                {
                    string n;

                    switch (Method)
                    {
                        case DrawMethod.Win2DSpriteBatch:    n = "C# Win2D"; break;
                        case DrawMethod.CppWin2DSpriteBatch: n = "C++ Win2D"; break;
                        case DrawMethod.D2DSpriteBatch:      n = "C++ D2D"; break;
                        case DrawMethod.DrawImage:           n = "C# DrawImage"; break;
                        default: throw new Exception();
                    }

                    if (SortMode == CanvasSpriteSortMode.Bitmap)
                        n = n + " (sorted)";

                    return n;
                }
            }

            public string Summary
            {
                get
                {
                    if (Data == null || Data.Count == 0)
                        return "no data";

                    return string.Format("Average: {0:0.00}ms", Data.Values.Average());
                }
            }

            public Dictionary<int, double> Data { get; private set; }

            public event PropertyChangedEventHandler PropertyChanged;

            public Scenario(DrawMethod method, CanvasSpriteSortMode sortMode)
            {
                Method = method;
                SortMode = sortMode;
                Reset();
            }

            IScenarioRunner MakeScenarioRunner()
            {
                switch (Method)
                {
                    case DrawMethod.Win2DSpriteBatch:
                    case DrawMethod.DrawImage:
                        return new Win2DScenarioRunner(Method);

                    case DrawMethod.CppWin2DSpriteBatch:
                        return new CppWin2DScenarioRunner();

                    case DrawMethod.D2DSpriteBatch:
                        return new Direct2DScenarioRunner();

                    default:
                        throw new Exception();
                }
            }

            internal async Task Go(CancellationToken cancellationToken, CanvasDevice device, ScenarioData scenarioData)
            {
                var runner = MakeScenarioRunner();
                scenarioData.AddDataToScenarioRunner(runner);

                // Create the image source and the render target.  These sizes are hardcoded and independent of the 
                // display's DPI since we just want a small image to convince ourselves that the scenarios really are 
                // rendering the right thing.
                Image = new CanvasImageSource(device, 128, 128, 96);

                using (var rt = new CanvasRenderTarget(device, 128, 128, 96))
                {
                    // We actually run the scenario on the thread pool - XAML really falls down if you overload the UI thread.
                    var timeInMs = await Task.Run(() =>
                    {
                        // Run the scenario multiple times to try and avoid too much noise
                        List<double> times = new List<double>();

                        for (int i = 0; i < 4; ++i)
                        {
                            times.Add(RunScenario(runner, rt));
                            if (cancellationToken.IsCancellationRequested)
                                return 0;
                        }

                        // Drop the slowest and fastest times and take the average
                        times.Sort();
                        return times.GetRange(1, times.Count - 2).Average();
                    }, cancellationToken);

                    Data[scenarioData.Value] = timeInMs;

                    if (cancellationToken.IsCancellationRequested)
                        return;

                    using (var ds = Image.CreateDrawingSession(Colors.Transparent))
                    {
                        ds.DrawImage(rt);

                        var timing = string.Format("{0:0.00}ms", timeInMs);
                        ds.DrawText(timing, 0, 0, Colors.White);
                        ds.DrawText(timing, 1, 1, Colors.Black);
                    }
                }

                if (PropertyChanged != null)
                {
                    PropertyChanged(this, new PropertyChangedEventArgs("Image"));
                    PropertyChanged(this, new PropertyChangedEventArgs("Summary"));
                }
            }           

            double RunScenario(IScenarioRunner runner, CanvasRenderTarget rt)
            {
                Stopwatch stopWatch = new Stopwatch();
                stopWatch.Start();

                using (var ds = rt.CreateDrawingSession())
                {
                    ds.Clear(Colors.Black);
                    runner.RunScenario(ds, SortMode);
                }

                // This copies the pixels from the rendertarget - doing this forces the 
                // GPU to finish drawing, and so allows us to account more accurately for this time.
                var forceGpuToComplete = rt.GetPixelBytes();

                stopWatch.Stop();
                return stopWatch.Elapsed.TotalMilliseconds;
            }

            public void Reset()
            {
                Data = new Dictionary<int, double>();
                if (PropertyChanged != null)
                    PropertyChanged(this, new PropertyChangedEventArgs("Summary"));
            }

            public void PopulateWithThumbnailData()
            {
                Reset();

                int seed = (int)Method + (int)SortMode * Enum.GetValues(typeof(DrawMethod)).Length;

                var rnd = new Random(seed);

                double scale = rnd.NextDouble() * 30;
                double jitter = 10;

                for (int i = 0; i < 100; ++i)
                {
                    Data.Add(i * 100, (i / 100.0) * scale + rnd.NextDouble() * jitter);
                }
            }
        }


        class Win2DScenarioRunner : IScenarioRunner
        {
            private Scenario.DrawMethod method;

            List<ScenarioData.DrawInfo> sprites = new List<ScenarioData.DrawInfo>();         

            public Win2DScenarioRunner(Scenario.DrawMethod method)
            {
                this.method = method;
            }

            public void AddSprite(CanvasBitmap bitmap, Vector4 tint, Vector2 position, float rotation)
            {
                sprites.Add(new ScenarioData.DrawInfo()
                {
                    Bitmap = bitmap,
                    Tint = tint,
                    Position = position,
                    Rotation = rotation
                });
            }

            public void RunScenario(CanvasDrawingSession drawingSession, CanvasSpriteSortMode sortMode)
            {
                switch (method)
                {
                    case Scenario.DrawMethod.Win2DSpriteBatch:
                        using (var sb = drawingSession.CreateSpriteBatch(sortMode))
                        {
                            foreach (var sprite in sprites)
                            {
                                sb.Draw(sprite.Bitmap, sprite.Position, sprite.Tint, Vector2.Zero, sprite.Rotation, Vector2.One, CanvasSpriteFlip.None);
                            }
                        }
                        break;

                    case Scenario.DrawMethod.DrawImage:
                        var oldTransform = drawingSession.Transform;
                        foreach (var sprite in sprites)
                        {
                            drawingSession.Transform = Matrix3x2.CreateRotation(sprite.Rotation) * Matrix3x2.CreateTranslation(sprite.Position);
                            drawingSession.DrawImage(sprite.Bitmap, Vector2.Zero, sprite.Bitmap.Bounds, sprite.Tint.W);
                        }
                        drawingSession.Transform = oldTransform;
                        break;
                }
            }
        }


        class GraphDrawer
        {
            float actualHeight;
            float actualWidth;
            List<Scenario> scenarios;

            float maxXValue;
            float maxYValue;

            Vector2 origin;
            Vector2 xAxisEnd;
            Vector2 yAxisEnd;

            public GraphDrawer(float actualWidth, float actualHeight, List<Scenario> scenarios)
            {
                this.actualWidth = actualWidth;
                this.actualHeight = actualHeight;
                this.scenarios = scenarios;

                maxXValue = (float)(Math.Ceiling(FindMaxCount() / 100) * 100); // round to next 100
                maxYValue = (float)(Math.Ceiling(FindMaxTime() / 10) * 10); // round to next 10
            }

            CanvasTextFormat axisFormat = new CanvasTextFormat()
            {
                FontSize = 12,
                HorizontalAlignment = CanvasHorizontalAlignment.Right,
                VerticalAlignment = CanvasVerticalAlignment.Center
            };

            public void Draw(CanvasDrawingSession ds)
            {
                var widestLayoutRect = new CanvasTextLayout(ds, string.Format("{0}\t", maxYValue.ToString()), axisFormat, 500, 100).LayoutBounds;

                origin = new Vector2((float)widestLayoutRect.Width, actualHeight - (float)widestLayoutRect.Height);
                yAxisEnd = new Vector2(origin.X, (float)widestLayoutRect.Height);
                xAxisEnd = new Vector2(actualWidth, origin.Y);

                foreach (var scenario in scenarios)
                {
                    DrawSeries(ds, scenario);
                }

                DrawAxes(ds);
            }

            private void DrawSeries(CanvasDrawingSession ds, Scenario scenario)
            {
                var data = scenario.Data;

                var lastPoint = origin;
                var color = scenario.Color.Color;

                foreach (var key in data.Keys.OrderBy(k => k))
                {
                    var point = new Vector2(GetX(key), GetY((float)data[key]));

                    ds.DrawLine(lastPoint, point, color, 2);

                    lastPoint = point;
                }
            }

            private void DrawAxes(CanvasDrawingSession ds)
            {
                ds.DrawLine(origin, xAxisEnd, Colors.White, 1);
                ds.DrawLine(origin, yAxisEnd, Colors.White, 1);

                for (int i = 0; i <= 9; ++i)
                {
                    float y = (maxYValue / 9.0f) * (float)i;

                    ds.DrawText(string.Format("{0}", (int)y), new Vector2(origin.X - 5, GetY(y)), Colors.White, axisFormat);
                }

                axisFormat.VerticalAlignment = CanvasVerticalAlignment.Top;

                for (int i = 0; i <= 9; ++i)
                {
                    float x = (maxXValue / 9.0f) * (float)i;

                    ds.DrawText(string.Format("{0}", (int)x), new Vector2(GetX(x), origin.Y), Colors.White, axisFormat);
                }
            }

            double FindMaxTime()
            {
                double maxTime = 0;

                foreach (var scenario in scenarios)
                {
                    if (scenario.Data.Count > 0)
                        maxTime = Math.Max(maxTime, scenario.Data.Values.Max());
                }

                return maxTime;
            }

            float FindMaxCount()
            {
                float maxCount = 0;

                foreach (var scenario in scenarios)
                {
                    if (scenario.Data.Count > 0)
                        maxCount = Math.Max(maxCount, scenario.Data.Keys.Max());
                }

                return maxCount;
            }

            float GetY(float value)
            {
                return Vector2.Lerp(origin, yAxisEnd, (value / maxYValue)).Y;
            }

            float GetX(float value)
            {
                return Vector2.Lerp(origin, xAxisEnd, (value / maxXValue)).X;
            }
        }


        internal class ScenarioData : IDisposable
        {
            public int DrawCount { get; private set; }
            public int BitmapCount { get; private set; }
            public int Value { get; private set; }

            public struct DrawInfo
            {
                public CanvasBitmap Bitmap;
                public Vector4 Tint;
                public Vector2 Position;
                public float Rotation;
            }

            List<DrawInfo> drawInfo;

            public ScenarioData(CanvasDevice device, int drawCount, int bitmapCount, int value)
            {
                DrawCount = drawCount;
                BitmapCount = bitmapCount;
                Value = value;

                var bitmaps = new List<CanvasBitmap>();
                for (int i = 0; i < bitmapCount; ++i)
                {
                    var bitmap = new CanvasRenderTarget(device, 64, 64, 96);
                    using (var ds = bitmap.CreateDrawingSession())
                    {
                        ds.FillCircle(32, 32, 32, Colors.White);
                        ds.DrawText(i.ToString(), new Rect(0, 0, 64, 64), Colors.Black,
                            new CanvasTextFormat()
                            {
                                FontFamily = "Comic Sans MS",
                                HorizontalAlignment = CanvasHorizontalAlignment.Center,
                                VerticalAlignment = CanvasVerticalAlignment.Center
                            });
                    }
                    bitmaps.Add(bitmap);
                }

                var rng = new Random();

                drawInfo = new List<DrawInfo>();
                for (int i = 0; i < drawCount; ++i)
                {
                    drawInfo.Add(new DrawInfo()
                    {
                        Bitmap = bitmaps[i % bitmaps.Count],
                        Position = new Vector2(64.0f + (float)Math.Sin(i * 0.1) * 64, 64.0f + (float)Math.Cos(i * 0.1) * 64),
                        Rotation = (float)(i * 0.3),
                        Tint = new Vector4((float)rng.NextDouble(), (float)rng.NextDouble(), (float)rng.NextDouble(), (float)rng.NextDouble())
                    });
                }
            }

            public void Dispose()
            {
                foreach (var d in drawInfo)
                {
                    d.Bitmap.Dispose();
                }
            }

            public void AddDataToScenarioRunner(IScenarioRunner runner)
            {
                foreach (var d in drawInfo)
                {
                    runner.AddSprite(d.Bitmap, d.Tint, d.Position, d.Rotation);
                }
            }
        }
    }
}
