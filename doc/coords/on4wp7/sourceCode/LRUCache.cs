using System;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using System.Collections.Generic;
using System.Windows.Media.Imaging;

namespace Navi
{
    public class LRUCache
    {
        Dictionary<string, long> use;
        Dictionary<string, BitmapSource> map;
        int size;
        long clk = 1;

        public LRUCache(int size)
        {
            use = new Dictionary<string, long>();
            map = new Dictionary<string, BitmapSource>();
            this.size = size;
        }

        public void add(string key, BitmapSource val)
        {
            use.Add(key, clk);
            clk++;
            map.Add(key, val);
            if (map.Count >= size)
                cleanup();
        }

        public BitmapSource get(string key)
        {
            BitmapSource src;
            try
            {
                src = map[key];
                use[key] = clk;
                clk++;
            }
            catch
            {
                return null;
            }
            return src;
        }

        void cleanup()
        {
            var v = use.Values;
            long[] array = new long[v.Count];
            v.CopyTo(array, 0);
            Array.Sort<long>(array);
            long mid = array[array.Length / 2];
            List<string> rec = new List<string>();
            foreach (var x in use)
            {
                if (x.Value > mid) continue;
                rec.Add(x.Key);
            }
            foreach (var x in rec)
            {
                use.Remove(x);
                map.Remove(x);
            }
        }
    }
}
