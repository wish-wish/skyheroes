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
using System.IO;

namespace Navi
{
    public class RangeStream : Stream
    {
        Stream stream;
        long begin;
        long length;
        long pos;

        public RangeStream(Stream stream, long begin, long length)
        {
            this.stream = stream;
            this.begin = begin;
            this.length = length;
            this.pos = 0;
        }

        public override void Flush() { }

        public override int Read(byte[] buffer, int offset, int count)
        {
            stream.Seek(begin + pos, SeekOrigin.Begin);
            count = stream.Read(buffer, offset, count);
            pos += count;
            return count;
        }

        public override long Seek(long offset, SeekOrigin origin)
        {
            switch (origin)
            {
                case SeekOrigin.Begin:
                    pos = offset;
                    break;
                case SeekOrigin.Current:
                    pos += offset;
                    break;
                case SeekOrigin.End:
                    pos = length;
                    break;
            }
            if (pos > length) pos = length;
            if (pos < 0) pos = 0;
            return pos;
        }

        public override void SetLength(long value) { }

        public override void Write(byte[] buffer, int offset, int count) { }

        public override bool CanRead { get { return true; } }

        public override bool CanSeek { get { return true; } }

        public override bool CanWrite { get { return false; } }

        public override long Length { get { return length; } }

        public override long Position { get { return pos; } set { Seek(value, SeekOrigin.Begin); } }
    }
}
