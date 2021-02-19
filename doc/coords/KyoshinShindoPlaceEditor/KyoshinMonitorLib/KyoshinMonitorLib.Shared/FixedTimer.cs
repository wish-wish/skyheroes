﻿using System;
using System.Threading;

namespace KyoshinMonitorLib
{
	/// <summary>
	/// 誤差を蓄積させないようにするタイマー ただし極微量のずれはあります
	/// </summary>
	public class FixedTimer
	{
		private Timer _timer;
		private TimeSpan _lastTime;

		private bool _isEventRunning;

		private QueryPerformanceStopwatch _sw;

		private TimeSpan _interval = TimeSpan.FromMilliseconds(1000);

		/// <summary>
		/// タイマーの間隔
		/// </summary>
		public TimeSpan Interval
		{
			get => _interval;
			set
			{
				if (_accuracy.Ticks <= 0)
					throw new ArgumentOutOfRangeException("Intervalには1Tick以上の時間を指定してください。");
				_interval = value;
			}
		}

		private TimeSpan _accuracy = TimeSpan.FromMilliseconds(1);

		/// <summary>
		/// タイマーの精度 ただし精度を保証するものではありません。
		/// <para>上限がある上、精度を高くするとその分重くなります。</para>
		/// <para>この値より短い単位でイベントが発行されることはありません。</para>
		/// <para>Intervalよりも大きな値を指定した際、誤差が蓄積されない特性が消滅することに気をつけてください。</para>
		/// </summary>
		public TimeSpan Accuracy
		{
			get => _accuracy;
			set
			{
				if (_accuracy.Ticks <= 0)
					throw new ArgumentOutOfRangeException("Accuracyには1Tick以上の時間を指定してください。");
				_accuracy = value;
			}
		}

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
		public event Action Elapsed;

		/// <summary>
		/// FixedTimerを初期化します。
		/// </summary>
		/// <param name="interval">間隔</param>
		public FixedTimer(TimeSpan interval) : this()
		{
			Interval = interval;
		}

		/// <summary>
		/// FixedTimerを初期化します。
		/// </summary>
		public FixedTimer()
		{
			_timer = new Timer(s =>
			{
				if (_sw.Elapsed - _lastTime >= Interval)
				{
					if (!BlockingMode || !_isEventRunning)
						ThreadPool.QueueUserWorkItem(s2 =>
						{
							_isEventRunning = true;
							Elapsed?.Invoke();
							_isEventRunning = false;
						});

					//かなり時間のズレが大きければその分修正する
					if (_sw.Elapsed.Ticks - _lastTime.Ticks >= _interval.Ticks * 2)
					{
						var skipCount = ((_sw.Elapsed.Ticks - _lastTime.Ticks) / _interval.Ticks);
						_lastTime += TimeSpan.FromTicks(_interval.Ticks * skipCount);
					}
					else //そうでなかったばあい普通に修正
						_lastTime += _interval;

					if (!AutoReset)
						_timer.Change(Timeout.Infinite, Timeout.Infinite);
				}
			}, null, Timeout.Infinite, Timeout.Infinite);
			_sw = new QueryPerformanceStopwatch();
		}

		/// <summary>
		/// タイマーを開始します。
		/// </summary>
		public void Start()
		{
			_isEventRunning = false;
			_lastTime = TimeSpan.Zero;
			_timer.Change(Accuracy, Accuracy);
			_sw.Start();
		}

		/// <summary>
		/// タイマーを停止します。
		/// </summary>
		public void Stop()
			=> _timer.Change(Timeout.Infinite, Timeout.Infinite);
	}
}