using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;

namespace COGSD
{
	/// <summary>
	/// fmMotion에 대한 요약 설명입니다.
	/// </summary>
	public class fmMotion : System.Windows.Forms.Form
	{
		/// <summary>
		/// 필수 디자이너 변수입니다.
		/// </summary>
		private System.ComponentModel.Container components = null;
		int XX1,XX2,YY1,YY2,DX,DY,GX1,GX2,GY1,GY2;
		fmMain parentFm;
		public const int GRP_BORDER = 16;
		Font drawFont = new Font("Arial", 10);
		SolidBrush drawBrush = new SolidBrush(Color.Black);
		int motionTimeRange=25;

		DataTable dtPos=new DataTable("Motion");


		public void GetProperMotion(int orgLead,int tgtLead,int orgTrail,int tgtTrail,int delay,int MS,ref int jerk,ref int acc,ref int vel)
		{
			int minMargin,tmpMargin;
			int nLandings=(int)parentFm.configDs.FloorHeight.Rows.Count;
			int maxHeight=(int)parentFm.configDs.FloorHeight.Rows[nLandings-1][2]+(int)parentFm.configDs.FloorHeight.Rows[nLandings-1][1];
			int jerkTest=(int)(ushort)parentFm.configDs.CarInfo.Rows[0]["maxJerk"];
			int accTest=(int)(ushort)parentFm.configDs.CarInfo.Rows[0]["maxAcceleration"];
			int velTest=(int)(ushort)parentFm.configDs.CarInfo.Rows[0]["maxVelocity"];
			int jerkOrg=jerkTest;
			int accOrg=accTest;
			int velOrg=velTest;
			int accSlice=0;
			bool bFirst=true;
			int[] pos=new int[fmMain.MAX_OCSS];
			int prevPos0,prevPos1,prevDiff;
			int nSameCnt;
			int ACC_SLICE=1;

			bool bNeedTSA=(tgtTrail>=orgLead) ? true : false;
			while(bNeedTSA)
			{
				minMargin=(int)maxHeight;

				if(delay>0)
				{
					parentFm.dll.MotionCmd(0,orgTrail,tgtTrail,delay,jerkTest,accTest,velTest);
					parentFm.dll.MotionCmd(1,orgLead,tgtLead,0,jerkOrg,accOrg,velOrg);
				}
				else if(delay<0)
				{
					parentFm.dll.MotionCmd(0,orgTrail,tgtTrail,0,jerkTest,accTest,velTest);
					parentFm.dll.MotionCmd(1,orgLead,tgtLead,-delay,jerkOrg,accOrg,velOrg);
				}
				else
				{
					parentFm.dll.MotionCmd(0,orgTrail,tgtTrail,0,jerkTest,accTest,velTest);
					parentFm.dll.MotionCmd(1,orgLead,tgtLead,0,jerkOrg,accOrg,velOrg);
				}

				dtPos.Rows.Clear();
				prevPos0=prevPos1=prevDiff=0;
				nSameCnt=0;
				for(int i=0;i<motionTimeRange*10;i++)
				{
					pos[0]=(int)parentFm.dll.GetMotion(0,i*100,(int)TSA_TYPE.POSITION);
					pos[1]=(int)parentFm.dll.GetMotion(1,i*100,(int)TSA_TYPE.POSITION);
					pos[2]=(int)parentFm.dll.GetMotion(0,i*100,(int)TSA_TYPE.NCP);
					pos[3]=(int)parentFm.dll.GetMotion(1,i*100,(int)TSA_TYPE.NSP);
					pos[4]=0;
					pos[5]=0;
					pos[6]=0;
					pos[7]=0;
					tmpMargin=(int)pos[3]-(int)pos[2];
					if(tmpMargin<minMargin)
					{
						minMargin=tmpMargin;
					}
					if(prevPos0==pos[0] && prevPos1==pos[1] && prevDiff==tmpMargin)
					{
						nSameCnt++;
						if(nSameCnt>10)
							break;
					}
					prevPos0=pos[0];
					prevPos1=pos[1];
					prevDiff=tmpMargin;
					if(i>motionTimeRange*10-10)
						motionTimeRange+=10; // add 10 sec

					DataRow row=dtPos.NewRow();
					double time=(double)i/10.0;
					row[0]=time.ToString(); //time
					for(int car=0;car<4;car++)
					{
						try
						{
							if(pos[car]<0)
								row[car+1]="0";
							else if(pos[car+1]>=(uint)maxHeight)
								row[car+1]=maxHeight.ToString();
							else
								row[car+1]=pos[car].ToString();
						}
						catch
						{
							row[car+1]="0";
						}
					}
					dtPos.Rows.Add(row);
					if(dtPos.Rows.Count>motionTimeRange*10)
						dtPos.Rows.RemoveAt(0);
				}
				this.DrawGrpFrame();
				//this.statusBarTSA.Panels[1].Text="Min Separation : "+minMargin.ToString()+" mm";
				if(bFirst && minMargin<MS)
				{
					bFirst=false;
					accSlice=-ACC_SLICE;
				}
				if(bFirst && minMargin>=MS)
				{
					bFirst=false;
					accSlice=ACC_SLICE;
				}
				if(accSlice<0 && minMargin>=MS)
					break;
				else if(accSlice>0 && minMargin<MS)
				{
					accTest-=ACC_SLICE;
					break;
				}
				accTest+=accSlice;
				Application.DoEvents();
			}
			jerk=jerkTest;
			acc=accTest;
			vel=velTest;
/*
			if(shift<=0 && state.Equals(SPY_STATE.TSA_TSADELAY))
			{
				minMargin=(int)maxHeight;
				dll.SetTSA(0,orgTrail,tgtTrail,0);
				dll.SetTSA(1,orgLead,tgtLead,-shift);


				debugDs.pos.Rows.Clear();
			
				for(int i=0;i<TSATimeRange*10;i++)
				{
					pos[0]=(int)dll.GetMotion(0,i*100,(int)TSA_TYPE.POSITION);
					pos[1]=(int)dll.GetMotion(1,i*100,(int)TSA_TYPE.POSITION);
					pos[2]=(int)dll.GetMotion(0,i*100,this.cbTrailing.SelectedIndex);
					pos[3]=(int)dll.GetMotion(1,i*100,this.cbLeading.SelectedIndex);
					pos[4]=0;
					pos[5]=0;
					pos[6]=0;
					pos[7]=0;
					tmpMargin=(int)pos[3]-(int)pos[2];
					if(tmpMargin<minMargin)
					{
						minMargin=tmpMargin;
					}
					DataRow row=debugDs.pos.NewRow();
					double time=(double)i/10.0;
					row[0]=time.ToString(); //time
					for(int car=0;car<4;car++)
					{
						try
						{
							if(pos[car]<0)
								row[car+1]="0";
							else if(pos[car+1]>=(uint)maxHeight)
								row[car+1]=maxHeight.ToString();
							else
								row[car+1]=pos[car].ToString();
						}
						catch
						{
							row[car+1]="0";
						}
					}
					debugDs.pos.Rows.Add(row);
					if(debugDs.pos.Rows.Count>TSATimeRange*10)
						debugDs.pos.Rows.RemoveAt(0);
				}
				this.DrawGrpFrameTSA();
				//this.statusBarTSA.Panels[1].Text="Min Separation : "+minMargin.ToString()+" mm";
			}
			if(state.Equals(SPY_STATE.TSA_TSADELAY))
				this.statusBarTSA.Panels[2].Text="TSA Delay : "+shift.ToString()+" msec";
				*/
		}

		public void DrawGrpFrame()
		{
			int i;
			Graphics dc=this.CreateGraphics();

			/*
			Pen BluePen=new Pen(Color.Blue,3);
			dc.DrawLine(BluePen,20,70,260,70);
			Pen GreenPen=new Pen(Color.Green,4);
			dc.DrawLine(GreenPen,20,120,260,120);
			Pen RedPen=new Pen(Color.Red,4);
			dc.DrawLine(RedPen,20,170,260,170);
*/
			Brush br=new SolidBrush(Color.White);
			dc.FillRectangle(br,0,0,this.Width,this.Height);
			XX1 = 0;
			YY1 = 0;
			XX2 = this.Width-1;
			YY2 = this.Height;
			GX1 = XX1 + GRP_BORDER*3;
			GY1 = YY1 + GRP_BORDER;
			GX2 = XX2 - GRP_BORDER;
			GY2 = YY2 - GRP_BORDER*3;

			DX = GX2 - GX1;
			DY = GY2 - GY1;
			Pen pen=new Pen(Color.Black,1);
			dc.DrawRectangle(pen,XX1, YY1, XX2, YY2);
			pen.Color=Color.FromArgb(0xc0,0xc0,0xc0);
			dc.DrawRectangle(pen,GX1, GY1, DX, DY);
			int nFloor=parentFm.configDs.FloorHeight.Rows.Count;
			int bdHeight=(int)parentFm.configDs.FloorHeight.Rows[nFloor-1][1]+(int)parentFm.configDs.FloorHeight.Rows[nFloor-1][2];
			for (i=0; i<parentFm.configDs.FloorHeight.Rows.Count; i++)
			{
				int y=(int)parentFm.configDs.FloorHeight.Rows[i][2];
				int y1=(int)((double)DY*((double)y/(double)bdHeight));
				dc.DrawLine(pen,GX1, GY2-y1,GX2, GY2-y1);
				// Draw string to screen.
				dc.DrawString((string)parentFm.configDs.FloorHeight.Rows[i][0], drawFont, drawBrush, 2, GY2-y1-20);
			}
			for (i=0; i<10; i++)
			{
				dc.DrawLine(pen,GX1 + DX * i /10, GY1,GX1 + DX * i /10, GY2);
			}
			pen.Color=Color.FromArgb(0,0,0);
			dc.DrawLine(pen,GX1,GY1,GX1,GY2);
			dc.DrawLine(pen,GX1,GY2,GX2,GY2);

			br=new SolidBrush(Color.CadetBlue);
			Font DispFont=new Font("Arial",10,FontStyle.Bold);
			try
			{
				DisplayUpdate();
			}
			catch(Exception e)
			{
				MessageBox.Show(e.Message);
			}
		}

		public void DisplayUpdate()
		{
			double minSepX;
			int[] minSepY=new int[2];
			string time;
			int penInx;
			double dTime;
			string name;
			int x1,x2,y1,y2;
			double xSub;
			int xMax;
			int nLandings=parentFm.configDs.FloorHeight.Rows.Count;
			int maxHeight=(int)parentFm.configDs.FloorHeight.Rows[nLandings-1][2]+(int)parentFm.configDs.FloorHeight.Rows[nLandings-1][1];
			int minSep=maxHeight;
			int minSepY1=minSep;
			int minSepY0=0;

			Graphics dc=this.CreateGraphics();
			int i;
			if(dtPos.Rows.Count<=1)
				return;
			int lastInx=dtPos.Rows.Count-1;
			double firstTime=double.Parse((string)dtPos.Rows[0][0]);
			minSepX=firstTime;
			for(i=0;i<dtPos.Rows.Count-1;i++)
			{
				time=(string)dtPos.Rows[i][0]; 
				dTime=double.Parse(time);
				x1=GX1+(int)(DX*((dTime-firstTime)/motionTimeRange));
				time=(string)dtPos.Rows[i+1][0]; // 현재 X좌표를 읽어옴
				dTime=double.Parse(time);
				x2=GX1+(int)(DX*((dTime-firstTime)/motionTimeRange));
				if(x2>GX2)
					return;
				for(int j=0;j<4;j++)
				{
					y1=(int)((double)DY*(double.Parse((string)dtPos.Rows[i][j+1])/(double)maxHeight));
					y2=(int)((double)DY*(double.Parse((string)dtPos.Rows[i+1][j+1])/(double)maxHeight));
					dc.DrawLine(parentFm.penArr[j],x1,GY2-y1,x2,GY2-y2);
				}	
				minSepY[1]=(int)double.Parse((string)dtPos.Rows[i][4]);
				minSepY[0]=(int)double.Parse((string)dtPos.Rows[i][3]);
				if((minSepY[1]-minSepY[0])<minSep)
				{
					minSep=(minSepY[1]-minSepY[0]);
					minSepY1=minSepY[1];
					minSepY0=minSepY[0];
					time=(string)dtPos.Rows[i][0]; 
					minSepX=double.Parse(time);
				}
			}
			if(dtPos.Rows.Count>1)
			{
				y1=(int)((double)DY*(double)minSepY1/(double)maxHeight);
				y2=(int)((double)DY*(double)minSepY0/(double)maxHeight);
				x1=GX1+(int)(DX*((minSepX-firstTime)/motionTimeRange));
				dc.DrawLine(parentFm.penArr[2],x1,GY2-y1,x1,GY2-y2);
				string strTime=minSepX.ToString()+" sec";
				string strMin="Min Separation :"+minSep.ToString()+" mm";
			}
		}

		
		public fmMotion(fmMain fm)
		{
			//
			// Windows Form 디자이너 지원에 필요합니다.
			//
			parentFm=fm;
			InitializeComponent();
			dtPos.Columns.Add(new DataColumn("time",System.Type.GetType("System.String"))); 
			dtPos.Columns.Add(new DataColumn("CAR0",System.Type.GetType("System.String"))); 
			dtPos.Columns.Add(new DataColumn("CAR1",System.Type.GetType("System.String"))); 
			dtPos.Columns.Add(new DataColumn("CAR2",System.Type.GetType("System.String"))); 
			dtPos.Columns.Add(new DataColumn("CAR3",System.Type.GetType("System.String"))); 
			dtPos.Columns.Add(new DataColumn("CAR4",System.Type.GetType("System.String"))); 
			dtPos.Columns.Add(new DataColumn("CAR5",System.Type.GetType("System.String"))); 
			dtPos.Columns.Add(new DataColumn("CAR6",System.Type.GetType("System.String"))); 
			dtPos.Columns.Add(new DataColumn("CAR7",System.Type.GetType("System.String"))); 

			//
			// TODO: InitializeComponent를 호출한 다음 생성자 코드를 추가합니다.
			//
		}

		/// <summary>
		/// 사용 중인 모든 리소스를 정리합니다.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if(components != null)
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		#region Windows Form 디자이너에서 생성한 코드
		/// <summary>
		/// 디자이너 지원에 필요한 메서드입니다.
		/// 이 메서드의 내용을 코드 편집기로 수정하지 마십시오.
		/// </summary>
		private void InitializeComponent()
		{
			// 
			// fmMotion
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(6, 14);
			this.ClientSize = new System.Drawing.Size(344, 213);
			this.Name = "fmMotion";
			this.Text = "Hoistway Plan Executer";
			this.Paint += new System.Windows.Forms.PaintEventHandler(this.fmMotion_Paint);

		}
		#endregion

		private void fmMotion_Paint(object sender, System.Windows.Forms.PaintEventArgs e)
		{
			this.DrawGrpFrame();
		}
	}
}
