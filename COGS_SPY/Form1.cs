using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;
using System.Xml;
using System.IO;
using System.Threading;
using System.Runtime.InteropServices;
using System.Resources;

namespace COGSD
{
	public class Form1 : System.Windows.Forms.Form
	{
		const int        S_FCC            = 0x01;    /* front up car call          */
		const int        S_FUHC           = 0x02 ;   /* front up hall call		*/
		const int        S_FDHC           = 0x04;	  /* front down hall call       */
		const int        S_FEHC           = 0x08;    /* front emergency hall call  */
		const int        S_RCC            = 0x10 ;   /* front car call             */
		const int        S_RUHC          =  0x20;	  /* rear up hall call		*/
		const int        S_RDHC          =  0x40;	  /* rear down hall call	*/
		const int        S_REHC         =   0x80 ;   /* rear emergency hall call   */

		const int        E_FCC           =  0x0100 ; /* front up car call          */
		const int        E_FUHC          =  0x0200;  /* front up hall call		*/
		const int        E_FDHC          =  0x0400;  /* front down hall call	*/
		const int        E_FEHC          =  0x0800 ; /* front emergency hall call  */
		const int        E_RCC           =  0x1000 ; /* rear car call              */
		const int        E_RUHC          =  0x2000;  /* rear up hall call		*/
		const int        E_RDHC          =  0x4000 ; /* rear down hall call        */
		const int        E_REHC           = 0x8000 ; /* rear emergency hall call   */

		int maxFloor=0;
		public int floorHeight;
		const int HOIST_WIDTH=25;
		const int HOIST_HEIGHT=30;
		const int HALL_WIDTH=25;
		const int HALL_HEIGHT=30;
		public int HOIST_OX=200;

		ArrayList ocssList=new ArrayList();
		ArrayList hallList=new ArrayList();
		private System.Windows.Forms.DataGrid gridDebug;
		public dsDebug debugDs;
		public dsReport reportDs;
		public dsConfig configDs;
		public dsMon	monDs;

		private System.Windows.Forms.Timer timer1;
		private System.ComponentModel.IContainer components;
		private System.Windows.Forms.TabPage tabParam;
		private System.Windows.Forms.TabPage tabFloor;
		private System.Windows.Forms.DataGrid gridFloor;
		private System.Windows.Forms.TabControl tabDebug;
		private System.Windows.Forms.TabPage tabConfig;
		private System.Windows.Forms.TabPage tabPage1;
		private System.Windows.Forms.TabPage tabPage2;
		private System.Windows.Forms.TabPage tabPage3;
		private System.Windows.Forms.DataGrid grid2d_p;
		private System.Windows.Forms.DataGrid grid2d_c;
		private System.Windows.Forms.DataGrid gridReport1;
		private System.Windows.Forms.DataGrid gridReport2;

		int MAX_OCSS=9;
		private System.Windows.Forms.TabPage tabReport;
		private CrystalDecisions.Windows.Forms.CrystalReportViewer crystalReportViewer1;
		int lobby_pos;
		private System.Windows.Forms.TabPage tabTraject;
		private System.Windows.Forms.Panel panel1;
		private System.Windows.Forms.Panel panel2;
		private System.Windows.Forms.CheckBox checkBox1;
		private System.Windows.Forms.CheckBox checkBox2;
		private System.Windows.Forms.CheckBox checkBox4;
		private System.Windows.Forms.CheckBox checkBox3;
		private System.Windows.Forms.CheckBox checkBox6;
		private System.Windows.Forms.CheckBox checkBox7;
		private System.Windows.Forms.CheckBox checkBox5;
		private System.Windows.Forms.CheckBox checkBox8;

		public const int GRP_BORDER = 16;
		public int XX1;
		public int YY1;
		public int XX2;
		public int YY2;
		public int GX1;
		public int GX2;
		public int GY1;
		public int GY2;
		public int DX,DY;
		public CrystalReport1 oRpt=null;
		public Color[] colorArr;
		public Pen[] penArr;
		public CheckBox[] checkArr;
		public long CurrentTimeRange;
		private System.Windows.Forms.NumericUpDown maxY;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.TabPage tabTr;
		private System.Windows.Forms.DataGrid gridTrajectory;
		private System.Windows.Forms.TabPage tabSimul;
		private System.Windows.Forms.TabPage tabFloorHeight;
		private System.Windows.Forms.TabPage tabCalc;
		private System.Windows.Forms.DataGrid gridFloorHeight;
		private System.Windows.Forms.DataGrid gridCalc;
		private System.Windows.Forms.TextBox textBox8;
		private System.Windows.Forms.Label label19;
		private System.Windows.Forms.Label label20;
		private System.Windows.Forms.Label label21;
		private System.Windows.Forms.Label label22;
		private System.Windows.Forms.Label label23;
		private System.Windows.Forms.Label label24;
		private System.Windows.Forms.TextBox textBox10;
		private System.Windows.Forms.TextBox textBox11;
		private System.Windows.Forms.TextBox textBox12;
		private System.Windows.Forms.Label label25;
		private System.Windows.Forms.Label label26;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.TextBox txtLobby;
		private System.Windows.Forms.Button btnReport;
		int LastDispInx=0;
		private System.Windows.Forms.ToolBar tbSimul;
		private System.Windows.Forms.ToolBarButton toolBarButton1;
		private System.Windows.Forms.ToolBarButton toolBarButton2;
		private System.Windows.Forms.ToolBarButton toolBarButton3;
		private System.Windows.Forms.ToolBarButton toolBarButton4;
		private System.Windows.Forms.ToolBarButton toolBarButton5;
		private System.Windows.Forms.CheckBox checkBox9;
		private System.Windows.Forms.CheckBox checkBox10;
		private System.Windows.Forms.CheckBox checkBox11;
		XmlTextReader xmlReader;
		double fullSpeedDistance;
		double fullAccDistance;
		private System.Windows.Forms.TabPage tabMonitor;
		private System.Windows.Forms.DataGrid gridPassenger;
		private System.Windows.Forms.TextBox txtVelU;
		private System.Windows.Forms.TextBox txtAccU;
		private System.Windows.Forms.TextBox txtJerkU;
		private System.Windows.Forms.TextBox txtSafeGU;
		private System.Windows.Forms.TextBox txtVelL;
		private System.Windows.Forms.Label lbVelL;
		private System.Windows.Forms.Label lbAccL;
		private System.Windows.Forms.TextBox txtAccL;
		private System.Windows.Forms.TextBox txtJerkL;
		private System.Windows.Forms.Label lbJerkL;
		private System.Windows.Forms.Label lbmsL;
		private System.Windows.Forms.Label lbmssL;
		private System.Windows.Forms.Label lbmsssL;
		private System.Windows.Forms.Label lbSafeL;
		private System.Windows.Forms.Label lbGL;
		private System.Windows.Forms.TextBox txtSafeGL;
		private System.Windows.Forms.Label label3;
		private System.Windows.Forms.Label label4;
		private System.Windows.Forms.Label label5;
		private System.Windows.Forms.Label label6;
		private System.Windows.Forms.Label label7;
		private System.Windows.Forms.Label label8;
		private System.Windows.Forms.Label label9;
		private System.Windows.Forms.Label label10;
		private System.Windows.Forms.DataGrid gridTraffic;
		private System.Windows.Forms.StatusBar statusBar1;
		private System.Windows.Forms.ImageList imageList1;
		
		double vel;

		[DllImport("user32.dll", CharSet=CharSet.Auto, ExactSpelling=true, CallingConvention=CallingConvention.Winapi)]
		public static extern short GetKeyState(int keyCode); 

		public Form1()
		{
			//
			// Windows Form 디자이너 지원에 필요합니다.
			//
			InitializeComponent();

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
				if (components != null) 
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
			this.components = new System.ComponentModel.Container();
			System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(Form1));
			this.gridDebug = new System.Windows.Forms.DataGrid();
			this.timer1 = new System.Windows.Forms.Timer(this.components);
			this.tabDebug = new System.Windows.Forms.TabControl();
			this.tabParam = new System.Windows.Forms.TabPage();
			this.tabFloor = new System.Windows.Forms.TabPage();
			this.gridFloor = new System.Windows.Forms.DataGrid();
			this.tabPage1 = new System.Windows.Forms.TabPage();
			this.grid2d_p = new System.Windows.Forms.DataGrid();
			this.tabPage2 = new System.Windows.Forms.TabPage();
			this.grid2d_c = new System.Windows.Forms.DataGrid();
			this.tabPage3 = new System.Windows.Forms.TabPage();
			this.gridReport1 = new System.Windows.Forms.DataGrid();
			this.gridReport2 = new System.Windows.Forms.DataGrid();
			this.tabReport = new System.Windows.Forms.TabPage();
			this.crystalReportViewer1 = new CrystalDecisions.Windows.Forms.CrystalReportViewer();
			this.tabTraject = new System.Windows.Forms.TabPage();
			this.panel1 = new System.Windows.Forms.Panel();
			this.label2 = new System.Windows.Forms.Label();
			this.maxY = new System.Windows.Forms.NumericUpDown();
			this.checkBox1 = new System.Windows.Forms.CheckBox();
			this.checkBox2 = new System.Windows.Forms.CheckBox();
			this.checkBox4 = new System.Windows.Forms.CheckBox();
			this.checkBox3 = new System.Windows.Forms.CheckBox();
			this.checkBox6 = new System.Windows.Forms.CheckBox();
			this.checkBox7 = new System.Windows.Forms.CheckBox();
			this.checkBox5 = new System.Windows.Forms.CheckBox();
			this.checkBox8 = new System.Windows.Forms.CheckBox();
			this.tabTr = new System.Windows.Forms.TabPage();
			this.gridTrajectory = new System.Windows.Forms.DataGrid();
			this.tabSimul = new System.Windows.Forms.TabPage();
			this.gridTraffic = new System.Windows.Forms.DataGrid();
			this.panel2 = new System.Windows.Forms.Panel();
			this.checkBox11 = new System.Windows.Forms.CheckBox();
			this.checkBox10 = new System.Windows.Forms.CheckBox();
			this.checkBox9 = new System.Windows.Forms.CheckBox();
			this.tbSimul = new System.Windows.Forms.ToolBar();
			this.toolBarButton1 = new System.Windows.Forms.ToolBarButton();
			this.toolBarButton2 = new System.Windows.Forms.ToolBarButton();
			this.toolBarButton3 = new System.Windows.Forms.ToolBarButton();
			this.toolBarButton4 = new System.Windows.Forms.ToolBarButton();
			this.toolBarButton5 = new System.Windows.Forms.ToolBarButton();
			this.tabConfig = new System.Windows.Forms.TabPage();
			this.txtVelL = new System.Windows.Forms.TextBox();
			this.lbVelL = new System.Windows.Forms.Label();
			this.lbAccL = new System.Windows.Forms.Label();
			this.txtAccL = new System.Windows.Forms.TextBox();
			this.txtJerkL = new System.Windows.Forms.TextBox();
			this.lbJerkL = new System.Windows.Forms.Label();
			this.lbmsL = new System.Windows.Forms.Label();
			this.lbmssL = new System.Windows.Forms.Label();
			this.lbmsssL = new System.Windows.Forms.Label();
			this.lbSafeL = new System.Windows.Forms.Label();
			this.lbGL = new System.Windows.Forms.Label();
			this.txtSafeGL = new System.Windows.Forms.TextBox();
			this.txtVelU = new System.Windows.Forms.TextBox();
			this.txtAccU = new System.Windows.Forms.TextBox();
			this.txtJerkU = new System.Windows.Forms.TextBox();
			this.txtSafeGU = new System.Windows.Forms.TextBox();
			this.btnReport = new System.Windows.Forms.Button();
			this.label1 = new System.Windows.Forms.Label();
			this.txtLobby = new System.Windows.Forms.TextBox();
			this.textBox8 = new System.Windows.Forms.TextBox();
			this.label19 = new System.Windows.Forms.Label();
			this.label20 = new System.Windows.Forms.Label();
			this.label21 = new System.Windows.Forms.Label();
			this.label22 = new System.Windows.Forms.Label();
			this.label23 = new System.Windows.Forms.Label();
			this.label24 = new System.Windows.Forms.Label();
			this.textBox10 = new System.Windows.Forms.TextBox();
			this.textBox11 = new System.Windows.Forms.TextBox();
			this.textBox12 = new System.Windows.Forms.TextBox();
			this.label25 = new System.Windows.Forms.Label();
			this.label26 = new System.Windows.Forms.Label();
			this.gridFloorHeight = new System.Windows.Forms.DataGrid();
			this.label3 = new System.Windows.Forms.Label();
			this.label4 = new System.Windows.Forms.Label();
			this.label5 = new System.Windows.Forms.Label();
			this.label6 = new System.Windows.Forms.Label();
			this.label7 = new System.Windows.Forms.Label();
			this.label8 = new System.Windows.Forms.Label();
			this.label9 = new System.Windows.Forms.Label();
			this.label10 = new System.Windows.Forms.Label();
			this.tabMonitor = new System.Windows.Forms.TabPage();
			this.gridPassenger = new System.Windows.Forms.DataGrid();
			this.tabCalc = new System.Windows.Forms.TabPage();
			this.gridCalc = new System.Windows.Forms.DataGrid();
			this.statusBar1 = new System.Windows.Forms.StatusBar();
			this.imageList1 = new System.Windows.Forms.ImageList(this.components);
			((System.ComponentModel.ISupportInitialize)(this.gridDebug)).BeginInit();
			this.tabDebug.SuspendLayout();
			this.tabParam.SuspendLayout();
			this.tabFloor.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.gridFloor)).BeginInit();
			this.tabPage1.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.grid2d_p)).BeginInit();
			this.tabPage2.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.grid2d_c)).BeginInit();
			this.tabPage3.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.gridReport1)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.gridReport2)).BeginInit();
			this.tabReport.SuspendLayout();
			this.tabTraject.SuspendLayout();
			this.panel1.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.maxY)).BeginInit();
			this.tabTr.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.gridTrajectory)).BeginInit();
			this.tabSimul.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.gridTraffic)).BeginInit();
			this.panel2.SuspendLayout();
			this.tabConfig.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.gridFloorHeight)).BeginInit();
			this.tabMonitor.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.gridPassenger)).BeginInit();
			this.tabCalc.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.gridCalc)).BeginInit();
			this.SuspendLayout();
			// 
			// gridDebug
			// 
			this.gridDebug.DataMember = "";
			this.gridDebug.Dock = System.Windows.Forms.DockStyle.Fill;
			this.gridDebug.HeaderForeColor = System.Drawing.SystemColors.ControlText;
			this.gridDebug.Location = new System.Drawing.Point(0, 0);
			this.gridDebug.Name = "gridDebug";
			this.gridDebug.Size = new System.Drawing.Size(952, 572);
			this.gridDebug.TabIndex = 0;
			this.gridDebug.Navigate += new System.Windows.Forms.NavigateEventHandler(this.gridDebug_Navigate);
			// 
			// timer1
			// 
			this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
			// 
			// tabDebug
			// 
			this.tabDebug.Controls.Add(this.tabParam);
			this.tabDebug.Controls.Add(this.tabFloor);
			this.tabDebug.Controls.Add(this.tabPage1);
			this.tabDebug.Controls.Add(this.tabPage2);
			this.tabDebug.Controls.Add(this.tabPage3);
			this.tabDebug.Controls.Add(this.tabReport);
			this.tabDebug.Controls.Add(this.tabTraject);
			this.tabDebug.Controls.Add(this.tabTr);
			this.tabDebug.Controls.Add(this.tabSimul);
			this.tabDebug.Controls.Add(this.tabConfig);
			this.tabDebug.Controls.Add(this.tabMonitor);
			this.tabDebug.Controls.Add(this.tabCalc);
			this.tabDebug.Dock = System.Windows.Forms.DockStyle.Fill;
			this.tabDebug.Location = new System.Drawing.Point(0, 0);
			this.tabDebug.Multiline = true;
			this.tabDebug.Name = "tabDebug";
			this.tabDebug.SelectedIndex = 0;
			this.tabDebug.Size = new System.Drawing.Size(960, 597);
			this.tabDebug.TabIndex = 1;
			this.tabDebug.SelectedIndexChanged += new System.EventHandler(this.tabControl1_SelectedIndexChanged);
			// 
			// tabParam
			// 
			this.tabParam.Controls.Add(this.gridDebug);
			this.tabParam.Location = new System.Drawing.Point(4, 21);
			this.tabParam.Name = "tabParam";
			this.tabParam.Size = new System.Drawing.Size(952, 572);
			this.tabParam.TabIndex = 0;
			this.tabParam.Text = "Parameter";
			// 
			// tabFloor
			// 
			this.tabFloor.Controls.Add(this.gridFloor);
			this.tabFloor.Location = new System.Drawing.Point(4, 21);
			this.tabFloor.Name = "tabFloor";
			this.tabFloor.Size = new System.Drawing.Size(952, 572);
			this.tabFloor.TabIndex = 1;
			this.tabFloor.Text = "Floor";
			// 
			// gridFloor
			// 
			this.gridFloor.DataMember = "";
			this.gridFloor.Dock = System.Windows.Forms.DockStyle.Fill;
			this.gridFloor.HeaderForeColor = System.Drawing.SystemColors.ControlText;
			this.gridFloor.Location = new System.Drawing.Point(0, 0);
			this.gridFloor.Name = "gridFloor";
			this.gridFloor.Size = new System.Drawing.Size(952, 572);
			this.gridFloor.TabIndex = 0;
			// 
			// tabPage1
			// 
			this.tabPage1.Controls.Add(this.grid2d_p);
			this.tabPage1.Location = new System.Drawing.Point(4, 21);
			this.tabPage1.Name = "tabPage1";
			this.tabPage1.Size = new System.Drawing.Size(952, 572);
			this.tabPage1.TabIndex = 2;
			this.tabPage1.Text = "2d-p";
			// 
			// grid2d_p
			// 
			this.grid2d_p.DataMember = "";
			this.grid2d_p.Dock = System.Windows.Forms.DockStyle.Fill;
			this.grid2d_p.HeaderForeColor = System.Drawing.SystemColors.ControlText;
			this.grid2d_p.Location = new System.Drawing.Point(0, 0);
			this.grid2d_p.Name = "grid2d_p";
			this.grid2d_p.Size = new System.Drawing.Size(952, 572);
			this.grid2d_p.TabIndex = 0;
			// 
			// tabPage2
			// 
			this.tabPage2.Controls.Add(this.grid2d_c);
			this.tabPage2.Location = new System.Drawing.Point(4, 21);
			this.tabPage2.Name = "tabPage2";
			this.tabPage2.Size = new System.Drawing.Size(952, 572);
			this.tabPage2.TabIndex = 3;
			this.tabPage2.Text = "2d-c";
			// 
			// grid2d_c
			// 
			this.grid2d_c.DataMember = "";
			this.grid2d_c.Dock = System.Windows.Forms.DockStyle.Fill;
			this.grid2d_c.HeaderForeColor = System.Drawing.SystemColors.ControlText;
			this.grid2d_c.Location = new System.Drawing.Point(0, 0);
			this.grid2d_c.Name = "grid2d_c";
			this.grid2d_c.Size = new System.Drawing.Size(952, 572);
			this.grid2d_c.TabIndex = 0;
			// 
			// tabPage3
			// 
			this.tabPage3.Controls.Add(this.gridReport1);
			this.tabPage3.Controls.Add(this.gridReport2);
			this.tabPage3.Location = new System.Drawing.Point(4, 21);
			this.tabPage3.Name = "tabPage3";
			this.tabPage3.Size = new System.Drawing.Size(952, 572);
			this.tabPage3.TabIndex = 4;
			this.tabPage3.Text = "result";
			// 
			// gridReport1
			// 
			this.gridReport1.DataMember = "";
			this.gridReport1.HeaderForeColor = System.Drawing.SystemColors.ControlText;
			this.gridReport1.Location = new System.Drawing.Point(24, 16);
			this.gridReport1.Name = "gridReport1";
			this.gridReport1.Size = new System.Drawing.Size(408, 328);
			this.gridReport1.TabIndex = 0;
			// 
			// gridReport2
			// 
			this.gridReport2.DataMember = "";
			this.gridReport2.HeaderForeColor = System.Drawing.SystemColors.ControlText;
			this.gridReport2.Location = new System.Drawing.Point(448, 16);
			this.gridReport2.Name = "gridReport2";
			this.gridReport2.Size = new System.Drawing.Size(448, 328);
			this.gridReport2.TabIndex = 0;
			// 
			// tabReport
			// 
			this.tabReport.Controls.Add(this.crystalReportViewer1);
			this.tabReport.Location = new System.Drawing.Point(4, 21);
			this.tabReport.Name = "tabReport";
			this.tabReport.Size = new System.Drawing.Size(952, 572);
			this.tabReport.TabIndex = 5;
			this.tabReport.Text = "Report";
			// 
			// crystalReportViewer1
			// 
			this.crystalReportViewer1.ActiveViewIndex = -1;
			this.crystalReportViewer1.DisplayGroupTree = false;
			this.crystalReportViewer1.DisplayToolbar = false;
			this.crystalReportViewer1.Dock = System.Windows.Forms.DockStyle.Fill;
			this.crystalReportViewer1.Location = new System.Drawing.Point(0, 0);
			this.crystalReportViewer1.Name = "crystalReportViewer1";
			this.crystalReportViewer1.ReportSource = null;
			this.crystalReportViewer1.Size = new System.Drawing.Size(952, 572);
			this.crystalReportViewer1.TabIndex = 0;
			// 
			// tabTraject
			// 
			this.tabTraject.BackColor = System.Drawing.SystemColors.ActiveCaptionText;
			this.tabTraject.Controls.Add(this.panel1);
			this.tabTraject.Location = new System.Drawing.Point(4, 21);
			this.tabTraject.Name = "tabTraject";
			this.tabTraject.Size = new System.Drawing.Size(952, 572);
			this.tabTraject.TabIndex = 6;
			this.tabTraject.Text = "Trajectory";
			this.tabTraject.Resize += new System.EventHandler(this.tabTraject_Resize);
			this.tabTraject.Paint += new System.Windows.Forms.PaintEventHandler(this.tabTraject_Paint);
			// 
			// panel1
			// 
			this.panel1.BackColor = System.Drawing.SystemColors.AppWorkspace;
			this.panel1.Controls.Add(this.label2);
			this.panel1.Controls.Add(this.maxY);
			this.panel1.Controls.Add(this.checkBox1);
			this.panel1.Controls.Add(this.checkBox2);
			this.panel1.Controls.Add(this.checkBox4);
			this.panel1.Controls.Add(this.checkBox3);
			this.panel1.Controls.Add(this.checkBox6);
			this.panel1.Controls.Add(this.checkBox7);
			this.panel1.Controls.Add(this.checkBox5);
			this.panel1.Controls.Add(this.checkBox8);
			this.panel1.Dock = System.Windows.Forms.DockStyle.Top;
			this.panel1.Location = new System.Drawing.Point(0, 0);
			this.panel1.Name = "panel1";
			this.panel1.Size = new System.Drawing.Size(952, 40);
			this.panel1.TabIndex = 0;
			// 
			// label2
			// 
			this.label2.Location = new System.Drawing.Point(66, 13);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(40, 16);
			this.label2.TabIndex = 14;
			this.label2.Text = "maxY";
			// 
			// maxY
			// 
			this.maxY.Location = new System.Drawing.Point(120, 8);
			this.maxY.Name = "maxY";
			this.maxY.Size = new System.Drawing.Size(56, 21);
			this.maxY.TabIndex = 13;
			this.maxY.Value = new System.Decimal(new int[] {
															   50,
															   0,
															   0,
															   0});
			// 
			// checkBox1
			// 
			this.checkBox1.Appearance = System.Windows.Forms.Appearance.Button;
			this.checkBox1.BackColor = System.Drawing.Color.IndianRed;
			this.checkBox1.Location = new System.Drawing.Point(544, 8);
			this.checkBox1.Name = "checkBox1";
			this.checkBox1.Size = new System.Drawing.Size(48, 24);
			this.checkBox1.TabIndex = 10;
			this.checkBox1.Text = "CAR1";
			this.checkBox1.CheckedChanged += new System.EventHandler(this.checkBox1_CheckedChanged);
			// 
			// checkBox2
			// 
			this.checkBox2.Appearance = System.Windows.Forms.Appearance.Button;
			this.checkBox2.BackColor = System.Drawing.Color.IndianRed;
			this.checkBox2.Location = new System.Drawing.Point(592, 8);
			this.checkBox2.Name = "checkBox2";
			this.checkBox2.Size = new System.Drawing.Size(48, 24);
			this.checkBox2.TabIndex = 9;
			this.checkBox2.Text = "CAR2";
			this.checkBox2.CheckedChanged += new System.EventHandler(this.checkBox1_CheckedChanged);
			// 
			// checkBox4
			// 
			this.checkBox4.Appearance = System.Windows.Forms.Appearance.Button;
			this.checkBox4.BackColor = System.Drawing.Color.IndianRed;
			this.checkBox4.Location = new System.Drawing.Point(688, 8);
			this.checkBox4.Name = "checkBox4";
			this.checkBox4.Size = new System.Drawing.Size(48, 24);
			this.checkBox4.TabIndex = 12;
			this.checkBox4.Text = "CAR4";
			this.checkBox4.CheckedChanged += new System.EventHandler(this.checkBox1_CheckedChanged);
			// 
			// checkBox3
			// 
			this.checkBox3.Appearance = System.Windows.Forms.Appearance.Button;
			this.checkBox3.BackColor = System.Drawing.Color.IndianRed;
			this.checkBox3.Location = new System.Drawing.Point(640, 8);
			this.checkBox3.Name = "checkBox3";
			this.checkBox3.Size = new System.Drawing.Size(48, 24);
			this.checkBox3.TabIndex = 11;
			this.checkBox3.Text = "CAR3";
			this.checkBox3.CheckedChanged += new System.EventHandler(this.checkBox1_CheckedChanged);
			// 
			// checkBox6
			// 
			this.checkBox6.Appearance = System.Windows.Forms.Appearance.Button;
			this.checkBox6.BackColor = System.Drawing.Color.IndianRed;
			this.checkBox6.Location = new System.Drawing.Point(784, 8);
			this.checkBox6.Name = "checkBox6";
			this.checkBox6.Size = new System.Drawing.Size(48, 24);
			this.checkBox6.TabIndex = 6;
			this.checkBox6.Text = "CAR6";
			this.checkBox6.CheckedChanged += new System.EventHandler(this.checkBox1_CheckedChanged);
			// 
			// checkBox7
			// 
			this.checkBox7.Appearance = System.Windows.Forms.Appearance.Button;
			this.checkBox7.BackColor = System.Drawing.Color.IndianRed;
			this.checkBox7.Location = new System.Drawing.Point(832, 8);
			this.checkBox7.Name = "checkBox7";
			this.checkBox7.Size = new System.Drawing.Size(48, 24);
			this.checkBox7.TabIndex = 5;
			this.checkBox7.Text = "CAR7";
			this.checkBox7.CheckedChanged += new System.EventHandler(this.checkBox1_CheckedChanged);
			// 
			// checkBox5
			// 
			this.checkBox5.Appearance = System.Windows.Forms.Appearance.Button;
			this.checkBox5.BackColor = System.Drawing.Color.IndianRed;
			this.checkBox5.Location = new System.Drawing.Point(736, 8);
			this.checkBox5.Name = "checkBox5";
			this.checkBox5.Size = new System.Drawing.Size(48, 24);
			this.checkBox5.TabIndex = 8;
			this.checkBox5.Text = "CAR5";
			this.checkBox5.CheckedChanged += new System.EventHandler(this.checkBox1_CheckedChanged);
			// 
			// checkBox8
			// 
			this.checkBox8.Appearance = System.Windows.Forms.Appearance.Button;
			this.checkBox8.BackColor = System.Drawing.Color.IndianRed;
			this.checkBox8.Location = new System.Drawing.Point(880, 8);
			this.checkBox8.Name = "checkBox8";
			this.checkBox8.Size = new System.Drawing.Size(48, 24);
			this.checkBox8.TabIndex = 7;
			this.checkBox8.Text = "CAR8";
			this.checkBox8.CheckedChanged += new System.EventHandler(this.checkBox1_CheckedChanged);
			// 
			// tabTr
			// 
			this.tabTr.Controls.Add(this.gridTrajectory);
			this.tabTr.Location = new System.Drawing.Point(4, 21);
			this.tabTr.Name = "tabTr";
			this.tabTr.Size = new System.Drawing.Size(952, 572);
			this.tabTr.TabIndex = 7;
			this.tabTr.Text = "TableTr";
			// 
			// gridTrajectory
			// 
			this.gridTrajectory.DataMember = "";
			this.gridTrajectory.Dock = System.Windows.Forms.DockStyle.Fill;
			this.gridTrajectory.HeaderForeColor = System.Drawing.SystemColors.ControlText;
			this.gridTrajectory.Location = new System.Drawing.Point(0, 0);
			this.gridTrajectory.Name = "gridTrajectory";
			this.gridTrajectory.Size = new System.Drawing.Size(952, 572);
			this.gridTrajectory.TabIndex = 0;
			// 
			// tabSimul
			// 
			this.tabSimul.Controls.Add(this.gridTraffic);
			this.tabSimul.Controls.Add(this.panel2);
			this.tabSimul.Location = new System.Drawing.Point(4, 21);
			this.tabSimul.Name = "tabSimul";
			this.tabSimul.Size = new System.Drawing.Size(952, 572);
			this.tabSimul.TabIndex = 8;
			this.tabSimul.Tag = "\"FIRST\"";
			this.tabSimul.Text = "TSA_DELAY";
			this.tabSimul.Click += new System.EventHandler(this.tabSimul_Click);
			// 
			// gridTraffic
			// 
			this.gridTraffic.DataMember = "";
			this.gridTraffic.HeaderForeColor = System.Drawing.SystemColors.ControlText;
			this.gridTraffic.Location = new System.Drawing.Point(16, 24);
			this.gridTraffic.Name = "gridTraffic";
			this.gridTraffic.Size = new System.Drawing.Size(192, 488);
			this.gridTraffic.TabIndex = 3;
			// 
			// panel2
			// 
			this.panel2.BackColor = System.Drawing.Color.White;
			this.panel2.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.panel2.Controls.Add(this.checkBox11);
			this.panel2.Controls.Add(this.checkBox10);
			this.panel2.Controls.Add(this.checkBox9);
			this.panel2.Controls.Add(this.tbSimul);
			this.panel2.Location = new System.Drawing.Point(352, 16);
			this.panel2.Name = "panel2";
			this.panel2.Size = new System.Drawing.Size(592, 504);
			this.panel2.TabIndex = 2;
			// 
			// checkBox11
			// 
			this.checkBox11.Location = new System.Drawing.Point(328, 4);
			this.checkBox11.Name = "checkBox11";
			this.checkBox11.Size = new System.Drawing.Size(72, 24);
			this.checkBox11.TabIndex = 3;
			this.checkBox11.Text = "NSF(U)";
			// 
			// checkBox10
			// 
			this.checkBox10.Location = new System.Drawing.Point(248, 4);
			this.checkBox10.Name = "checkBox10";
			this.checkBox10.Size = new System.Drawing.Size(72, 24);
			this.checkBox10.TabIndex = 2;
			this.checkBox10.Text = "NSP(U)";
			// 
			// checkBox9
			// 
			this.checkBox9.Location = new System.Drawing.Point(168, 4);
			this.checkBox9.Name = "checkBox9";
			this.checkBox9.Size = new System.Drawing.Size(72, 24);
			this.checkBox9.TabIndex = 1;
			this.checkBox9.Text = "NCF(U)";
			// 
			// tbSimul
			// 
			this.tbSimul.Buttons.AddRange(new System.Windows.Forms.ToolBarButton[] {
																					   this.toolBarButton1,
																					   this.toolBarButton2,
																					   this.toolBarButton3,
																					   this.toolBarButton4,
																					   this.toolBarButton5});
			this.tbSimul.DropDownArrows = true;
			this.tbSimul.Location = new System.Drawing.Point(0, 0);
			this.tbSimul.Name = "tbSimul";
			this.tbSimul.ShowToolTips = true;
			this.tbSimul.Size = new System.Drawing.Size(588, 42);
			this.tbSimul.TabIndex = 0;
			this.tbSimul.ButtonClick += new System.Windows.Forms.ToolBarButtonClickEventHandler(this.tbSimul_ButtonClick);
			// 
			// toolBarButton5
			// 
			this.toolBarButton5.Text = "Run";
			// 
			// tabConfig
			// 
			this.tabConfig.Controls.Add(this.txtVelL);
			this.tabConfig.Controls.Add(this.lbVelL);
			this.tabConfig.Controls.Add(this.lbAccL);
			this.tabConfig.Controls.Add(this.txtAccL);
			this.tabConfig.Controls.Add(this.txtJerkL);
			this.tabConfig.Controls.Add(this.lbJerkL);
			this.tabConfig.Controls.Add(this.lbmsL);
			this.tabConfig.Controls.Add(this.lbmssL);
			this.tabConfig.Controls.Add(this.lbmsssL);
			this.tabConfig.Controls.Add(this.lbSafeL);
			this.tabConfig.Controls.Add(this.lbGL);
			this.tabConfig.Controls.Add(this.txtSafeGL);
			this.tabConfig.Controls.Add(this.txtVelU);
			this.tabConfig.Controls.Add(this.txtAccU);
			this.tabConfig.Controls.Add(this.txtJerkU);
			this.tabConfig.Controls.Add(this.txtSafeGU);
			this.tabConfig.Controls.Add(this.btnReport);
			this.tabConfig.Controls.Add(this.label1);
			this.tabConfig.Controls.Add(this.txtLobby);
			this.tabConfig.Controls.Add(this.textBox8);
			this.tabConfig.Controls.Add(this.label19);
			this.tabConfig.Controls.Add(this.label20);
			this.tabConfig.Controls.Add(this.label21);
			this.tabConfig.Controls.Add(this.label22);
			this.tabConfig.Controls.Add(this.label23);
			this.tabConfig.Controls.Add(this.label24);
			this.tabConfig.Controls.Add(this.textBox10);
			this.tabConfig.Controls.Add(this.textBox11);
			this.tabConfig.Controls.Add(this.textBox12);
			this.tabConfig.Controls.Add(this.label25);
			this.tabConfig.Controls.Add(this.label26);
			this.tabConfig.Controls.Add(this.gridFloorHeight);
			this.tabConfig.Controls.Add(this.label3);
			this.tabConfig.Controls.Add(this.label4);
			this.tabConfig.Controls.Add(this.label5);
			this.tabConfig.Controls.Add(this.label6);
			this.tabConfig.Controls.Add(this.label7);
			this.tabConfig.Controls.Add(this.label8);
			this.tabConfig.Controls.Add(this.label9);
			this.tabConfig.Controls.Add(this.label10);
			this.tabConfig.Location = new System.Drawing.Point(4, 21);
			this.tabConfig.Name = "tabConfig";
			this.tabConfig.Size = new System.Drawing.Size(952, 572);
			this.tabConfig.TabIndex = 9;
			this.tabConfig.Text = "Config";
			// 
			// txtVelL
			// 
			this.txtVelL.Location = new System.Drawing.Point(448, 384);
			this.txtVelL.Name = "txtVelL";
			this.txtVelL.Size = new System.Drawing.Size(48, 21);
			this.txtVelL.TabIndex = 39;
			this.txtVelL.Text = "";
			// 
			// lbVelL
			// 
			this.lbVelL.Location = new System.Drawing.Point(408, 392);
			this.lbVelL.Name = "lbVelL";
			this.lbVelL.Size = new System.Drawing.Size(32, 16);
			this.lbVelL.TabIndex = 38;
			this.lbVelL.Text = "Vel";
			// 
			// lbAccL
			// 
			this.lbAccL.Location = new System.Drawing.Point(408, 416);
			this.lbAccL.Name = "lbAccL";
			this.lbAccL.Size = new System.Drawing.Size(32, 16);
			this.lbAccL.TabIndex = 36;
			this.lbAccL.Text = "Acc";
			// 
			// txtAccL
			// 
			this.txtAccL.Location = new System.Drawing.Point(448, 408);
			this.txtAccL.Name = "txtAccL";
			this.txtAccL.Size = new System.Drawing.Size(48, 21);
			this.txtAccL.TabIndex = 41;
			this.txtAccL.Text = "";
			// 
			// txtJerkL
			// 
			this.txtJerkL.Location = new System.Drawing.Point(448, 432);
			this.txtJerkL.Name = "txtJerkL";
			this.txtJerkL.Size = new System.Drawing.Size(48, 21);
			this.txtJerkL.TabIndex = 40;
			this.txtJerkL.Text = "";
			// 
			// lbJerkL
			// 
			this.lbJerkL.Location = new System.Drawing.Point(408, 432);
			this.lbJerkL.Name = "lbJerkL";
			this.lbJerkL.Size = new System.Drawing.Size(32, 16);
			this.lbJerkL.TabIndex = 37;
			this.lbJerkL.Text = "Jerk";
			// 
			// lbmsL
			// 
			this.lbmsL.Location = new System.Drawing.Point(504, 392);
			this.lbmsL.Name = "lbmsL";
			this.lbmsL.Size = new System.Drawing.Size(32, 16);
			this.lbmsL.TabIndex = 32;
			this.lbmsL.Text = "m/s";
			// 
			// lbmssL
			// 
			this.lbmssL.Location = new System.Drawing.Point(504, 416);
			this.lbmssL.Name = "lbmssL";
			this.lbmssL.Size = new System.Drawing.Size(40, 16);
			this.lbmssL.TabIndex = 31;
			this.lbmssL.Text = "m/ss";
			// 
			// lbmsssL
			// 
			this.lbmsssL.Location = new System.Drawing.Point(504, 432);
			this.lbmsssL.Name = "lbmsssL";
			this.lbmsssL.Size = new System.Drawing.Size(40, 16);
			this.lbmsssL.TabIndex = 30;
			this.lbmsssL.Text = "m/sss";
			// 
			// lbSafeL
			// 
			this.lbSafeL.Location = new System.Drawing.Point(408, 456);
			this.lbSafeL.Name = "lbSafeL";
			this.lbSafeL.Size = new System.Drawing.Size(32, 16);
			this.lbSafeL.TabIndex = 33;
			this.lbSafeL.Text = "Safe";
			// 
			// lbGL
			// 
			this.lbGL.Location = new System.Drawing.Point(504, 456);
			this.lbGL.Name = "lbGL";
			this.lbGL.Size = new System.Drawing.Size(40, 16);
			this.lbGL.TabIndex = 34;
			this.lbGL.Text = "g";
			// 
			// txtSafeGL
			// 
			this.txtSafeGL.Location = new System.Drawing.Point(448, 456);
			this.txtSafeGL.Name = "txtSafeGL";
			this.txtSafeGL.Size = new System.Drawing.Size(48, 21);
			this.txtSafeGL.TabIndex = 35;
			this.txtSafeGL.Text = "";
			// 
			// txtVelU
			// 
			this.txtVelU.Location = new System.Drawing.Point(448, 238);
			this.txtVelU.Name = "txtVelU";
			this.txtVelU.Size = new System.Drawing.Size(48, 21);
			this.txtVelU.TabIndex = 26;
			this.txtVelU.Text = "";
			// 
			// txtAccU
			// 
			this.txtAccU.Location = new System.Drawing.Point(448, 262);
			this.txtAccU.Name = "txtAccU";
			this.txtAccU.Size = new System.Drawing.Size(48, 21);
			this.txtAccU.TabIndex = 28;
			this.txtAccU.Text = "";
			// 
			// txtJerkU
			// 
			this.txtJerkU.Location = new System.Drawing.Point(448, 286);
			this.txtJerkU.Name = "txtJerkU";
			this.txtJerkU.Size = new System.Drawing.Size(48, 21);
			this.txtJerkU.TabIndex = 29;
			this.txtJerkU.Text = "";
			// 
			// txtSafeGU
			// 
			this.txtSafeGU.Location = new System.Drawing.Point(448, 310);
			this.txtSafeGU.Name = "txtSafeGU";
			this.txtSafeGU.Size = new System.Drawing.Size(48, 21);
			this.txtSafeGU.TabIndex = 27;
			this.txtSafeGU.Text = "";
			// 
			// btnReport
			// 
			this.btnReport.Location = new System.Drawing.Point(584, 136);
			this.btnReport.Name = "btnReport";
			this.btnReport.Size = new System.Drawing.Size(72, 32);
			this.btnReport.TabIndex = 17;
			this.btnReport.Text = "report";
			// 
			// label1
			// 
			this.label1.Location = new System.Drawing.Point(400, 136);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(80, 16);
			this.label1.TabIndex = 16;
			this.label1.Text = "lobby_pos :";
			// 
			// txtLobby
			// 
			this.txtLobby.Location = new System.Drawing.Point(480, 136);
			this.txtLobby.Name = "txtLobby";
			this.txtLobby.Size = new System.Drawing.Size(80, 21);
			this.txtLobby.TabIndex = 15;
			this.txtLobby.Text = "0";
			// 
			// textBox8
			// 
			this.textBox8.Location = new System.Drawing.Point(440, 24);
			this.textBox8.Name = "textBox8";
			this.textBox8.Size = new System.Drawing.Size(48, 21);
			this.textBox8.TabIndex = 11;
			this.textBox8.Text = "";
			// 
			// label19
			// 
			this.label19.Location = new System.Drawing.Point(400, 32);
			this.label19.Name = "label19";
			this.label19.Size = new System.Drawing.Size(32, 16);
			this.label19.TabIndex = 8;
			this.label19.Text = "Vel";
			// 
			// label20
			// 
			this.label20.Location = new System.Drawing.Point(400, 48);
			this.label20.Name = "label20";
			this.label20.Size = new System.Drawing.Size(32, 16);
			this.label20.TabIndex = 9;
			this.label20.Text = "Acc";
			// 
			// label21
			// 
			this.label21.Location = new System.Drawing.Point(400, 72);
			this.label21.Name = "label21";
			this.label21.Size = new System.Drawing.Size(32, 16);
			this.label21.TabIndex = 10;
			this.label21.Text = "Jerk";
			// 
			// label22
			// 
			this.label22.Location = new System.Drawing.Point(496, 72);
			this.label22.Name = "label22";
			this.label22.Size = new System.Drawing.Size(40, 16);
			this.label22.TabIndex = 7;
			this.label22.Text = "m/sss";
			// 
			// label23
			// 
			this.label23.Location = new System.Drawing.Point(496, 48);
			this.label23.Name = "label23";
			this.label23.Size = new System.Drawing.Size(40, 16);
			this.label23.TabIndex = 4;
			this.label23.Text = "m/ss";
			// 
			// label24
			// 
			this.label24.Location = new System.Drawing.Point(496, 32);
			this.label24.Name = "label24";
			this.label24.Size = new System.Drawing.Size(32, 16);
			this.label24.TabIndex = 3;
			this.label24.Text = "m/s";
			// 
			// textBox10
			// 
			this.textBox10.Location = new System.Drawing.Point(440, 48);
			this.textBox10.Name = "textBox10";
			this.textBox10.Size = new System.Drawing.Size(48, 21);
			this.textBox10.TabIndex = 13;
			this.textBox10.Text = "";
			// 
			// textBox11
			// 
			this.textBox11.Location = new System.Drawing.Point(440, 72);
			this.textBox11.Name = "textBox11";
			this.textBox11.Size = new System.Drawing.Size(48, 21);
			this.textBox11.TabIndex = 14;
			this.textBox11.Text = "";
			// 
			// textBox12
			// 
			this.textBox12.Location = new System.Drawing.Point(440, 96);
			this.textBox12.Name = "textBox12";
			this.textBox12.Size = new System.Drawing.Size(48, 21);
			this.textBox12.TabIndex = 12;
			this.textBox12.Text = "";
			// 
			// label25
			// 
			this.label25.Location = new System.Drawing.Point(400, 96);
			this.label25.Name = "label25";
			this.label25.Size = new System.Drawing.Size(32, 16);
			this.label25.TabIndex = 6;
			this.label25.Text = "Safe";
			// 
			// label26
			// 
			this.label26.Location = new System.Drawing.Point(496, 96);
			this.label26.Name = "label26";
			this.label26.Size = new System.Drawing.Size(40, 16);
			this.label26.TabIndex = 5;
			this.label26.Text = "g";
			// 
			// gridFloorHeight
			// 
			this.gridFloorHeight.DataMember = "";
			this.gridFloorHeight.HeaderForeColor = System.Drawing.SystemColors.ControlText;
			this.gridFloorHeight.Location = new System.Drawing.Point(32, 24);
			this.gridFloorHeight.Name = "gridFloorHeight";
			this.gridFloorHeight.Size = new System.Drawing.Size(248, 512);
			this.gridFloorHeight.TabIndex = 0;
			// 
			// label3
			// 
			this.label3.Location = new System.Drawing.Point(400, 248);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(32, 16);
			this.label3.TabIndex = 38;
			this.label3.Text = "Vel";
			// 
			// label4
			// 
			this.label4.Location = new System.Drawing.Point(400, 272);
			this.label4.Name = "label4";
			this.label4.Size = new System.Drawing.Size(32, 16);
			this.label4.TabIndex = 36;
			this.label4.Text = "Acc";
			// 
			// label5
			// 
			this.label5.Location = new System.Drawing.Point(400, 288);
			this.label5.Name = "label5";
			this.label5.Size = new System.Drawing.Size(32, 16);
			this.label5.TabIndex = 37;
			this.label5.Text = "Jerk";
			// 
			// label6
			// 
			this.label6.Location = new System.Drawing.Point(400, 312);
			this.label6.Name = "label6";
			this.label6.Size = new System.Drawing.Size(32, 16);
			this.label6.TabIndex = 33;
			this.label6.Text = "Safe";
			// 
			// label7
			// 
			this.label7.Location = new System.Drawing.Point(512, 248);
			this.label7.Name = "label7";
			this.label7.Size = new System.Drawing.Size(32, 16);
			this.label7.TabIndex = 32;
			this.label7.Text = "m/s";
			// 
			// label8
			// 
			this.label8.Location = new System.Drawing.Point(512, 272);
			this.label8.Name = "label8";
			this.label8.Size = new System.Drawing.Size(40, 16);
			this.label8.TabIndex = 31;
			this.label8.Text = "m/ss";
			// 
			// label9
			// 
			this.label9.Location = new System.Drawing.Point(512, 288);
			this.label9.Name = "label9";
			this.label9.Size = new System.Drawing.Size(40, 16);
			this.label9.TabIndex = 30;
			this.label9.Text = "m/sss";
			// 
			// label10
			// 
			this.label10.Location = new System.Drawing.Point(512, 312);
			this.label10.Name = "label10";
			this.label10.Size = new System.Drawing.Size(40, 16);
			this.label10.TabIndex = 34;
			this.label10.Text = "g";
			// 
			// tabMonitor
			// 
			this.tabMonitor.Controls.Add(this.gridPassenger);
			this.tabMonitor.Location = new System.Drawing.Point(4, 21);
			this.tabMonitor.Name = "tabMonitor";
			this.tabMonitor.Size = new System.Drawing.Size(952, 572);
			this.tabMonitor.TabIndex = 11;
			this.tabMonitor.Text = "Monitor";
			this.tabMonitor.Click += new System.EventHandler(this.tabMonitor_Click);
			// 
			// gridPassenger
			// 
			this.gridPassenger.DataMember = "";
			this.gridPassenger.HeaderForeColor = System.Drawing.SystemColors.ControlText;
			this.gridPassenger.Location = new System.Drawing.Point(8, 24);
			this.gridPassenger.Name = "gridPassenger";
			this.gridPassenger.Size = new System.Drawing.Size(232, 536);
			this.gridPassenger.TabIndex = 0;
			// 
			// tabCalc
			// 
			this.tabCalc.Controls.Add(this.gridCalc);
			this.tabCalc.Location = new System.Drawing.Point(4, 21);
			this.tabCalc.Name = "tabCalc";
			this.tabCalc.Size = new System.Drawing.Size(952, 572);
			this.tabCalc.TabIndex = 10;
			this.tabCalc.Text = "Calc";
			// 
			// gridCalc
			// 
			this.gridCalc.DataMember = "";
			this.gridCalc.Dock = System.Windows.Forms.DockStyle.Fill;
			this.gridCalc.HeaderForeColor = System.Drawing.SystemColors.ControlText;
			this.gridCalc.Location = new System.Drawing.Point(0, 0);
			this.gridCalc.Name = "gridCalc";
			this.gridCalc.Size = new System.Drawing.Size(952, 572);
			this.gridCalc.TabIndex = 0;
			// 
			// statusBar1
			// 
			this.statusBar1.Location = new System.Drawing.Point(0, 575);
			this.statusBar1.Name = "statusBar1";
			this.statusBar1.ShowPanels = true;
			this.statusBar1.Size = new System.Drawing.Size(960, 22);
			this.statusBar1.TabIndex = 2;
			this.statusBar1.Text = "statusBar1";
			// 
			// imageList1
			// 
			this.imageList1.ImageSize = new System.Drawing.Size(32, 32);
			this.imageList1.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageList1.ImageStream")));
			this.imageList1.TransparentColor = System.Drawing.Color.Transparent;
			// 
			// Form1
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(6, 14);
			this.ClientSize = new System.Drawing.Size(960, 597);
			this.Controls.Add(this.statusBar1);
			this.Controls.Add(this.tabDebug);
			this.Name = "Form1";
			this.Text = "COGS_SPY";
			this.Closing += new System.ComponentModel.CancelEventHandler(this.Form1_Closing);
			this.Load += new System.EventHandler(this.Form1_Load);
			((System.ComponentModel.ISupportInitialize)(this.gridDebug)).EndInit();
			this.tabDebug.ResumeLayout(false);
			this.tabParam.ResumeLayout(false);
			this.tabFloor.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.gridFloor)).EndInit();
			this.tabPage1.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.grid2d_p)).EndInit();
			this.tabPage2.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.grid2d_c)).EndInit();
			this.tabPage3.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.gridReport1)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.gridReport2)).EndInit();
			this.tabReport.ResumeLayout(false);
			this.tabTraject.ResumeLayout(false);
			this.panel1.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.maxY)).EndInit();
			this.tabTr.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.gridTrajectory)).EndInit();
			this.tabSimul.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.gridTraffic)).EndInit();
			this.panel2.ResumeLayout(false);
			this.tabConfig.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.gridFloorHeight)).EndInit();
			this.tabMonitor.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.gridPassenger)).EndInit();
			this.tabCalc.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.gridCalc)).EndInit();
			this.ResumeLayout(false);

		}
		#endregion

		/// <summary>
		/// 해당 응용 프로그램의 주 진입점입니다.
		/// </summary>
		[STAThread]
		static void Main() 
		{
			Application.Run(new Form1());
		}

		public void ShowParam()
		{
			int rowInx=0;
			try
			{
				StreamReader sr=new StreamReader(@"c:\debug.csv");
				while (sr.Peek() >= 0) 
				{
					string str=sr.ReadLine();
					string[] strArr=str.Split(',');
					if(debugDs.values.Rows.Count<=rowInx) //열이 없으면
					{
						DataRow row=debugDs.values.NewRow();
						for(int i=0;i<MAX_OCSS;i++)
						{
							row[i]=strArr[i];
						}
						debugDs.values.Rows.Add(row);
					}
					else
					{
						for(int i=0;i<MAX_OCSS;i++)
						{
							if(!debugDs.values.Rows[rowInx][i].Equals(strArr[i]))
							{
								debugDs.values.Rows[rowInx][i]=strArr[i];
							}
						}
					}
					rowInx++;
				}
				sr.Close();
			}
			catch(Exception eee)
			{
				MessageBox.Show(eee.Message);
			}
		}
		
		public void ShowFloor()
		{
			int rowInx=0;
			try
			{
				using (StreamReader sr=new StreamReader(@"c:\dFloor.csv"))
				{
					while (sr.Peek() >= 0) 
					{
						string str=sr.ReadLine();
						string[] strArr=str.Split(',');
						if(debugDs.floor.Rows.Count<=rowInx) //열이 없으면
						{
							DataRow row=debugDs.floor.NewRow();
							for(int i=0;i<MAX_OCSS;i++)
							{
								if(i==0)
									row[i]=strArr[i];
								else
								{
									string strTmp="";
									if(strArr[i].IndexOf("HU",0)>=0)
										strTmp+="▲";
									if(strArr[i].IndexOf("HD",0)>=0)
										strTmp+="▼";
									if(strArr[i].IndexOf("CC",0)>=0)
										strTmp+="●";
									if(strArr[i].IndexOf("DU",0)>=0)
										strTmp+="∧";
									if(strArr[i].IndexOf("DD",0)>=0)
										strTmp+="∨";
									row[i]=strTmp;
								}
							}
							
							debugDs.floor.Rows.Add(row);
						}
						else
						{
							for(int i=0;i<MAX_OCSS;i++)
							{
								if(!debugDs.floor.Rows[rowInx][i].Equals(strArr[i]))
								{
									if(i==0)
										debugDs.floor.Rows[rowInx][i]=strArr[i];
									else
									{
										string strTmp="";
										if(strArr[i].IndexOf("HU",0)>=0)
											strTmp+="▲";
										if(strArr[i].IndexOf("HD",0)>=0)
											strTmp+="▼";
										if(strArr[i].IndexOf("CC",0)>=0)
											strTmp+="●";
										if(strArr[i].IndexOf("DU",0)>=0)
											strTmp+="∧";
										if(strArr[i].IndexOf("DD",0)>=0)
											strTmp+="∨";
										debugDs.floor.Rows[rowInx][i]=strTmp;
									}
								}
							}
						}
						rowInx++;
					}
				}
			}
			catch(Exception eee)
			{
				MessageBox.Show(eee.Message);
			}
		}

		public void UpdatePos()
		{
			try
			{
				using (StreamReader sr=new StreamReader(@"c:\dPos.csv"))
				{
					while (sr.Peek() >= 0) 
					{
						string str=sr.ReadLine();
						string[] strArr=str.Split(',');
						int lastInx=debugDs.pos.Rows.Count-1;
						if(lastInx>=0)
						{
							double lastTime=double.Parse((string)debugDs.pos.Rows[lastInx][0]);
							double curTime=double.Parse(strArr[0]);
							if((curTime-lastTime)<1 && (curTime-lastTime)>=0)
								continue;
							if((curTime-lastTime)<0)
							{
								lastInx=-1;
								debugDs.pos.Rows.Clear();
							}
						}
						DataRow row=debugDs.pos.NewRow();
						row[0]=strArr[0]; //time
						for(int i=1;i<MAX_OCSS;i++)
						{
							try
							{
								if(double.Parse(strArr[i])<0)
									row[i]="0";
								else if(double.Parse(strArr[i])>=(double)this.maxY.Value)
									row[i]=maxY.Value.ToString();
								else
									row[i]=strArr[i];
							}
							catch
							{
								row[i]="0";
							}
						}
						debugDs.pos.Rows.Add(row);
						if(debugDs.pos.Rows.Count>100)
							debugDs.pos.Rows.RemoveAt(0);
					}
				}
			}
			catch(Exception eee)
			{
				MessageBox.Show(eee.Message);
			}
		}

		protected override void  DefWndProc(ref Message msg)
		{
			switch(msg.Msg)
			{
				case 0x401://WM_QUERYENDSESSION
					UpdatePos();
					if(tabDebug.SelectedTab.Text.Equals("Parameter") || tabDebug.SelectedTab.Text.Equals("Floor"))
					{
						ShowParam();
						ShowFloor();
					}
					else if(tabDebug.SelectedTab.Text.Equals("Trajectory"))
						RefreshGrp();
					break;

			}
			base.DefWndProc(ref msg);
		}

		private void Form1_Load(object sender, System.EventArgs e)
		{
			this.CurrentTimeRange=100;
			debugDs=new dsDebug();
			reportDs=new dsReport();
			configDs=new dsConfig();
			monDs=new dsMon();

			oRpt=new CrystalReport1();
			try
			{
				xmlReader=new XmlTextReader("cogsd.xml");
				configDs.ReadXml(xmlReader,System.Data.XmlReadMode.Auto);
				xmlReader.Close();
			}
			catch(Exception ex)
			{
				MessageBox.Show(ex.Message);
			}
			
			this.gridFloorHeight.DataSource=configDs.FloorHeight;
			this.gridCalc.DataSource=debugDs.Calc;
			this.grid2d_p.DataSource=reportDs._2d_p;
			this.grid2d_c.DataSource=reportDs._2d_c;
			this.gridReport1.DataSource=reportDs.report_item1;
			this.gridReport2.DataSource=reportDs.report_item2;
			this.gridDebug.DataSource=debugDs.values;
			this.gridFloor.DataSource=debugDs.floor;
			this.gridTrajectory.DataSource=debugDs.pos;
			this.gridPassenger.DataSource=monDs.p_log;

			this.timer1.Enabled=true;
			using (StreamWriter sw = new StreamWriter(@"c:\handle.txt")) 
			{
				// Add some text to the file.
				sw.Write(this.Handle.ToString());
			}

			DataGridTableStyle ts = new DataGridTableStyle();
			// Set the DataGridTableStyle.MappingName property
			// to the table in the data source to map to.
			ts.MappingName = this.gridReport1.DataSource.ToString();
		
			// Add it to the datagrid's TableStyles collection
			gridReport1.TableStyles.Add(ts);

			// Hide the first column (index 0)
			gridReport1.TableStyles[0].GridColumnStyles[0].Width = 200;
			gridReport1.TableStyles[0].GridColumnStyles[1].Width = 100;

			colorArr=new Color[8];
			colorArr[0]=Color.FromArgb(100,100,100);
			colorArr[1]=Color.Blue;
			colorArr[2]=Color.Red;
			colorArr[3]=Color.Brown;
			colorArr[4]=Color.Cyan;
			colorArr[5]=Color.DarkCyan;
			colorArr[6]=Color.Violet;
			colorArr[7]=Color.Black;


			penArr=new Pen[8];
			penArr[0]=new Pen(colorArr[0],1);
			penArr[1]=new Pen(colorArr[1],1);
			penArr[2]=new Pen(colorArr[2],1);
			penArr[3]=new Pen(colorArr[3],1);
			penArr[4]=new Pen(colorArr[4],1);
			penArr[5]=new Pen(colorArr[5],1);
			penArr[6]=new Pen(colorArr[6],1);
			penArr[7]=new Pen(colorArr[7],1);
			
			//CheckBox Button 초기화
			checkArr=new CheckBox[8];
			checkArr[0]=checkBox1;
			checkArr[1]=checkBox2;
			checkArr[2]=checkBox3;
			checkArr[3]=checkBox4;
			checkArr[4]=checkBox5;
			checkArr[5]=checkBox6;
			checkArr[6]=checkBox7;
			checkArr[7]=checkBox8;
			for(int i=0;i<8;i++)
			{
				checkArr[i].BackColor=colorArr[i];
			}
			if(configDs.ocss_info.Rows.Count<2)
			{
				this.txtVelL.Text="2";
				this.txtAccL.Text="1";
				this.txtJerkL.Text="1";
				this.txtSafeGL.Text="0.3";
				this.txtVelU.Text="2";
				this.txtAccU.Text="1";
				this.txtJerkU.Text="1";
				this.txtSafeGU.Text="0.3";
				configDs.ocss_info.Rows.Add(new object[] {"2","1","1","0.3"});
				configDs.ocss_info.Rows.Add(new object[] {"2","1","1","0.3"});

			}
			this.txtVelL.DataBindings.Clear();
			this.txtAccL.DataBindings.Clear();
			this.txtJerkL.DataBindings.Clear();
			this.txtSafeGL.DataBindings.Clear();

			this.txtVelU.DataBindings.Clear();
			this.txtAccU.DataBindings.Clear();
			this.txtJerkU.DataBindings.Clear();
			this.txtSafeGU.DataBindings.Clear();
					
			this.txtVelL.DataBindings.Add(new Binding("Text",configDs.ocss_info.Rows[0],"Vel"));
			this.txtAccL.DataBindings.Add(new Binding("Text",configDs.ocss_info.Rows[0],"Acc"));
			this.txtJerkL.DataBindings.Add(new Binding("Text",configDs.ocss_info.Rows[0],"Jerk"));
			this.txtSafeGL.DataBindings.Add(new Binding("Text",configDs.ocss_info.Rows[0],"SafeG"));

			this.txtVelU.DataBindings.Add(new Binding("Text",configDs.ocss_info.Rows[1],"Vel"));
			this.txtAccU.DataBindings.Add(new Binding("Text",configDs.ocss_info.Rows[1],"Acc"));
			this.txtJerkU.DataBindings.Add(new Binding("Text",configDs.ocss_info.Rows[1],"Jerk"));
			this.txtSafeGU.DataBindings.Add(new Binding("Text",configDs.ocss_info.Rows[1],"SafeG"));
			if(configDs.FloorHeight.Rows.Count<1)
			{
				for(int i=0;i<40;i++)
				{
					int FloorHeight=4;
					int FloorPos=4*i;
					configDs.FloorHeight.Rows.Add(new object[] {i.ToString(),FloorHeight.ToString(),FloorPos.ToString()});
				}
			}
			if(configDs.environment.Rows.Count<1)
			{
				for(int i=0;i<MAX_OCSS;i++)
				{
					configDs.environment.Rows.Add(new object[] {i,false});
				}
			}
			for(int i=0;i<checkArr.Length;i++)
			{
				checkArr[i].DataBindings.Clear();
				checkArr[i].DataBindings.Add(new Binding("Checked",configDs.environment.Rows[i],"isChecked"));
			}
		}

		[DllImport("USER32.DLL", EntryPoint= "PostMessage")]
		public static extern bool PostMessage(IntPtr hwnd, uint msg,IntPtr wParam, IntPtr lParam);

		[DllImport("USER32.DLL", EntryPoint= "SendMessage")]
		public static extern int SendMessage(IntPtr hwnd, int msg, IntPtr wParam,  IntPtr lParam);

		private void button1_Click(object sender, System.EventArgs e)
		{
			SendMessage(this.Handle,0x401,(IntPtr)0x20,(IntPtr)0x30);
		}

		private void timer1_Tick(object sender, System.EventArgs e)
		{

		}

		private void button1_Click_1(object sender, System.EventArgs e)
		{
		}

		private void gridDebug_Navigate(object sender, System.Windows.Forms.NavigateEventArgs ne)
		{
		
		}
		private void lbOcss_Click(object sender, System.EventArgs e)
		{
			
			//ushort nControl=(ushort)GetKeyState(0x11 /*VK_CONTROL*/);
			//ushort nShift=(ushort)GetKeyState(0x10 /*VK_CONTROL*/);
			//double dOTime,dCTime,dWTime;
			/*
						if(nControl>10)
						{
							dsConfig.select.Rows.Clear();
							this.txtDOTime.DataBindings.Clear();
							this.txtDCTime.DataBindings.Clear();
							this.txtDWTime.DataBindings.Clear();
							dOTime=dCTime=dWTime=0;
							for(int i=0;i<(int)dsConfig.building_info.Rows[0]["CarCnt"];i++)
							{
								dsConfig.select.Rows.Add(new object[]{"lbOcss",i});
								csOcss ocss_info=(csOcss)ocssList[i];
								Graphics dc=ocss_info.lbOcss.CreateGraphics();
								Pen pen=new Pen(Color.Green,2);
								pen.DashStyle=DashStyle.Dash;
								dc.DrawEllipse(pen,2,2,ocss_info.lbOcss.Width-6,ocss_info.lbOcss.Height-6);
								pen.Dispose();
								dc.Dispose();
								if(dOTime==0)
									dOTime=(double)dsConfig.ocss_info.Rows[i]["DOTime"];
								else if(dOTime>0)
								{
									if(dOTime!=(double)dsConfig.ocss_info.Rows[i]["DOTime"])
										dOTime=-1;
								}
								if(dCTime==0)
									dCTime=(double)dsConfig.ocss_info.Rows[i]["DCTime"];
								else if(dCTime>0)
								{
									if(dCTime!=(double)dsConfig.ocss_info.Rows[i]["DCTime"])
										dCTime=-1;
								}
								if(dWTime==0)
									dWTime=(double)dsConfig.ocss_info.Rows[i]["DWTime"];
								else if(dWTime>0)
								{
									if(dWTime!=(double)dsConfig.ocss_info.Rows[i]["DWTime"])
										dWTime=-1;
								}
							}
							if(dOTime<0)
								this.txtDOTime.Text="";
							if(dCTime<0)
								this.txtDCTime.Text="";
							if(dWTime<0)
								this.txtDWTime.Text="";
							return;
						}
						else if(nShift>10)
						{
							for(int i=0;i<(int)dsConfig.building_info.Rows[0]["CarCnt"];i++)
							{
								csOcss ocss_info=(csOcss)ocssList[i];
								if(sender==ocss_info.lbOcss)
								{
									if(dsConfig.select.Rows.Count>0)
									{
										dsConfig.select.Rows.Add(new object[]{"lbOcss",i});
									}
									this.btn2D.Enabled=true;
									Graphics dc=ocss_info.lbOcss.CreateGraphics();
									Pen pen=new Pen(Color.Green,2);
									pen.DashStyle=DashStyle.Dash;
									dc.DrawEllipse(pen,2,2,ocss_info.lbOcss.Width-6,ocss_info.lbOcss.Height-6);
									pen.Dispose();
									dc.Dispose();
								}
							}
						}
						else
						{
							if(dsConfig.select.Rows.Count>0)
								deSelect();
							dsConfig.select.Rows.Clear();
							for(int i=0;i<(int)dsConfig.building_info.Rows[0]["CarCnt"];i++)
							{
								csOcss ocss_info=(csOcss)ocssList[i];
				
								if(sender==ocss_info.lbOcss)
								{
									dsConfig.select.Rows.Add(new object[]{"lbOcss",i});
									Graphics dc=ocss_info.lbOcss.CreateGraphics();
									Pen pen=new Pen(Color.Green,2);
									pen.DashStyle=DashStyle.Dash;
									dc.DrawEllipse(pen,2,2,ocss_info.lbOcss.Width-6,ocss_info.lbOcss.Height-6);
									pen.Dispose();
									dc.Dispose();
									this.txtDOTime.DataBindings.Clear();
									this.txtDCTime.DataBindings.Clear();
									this.txtDWTime.DataBindings.Clear();
					
									this.txtDOTime.DataBindings.Add(new Binding("Text",dsConfig.ocss_info.Rows[i],"DOTime"));
									this.txtDCTime.DataBindings.Add(new Binding("Text",dsConfig.ocss_info.Rows[i],"DCTime"));
									this.txtDWTime.DataBindings.Add(new Binding("Text",dsConfig.ocss_info.Rows[i],"DWTime"));
									return;
								}
							}
						}
			*/			
		}

		private void picHoist_MouseDown(object sender, System.Windows.Forms.MouseEventArgs e)
		{
			PictureBox picHoist=(PictureBox)sender;
			csFloor floor;
			if(e.Button ==System.Windows.Forms.MouseButtons.Left)
			{
				for(int i=0;i<ocssList.Count;i++)
				{
					csOcss ocss_info=(csOcss)ocssList[i];
					for(int j=0;j<ocss_info.floorList.Count;j++)
					{
						floor=(csFloor)ocss_info.floorList[j];
						if(picHoist.Equals(floor.picHoist))
						{
							if(floor.upCarCall==0)
							{
								ocss_info.assigned_calls[j]|=S_FCC;
								floor.upCarCall=1;
								floor.picHoist.Image=this.imageList1.Images[(int)floor.carCall+(int)IMG.NO_CARCALL];
							}
							else
							{
								ocss_info.assigned_calls[j] &=~S_FCC;
								floor.upCarCall=0;
								floor.picHoist.Image=this.imageList1.Images[(int)floor.carCall+(int)IMG.NO_CARCALL];
							}
							break;
						}
					}
				}
			}
		}

		private void picHall_MouseDown(object sender, System.Windows.Forms.MouseEventArgs e)
		{

			PictureBox picHall=(PictureBox)sender;
			csFloor floor;
			if(e.Button ==System.Windows.Forms.MouseButtons.Left)
			{
				for(int i=0;i<ocssList.Count;i++)
				{
					csOcss ocss_info=(csOcss)ocssList[i];
					for(int j=0;j<ocss_info.floorList.Count;j++)
					{
						floor=(csFloor)ocss_info.floorList[j];
						if(picHall.Equals(floor.picHall))
						{
							if(floor.upHallCall==0)
							{
								ocss_info.assigned_calls[j] |= S_FUHC;
								floor.upHallCall=1;
								floor.picHall.Image=this.imageList1.Images[(int)floor.hallCall+5];
							}
							else
							{
								ocss_info.assigned_calls[j] &= ~S_FUHC;
								floor.upHallCall=0;
								floor.picHall.Image=this.imageList1.Images[(int)floor.hallCall+5];
							}
							break;
						}
					}
				}
			}
			else
			{
				for(int i=0;i<ocssList.Count;i++)
				{
					csOcss ocss_info=(csOcss)ocssList[i];
					for(int j=0;j<ocss_info.floorList.Count;j++)
					{
						floor=(csFloor)ocss_info.floorList[j];
						if(picHall.Equals(floor.picHall))
						{
							if(floor.dnHallCall==0)
							{
								ocss_info.assigned_calls[j] |= S_FDHC;
								floor.dnHallCall=1;
								floor.picHall.Image=this.imageList1.Images[(int)floor.hallCall+5];
							}
							else
							{
								ocss_info.assigned_calls[j] &= ~S_FDHC;
								floor.dnHallCall=0;
								floor.picHall.Image=this.imageList1.Images[(int)floor.hallCall+5];
							}
							break;
						}
					}
				}
			}
		}

		private void tabControl1_SelectedIndexChanged(object sender, System.EventArgs e)
		{
			if(tabDebug.SelectedTab.Text.Equals("TSA_DELAY"))
			{	
				if(this.bFirstTSA.Equals(true))
				{
					bFirstTSA=false;
					gridTraffic.DataSource=configDs.traffic;
					DataGridTableStyle ts = new DataGridTableStyle();
					// Set the DataGridTableStyle.MappingName property
					// to the table in the data source to map to.
					ts.MappingName = this.gridTraffic.DataSource.ToString();
					// Add it to the datagrid's TableStyles collection
					gridTraffic.TableStyles.Add(ts);

					// Hide the first column (index 0)
					gridTraffic.TableStyles[0].GridColumnStyles[0].Width = 40;
					gridTraffic.TableStyles[0].GridColumnStyles[1].Width = 30;
					gridTraffic.TableStyles[0].GridColumnStyles[2].Width = 30;
					gridTraffic.TableStyles[0].GridColumnStyles[3].Width = 30;
					gridTraffic.TableStyles[0].GridColumnStyles[4].Width = 30;
					gridTraffic.TableStyles[0].GridColumnStyles[5].Width = 30;
					gridTraffic.TableStyles[0].RowHeaderWidth=10;
					gridTraffic.TableStyles[0].RowHeadersVisible=true;

					maxFloor=(int)configDs.FloorHeight.Rows.Count;
					for(int i=0;i<2;i++)
					{
						csOcss ocss_info=new csOcss();
						ocss_info.ocss_init();
						PictureBox picDoor=new PictureBox();
						this.tabSimul.Controls.Add(picDoor);
						ocss_info.picDoor=picDoor;

						PictureBox picCar=new PictureBox();
						this.tabSimul.Controls.Add(picCar);
						ocss_info.picCar=picCar;
						ocssList.Add(ocss_info);
					}

					for(int i=0;i<2;i++)
					{
						csOcss ocss_info=(csOcss)ocssList[i];
						ocss_info.bottom_pos=0;
						ocss_info.top_pos=(int)configDs.FloorHeight.Rows.Count-1;
						PictureBox picDoor=(PictureBox)ocss_info.picDoor;
						picDoor.BackColor=Color.White;
						picDoor.Visible=false;
						PictureBox picCar=(PictureBox)ocss_info.picCar;
						picCar.BackColor=Color.Khaki;
						picCar.Image=this.imageList1.Images[(int)IMG.CAR];
						picCar.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
				
						//OCSS Label 만들기
						Label lbOcss=new Label();
						this.tabSimul.Controls.Add(lbOcss);
						lbOcss.BackColor = System.Drawing.Color.Gainsboro;
						lbOcss.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
						lbOcss.Text = "OCSS"+string.Format("{0:D}",i+1);
						lbOcss.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
						lbOcss.Click += new System.EventHandler(this.lbOcss_Click);

						ocss_info.lbOcss=lbOcss;				
						//lbOcss.Click += new System.EventHandler(this.label1_Click);
						PictureBox picHoistPark=new PictureBox();
						this.tabSimul.Controls.Add(picHoistPark);
						picHoistPark.Image=this.imageList1.Images[(int)IMG.NO_CARCALL];
						picHoistPark.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
						picHoistPark.Visible=false;
						ocss_info.picHoistPark=picHoistPark;

						PictureBox picHallPark=new PictureBox();
						this.tabSimul.Controls.Add(picHallPark);
						picHallPark.BackColor=Color.White;
						picHallPark.Visible=false;
						ocss_info.picHallPark=picHallPark;

						for(int j=0;j<maxFloor;j++)
						{
							csFloor fl=new csFloor();
							fl.carId=i+1;
							fl.floor=j+1;
							PictureBox picHoist=new PictureBox();
							this.tabSimul.Controls.Add(picHoist);
							picHoist.BackColor=Color.Black;
							picHoist.Image=this.imageList1.Images[(int)IMG.NO_CARCALL];
							picHoist.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
							picHoist.MouseDown += new System.Windows.Forms.MouseEventHandler(this.picHoist_MouseDown);
							fl.picHoist=picHoist;
					
							PictureBox picHall=new PictureBox();
							this.tabSimul.Controls.Add(picHall);
							picHall.BackColor=Color.White;
							picHall.Image=this.imageList1.Images[(int)IMG.NO_HALLCALL];
							picHall.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
							picHall.MouseDown += new System.Windows.Forms.MouseEventHandler(this.picHall_MouseDown);
							fl.picHall=picHall;
							ocss_info.floorList.Add(fl);
						}
					}
					RefreshTSA();
				}
			}
		}

		public void DrawGrpFrame()
		{
			int i;
			Graphics dc=this.tabTraject.CreateGraphics();
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
			YY1 = panel1.Height;
			XX2 = this.tabTraject.Width-1;
			YY2 = this.tabTraject.Height -panel1.Height- 2;
			GX1 = XX1 + GRP_BORDER;
			GY1 = YY1 + GRP_BORDER;
			GX2 = XX2 - GRP_BORDER;
			GY2 = YY2 - GRP_BORDER;

			DX = GX2 - GX1;
			DY = GY2 - GY1;
			Pen pen=new Pen(Color.Black,1);
			dc.DrawRectangle(pen,XX1, YY1, XX2, YY2);
			pen.Color=Color.FromArgb(0xc0,0xc0,0xc0);
			dc.DrawRectangle(pen,GX1, GY1, DX, DY);

			for (i=0; i<10; i++)
			{
				dc.DrawLine(pen,GX1, GY1 + DY * i /10,GX2, GY1 + DY * i /10);
			}
			for (i=0; i<10; i++)
			{
				dc.DrawLine(pen,GX1 + DX * i /10, GY1,GX1 + DX * i /10, GY2);
			}
			pen.Color=Color.FromArgb(0,0,0);
			dc.DrawLine(pen,GX1,GY1,GX1,GY2);
			dc.DrawLine(pen,GX1,GY2,GX2,GY2);

			//			DisplayStart = (DisplayAllRange)? 0 : (CurrentTimeRange - DisplayRange);
			//			if (DisplayStart<0) DisplayStart = 0;
			br=new SolidBrush(Color.CadetBlue);
			Font DispFont=new Font("Arial",10,FontStyle.Bold);
			//g.DrawString("Arial Bold",DispFont_2,br,10,150);
			LastDispInx=0;
			DisplayUpdate();

		}

		public void DisplayUpdate()
		{
			string time;
			int penInx;
			double dTime;
			string name;
			int x1,x2,y1,y2;
			double xSub;
			int xMax;
			Graphics dc=this.tabTraject.CreateGraphics();
			int i;
			if(this.debugDs.pos.Rows.Count<=1)
				return;
			for(i=this.debugDs.pos.Rows.Count-1;i>0;i--)
			{
				if(true)  //X축이 시간이면
				{
					int lastInx=this.debugDs.pos.Rows.Count-1;
					double lastTime=double.Parse((string)this.debugDs.pos.Rows[lastInx][0]);
					time=(string)debugDs.pos.Rows[i][0]; 
					dTime=double.Parse(time);
					x1=GX2-(int)(DX*((lastTime-dTime)/CurrentTimeRange));
					time=(string)debugDs.pos.Rows[i-1][0]; // 현재 X좌표를 읽어옴
					dTime=double.Parse(time);
					x2=GX2-(int)(DX*((lastTime-dTime)/CurrentTimeRange));
					if(x1<GX1)
						return;
				}
				penInx=0;
				for(int j=0;j<MAX_OCSS-1;j++)
				{
					if(!this.checkArr[j].Checked)
					{
						y1=(int)((double)DY*(double.Parse((string)debugDs.pos.Rows[i][j+1])/(double)maxY.Value));
						y2=(int)((double)DY*(double.Parse((string)debugDs.pos.Rows[i-1][j+1])/(double)maxY.Value));
						dc.DrawLine(penArr[j],x1,GY2-y1,x2,GY2-y2);
					}
				}	
			}
		}
		
		public void RefreshTSA()
		{
			gridTraffic.Top=10;
			gridTraffic.Width=230;
			gridTraffic.Height=this.tabSimul.ClientSize.Height-gridTraffic.Top-this.statusBar1.Height-5;
			HOIST_OX=gridTraffic.Left+gridTraffic.Width+10;

			GY2=(this.tabSimul.ClientSize.Height-GRP_BORDER);
			maxFloor=(int)configDs.FloorHeight.Rows.Count;
			floorHeight=(this.tabSimul.ClientSize.Height-GRP_BORDER*3)/maxFloor;

			for(int i=0;i<2;i++)
			{
				csOcss ocss_info=(csOcss)ocssList[i];
				ocss_info.bottom_pos=0;
				ocss_info.top_pos=(int)configDs.FloorHeight.Rows.Count-1;
				PictureBox picDoor=(PictureBox)ocss_info.picDoor;
				PictureBox picCar=(PictureBox)ocss_info.picCar;
				picCar.Width=HOIST_WIDTH;
				picCar.Height=floorHeight;
				picCar.Top=GY2-picCar.Height;
				picCar.Left=HOIST_OX+i*(HOIST_WIDTH+HALL_WIDTH+10);
				
				//OCSS Label 만들기
				Label lbOcss=(Label)ocss_info.lbOcss;
				lbOcss.Height=25;
				lbOcss.Top=GY2-((ocss_info.top_pos+1)*picCar.Height+lbOcss.Height);
				lbOcss.Left=HOIST_OX+i*(HOIST_WIDTH+HALL_WIDTH+10);
				lbOcss.Width=HALL_WIDTH+HOIST_WIDTH;
				for(int j=0;j<maxFloor;j++)
				{
					csFloor fl=(csFloor)ocss_info.floorList[j];
					PictureBox picHoist=(PictureBox)fl.picHoist;
					picHoist.Width=HOIST_WIDTH;
					picHoist.Height=floorHeight;
					picHoist.Top=GY2-((j+1)*picHoist.Height);
					picHoist.Left=HOIST_OX+i*(HOIST_WIDTH+HALL_WIDTH+10);

					PictureBox picHall=(PictureBox)fl.picHall;
					picHall.Width=HALL_WIDTH;
					picHall.Height=floorHeight;
					picHall.Top=GY2-((j+1)*picHall.Height);
					picHall.Left=HOIST_OX+i*(HOIST_WIDTH+HALL_WIDTH+10)+picHoist.Width;
				}
			}
		}

		public void RefreshGrp()
		{
			int TimeIndex;
			DrawGrpFrame();
		}

		public void read2d_p()
		{
			int rowInx=0;
			try
			{
				StreamReader sr=new StreamReader(@"2d-p.csv");
				while (sr.Peek() >= 0) 
				{
					string str=sr.ReadLine();
					string[] strArr=str.Split(',');
					if(rowInx==0)
					{
						rowInx++;
						continue;
					}

					if(reportDs._2d_p.Rows.Count<=(rowInx-1)) //열이 없으면
					{
						DataRow row=reportDs._2d_p.NewRow();
						for(int i=0;i<strArr.Length;i++)
						{
							row[i]=strArr[i];
						}
						reportDs._2d_p.Rows.Add(row);
					}
					else
					{
						for(int i=0;i<strArr.Length;i++)
						{
							if(!reportDs._2d_p.Rows[rowInx][i].Equals(strArr[i]))
							{
								reportDs._2d_p.Rows[rowInx][i]=strArr[i];
							}
						}
					}
					rowInx++;
				}
				sr.Close();
			}
			catch(Exception eee)
			{
				MessageBox.Show(eee.Message);
			}

		}

		public void read2d_c()
		{
			int rowInx=0;
			try
			{
				StreamReader sr=new StreamReader(@"2d-c.csv");
				while (sr.Peek() >= 0) 
				{
					string str=sr.ReadLine();
					string[] strArr=str.Split(',');
					if(rowInx==0)
					{
						rowInx++;
						continue;
					}
					if(reportDs._2d_c.Rows.Count<=(rowInx-1)) //열이 없으면
					{
						DataRow row=reportDs._2d_c.NewRow();
						for(int i=0;i<strArr.Length;i++)
						{
							row[i]=strArr[i];
						}
						reportDs._2d_c.Rows.Add(row);
					}
					else
					{
						for(int i=0;i<strArr.Length;i++)
						{
							if(!reportDs._2d_c.Rows[rowInx][i].Equals(strArr[i]))
							{
								reportDs._2d_c.Rows[rowInx][i]=strArr[i];
							}
						}
					}
					rowInx++;
				}
				sr.Close();
			}
			catch(Exception eee)
			{
				MessageBox.Show(eee.Message);
			}
		}

		public void getWaitTime()
		{
			int count30=0;
			int count45=0;
			int count60=0;
			double lobbyAvgWaitT=0;
			double lobbyWaitSum=0;
			double lobbyMaxWaitT=0;
			int lobbyWaitCnt=0;
			DataRow row;
			double per;
			for(int i=0;i<reportDs._2d_p.Rows.Count;i++)
			{
				double waitTime=double.Parse((string)reportDs._2d_p.Rows[i]["Wait_AP_T"]);
				if(waitTime<30)
					count30++;
				else if(waitTime>45 && waitTime<60)
					count45++;
				else if(waitTime>60)
					count60++;
				int origin=Int32.Parse((string)reportDs._2d_p.Rows[i]["Origin"]);
				if(origin==lobby_pos)
				{
					lobbyWaitSum+=double.Parse((string)reportDs._2d_p.Rows[i]["Wait_AP_T"]);
					lobbyWaitCnt++;
				}
			}
			row=reportDs.report_item1.NewRow();
			row[0]="Wait Time Less Than 30 sec         [%]";
			per=100.0*(double)count30/(double)reportDs._2d_p.Rows.Count;
			row[1]=string.Format("{0:#,##0.00}",per)+"%";
			reportDs.report_item1.Rows.Add(row);

			row=reportDs.report_item1.NewRow();
			row[0]="Wait Time More Than 45 sec         [%]";
			per=100.0*(double)count45/(double)reportDs._2d_p.Rows.Count;
			row[1]=string.Format("{0:#,##0.00}",per)+"%";
			reportDs.report_item1.Rows.Add(row);

			row=reportDs.report_item1.NewRow();
			row[0]="Wait Time More Than 60 sec         [%]";
			per=100.0*(double)count60/(double)reportDs._2d_p.Rows.Count;
			row[1]=string.Format("{0:#,##0.00}",per)+"%";
			reportDs.report_item1.Rows.Add(row);

			row=reportDs.report_item1.NewRow();
			row[0]="LOBBY Ave. Waiting Time            [s]";
			per=lobbyWaitSum/(double)lobbyWaitCnt;
			row[1]=string.Format("{0:#,##0.00}",per);
			reportDs.report_item1.Rows.Add(row);

		}
		
		public void getSvcTime()
		{
			int count90=0;
			int count120=0;
			int count180=0;
			DataRow row;
			double per;
			for(int i=0;i<reportDs._2d_p.Rows.Count;i++)
			{
				double svcTime=double.Parse((string)reportDs._2d_p.Rows[i]["Srvc_T"]);
				if(svcTime<90)
					count90++;
				else if(svcTime>120 && svcTime<180)
					count120++;
				else if(svcTime>60)
					count180++;
			}
			row=reportDs.report_item1.NewRow();
			row[0]="Serv. Time Less Than 90 sec        [%]";
			per=100.0*(double)count90/(double)reportDs._2d_p.Rows.Count;
			row[1]=string.Format("{0:#,##0.00}",per)+"%";
			reportDs.report_item1.Rows.Add(row);

			row=reportDs.report_item1.NewRow();
			row[0]="Serv. Time More Than 120 sec       [%]";
			per=100.0*(double)count120/(double)reportDs._2d_p.Rows.Count;
			row[1]=string.Format("{0:#,##0.00}",per)+"%";
			reportDs.report_item1.Rows.Add(row);

			row=reportDs.report_item1.NewRow();
			row[0]="Serv. Time More Than 180 sec       [%]";
			per=100.0*(double)count180/(double)reportDs._2d_p.Rows.Count;
			row[1]=string.Format("{0:#,##0.00}",per)+"%";
			reportDs.report_item1.Rows.Add(row);
		}
		
		public void getAvgWaitChart()
		{
			DataRow row;
			row=reportDs.svcwait.NewRow();
			row[0]="Avg Wait Time";
			row[1]=14.5;
			reportDs.svcwait.Rows.Add(row);

			row=reportDs.svcwait.NewRow();
			row[0]="Max Wait Time";
			row[1]=35.7;
			reportDs.svcwait.Rows.Add(row);

			row=reportDs.svcwait.NewRow();
			row[0]="Avg Svc Time";
			row[1]=36.5;
			reportDs.svcwait.Rows.Add(row);

			row=reportDs.svcwait.NewRow();
			row[0]="Max Svc Time";
			row[1]=107.5;
			reportDs.svcwait.Rows.Add(row);
		}

		private void btnReport_Click(object sender, System.EventArgs e)
		{
			lobby_pos=Int32.Parse(txtLobby.Text);
			read2d_p();
			read2d_c();
			reportDs.report_item1.Rows.Clear();
			reportDs.report_item2.Rows.Clear();
			getWaitTime();
			getSvcTime();
			getAvgWaitChart();
			tabDebug.SelectedIndex=5;
			oRpt.SetDataSource(this.reportDs);
			this.crystalReportViewer1.ReportSource=oRpt;
		}

		private void tabTraject_Resize(object sender, System.EventArgs e)
		{
			RefreshGrp();
		}

		private void tabTraject_Paint(object sender, System.Windows.Forms.PaintEventArgs e)
		{
			RefreshGrp();
		}

		private void checkBox1_CheckedChanged(object sender, System.EventArgs e)
		{
			RefreshGrp();
		}

		private void tabSimul_Click(object sender, System.EventArgs e)
		{
		
		}

		private void Form1_Closing(object sender, System.ComponentModel.CancelEventArgs e)
		{
			configDs.WriteXml("cogsd.xml",XmlWriteMode.IgnoreSchema);
		}

		public double getPos(int floor)
		{
			double rv=0;
			for(int i=0;i<configDs.FloorHeight.Rows.Count;i++)
			{
				if((int)configDs.FloorHeight.Rows[i][0]==floor)
					rv=(double)configDs.FloorHeight.Rows[i][2];
			}
			return rv;
		}

		public int getFloor(double position)
		{
			int rv=0;
			for(int i=0;i<configDs.FloorHeight.Rows.Count-1;i++)
			{
				double H1=(double)configDs.FloorHeight.Rows[i][2];
				double H2=(double)configDs.FloorHeight.Rows[i+1][2];
				if(position<=(H1+H2)/2)
					return((int)configDs.FloorHeight.Rows[i][0]);
			}
			return rv;
		}

		public int getNCF(double elapsedTime,int sourceFloor,int destFloor,double jerk,double vel,double acc)
		{
			int NCF=0;
			const int MaxFloors=100;
			double[] NCF_Time=new double[MaxFloors];
			int n_Floor;
			int i;
			double originPosition, destPosition;
			double distance;
			double fullAccDecisionDistance;
			fullAccDecisionDistance=2*(Math.Pow(acc,3))/(Math.Pow(jerk,2));
			n_Floor = Math.Abs(sourceFloor - destFloor);
			originPosition = getPos(sourceFloor);
			destPosition = getPos(destFloor);
    
			//' add by CSW on 4/4/05, considering decending as well
			int direction;
  
			if(destFloor >= sourceFloor)
				direction = 1;
			else
				direction = -1;
    
			if(elapsedTime <= 0)
			{
				NCF = sourceFloor;
			}
			else
			{
				for(i = 1;i<n_Floor;i++)
				{
					distance = Math.Abs(getPos(sourceFloor) - getPos(sourceFloor + i * direction));
					// full speed
					if(distance >= fullSpeedDistance)
					{ 
						if (elapsedTime < distance / vel)
						{
							NCF = sourceFloor + i * direction;
							return NCF;
						}
						else
						{
							if (distance >= fullAccDecisionDistance)
							{
								//Then  ' full acc
								if(elapsedTime < Math.Pow(acc * acc * acc + 4 * distance * jerk * jerk, 1 / 2) / (2 * jerk * Math.Pow(acc, 1 / 2)) - acc / (2 * jerk))
								{
									NCF = sourceFloor + i * direction;
									return NCF;
								}
								else
								{
									if (elapsedTime < Math.Pow(distance / (2 * jerk), 1 / 3))
									{
										NCF = sourceFloor + i * direction;
									}
								}
							}
							NCF = destFloor;
							return NCF;
						}
					}
				}
			}
			return NCF;
		}

		public void MakeTrajectory()
		{
			double dt1;
			double dt2;
			double floor_offset;  // for pit exist
    
			double T1, T2, T3, T4, T5, T6, T7;
			double jt, at, vt, st;
			double tt;
			double timeOffset;
    
			const int colTimeA = 0;
			const int colAccA = 1;
			const int colVelA = 2;
			const int colDistA = 3;
			const int colFloorA = 4;
			const int colCurrentFloorA = 5;
			const int colNCFA = 6;
			const int colNSPA = 7;                        //' Added by CSW 4/8/05
			const int colNSFA = 8;                        //' Added by CSW 4/8/05
			const int colTimeB = 9;
			const int colAccB = 10;
			const int colVelB = 11;
			const int colDistB = 12;
			const int colFloorB = 13;
			const int colCurrentFloorB = 14;
			const int colNCFB = 15;
			const int colNSPB = 16;
			const int colNSFB = 17;
			int dir=1;
			double timeStep=0.1;
			double jerkUpper=double.Parse(this.txtJerkU.Text);
			double jerkLower=double.Parse(this.txtJerkL.Text);
			double accUpper=double.Parse(this.txtAccU.Text);
			double accLower=double.Parse(this.txtAccL.Text);
			double velUpper=double.Parse(this.txtVelU.Text);
			double velLower=double.Parse(this.txtVelL.Text);
			double safeGU=double.Parse(this.txtSafeGU.Text);
			double safeGL=double.Parse(this.txtSafeGL.Text);
			double fullSpeedDistanceU=((Math.Pow(accUpper,2))*velUpper+(Math.Pow(velUpper,2))*jerkUpper)/(jerkUpper*accUpper);
			double fullSpeedDistanceL=((Math.Pow(accLower,2))*velLower+(Math.Pow(velLower,2))*jerkLower)/(jerkLower*accLower);
			double fullAccDistanceU=2*(Math.Pow(accUpper,3))/(Math.Pow(jerkUpper,2));
			double fullAccDistanceL=2*(Math.Pow(accLower,3))/(Math.Pow(jerkLower,2));
			int rowno=0;
    
    
			//'' first run of Car A
			double jerk,acc,vel,safety_dec;
			debugDs.Calc.Clear();

			for(int carno = 0;carno<2;carno++)
			{
				timeOffset=0;
				//' The if statement allow different parameters for different cars, if the user choose to
				jerk=(carno==0) ? jerkUpper : jerkLower;
				acc=(carno==0) ? accUpper : accLower;
				vel=(carno==0) ? velUpper : velLower;
				safety_dec=(carno==0) ? safeGU : safeGL;
				fullSpeedDistance=(carno==0) ? fullSpeedDistanceU : fullSpeedDistanceL;
				fullAccDistance=(carno==0) ? fullAccDistanceU : fullAccDistanceL;
				double normalFloorHeight=4.00;
				rowno=0;
				if(carno==0)
				{
					debugDs.Calc.Rows.Add(new object[] {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0});
				}
				debugDs.Calc.Rows[0][carno*colTimeB+colTimeA]=timeOffset.ToString();
				debugDs.Calc.Rows[0][carno*colTimeB+colAccA]="0";
				debugDs.Calc.Rows[0][carno*colTimeB+colVelA]="0";
				int destFloor=(carno==0) ? Int32.Parse((string)configDs.SqcUpper.Rows[0][0]) :Int32.Parse((string)configDs.SqcLower.Rows[0][0]); 
				double destPos=getPos(destFloor);
				debugDs.Calc.Rows[0][carno*colTimeB+colDistA]=destPos.ToString();
				debugDs.Calc.Rows[0][carno*colTimeB+colFloorA]=string.Format("{0:#,##0.00}",destPos/normalFloorHeight);
				debugDs.Calc.Rows[0][carno*colTimeB+colCurrentFloorA]=getFloor(destPos);
				debugDs.Calc.Rows[0][carno*colTimeB+colNCFA]=debugDs.Calc.Rows[0][carno*colTimeB+colCurrentFloorA];				
				double nsp=(double)debugDs.Calc.Rows[0][carno*colTimeB+colDistA];
				debugDs.Calc.Rows[0][carno*colTimeB+colNSPA]=nsp/normalFloorHeight;
				if(dir==1)
				{
					debugDs.Calc.Rows[0][carno*colTimeB+colNSFA]=(int)((double)debugDs.Calc.Rows[0][carno*colTimeB+colNSPA]+0.0005);
				}
				else
				{
					debugDs.Calc.Rows[0][carno*colTimeB+colNSFA]=(int)((double)debugDs.Calc.Rows[0][carno*colTimeB+colNSPA]+0.9999);
				}
				rowno++;
				int rowCount=(carno==0) ? configDs.SqcUpper.Rows.Count : configDs.SqcLower.Rows.Count;
				for(int runno=1;runno<rowCount;runno++)
				{
					int destFloor1=(carno==0) ? Int32.Parse((string)configDs.SqcUpper.Rows[runno-1][0]) :Int32.Parse((string)configDs.SqcLower.Rows[runno-1][0]); 
					int destFloor2=(carno==0) ? Int32.Parse((string)configDs.SqcUpper.Rows[runno][0]) :Int32.Parse((string)configDs.SqcLower.Rows[runno][0]); 
					double dist=getPos(destFloor2)-getPos(destFloor1);
					if(dist<0)
					{
						dir=-1;
						dist=-1*dist;
					}
					else
						dir=1;
					if(dist>=fullSpeedDistance)
					{
						T1 = acc / jerk;
						T2 = vel / acc;
						T3 = acc / jerk + vel / acc;
						T4 = dist / vel;
						T5 = dist / vel + acc / jerk;
						T6 = dist / vel + vel / acc;
						T7 = dist / vel + acc / jerk + vel / acc;
						double doorTime=(carno==0) ? double.Parse((string)configDs.SqcUpper.Rows[runno-1][1]) : double.Parse((string)configDs.SqcLower.Rows[runno-1][1]);
						timeOffset = timeOffset + doorTime;
				

						if(carno==0)
						{
							debugDs.Calc.Rows.Add(new object[] {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0});
						}
						debugDs.Calc.Rows[rowno][carno*colTimeB+colTimeA]=timeOffset.ToString();
						debugDs.Calc.Rows[rowno][carno*colTimeB+colAccA]="0";
						debugDs.Calc.Rows[rowno][carno*colTimeB+colVelA]="0";
						destFloor=(carno==0) ? Int32.Parse((string)configDs.SqcUpper.Rows[runno-1][0]) :Int32.Parse((string)configDs.SqcLower.Rows[runno-1][0]); 
						destPos=getPos(destFloor);
						debugDs.Calc.Rows[rowno][carno*colTimeB+colDistA]=destPos.ToString();
						debugDs.Calc.Rows[rowno][carno*colTimeB+colFloorA]=string.Format("{0:#,##0.00}",destPos/normalFloorHeight);
						debugDs.Calc.Rows[rowno][carno*colTimeB+colCurrentFloorA]=getFloor(destPos);
						debugDs.Calc.Rows[rowno][carno*colTimeB+colNCFA]=debugDs.Calc.Rows[rowno][carno*colTimeB+colCurrentFloorA];				
						nsp=(double)debugDs.Calc.Rows[rowno][carno*colTimeB+colDistA];
						debugDs.Calc.Rows[rowno][carno*colTimeB+colNSPA]=nsp/normalFloorHeight;
						if(dir==1)
						{
							debugDs.Calc.Rows[runno][carno*colTimeB+colNSFA]=(int)((double)debugDs.Calc.Rows[0][carno*colTimeB+colNSPA]+0.0005);
						}
						else
						{
							debugDs.Calc.Rows[runno][carno*colTimeB+colNSFA]=(int)((double)debugDs.Calc.Rows[0][carno*colTimeB+colNSPA]+0.9999);
						}
						rowno++;
						//T1 Part
						for(double t=0;t<T1;t+=timeStep)
						{
							tt=t;
							at=jerk*t;
							vt=jerk*(Math.Pow(t,2))/2;
							st=jerk*(Math.Pow(t,3))/6;
			
							if(carno==0)
							{
								debugDs.Calc.Rows.Add(new object[] {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0});
							}
							debugDs.Calc.Rows[rowno][carno*colTimeB+colTimeA]=string.Format("{0:#,##0.00}",tt + timeOffset);
							debugDs.Calc.Rows[rowno][carno*colTimeB+colAccA]=string.Format("{0:#,##0.00}",dir * at);
							debugDs.Calc.Rows[rowno][carno*colTimeB+colVelA]=string.Format("{0:#,##0.00}",dir * vt);
							destFloor=(carno==0) ? Int32.Parse((string)configDs.SqcUpper.Rows[runno-1][0]) :Int32.Parse((string)configDs.SqcLower.Rows[runno-1][0]); 
							destPos=getPos(destFloor);
							debugDs.Calc.Rows[rowno][carno*colTimeB+colDistA]=dir * st+destPos;
							debugDs.Calc.Rows[rowno][carno*colTimeB+colFloorA]=string.Format("{0:#,##0.00}",(dir * st+destPos)/normalFloorHeight);
							debugDs.Calc.Rows[rowno][carno*colTimeB+colCurrentFloorA]=getFloor(destPos);
							debugDs.Calc.Rows[rowno][carno*colTimeB+colNCFA]=getNCF(t, destFloor1, destFloor2,jerk,vel,acc);
							nsp=(double)debugDs.Calc.Rows[rowno][carno*colTimeB+colDistA]+dir*Math.Pow(vt,2)/(2*safety_dec);
							debugDs.Calc.Rows[rowno][carno*colTimeB+colNSPA]=nsp/normalFloorHeight;
							if(dir==1)
							{
								debugDs.Calc.Rows[rowno][carno*colTimeB+colNSFA]=(int)((double)debugDs.Calc.Rows[rowno][carno*colTimeB+colNSPA]+0.0005);
							}
							else
							{
								debugDs.Calc.Rows[rowno][carno*colTimeB+colNSFA]=(int)((double)debugDs.Calc.Rows[rowno][carno*colTimeB+colNSPA]+0.9999);
							}
							rowno++;
						}
						//T2
						for(double t=T1;t<T2;t+=timeStep)
						{
							tt=t;
							at=acc;
							vt=-acc*acc/(2*jerk)+acc*t;
							st=(acc*acc*acc)/(6*(jerk*jerk))-(Math.Pow(acc,2)*t)/(2*jerk)+(acc*(Math.Pow(t,2)))/2;

							if(carno==0)
							{
								debugDs.Calc.Rows.Add(new object[] {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0});
							}
							debugDs.Calc.Rows[rowno][carno*colTimeB+colTimeA]=string.Format("{0:#,##0.00}",tt + timeOffset);
							debugDs.Calc.Rows[rowno][carno*colTimeB+colAccA]=string.Format("{0:#,##0.00}",dir * at);
							debugDs.Calc.Rows[rowno][carno*colTimeB+colVelA]=string.Format("{0:#,##0.00}",dir * vt);
							destFloor=(carno==0) ? Int32.Parse((string)configDs.SqcUpper.Rows[runno-1][0]) :Int32.Parse((string)configDs.SqcLower.Rows[runno-1][0]); 
							destPos=getPos(destFloor);
							debugDs.Calc.Rows[rowno][carno*colTimeB+colDistA]=string.Format("{0}",dir * st+destPos);
							debugDs.Calc.Rows[rowno][carno*colTimeB+colFloorA]=string.Format("{0:#,##0.00}",(dir * st+destPos)/normalFloorHeight);
							debugDs.Calc.Rows[rowno][carno*colTimeB+colCurrentFloorA]=getFloor(destPos);
							debugDs.Calc.Rows[rowno][carno*colTimeB+colNCFA]=getNCF(t, destFloor1, destFloor2,jerk,vel,acc);
							nsp=(double)debugDs.Calc.Rows[rowno][carno*colTimeB+colDistA]+dir*Math.Pow(vt,2)/(2*safety_dec);
							debugDs.Calc.Rows[rowno][carno*colTimeB+colNSPA]=nsp/normalFloorHeight;
							if(dir==1)
							{
								debugDs.Calc.Rows[rowno][carno*colTimeB+colNSFA]=(int)((double)debugDs.Calc.Rows[rowno][carno*colTimeB+colNSPA]+0.0005);
							}
							else
							{
								debugDs.Calc.Rows[rowno][carno*colTimeB+colNSFA]=(int)((double)debugDs.Calc.Rows[rowno][carno*colTimeB+colNSPA]+0.9999);
							}
							rowno++;
						}
						///T3
						for(double t=T2;t<T3;t+=timeStep)
						{
							tt=t;
							at=acc-jerk*t+(vel*jerk)/(acc);
							vt=-(Math.Pow(acc,2)) / (2 * jerk) + acc * t - (jerk * Math.Pow(t,2)) / 2 + (t * vel * jerk) / acc - (Math.Pow(vel,2) * jerk) / (2 * Math.Pow(acc,2));
							dt1 = (Math.Pow(acc,3)) / (6 * (Math.Pow(jerk,2))) - (t * (Math.Pow(acc,2))) / (2 * jerk) - (jerk * (Math.Pow(t,3))) / 6;
							dt2 = (acc * (Math.Pow(t,2))) / 2 + (jerk * vel * (Math.Pow(t,2))) / (2 * acc) - (jerk * (Math.Pow(vel,2)) * t) / (2 * (Math.Pow(acc,2))) + ((Math.Pow(vel,3)) * jerk) / (6 * (Math.Pow(acc,3)));
							st = dt1 + dt2;
							if(carno==0)
							{
								debugDs.Calc.Rows.Add(new object[] {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0});
							}
							debugDs.Calc.Rows[rowno][carno*colTimeB+colTimeA]=string.Format("{0:#,##0.00}",tt + timeOffset);
							debugDs.Calc.Rows[rowno][carno*colTimeB+colAccA]=string.Format("{0:#,##0.00}",dir * at);
							debugDs.Calc.Rows[rowno][carno*colTimeB+colVelA]=string.Format("{0:#,##0.00}",dir * vt);
							destFloor=(carno==0) ? Int32.Parse((string)configDs.SqcUpper.Rows[runno-1][0]) :Int32.Parse((string)configDs.SqcLower.Rows[runno-1][0]); 
							destPos=getPos(destFloor);
							debugDs.Calc.Rows[rowno][carno*colTimeB+colDistA]=string.Format("{0}",dir * st+destPos);
							debugDs.Calc.Rows[rowno][carno*colTimeB+colFloorA]=string.Format("{0:#,##0.00}",(dir * st+destPos)/normalFloorHeight);
							debugDs.Calc.Rows[rowno][carno*colTimeB+colCurrentFloorA]=getFloor(destPos);
							debugDs.Calc.Rows[rowno][carno*colTimeB+colNCFA]=getNCF(t, destFloor1, destFloor2,jerk,vel,acc);
							nsp=(double)debugDs.Calc.Rows[rowno][carno*colTimeB+colDistA]+dir*Math.Pow(vt,2)/(2*safety_dec);
							debugDs.Calc.Rows[rowno][carno*colTimeB+colNSPA]=nsp/normalFloorHeight;
							if(dir==1)
							{
								debugDs.Calc.Rows[rowno][carno*colTimeB+colNSFA]=(int)((double)debugDs.Calc.Rows[rowno][carno*colTimeB+colNSPA]+0.0005);
							}
							else
							{
								debugDs.Calc.Rows[rowno][carno*colTimeB+colNSFA]=(int)((double)debugDs.Calc.Rows[rowno][carno*colTimeB+colNSPA]+0.9999);
							}
							rowno++;
						}
						//T4
						for(double t=T3;t<T4;t+=timeStep)
						{
							tt = t;  //'t
							at = 0; //'A
							vt = vel; //'V
							st = -acc * vel / (2 * jerk) - (Math.Pow(vel,2)) / (2 * acc) + vel * t;
							if(carno==0)
							{
								debugDs.Calc.Rows.Add(new object[] {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0});
							}
							debugDs.Calc.Rows[rowno][carno*colTimeB+colTimeA]=string.Format("{0:#,##0.00}",tt + timeOffset);
							debugDs.Calc.Rows[rowno][carno*colTimeB+colAccA]=string.Format("{0:#,##0.00}",dir * at);
							debugDs.Calc.Rows[rowno][carno*colTimeB+colVelA]=string.Format("{0:#,##0.00}",dir * vt);
							destFloor=(carno==0) ? Int32.Parse((string)configDs.SqcUpper.Rows[runno-1][0]) :Int32.Parse((string)configDs.SqcLower.Rows[runno-1][0]); 
							destPos=getPos(destFloor);
							debugDs.Calc.Rows[rowno][carno*colTimeB+colDistA]=string.Format("{0}",dir * st+destPos);
							debugDs.Calc.Rows[rowno][carno*colTimeB+colFloorA]=string.Format("{0:#,##0.00}",(dir * st+destPos)/normalFloorHeight);
							debugDs.Calc.Rows[rowno][carno*colTimeB+colCurrentFloorA]=getFloor(destPos);
							debugDs.Calc.Rows[rowno][carno*colTimeB+colNCFA]=getNCF(t, destFloor1, destFloor2,jerk,vel,acc);
							nsp=(double)debugDs.Calc.Rows[rowno][carno*colTimeB+colDistA]+dir*Math.Pow(vt,2)/(2*safety_dec);
							debugDs.Calc.Rows[rowno][carno*colTimeB+colNSPA]=nsp/normalFloorHeight;
							if(dir==1)
							{
								debugDs.Calc.Rows[rowno][carno*colTimeB+colNSFA]=(int)((double)debugDs.Calc.Rows[rowno][carno*colTimeB+colNSPA]+0.0005);
							}
							else
							{
								debugDs.Calc.Rows[rowno][carno*colTimeB+colNSFA]=(int)((double)debugDs.Calc.Rows[rowno][carno*colTimeB+colNSPA]+0.9999);
							}
							rowno++;
						}
						//T5
						for(double t=T4;t<T5;t+=timeStep)
						{
							tt = t;//  't
							at = (jerk * dist) / vel - jerk * t;// 'A
							vt = vel - (jerk * (Math.Pow(t,2))) / 2 + (dist * jerk * t) / (vel) - ((Math.Pow(dist,2)) * jerk) / ((Math.Pow(vel,2)) * 2);// 'V
							dt1 = -(acc * vel) / (2 * jerk) - (Math.Pow(vel,2)) / (2 * acc) + (vel * t);
							dt2 = (Math.Pow(t,2) * jerk * dist) / (2 * vel) - (t * jerk * Math.Pow(dist,2)) / (2 * Math.Pow(vel,2)) - (Math.Pow(t,3) * jerk) / 6 + (Math.Pow(dist,3) * jerk) / (Math.Pow(vel,3) * 6);
							st = dt1 + dt2;
							if(carno==0)
							{
								debugDs.Calc.Rows.Add(new object[] {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0});
							}
							debugDs.Calc.Rows[rowno][carno*colTimeB+colTimeA]=string.Format("{0:#,##0.00}",tt + timeOffset);
							debugDs.Calc.Rows[rowno][carno*colTimeB+colAccA]=string.Format("{0:#,##0.00}",dir * at);
							debugDs.Calc.Rows[rowno][carno*colTimeB+colVelA]=string.Format("{0:#,##0.00}",dir * vt);
							destFloor=(carno==0) ? Int32.Parse((string)configDs.SqcUpper.Rows[runno-1][0]) :Int32.Parse((string)configDs.SqcLower.Rows[runno-1][0]); 
							destPos=getPos(destFloor);
							debugDs.Calc.Rows[rowno][carno*colTimeB+colDistA]=string.Format("{0}",dir * st+destPos);
							debugDs.Calc.Rows[rowno][carno*colTimeB+colFloorA]=string.Format("{0:#,##0.00}",(dir * st+destPos)/normalFloorHeight);
							debugDs.Calc.Rows[rowno][carno*colTimeB+colCurrentFloorA]=getFloor(destPos);
							debugDs.Calc.Rows[rowno][carno*colTimeB+colNCFA]=getNCF(t, destFloor1, destFloor2,jerk,vel,acc);
							nsp=(double)debugDs.Calc.Rows[rowno][carno*colTimeB+colDistA]+dir*Math.Pow(vt,2)/(2*safety_dec);
							debugDs.Calc.Rows[rowno][carno*colTimeB+colNSPA]=nsp/normalFloorHeight;
							if(dir==1)
							{
								debugDs.Calc.Rows[rowno][carno*colTimeB+colNSFA]=(int)((double)debugDs.Calc.Rows[rowno][carno*colTimeB+colNSPA]+0.0005);
							}
							else
							{
								debugDs.Calc.Rows[rowno][carno*colTimeB+colNSFA]=(int)((double)debugDs.Calc.Rows[rowno][carno*colTimeB+colNSPA]+0.9999);
							}
							rowno++;
						}
						//T6
						for(double t=T5;t<T6;t+=timeStep)
						{
							tt = t;//  't
							at = -acc;// 'Acc
							vt = vel + (acc * dist) / vel + (Math.Pow(acc,2)) / (2 * jerk) - acc * t;
							dt1 = -(acc * vel) / (2 * jerk) - (Math.Pow(vel,2)) / (2 * acc) - (acc * (Math.Pow(dist,2))) / ((Math.Pow(vel,2)) * 2) - (dist * Math.Pow(acc,2)) / (2 * jerk * vel);
							dt2 = -(Math.Pow(acc,3)) / ((Math.Pow(jerk,2)) * 6) + (vel * t) + (acc * dist * t) / (vel) + (t * Math.Pow(acc,2)) / (2 * jerk) - ((Math.Pow(t,2)) * acc) / 2;
							st = dt1 + dt2;
							if(carno==0)
							{
								debugDs.Calc.Rows.Add(new object[] {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0});
							}
							debugDs.Calc.Rows[rowno][carno*colTimeB+colTimeA]=string.Format("{0:#,##0.00}",tt + timeOffset);
							debugDs.Calc.Rows[rowno][carno*colTimeB+colAccA]=string.Format("{0:#,##0.00}",dir * at);
							debugDs.Calc.Rows[rowno][carno*colTimeB+colVelA]=string.Format("{0:#,##0.00}",dir * vt);
							destFloor=(carno==0) ? Int32.Parse((string)configDs.SqcUpper.Rows[runno-1][0]) :Int32.Parse((string)configDs.SqcLower.Rows[runno-1][0]); 
							destPos=getPos(destFloor);
							debugDs.Calc.Rows[rowno][carno*colTimeB+colDistA]=string.Format("{0}",dir * st+destPos);
							debugDs.Calc.Rows[rowno][carno*colTimeB+colFloorA]=string.Format("{0:#,##0.00}",(dir * st+destPos)/normalFloorHeight);
							debugDs.Calc.Rows[rowno][carno*colTimeB+colCurrentFloorA]=getFloor(destPos);
							debugDs.Calc.Rows[rowno][carno*colTimeB+colNCFA]=getNCF(t, destFloor1, destFloor2,jerk,vel,acc);
							nsp=(double)debugDs.Calc.Rows[rowno][carno*colTimeB+colDistA]+dir*Math.Pow(vt,2)/(2*safety_dec);
							debugDs.Calc.Rows[rowno][carno*colTimeB+colNSPA]=nsp/normalFloorHeight;
							if(dir==1)
							{
								debugDs.Calc.Rows[rowno][carno*colTimeB+colNSFA]=(int)((double)debugDs.Calc.Rows[rowno][carno*colTimeB+colNSPA]+0.0005);
							}
							else
							{
								debugDs.Calc.Rows[rowno][carno*colTimeB+colNSFA]=(int)((double)debugDs.Calc.Rows[rowno][carno*colTimeB+colNSPA]+0.9999);
							}
							rowno++;
						}
						//T7
						for(double t=T6;t<T7;t+=timeStep)
						{
							tt = t; //  't
							at = -acc - jerk * dist / vel - jerk * vel / acc + jerk * t; // 'Acc
							vt = vel + acc * dist / vel + (Math.Pow(acc,2)) / (2 * jerk) - acc * t - jerk * dist * t / vel - jerk * vel * t / acc + jerk * (Math.Pow(t,2)) / 2 + jerk * (Math.Pow(dist,2)) / (2 * (Math.Pow(vel,2))) + jerk * dist / acc + jerk * (Math.Pow(vel,2)) / (2 * (Math.Pow(acc,2))); // 'v
							dt1 = -(Math.Pow(vel,2)) / (2 * acc) + vel * t - (jerk * dist * vel) / (2 * (Math.Pow(acc,2))) - (Math.Pow(dist,2)) * jerk / (2 * vel * acc) + (Math.Pow(t,3)) * jerk / 6 - acc * vel / (2 * jerk) - (Math.Pow(dist,3)) * jerk / ((Math.Pow(vel,3)) * 6) - (Math.Pow(t,2)) * jerk * dist / (2 * vel) + (jerk * (Math.Pow(dist,2)) * t) / (2 * (Math.Pow(vel,2))) + acc * t * dist / vel;
							dt2 = t * (Math.Pow(acc,2)) / (2 * jerk) - (Math.Pow(t,2)) * acc / 2 - (Math.Pow(acc,3)) / (6 * (Math.Pow(jerk,2))) - (Math.Pow(dist,2)) * acc / (2 * (Math.Pow(vel,2))) - (Math.Pow(acc,2)) * dist / (2 * jerk * vel) + t * dist * jerk / acc + t * (Math.Pow(vel,2)) * jerk / (2 * (Math.Pow(acc,2))) - (Math.Pow(t,2)) * vel * jerk / (2 * acc) - jerk * (Math.Pow(vel,3)) / (6 * (Math.Pow(acc,3)));
							st = dt1 + dt2;
							if(carno==0)
							{
								debugDs.Calc.Rows.Add(new object[] {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0});
							}
							debugDs.Calc.Rows[rowno][carno*colTimeB+colTimeA]=string.Format("{0:#,##0.00}",tt + timeOffset);
							debugDs.Calc.Rows[rowno][carno*colTimeB+colAccA]=string.Format("{0:#,##0.00}",dir * at);
							debugDs.Calc.Rows[rowno][carno*colTimeB+colVelA]=string.Format("{0:#,##0.00}",dir * vt);
							destFloor=(carno==0) ? Int32.Parse((string)configDs.SqcUpper.Rows[runno-1][0]) :Int32.Parse((string)configDs.SqcLower.Rows[runno-1][0]); 
							destPos=getPos(destFloor);
							debugDs.Calc.Rows[rowno][carno*colTimeB+colDistA]=string.Format("{0}",dir * st+destPos);
							debugDs.Calc.Rows[rowno][carno*colTimeB+colFloorA]=string.Format("{0:#,##0.00}",(dir * st+destPos)/normalFloorHeight);
							debugDs.Calc.Rows[rowno][carno*colTimeB+colCurrentFloorA]=getFloor(destPos);
							debugDs.Calc.Rows[rowno][carno*colTimeB+colNCFA]=getNCF(t, destFloor1, destFloor2,jerk,vel,acc);
							nsp=(double)debugDs.Calc.Rows[rowno][carno*colTimeB+colDistA]+dir*Math.Pow(vt,2)/(2*safety_dec);
							debugDs.Calc.Rows[rowno][carno*colTimeB+colNSPA]=nsp/normalFloorHeight;
							if(dir==1)
							{
								debugDs.Calc.Rows[rowno][carno*colTimeB+colNSFA]=(int)((double)debugDs.Calc.Rows[rowno][carno*colTimeB+colNSPA]+0.0005);
							}
							else
							{
								debugDs.Calc.Rows[rowno][carno*colTimeB+colNSFA]=(int)((double)debugDs.Calc.Rows[rowno][carno*colTimeB+colNSPA]+0.9999);
							}
							rowno++;
						}
					}
				}
			}    
		}
		public bool bFirstTSA = true;

		private void tbSimul_ButtonClick(object sender, System.Windows.Forms.ToolBarButtonClickEventArgs e)
		{
			if(e.Button.Equals(this.tbSimul.Buttons[4]))
			{
				MakeTrajectory();
			}
		}

		private void tabMonitor_Click(object sender, System.EventArgs e)
		{
		
		}
	}
}
namespace COGSD
{
	/// <summary>
	/// Form1에 대한 요약 설명입니다.
	/// </summary>
	internal enum IMG
	{
		OPEN, 
		RUN, 
		STOP, 
		SAVE, 
		CAR, 
		NO_HALLCALL, 
		UP, 
		DOWN, 
		UPDOWN, 
		NO_CARCALL, 
		UP_CARCALL, 
		DN_CARCALL, 
		UPDN_CARCALL
	}
}
