﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace KyoshinMonitorLib
{
	/// <summary>
	/// NTPアシスト機能付きタイマー 1時間毎に自動で再取得します。
	/// </summary>
	public class NtpTimer
	{
		private Timer _timer;
		private TimeSpan _lastTime;
		private DateTime _currentTime;

		private bool updating;
		private bool _isEventRunning;

		//最後にNTP更新された時刻
		private DateTime _lastUpdatedTime;

		private QueryPerformanceStopwatch _sw;

		private TimeSpan _interval = TimeSpan.FromSeconds(1);

		private TimeSpan _offset = TimeSpan.Zero;
		/// <summary>
		/// オフセットを調整します。
		/// </summary>
		public TimeSpan Offset
		{
			get => _offset;
			set
			{
				_lastTime -= (_offset - value);
				_offset = value;
			}
		}

		/// <summary>
		/// タイマーの間隔(秒) 1もしくは2しか指定できません。
		/// </summary>
		public int Interval
		{
			get => (int)_interval.TotalSeconds;
			set
			{
				if (value != 1 && value != 2)
					throw new ArgumentOutOfRangeException("Intervalは1もしくは2を指定してください。");
				_interval = TimeSpan.FromSeconds(value);
			}
		}

		/// <summary>
		/// タイマーの精度 ただし精度を保証するものではありません。
		/// <para>上限がある上、精度を高くするとその分重くなります。</para>
		/// <para>この値より短い単位でイベントが発行されることはありません。</para>
		/// <para>Intervalよりも大きな値を指定した際、誤差が蓄積されない特性が消滅することに気をつけてください。</para>
		/// </summary>
		private TimeSpan Accuracy => TimeSpan.FromMilliseconds(1);

		/// <summary>
		/// タイマーが1度実行された後
		/// </summary>
		public bool AutoReset { get; set; } = true;

		/// <summary>
		/// イベント発生時にすでに他のイベントが実行中の場合新規イベントを実行させないようにするかどうか
		/// </summary>
		public bool BlockingMode { get; set; } = true;

		/// <summary>
		/// タイマーイベント
		/// </summary>
		public event Action<DateTime> Elapsed;

		/// <summary>
		/// NtpTimerを初期化します。
		/// </summary>
		public NtpTimer()
		{
			_timer = new Timer(async s =>
			{
				if (_sw.Elapsed - _lastTime >= _interval)
				{
					if (!BlockingMode || !_isEventRunning)
						ThreadPool.QueueUserWorkItem(s2 =>
						{
							_isEventRunning = true;
							Elapsed?.Invoke(_currentTime);
							_isEventRunning = false;
						});
					//かなり時間のズレが大きければその分修正する
					if (_sw.Elapsed.Ticks - _lastTime.Ticks >= _interval.Ticks * 2)
					{
						var skipCount = ((_sw.Elapsed.Ticks - _lastTime.Ticks) / _interval.Ticks);
						_lastTime += TimeSpan.FromTicks(_interval.Ticks * skipCount);
						_currentTime = _currentTime.AddSeconds(Interval * skipCount);
					}
					else //そうでなかったばあい普通に修正
					{
						_lastTime += _interval;
						_currentTime = _currentTime.AddSeconds(Interval);
					}
					if (!AutoReset)
						_timer.Change(Timeout.Infinite, Timeout.Infinite);
				}
				if (!updating && (_currentTime - _lastUpdatedTime).TotalHours >= 1)
					await UpdateTime();
			}, null, Timeout.Infinite, Timeout.Infinite);
			_sw = new QueryPerformanceStopwatch();
		}

		private async Task UpdateTime()
		{
			updating = true;
			_lastUpdatedTime = await NtpAssistance.GetNetworkTimeWithNtp();
			_currentTime = _lastUpdatedTime.AddMilliseconds(-_lastUpdatedTime.Millisecond);
			_lastTime = _sw.Elapsed - TimeSpan.FromMilliseconds(_lastUpdatedTime.Millisecond);
			_lastTime += Offset;
			updating = false;
		}

		/// <summary>
		/// タイマーを開始します。
		/// </summary>
		public async Task Start()
		{
			_isEventRunning = false;
			_sw.Start();
			await UpdateTime();
			_timer.Change(Accuracy, Accuracy);
		}

		/// <summary>
		/// タイマーを停止します。
		/// </summary>
		public void Stop()
			=> _timer.Change(Timeout.Infinite, Timeout.Infinite);
	}
}
