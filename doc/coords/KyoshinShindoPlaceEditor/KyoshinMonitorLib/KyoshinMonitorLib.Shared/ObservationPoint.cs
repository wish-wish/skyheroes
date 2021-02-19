﻿using MessagePack;
#if !NETFX_CORE
using ProtoBuf;
#endif
using System;
using System.Collections.Generic;
using System.IO;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Json;
using System.Text;

namespace KyoshinMonitorLib
{
	/// <summary>
	/// NIEDの観測点情報
	/// </summary>
	[MessagePackObject, DataContract
#if !NETFX_CORE
		, ProtoContract
#endif
		]
	public class ObservationPoint : IComparable
	{
#if !NETFX_CORE
		/// <summary>
		/// 観測点情報をpbfから読み込みます。失敗した場合は例外がスローされます。
		/// </summary>
		/// <param name="path">読み込むpbfファイルのパス</param>
		/// <returns>読み込まれた観測点情報</returns>
		public static ObservationPoint[] LoadFromPbf(string path)
		{
			using (var stream = new FileStream(path, FileMode.Open))
				return Serializer.Deserialize<ObservationPoint[]>(stream);
		}

		/// <summary>
		/// 観測点情報をpbfに保存します。失敗した場合は例外がスローされます。
		/// </summary>
		/// <param name="path">書き込むpbfファイルのパス</param>
		/// <param name="points">書き込む観測点情報の配列</param>
		public static void SaveToPbf(string path, IEnumerable<ObservationPoint> points)
		{
			using (var stream = new FileStream(path, FileMode.Create))
				Serializer.Serialize(stream, points);
		}
#endif

		/// <summary>
		/// 観測点情報をmpkから読み込みます。失敗した場合は例外がスローされます。
		/// </summary>
		/// <param name="path">読み込むmpkファイルのパス</param>
		/// <param name="usingLz4">lz4で圧縮させるかどうか(させる場合は拡張子を.mpk.lz4にすることをおすすめします)</param>
		/// <returns>読み込まれた観測点情報</returns>
		public static ObservationPoint[] LoadFromMpk(string path, bool usingLz4 = false)
		{
			using (var stream = new FileStream(path, FileMode.Open))
				return usingLz4 ? LZ4MessagePackSerializer.Deserialize<ObservationPoint[]>(stream) : MessagePackSerializer.Deserialize<ObservationPoint[]>(stream);
		}

		/// <summary>
		/// 観測点情報をmpk形式で保存します。失敗した場合は例外がスローされます。
		/// </summary>
		/// <param name="path">書き込むmpkファイルのパス</param>
		/// <param name="points">書き込む観測点情報の配列</param>
		/// <param name="usingLz4">lz4で圧縮させるかどうか(させる場合は拡張子を.mpk.lz4にすることをおすすめします)</param>
		public static void SaveToMpk(string path, IEnumerable<ObservationPoint> points, bool usingLz4 = false)
		{
			using (var stream = new FileStream(path, FileMode.Create))
				if (usingLz4)
					LZ4MessagePackSerializer.Serialize(stream, points);
				else
					MessagePackSerializer.Serialize(stream, points);
		}

		/// <summary>
		/// 観測点情報をJsonから読み込みます。失敗した場合は例外がスローされます。
		/// </summary>
		/// <param name="path">読み込むJsonファイルのパス</param>
		/// <returns>読み込まれた観測点情報</returns>
		public static ObservationPoint[] LoadFromJson(string path)
		{
			using (var stream = new FileStream(path, FileMode.Open))
				return new DataContractJsonSerializer(typeof(ObservationPoint[])).ReadObject(stream) as ObservationPoint[];
		}

		/// <summary>
		/// 観測点情報をJson形式で保存します。失敗した場合は例外がスローされます。
		/// </summary>
		/// <param name="path">書き込むJsonファイルのパス</param>
		/// <param name="points">書き込む観測点情報の配列</param>
		public static void SaveToJson(string path, IEnumerable<ObservationPoint> points)
		{
			using (var stream = new FileStream(path, FileMode.Create))
				new DataContractJsonSerializer(typeof(ObservationPoint[])).WriteObject(stream, points);
		}

		/// <summary>
		/// 観測点情報をcsvから読み込みます。失敗した場合は例外がスローされます。
		/// </summary>
		/// <param name="path">読み込むcsvファイルのパス</param>
		/// <param name="encoding">読み込むファイル文字コード 何も指定していない場合はUTF8が使用されます。</param>
		/// <returns>list:読み込まれた観測点情報 success:読み込みに成功した項目のカウント error:読み込みに失敗した項目のカウント</returns>
		public static (ObservationPoint[] points, uint success, uint error) LoadFromCsv(string path, Encoding encoding = null)
		{
			var addedCount = 0u;
			var errorCount = 0u;

			var points = new List<ObservationPoint>();

			using(var stream = File.OpenRead(path))
			using (var reader = new StreamReader(stream))
				while (reader.Peek() >= 0)
					try
					{
						var strings = reader.ReadLine().Split(',');

						var point = new ObservationPoint()
						{
							Type = (ObservationPointType)int.Parse(strings[0]),
							Code = strings[1],
							IsSuspended = bool.Parse(strings[2]),
							Name = strings[3],
							Region = strings[4],
							Location = new Location(float.Parse(strings[5]), float.Parse(strings[6])),
							Point = null
						};
						if (!string.IsNullOrWhiteSpace(strings[7]) && !string.IsNullOrWhiteSpace(strings[8]))
							point.Point = new Point2(int.Parse(strings[7]), int.Parse(strings[8]));
						if (strings.Length >= 11)
						{
							point.ClassificationId = int.Parse(strings[9]);
							point.PrefectureClassificationId = int.Parse(strings[10]);
						}
						points.Add(point);
						addedCount++;
					}
					catch
					{
						errorCount++;
					}

			return (points.ToArray(), addedCount, errorCount);
		}

		/// <summary>
		/// 観測点情報をcsvに保存します。失敗した場合は例外がスローされます。
		/// </summary>
		/// <param name="path">書き込むcsvファイルのパス</param>
		/// <param name="points">書き込む観測点情報の配列</param>
		public static void SaveToCsv(string path, IEnumerable<ObservationPoint> points)
		{
			using (var stream = File.OpenWrite(path))
			using (var writer = new StreamWriter(stream))
				foreach (var point in points)
					writer.WriteLine($"{(int)point.Type},{point.Code},{point.IsSuspended},{point.Name},{point.Region},{point.Location.Latitude},{point.Location.Longitude},{point.Point?.X.ToString() ?? ""},{point.Point?.Y.ToString() ?? ""},{point.ClassificationId?.ToString() ?? ""},{point.PrefectureClassificationId?.ToString() ?? ""}");
		}

		/// <summary>
		/// ObservationPointを初期化します。
		/// </summary>
		public ObservationPoint()
		{
		}

		/// <summary>
		/// ObservationPointの情報をコピーした上で初期化します。
		/// </summary>
		/// <param name="point"></param>
		public ObservationPoint(ObservationPoint point) :
			this(point.Type, point.Code, point.IsSuspended, point.Name, point.Region, point.Location, point.Point, point.ClassificationId, point.PrefectureClassificationId)
		{
		}

		/// <summary>
		/// ObservationPointを初期化します。
		/// </summary>
		/// <param name="type"></param>
		/// <param name="code"></param>
		/// <param name="isSuspended"></param>
		/// <param name="name"></param>
		/// <param name="region"></param>
		/// <param name="location"></param>
		/// <param name="point"></param>
		/// <param name="classId"></param>
		/// <param name="prefClassId"></param>
		public ObservationPoint(ObservationPointType type, string code, bool isSuspended, string name, string region, Location location, Point2? point = null, int? classId = null, int? prefClassId = null)
		{
			Type = type;
			Code = code;
			IsSuspended = isSuspended;
			Name = name;
			Region = region;
			Location = location;
			Point = point;
			ClassificationId = classId;
			PrefectureClassificationId = prefClassId;
		}

		/// <summary>
		/// 観測地点のネットワークの種類
		/// </summary>
		[Key(0), DataMember(Order = 0)
#if !NETFX_CORE
			, ProtoMember(1)
#endif
			]
		public ObservationPointType Type { get; set; }

		/// <summary>
		/// 観測点コード
		/// </summary>
		[Key(1), DataMember(Order = 1)
#if !NETFX_CORE
			, ProtoMember(2)
#endif
]
		public string Code { get; set; }

		/// <summary>
		/// 観測点名
		/// </summary>
		[Key(2), DataMember(Order = 2)
#if !NETFX_CORE
			, ProtoMember(4)
#endif
]
		public string Name { get; set; }

		/// <summary>
		/// 観測点広域名
		/// </summary>
		[Key(3), DataMember(Order = 3)
#if !NETFX_CORE
			, ProtoMember(5)
#endif
]
		public string Region { get; set; }

		/// <summary>
		/// 観測地点が休止状態(無効)かどうか
		/// </summary>
		[Key(4), DataMember(Order = 4)
#if !NETFX_CORE
			, ProtoMember(3)
#endif
]
		public bool IsSuspended { get; set; }

		/// <summary>
		/// 地理座標
		/// </summary>
		[Key(5), DataMember(Order = 5)
#if !NETFX_CORE
			, ProtoMember(6)
#endif
]
		public Location Location { get; set; }

		/// <summary>
		/// 強震モニタ画像上での座標
		/// </summary>
		[Key(6), DataMember(Order = 6)
#if !NETFX_CORE
			, ProtoMember(7)
#endif
]
		public Point2? Point { get; set; }

		/// <summary>
		/// 緊急地震速報や震度速報で用いる区域のID(EqWatchインポート用)
		/// </summary>
		[Key(7), DataMember(Order = 7)
#if !NETFX_CORE
			, ProtoMember(8, IsRequired = false)
#endif
			]
		public int? ClassificationId { get; set; }

		/// <summary>
		/// 緊急地震速報で用いる府県予報区のID(EqWatchインポート用)
		/// </summary>
		[Key(8), DataMember(Order = 8)
#if !NETFX_CORE
			, ProtoMember(9, IsRequired = false)
#endif
			]
		public int? PrefectureClassificationId { get; set; }

		/// <summary>
		/// ObservationPoint同士を比較します。
		/// </summary>
		/// <param name="obj">比較対象のObservationPoint</param>
		/// <returns></returns>
		public int CompareTo(object obj)
		{
			if (!(obj is ObservationPoint ins))
				throw new ArgumentException("比較対象はObservationPointにキャストできる型でなければなりません。");
			return Code.CompareTo(ins.Code);
		}
	}
}