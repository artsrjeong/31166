using System;
using System.Collections;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using System.Xml;
namespace COGS_SPY
{
	//internal class csFloor
	//{
	//	public uint carCall;
	//	public int carId;
	//	public int floor;
	//	public uint hallCall;
	//	public System.Windows.Forms.PictureBox picHall;
	//	public System.Windows.Forms.PictureBox picHoist;
	//	public uint dnCarCall
	//	{
	//		get
	//		{
	//			return 0;
	//		}
	//		set
	//		{
	//		}
	//	}
	//	public uint dnHallCall
	//	{
	//		get
	//		{
	//			return 0;
	//		}
	//		set
	//		{
	//		}
	//	}
	//	public uint upCarCall
	//	{
	//		get
	//		{
	//			return 0;
	//		}
	//		set
	//		{
	//		}
	//	}
	//	public uint upHallCall
	//	{
	//		get
	//		{
	//			return 0;
	//		}
	//		set
	//		{
	//		}
	//	}
	//}


}
namespace COGSD
{
	internal class csFloor
	{
		public uint carCall;
		public int carId;
		public int floor;
		public uint hallCall;
		public System.Windows.Forms.PictureBox picHall;
		public System.Windows.Forms.PictureBox picHoist;

		public uint upCarCall{get {return (carCall) & 1;}
			set {carCall=carCall & ~(1U) | (value);}
		}
		public uint dnCarCall {get {return (carCall>>1) & 1;}
			set {carCall=carCall & ~(1U<<1) | (value<<1);}
		}

		public uint upHallCall{get {return (hallCall) & 1;}
			set {hallCall=hallCall & ~(1U) | (value);}
		}
		public uint dnHallCall {get {return (hallCall>>1) & 1;}
			set {hallCall=hallCall & ~(1U<<1) | (value<<1);}
		}
	}
}
