using System;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using System.Text;
using System.IO;
using System.Threading;
using System.Collections;

namespace COGSD
{
	/// <summary>
	/// csDLL에 대한 요약 설명입니다.
	/// </summary>
	public class csDLL
	{
		public fmMain mainFm;
		[DllImport("kernel32.dll", SetLastError=true, CharSet=CharSet.Auto)]
		static extern uint CreateFile(string filename, uint access, uint sharemode, uint security_attributes, uint creation, uint flags, uint template);

		[DllImport("kernel32.dll", SetLastError=true)]
		static extern bool CloseHandle(uint handle);

		[DllImport("kernel32.dll", SetLastError=true)]
		static extern bool ReadFile(uint hFile, IntPtr lpBuffer, int nNumberOfBytesToRead, ref uint lpNumberOfBytesRead, IntPtr lpOverlapped);

		[DllImport("kernel32.dll", SetLastError=true)]
		static extern bool WriteFile(uint hFile, IntPtr lpBuffer, int nNumberOfBytesToWrite, ref uint lpNumberOfBytesWritten, IntPtr lpOverlapped);

		[DllImport("Hexec.dll", SetLastError=true)]
		static extern int TransmitCommChar(uint hFile, byte TxChar);


		[DllImport("SpyDll.dll", SetLastError=true)]
		static extern void SPY_SetBuilding(ref BuildingInfo mBdInfo,ref Parameter mParam);

		[DllImport("SpyDll.dll", SetLastError=true)]
		static extern void SPY_SetHoistWay(int hwy,ref HoistwayInfo mHwyInfo);

		[DllImport("SpyDll.dll", SetLastError=true)]
		static extern void SPY_SetCar(int car,ref CarInfo mCarInfo);

		[DllImport("SpyDll.dll", SetLastError=true)]
		static extern void SPY_HPEInitialize();

		[DllImport("SpyDll.dll", SetLastError=true)]
		static extern void SPY_SetPos(int floor,int pos);

		[DllImport("SpyDll.dll", SetLastError=true)]
		static extern void SPY_SetTSACase(int car,int origin,int target,int delay);

		[DllImport("SpyDll.dll", SetLastError=true)]
		static extern void SPY_MotionCmd(int car,int origin,int target,int delay,int jerk,int acc,int vel);

		[DllImport("SpyDll.dll", SetLastError=true)]
		static extern int SPY_GetMotionValue(int car,int time,int type);

		[DllImport("SpyDll.dll", SetLastError=true)]
		static extern int SPY_GetCommittableTime(int car,int s, int d);

		[DllImport("SpyDll.dll", SetLastError=true)]
		static extern int SPY_GetNCP_T3(int car,int s, int d);

		[DllImport("SpyDll.dll", SetLastError=true)]
		static extern int SPY_GetTSADelay(int orgLead,int tgtLead,int orgTrail,int tgtTrail,int minSep);

		public const int MAX_FLOOR=128;
		public const int MAXHWY=16;
		public const int MAXCAR=16;
		unsafe public struct BuildingInfo				// static building information 
		{
			public byte nCars;				// total number of cars in a group
			public byte	nHoistways;			// total number of hoistway in a group
			public byte	lobby1Floor;		// floor index of lobby 1
			public byte	lobby2Floor;		// floor index of lobby 2
			public byte	lobby3Floor;		// floor index of lobby 3
			public byte	rest1Floor;			// floor index of restaurant 1
			public byte	rest2Floor;			// floor index of restaurant 2
			public byte	EZ1Top;				// floor index of express zone 1 top
			public byte	EZ1Bottom;			// floor index of express zone 1 bottom
			public byte	EZ2Top;				// floor index of express zone 2 top
			public byte	EZ2Bottom;			// floor index of express zone 2 bottom
			public byte	EZ3Top;				// floor index of express zone 3 top
			public byte	EZ3Bottom;			// floor index of express zone 3 bottom
			public byte	nLandings;					// number of landings, including pit, overhead, virtual stops in express zone
			public int	f_offset;		// floor offset for logical floor to numerical floor(pit exist then -1, else 0)
		}
	
		public struct HoistwayInfo				// static hoistway information per hoistway
		{
			public int		bPit;				// BOOL true means pit exists
			public int	bOverhead;			// true means overhead exists
			public byte	nCars;				// number of cars in this hoistway
			public sbyte	idLowerCar;			// index of lower car in the hoistway, -1 represent no lower car
			public sbyte	idUpperCar;			// index of upper car in the hoistway, -1 represent no upper car
		};

		public struct CarInfo						// static information per car
		{
			public ushort  Afd;					// forced deceleration
			public ushort  maxJerk;				// maximum jerk value in mm/s/s/s
			public ushort	maxAcceleration;		// maximum acceleration rate in mm/s/s
			public ushort	maxVelocity;			// maximum speed of the car in mm/s
			public int 	tDoorOpening;			// door opening time in milliseconds
			public int	tDoorClosing;			// door closing time in milliseconds
			public ushort	boardingRatio;			// percentage [CSW: currently not used]
			public ushort	capacity;				// max number of passengers the car can carry
			public byte	nDoors;					// number of doors
			public ushort	doorWidth;				// door width in mm
			public byte	hoistwayID;				// index of hoistway this car belongs
			/* Definition of each bit:
			 * Bit 0 : Front car call
			 * Bit 1 : Front up hall call
			 * Bit 2 : Front down hall call
			 * Bit 3 : Front emergency hall call
			 * Bit 4 : Rear car call
			 * Bit 5 : Rear up hall call
			 * Bit 6 : Rear down hall call
			 * Bit 7 : Rear emergency hall call
			 * Bit 8 : Extended front car call
			 * Bit 9 : Extended front up hall call
			 * Bit 10 : Extended front down hall call
			 * Bit 11 : Extended front emergency hall call
			 * Bit 12 : Extended rear car call
			 * Bit 13 : Extended rear up hall call
			 * Bit 14 : Extended rear down hall call
			 * Bit 15 : Extended rear emergency hall call
			 */
			public int	tStartDelay;
			public int	tStopDelay;
		};
		// e.g., CarInfo car_info[MAX_CAR];			

		struct Parameter						// static global parameters		
		{
			public int tCarMinDwell;				
			public int tHallMinDwell;
			public int tHallMaxDwell;
			public int tLobbyDwell;
			public int tBoardingDeboardingTime;
			public int bReopenEnabled;				// true means reopen is enabled
			public uint A1S,A2S,A1W,A2W,W,V;			// parameter for scoring
			public sbyte GDAlgorithmID;					// algorithm ID for group dispatching
		};

		BuildingInfo bdInfo;
		HoistwayInfo hwyInfo;
		CarInfo carInfo;
		Parameter param;

		public csDLL(fmMain fm)
		{
			mainFm=fm;
			bdInfo=new BuildingInfo();
			hwyInfo=new HoistwayInfo();
			carInfo=new CarInfo();
			param=new Parameter();


			//			GetCommTimeouts(m_hFile,ref cto);
		}
		
		public void updateConfig()
		{
			int nLandings=(byte)mainFm.configDs.FloorHeight.Rows.Count;
			int nHoistWays=(byte)mainFm.configDs.HoistWay.Rows.Count;
			int noCar=0;

			for(int i=0;i<nHoistWays;i++)
				for(int j=0;j<2;j++)
				{
					if((int)mainFm.configDs.HoistWay.Rows[i][j]>=0)
						noCar++;
				}
			bdInfo.nLandings=(byte)nLandings;
			bdInfo.nHoistways=(byte)nHoistWays;
			bdInfo.nCars=(byte)noCar;

			param.bReopenEnabled=((bool)mainFm.configDs.Parameter.Rows[0]["bReopenEnabled"]==true) ? 1 : 0;
			param.tCarMinDwell=(int)mainFm.configDs.Parameter.Rows[0]["tCarMinDwell"];
			param.tHallMaxDwell=(int)mainFm.configDs.Parameter.Rows[0]["tHallMaxDwell"];
			param.tHallMinDwell=(int)mainFm.configDs.Parameter.Rows[0]["tHallMinDwell"];
			param.tLobbyDwell=(int)mainFm.configDs.Parameter.Rows[0]["tLobbyDwell"];
			
			SPY_SetBuilding(ref bdInfo,ref param);

			for(int i=0;i<bdInfo.nHoistways;i++)
			{
				hwyInfo.idLowerCar=(sbyte)(int)mainFm.configDs.HoistWay.Rows[i][0];
				hwyInfo.idUpperCar=(sbyte)(int)mainFm.configDs.HoistWay.Rows[i][1];
				SPY_SetHoistWay(i,ref hwyInfo);
			}
			for(int i=0;i<MAXCAR;i++)
			{
				carInfo.Afd=(ushort)mainFm.configDs.CarInfo.Rows[0]["Afd"];
				carInfo.maxAcceleration=(ushort)mainFm.configDs.CarInfo.Rows[0]["maxAcceleration"];
				carInfo.maxJerk=(ushort)mainFm.configDs.CarInfo.Rows[0]["maxJerk"];
				carInfo.maxVelocity=(ushort)mainFm.configDs.CarInfo.Rows[0]["maxVelocity"];
				carInfo.tStartDelay=(int)mainFm.configDs.CarInfo.Rows[0]["tStartDelay"];
				carInfo.tStopDelay=(int)mainFm.configDs.CarInfo.Rows[0]["tStopDelay"];
				carInfo.boardingRatio=(ushort)mainFm.configDs.CarInfo.Rows[0]["boardingRatio"];
				carInfo.capacity=(ushort)mainFm.configDs.CarInfo.Rows[0]["capacity"];
				carInfo.doorWidth=(ushort)mainFm.configDs.CarInfo.Rows[0]["doorWidth"];
				carInfo.nDoors=(byte)mainFm.configDs.CarInfo.Rows[0]["nDoors"];
				carInfo.tDoorClosing=(int)mainFm.configDs.CarInfo.Rows[0]["tDoorClosing"];
				carInfo.tDoorOpening=(int)mainFm.configDs.CarInfo.Rows[0]["tDoorOpening"];
				SPY_SetCar(i,ref carInfo);
			}


			for(int i=0;i<bdInfo.nLandings;i++)
			{
				SPY_SetPos(i,(int)mainFm.configDs.FloorHeight.Rows[i][2]);
			}
			SPY_HPEInitialize();

			byte[] data=new byte[100];
			unsafe
			{
				fixed(byte* pBuf=data)
				{

					//					HPE(ref bdInfo);
					//HPEInitialize(ref bdInfo,ref hwyInfo,ref carInfo,ref param,(IntPtr)pBuf);
					//					ReadFile(m_hFile,(IntPtr)pBuf,100,ref dwBytes,(IntPtr)0);
				}
			}

			/*
						byte[] byteDateLine = System.Text.Encoding.ASCII.GetBytes( str.ToCharArray() );
						byte[] kkk=new byte[30];
						unsafe
						{
							fixed (byte* pBuf = kkk)
							{				
							}				
						}
			*/
		}
		
		public void MotionCmd(int car,int origin,int target,int delay,int jerk, int acc, int vel)
		{
			SPY_MotionCmd(car,origin,target,delay,jerk,acc,vel);
		}

		public void SetTSA(int car,int origin,int target,int delay)
		{
			SPY_SetTSACase(car,origin,target,delay);
		}
		
		public int GetMotion(int car,int time,int type)
		{
			try
			{
				return SPY_GetMotionValue(car,time,type);
			}
			catch
			{
				MessageBox.Show(car.ToString()+","+time.ToString()+","+type.ToString());
				return 0;
			}
		}

		public int GetCommittableTime(int car,int s, int d)
		{
			return SPY_GetCommittableTime(car,s,d);
		}

		public int GetNCP_T3(int car,int s,int d)
		{
			return SPY_GetNCP_T3(car,s,d);
		}

		public int GetTSADelay(int orgLead,int tgtLead,int orgTrail,int tgtTrail,int minSep)
		{
			return SPY_GetTSADelay(orgLead,tgtLead,orgTrail,tgtTrail,minSep);
		}

	}
}
