using System;
using System.Collections;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using System.Xml;
namespace COGSD
{
	public class csOcss
	{
		public int decelTime;
		public int acc,jerk,vel;
		public System.Windows.Forms.ImageList imageList1;
		public CAR_STATE state;
		public  const int STOP_NOT_IN_PROGRESS= 	0; /* No Stop in progress */
		public  const int STOP_NOT_STARTED=        1; /* door cycle not yet started */
		public  const int STOP_IN_PROGRESS=        2; /* Stop in progress, in door cycle */
		public  const int NO_DIR=0;
		public  const int UP_DIR=1;
		public  const int DOWN_DIR=2;
		public  const int BOTH_DIR=3;
		public const int DOORS_CLOSED=0;
		public const int DOORS_CLOSING=1;
		public const int DOORS_OPENED=2;
		public const int DOORS_OPENING=3;

		public int[] assigned_calls = new int[256];
		public bool b2D = false;
		public int bottom_pos;
		public bool bUpper = false;
		public double dActual_pos;
		public double DCTime;
		public double DOTime;
		public double DWTime;
		public int f_door_state;
		public System.Collections.ArrayList floorList = new ArrayList();
		public System.Collections.ArrayList paxList=new ArrayList();
		public System.Windows.Forms.Label lbOcss;
		public int mov_dir;
		public int NCF;
		public int old_actual_pos;
		public int openedCnt;
		public double openWidth;
		public byte opmode;
		public System.Windows.Forms.PictureBox picCar;
		public System.Windows.Forms.PictureBox picDoor;
		public int pre_dir;
		public int r_door_state;
		public int stop_pos;
		public int stop_status;
		public int stop_type;
		public int target_pos;
		public int top_pos;
		public int absolute_pos;
		public int bldHeight;
		public int actual_pos;
		public int prevFloor;
		public int prevEventTime;
		public csOcss otherCar;

		public void SetParam(int bldHeight)
		{


		}
		public csOcss()
		{
		}

		public int GetNextTarget()
		{
			int maxFloor=this.floorList.Count;
			int lowestUpHall,highestDnHall;
			highestDnHall=0;
			lowestUpHall=maxFloor-1;
			switch(pre_dir)
			{
				case NO_DIR:
					for(int i=0;i<maxFloor;i++)
					{
						csFloor fl=(csFloor)this.floorList[i];
						if(fl.dnHallCall>0)
						{
							if(i>highestDnHall)
							{
								highestDnHall=i;
							}
						}
						if(fl.upHallCall>0)
						{
							if(i<lowestUpHall)
							{
								lowestUpHall=i;
							}
						}
					}
					if(highestDnHall>0)
						return highestDnHall;
					else if(lowestUpHall<maxFloor-1)
						return lowestUpHall;
					else
						return this.actual_pos;
					break;
				case UP_DIR:
					break;
				case DOWN_DIR:
					break;
			}
			return 3;
		}

		public void ocss_init()
		{
			stop_status=STOP_NOT_IN_PROGRESS;
			f_door_state=DOORS_CLOSED;
			dActual_pos=0;
			mov_dir=NO_DIR;
			pre_dir=NO_DIR;
			state=CAR_STATE.CIS;
			for(int i=0;i<this.floorList.Count;i++)
			{
				csFloor floor=(csFloor)floorList[i];
				floor.carCall=0;
				floor.hallCall=0;
				this.assigned_calls[i]=0;
				floor.picHoist.Image=this.imageList1.Images[(int)floor.carCall+(int)IMG.NO_CARCALL];
				if(this.bUpper.Equals(true) && i==floorList.Count-1)
					floor.picHall.Image=this.imageList1.Images[(int)IMG.NO_CARCALL];
				else if(this.bUpper.Equals(false) && i==0)
					floor.picHall.Image=this.imageList1.Images[(int)IMG.NO_CARCALL];
				else
					floor.picHall.Image=this.imageList1.Images[(int)floor.hallCall+(int)IMG.NO_HALLCALL];
				
			}
		}
	}
}
