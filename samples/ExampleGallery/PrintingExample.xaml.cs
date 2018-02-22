// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using Microsoft.Graphics.Canvas;
using Microsoft.Graphics.Canvas.Printing;
using Microsoft.Graphics.Canvas.Text;
using System;
using System.Collections.Generic;
using System.Numerics;
using System.Threading.Tasks;
using Windows.ApplicationModel;
using Windows.Foundation;
using Windows.Graphics.Printing;
using Windows.Graphics.Printing.OptionDetails;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace ExampleGallery
{
    public sealed partial class PrintingExample : UserControl
    {
        CanvasPrintDocument printDocument;

        struct BitmapInfo
        {
            public string Name;
            public CanvasBitmap Bitmap;
        }

        List<BitmapInfo> bitmaps;
        Vector2 largestBitmap;

        Vector2 pageSize;
        Vector2 imagePadding = new Vector2(64, 64);
        Vector2 textPadding = new Vector2(8, 8);
        Vector2 cellSize;
        int bitmapCount;
        int columns;
        int rows;
        int bitmapsPerPage;
        int pageCount = -1;


        public PrintingExample()
        {
            this.InitializeComponent();
        }


        private void UserControl_Unloaded(object sender, RoutedEventArgs e)
        {
            if (printDocument != null)
            {
                printDocument.Dispose();
                printDocument = null;
            }
        }


        private async void OnPrintClicked(object sender, RoutedEventArgs e)
        {
            // Create a new CanvasPrintDocument
            if (printDocument != null)
            {
                printDocument.Dispose();
            }

            printDocument = new CanvasPrintDocument();
            printDocument.PrintTaskOptionsChanged += PrintDocument_PrintTaskOptionsChanged;
            printDocument.Preview += PrintDocument_Preview;
            printDocument.Print += PrintDocument_Print;

            // Show the print UI, with the print manager connected to us
            var printManager = PrintManager.GetForCurrentView();
            printManager.PrintTaskRequested += PrintingExample_PrintTaskRequested;
            try
            {
                await PrintManager.ShowPrintUIAsync();
            }
            finally
            {
                printManager.PrintTaskRequested -= PrintingExample_PrintTaskRequested;
            }
        }


        private void PrintingExample_PrintTaskRequested(PrintManager sender, PrintTaskRequestedEventArgs args)
        {
            var printTask = args.Request.CreatePrintTask("Win2D Printing Example", (createPrintTaskArgs) =>
            {
                createPrintTaskArgs.SetSource(printDocument);
            });

            var detailedOptions = PrintTaskOptionDetails.GetFromPrintTaskOptions(printTask.Options);
            var pageRange = detailedOptions.CreateItemListOption("PageRange", "Page Range");
            pageRange.AddItem("PrintAll", "Print All");
            pageRange.AddItem("PrintFirstPage", "Print Only First Page");

            var displayedOptions = printTask.Options.DisplayedOptions;
            displayedOptions.Clear();

            displayedOptions.Add(StandardPrintTaskOptions.MediaSize);
            displayedOptions.Add(StandardPrintTaskOptions.Orientation);
            displayedOptions.Add("PageRange");

            detailedOptions.OptionChanged += PrintDetailedOptions_OptionChanged;
        }

        private void PrintDetailedOptions_OptionChanged(PrintTaskOptionDetails sender, PrintTaskOptionChangedEventArgs args)
        {
            if (args.OptionId == null)
            {
                // Invalidate the preview when switching printers.
                this.printDocument.InvalidatePreview();
            }
        }

        private async void PrintDocument_PrintTaskOptionsChanged(CanvasPrintDocument sender, CanvasPrintTaskOptionsChangedEventArgs args)
        {
            var deferral = args.GetDeferral();

            try
            {
                await EnsureBitmapsLoaded(sender);

                var pageDesc = args.PrintTaskOptions.GetPageDescription(1);
                var newPageSize = pageDesc.PageSize.ToVector2();

                if (pageSize == newPageSize && pageCount != -1)
                {
                    // We've already figured out the pages and the page size hasn't changed, so there's nothing left for us to do here.
                    return;
                }

                pageSize = newPageSize;
                sender.InvalidatePreview();

                // Figure out the bitmap index at the top of the current preview page.  We'll request that the preview defaults to showing
                // the page that still has this bitmap on it in the new layout.
                int indexOnCurrentPage = 0;
                if (pageCount != -1)
                {
                    indexOnCurrentPage = (int)(args.CurrentPreviewPageNumber - 1) * bitmapsPerPage;
                }

                // Calculate the new layout
                var printablePageSize = pageSize * 0.9f;

                cellSize = largestBitmap + imagePadding;

                var cellsPerPage = printablePageSize / cellSize;

                columns = Math.Max(1, (int)Math.Floor(cellsPerPage.X));
                rows = Math.Max(1, (int)Math.Floor(cellsPerPage.Y));

                bitmapsPerPage = columns * rows;

                // Calculate the page count
                bitmapCount = bitmaps.Count;
                pageCount = (int)Math.Ceiling(bitmapCount / (double)bitmapsPerPage);
                sender.SetPageCount((uint)pageCount);

                // Set the preview page to the one that has the item that was currently displayed in the last preview
                args.NewPreviewPageNumber = (uint)(indexOnCurrentPage / bitmapsPerPage) + 1;
            }
            finally
            {
                deferral.Complete();
            }
        }


        private async Task EnsureBitmapsLoaded(CanvasPrintDocument sender)
        {
            if (bitmaps != null)
                return;

            bitmaps = new List<BitmapInfo>();

            // For this demo we use example gallery's thumbnails (since that's a convenient source of images for the demo)
            var thumbnailsFolder = await Package.Current.InstalledLocation.GetFolderAsync("Thumbnails");
            var files = await thumbnailsFolder.GetFilesAsync();

            foreach (var file in files)
            {
                if (file.Name.EndsWith("png"))
                {
                    var bitmap = await CanvasBitmap.LoadAsync(sender, await file.OpenReadAsync());
                    bitmaps.Add(new BitmapInfo
                    {
                        Name = file.DisplayName,
                        Bitmap = bitmap
                    });
                }
            }

            largestBitmap = Vector2.Zero;
            foreach (var bitmap in bitmaps)
            {
                largestBitmap.X = Math.Max(largestBitmap.X, (float)bitmap.Bitmap.Size.Width);
                largestBitmap.Y = Math.Max(largestBitmap.Y, (float)bitmap.Bitmap.Size.Height);
            }
        }


        private void PrintDocument_Preview(CanvasPrintDocument sender, CanvasPreviewEventArgs args)
        {
            var ds = args.DrawingSession;
            var pageNumber = args.PageNumber;
            var imageableRect = args.PrintTaskOptions.GetPageDescription(args.PageNumber).ImageableRect;

            DrawPage(sender, ds, pageNumber, imageableRect);
        }


        private void PrintDocument_Print(CanvasPrintDocument sender, CanvasPrintEventArgs args)
        {
            var detailedOptions = PrintTaskOptionDetails.GetFromPrintTaskOptions(args.PrintTaskOptions);
            var pageRange = detailedOptions.Options["PageRange"].Value.ToString();

            int pageCountToPrint;

            if (pageRange == "PrintFirstPage")
                pageCountToPrint = 1;
            else
                pageCountToPrint = pageCount;

            for (uint i = 1; i <= pageCountToPrint; ++i)
            {
                using (var ds = args.CreateDrawingSession())
                {
                    var imageableRect = args.PrintTaskOptions.GetPageDescription(i).ImageableRect;

                    DrawPage(sender, ds, i, imageableRect);
                }
            }
        }


        private void DrawPage(CanvasPrintDocument sender, CanvasDrawingSession ds, uint pageNumber, Rect imageableRect)
        {
            var cellAcross = new Vector2(cellSize.X, 0);
            var cellDown = new Vector2(0, cellSize.Y);

            var totalSize = cellAcross * columns + cellDown * rows;
            Vector2 topLeft = (pageSize - totalSize) / 2;

            int bitmapIndex = ((int)pageNumber - 1) * bitmapsPerPage;

            var labelFormat = new CanvasTextFormat()
            {
                FontFamily = "Comic Sans MS",
                FontSize = 12,
                VerticalAlignment = CanvasVerticalAlignment.Bottom,
                HorizontalAlignment = CanvasHorizontalAlignment.Left
            };

            var numberFormat = new CanvasTextFormat()
            {
                FontFamily = "Comic Sans MS",
                FontSize = 18,
                VerticalAlignment = CanvasVerticalAlignment.Top,
                HorizontalAlignment = CanvasHorizontalAlignment.Left
            };

            var pageNumberFormat = new CanvasTextFormat()
            {
                FontFamily = "Comic Sans MS",
                FontSize = 10,
                VerticalAlignment = CanvasVerticalAlignment.Bottom,
                HorizontalAlignment = CanvasHorizontalAlignment.Center
            };

            var titleFormat = new CanvasTextFormat()
            {
                FontFamily = "Comic Sans MS",
                FontSize = 24,
                VerticalAlignment = CanvasVerticalAlignment.Top,
                HorizontalAlignment = CanvasHorizontalAlignment.Left
            };


            if (pageNumber == 1)
                ds.DrawText("Win2D Printing Example", imageableRect, Colors.Black, titleFormat);

            ds.DrawText(string.Format("Page {0} / {1}", pageNumber, pageCount),
                imageableRect,
                Colors.Black,
                pageNumberFormat);

            DrawGrid(ds, cellAcross, cellDown, topLeft);

            for (int row = 0; row < rows; ++row)
            {
                for (int column = 0; column < columns; ++column)
                {
                    var cellTopLeft = topLeft + (cellAcross * column) + (cellDown * row);

                    var paddedTopLeft = cellTopLeft + textPadding / 2;
                    var paddedSize = cellSize - textPadding;

                    var bitmapInfo = bitmaps[bitmapIndex % bitmaps.Count];

                    // Center the bitmap in the cell
                    var bitmapPos = cellTopLeft + (cellSize - bitmapInfo.Bitmap.Size.ToVector2()) / 2;

                    ds.DrawImage(bitmapInfo.Bitmap, bitmapPos);

                    using (var labelLayout = new CanvasTextLayout(sender, bitmapInfo.Name, labelFormat, paddedSize.X, paddedSize.Y))
                    using (var numberLayout = new CanvasTextLayout(sender, (bitmapIndex + 1).ToString(), numberFormat, paddedSize.X, paddedSize.Y))
                    {
                        DrawTextOverWhiteRectangle(ds, paddedTopLeft, labelLayout);
                        DrawTextOverWhiteRectangle(ds, paddedTopLeft, numberLayout);
                    }

                    bitmapIndex++;
                }
            }
        }


        private void DrawGrid(CanvasDrawingSession ds, Vector2 cellAcross, Vector2 cellDown, Vector2 topLeft)
        {
            var gridTopLeft = topLeft;
            var gridBottomRight = topLeft + cellAcross * columns + cellDown * rows;

            ds.DrawRectangle(new Rect(gridTopLeft.ToPoint(), gridBottomRight.ToPoint()), Colors.Black, 1);

            for (int row = 0; row <= rows; ++row)
            {
                ds.DrawLine(topLeft + cellDown * row, topLeft + cellAcross * columns + cellDown * row, Colors.Black, 1);
            }

            for (int column = 0; column <= columns; ++column)
            {
                ds.DrawLine(topLeft + cellAcross * column, topLeft + cellAcross * column + cellDown * rows, Colors.Black, 1);
            }
        }


        private static void DrawTextOverWhiteRectangle(CanvasDrawingSession ds, Vector2 paddedTopLeft, CanvasTextLayout textLayout)
        {
            var bounds = textLayout.LayoutBounds;

            var rect = new Rect(bounds.Left + paddedTopLeft.X, bounds.Top + paddedTopLeft.Y, bounds.Width, bounds.Height);

            ds.FillRectangle(rect, Colors.White);
            ds.DrawTextLayout(textLayout, paddedTopLeft, Colors.Black);
        }
    }
}
