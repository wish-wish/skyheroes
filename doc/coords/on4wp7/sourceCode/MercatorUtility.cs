using System;
using System.Windows;
using System.Device.Location;

namespace Navi
{
    class MercatorUtility
    {
        public static Point LocationToLogicalPoint(GeoCoordinate location)
        {
            double num;
            if (location.Latitude > 85.051128)
            {
                num = 0.0;
            }
            else if (location.Latitude < -85.051128)
            {
                num = 1.0;
            }
            else
            {
                double num2 = Math.Sin((location.Latitude * 3.14159265358979323) / 180.0);
                num = 0.5 - (Math.Log((1.0 + num2) / (1.0 - num2)) / 12.56637061435917292);
            }
            return new Point((location.Longitude + 180.0) / 360.0, num);
        }

        public static GeoCoordinate LogicalPointToLocation(Point logicalPoint)
        {
            GeoCoordinate coordinate = new GeoCoordinate();
            coordinate.Latitude = (90.0 - ((360.0 * Math.Atan(Math.Exp(((logicalPoint.Y * 2.0) - 1.0) * 3.14159265358979323))) / 3.14159265358979323));
            coordinate.Longitude = (NormalizeLongitude((360.0 * logicalPoint.X) - 180.0));
            return coordinate;
        }

        public static double NormalizeLongitude(double longitude)
        {
            if ((longitude >= -180.0) && (longitude <= 180.0))
            {
                return longitude;
            }
            return (longitude - (Math.Floor((longitude + 180.0) / 360.0) * 360.0));
        }
    }
}
