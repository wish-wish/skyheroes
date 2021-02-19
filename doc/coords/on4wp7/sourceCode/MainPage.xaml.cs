using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using Microsoft.Phone.Controls;
using System.IO.IsolatedStorage;
using System.IO;
using System.Device;
using System.Device.Location;
using System.Threading;
using System.Windows.Media.Imaging;
using System.Windows.Markup;
using System.Windows.Data;

namespace Navi
{
    public partial class MainPage : PhoneApplicationPage
    {
        public static GeoCoordinate geoPosition = GeoCoordinate.Unknown;
        public static GeoPositionStatus geoStatus = GeoPositionStatus.Disabled;
        public static GeoCoordinate mapCenter;

        const int WIDTH = 480;
        const int HEIGHT = 607;

        GeoCoordinateWatcher geoWatcher;
        Timer refreshTimer;
        GeoCoordinate geoCenter = new GeoCoordinate(39.0, 116.0);
        int mapCenterX = -1, mapCenterY = -1;
        int zoomLevel = 8;
        bool refreshing = false;
        bool followGPS = true;

        // Constructor
        public MainPage()
        {
            InitializeComponent();
            load_data();

            bool hd = false;
            if (IsolatedStorageSettings.ApplicationSettings.TryGetValue<bool>("HighAccuracy", out hd) && hd)
                geoWatcher = new GeoCoordinateWatcher(GeoPositionAccuracy.High);
            else
                geoWatcher = new GeoCoordinateWatcher(GeoPositionAccuracy.Default);
            geoWatcher.PositionChanged += new EventHandler<GeoPositionChangedEventArgs<GeoCoordinate>>(geoWatcher_PositionChanged);
            geoWatcher.StatusChanged += new EventHandler<GeoPositionStatusChangedEventArgs>(geoWatcher_StatusChanged);
            geoWatcher.MovementThreshold = 20.0;
            geoWatcher.Start();

            UpdateLayout();
            refreshTimer = new Timer(new TimerCallback(tick_refresh), null, 0, 100);
        }

        private void mapImage_ManipulationDelta(object sender, ManipulationDeltaEventArgs e)
        {
            followGPS = false;
            var p = MercatorUtility.LocationToLogicalPoint(geoCenter);
            double length = Math.Pow(2.0, zoomLevel) * 256.0;
            double x = p.X * length - e.DeltaManipulation.Translation.X;
            double y = p.Y * length - e.DeltaManipulation.Translation.Y;
            p.X = x / length;
            p.Y = y / length;
            geoCenter = MercatorUtility.LogicalPointToLocation(p);
        }

        private void mapImage_DoubleTap(object sender, GestureEventArgs e)
        {
            var p = e.GetPosition(mapImage);
            Point pp = new Point();
            double length = Math.Pow(2.0, zoomLevel) * 256.0;
            pp.X = (mapCenterX - (WIDTH / 2) + p.X) / length;
            pp.Y = (mapCenterY - (HEIGHT / 2) + p.Y) / length;
            if (zoomLevel >= 19)
                geoCenter = MercatorUtility.LogicalPointToLocation(pp);
            else
            {
                pp.X += (WIDTH / 2 - p.X) / length / 2.0;
                pp.Y += (HEIGHT / 2 - p.Y) / length / 2.0;
                geoCenter = MercatorUtility.LogicalPointToLocation(pp);
                zoomLevel++;
            }
        }

        private void buttonPlus_Click(object sender, RoutedEventArgs e)
        {
            if (zoomLevel < 19)
                zoomLevel++;
        }

        private void buttonMinus_Click(object sender, RoutedEventArgs e)
        {
            if (zoomLevel > 1)
                zoomLevel--;
        }

        void follow()
        {
            if (geoStatus == GeoPositionStatus.Ready)
            {
                double latitude, longitude;
                EvilTransform.transform(geoPosition.Latitude, geoPosition.Longitude, out latitude, out longitude);
                geoCenter = new GeoCoordinate(latitude, longitude);
            }
        }

        private void buttonFocus_Click(object sender, RoutedEventArgs e)
        {
            follow();
            followGPS = true;
        }

        private void buttonMore_Click(object sender, RoutedEventArgs e)
        {
            mapCenter = geoCenter;
            NavigationService.Navigate(new Uri("/Config.xaml", UriKind.Relative));
        }

        void geoWatcher_StatusChanged(object sender, GeoPositionStatusChangedEventArgs e)
        {
            geoStatus = e.Status;
            switch (e.Status)
            {
                case GeoPositionStatus.Disabled:
                    ApplicationTitle.Text = "Navigation (GPS Disabled)";
                    break;
                case GeoPositionStatus.Initializing:
                    ApplicationTitle.Text = "Navigation (GPS Initializing)";
                    break;
                case GeoPositionStatus.NoData:
                    ApplicationTitle.Text = "Navigation (GPS No Data)";
                    break;
                case GeoPositionStatus.Ready:
                    ApplicationTitle.Text = "Navigation (GPS Ready)";
                    break;
            }
        }

        void geoWatcher_PositionChanged(object sender, GeoPositionChangedEventArgs<GeoCoordinate> e)
        {
            geoPosition = e.Position.Location;
            if (followGPS) follow();
        }

        void tick_refresh(object state)
        {
            if (refreshing) return;
            refreshing = true;

            var p = MercatorUtility.LocationToLogicalPoint(geoCenter);
            double length = Math.Pow(2.0, (double)zoomLevel);
            int centerX = (int)Math.Floor(p.X * length * 256.0);
            int centerY = (int)Math.Floor(p.Y * length * 256.0);
            if (centerX == mapCenterX && centerY == mapCenterY)
            {
                refreshing = false;
                return;
            }

            mapCenterX = centerX;
            mapCenterY = centerY;
            int tileX = (int)Math.Floor(p.X * length);
            int tileY = (int)Math.Floor(p.Y * length);
            int offX = centerX - 256 * tileX;
            int offY = centerY - 256 * tileY;

            Dispatcher.BeginInvoke(delegate()
            {
                WriteableBitmap map = new WriteableBitmap(WIDTH, HEIGHT);

                int i = 0, j = 0;
                while (i < HEIGHT)
                {
                    int ox = j - WIDTH / 2 + offX;
                    int oy = i - HEIGHT / 2 + offY;
                    int tx = ox >> 8, ty = oy >> 8;
                    int lx = WIDTH / 2 - offX + tx * 256;
                    int ly = HEIGHT / 2 - offY + ty * 256;
                    if (lx < 0) lx = 256 + lx;
                    else lx = 256;
                    if (ly < 0) ly = 256 + ly;
                    else ly = 256;

                    WriteableBitmap img = new WriteableBitmap(getTile(tileX + tx, tileY + ty, zoomLevel));
                    for (int r = 0; r < 256; ++r)
                    {
                        int y = HEIGHT / 2 - offY + ty * 256 + r;
                        if (y < 0)
                            continue;
                        if (y >= HEIGHT)
                            break;
                        for (int c = 0; c < 256; ++c)
                        {
                            int x = WIDTH / 2 - offX + tx * 256 + c;
                            if (x < 0)
                                continue;
                            if (x >= WIDTH)
                                break;
                            map.Pixels[x + y * WIDTH] = img.Pixels[c + r * 256];
                        }
                    }

                    j += lx;
                    if (j >= WIDTH)
                    {
                        j = 0;
                        i += ly;
                    }
                }

                if (geoStatus == GeoPositionStatus.Ready)
                {
                    double latitude = 0, longitude = 0;
                    EvilTransform.transform(geoPosition.Latitude, geoPosition.Longitude, out latitude, out longitude);
                    GeoCoordinate geo = new GeoCoordinate(latitude, longitude);
                    var pos = MercatorUtility.LocationToLogicalPoint(geo);
                    int dx = (int)Math.Floor(pos.X * length * 256.0) - centerX + WIDTH / 2;
                    int dy = (int)Math.Floor(pos.Y * length * 256.0) - centerY + HEIGHT / 2;
                    for (int r = -7; r <= 7; ++r)
                    {
                        int y = dy + r;
                        if (y < 0) continue;
                        if (y >= HEIGHT) break;
                        for (int c = -7; c <= 7; ++c)
                        {
                            int x = dx + c;
                            if (x < 0) continue;
                            if (x >= WIDTH) break;
                            int s = r * r + c * c;
                            if (s < 49)
                                map.Pixels[x + y * WIDTH] = -1;
                            if (s < 25)
                                map.Pixels[x + y * WIDTH] = -15695651;
                        }
                    }
                }

                mapImage.Source = map;
                refreshing = false;
            });
        }

        struct ImageRange
        {
            public int begin;
            public int length;
        };
        static Stream imgStream = null;
        static Dictionary<string, ImageRange> dictIR = null;
        static WriteableBitmap blankImage = null;
        static LRUCache imageCache = new LRUCache(512);

        static BitmapSource getTile(int tileX, int tileY, int zoomLevel)
        {
            if (blankImage == null)
            {
                blankImage = new WriteableBitmap(256, 256);
                for (int i = 0; i < 256; ++i)
                    for (int j = 0; j < 256; ++j)
                        blankImage.Pixels[i * 256 + j] = 0;
            }

            string key = string.Format("{0}~{1}~{2}", tileX, tileY, zoomLevel);
            ImageRange range;
            if (!dictIR.TryGetValue(key, out range))
            {
                return blankImage;
            }

            BitmapSource ret = imageCache.get(key);
            if (ret != null)
                return ret;

            ret = new BitmapImage();
            try
            {
                ret.SetSource(new RangeStream(imgStream, range.begin, range.length));
                imageCache.add(key, ret);
            }
            catch
            {
                return blankImage;
            }
            return ret;
        }

        void load_data()
        {
            if (dictIR != null)
                return;
            IsolatedStorageFile isf = IsolatedStorageFile.GetUserStoreForApplication();
            dictIR = new Dictionary<string, ImageRange>();

            if (!isf.FileExists("GOMapData"))
                return;
            imgStream = isf.OpenFile("GOMapData", FileMode.Open, FileAccess.Read);

            BinaryReader r = new BinaryReader(imgStream);
            int count = r.ReadInt32();
            for (int i = 0; i < count; ++i)
            {
                int x = r.ReadInt32();
                int y = r.ReadInt32();
                int z = r.ReadInt32();
                string key = string.Format("{0}~{1}~{2}", x, y, z);
                ImageRange val = new ImageRange();
                val.begin = r.ReadInt32();
                val.length = r.ReadInt32();
                dictIR.Add(key, val);
            }
        }
    }

}