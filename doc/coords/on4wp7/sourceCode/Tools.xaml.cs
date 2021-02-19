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
using Microsoft.Devices;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Media;

namespace Navi
{
    public partial class Tools : PhoneApplicationPage
    {
        public Tools()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, RoutedEventArgs e)
        {
            string bak = button1.Content.ToString();
            for (int i = 1; i < 30; ++i)
            {
                string str = i.ToString();
                button1.Content = str;
                button1.UpdateLayout();
                MediaHistoryItem item = new MediaHistoryItem
                {
                    ImageStream = Application.GetResourceStream(new Uri("Blank.png", UriKind.Relative)).Stream,
                    Source = "",
                    Title = str
                };
                item.PlayerContext.Add("keyString", str);
                MediaHistory.Instance.WriteRecentPlay(item);
            }
            button1.Content = bak;
        }

        private void button2_Click(object sender, RoutedEventArgs e)
        {
            Song song = Song.FromUri("blank", new Uri("Blank.mp3", UriKind.Relative));
            FrameworkDispatcher.Update();
            MediaPlayer.Play(song);
        }
    }
}