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
using System.Threading;
using Microsoft.Phone.Controls;
using System.IO.IsolatedStorage;
using System.IO;
using System.Device.Location;

namespace Navi
{
    public partial class Config : PhoneApplicationPage
    {
        Timer timer;

        bool highAccuracy = false;

        public Config()
        {
            InitializeComponent();

            IsolatedStorageSettings.ApplicationSettings.TryGetValue<bool>("HighAccuracy", out highAccuracy);

            if (highAccuracy)
                btnAccuracy.Content = "高精度 --> 普通精度";
            else
                btnAccuracy.Content = "普通精度 --> 高精度";

            timer = new Timer(new TimerCallback(tick), null, 0, 1000);
        }

        void tick(object obj)
        {
            Dispatcher.BeginInvoke(delegate()
            {
                double latitude = 0, longitude = 0;
                EvilTransform.transform(MainPage.geoPosition.Latitude, MainPage.geoPosition.Longitude, out latitude, out longitude);
                gpsmsg.Text = string.Format("GPS 　纬度 {0}\n　　　经度 {1}\n" +
                    "　　　海拔 {6} 米\n　　　速度 {7} 米/秒\n" +
                    "　　　水平精确度 {8} 米\n　　　垂直精确度 {9} 米\n" +
                    "修正　纬度 {2}\n　　　经度 {3}\n" +
                    "地图　纬度 {4}\n　　　经度 {5}",
                        MainPage.geoPosition.Latitude, MainPage.geoPosition.Longitude,
                        latitude, longitude,
                        MainPage.mapCenter.Latitude, MainPage.mapCenter.Longitude,
                        MainPage.geoPosition.Altitude, MainPage.geoPosition.Speed,
                        MainPage.geoPosition.HorizontalAccuracy, MainPage.geoPosition.VerticalAccuracy);
            });
        }

        private void btnAccuracy_Click(object sender, RoutedEventArgs e)
        {
            highAccuracy = !highAccuracy;
            try
            {
                IsolatedStorageSettings.ApplicationSettings.Add("HighAccuracy", highAccuracy);
            }
            catch
            {
                IsolatedStorageSettings.ApplicationSettings["HighAccuracy"] = highAccuracy;
            }
            if (highAccuracy)
                btnAccuracy.Content = "高精度 --> 普通精度";
            else
                btnAccuracy.Content = "普通精度 --> 高精度";
        }

        private void btn_Click(object sender, RoutedEventArgs e)
        {
            double dlat, dlong;
            dlat = MainPage.mapCenter.Latitude - MainPage.geoPosition.Latitude;
            dlong = MainPage.mapCenter.Longitude - MainPage.geoPosition.Longitude;
            txtLatitude.Text = dlat.ToString();
            txtLongtitude.Text = dlong.ToString();
        }

        private void btnMore_Click(object sender, RoutedEventArgs e)
        {
            NavigationService.Navigate(new Uri("/Tools.xaml", UriKind.Relative));
        }
    }
}