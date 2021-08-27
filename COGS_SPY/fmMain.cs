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
	public class fmMain : System.Windows.Forms.Form
	{
		public bool bFirstTSA = true;
		bool bFirstConfig=true;

		bool bTSAConfigChanged=false;
		DataTable dtPosTSA=new DataTable("PosTSA");
		int prevPaxInx=-1;
		int curTime=0;
		SPY_STATE state=SPY_STATE.NONE;
		SIMUL_STATE simulState=SIMUL_STATE.NONE;
		const int		 TIME_SLICE=100;
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
		int chkBoxInx=0;
		int TSAPrecision=100;
		int TSATimeRange=25;
		int maxFloor=0;
		const int HOIST_WIDTH=25;
		const int HOIST_HEIGHT=30;
		const int HALL_WIDTH=25;
		const int HALL_HEIGHT=30;
		public int HOIST_OX=200;
		public csDLL dll;
		ArrayList ocssList=new ArrayList();
		ArrayList hallList=new ArrayList();
		private System.Windows.Forms.DataGrid gridDebug;
		public dsDebug debugDs;
		public dsReport reportDs;
		public dsConfig configDs;

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

		public const int MAX_OCSS=9;
		private System.Windows.Forms.TabPage tabReport;
		private CrystalDecisions.Windows.Forms.CrystalReportViewer crystalReportViewer1;
		int lobby_pos;
		private System.Windows.Forms.TabPage tabTraject;
		private System.Windows.Forms.Panel panel1;
		private System.Windows.Forms.CheckBox checkBox1;
		private System.Windows.Forms.CheckBox checkBox2;
		private System.Windows.Forms.CheckBox checkBox4;
		private System.Windows.Forms.CheckBox checkBox3;
		private System.Windows.Forms.CheckBox checkBox6;
		private System.Windows.Forms.CheckBox checkBox7;
		private System.Windows.Forms.CheckBox checkBox5;
		private System.Windows.Forms.CheckBox checkBox8;
		fmMotion motionFm;
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
		public Color[] colorArr;
		public Pen[] penArr;
		public CheckBox[] checkArr;
		public long CurrentTimeRange;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.TabPage tabTr;
		private System.Windows.Forms.DataGrid gridTrajectory;
		private System.Windows.Forms.TabPage tabSimul;
		private System.Windows.Forms.TabPage tabFloorHeight;
		private System.Windows.Forms.TabPage tabCalc;
		private System.Windows.Forms.DataGrid gridFloorHeight;
		private System.Windows.Forms.DataGrid gridCalc;
		private System.Windows.Forms.Label label19;
		private System.Windows.Forms.Label label20;
		private System.Windows.Forms.Label label21;
		private System.Windows.Forms.Label label22;
		private System.Windows.Forms.Label label23;
		private System.Windows.Forms.Label label24;
		private System.Windows.Forms.Label label25;
		private System.Windows.Forms.Label label26;
		private System.Windows.Forms.Label label1;
		int LastDispInx=0;
		private System.Windows.Forms.ToolBar tbSimul;
		XmlTextReader xmlReader;
		double fullSpeedDistance;
		double fullAccDistance;
		private System.Windows.Forms.DataGrid gridTraffic;
		private System.Windows.Forms.ImageList imageList1;
		private System.Windows.Forms.Label label11;
		private System.Windows.Forms.Label label12;
		private System.Windows.Forms.DataGrid gridHoistWay;
		private System.Windows.Forms.TextBox txtLobby1;
		private System.Windows.Forms.TextBox txtVel;
		private System.Windows.Forms.TextBox txtAcc;
		private System.Windows.Forms.TextBox txtJerk;
		private System.Windows.Forms.Label label13;
		private System.Windows.Forms.TextBox txtLobby2;
		private System.Windows.Forms.Label label14;
		private System.Windows.Forms.TextBox txtLobby3;
		private System.Windows.Forms.Label label15;
		private System.Windows.Forms.Label label16;
		private System.Windows.Forms.TextBox txtDOTime;
		private System.Windows.Forms.Label label3;
		private System.Windows.Forms.Label label4;
		private System.Windows.Forms.TextBox txtDCTime;
		private System.Windows.Forms.Label label5;
		private System.Windows.Forms.Label label6;
		private System.Windows.Forms.TextBox txtBoardingRatio;
		private System.Windows.Forms.TextBox txtCapacity;
		private System.Windows.Forms.Label label7;
		private System.Windows.Forms.Label label10;
		private System.Windows.Forms.TextBox txtNDoors;
		private System.Windows.Forms.Label label17;
		private System.Windows.Forms.Label label18;
		private System.Windows.Forms.TextBox txtDoorWidth;
		private System.Windows.Forms.TextBox txtStartDelay;
		private System.Windows.Forms.TextBox txtStopDelay;
		private System.Windows.Forms.Label label9;
		private System.Windows.Forms.Label label31;
		private System.Windows.Forms.Label label32;
		private System.Windows.Forms.Label label33;
		private System.Windows.Forms.Label label27;
		private System.Windows.Forms.Label label8;
		private System.Windows.Forms.Label label28;
		private System.Windows.Forms.TextBox txtHallMinDwell;
		private System.Windows.Forms.Label label29;
		private System.Windows.Forms.TextBox txtHallMaxDwell;
		private System.Windows.Forms.Label label30;
		private System.Windows.Forms.TextBox txtLobbyDwell;
		private System.Windows.Forms.CheckBox chkReopen;
		private System.Windows.Forms.Label label34;
		private System.Windows.Forms.Label label35;
		private System.Windows.Forms.Label label36;
		private System.Windows.Forms.Label label37;
		private System.Windows.Forms.TextBox txtCarDwell;
		private System.Windows.Forms.Panel pnTrajectory;
		private System.Windows.Forms.Label label38;
		private System.Windows.Forms.ComboBox cbLeading;
		private System.Windows.Forms.Label label39;
		private System.Windows.Forms.ComboBox cbTrailing;
		private System.Windows.Forms.ToolBarButton toolTSARun;
		private System.Windows.Forms.ToolBarButton toolTSAStop;
		private System.Windows.Forms.Button btnTSA;
		private System.Windows.Forms.TabPage tabHexec;
		private System.Windows.Forms.Label label40;
		
		double vel;
		public DataTable[] dtPlan;
		private System.Windows.Forms.DataGrid gridUpperPlan;
		private System.Windows.Forms.DataGrid gridLowerPlan;
		private System.Windows.Forms.ComboBox cbHwy;
		private System.Windows.Forms.DataGrid gridHexecL;
		private System.Windows.Forms.DataGrid gridHexecU;
		private System.Windows.Forms.StatusBar statusBarTSA;
		private System.Windows.Forms.StatusBarPanel statusTime;
		private System.Windows.Forms.StatusBarPanel statusMinMargin;
		private System.Windows.Forms.Button btnLoad;
		private System.Windows.Forms.OpenFileDialog openFile2dp;
		private System.Windows.Forms.Label label41;
		private System.Windows.Forms.TextBox txtMaxY;
		public DataTable[] dtHexec;
		private System.Windows.Forms.Button btnTSAAll;
		private System.Windows.Forms.Label label42;
		private System.Windows.Forms.TextBox txtSep;
		private System.Windows.Forms.Label label43;
		private System.Windows.Forms.ContextMenu contextPen;
		private System.Windows.Forms.MenuItem mnuSolid;
		private System.Windows.Forms.MenuItem mnuDot;
		private System.Windows.Forms.StatusBarPanel statusDelay;
		private System.Windows.Forms.Timer tmrSimul;
		private System.Windows.Forms.ToolBarButton toolBarButton1;
		private System.Windows.Forms.CheckBox chkOC;
		private System.Windows.Forms.CheckBox chkHPE;
		private System.Windows.Forms.CheckBox chkHP;
		private System.Windows.Forms.StatusBarPanel statusMode;
		private System.Windows.Forms.ContextMenu contextCompress;
		private System.Windows.Forms.MenuItem mnuCompress;
		private System.Windows.Forms.MenuItem mnuRestore;
		private System.Windows.Forms.Button btnClearHexec;
		private System.Windows.Forms.CheckBox ckMotion;
		private System.Windows.Forms.TextBox txtAfd;
        private TabPage tabSimulation;
        private DataGridView dataGridView1;
        private DataGridViewTextBoxColumn timeDataGridViewTextBoxColumn;
        private DataGridViewTextBoxColumn posDataGridViewTextBoxColumn;
        private DataGridViewTextBoxColumn dstDataGridViewTextBoxColumn;
        private DataGridViewTextBoxColumn carDataGridViewTextBoxColumn;
        private DataGridViewTextBoxColumn wtDataGridViewTextBoxColumn;
        private DataGridViewTextBoxColumn stDataGridViewTextBoxColumn;
        private BindingSource trafficBindingSource;
        private dsConfig dsConfig;
		bool bFormLoaded=false;
		[DllImport("user32.dll", CharSet=CharSet.Auto, ExactSpelling=true, CallingConvention=CallingConvention.Winapi)]
		public static extern short GetKeyState(int keyCode); 

		Font drawFont = new Font("Arial", 10);
		SolidBrush drawBrush = new SolidBrush(Color.Black);
		int shift;	
		public fmMain()
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(fmMain));
            this.gridDebug = new System.Windows.Forms.DataGrid();
            this.timer1 = new System.Windows.Forms.Timer(this.components);
            this.tabDebug = new System.Windows.Forms.TabControl();
            this.tabParam = new System.Windows.Forms.TabPage();
            this.tabConfig = new System.Windows.Forms.TabPage();
            this.btnLoad = new System.Windows.Forms.Button();
            this.chkReopen = new System.Windows.Forms.CheckBox();
            this.txtLobbyDwell = new System.Windows.Forms.TextBox();
            this.label30 = new System.Windows.Forms.Label();
            this.txtHallMaxDwell = new System.Windows.Forms.TextBox();
            this.label29 = new System.Windows.Forms.Label();
            this.txtHallMinDwell = new System.Windows.Forms.TextBox();
            this.label28 = new System.Windows.Forms.Label();
            this.txtCarDwell = new System.Windows.Forms.TextBox();
            this.label8 = new System.Windows.Forms.Label();
            this.gridHoistWay = new System.Windows.Forms.DataGrid();
            this.label12 = new System.Windows.Forms.Label();
            this.label11 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.txtLobby1 = new System.Windows.Forms.TextBox();
            this.txtVel = new System.Windows.Forms.TextBox();
            this.label19 = new System.Windows.Forms.Label();
            this.label20 = new System.Windows.Forms.Label();
            this.label21 = new System.Windows.Forms.Label();
            this.label22 = new System.Windows.Forms.Label();
            this.label23 = new System.Windows.Forms.Label();
            this.label24 = new System.Windows.Forms.Label();
            this.txtAcc = new System.Windows.Forms.TextBox();
            this.txtJerk = new System.Windows.Forms.TextBox();
            this.txtAfd = new System.Windows.Forms.TextBox();
            this.label25 = new System.Windows.Forms.Label();
            this.label26 = new System.Windows.Forms.Label();
            this.gridFloorHeight = new System.Windows.Forms.DataGrid();
            this.label13 = new System.Windows.Forms.Label();
            this.txtLobby2 = new System.Windows.Forms.TextBox();
            this.label14 = new System.Windows.Forms.Label();
            this.txtLobby3 = new System.Windows.Forms.TextBox();
            this.txtDOTime = new System.Windows.Forms.TextBox();
            this.label15 = new System.Windows.Forms.Label();
            this.label16 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.txtDCTime = new System.Windows.Forms.TextBox();
            this.label5 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.txtBoardingRatio = new System.Windows.Forms.TextBox();
            this.txtCapacity = new System.Windows.Forms.TextBox();
            this.label7 = new System.Windows.Forms.Label();
            this.label10 = new System.Windows.Forms.Label();
            this.txtNDoors = new System.Windows.Forms.TextBox();
            this.label17 = new System.Windows.Forms.Label();
            this.label18 = new System.Windows.Forms.Label();
            this.txtDoorWidth = new System.Windows.Forms.TextBox();
            this.txtStartDelay = new System.Windows.Forms.TextBox();
            this.txtStopDelay = new System.Windows.Forms.TextBox();
            this.label9 = new System.Windows.Forms.Label();
            this.label31 = new System.Windows.Forms.Label();
            this.label32 = new System.Windows.Forms.Label();
            this.label33 = new System.Windows.Forms.Label();
            this.label27 = new System.Windows.Forms.Label();
            this.label34 = new System.Windows.Forms.Label();
            this.label35 = new System.Windows.Forms.Label();
            this.label36 = new System.Windows.Forms.Label();
            this.label37 = new System.Windows.Forms.Label();
            this.tabTraject = new System.Windows.Forms.TabPage();
            this.contextPen = new System.Windows.Forms.ContextMenu();
            this.mnuSolid = new System.Windows.Forms.MenuItem();
            this.mnuDot = new System.Windows.Forms.MenuItem();
            this.panel1 = new System.Windows.Forms.Panel();
            this.txtMaxY = new System.Windows.Forms.TextBox();
            this.label41 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.checkBox1 = new System.Windows.Forms.CheckBox();
            this.checkBox2 = new System.Windows.Forms.CheckBox();
            this.checkBox4 = new System.Windows.Forms.CheckBox();
            this.checkBox3 = new System.Windows.Forms.CheckBox();
            this.checkBox6 = new System.Windows.Forms.CheckBox();
            this.checkBox7 = new System.Windows.Forms.CheckBox();
            this.checkBox5 = new System.Windows.Forms.CheckBox();
            this.checkBox8 = new System.Windows.Forms.CheckBox();
            this.tabPage2 = new System.Windows.Forms.TabPage();
            this.grid2d_c = new System.Windows.Forms.DataGrid();
            this.tabPage1 = new System.Windows.Forms.TabPage();
            this.grid2d_p = new System.Windows.Forms.DataGrid();
            this.tabSimul = new System.Windows.Forms.TabPage();
            this.ckMotion = new System.Windows.Forms.CheckBox();
            this.chkHP = new System.Windows.Forms.CheckBox();
            this.chkHPE = new System.Windows.Forms.CheckBox();
            this.chkOC = new System.Windows.Forms.CheckBox();
            this.label43 = new System.Windows.Forms.Label();
            this.txtSep = new System.Windows.Forms.TextBox();
            this.label42 = new System.Windows.Forms.Label();
            this.btnTSAAll = new System.Windows.Forms.Button();
            this.btnTSA = new System.Windows.Forms.Button();
            this.gridTraffic = new System.Windows.Forms.DataGrid();
            this.contextCompress = new System.Windows.Forms.ContextMenu();
            this.mnuCompress = new System.Windows.Forms.MenuItem();
            this.mnuRestore = new System.Windows.Forms.MenuItem();
            this.pnTrajectory = new System.Windows.Forms.Panel();
            this.cbTrailing = new System.Windows.Forms.ComboBox();
            this.label39 = new System.Windows.Forms.Label();
            this.cbLeading = new System.Windows.Forms.ComboBox();
            this.label38 = new System.Windows.Forms.Label();
            this.tbSimul = new System.Windows.Forms.ToolBar();
            this.toolTSARun = new System.Windows.Forms.ToolBarButton();
            this.toolTSAStop = new System.Windows.Forms.ToolBarButton();
            this.toolBarButton1 = new System.Windows.Forms.ToolBarButton();
            this.imageList1 = new System.Windows.Forms.ImageList(this.components);
            this.tabFloor = new System.Windows.Forms.TabPage();
            this.gridFloor = new System.Windows.Forms.DataGrid();
            this.tabPage3 = new System.Windows.Forms.TabPage();
            this.gridReport1 = new System.Windows.Forms.DataGrid();
            this.gridReport2 = new System.Windows.Forms.DataGrid();
            this.tabReport = new System.Windows.Forms.TabPage();
            this.crystalReportViewer1 = new CrystalDecisions.Windows.Forms.CrystalReportViewer();
            this.tabTr = new System.Windows.Forms.TabPage();
            this.gridTrajectory = new System.Windows.Forms.DataGrid();
            this.tabCalc = new System.Windows.Forms.TabPage();
            this.gridCalc = new System.Windows.Forms.DataGrid();
            this.tabHexec = new System.Windows.Forms.TabPage();
            this.btnClearHexec = new System.Windows.Forms.Button();
            this.gridLowerPlan = new System.Windows.Forms.DataGrid();
            this.gridUpperPlan = new System.Windows.Forms.DataGrid();
            this.cbHwy = new System.Windows.Forms.ComboBox();
            this.gridHexecL = new System.Windows.Forms.DataGrid();
            this.label40 = new System.Windows.Forms.Label();
            this.gridHexecU = new System.Windows.Forms.DataGrid();
            this.tabSimulation = new System.Windows.Forms.TabPage();
            this.statusBarTSA = new System.Windows.Forms.StatusBar();
            this.statusTime = new System.Windows.Forms.StatusBarPanel();
            this.statusMinMargin = new System.Windows.Forms.StatusBarPanel();
            this.statusDelay = new System.Windows.Forms.StatusBarPanel();
            this.statusMode = new System.Windows.Forms.StatusBarPanel();
            this.openFile2dp = new System.Windows.Forms.OpenFileDialog();
            this.tmrSimul = new System.Windows.Forms.Timer(this.components);
            this.dataGridView1 = new System.Windows.Forms.DataGridView();
            this.dsConfig = new COGSD.dsConfig();
            this.trafficBindingSource = new System.Windows.Forms.BindingSource(this.components);
            this.timeDataGridViewTextBoxColumn = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.posDataGridViewTextBoxColumn = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.dstDataGridViewTextBoxColumn = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.carDataGridViewTextBoxColumn = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.wtDataGridViewTextBoxColumn = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.stDataGridViewTextBoxColumn = new System.Windows.Forms.DataGridViewTextBoxColumn();
            ((System.ComponentModel.ISupportInitialize)(this.gridDebug)).BeginInit();
            this.tabDebug.SuspendLayout();
            this.tabParam.SuspendLayout();
            this.tabConfig.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.gridHoistWay)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.gridFloorHeight)).BeginInit();
            this.tabTraject.SuspendLayout();
            this.panel1.SuspendLayout();
            this.tabPage2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.grid2d_c)).BeginInit();
            this.tabPage1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.grid2d_p)).BeginInit();
            this.tabSimul.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.gridTraffic)).BeginInit();
            this.pnTrajectory.SuspendLayout();
            this.tabFloor.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.gridFloor)).BeginInit();
            this.tabPage3.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.gridReport1)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.gridReport2)).BeginInit();
            this.tabReport.SuspendLayout();
            this.tabTr.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.gridTrajectory)).BeginInit();
            this.tabCalc.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.gridCalc)).BeginInit();
            this.tabHexec.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.gridLowerPlan)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.gridUpperPlan)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.gridHexecL)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.gridHexecU)).BeginInit();
            this.tabSimulation.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.statusTime)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.statusMinMargin)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.statusDelay)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.statusMode)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.dataGridView1)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.dsConfig)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.trafficBindingSource)).BeginInit();
            this.SuspendLayout();
            // 
            // gridDebug
            // 
            this.gridDebug.DataMember = "";
            this.gridDebug.Dock = System.Windows.Forms.DockStyle.Fill;
            this.gridDebug.HeaderForeColor = System.Drawing.SystemColors.ControlText;
            this.gridDebug.Location = new System.Drawing.Point(0, 0);
            this.gridDebug.Name = "gridDebug";
            this.gridDebug.Size = new System.Drawing.Size(950, 571);
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
            this.tabDebug.Controls.Add(this.tabConfig);
            this.tabDebug.Controls.Add(this.tabTraject);
            this.tabDebug.Controls.Add(this.tabPage2);
            this.tabDebug.Controls.Add(this.tabPage1);
            this.tabDebug.Controls.Add(this.tabSimul);
            this.tabDebug.Controls.Add(this.tabFloor);
            this.tabDebug.Controls.Add(this.tabPage3);
            this.tabDebug.Controls.Add(this.tabReport);
            this.tabDebug.Controls.Add(this.tabTr);
            this.tabDebug.Controls.Add(this.tabCalc);
            this.tabDebug.Controls.Add(this.tabHexec);
            this.tabDebug.Controls.Add(this.tabSimulation);
            this.tabDebug.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabDebug.Location = new System.Drawing.Point(0, 0);
            this.tabDebug.Multiline = true;
            this.tabDebug.Name = "tabDebug";
            this.tabDebug.SelectedIndex = 0;
            this.tabDebug.Size = new System.Drawing.Size(958, 596);
            this.tabDebug.TabIndex = 1;
            this.tabDebug.SelectedIndexChanged += new System.EventHandler(this.tabControl1_SelectedIndexChanged);
            // 
            // tabParam
            // 
            this.tabParam.Controls.Add(this.gridDebug);
            this.tabParam.Location = new System.Drawing.Point(4, 21);
            this.tabParam.Name = "tabParam";
            this.tabParam.Size = new System.Drawing.Size(950, 571);
            this.tabParam.TabIndex = 0;
            this.tabParam.Text = "Parameter";
            this.tabParam.UseVisualStyleBackColor = true;
            // 
            // tabConfig
            // 
            this.tabConfig.Controls.Add(this.btnLoad);
            this.tabConfig.Controls.Add(this.chkReopen);
            this.tabConfig.Controls.Add(this.txtLobbyDwell);
            this.tabConfig.Controls.Add(this.label30);
            this.tabConfig.Controls.Add(this.txtHallMaxDwell);
            this.tabConfig.Controls.Add(this.label29);
            this.tabConfig.Controls.Add(this.txtHallMinDwell);
            this.tabConfig.Controls.Add(this.label28);
            this.tabConfig.Controls.Add(this.txtCarDwell);
            this.tabConfig.Controls.Add(this.label8);
            this.tabConfig.Controls.Add(this.gridHoistWay);
            this.tabConfig.Controls.Add(this.label12);
            this.tabConfig.Controls.Add(this.label11);
            this.tabConfig.Controls.Add(this.label1);
            this.tabConfig.Controls.Add(this.txtLobby1);
            this.tabConfig.Controls.Add(this.txtVel);
            this.tabConfig.Controls.Add(this.label19);
            this.tabConfig.Controls.Add(this.label20);
            this.tabConfig.Controls.Add(this.label21);
            this.tabConfig.Controls.Add(this.label22);
            this.tabConfig.Controls.Add(this.label23);
            this.tabConfig.Controls.Add(this.label24);
            this.tabConfig.Controls.Add(this.txtAcc);
            this.tabConfig.Controls.Add(this.txtJerk);
            this.tabConfig.Controls.Add(this.txtAfd);
            this.tabConfig.Controls.Add(this.label25);
            this.tabConfig.Controls.Add(this.label26);
            this.tabConfig.Controls.Add(this.gridFloorHeight);
            this.tabConfig.Controls.Add(this.label13);
            this.tabConfig.Controls.Add(this.txtLobby2);
            this.tabConfig.Controls.Add(this.label14);
            this.tabConfig.Controls.Add(this.txtLobby3);
            this.tabConfig.Controls.Add(this.txtDOTime);
            this.tabConfig.Controls.Add(this.label15);
            this.tabConfig.Controls.Add(this.label16);
            this.tabConfig.Controls.Add(this.label3);
            this.tabConfig.Controls.Add(this.label4);
            this.tabConfig.Controls.Add(this.txtDCTime);
            this.tabConfig.Controls.Add(this.label5);
            this.tabConfig.Controls.Add(this.label6);
            this.tabConfig.Controls.Add(this.txtBoardingRatio);
            this.tabConfig.Controls.Add(this.txtCapacity);
            this.tabConfig.Controls.Add(this.label7);
            this.tabConfig.Controls.Add(this.label10);
            this.tabConfig.Controls.Add(this.txtNDoors);
            this.tabConfig.Controls.Add(this.label17);
            this.tabConfig.Controls.Add(this.label18);
            this.tabConfig.Controls.Add(this.txtDoorWidth);
            this.tabConfig.Controls.Add(this.txtStartDelay);
            this.tabConfig.Controls.Add(this.txtStopDelay);
            this.tabConfig.Controls.Add(this.label9);
            this.tabConfig.Controls.Add(this.label31);
            this.tabConfig.Controls.Add(this.label32);
            this.tabConfig.Controls.Add(this.label33);
            this.tabConfig.Controls.Add(this.label27);
            this.tabConfig.Controls.Add(this.label34);
            this.tabConfig.Controls.Add(this.label35);
            this.tabConfig.Controls.Add(this.label36);
            this.tabConfig.Controls.Add(this.label37);
            this.tabConfig.Location = new System.Drawing.Point(4, 21);
            this.tabConfig.Name = "tabConfig";
            this.tabConfig.Size = new System.Drawing.Size(950, 571);
            this.tabConfig.TabIndex = 9;
            this.tabConfig.Text = "Config";
            this.tabConfig.UseVisualStyleBackColor = true;
            // 
            // btnLoad
            // 
            this.btnLoad.Location = new System.Drawing.Point(288, 232);
            this.btnLoad.Name = "btnLoad";
            this.btnLoad.Size = new System.Drawing.Size(96, 23);
            this.btnLoad.TabIndex = 54;
            this.btnLoad.Text = "Load 2dp file";
            this.btnLoad.Click += new System.EventHandler(this.btnLoad_Click);
            // 
            // chkReopen
            // 
            this.chkReopen.Location = new System.Drawing.Point(816, 248);
            this.chkReopen.Name = "chkReopen";
            this.chkReopen.Size = new System.Drawing.Size(80, 24);
            this.chkReopen.TabIndex = 53;
            this.chkReopen.Text = "Reopen";
            // 
            // txtLobbyDwell
            // 
            this.txtLobbyDwell.Location = new System.Drawing.Point(816, 216);
            this.txtLobbyDwell.Name = "txtLobbyDwell";
            this.txtLobbyDwell.Size = new System.Drawing.Size(80, 21);
            this.txtLobbyDwell.TabIndex = 52;
            // 
            // label30
            // 
            this.label30.Location = new System.Drawing.Point(704, 216);
            this.label30.Name = "label30";
            this.label30.Size = new System.Drawing.Size(104, 16);
            this.label30.TabIndex = 51;
            this.label30.Text = "LobbyDwellTime";
            // 
            // txtHallMaxDwell
            // 
            this.txtHallMaxDwell.Location = new System.Drawing.Point(816, 184);
            this.txtHallMaxDwell.Name = "txtHallMaxDwell";
            this.txtHallMaxDwell.Size = new System.Drawing.Size(80, 21);
            this.txtHallMaxDwell.TabIndex = 50;
            // 
            // label29
            // 
            this.label29.Location = new System.Drawing.Point(696, 184);
            this.label29.Name = "label29";
            this.label29.Size = new System.Drawing.Size(112, 16);
            this.label29.TabIndex = 49;
            this.label29.Text = "HallMaxDwellTime";
            // 
            // txtHallMinDwell
            // 
            this.txtHallMinDwell.Location = new System.Drawing.Point(816, 152);
            this.txtHallMinDwell.Name = "txtHallMinDwell";
            this.txtHallMinDwell.Size = new System.Drawing.Size(80, 21);
            this.txtHallMinDwell.TabIndex = 48;
            // 
            // label28
            // 
            this.label28.Location = new System.Drawing.Point(696, 152);
            this.label28.Name = "label28";
            this.label28.Size = new System.Drawing.Size(112, 16);
            this.label28.TabIndex = 47;
            this.label28.Text = "HallMinDwellTime";
            // 
            // txtCarDwell
            // 
            this.txtCarDwell.Location = new System.Drawing.Point(816, 120);
            this.txtCarDwell.Name = "txtCarDwell";
            this.txtCarDwell.Size = new System.Drawing.Size(80, 21);
            this.txtCarDwell.TabIndex = 46;
            // 
            // label8
            // 
            this.label8.Location = new System.Drawing.Point(720, 120);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(88, 23);
            this.label8.TabIndex = 45;
            this.label8.Text = "CarDwellTime";
            // 
            // gridHoistWay
            // 
            this.gridHoistWay.DataMember = "";
            this.gridHoistWay.HeaderForeColor = System.Drawing.SystemColors.ControlText;
            this.gridHoistWay.Location = new System.Drawing.Point(288, 40);
            this.gridHoistWay.Name = "gridHoistWay";
            this.gridHoistWay.Size = new System.Drawing.Size(168, 184);
            this.gridHoistWay.TabIndex = 44;
            // 
            // label12
            // 
            this.label12.Location = new System.Drawing.Point(288, 16);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(88, 16);
            this.label12.TabIndex = 43;
            this.label12.Text = "HoistWay Information";
            // 
            // label11
            // 
            this.label11.Location = new System.Drawing.Point(32, 16);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(152, 16);
            this.label11.TabIndex = 42;
            this.label11.Text = "Floor Information";
            // 
            // label1
            // 
            this.label1.Location = new System.Drawing.Point(728, 32);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(80, 16);
            this.label1.TabIndex = 16;
            this.label1.Text = "lobby1_pos :";
            // 
            // txtLobby1
            // 
            this.txtLobby1.Location = new System.Drawing.Point(816, 24);
            this.txtLobby1.Name = "txtLobby1";
            this.txtLobby1.Size = new System.Drawing.Size(80, 21);
            this.txtLobby1.TabIndex = 15;
            this.txtLobby1.Text = "0";
            // 
            // txtVel
            // 
            this.txtVel.Location = new System.Drawing.Point(584, 24);
            this.txtVel.Name = "txtVel";
            this.txtVel.Size = new System.Drawing.Size(48, 21);
            this.txtVel.TabIndex = 11;
            this.txtVel.TextChanged += new System.EventHandler(this.ConfigChanged);
            // 
            // label19
            // 
            this.label19.Location = new System.Drawing.Point(544, 32);
            this.label19.Name = "label19";
            this.label19.Size = new System.Drawing.Size(32, 16);
            this.label19.TabIndex = 8;
            this.label19.Text = "Vel";
            // 
            // label20
            // 
            this.label20.Location = new System.Drawing.Point(544, 60);
            this.label20.Name = "label20";
            this.label20.Size = new System.Drawing.Size(32, 16);
            this.label20.TabIndex = 9;
            this.label20.Text = "Acc";
            // 
            // label21
            // 
            this.label21.Location = new System.Drawing.Point(544, 88);
            this.label21.Name = "label21";
            this.label21.Size = new System.Drawing.Size(32, 16);
            this.label21.TabIndex = 10;
            this.label21.Text = "Jerk";
            // 
            // label22
            // 
            this.label22.Location = new System.Drawing.Point(640, 87);
            this.label22.Name = "label22";
            this.label22.Size = new System.Drawing.Size(56, 16);
            this.label22.TabIndex = 7;
            this.label22.Text = "mm/sss";
            // 
            // label23
            // 
            this.label23.Location = new System.Drawing.Point(640, 59);
            this.label23.Name = "label23";
            this.label23.Size = new System.Drawing.Size(72, 16);
            this.label23.TabIndex = 4;
            this.label23.Text = "mm/ss";
            // 
            // label24
            // 
            this.label24.Location = new System.Drawing.Point(640, 31);
            this.label24.Name = "label24";
            this.label24.Size = new System.Drawing.Size(48, 16);
            this.label24.TabIndex = 3;
            this.label24.Text = "mm/s";
            // 
            // txtAcc
            // 
            this.txtAcc.Location = new System.Drawing.Point(584, 53);
            this.txtAcc.Name = "txtAcc";
            this.txtAcc.Size = new System.Drawing.Size(48, 21);
            this.txtAcc.TabIndex = 13;
            this.txtAcc.TextChanged += new System.EventHandler(this.ConfigChanged);
            // 
            // txtJerk
            // 
            this.txtJerk.Location = new System.Drawing.Point(584, 82);
            this.txtJerk.Name = "txtJerk";
            this.txtJerk.Size = new System.Drawing.Size(48, 21);
            this.txtJerk.TabIndex = 14;
            this.txtJerk.TextChanged += new System.EventHandler(this.ConfigChanged);
            // 
            // txtAfd
            // 
            this.txtAfd.Location = new System.Drawing.Point(584, 111);
            this.txtAfd.Name = "txtAfd";
            this.txtAfd.Size = new System.Drawing.Size(48, 21);
            this.txtAfd.TabIndex = 12;
            this.txtAfd.TextChanged += new System.EventHandler(this.ConfigChanged);
            // 
            // label25
            // 
            this.label25.Location = new System.Drawing.Point(492, 116);
            this.label25.Name = "label25";
            this.label25.Size = new System.Drawing.Size(88, 16);
            this.label25.TabIndex = 6;
            this.label25.Text = "Forced Decel";
            // 
            // label26
            // 
            this.label26.Location = new System.Drawing.Point(640, 115);
            this.label26.Name = "label26";
            this.label26.Size = new System.Drawing.Size(48, 16);
            this.label26.TabIndex = 5;
            this.label26.Text = "mm/ss";
            // 
            // gridFloorHeight
            // 
            this.gridFloorHeight.DataMember = "";
            this.gridFloorHeight.HeaderForeColor = System.Drawing.SystemColors.ControlText;
            this.gridFloorHeight.Location = new System.Drawing.Point(32, 40);
            this.gridFloorHeight.Name = "gridFloorHeight";
            this.gridFloorHeight.Size = new System.Drawing.Size(248, 496);
            this.gridFloorHeight.TabIndex = 0;
            this.gridFloorHeight.CurrentCellChanged += new System.EventHandler(this.gridFloorHeight_CurrentCellChanged);
            // 
            // label13
            // 
            this.label13.Location = new System.Drawing.Point(728, 56);
            this.label13.Name = "label13";
            this.label13.Size = new System.Drawing.Size(80, 16);
            this.label13.TabIndex = 16;
            this.label13.Text = "lobby2_pos :";
            // 
            // txtLobby2
            // 
            this.txtLobby2.Location = new System.Drawing.Point(816, 56);
            this.txtLobby2.Name = "txtLobby2";
            this.txtLobby2.Size = new System.Drawing.Size(80, 21);
            this.txtLobby2.TabIndex = 15;
            this.txtLobby2.Text = "0";
            // 
            // label14
            // 
            this.label14.Location = new System.Drawing.Point(728, 88);
            this.label14.Name = "label14";
            this.label14.Size = new System.Drawing.Size(80, 16);
            this.label14.TabIndex = 16;
            this.label14.Text = "lobby3_pos :";
            // 
            // txtLobby3
            // 
            this.txtLobby3.Location = new System.Drawing.Point(816, 88);
            this.txtLobby3.Name = "txtLobby3";
            this.txtLobby3.Size = new System.Drawing.Size(80, 21);
            this.txtLobby3.TabIndex = 15;
            this.txtLobby3.Text = "0";
            // 
            // txtDOTime
            // 
            this.txtDOTime.Location = new System.Drawing.Point(584, 140);
            this.txtDOTime.Name = "txtDOTime";
            this.txtDOTime.Size = new System.Drawing.Size(48, 21);
            this.txtDOTime.TabIndex = 12;
            this.txtDOTime.TextChanged += new System.EventHandler(this.ConfigChanged);
            // 
            // label15
            // 
            this.label15.Location = new System.Drawing.Point(520, 144);
            this.label15.Name = "label15";
            this.label15.Size = new System.Drawing.Size(56, 16);
            this.label15.TabIndex = 6;
            this.label15.Text = "DOTime";
            // 
            // label16
            // 
            this.label16.Location = new System.Drawing.Point(640, 143);
            this.label16.Name = "label16";
            this.label16.Size = new System.Drawing.Size(40, 16);
            this.label16.TabIndex = 5;
            this.label16.Text = "msec";
            // 
            // label3
            // 
            this.label3.Location = new System.Drawing.Point(640, 171);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(40, 16);
            this.label3.TabIndex = 5;
            this.label3.Text = "msec";
            // 
            // label4
            // 
            this.label4.Location = new System.Drawing.Point(520, 172);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(56, 16);
            this.label4.TabIndex = 6;
            this.label4.Text = "DCTime";
            // 
            // txtDCTime
            // 
            this.txtDCTime.Location = new System.Drawing.Point(584, 169);
            this.txtDCTime.Name = "txtDCTime";
            this.txtDCTime.Size = new System.Drawing.Size(48, 21);
            this.txtDCTime.TabIndex = 12;
            this.txtDCTime.TextChanged += new System.EventHandler(this.ConfigChanged);
            // 
            // label5
            // 
            this.label5.Location = new System.Drawing.Point(640, 199);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(40, 16);
            this.label5.TabIndex = 5;
            this.label5.Text = "%";
            // 
            // label6
            // 
            this.label6.Location = new System.Drawing.Point(488, 200);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(88, 16);
            this.label6.TabIndex = 6;
            this.label6.Text = "BoardingRatio";
            // 
            // txtBoardingRatio
            // 
            this.txtBoardingRatio.Location = new System.Drawing.Point(584, 198);
            this.txtBoardingRatio.Name = "txtBoardingRatio";
            this.txtBoardingRatio.Size = new System.Drawing.Size(48, 21);
            this.txtBoardingRatio.TabIndex = 12;
            // 
            // txtCapacity
            // 
            this.txtCapacity.Location = new System.Drawing.Point(584, 227);
            this.txtCapacity.Name = "txtCapacity";
            this.txtCapacity.Size = new System.Drawing.Size(48, 21);
            this.txtCapacity.TabIndex = 12;
            // 
            // label7
            // 
            this.label7.Location = new System.Drawing.Point(520, 228);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(56, 16);
            this.label7.TabIndex = 6;
            this.label7.Text = "capacity";
            // 
            // label10
            // 
            this.label10.Location = new System.Drawing.Point(512, 256);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(56, 16);
            this.label10.TabIndex = 6;
            this.label10.Text = "nDoors";
            // 
            // txtNDoors
            // 
            this.txtNDoors.Location = new System.Drawing.Point(584, 256);
            this.txtNDoors.Name = "txtNDoors";
            this.txtNDoors.Size = new System.Drawing.Size(48, 21);
            this.txtNDoors.TabIndex = 12;
            // 
            // label17
            // 
            this.label17.Location = new System.Drawing.Point(496, 284);
            this.label17.Name = "label17";
            this.label17.Size = new System.Drawing.Size(72, 16);
            this.label17.TabIndex = 6;
            this.label17.Text = "DoorWidth";
            // 
            // label18
            // 
            this.label18.Location = new System.Drawing.Point(632, 288);
            this.label18.Name = "label18";
            this.label18.Size = new System.Drawing.Size(40, 16);
            this.label18.TabIndex = 5;
            this.label18.Text = "mm";
            // 
            // txtDoorWidth
            // 
            this.txtDoorWidth.Location = new System.Drawing.Point(584, 285);
            this.txtDoorWidth.Name = "txtDoorWidth";
            this.txtDoorWidth.Size = new System.Drawing.Size(48, 21);
            this.txtDoorWidth.TabIndex = 12;
            // 
            // txtStartDelay
            // 
            this.txtStartDelay.Location = new System.Drawing.Point(584, 314);
            this.txtStartDelay.Name = "txtStartDelay";
            this.txtStartDelay.Size = new System.Drawing.Size(48, 21);
            this.txtStartDelay.TabIndex = 12;
            this.txtStartDelay.TextChanged += new System.EventHandler(this.ConfigChanged);
            // 
            // txtStopDelay
            // 
            this.txtStopDelay.Location = new System.Drawing.Point(584, 342);
            this.txtStopDelay.Name = "txtStopDelay";
            this.txtStopDelay.Size = new System.Drawing.Size(48, 21);
            this.txtStopDelay.TabIndex = 12;
            this.txtStopDelay.TextChanged += new System.EventHandler(this.ConfigChanged);
            // 
            // label9
            // 
            this.label9.Location = new System.Drawing.Point(504, 340);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(64, 16);
            this.label9.TabIndex = 6;
            this.label9.Text = "StopDelay";
            // 
            // label31
            // 
            this.label31.Location = new System.Drawing.Point(632, 344);
            this.label31.Name = "label31";
            this.label31.Size = new System.Drawing.Size(40, 16);
            this.label31.TabIndex = 5;
            this.label31.Text = "msec";
            // 
            // label32
            // 
            this.label32.Location = new System.Drawing.Point(504, 312);
            this.label32.Name = "label32";
            this.label32.Size = new System.Drawing.Size(64, 16);
            this.label32.TabIndex = 6;
            this.label32.Text = "StartDelay";
            // 
            // label33
            // 
            this.label33.Location = new System.Drawing.Point(632, 316);
            this.label33.Name = "label33";
            this.label33.Size = new System.Drawing.Size(40, 16);
            this.label33.TabIndex = 5;
            this.label33.Text = "msec";
            // 
            // label27
            // 
            this.label27.Location = new System.Drawing.Point(632, 227);
            this.label27.Name = "label27";
            this.label27.Size = new System.Drawing.Size(48, 16);
            this.label27.TabIndex = 5;
            this.label27.Text = "Person";
            // 
            // label34
            // 
            this.label34.Location = new System.Drawing.Point(904, 120);
            this.label34.Name = "label34";
            this.label34.Size = new System.Drawing.Size(40, 16);
            this.label34.TabIndex = 5;
            this.label34.Text = "msec";
            // 
            // label35
            // 
            this.label35.Location = new System.Drawing.Point(904, 152);
            this.label35.Name = "label35";
            this.label35.Size = new System.Drawing.Size(40, 16);
            this.label35.TabIndex = 5;
            this.label35.Text = "msec";
            // 
            // label36
            // 
            this.label36.Location = new System.Drawing.Point(904, 184);
            this.label36.Name = "label36";
            this.label36.Size = new System.Drawing.Size(40, 16);
            this.label36.TabIndex = 5;
            this.label36.Text = "msec";
            // 
            // label37
            // 
            this.label37.Location = new System.Drawing.Point(904, 216);
            this.label37.Name = "label37";
            this.label37.Size = new System.Drawing.Size(40, 16);
            this.label37.TabIndex = 5;
            this.label37.Text = "msec";
            // 
            // tabTraject
            // 
            this.tabTraject.BackColor = System.Drawing.SystemColors.ActiveCaptionText;
            this.tabTraject.ContextMenu = this.contextPen;
            this.tabTraject.Controls.Add(this.panel1);
            this.tabTraject.Location = new System.Drawing.Point(4, 21);
            this.tabTraject.Name = "tabTraject";
            this.tabTraject.Size = new System.Drawing.Size(950, 571);
            this.tabTraject.TabIndex = 6;
            this.tabTraject.Text = "Trajectory";
            this.tabTraject.UseVisualStyleBackColor = true;
            this.tabTraject.Resize += new System.EventHandler(this.tabTraject_Resize);
            this.tabTraject.Paint += new System.Windows.Forms.PaintEventHandler(this.tabTraject_Paint);
            // 
            // contextPen
            // 
            this.contextPen.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
            this.mnuSolid,
            this.mnuDot});
            // 
            // mnuSolid
            // 
            this.mnuSolid.Index = 0;
            this.mnuSolid.Text = "Normal";
            // 
            // mnuDot
            // 
            this.mnuDot.Index = 1;
            this.mnuDot.Text = "Dot";
            this.mnuDot.Click += new System.EventHandler(this.mnuDot_Click);
            // 
            // panel1
            // 
            this.panel1.BackColor = System.Drawing.SystemColors.AppWorkspace;
            this.panel1.Controls.Add(this.txtMaxY);
            this.panel1.Controls.Add(this.label41);
            this.panel1.Controls.Add(this.label2);
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
            this.panel1.Size = new System.Drawing.Size(950, 40);
            this.panel1.TabIndex = 0;
            // 
            // txtMaxY
            // 
            this.txtMaxY.Location = new System.Drawing.Point(112, 8);
            this.txtMaxY.Name = "txtMaxY";
            this.txtMaxY.ReadOnly = true;
            this.txtMaxY.RightToLeft = System.Windows.Forms.RightToLeft.Yes;
            this.txtMaxY.Size = new System.Drawing.Size(80, 21);
            this.txtMaxY.TabIndex = 16;
            // 
            // label41
            // 
            this.label41.Location = new System.Drawing.Point(200, 13);
            this.label41.Name = "label41";
            this.label41.Size = new System.Drawing.Size(100, 23);
            this.label41.TabIndex = 15;
            this.label41.Text = "mm";
            // 
            // label2
            // 
            this.label2.Location = new System.Drawing.Point(8, 13);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(96, 16);
            this.label2.TabIndex = 14;
            this.label2.Text = "Building Height";
            // 
            // checkBox1
            // 
            this.checkBox1.Appearance = System.Windows.Forms.Appearance.Button;
            this.checkBox1.BackColor = System.Drawing.Color.IndianRed;
            this.checkBox1.ContextMenu = this.contextPen;
            this.checkBox1.Location = new System.Drawing.Point(544, 8);
            this.checkBox1.Name = "checkBox1";
            this.checkBox1.Size = new System.Drawing.Size(48, 24);
            this.checkBox1.TabIndex = 10;
            this.checkBox1.Text = "CAR1";
            this.checkBox1.UseVisualStyleBackColor = false;
            this.checkBox1.MouseDown += new System.Windows.Forms.MouseEventHandler(this.checkBox1_MouseDown);
            this.checkBox1.CheckedChanged += new System.EventHandler(this.checkBox1_CheckedChanged);
            // 
            // checkBox2
            // 
            this.checkBox2.Appearance = System.Windows.Forms.Appearance.Button;
            this.checkBox2.BackColor = System.Drawing.Color.IndianRed;
            this.checkBox2.ContextMenu = this.contextPen;
            this.checkBox2.Location = new System.Drawing.Point(592, 8);
            this.checkBox2.Name = "checkBox2";
            this.checkBox2.Size = new System.Drawing.Size(48, 24);
            this.checkBox2.TabIndex = 9;
            this.checkBox2.Text = "CAR2";
            this.checkBox2.UseVisualStyleBackColor = false;
            this.checkBox2.MouseDown += new System.Windows.Forms.MouseEventHandler(this.checkBox1_MouseDown);
            this.checkBox2.CheckedChanged += new System.EventHandler(this.checkBox1_CheckedChanged);
            // 
            // checkBox4
            // 
            this.checkBox4.Appearance = System.Windows.Forms.Appearance.Button;
            this.checkBox4.BackColor = System.Drawing.Color.IndianRed;
            this.checkBox4.ContextMenu = this.contextPen;
            this.checkBox4.Location = new System.Drawing.Point(688, 8);
            this.checkBox4.Name = "checkBox4";
            this.checkBox4.Size = new System.Drawing.Size(48, 24);
            this.checkBox4.TabIndex = 12;
            this.checkBox4.Text = "CAR4";
            this.checkBox4.UseVisualStyleBackColor = false;
            this.checkBox4.MouseDown += new System.Windows.Forms.MouseEventHandler(this.checkBox1_MouseDown);
            this.checkBox4.CheckedChanged += new System.EventHandler(this.checkBox1_CheckedChanged);
            // 
            // checkBox3
            // 
            this.checkBox3.Appearance = System.Windows.Forms.Appearance.Button;
            this.checkBox3.BackColor = System.Drawing.Color.IndianRed;
            this.checkBox3.ContextMenu = this.contextPen;
            this.checkBox3.Location = new System.Drawing.Point(640, 8);
            this.checkBox3.Name = "checkBox3";
            this.checkBox3.Size = new System.Drawing.Size(48, 24);
            this.checkBox3.TabIndex = 11;
            this.checkBox3.Text = "CAR3";
            this.checkBox3.UseVisualStyleBackColor = false;
            this.checkBox3.MouseDown += new System.Windows.Forms.MouseEventHandler(this.checkBox1_MouseDown);
            this.checkBox3.CheckedChanged += new System.EventHandler(this.checkBox1_CheckedChanged);
            // 
            // checkBox6
            // 
            this.checkBox6.Appearance = System.Windows.Forms.Appearance.Button;
            this.checkBox6.BackColor = System.Drawing.Color.IndianRed;
            this.checkBox6.ContextMenu = this.contextPen;
            this.checkBox6.Location = new System.Drawing.Point(784, 8);
            this.checkBox6.Name = "checkBox6";
            this.checkBox6.Size = new System.Drawing.Size(48, 24);
            this.checkBox6.TabIndex = 6;
            this.checkBox6.Text = "CAR6";
            this.checkBox6.UseVisualStyleBackColor = false;
            this.checkBox6.MouseDown += new System.Windows.Forms.MouseEventHandler(this.checkBox1_MouseDown);
            this.checkBox6.CheckedChanged += new System.EventHandler(this.checkBox1_CheckedChanged);
            // 
            // checkBox7
            // 
            this.checkBox7.Appearance = System.Windows.Forms.Appearance.Button;
            this.checkBox7.BackColor = System.Drawing.Color.IndianRed;
            this.checkBox7.ContextMenu = this.contextPen;
            this.checkBox7.Location = new System.Drawing.Point(832, 8);
            this.checkBox7.Name = "checkBox7";
            this.checkBox7.Size = new System.Drawing.Size(48, 24);
            this.checkBox7.TabIndex = 5;
            this.checkBox7.Text = "CAR7";
            this.checkBox7.UseVisualStyleBackColor = false;
            this.checkBox7.MouseDown += new System.Windows.Forms.MouseEventHandler(this.checkBox1_MouseDown);
            this.checkBox7.CheckedChanged += new System.EventHandler(this.checkBox1_CheckedChanged);
            // 
            // checkBox5
            // 
            this.checkBox5.Appearance = System.Windows.Forms.Appearance.Button;
            this.checkBox5.BackColor = System.Drawing.Color.IndianRed;
            this.checkBox5.ContextMenu = this.contextPen;
            this.checkBox5.Location = new System.Drawing.Point(736, 8);
            this.checkBox5.Name = "checkBox5";
            this.checkBox5.Size = new System.Drawing.Size(48, 24);
            this.checkBox5.TabIndex = 8;
            this.checkBox5.Text = "CAR5";
            this.checkBox5.UseVisualStyleBackColor = false;
            this.checkBox5.MouseDown += new System.Windows.Forms.MouseEventHandler(this.checkBox1_MouseDown);
            this.checkBox5.CheckedChanged += new System.EventHandler(this.checkBox1_CheckedChanged);
            // 
            // checkBox8
            // 
            this.checkBox8.Appearance = System.Windows.Forms.Appearance.Button;
            this.checkBox8.BackColor = System.Drawing.Color.IndianRed;
            this.checkBox8.ContextMenu = this.contextPen;
            this.checkBox8.Location = new System.Drawing.Point(880, 8);
            this.checkBox8.Name = "checkBox8";
            this.checkBox8.Size = new System.Drawing.Size(48, 24);
            this.checkBox8.TabIndex = 7;
            this.checkBox8.Text = "CAR8";
            this.checkBox8.UseVisualStyleBackColor = false;
            this.checkBox8.MouseDown += new System.Windows.Forms.MouseEventHandler(this.checkBox1_MouseDown);
            this.checkBox8.CheckedChanged += new System.EventHandler(this.checkBox1_CheckedChanged);
            // 
            // tabPage2
            // 
            this.tabPage2.Controls.Add(this.grid2d_c);
            this.tabPage2.Location = new System.Drawing.Point(4, 21);
            this.tabPage2.Name = "tabPage2";
            this.tabPage2.Size = new System.Drawing.Size(950, 571);
            this.tabPage2.TabIndex = 3;
            this.tabPage2.Text = "2d-c";
            this.tabPage2.UseVisualStyleBackColor = true;
            // 
            // grid2d_c
            // 
            this.grid2d_c.DataMember = "";
            this.grid2d_c.Dock = System.Windows.Forms.DockStyle.Fill;
            this.grid2d_c.HeaderForeColor = System.Drawing.SystemColors.ControlText;
            this.grid2d_c.Location = new System.Drawing.Point(0, 0);
            this.grid2d_c.Name = "grid2d_c";
            this.grid2d_c.Size = new System.Drawing.Size(950, 571);
            this.grid2d_c.TabIndex = 0;
            // 
            // tabPage1
            // 
            this.tabPage1.Controls.Add(this.grid2d_p);
            this.tabPage1.Location = new System.Drawing.Point(4, 21);
            this.tabPage1.Name = "tabPage1";
            this.tabPage1.Size = new System.Drawing.Size(950, 571);
            this.tabPage1.TabIndex = 2;
            this.tabPage1.Text = "2d-p";
            this.tabPage1.UseVisualStyleBackColor = true;
            // 
            // grid2d_p
            // 
            this.grid2d_p.DataMember = "";
            this.grid2d_p.Dock = System.Windows.Forms.DockStyle.Fill;
            this.grid2d_p.HeaderForeColor = System.Drawing.SystemColors.ControlText;
            this.grid2d_p.Location = new System.Drawing.Point(0, 0);
            this.grid2d_p.Name = "grid2d_p";
            this.grid2d_p.Size = new System.Drawing.Size(950, 571);
            this.grid2d_p.TabIndex = 0;
            // 
            // tabSimul
            // 
            this.tabSimul.Controls.Add(this.ckMotion);
            this.tabSimul.Controls.Add(this.chkHP);
            this.tabSimul.Controls.Add(this.chkHPE);
            this.tabSimul.Controls.Add(this.chkOC);
            this.tabSimul.Controls.Add(this.label43);
            this.tabSimul.Controls.Add(this.txtSep);
            this.tabSimul.Controls.Add(this.label42);
            this.tabSimul.Controls.Add(this.btnTSAAll);
            this.tabSimul.Controls.Add(this.btnTSA);
            this.tabSimul.Controls.Add(this.gridTraffic);
            this.tabSimul.Controls.Add(this.pnTrajectory);
            this.tabSimul.Location = new System.Drawing.Point(4, 21);
            this.tabSimul.Name = "tabSimul";
            this.tabSimul.Size = new System.Drawing.Size(950, 571);
            this.tabSimul.TabIndex = 8;
            this.tabSimul.Tag = "\"FIRST\"";
            this.tabSimul.Text = "TSA_DELAY";
            this.tabSimul.UseVisualStyleBackColor = true;
            this.tabSimul.Click += new System.EventHandler(this.tabSimul_Click);
            this.tabSimul.Resize += new System.EventHandler(this.tabSimul_Resize);
            this.tabSimul.Paint += new System.Windows.Forms.PaintEventHandler(this.tabSimul_Paint);
            // 
            // ckMotion
            // 
            this.ckMotion.Location = new System.Drawing.Point(232, 528);
            this.ckMotion.Name = "ckMotion";
            this.ckMotion.Size = new System.Drawing.Size(128, 24);
            this.ckMotion.TabIndex = 12;
            this.ckMotion.Text = "Dynamic Motion";
            // 
            // chkHP
            // 
            this.chkHP.Location = new System.Drawing.Point(168, 528);
            this.chkHP.Name = "chkHP";
            this.chkHP.Size = new System.Drawing.Size(48, 24);
            this.chkHP.TabIndex = 11;
            this.chkHP.Text = "HP";
            this.chkHP.CheckedChanged += new System.EventHandler(this.TSAChk_Changed);
            // 
            // chkHPE
            // 
            this.chkHPE.Location = new System.Drawing.Point(88, 528);
            this.chkHPE.Name = "chkHPE";
            this.chkHPE.Size = new System.Drawing.Size(64, 24);
            this.chkHPE.TabIndex = 10;
            this.chkHPE.Text = "HPE";
            this.chkHPE.CheckedChanged += new System.EventHandler(this.TSAChk_Changed);
            // 
            // chkOC
            // 
            this.chkOC.Location = new System.Drawing.Point(16, 528);
            this.chkOC.Name = "chkOC";
            this.chkOC.Size = new System.Drawing.Size(56, 24);
            this.chkOC.TabIndex = 9;
            this.chkOC.Text = "OC";
            this.chkOC.CheckedChanged += new System.EventHandler(this.TSAChk_Changed);
            // 
            // label43
            // 
            this.label43.Location = new System.Drawing.Point(320, 22);
            this.label43.Name = "label43";
            this.label43.Size = new System.Drawing.Size(32, 16);
            this.label43.TabIndex = 8;
            this.label43.Text = "mm";
            // 
            // txtSep
            // 
            this.txtSep.Location = new System.Drawing.Point(264, 16);
            this.txtSep.Name = "txtSep";
            this.txtSep.Size = new System.Drawing.Size(48, 21);
            this.txtSep.TabIndex = 7;
            this.txtSep.Text = "4000";
            // 
            // label42
            // 
            this.label42.Location = new System.Drawing.Point(224, 22);
            this.label42.Name = "label42";
            this.label42.Size = new System.Drawing.Size(40, 16);
            this.label42.TabIndex = 6;
            this.label42.Text = "Sep :";
            // 
            // btnTSAAll
            // 
            this.btnTSAAll.Location = new System.Drawing.Point(104, 16);
            this.btnTSAAll.Name = "btnTSAAll";
            this.btnTSAAll.Size = new System.Drawing.Size(96, 23);
            this.btnTSAAll.TabIndex = 5;
            this.btnTSAAll.Text = "TSA Delay All";
            this.btnTSAAll.Click += new System.EventHandler(this.btnTSAAll_Click);
            // 
            // btnTSA
            // 
            this.btnTSA.Location = new System.Drawing.Point(16, 16);
            this.btnTSA.Name = "btnTSA";
            this.btnTSA.Size = new System.Drawing.Size(75, 23);
            this.btnTSA.TabIndex = 4;
            this.btnTSA.Text = "TSA Delay";
            this.btnTSA.Click += new System.EventHandler(this.btnTSA_Click);
            // 
            // gridTraffic
            // 
            this.gridTraffic.ContextMenu = this.contextCompress;
            this.gridTraffic.DataMember = "";
            this.gridTraffic.HeaderForeColor = System.Drawing.SystemColors.ControlText;
            this.gridTraffic.Location = new System.Drawing.Point(16, 40);
            this.gridTraffic.Name = "gridTraffic";
            this.gridTraffic.Size = new System.Drawing.Size(192, 472);
            this.gridTraffic.TabIndex = 3;
            this.gridTraffic.CurrentCellChanged += new System.EventHandler(this.gridTraffic_CurrentCellChanged);
            // 
            // contextCompress
            // 
            this.contextCompress.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
            this.mnuCompress,
            this.mnuRestore});
            // 
            // mnuCompress
            // 
            this.mnuCompress.Index = 0;
            this.mnuCompress.Text = "Compress";
            this.mnuCompress.Click += new System.EventHandler(this.mnuCompress_Click);
            // 
            // mnuRestore
            // 
            this.mnuRestore.Index = 1;
            this.mnuRestore.Text = "Resotre from Compressed table";
            this.mnuRestore.Click += new System.EventHandler(this.mnuRestore_Click);
            // 
            // pnTrajectory
            // 
            this.pnTrajectory.BackColor = System.Drawing.Color.White;
            this.pnTrajectory.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.pnTrajectory.Controls.Add(this.cbTrailing);
            this.pnTrajectory.Controls.Add(this.label39);
            this.pnTrajectory.Controls.Add(this.cbLeading);
            this.pnTrajectory.Controls.Add(this.label38);
            this.pnTrajectory.Controls.Add(this.tbSimul);
            this.pnTrajectory.Location = new System.Drawing.Point(352, 16);
            this.pnTrajectory.Name = "pnTrajectory";
            this.pnTrajectory.Size = new System.Drawing.Size(592, 504);
            this.pnTrajectory.TabIndex = 2;
            this.pnTrajectory.Resize += new System.EventHandler(this.tabSimul_Resize);
            this.pnTrajectory.Paint += new System.Windows.Forms.PaintEventHandler(this.tabSimul_Paint);
            // 
            // cbTrailing
            // 
            this.cbTrailing.Items.AddRange(new object[] {
            "Position",
            "ESP",
            "NSF",
            "NSP",
            "NCF"});
            this.cbTrailing.Location = new System.Drawing.Point(488, 12);
            this.cbTrailing.Name = "cbTrailing";
            this.cbTrailing.Size = new System.Drawing.Size(96, 20);
            this.cbTrailing.TabIndex = 4;
            this.cbTrailing.Text = "Position";
            // 
            // label39
            // 
            this.label39.BackColor = System.Drawing.SystemColors.ActiveBorder;
            this.label39.Location = new System.Drawing.Point(408, 16);
            this.label39.Name = "label39";
            this.label39.Size = new System.Drawing.Size(88, 16);
            this.label39.TabIndex = 3;
            this.label39.Text = "Trailing Car";
            // 
            // cbLeading
            // 
            this.cbLeading.Items.AddRange(new object[] {
            "Position",
            "ESP",
            "NSF",
            "NSP",
            "NCF"});
            this.cbLeading.Location = new System.Drawing.Point(304, 13);
            this.cbLeading.Name = "cbLeading";
            this.cbLeading.Size = new System.Drawing.Size(80, 20);
            this.cbLeading.TabIndex = 2;
            this.cbLeading.Text = "Position";
            // 
            // label38
            // 
            this.label38.BackColor = System.Drawing.SystemColors.ActiveBorder;
            this.label38.Location = new System.Drawing.Point(216, 16);
            this.label38.Name = "label38";
            this.label38.Size = new System.Drawing.Size(80, 16);
            this.label38.TabIndex = 1;
            this.label38.Text = "Leading Car";
            // 
            // tbSimul
            // 
            this.tbSimul.Buttons.AddRange(new System.Windows.Forms.ToolBarButton[] {
            this.toolTSARun,
            this.toolTSAStop,
            this.toolBarButton1});
            this.tbSimul.ButtonSize = new System.Drawing.Size(40, 40);
            this.tbSimul.DropDownArrows = true;
            this.tbSimul.ImageList = this.imageList1;
            this.tbSimul.Location = new System.Drawing.Point(0, 0);
            this.tbSimul.Name = "tbSimul";
            this.tbSimul.ShowToolTips = true;
            this.tbSimul.Size = new System.Drawing.Size(588, 41);
            this.tbSimul.TabIndex = 0;
            this.tbSimul.ButtonClick += new System.Windows.Forms.ToolBarButtonClickEventHandler(this.tbSimul_ButtonClick);
            // 
            // toolTSARun
            // 
            this.toolTSARun.ImageIndex = 1;
            this.toolTSARun.Name = "toolTSARun";
            this.toolTSARun.Text = "RUN";
            // 
            // toolTSAStop
            // 
            this.toolTSAStop.ImageIndex = 2;
            this.toolTSAStop.Name = "toolTSAStop";
            this.toolTSAStop.Text = "STOP";
            // 
            // toolBarButton1
            // 
            this.toolBarButton1.ImageIndex = 13;
            this.toolBarButton1.Name = "toolBarButton1";
            this.toolBarButton1.Text = "PAU";
            // 
            // imageList1
            // 
            this.imageList1.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageList1.ImageStream")));
            this.imageList1.TransparentColor = System.Drawing.Color.Transparent;
            this.imageList1.Images.SetKeyName(0, "");
            this.imageList1.Images.SetKeyName(1, "");
            this.imageList1.Images.SetKeyName(2, "");
            this.imageList1.Images.SetKeyName(3, "");
            this.imageList1.Images.SetKeyName(4, "");
            this.imageList1.Images.SetKeyName(5, "");
            this.imageList1.Images.SetKeyName(6, "");
            this.imageList1.Images.SetKeyName(7, "");
            this.imageList1.Images.SetKeyName(8, "");
            this.imageList1.Images.SetKeyName(9, "");
            this.imageList1.Images.SetKeyName(10, "");
            this.imageList1.Images.SetKeyName(11, "");
            this.imageList1.Images.SetKeyName(12, "");
            this.imageList1.Images.SetKeyName(13, "");
            // 
            // tabFloor
            // 
            this.tabFloor.Controls.Add(this.gridFloor);
            this.tabFloor.Location = new System.Drawing.Point(4, 21);
            this.tabFloor.Name = "tabFloor";
            this.tabFloor.Size = new System.Drawing.Size(950, 571);
            this.tabFloor.TabIndex = 1;
            this.tabFloor.Text = "Floor";
            this.tabFloor.UseVisualStyleBackColor = true;
            // 
            // gridFloor
            // 
            this.gridFloor.DataMember = "";
            this.gridFloor.Dock = System.Windows.Forms.DockStyle.Fill;
            this.gridFloor.HeaderForeColor = System.Drawing.SystemColors.ControlText;
            this.gridFloor.Location = new System.Drawing.Point(0, 0);
            this.gridFloor.Name = "gridFloor";
            this.gridFloor.Size = new System.Drawing.Size(950, 571);
            this.gridFloor.TabIndex = 0;
            // 
            // tabPage3
            // 
            this.tabPage3.Controls.Add(this.gridReport1);
            this.tabPage3.Controls.Add(this.gridReport2);
            this.tabPage3.Location = new System.Drawing.Point(4, 21);
            this.tabPage3.Name = "tabPage3";
            this.tabPage3.Size = new System.Drawing.Size(950, 571);
            this.tabPage3.TabIndex = 4;
            this.tabPage3.Text = "result";
            this.tabPage3.UseVisualStyleBackColor = true;
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
            this.tabReport.Size = new System.Drawing.Size(950, 571);
            this.tabReport.TabIndex = 5;
            this.tabReport.Text = "Report";
            this.tabReport.UseVisualStyleBackColor = true;
            // 
            // crystalReportViewer1
            // 
            this.crystalReportViewer1.ActiveViewIndex = -1;
            this.crystalReportViewer1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.crystalReportViewer1.DisplayGroupTree = false;
            this.crystalReportViewer1.DisplayStatusBar = false;
            this.crystalReportViewer1.DisplayToolbar = false;
            this.crystalReportViewer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.crystalReportViewer1.Location = new System.Drawing.Point(0, 0);
            this.crystalReportViewer1.Name = "crystalReportViewer1";
            this.crystalReportViewer1.SelectionFormula = "";
            this.crystalReportViewer1.Size = new System.Drawing.Size(950, 571);
            this.crystalReportViewer1.TabIndex = 0;
            this.crystalReportViewer1.ViewTimeSelectionFormula = "";
            // 
            // tabTr
            // 
            this.tabTr.Controls.Add(this.gridTrajectory);
            this.tabTr.Location = new System.Drawing.Point(4, 21);
            this.tabTr.Name = "tabTr";
            this.tabTr.Size = new System.Drawing.Size(950, 571);
            this.tabTr.TabIndex = 7;
            this.tabTr.Text = "TableTr";
            this.tabTr.UseVisualStyleBackColor = true;
            // 
            // gridTrajectory
            // 
            this.gridTrajectory.DataMember = "";
            this.gridTrajectory.Dock = System.Windows.Forms.DockStyle.Fill;
            this.gridTrajectory.HeaderForeColor = System.Drawing.SystemColors.ControlText;
            this.gridTrajectory.Location = new System.Drawing.Point(0, 0);
            this.gridTrajectory.Name = "gridTrajectory";
            this.gridTrajectory.Size = new System.Drawing.Size(950, 571);
            this.gridTrajectory.TabIndex = 0;
            // 
            // tabCalc
            // 
            this.tabCalc.Controls.Add(this.gridCalc);
            this.tabCalc.Location = new System.Drawing.Point(4, 21);
            this.tabCalc.Name = "tabCalc";
            this.tabCalc.Size = new System.Drawing.Size(950, 571);
            this.tabCalc.TabIndex = 10;
            this.tabCalc.Text = "Calc";
            this.tabCalc.UseVisualStyleBackColor = true;
            // 
            // gridCalc
            // 
            this.gridCalc.DataMember = "";
            this.gridCalc.Dock = System.Windows.Forms.DockStyle.Fill;
            this.gridCalc.HeaderForeColor = System.Drawing.SystemColors.ControlText;
            this.gridCalc.Location = new System.Drawing.Point(0, 0);
            this.gridCalc.Name = "gridCalc";
            this.gridCalc.Size = new System.Drawing.Size(950, 571);
            this.gridCalc.TabIndex = 0;
            // 
            // tabHexec
            // 
            this.tabHexec.Controls.Add(this.btnClearHexec);
            this.tabHexec.Controls.Add(this.gridLowerPlan);
            this.tabHexec.Controls.Add(this.gridUpperPlan);
            this.tabHexec.Controls.Add(this.cbHwy);
            this.tabHexec.Controls.Add(this.gridHexecL);
            this.tabHexec.Controls.Add(this.label40);
            this.tabHexec.Controls.Add(this.gridHexecU);
            this.tabHexec.Location = new System.Drawing.Point(4, 21);
            this.tabHexec.Name = "tabHexec";
            this.tabHexec.Size = new System.Drawing.Size(950, 571);
            this.tabHexec.TabIndex = 12;
            this.tabHexec.Text = "HEXEC";
            this.tabHexec.UseVisualStyleBackColor = true;
            // 
            // btnClearHexec
            // 
            this.btnClearHexec.Location = new System.Drawing.Point(136, 24);
            this.btnClearHexec.Name = "btnClearHexec";
            this.btnClearHexec.Size = new System.Drawing.Size(56, 24);
            this.btnClearHexec.TabIndex = 7;
            this.btnClearHexec.Text = "Clear";
            this.btnClearHexec.Click += new System.EventHandler(this.btnClearHexec_Click);
            // 
            // gridLowerPlan
            // 
            this.gridLowerPlan.DataMember = "";
            this.gridLowerPlan.HeaderForeColor = System.Drawing.SystemColors.ControlText;
            this.gridLowerPlan.Location = new System.Drawing.Point(8, 280);
            this.gridLowerPlan.Name = "gridLowerPlan";
            this.gridLowerPlan.Size = new System.Drawing.Size(184, 240);
            this.gridLowerPlan.TabIndex = 6;
            // 
            // gridUpperPlan
            // 
            this.gridUpperPlan.DataMember = "";
            this.gridUpperPlan.HeaderForeColor = System.Drawing.SystemColors.ControlText;
            this.gridUpperPlan.Location = new System.Drawing.Point(8, 56);
            this.gridUpperPlan.Name = "gridUpperPlan";
            this.gridUpperPlan.Size = new System.Drawing.Size(184, 208);
            this.gridUpperPlan.TabIndex = 5;
            // 
            // cbHwy
            // 
            this.cbHwy.Items.AddRange(new object[] {
            "Hwy0",
            "Hwy1",
            "Hwy2",
            "Hwy3"});
            this.cbHwy.Location = new System.Drawing.Point(8, 24);
            this.cbHwy.Name = "cbHwy";
            this.cbHwy.Size = new System.Drawing.Size(112, 20);
            this.cbHwy.TabIndex = 4;
            this.cbHwy.Text = "Hwy0";
            this.cbHwy.SelectedIndexChanged += new System.EventHandler(this.cbHwy_SelectedIndexChanged);
            // 
            // gridHexecL
            // 
            this.gridHexecL.DataMember = "";
            this.gridHexecL.HeaderForeColor = System.Drawing.SystemColors.ControlText;
            this.gridHexecL.Location = new System.Drawing.Point(208, 280);
            this.gridHexecL.Name = "gridHexecL";
            this.gridHexecL.Size = new System.Drawing.Size(736, 240);
            this.gridHexecL.TabIndex = 3;
            // 
            // label40
            // 
            this.label40.Location = new System.Drawing.Point(16, 8);
            this.label40.Name = "label40";
            this.label40.Size = new System.Drawing.Size(72, 16);
            this.label40.TabIndex = 2;
            this.label40.Text = "Select Hwy";
            // 
            // gridHexecU
            // 
            this.gridHexecU.DataMember = "";
            this.gridHexecU.HeaderForeColor = System.Drawing.SystemColors.ControlText;
            this.gridHexecU.Location = new System.Drawing.Point(208, 16);
            this.gridHexecU.Name = "gridHexecU";
            this.gridHexecU.Size = new System.Drawing.Size(736, 248);
            this.gridHexecU.TabIndex = 1;
            // 
            // tabSimulation
            // 
            this.tabSimulation.Controls.Add(this.dataGridView1);
            this.tabSimulation.Location = new System.Drawing.Point(4, 21);
            this.tabSimulation.Name = "tabSimulation";
            this.tabSimulation.Padding = new System.Windows.Forms.Padding(3);
            this.tabSimulation.Size = new System.Drawing.Size(950, 571);
            this.tabSimulation.TabIndex = 13;
            this.tabSimulation.Text = "Simulation";
            this.tabSimulation.UseVisualStyleBackColor = true;
            // 
            // statusBarTSA
            // 
            this.statusBarTSA.Location = new System.Drawing.Point(0, 574);
            this.statusBarTSA.Name = "statusBarTSA";
            this.statusBarTSA.Panels.AddRange(new System.Windows.Forms.StatusBarPanel[] {
            this.statusTime,
            this.statusMinMargin,
            this.statusDelay,
            this.statusMode});
            this.statusBarTSA.ShowPanels = true;
            this.statusBarTSA.Size = new System.Drawing.Size(958, 22);
            this.statusBarTSA.TabIndex = 2;
            this.statusBarTSA.Text = "statusBar1";
            // 
            // statusTime
            // 
            this.statusTime.Name = "statusTime";
            this.statusTime.Text = "0:0:0";
            this.statusTime.Width = 80;
            // 
            // statusMinMargin
            // 
            this.statusMinMargin.Name = "statusMinMargin";
            this.statusMinMargin.Text = "Min Separation :";
            this.statusMinMargin.Width = 200;
            // 
            // statusDelay
            // 
            this.statusDelay.Name = "statusDelay";
            this.statusDelay.Text = "TSA Delay :";
            this.statusDelay.Width = 140;
            // 
            // statusMode
            // 
            this.statusMode.Name = "statusMode";
            this.statusMode.Text = "Mode :";
            // 
            // tmrSimul
            // 
            this.tmrSimul.Tick += new System.EventHandler(this.tmrSimul_Tick);
            // 
            // dataGridView1
            // 
            this.dataGridView1.AutoGenerateColumns = false;
            this.dataGridView1.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.dataGridView1.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.timeDataGridViewTextBoxColumn,
            this.posDataGridViewTextBoxColumn,
            this.dstDataGridViewTextBoxColumn,
            this.carDataGridViewTextBoxColumn,
            this.wtDataGridViewTextBoxColumn,
            this.stDataGridViewTextBoxColumn});
            this.dataGridView1.DataSource = this.trafficBindingSource;
            this.dataGridView1.Location = new System.Drawing.Point(8, 19);
            this.dataGridView1.Name = "dataGridView1";
            this.dataGridView1.RowHeadersWidth = 20;
            this.dataGridView1.RowTemplate.Height = 23;
            this.dataGridView1.Size = new System.Drawing.Size(212, 406);
            this.dataGridView1.TabIndex = 0;
            // 
            // dsConfig
            // 
            this.dsConfig.DataSetName = "dsConfig";
            this.dsConfig.SchemaSerializationMode = System.Data.SchemaSerializationMode.IncludeSchema;
            // 
            // trafficBindingSource
            // 
            this.trafficBindingSource.DataMember = "traffic";
            this.trafficBindingSource.DataSource = this.dsConfig;
            // 
            // timeDataGridViewTextBoxColumn
            // 
            this.timeDataGridViewTextBoxColumn.DataPropertyName = "time";
            this.timeDataGridViewTextBoxColumn.HeaderText = "time";
            this.timeDataGridViewTextBoxColumn.Name = "timeDataGridViewTextBoxColumn";
            this.timeDataGridViewTextBoxColumn.Width = 40;
            // 
            // posDataGridViewTextBoxColumn
            // 
            this.posDataGridViewTextBoxColumn.DataPropertyName = "pos";
            this.posDataGridViewTextBoxColumn.HeaderText = "pos";
            this.posDataGridViewTextBoxColumn.Name = "posDataGridViewTextBoxColumn";
            this.posDataGridViewTextBoxColumn.Width = 30;
            // 
            // dstDataGridViewTextBoxColumn
            // 
            this.dstDataGridViewTextBoxColumn.DataPropertyName = "dst";
            this.dstDataGridViewTextBoxColumn.HeaderText = "dst";
            this.dstDataGridViewTextBoxColumn.Name = "dstDataGridViewTextBoxColumn";
            this.dstDataGridViewTextBoxColumn.Width = 30;
            // 
            // carDataGridViewTextBoxColumn
            // 
            this.carDataGridViewTextBoxColumn.DataPropertyName = "car";
            this.carDataGridViewTextBoxColumn.HeaderText = "car";
            this.carDataGridViewTextBoxColumn.Name = "carDataGridViewTextBoxColumn";
            this.carDataGridViewTextBoxColumn.Width = 30;
            // 
            // wtDataGridViewTextBoxColumn
            // 
            this.wtDataGridViewTextBoxColumn.DataPropertyName = "w_t";
            this.wtDataGridViewTextBoxColumn.HeaderText = "w_t";
            this.wtDataGridViewTextBoxColumn.Name = "wtDataGridViewTextBoxColumn";
            this.wtDataGridViewTextBoxColumn.Width = 30;
            // 
            // stDataGridViewTextBoxColumn
            // 
            this.stDataGridViewTextBoxColumn.DataPropertyName = "s_t";
            this.stDataGridViewTextBoxColumn.HeaderText = "s_t";
            this.stDataGridViewTextBoxColumn.Name = "stDataGridViewTextBoxColumn";
            this.stDataGridViewTextBoxColumn.Width = 30;
            // 
            // fmMain
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(6, 14);
            this.ClientSize = new System.Drawing.Size(958, 596);
            this.Controls.Add(this.statusBarTSA);
            this.Controls.Add(this.tabDebug);
            this.Name = "fmMain";
            this.Text = "COGS_SPY";
            this.Closing += new System.ComponentModel.CancelEventHandler(this.fmMain_Closing);
            this.Load += new System.EventHandler(this.fmMain_Load);
            ((System.ComponentModel.ISupportInitialize)(this.gridDebug)).EndInit();
            this.tabDebug.ResumeLayout(false);
            this.tabParam.ResumeLayout(false);
            this.tabConfig.ResumeLayout(false);
            this.tabConfig.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.gridHoistWay)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.gridFloorHeight)).EndInit();
            this.tabTraject.ResumeLayout(false);
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            this.tabPage2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.grid2d_c)).EndInit();
            this.tabPage1.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.grid2d_p)).EndInit();
            this.tabSimul.ResumeLayout(false);
            this.tabSimul.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.gridTraffic)).EndInit();
            this.pnTrajectory.ResumeLayout(false);
            this.pnTrajectory.PerformLayout();
            this.tabFloor.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.gridFloor)).EndInit();
            this.tabPage3.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.gridReport1)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.gridReport2)).EndInit();
            this.tabReport.ResumeLayout(false);
            this.tabTr.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.gridTrajectory)).EndInit();
            this.tabCalc.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.gridCalc)).EndInit();
            this.tabHexec.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.gridLowerPlan)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.gridUpperPlan)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.gridHexecL)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.gridHexecU)).EndInit();
            this.tabSimulation.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.statusTime)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.statusMinMargin)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.statusDelay)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.statusMode)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.dataGridView1)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.dsConfig)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.trafficBindingSource)).EndInit();
            this.ResumeLayout(false);

		}
		#endregion

		/// <summary>
		/// 해당 응용 프로그램의 주 진입점입니다.
		/// </summary>
		[STAThread]
		static void Main() 
		{
			Application.Run(new fmMain());
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
		
		public void UpdatePlan(int hwy)
		{
			int planInx=0;
			int count=0;
			try
			{
				using (StreamReader sr=new StreamReader(@"c:\plan"+hwy.ToString()+".csv"))
				{
					while (sr.Peek() >= 0) 
					{
						string str=sr.ReadLine();
						if(str.IndexOf("UPPER")>=0)
						{
							planInx=hwy*2+1;
							count=0;
							dtPlan[planInx].Rows.Clear();
							continue;
						}
						else if(str.IndexOf("LOWER")>=0)
						{
							planInx=hwy*2;
							count=0;
							dtPlan[planInx].Rows.Clear();
							continue;
						}
						string[] strArr=str.Split(',');

						DataRow row=dtPlan[planInx].NewRow();
						row[0]=strArr[0]; //index
						row[1]=count.ToString(); //sequence index
						row[2]=strArr[1]; //floor
						row[3]=strArr[2]; //type
						row[4]=strArr[3]; //precedence
										  //precedence sequence index

						row[0]=strArr[0]; //time
						dtPlan[planInx].Rows.Add(row);
						count++;
					}
				}
			}
			catch(Exception eee)
			{
				MessageBox.Show(eee.Message);
			}
			
			int carId,otherCarId;
			int prec;
			int otherInx;

			for(int car=0;car<2;car++)
			{
				carId=(car==0) ? hwy*2 : hwy*2+1;
				otherCarId=(car==0) ? hwy*2+1 : hwy*2;
				for(int i=0;i<dtPlan[carId].Rows.Count;i++)
				{
					prec=int.Parse((string)dtPlan[carId].Rows[i][4]);
					if(prec!=-1)
					{
						for(int j=0;j<dtPlan[otherCarId].Rows.Count;j++)
						{
							otherInx=int.Parse((string)dtPlan[otherCarId].Rows[j][0]);
							if(otherInx==prec)
							{
								string str=(string)dtPlan[otherCarId].Rows[j][1];
								dtPlan[carId].Rows[i][5]=str;
							}	
						}    
					}
				}
			}
			if(tabDebug.SelectedTab.Text.Equals("HEXEC") && this.cbHwy.SelectedIndex==hwy)
			{
				if(this.gridUpperPlan.DataSource!=dtPlan[hwy*2+1])
				{
					gridUpperPlan.DataSource=dtPlan[hwy*2+1];
				}
				if(this.gridLowerPlan.DataSource!=dtPlan[hwy*2])
				{
					gridLowerPlan.DataSource=dtPlan[hwy*2];
				}
			}
			else
				return;
			gridUpperPlan.Select(0);
			gridLowerPlan.Select(0);
		}

		public void UpdateHexec(int car)
		{
			int curInx=0;
			try
			{
				using (StreamReader sr=new StreamReader(@"c:\carState"+car.ToString()+".csv"))
				{
					while (sr.Peek() >= 0) 
					{
						string str=sr.ReadLine();
						if(str.IndexOf("curInx")>=0)
							continue;
//						fprintf(fpos,"curInx,time,state,actual_pos,target_pos,NCF,doorState,motionState,mov_dir,decel,cmd_target,cmd_door,cmd_dwell,otherCarControl\n");

						string[] strArr=str.Split(',');
						int lastInx=dtHexec[car].Rows.Count-1;
						if(lastInx>=0)
						{
							double lastTime=double.Parse((string)this.dtHexec[car].Rows[lastInx][0]);
							double curTime=double.Parse(strArr[1]);
							//if((curTime-lastTime)<1 && (curTime-lastTime)>=0)
							//	continue;
							if((curTime-lastTime)<0)
							{
								lastInx=-1;
								dtHexec[car].Rows.Clear();
							}
						}
						DataRow row=dtHexec[car].NewRow();
						curInx=Int32.Parse(strArr[0]);
						for(int i=1;i<strArr.Length;i++)
						{
							try
							{
								row[i-1]=strArr[i];
							}
							catch
							{
								row[i-1]="0";
							}
						}
						//debugDs.pos.Rows.Add(row);
						dtHexec[car].Rows.Add(row);

						if(dtHexec[car].Rows.Count>100)
							dtHexec[car].Rows.RemoveAt(0);
					}
				}
			}
			catch(Exception eee)
			{
			}
			int hwy=car/2;
			int count=0;
			if(tabDebug.SelectedTab.Text.Equals("HEXEC") && this.cbHwy.SelectedIndex==hwy)
			{
				try
				{
					if(car%2==1)
					{
						for(int i=0;i<dtPlan[car].Rows.Count;i++)
						{
							int inx=Int32.Parse((string)dtPlan[car].Rows[i][0]);
							if(inx==curInx)
							{
								this.gridUpperPlan.Select(count);
								break;
							}
							this.gridUpperPlan.Select(count);
							count++;
						}
						if(this.gridHexecU.DataSource!=dtHexec[car])
						{
							gridHexecU.DataSource=dtHexec[car];
						}
					}
					else
					{
						for(int i=0;i<dtPlan[car].Rows.Count;i++)
						{
							int inx=Int32.Parse((string)dtPlan[car].Rows[i][0]);
							if(inx==curInx)
							{
								this.gridLowerPlan.Select(count);
								break;
							}
							this.gridLowerPlan.Select(count);
							count++;
						}
						if(this.gridHexecL.DataSource!=dtHexec[car])
						{
							gridHexecL.DataSource=dtHexec[car];
						}
					}
				}
				catch
				{}
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
								else if(double.Parse(strArr[i])>=double.Parse(this.txtMaxY.Text))
									row[i]=this.txtMaxY.Text;
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

			try
			{
				using (StreamReader sr=new StreamReader(@"c:\dNCF.csv"))
				{
					while (sr.Peek() >= 0) 
					{
						string str=sr.ReadLine();
						string[] strArr=str.Split(',');
						int lastInx=debugDs.ncf.Rows.Count-1;
						if(lastInx>=0)
						{
							double lastTime=double.Parse((string)debugDs.ncf.Rows[lastInx][0]);
							double curTime=double.Parse(strArr[0]);
							if((curTime-lastTime)<1 && (curTime-lastTime)>=0)
								continue;
							if((curTime-lastTime)<0)
							{
								lastInx=-1;
								debugDs.ncf.Rows.Clear();
							}
						}
						DataRow row=debugDs.ncf.NewRow();
						row[0]=strArr[0]; //time
						for(int i=1;i<MAX_OCSS;i++)
						{
							try
							{
								row[i]=strArr[i];
							}
							catch
							{
								row[i]="0";
							}
						}
						debugDs.ncf.Rows.Add(row);
						if(debugDs.ncf.Rows.Count>100)
							debugDs.ncf.Rows.RemoveAt(0);
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
				case 0x402: //update Plan
					int hwy=msg.WParam.ToInt32();
					UpdatePlan(hwy);
					break;
				case 0x403: //update car state in hexec
					int car=msg.WParam.ToInt32();
					UpdateHexec(car);
					break;
				case 0x404:
					if(tabDebug.SelectedTab.Text.Equals("Floor"))
					{
						ShowFloor();
					}
					break;

			}
			base.DefWndProc(ref msg);
		}

		private void fmMain_Load(object sender, System.EventArgs e)
		{
			dtPosTSA.Columns.Add(new DataColumn("time",System.Type.GetType("System.String"))); 
			dtPosTSA.Columns.Add(new DataColumn("CAR0",System.Type.GetType("System.String"))); 
			dtPosTSA.Columns.Add(new DataColumn("CAR1",System.Type.GetType("System.String"))); 
			dtPosTSA.Columns.Add(new DataColumn("CAR2",System.Type.GetType("System.String"))); 
			dtPosTSA.Columns.Add(new DataColumn("CAR3",System.Type.GetType("System.String"))); 
			dtPosTSA.Columns.Add(new DataColumn("CAR4",System.Type.GetType("System.String"))); 
			dtPosTSA.Columns.Add(new DataColumn("CAR5",System.Type.GetType("System.String"))); 
			dtPosTSA.Columns.Add(new DataColumn("CAR6",System.Type.GetType("System.String"))); 
			dtPosTSA.Columns.Add(new DataColumn("CAR7",System.Type.GetType("System.String"))); 

			dtPlan=new DataTable[MAX_OCSS];
			dtHexec=new DataTable[MAX_OCSS];
			for(int i=0;i<MAX_OCSS;i++)
			{
				dtPlan[i]=new DataTable("Plan"+i.ToString());
				dtPlan[i].Columns.Add(new DataColumn("planInx",System.Type.GetType("System.String"))); 
				dtPlan[i].Columns.Add(new DataColumn("inx",System.Type.GetType("System.String"))); 
				dtPlan[i].Columns.Add(new DataColumn("floor",System.Type.GetType("System.String"))); 
				dtPlan[i].Columns.Add(new DataColumn("type",System.Type.GetType("System.String"))); 
				dtPlan[i].Columns.Add(new DataColumn("planPrec",System.Type.GetType("System.String")));
				dtPlan[i].Columns.Add(new DataColumn("prec",System.Type.GetType("System.String")));

				dtHexec[i]=new DataTable("Hexec"+i.ToString());
				dtHexec[i].Columns.Add(new DataColumn("time",System.Type.GetType("System.String"))); 
				dtHexec[i].Columns.Add(new DataColumn("state",System.Type.GetType("System.String"))); 
				dtHexec[i].Columns.Add(new DataColumn("pos",System.Type.GetType("System.String"))); 
				dtHexec[i].Columns.Add(new DataColumn("tgt",System.Type.GetType("System.String"))); 
				dtHexec[i].Columns.Add(new DataColumn("NCF",System.Type.GetType("System.String"))); 
				dtHexec[i].Columns.Add(new DataColumn("Door",System.Type.GetType("System.String"))); 
				dtHexec[i].Columns.Add(new DataColumn("Motion",System.Type.GetType("System.String"))); 
				dtHexec[i].Columns.Add(new DataColumn("mov_dir",System.Type.GetType("System.String"))); 
				dtHexec[i].Columns.Add(new DataColumn("decel",System.Type.GetType("System.String"))); 
				dtHexec[i].Columns.Add(new DataColumn("cmd_target",System.Type.GetType("System.String"))); 
				dtHexec[i].Columns.Add(new DataColumn("cmd_door",System.Type.GetType("System.String"))); 
				dtHexec[i].Columns.Add(new DataColumn("cmd_dir",System.Type.GetType("System.String"))); 
				dtHexec[i].Columns.Add(new DataColumn("otherCarControl",System.Type.GetType("System.String"))); 

			}
			DataGridTableStyle ts;
			for(int i=0;i<MAX_OCSS/2;i++)
			{
				this.gridUpperPlan.DataSource=dtPlan[i*2+1];
				this.gridLowerPlan.DataSource=dtPlan[i*2];
				ts = new DataGridTableStyle();
				// Set the DataGridTableStyle.MappingName property
				// to the table in the data source to map to.
				ts.MappingName = this.gridUpperPlan.DataSource.ToString();
				// Add it to the datagrid's TableStyles collection
				gridUpperPlan.TableStyles.Add(ts);

				// Hide the first column (index 0)
				gridUpperPlan.TableStyles[i].GridColumnStyles[0].Width = 0;
				gridUpperPlan.TableStyles[i].GridColumnStyles[1].Width = 30;
				gridUpperPlan.TableStyles[i].GridColumnStyles[2].Width = 40;
				gridUpperPlan.TableStyles[i].GridColumnStyles[3].Width = 80;
				gridUpperPlan.TableStyles[i].GridColumnStyles[4].Width = 0;
				gridUpperPlan.TableStyles[i].GridColumnStyles[5].Width = 30;
				gridUpperPlan.TableStyles[i].RowHeaderWidth=0;
				gridUpperPlan.TableStyles[i].RowHeadersVisible=false;

				ts = new DataGridTableStyle();
				// Set the DataGridTableStyle.MappingName property
				// to the table in the data source to map to.
				ts.MappingName = this.gridLowerPlan.DataSource.ToString();
				// Add it to the datagrid's TableStyles collection
				gridLowerPlan.TableStyles.Add(ts);

				// Hide the first column (index 0)
				gridLowerPlan.TableStyles[i].GridColumnStyles[0].Width = 0;
				gridLowerPlan.TableStyles[i].GridColumnStyles[1].Width = 30;
				gridLowerPlan.TableStyles[i].GridColumnStyles[2].Width = 40;
				gridLowerPlan.TableStyles[i].GridColumnStyles[3].Width = 80;
				gridLowerPlan.TableStyles[i].GridColumnStyles[4].Width = 0;
				gridLowerPlan.TableStyles[i].GridColumnStyles[5].Width = 30;
				gridLowerPlan.TableStyles[i].RowHeaderWidth=0;
				gridLowerPlan.TableStyles[i].RowHeadersVisible=false;

				this.gridHexecL.DataSource=dtHexec[i*2];
				this.gridHexecU.DataSource=dtHexec[i*2+1];
				ts = new DataGridTableStyle();
				// Set the DataGridTableStyle.MappingName property
				// to the table in the data source to map to.
				ts.MappingName = this.gridHexecL.DataSource.ToString();
				// Add it to the datagrid's TableStyles collection
				gridHexecL.TableStyles.Add(ts);


				// Hide the first column (index 0)
				gridHexecL.TableStyles[i].GridColumnStyles[0].Width = 40;
				gridHexecL.TableStyles[i].GridColumnStyles[1].Width = 80;
				gridHexecL.TableStyles[i].GridColumnStyles[2].Width = 30;
				gridHexecL.TableStyles[i].GridColumnStyles[3].Width = 30;
				gridHexecL.TableStyles[i].GridColumnStyles[4].Width = 40;
				gridHexecL.TableStyles[i].GridColumnStyles[5].Width = 40;
				gridHexecL.TableStyles[i].GridColumnStyles[6].Width = 50;
				gridHexecL.TableStyles[i].GridColumnStyles[7].Width = 50;
				gridHexecL.TableStyles[i].GridColumnStyles[8].Width = 40;
				gridHexecL.TableStyles[i].GridColumnStyles[9].Width = 70;
				gridHexecL.TableStyles[i].GridColumnStyles[10].Width = 70;
				gridHexecL.TableStyles[i].GridColumnStyles[11].Width = 60;
				gridHexecL.TableStyles[i].GridColumnStyles[12].Width = 100;

				gridHexecL.TableStyles[i].RowHeaderWidth=0;
				gridHexecL.TableStyles[i].RowHeadersVisible=false;

				ts = new DataGridTableStyle();
				// Set the DataGridTableStyle.MappingName property
				// to the table in the data source to map to.
				ts.MappingName = this.gridHexecU.DataSource.ToString();
				// Add it to the datagrid's TableStyles collection
				gridHexecU.TableStyles.Add(ts);
				gridHexecU.TableStyles[i].GridColumnStyles[0].Width = 40;
				gridHexecU.TableStyles[i].GridColumnStyles[1].Width = 80;
				gridHexecU.TableStyles[i].GridColumnStyles[2].Width = 30;
				gridHexecU.TableStyles[i].GridColumnStyles[3].Width = 30;
				gridHexecU.TableStyles[i].GridColumnStyles[4].Width = 40;
				gridHexecU.TableStyles[i].GridColumnStyles[5].Width = 40;
				gridHexecU.TableStyles[i].GridColumnStyles[6].Width = 50;
				gridHexecU.TableStyles[i].GridColumnStyles[7].Width = 50;
				gridHexecU.TableStyles[i].GridColumnStyles[8].Width = 40;
				gridHexecU.TableStyles[i].GridColumnStyles[9].Width = 70;
				gridHexecU.TableStyles[i].GridColumnStyles[10].Width = 70;
				gridHexecU.TableStyles[i].GridColumnStyles[11].Width = 60;
				gridHexecU.TableStyles[i].GridColumnStyles[12].Width = 100;

				gridHexecU.TableStyles[i].RowHeaderWidth=0;
				gridHexecU.TableStyles[i].RowHeadersVisible=false;
				
			}

			this.CurrentTimeRange=100;
			debugDs=new dsDebug();
			reportDs=new dsReport();
			configDs=new dsConfig();
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
			this.gridHoistWay.DataSource=configDs.HoistWay;
			this.gridCalc.DataSource=debugDs.Calc;
			this.grid2d_p.DataSource=reportDs._2d_p;
			this.grid2d_c.DataSource=reportDs._2d_c;
			this.gridReport1.DataSource=reportDs.report_item1;
			this.gridReport2.DataSource=reportDs.report_item2;
			this.gridDebug.DataSource=debugDs.values;
			this.gridFloor.DataSource=debugDs.floor;
			this.gridTrajectory.DataSource=debugDs.pos;

			this.timer1.Enabled=true;
			using (StreamWriter sw = new StreamWriter(@"c:\handle.txt")) 
			{
				// Add some text to the file.
				sw.Write(this.Handle.ToString());
			}

			ts = new DataGridTableStyle();
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
			if(configDs.CarInfo.Rows.Count<1)
			{
				configDs.CarInfo.Rows.Add(new object[] {4000,1000,1000,2500,2000,100,16,1,2000,1600,"300","3000"});
			}
			this.txtVel.DataBindings.Clear();
			this.txtAcc.DataBindings.Clear();
			this.txtJerk.DataBindings.Clear();
			this.txtAfd.DataBindings.Clear();
			this.txtDOTime.DataBindings.Clear();
			this.txtDCTime.DataBindings.Clear();
			this.txtBoardingRatio.DataBindings.Clear();
			this.txtCapacity.DataBindings.Clear();
			this.txtDoorWidth.DataBindings.Clear();
			this.txtLobby1.DataBindings.Clear();
			this.txtLobby2.DataBindings.Clear();
			this.txtLobby3.DataBindings.Clear();
			this.txtStartDelay.DataBindings.Clear();
			this.txtStopDelay.DataBindings.Clear();

			this.txtVel.DataBindings.Add(new Binding("Text",configDs.CarInfo.Rows[0],"maxVelocity"));
			this.txtAcc.DataBindings.Add(new Binding("Text",configDs.CarInfo.Rows[0],"maxAcceleration"));
			this.txtJerk.DataBindings.Add(new Binding("Text",configDs.CarInfo.Rows[0],"maxJerk"));
			this.txtAfd.DataBindings.Add(new Binding("Text",configDs.CarInfo.Rows[0],"Afd"));
			this.txtDOTime.DataBindings.Add(new Binding("Text",configDs.CarInfo.Rows[0],"tDoorOpening"));
			this.txtDCTime.DataBindings.Add(new Binding("Text",configDs.CarInfo.Rows[0],"tDoorClosing"));
			this.txtBoardingRatio.DataBindings.Add(new Binding("Text",configDs.CarInfo.Rows[0],"boardingRatio"));
			this.txtCapacity.DataBindings.Add(new Binding("Text",configDs.CarInfo.Rows[0],"capacity"));
			this.txtNDoors.DataBindings.Add(new Binding("Text",configDs.CarInfo.Rows[0],"nDoors"));
			this.txtDoorWidth.DataBindings.Add(new Binding("Text",configDs.CarInfo.Rows[0],"doorWidth"));
			this.txtStartDelay.DataBindings.Add(new Binding("Text",configDs.CarInfo.Rows[0],"tStartDelay"));
			this.txtStopDelay.DataBindings.Add(new Binding("Text",configDs.CarInfo.Rows[0],"tStopDelay"));

			if(configDs.Parameter.Rows.Count<1)
			{
				configDs.Parameter.Rows.Add(new object[] {5000,7000,10000,11000,1000,true,1,255,255});
			}
			this.txtCarDwell.DataBindings.Clear();
			this.txtHallMaxDwell.DataBindings.Clear();
			this.txtHallMinDwell.DataBindings.Clear();
			this.txtLobbyDwell.DataBindings.Clear();
			this.chkReopen.DataBindings.Clear();
			this.txtLobby1.DataBindings.Clear();
			this.txtLobby2.DataBindings.Clear();
			this.txtLobby3.DataBindings.Clear();

			this.txtCarDwell.DataBindings.Add(new Binding("Text",configDs.Parameter.Rows[0],"tCarMinDwell"));
			this.txtHallMaxDwell.DataBindings.Add(new Binding("Text",configDs.Parameter.Rows[0],"tHallMaxDwell"));
			this.txtHallMinDwell.DataBindings.Add(new Binding("Text",configDs.Parameter.Rows[0],"tHallMinDwell"));
			this.txtLobbyDwell.DataBindings.Add(new Binding("Text",configDs.Parameter.Rows[0],"tLobbyDwell"));
			this.chkReopen.DataBindings.Add(new Binding("Checked",configDs.Parameter.Rows[0],"bReopenEnabled"));
			this.txtLobby1.DataBindings.Add(new Binding("Text",configDs.Parameter.Rows[0],"Lobby1Floor"));
			this.txtLobby2.DataBindings.Add(new Binding("Text",configDs.Parameter.Rows[0],"Lobby2Floor"));
			this.txtLobby3.DataBindings.Add(new Binding("Text",configDs.Parameter.Rows[0],"Lobby3Floor"));
			
				/*

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
			*/
			if(configDs.FloorHeight.Rows.Count<1)
			{
				for(int i=0;i<25;i++)
				{
					int FloorHeight=4000;
					int FloorPos=4000*i;
					configDs.FloorHeight.Rows.Add(new object[] {i.ToString(),FloorHeight,FloorPos});
				}
			}
			int nCount=configDs.FloorHeight.Rows.Count;
			int nBuildingHeight=(int)configDs.FloorHeight.Rows[nCount-1][2]+(int)configDs.FloorHeight.Rows[nCount-1][1];

			this.txtMaxY.Text=nBuildingHeight.ToString();

			if(configDs.HoistWay.Rows.Count<1)
			{
				for(int i=0;i<4;i++)
				{
					configDs.HoistWay.Rows.Add(new object[] {i*2,i*2+1});
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
			dll=new csDLL(this);
			bFormLoaded=true;
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
			else
			{
				for(int i=0;i<ocssList.Count;i++)
				{
					csOcss ocss_info=(csOcss)ocssList[i];
					for(int j=0;j<ocss_info.floorList.Count;j++)
					{
						floor=(csFloor)ocss_info.floorList[j];
						if(picHoist.Equals(floor.picHoist))
						{
							if(floor.dnCarCall==0)
							{
								ocss_info.assigned_calls[j]|=S_FCC;
								floor.dnCarCall=1;
								floor.picHoist.Image=this.imageList1.Images[(int)floor.carCall+(int)IMG.NO_CARCALL];
							}
							else
							{
								ocss_info.assigned_calls[j] &=~S_FCC;
								floor.dnCarCall=0;
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
			if(tabDebug.SelectedTab.Text.Equals("Config"))
			{
				if(bFirstConfig.Equals(true))
				{
					bTSAConfigChanged=false;
					bFirstConfig=false;
				}
			}
			if(tabDebug.SelectedTab.Text.Equals("HEXEC"))
			{
				int hwy=cbHwy.SelectedIndex;
				UpdatePlan(hwy);
				int car=cbHwy.SelectedIndex*2;
				UpdateHexec(car);
				car=cbHwy.SelectedIndex*2+1;
				UpdateHexec(car);
			}
			if(tabDebug.SelectedTab.Text.Equals("Trajectory"))
			{
				int nCount=configDs.FloorHeight.Rows.Count;
				int nBuildingHeight=(int)configDs.FloorHeight.Rows[nCount-1][2]+(int)configDs.FloorHeight.Rows[nCount-1][1];
				this.txtMaxY.Text=nBuildingHeight.ToString();
			}
			else if(tabDebug.SelectedTab.Text.Equals("TSA_DELAY"))
			{	
				if(bTSAConfigChanged.Equals(false) && bFirstTSA.Equals(false))
					return;
				dll.updateConfig();
				dtPosTSA.Rows.Clear();								
				bTSAConfigChanged=false;
				if(this.bFirstTSA.Equals(true))
				{
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

					gridTraffic.DataSource=configDs.TSADelay;
					ts = new DataGridTableStyle();
					// Set the DataGridTableStyle.MappingName property
					// to the table in the data source to map to.
					ts.MappingName = this.gridTraffic.DataSource.ToString();
					// Add it to the datagrid's TableStyles collection
					gridTraffic.TableStyles.Add(ts);

					// Hide the first column (index 0)
					gridTraffic.TableStyles[1].GridColumnStyles[0].Width = 55;
					gridTraffic.TableStyles[1].GridColumnStyles[1].Width = 55;
					gridTraffic.TableStyles[1].GridColumnStyles[2].Width = 55;
					gridTraffic.TableStyles[1].GridColumnStyles[3].Width = 55;
					gridTraffic.TableStyles[1].RowHeaderWidth=10;
					gridTraffic.TableStyles[1].RowHeadersVisible=false;

					gridTraffic.DataSource=configDs.traffic;
				}
				maxFloor=(int)configDs.FloorHeight.Rows.Count;
				if(this.bFirstTSA.Equals(false))
				{
					for(int i=0;i<2;i++)
					{
						csOcss ocss_info=(csOcss)ocssList[i];
						this.tabSimul.Controls.Remove(ocss_info.picCar);
						this.tabSimul.Controls.Remove(ocss_info.picDoor);
						this.tabSimul.Controls.Remove(ocss_info.lbOcss);
						for(int j=0;j<ocss_info.floorList.Count;j++)
						{
							csFloor fl=(csFloor)ocss_info.floorList[j];
							this.tabSimul.Controls.Remove(fl.picHall);
							this.tabSimul.Controls.Remove(fl.picHoist);
						}
					}
				}
				ocssList.Clear();

				for(int i=0;i<2;i++)
				{
					csOcss ocss_info=new csOcss();
					ocss_info.actual_pos=i%2;
					ocss_info.bUpper=(i%2==0) ? false : true;
					PictureBox picDoor=new PictureBox();
					this.tabSimul.Controls.Add(picDoor);
					ocss_info.picDoor=picDoor;

					PictureBox picCar=new PictureBox();
					picCar.Tag=i;
					this.tabSimul.Controls.Add(picCar);
					ocss_info.picCar=picCar;
					ocss_info.imageList1=this.imageList1;
					//Set Default motion value
					ocss_info.vel=(int)(ushort)this.configDs.CarInfo.Rows[0]["maxVelocity"];
					ocss_info.acc=(int)(ushort)this.configDs.CarInfo.Rows[0]["maxAcceleration"];
					ocss_info.jerk=(int)(ushort)this.configDs.CarInfo.Rows[0]["maxJerk"];
					ocssList.Add(ocss_info);
				}

				for(int i=0;i<2;i++)
				{
					csOcss ocss_info=(csOcss)ocssList[i];
					ocss_info.otherCar=(i%2==0) ? (csOcss)ocssList[i*2+1] : (csOcss)ocssList[(i-1)*2];
					ocss_info.bottom_pos=0;
					ocss_info.top_pos=(int)configDs.FloorHeight.Rows.Count-1;
					PictureBox picDoor=(PictureBox)ocss_info.picDoor;
					picDoor.BackColor=Color.White;
					picDoor.Visible=false;
					PictureBox picCar=(PictureBox)ocss_info.picCar;
					picCar.BackColor=Color.Khaki;
					picCar.Image=this.imageList1.Images[(int)IMG.CAR];
					picCar.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
					picCar.MouseUp += new System.Windows.Forms.MouseEventHandler(this.picCar_MouseUp);
					picCar.MouseMove += new System.Windows.Forms.MouseEventHandler(this.picCar_MouseMove);
					picCar.MouseDown += new System.Windows.Forms.MouseEventHandler(this.picCar_MouseDown);
				
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
					ocss_info.ocss_init();
				}
				RefreshTSA();
				if(bFirstTSA)
					bFirstTSA=false;
			}
		}

		public void DrawGrpFrameTSA()
		{
			int i;
			Graphics dc=this.pnTrajectory.CreateGraphics();
			/*
			Pen BluePen=new Pen(Color.Blue,3);
			dc.DrawLine(BluePen,20,70,260,70);
			Pen GreenPen=new Pen(Color.Green,4);
			dc.DrawLine(GreenPen,20,120,260,120);
			Pen RedPen=new Pen(Color.Red,4);
			dc.DrawLine(RedPen,20,170,260,170);
*/
			Brush br=new SolidBrush(Color.White);
			dc.FillRectangle(br,0,0,this.pnTrajectory.Width,this.pnTrajectory.Height);
			XX1 = 0;
			YY1 = this.tbSimul.Height;
			XX2 = this.pnTrajectory.Width-1;
			YY2 = this.pnTrajectory.Height -this.tbSimul.Height- 2;
			GX1 = XX1 + GRP_BORDER*3;
			GY1 = YY1 + GRP_BORDER;
			GX2 = XX2 - GRP_BORDER;
			GY2 = this.pnTrajectory.Height-(pnTrajectory.Top+pnTrajectory.Height-gridTraffic.Top-gridTraffic.Height);

			DX = GX2 - GX1;
			DY = GY2 - GY1;
			Pen pen=new Pen(Color.Black,1);
			dc.DrawRectangle(pen,XX1, YY1, XX2, YY2);
			pen.Color=Color.FromArgb(0xc0,0xc0,0xc0);
			dc.DrawRectangle(pen,GX1, GY1, DX, DY);
			
			int nLanding=configDs.FloorHeight.Rows.Count;
			int bldHeight=(int)configDs.FloorHeight.Rows[nLanding-1][1]+(int)configDs.FloorHeight.Rows[nLanding-1][2];
			for (i=0; i<configDs.FloorHeight.Rows.Count; i++)
			{
				int y=(int)configDs.FloorHeight.Rows[i][2];
				int y1=(int)((double)DY*((double)y/(double)bldHeight));
				dc.DrawLine(pen,GX1, GY2-y1,GX2, GY2-y1);
				// Draw string to screen.
				dc.DrawString((string)configDs.FloorHeight.Rows[i][0], drawFont, drawBrush, 2, GY2-y1-20);
			}
			for (i=0; i<10; i++)
			{
				dc.DrawLine(pen,GX1 + DX * i /10, GY1,GX1 + DX * i /10, GY2);
			}
			pen.Color=Color.FromArgb(0,0,0);
			dc.DrawLine(pen,GX1,GY1,GX1,GY2);
			dc.DrawLine(pen,GX1,GY2,GX2,GY2);
			dc.DrawString(TSATimeRange.ToString()+"sec",drawFont,drawBrush,GX2-30,GY2-5);


			//			DisplayStart = (DisplayAllRange)? 0 : (CurrentTimeRange - DisplayRange);
			//			if (DisplayStart<0) DisplayStart = 0;
			br=new SolidBrush(Color.CadetBlue);
			Font DispFont=new Font("Arial",10,FontStyle.Bold);
			//g.DrawString("Arial Bold",DispFont_2,br,10,150);
			LastDispInx=0;
			DisplayUpdate_TSA();

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
			GX1 = XX1 + GRP_BORDER*3;
			GY1 = YY1 + GRP_BORDER;
			GX2 = XX2 - GRP_BORDER;
			GY2 = YY2 - GRP_BORDER;

			DX = GX2 - GX1;
			DY = GY2 - GY1;
			Pen pen=new Pen(Color.Black,1);
			dc.DrawRectangle(pen,XX1, YY1, XX2, YY2);
			pen.Color=Color.FromArgb(0xc0,0xc0,0xc0);
			dc.DrawRectangle(pen,GX1, GY1, DX, DY);
			for (i=0; i<configDs.FloorHeight.Rows.Count; i++)
			{
				int y=(int)configDs.FloorHeight.Rows[i][2];
				int y1=(int)((double)DY*((double)y/double.Parse(this.txtMaxY.Text)));
				dc.DrawLine(pen,GX1, GY2-y1,GX2, GY2-y1);
				// Draw string to screen.
				dc.DrawString((string)configDs.FloorHeight.Rows[i][0], drawFont, drawBrush, 2, GY2-y1-20);
			}
			for (i=0; i<10; i++)
			{
				dc.DrawLine(pen,GX1 + DX * i /10, GY1,GX1 + DX * i /10, GY2);
			}
			pen.Color=Color.FromArgb(0,0,0);
			dc.DrawLine(pen,GX1,GY1,GX1,GY2);
			dc.DrawLine(pen,GX1,GY2,GX2,GY2);

			int lastInx=this.debugDs.pos.Rows.Count-1;
			if(lastInx>1)
			{
				double lastTime=double.Parse((string)this.debugDs.pos.Rows[lastInx][0]);
				dc.DrawString(lastTime.ToString()+"sec",drawFont,drawBrush,GX2-70,GY2+5);
			}

			//			DisplayStart = (DisplayAllRange)? 0 : (CurrentTimeRange - DisplayRange);
			//			if (DisplayStart<0) DisplayStart = 0;
			br=new SolidBrush(Color.CadetBlue);
			Font DispFont=new Font("Arial",10,FontStyle.Bold);
			//g.DrawString("Arial Bold",DispFont_2,br,10,150);
			LastDispInx=0;
			try
			{
				DisplayUpdate();
			}
			catch(Exception e)
			{
				MessageBox.Show(e.Message);
			}
		}

		public void DisplayUpdate_TSA()
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
			int nLandings=configDs.FloorHeight.Rows.Count;
			int maxHeight=(int)configDs.FloorHeight.Rows[nLandings-1][2]+(int)configDs.FloorHeight.Rows[nLandings-1][1];
			int minSep=maxHeight;
			int minSepY1=minSep;
			int minSepY0=0;

			Graphics dc=this.pnTrajectory.CreateGraphics();
			int i;
			if(dtPosTSA.Rows.Count<=1)
				return;
			int lastInx=dtPosTSA.Rows.Count-1;
			double firstTime=double.Parse((string)dtPosTSA.Rows[0][0]);
			minSepX=firstTime;
			for(i=0;i<dtPosTSA.Rows.Count-1;i++)
			{
				time=(string)dtPosTSA.Rows[i][0]; 
				dTime=double.Parse(time);
				x1=GX1+(int)(DX*((dTime-firstTime)/TSATimeRange));
				time=(string)dtPosTSA.Rows[i+1][0]; // 현재 X좌표를 읽어옴
				dTime=double.Parse(time);
				x2=GX1+(int)(DX*((dTime-firstTime)/TSATimeRange));
				if(x2>GX2)
					return;
				for(int j=0;j<4;j++)
				{
					y1=(int)((double)DY*(double.Parse((string)dtPosTSA.Rows[i][j+1])/(double)maxHeight));
					y2=(int)((double)DY*(double.Parse((string)dtPosTSA.Rows[i+1][j+1])/(double)maxHeight));
					dc.DrawLine(penArr[j],x1,GY2-y1,x2,GY2-y2);
				}	
				minSepY[1]=(int)double.Parse((string)dtPosTSA.Rows[i][4]);
				minSepY[0]=(int)double.Parse((string)dtPosTSA.Rows[i][3]);
				if((minSepY[1]-minSepY[0])<minSep)
				{
					minSep=(minSepY[1]-minSepY[0]);
					minSepY1=minSepY[1];
					minSepY0=minSepY[0];
					time=(string)dtPosTSA.Rows[i][0]; 
					minSepX=double.Parse(time);
				}
			}
			if(dtPosTSA.Rows.Count>1)
			{
				y1=(int)((double)DY*(double)minSepY1/(double)maxHeight);
				y2=(int)((double)DY*(double)minSepY0/(double)maxHeight);
				x1=GX1+(int)(DX*((minSepX-firstTime)/TSATimeRange));
				dc.DrawLine(penArr[2],x1,GY2-y1,x1,GY2-y2);
				string strTime=minSepX.ToString()+" sec";
				string strMin="Min Separation :"+minSep.ToString()+" mm";
				if(!strTime.Equals(this.statusBarTSA.Panels[0].Text))
					this.statusBarTSA.Panels[0].Text=strTime;
				if(!strMin.Equals(this.statusBarTSA.Panels[1].Text))
					this.statusBarTSA.Panels[1].Text=strMin;
			}
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
			int minSep=int.Parse(this.txtMaxY.Text);
			int[] minSepY=new int[2];
			int minSepY1=minSep;
			int minSepY0=0;
			double minSepX;
			bool bMinSep=false;
			int minSepInx;
			int nChkCnt=0;
			int state1,state2;
			Graphics dc=this.tabTraject.CreateGraphics();
			int i;
			if(this.debugDs.pos.Rows.Count<=1)
				return;
			
			for(i=0;i<MAX_OCSS-1;i++)
			{
				if(!this.checkArr[i].Checked)
				{
					nChkCnt++;
				}
			}	
			if(nChkCnt==2)
				bMinSep=true;

			int lastInx=this.debugDs.pos.Rows.Count-1;
			double lastTime=double.Parse((string)this.debugDs.pos.Rows[lastInx][0]);
			minSepX=lastTime;
			for(i=this.debugDs.pos.Rows.Count-1;i>0;i--)
			{
				if(true)  //X축이 시간이면
				{
					time=(string)debugDs.pos.Rows[i][0]; 
					dTime=double.Parse(time);
					x1=GX2-(int)(DX*((lastTime-dTime)/CurrentTimeRange));
					time=(string)debugDs.pos.Rows[i-1][0]; // 현재 X좌표를 읽어옴
					dTime=double.Parse(time);
					x2=GX2-(int)(DX*((lastTime-dTime)/CurrentTimeRange));
					if(x1<GX1)
						break;
				}
				penInx=0;
				minSepInx=0;
				for(int j=0;j<MAX_OCSS-1;j++)
				{
					if(!this.checkArr[j].Checked)
					{
						try
						{
							y1=(int)((double)DY*(double.Parse((string)debugDs.pos.Rows[i][j+1])*1000/double.Parse(this.txtMaxY.Text)));
							y2=(int)((double)DY*(double.Parse((string)debugDs.pos.Rows[i-1][j+1])*1000/double.Parse(this.txtMaxY.Text)));
						}
						catch
						{
							return;
						}
						dc.DrawLine(penArr[j],x1,GY2-y1,x2,GY2-y2);
						if(bMinSep)
						{
							minSepY[minSepInx++]=(int)(double.Parse((string)debugDs.pos.Rows[i][j+1])*1000);
						}
						try
						{
							state1=int.Parse((string)debugDs.ncf.Rows[i][j+1]);
							state1>>=8;
							state2=int.Parse((string)debugDs.ncf.Rows[i-1][j+1]);
							state2>>=8;
							if(state1!=state2)
							{
								dc.FillEllipse(this.drawBrush,x1,GY2-y1,3,3);
							}
						}
						catch
						{
							
						}
					}
				}
				if((minSepY[1]-minSepY[0])<minSep)
				{
					minSep=(minSepY[1]-minSepY[0]);
					minSepY1=minSepY[1];
					minSepY0=minSepY[0];
					time=(string)debugDs.pos.Rows[i][0]; 
					minSepX=double.Parse(time);
				}
			}
			if(bMinSep && debugDs.pos.Rows.Count>1)
			{
				y1=(int)((double)DY*(double)minSepY1/double.Parse(this.txtMaxY.Text));
				y2=(int)((double)DY*(double)minSepY0/double.Parse(this.txtMaxY.Text));
				x1=GX2-(int)(DX*((lastTime-minSepX)/CurrentTimeRange));
				dc.DrawLine(penArr[2],x1,GY2-y1,x1,GY2-y2);
				string strTime=minSepX.ToString()+" sec";
				string strMin="Min Separation :"+minSep.ToString()+" mm";
				if(!strTime.Equals(this.statusBarTSA.Panels[0].Text))
					this.statusBarTSA.Panels[0].Text=strTime;
				if(!strMin.Equals(this.statusBarTSA.Panels[1].Text))
					this.statusBarTSA.Panels[1].Text=strMin;
			}
		}
		
		public void RefreshTSA()
		{
			gridTraffic.Width=230;
			gridTraffic.Height=this.tabSimul.ClientSize.Height-gridTraffic.Top-this.statusBarTSA.Height-20;
			this.pnTrajectory.Height=this.tabSimul.ClientSize.Height-pnTrajectory.Top-this.statusBarTSA.Height-5;
			HOIST_OX=gridTraffic.Left+gridTraffic.Width+10;

			GY2=(this.tabSimul.ClientSize.Height-GRP_BORDER);
			maxFloor=(int)configDs.FloorHeight.Rows.Count;
			int floorHeight=(this.tabSimul.ClientSize.Height-GRP_BORDER*3)/maxFloor;

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
				lbOcss.Top=this.gridTraffic.Top+15;
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
			
			make2D(0,1,gridTraffic.Left+gridTraffic.Width+10,gridTraffic.Top+gridTraffic.Height,HOIST_WIDTH+HALL_WIDTH*2,gridTraffic.Height-15);
			DrawGrpFrameTSA();
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
		}

		private void tabTraject_Resize(object sender, System.EventArgs e)
		{
			if(bFormLoaded)
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

		private void fmMain_Closing(object sender, System.ComponentModel.CancelEventArgs e)
		{
			configDs.WriteXml("cogsd.xml",XmlWriteMode.IgnoreSchema);
		}

		public double getPos(int floor)
		{
			double rv=0;
			for(int i=0;i<configDs.FloorHeight.Rows.Count;i++)
			{
				if((int)configDs.FloorHeight.Rows[i][0]==floor)
					rv=(int)configDs.FloorHeight.Rows[i][2];
			}
			return (double)rv;
		}

		public int getFloor(int position)
		{
			int rv=0;
			for(int i=0;i<configDs.FloorHeight.Rows.Count-1;i++)
			{
				int H1=(int)configDs.FloorHeight.Rows[i][2];
				int H2=(int)configDs.FloorHeight.Rows[i+1][2];
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
			/*
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
			*/   
		}

		private void tbSimul_ButtonClick(object sender, System.Windows.Forms.ToolBarButtonClickEventArgs e)
		{
			if(e.Button.Equals(this.tbSimul.Buttons[0]))
			{
				if(state.Equals(SPY_STATE.TSA_SELF))
				{
					if(simulState.Equals(SIMUL_STATE.PAUSE))
					{
						this.tmrSimul.Enabled=true;						
					}
					else // start to run
					{
						dtPosTSA.Rows.Clear();
						this.DrawGrpFrameTSA();
						curTime=0;
						prevPaxInx=-1;
						foreach(csOcss ocss_info in this.ocssList)
						{
							ocss_info.ocss_init();
						}
					}
				}
				if(state.Equals(SPY_STATE.TSA_TSADELAY))
				{
					int orgTrail,tgtTrail,orgLead,tgtLead;
					csOcss ocssTrail=(csOcss)ocssList[0];
					csOcss ocssLead=(csOcss)ocssList[1];
					orgTrail=ocssTrail.actual_pos;
					orgLead=ocssLead.actual_pos;
					tgtTrail=orgTrail+1;
					tgtLead=orgLead+1;
					int fl;
					for(fl=orgTrail+1;fl<configDs.FloorHeight.Rows.Count-1;fl++)
					{
						if((ocssTrail.assigned_calls[fl] & S_FUHC)>0 || (ocssTrail.assigned_calls[fl] & S_FDHC)>0)
						{
							tgtTrail=fl;
							break;
						}
					}
					for(fl=orgLead+1;fl<configDs.FloorHeight.Rows.Count-1;fl++)
					{
						if((ocssLead.assigned_calls[fl] & S_FUHC)>0 || (ocssLead.assigned_calls[fl] & S_FDHC)>0)
						{
							tgtLead=fl;
							break;
						}
					}
					if(shift>=0)
					{
						dll.SetTSA(0,orgTrail,tgtTrail,shift);
						dll.SetTSA(1,orgLead,tgtLead,0);
					}
					else
					{
						dll.SetTSA(0,orgTrail,tgtTrail,0);
						dll.SetTSA(1,orgLead,tgtLead,-shift);
					}
					dtPosTSA.Rows.Clear();
					this.DrawGrpFrameTSA();
					this.tmrSameCnt=0;
					curTime=0;
				}
				this.tmrSimul.Enabled=true;	
			}
			else if(e.Button.Equals(this.tbSimul.Buttons[1]))
			{
				simulState=SIMUL_STATE.STOP;
				this.tmrSimul.Enabled=false;
				this.SpyStateCheck();
			}
			else if(e.Button.Equals(this.tbSimul.Buttons[2]))
			{
				simulState=SIMUL_STATE.PAUSE;
				this.tmrSimul.Enabled=false;
			}
		}

		private void tabMonitor_Click(object sender, System.EventArgs e)
		{
		
		}

		private void btnReport_Click_1(object sender, System.EventArgs e)
		{
		
		}

		private int GetScrHeight(int bdScreenH,int floor)
		{
			int ret;
			int noFloors=configDs.FloorHeight.Rows.Count;
			int bdHeight=(int)configDs.FloorHeight.Rows[noFloors-1]["Position"]+(int)configDs.FloorHeight.Rows[noFloors-1]["Height"];
			int floorHeight=(int)configDs.FloorHeight.Rows[floor]["Height"];
			ret=(int)bdScreenH*(int)configDs.FloorHeight.Rows[floor]["Height"]/(int)bdHeight;
			return ret;
		}

		private int GetScrPos(int bdScreenH,int floor)
		{
			int ret;
			int noFloors=configDs.FloorHeight.Rows.Count;
			int bdHeight=(int)configDs.FloorHeight.Rows[noFloors-1]["Position"]+(int)configDs.FloorHeight.Rows[noFloors-1]["Height"];
			ret=(int)bdScreenH*(int)configDs.FloorHeight.Rows[floor]["Position"]/(int)bdHeight;
			return ret;
		}

		private void make2D(int lowerInx,int upperInx,int left,int bottom,int width,int height)
		{
			int fl;
			csFloor floorUp,floorLo;
			csOcss ocssUp=(csOcss)ocssList[upperInx];
			csOcss ocssLo=(csOcss)ocssList[lowerInx];
			
			
			//int noFloors=configDs.FloorHeight.Rows.Count;
			int bdScreenH=height-ocssUp.lbOcss.Height;
			floorUp=(csFloor)ocssUp.floorList[0];
			floorUp.picHoist.Visible=false;
			floorUp.picHall.Visible=false;
			floorLo=(csFloor)ocssLo.floorList[0];
			floorLo.picHall.Image=this.imageList1.Images[(int)IMG.NO_CARCALL];
			floorLo.picHall.Left=left;
			floorLo.picHall.Width=HALL_WIDTH;
			floorLo.picHall.Top=bottom-GetScrPos(bdScreenH,1);
			floorLo.picHall.Height=bottom-floorLo.picHall.Top;
			floorLo.picHall.Visible=true;
			floorLo.picHoist.Left=floorLo.picHall.Left+floorLo.picHall.Width;
			floorLo.picHoist.Width=HOIST_WIDTH;
			floorLo.picHoist.Height=floorLo.picHall.Height;
			floorLo.picHoist.Top=floorLo.picHall.Top;
			floorLo.picHoist.Visible=true;
			
			for(fl=1;fl<(int)configDs.FloorHeight.Rows.Count-1;fl++)
			{
				floorUp=(csFloor)ocssUp.floorList[fl];
				floorLo=(csFloor)ocssLo.floorList[fl];
				floorLo.picHall.Left=left;
				floorLo.picHall.Width=HALL_WIDTH;
				floorLo.picHall.Top=bottom-GetScrPos(bdScreenH,fl+1);
				floorLo.picHall.Height=GetScrPos(bdScreenH,fl+1)-GetScrPos(bdScreenH,fl);
				floorLo.picHoist.Left=floorLo.picHall.Left+floorLo.picHall.Width;
				floorLo.picHoist.Width=HOIST_WIDTH/2;
				floorLo.picHoist.Top=floorLo.picHall.Top;
				floorLo.picHoist.Height=floorLo.picHall.Height;


				floorUp.picHoist.Left=floorLo.picHoist.Left+floorLo.picHoist.Width;
				floorUp.picHoist.Width=HOIST_WIDTH/2;
				floorUp.picHoist.Top=floorLo.picHoist.Top;
				floorUp.picHoist.Height=floorLo.picHoist.Height;
				floorUp.picHall.Left=floorUp.picHoist.Left+floorUp.picHoist.Width;
				floorUp.picHall.Top=floorUp.picHoist.Top;
				floorUp.picHall.Width=HALL_WIDTH;
				floorUp.picHall.Height=floorLo.picHoist.Height;
			}
			floorLo=(csFloor)ocssLo.floorList[fl];
			floorUp=(csFloor)ocssUp.floorList[fl];
			floorLo.picHall.Visible=false;
			floorLo.picHoist.Visible=false;
			
			csFloor prevfloorLo=(csFloor)ocssLo.floorList[fl-1];

			floorUp.picHoist.Left=prevfloorLo.picHoist.Left;
			floorUp.picHoist.Width=HOIST_WIDTH;
			floorUp.picHoist.Top=bottom-bdScreenH;
			floorUp.picHoist.Height=prevfloorLo.picHoist.Top-(bottom-bdScreenH);
			floorUp.picHoist.Visible=true;
			floorUp.picHall.Left=floorUp.picHoist.Left+floorUp.picHoist.Width;
			floorUp.picHall.Width=HALL_WIDTH;
			floorUp.picHall.Height=floorUp.picHoist.Height;
			floorUp.picHall.Top=floorUp.picHoist.Top;
			floorUp.picHall.Image=this.imageList1.Images[(int)IMG.NO_CARCALL];
			floorUp.picHall.Visible=true;

			ocssLo.lbOcss.Text="O_"+string.Format("{0:D}",lowerInx+1);
			ocssLo.lbOcss.Left=prevfloorLo.picHall.Left;
			ocssLo.lbOcss.Width=HALL_WIDTH+HOIST_WIDTH/2;
			//ocssLo.lbOcss.Top=floorUp.picHall.Top-ocssLo.lbOcss.Height;



			ocssUp.lbOcss.Text="O_"+string.Format("{0:D}",upperInx+1);
			ocssUp.lbOcss.Left=ocssLo.lbOcss.Left+ocssLo.lbOcss.Width;
			ocssUp.lbOcss.Width=HALL_WIDTH+HOIST_WIDTH/2;
			//ocssUp.lbOcss.Top=floorUp.picHall.Top-ocssUp.lbOcss.Height;
			int minFloorHeight=(int)configDs.FloorHeight.Rows[0]["Height"];
			int minHeightFloor=0;
			for(fl=1;fl<(int)configDs.FloorHeight.Rows.Count;fl++)
			{
				if((int)configDs.FloorHeight.Rows[fl]["Height"]<minFloorHeight)
				{
					minFloorHeight=(int)configDs.FloorHeight.Rows[fl]["Height"];
					minHeightFloor=fl;
				}
			}

			floorUp=(csFloor)ocssUp.floorList[1];
			floorLo=(csFloor)ocssLo.floorList[0];

			ocssLo.picCar.Left=floorLo.picHoist.Left;
			if(minHeightFloor==configDs.FloorHeight.Rows.Count-1)
			{
				ocssLo.picCar.Height=bdScreenH-GetScrPos(bdScreenH,minHeightFloor);
			}
			else
			{
				ocssLo.picCar.Height=GetScrPos(bdScreenH,minHeightFloor+1)-GetScrPos(bdScreenH,minHeightFloor);
			}
			ocssLo.picCar.Top=bottom-ocssLo.picCar.Height;
			ocssUp.picCar.Left=floorLo.picHoist.Left;
			ocssUp.picCar.Height=ocssLo.picCar.Height;
			ocssUp.picCar.Top=bottom-GetScrPos(bdScreenH,1)-ocssUp.picCar.Height;
		}

		private void btnTSA_Click(object sender, System.EventArgs e)
		{
			state=SPY_STATE.TSA_TSADELAY;
			int minMargin,tmpMargin;
			int nLandings=(int)configDs.FloorHeight.Rows.Count;
			int maxHeight=(int)configDs.FloorHeight.Rows[nLandings-1][2]+(int)configDs.FloorHeight.Rows[nLandings-1][1];
			shift=0;
			int timeSlice=0;
			bool bFirst=true;
			int fl;
			int[] pos=new int[MAX_OCSS];
			int prevPos0,prevPos1,prevDiff;
			int nSameCnt;

			int orgTrail,tgtTrail,orgLead,tgtLead;
			csOcss ocssTrail=(csOcss)ocssList[0];
			csOcss ocssLead=(csOcss)ocssList[1];
			orgTrail=ocssTrail.actual_pos;
			orgLead=ocssLead.actual_pos;
			tgtTrail=orgTrail+1;
			tgtLead=orgLead+1;
			this.statusBarTSA.Panels[2].Text="TSA Delay :";
			TSATimeRange=25;
			for(fl=orgTrail+1;fl<configDs.FloorHeight.Rows.Count-1;fl++)
			{
				if((ocssTrail.assigned_calls[fl] & S_FUHC)>0 || (ocssTrail.assigned_calls[fl] & S_FDHC)>0)
				{
					tgtTrail=fl;
					break;
				}
			}
			for(fl=orgLead+1;fl<configDs.FloorHeight.Rows.Count-1;fl++)
			{
				if((ocssLead.assigned_calls[fl] & S_FUHC)>0 || (ocssLead.assigned_calls[fl] & S_FDHC)>0)
				{
					tgtLead=fl;
					break;
				}
			}
			bool bNeedTSA=(tgtTrail>=orgLead) ? true : false;
			while(bNeedTSA)
			{
				minMargin=(int)maxHeight;
				if(shift>0)
				{
					dll.SetTSA(0,orgTrail,tgtTrail,shift);
					dll.SetTSA(1,orgLead,tgtLead,0);
				}
				else if(shift<0)
				{
					dll.SetTSA(0,orgTrail,tgtTrail,0);
					dll.SetTSA(1,orgLead,tgtLead,-shift);
				}
				else
				{
					dll.SetTSA(0,orgTrail,tgtTrail,0);
					dll.SetTSA(1,orgLead,tgtLead,0);
				}

				dtPosTSA.Rows.Clear();
				prevPos0=prevPos1=prevDiff=0;
				nSameCnt=0;
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
					if(prevPos0==pos[0] && prevPos1==pos[1] && prevDiff==tmpMargin)
					{
						nSameCnt++;
						if(nSameCnt>10)
							break;
					}
					prevPos0=pos[0];
					prevPos1=pos[1];
					prevDiff=tmpMargin;
					if(i>TSATimeRange*10-10)
						TSATimeRange+=10; // add 10 sec

					DataRow row=dtPosTSA.NewRow();
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
					dtPosTSA.Rows.Add(row);
					if(dtPosTSA.Rows.Count>TSATimeRange*10)
						dtPosTSA.Rows.RemoveAt(0);
				}
				this.DrawGrpFrameTSA();
				//this.statusBarTSA.Panels[1].Text="Min Separation : "+minMargin.ToString()+" mm";
				if(bFirst && minMargin<int.Parse(this.txtSep.Text))
				{
					bFirst=false;
					timeSlice=TIME_SLICE;
				}
				if(bFirst && minMargin>=int.Parse(this.txtSep.Text))
				{
					bFirst=false;
					timeSlice=-TIME_SLICE;
				}
				if(timeSlice>0 && minMargin>=int.Parse(this.txtSep.Text))
					break;
				else if(timeSlice<0 && minMargin<int.Parse(this.txtSep.Text))
				{
					shift-=timeSlice;
					break;
				}
				shift+=timeSlice;
				Application.DoEvents();
				if(!state.Equals(SPY_STATE.TSA_TSADELAY))
					break;
			}

			if(shift<=0 && state.Equals(SPY_STATE.TSA_TSADELAY))
			{
				minMargin=(int)maxHeight;
				dll.SetTSA(0,orgTrail,tgtTrail,0);
				dll.SetTSA(1,orgLead,tgtLead,-shift);


				dtPosTSA.Rows.Clear();
			
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
					DataRow row=dtPosTSA.NewRow();
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
					dtPosTSA.Rows.Add(row);
					if(dtPosTSA.Rows.Count>TSATimeRange*10)
						dtPosTSA.Rows.RemoveAt(0);
				}
				this.DrawGrpFrameTSA();
				//this.statusBarTSA.Panels[1].Text="Min Separation : "+minMargin.ToString()+" mm";
			}
			if(state.Equals(SPY_STATE.TSA_TSADELAY))
				this.statusBarTSA.Panels[2].Text="TSA Delay : "+shift.ToString()+" msec";
		}

		private void tabSimul_Resize(object sender, System.EventArgs e)
		{
			if(bFormLoaded)
				this.DrawGrpFrameTSA();
		}

		private void tabSimul_Paint(object sender, System.Windows.Forms.PaintEventArgs e)
		{
			if(bFormLoaded)
				this.DrawGrpFrameTSA();
		}

		void Load2dpFile(string path)
		{
			uint pos=0;
			TYPE2DP type=TYPE2DP.NONE;
			try
			{
				using (StreamReader sr=new StreamReader(path))
				{
					while (sr.Peek() >= 0) 
					{
						string str=sr.ReadLine();
						if(type.Equals(TYPE2DP.NONE))
						{
							if(str.IndexOf("FLOOR HEIGHT")>=0)
							{
								type=TYPE2DP.FLOOR;
								configDs.FloorHeight.Clear();
							}
							continue;
						}
						if(str.Length<1)
						{
							type=TYPE2DP.NONE;
							continue;
						}
						if(type.Equals(TYPE2DP.FLOOR))
						{
							string[] strArr=str.Split(' ');
							DataRow row=configDs.FloorHeight.NewRow();
							row[0]=strArr[strArr.Length-1]; //Label
							double dHeight=double.Parse(strArr[0]);
							uint nHeight=(uint)(dHeight*1000);
							row[1]=nHeight; //Height
							row[2]=pos; //Position
							pos+=nHeight;
							configDs.FloorHeight.Rows.Add(row);
						}
					}
				}
			}
			catch(Exception eee)
			{
				MessageBox.Show(eee.Message);
			}
		}

		private void btnLoad_Click(object sender, System.EventArgs e)
		{
			if(Directory.Exists(@"D:\31166\Developing\Debug"))
				openFile2dp.InitialDirectory=@"D:\31166\Developing\Debug";
			openFile2dp.Filter = "COGS 2dp files (*.2dp)|*.2dp|All files (*.*)|*.*" ;
			openFile2dp.FilterIndex = 1 ;
			openFile2dp.RestoreDirectory = true ;

			if(openFile2dp.ShowDialog() == DialogResult.OK)
			{
				Load2dpFile(openFile2dp.FileName);
			}
		}

		private void cbHwy_SelectedIndexChanged(object sender, System.EventArgs e)
		{
			int planInx=0;
			int count=0;
			int hwy=this.cbHwy.SelectedIndex;
			try
			{
				using (StreamReader sr=new StreamReader(@"c:\plan"+hwy.ToString()+".csv"))
				{
					while (sr.Peek() >= 0) 
					{
						string str=sr.ReadLine();
						if(str.IndexOf("UPPER")>=0)
						{
							planInx=hwy*2+1;
							count=0;
							dtPlan[planInx].Rows.Clear();
							continue;
						}
						else if(str.IndexOf("LOWER")>=0)
						{
							planInx=hwy*2;
							count=0;
							dtPlan[planInx].Rows.Clear();
							continue;
						}
						string[] strArr=str.Split(',');

						DataRow row=dtPlan[planInx].NewRow();
						row[0]=strArr[0]; //index
						row[1]=count.ToString(); //sequence index
						row[2]=strArr[1]; //floor
						row[3]=strArr[2]; //type
						row[4]=strArr[3]; //precedence
						//precedence sequence index

						row[0]=strArr[0]; //time
						dtPlan[planInx].Rows.Add(row);
						count++;
					}
				}
			}
			catch(Exception eee)
			{
				MessageBox.Show(eee.Message);
			}
			
			int carId,otherCarId;
			int prec;
			int otherInx;

			for(int car=0;car<2;car++)
			{
				carId=(car==0) ? hwy*2 : hwy*2+1;
				otherCarId=(car==0) ? hwy*2+1 : hwy*2;
				for(int i=0;i<dtPlan[carId].Rows.Count;i++)
				{
					prec=int.Parse((string)dtPlan[carId].Rows[i][4]);
					if(prec!=-1)
					{
						for(int j=0;j<dtPlan[otherCarId].Rows.Count;j++)
						{
							otherInx=int.Parse((string)dtPlan[otherCarId].Rows[j][0]);
							if(otherInx==prec)
							{
								string str=(string)dtPlan[otherCarId].Rows[j][1];
								dtPlan[carId].Rows[i][5]=str;
							}	
						}    
					}
				}
			}
			if(tabDebug.SelectedTab.Text.Equals("HEXEC") && this.cbHwy.SelectedIndex==hwy)
			{
				if(this.gridUpperPlan.DataSource!=dtPlan[hwy*2+1])
				{
					gridUpperPlan.DataSource=dtPlan[hwy*2+1];
				}
				if(this.gridLowerPlan.DataSource!=dtPlan[hwy*2])
				{
					gridLowerPlan.DataSource=dtPlan[hwy*2];
				}
			}
			else
				return;
		}

		private void btnTSAAll_Click(object sender, System.EventArgs e)
		{
			configDs.TSADelay.Rows.Clear();
			configDs.TSACompress.Rows.Clear();
			gridTraffic.DataSource=configDs.TSADelay;
			int maxFloor=this.configDs.FloorHeight.Rows.Count;
			int orgTrail,orgLead,tgtTrail,tgtLead;
			int TDLODist,LeadTrv,TrailTrv,TSADelay;
			bool bSameExist;
			int leadDecel,trailT3;
			int minSepTime=0;
			for(orgTrail=0;orgTrail<maxFloor-2;orgTrail++)
			{
				for(orgLead=orgTrail+1;orgLead<maxFloor-1;orgLead++)
				{
					for(tgtTrail=orgTrail+1;tgtTrail<maxFloor-1;tgtTrail++)
					{
						for(tgtLead=tgtTrail+1;tgtLead<maxFloor;tgtLead++)
						{
							if(tgtLead<=orgLead)
								continue;
							if(tgtTrail<orgLead)
								continue;
							TDLODist=(int)configDs.FloorHeight.Rows[orgLead][2]-(int)configDs.FloorHeight.Rows[tgtTrail][2];
							LeadTrv=(int)configDs.FloorHeight.Rows[tgtLead][2]-(int)configDs.FloorHeight.Rows[orgLead][2];
							TrailTrv=(int)configDs.FloorHeight.Rows[tgtTrail][2]-(int)configDs.FloorHeight.Rows[orgTrail][2];
							bSameExist=false;
							for(int i=0;i<configDs.TSADelay.Rows.Count;i++)
							{
								if(TDLODist==(int)configDs.TSADelay.Rows[i][0] &&
									LeadTrv==(int)configDs.TSADelay.Rows[i][1] &&
									TrailTrv==(int)configDs.TSADelay.Rows[i][2])
								{
									bSameExist=true;
									break;
								}
							}
							if(!bSameExist)
							{
								//TSADelay=GetTSADelay(orgLead,tgtLead,orgTrail,tgtTrail,ref minSepTime);
								TSADelay=dll.GetTSADelay(orgLead,tgtLead,orgTrail,tgtTrail,int.Parse(this.txtSep.Text));
								DataRow row=configDs.TSADelay.NewRow();
								row[0]=TDLODist;
								row[1]=LeadTrv;
								row[2]=TrailTrv;
								row[3]=TSADelay;
								configDs.TSADelay.Rows.Add(row);
								/*
								leadDecel=dll.GetCommittableTime(1,orgLead,tgtLead);//msec
								trailT3=dll.GetNCP_T3(0,orgTrail,tgtTrail);
								bool bType1=false;
								bool bType3=false;
								if(TSADelay>=0)
								{
									if(minSepTime<leadDecel)
									{
										bType1=true;
									}
									if(trailT3>0 && minSepTime>trailT3+TSADelay)
									{
										bType3=true;
									}
								}
								else
								{
									if(minSepTime<leadDecel-TSADelay)
									{
										bType1=true;										
									}
									if(trailT3>0 && minSepTime>trailT3)
									{
										bType3=true;
									}
								}
								bool bFound1=false;
								bool bFound3=false;
								if(bType1 || bType3)
								{
									for(int i=0;i<configDs.TSACompress.Rows.Count;i++)
									{
										if(bType1)
										{
											if(TDLODist==(int)configDs.TSACompress.Rows[i][0] &&
												TrailTrv==(int)configDs.TSACompress.Rows[i][2])
											{
												if(TSADelay==(int)configDs.TSACompress.Rows[i][3])
												{
													if(LeadTrv<(int)configDs.TSACompress.Rows[i][1])
													{
														configDs.TSACompress.Rows[i][1]=LeadTrv;
													}
													bFound1=true;
												}
											}
										}
										if(bType3)
										{
											if(TDLODist==(int)configDs.TSACompress.Rows[i][0] &&
												LeadTrv==(int)configDs.TSACompress.Rows[i][1])
											{
												int tableTrailTrv=(int)configDs.TSACompress.Rows[i][2];
												int tableDelay=(int)configDs.TSACompress.Rows[i][3];
												int diffDist=(tableTrailTrv>TrailTrv) ? tableTrailTrv-TrailTrv : TrailTrv-tableTrailTrv;
												int diffTime=(tableDelay>TSADelay) ? tableDelay-TSADelay : TSADelay-tableDelay;
												if(diffTime>0 && 1000*diffDist/diffTime==int.Parse(this.txtVel.Text))
												{
													if(TrailTrv<(int)configDs.TSACompress.Rows[i][2])
													{
														configDs.TSACompress.Rows[i][2]=TrailTrv;
													}
													bFound3=true;
												}
											}
										}
										if((bFound1 && bFound3) ||
											(!bType3 && bFound1) ||
											(!bType1 && bFound3))
											break;
									
									}
								}
								if(bType1 && !bFound1)
								{
									row=configDs.TSACompress.NewRow();
									row[0]=TDLODist;
									row[1]=LeadTrv;
									row[2]=TrailTrv;
									row[3]=TSADelay;
									row[4]=1;
									configDs.TSACompress.Rows.Add(row);
								}
								if(bType3 && !bFound3)
								{
									row=configDs.TSACompress.NewRow();
									row[0]=TDLODist;
									row[1]=LeadTrv;
									row[2]=TrailTrv;
									row[3]=TSADelay;
									row[4]=3;
									configDs.TSACompress.Rows.Add(row);
								}
								if(!bType1 && !bType3)
								{
									row=configDs.TSACompress.NewRow();
									row[0]=TDLODist;
									row[1]=LeadTrv;
									row[2]=TrailTrv;
									row[3]=TSADelay;
									row[4]=5;
									configDs.TSACompress.Rows.Add(row);
								}
								*/
							}
						}
						Application.DoEvents();
					}
				}
			}
			MessageBox.Show(configDs.TSADelay.Rows.Count.ToString()+"cases");
		}

		public int GetTSADelay(int orgLead,int tgtLead,int orgTrail,int tgtTrail,ref int minSepTime)
		{
			int minMargin,tmpMargin,prevMargin,curMargin,prevMinTime,curMinTime;
			int nLandings=(int)configDs.FloorHeight.Rows.Count;
			int maxHeight=(int)configDs.FloorHeight.Rows[nLandings-1][2]+(int)configDs.FloorHeight.Rows[nLandings-1][1];
			
			int timeSlice=0;

			bool bFirst=true;
			int fl;
			int nSameCnt;
			int[] pos=new int[MAX_OCSS];
			pos[0]=(int)configDs.FloorHeight.Rows[tgtTrail][2];
			pos[1]=(int)configDs.FloorHeight.Rows[tgtLead][2];
			pos[2]=(int)configDs.FloorHeight.Rows[orgTrail][2];
			pos[3]=(int)configDs.FloorHeight.Rows[orgLead][2];
			int orgDiff=(pos[2]>pos[3]) ? (pos[2]-pos[3]) : (pos[3]-pos[2]);
			int tgtDiff=(pos[0]>pos[1]) ? (pos[0]-pos[1]) : (pos[1]-pos[0]);
			if(tgtDiff<int.Parse(this.txtSep.Text) || orgDiff<int.Parse(this.txtSep.Text))
				return 100000;
			csOcss ocssTrail=(csOcss)ocssList[0];
			csOcss ocssLead=(csOcss)ocssList[1];
			int prevPos0,prevPos1,prevDiff;
			shift=0;
			curMargin=(int)maxHeight;
			curMinTime=0;
			prevMinTime=0;
			while(true)
			{
				minMargin=(int)maxHeight;
				if(shift>0)
				{
					dll.SetTSA(0,orgTrail,tgtTrail,shift);
					dll.SetTSA(1,orgLead,tgtLead,0);
				}
				else if(shift<0)
				{
					dll.SetTSA(0,orgTrail,tgtTrail,0);
					dll.SetTSA(1,orgLead,tgtLead,-shift);
				}
				else
				{
					dll.SetTSA(0,orgTrail,tgtTrail,0);
					dll.SetTSA(1,orgLead,tgtLead,0);
				}

				prevPos0=prevPos1=prevDiff=0;
				nSameCnt=0;
				for(int i=0;i<1000;i++)
				{
					pos[0]=(int)dll.GetMotion(0,i*100,(int)TSA_TYPE.POSITION);
					pos[1]=(int)dll.GetMotion(1,i*100,(int)TSA_TYPE.POSITION);
					pos[2]=(int)dll.GetMotion(0,i*100,this.cbTrailing.SelectedIndex);
					pos[3]=(int)dll.GetMotion(1,i*100,this.cbLeading.SelectedIndex);
					tmpMargin=(int)pos[3]-(int)pos[2];
					if(tmpMargin<minMargin)
					{
						minMargin=tmpMargin;
						curMargin=minMargin;
						curMinTime=i*100;
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
				}
				if(bFirst && minMargin<int.Parse(this.txtSep.Text))
				{
					bFirst=false;
					timeSlice=TIME_SLICE;
				}
				if(bFirst && minMargin>=int.Parse(this.txtSep.Text))
				{
					bFirst=false;
					timeSlice=-TIME_SLICE;
				}
				if(timeSlice>0 && minMargin>=int.Parse(this.txtSep.Text))
				{
					minSepTime=prevMinTime+100;
					break;
				}
				else if(timeSlice<0 && minMargin<int.Parse(this.txtSep.Text))
				{
					shift-=timeSlice;
					minSepTime=prevMinTime;
					break;
				}
				shift+=timeSlice;
				prevMargin=curMargin;
				prevMinTime=curMinTime;
			}
			return shift;
		}

		private void mnuDot_Click(object sender, System.EventArgs e)
		{
			float[] pattern=new float[2];
			pattern[0]=5;
			pattern[1]=3;
			penArr[chkBoxInx].DashPattern=pattern;
			this.DrawGrpFrame();
		}

		private void checkBox1_MouseDown(object sender, System.Windows.Forms.MouseEventArgs e)
		{
			if(e.Button ==System.Windows.Forms.MouseButtons.Right)
			{
				for(int i=0;i<MAX_OCSS-1;i++)
				{
					if(sender.Equals(this.checkArr[i]))
					{
						chkBoxInx=i;
					}
				}
			}
		}

		bool bCarManual=false;
		bool bDiscrete=false;
		int initialY=0;
		private void picCar_MouseDown(object sender, System.Windows.Forms.MouseEventArgs e)
		{
			ushort nControl=(ushort)GetKeyState(0x11 /*VK_CONTROL*/);
			ushort nShift=(ushort)GetKeyState(0x10 /*VK_CONTROL*/);
			bDiscrete=false;
			if(nControl>10)
			{
				bDiscrete=true;
			}
			bCarManual=true;
			initialY=e.Y;
		}

		private void picCar_MouseMove(object sender, System.Windows.Forms.MouseEventArgs e)
		{
			PictureBox picCar=(PictureBox)sender;
			if(bCarManual)
			{
				picCar.Top+=e.Y-initialY;
			}
		}

		private void picCar_MouseUp(object sender, System.Windows.Forms.MouseEventArgs e)
		{
			if(bCarManual)
				bCarManual=false;
			if(bDiscrete)
			{
				PictureBox picCar=(PictureBox)sender;
				int bottom=picCar.Top+picCar.Height;
				csFloor floor;
				int startInx=(int)picCar.Tag%2;
				csOcss ocss_info=(csOcss)ocssList[(int)picCar.Tag];
				for(int fl=startInx;fl<(int)configDs.FloorHeight.Rows.Count-1+startInx;fl++)
				{
					floor=(csFloor)ocss_info.floorList[fl];
					if(bottom>floor.picHall.Top+floor.picHall.Height/2)
					{
						picCar.Top=floor.picHall.Top+floor.picHall.Height-picCar.Height;
						ocss_info.actual_pos=fl;						
						break;
					}
				}
			}
		}
		
		private void AddData(int[] pos)
		{
			int x1,x2,y1,y2;
			double dTime;
			int[] minSepY=new int[2];
			int nLandings=configDs.FloorHeight.Rows.Count;
			int maxHeight=(int)configDs.FloorHeight.Rows[nLandings-1][2]+(int)configDs.FloorHeight.Rows[nLandings-1][1];
			DataRow row=dtPosTSA.NewRow();
			double time=(double)curTime/1000.0;
			row[0]=time.ToString(); //time
			Graphics dc=this.pnTrajectory.CreateGraphics();
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
			dtPosTSA.Rows.Add(row);
			if(dtPosTSA.Rows.Count>TSATimeRange*10)
				dtPosTSA.Rows.RemoveAt(0);
			
			double firstTime=double.Parse((string)this.dtPosTSA.Rows[0][0]);
			int nRows=dtPosTSA.Rows.Count;
			if(nRows>=2)
			{
				dTime=double.Parse((string)dtPosTSA.Rows[nRows-2][0]);
				x1=GX1+(int)(DX*((dTime-firstTime)/TSATimeRange));
				dTime=double.Parse((string)dtPosTSA.Rows[nRows-1][0]);
				x2=GX1+(int)(DX*((dTime-firstTime)/TSATimeRange));
				if(x2>GX2)
					return;
				for(int j=0;j<4;j++)
				{
					y1=(int)((double)DY*(double.Parse((string)dtPosTSA.Rows[nRows-2][j+1])/(double)maxHeight));
					y2=(int)((double)DY*(double.Parse((string)dtPosTSA.Rows[nRows-1][j+1])/(double)maxHeight));
					dc.DrawLine(penArr[j],x1,GY2-y1,x2,GY2-y2);
				}	
				minSepY[1]=(int)double.Parse((string)dtPosTSA.Rows[nRows-1][4]);
				minSepY[0]=(int)double.Parse((string)dtPosTSA.Rows[nRows-1][3]);
				if((minSepY[1]-minSepY[0])<int.Parse(this.txtSep.Text))
				{
					y1=DY*minSepY[0]/maxHeight;
					y2=DY*minSepY[1]/maxHeight;
					dc.DrawLine(penArr[2],x2,GY2-y1,x2,GY2-y2);
				}
			}
		}

		int tmrPrevPos0=0,tmrPrevPos1=0,tmrSameCnt=0;
		private void tmrSimul_Tick(object sender, System.EventArgs e)
		{
			if(state.Equals(SPY_STATE.TSA_TSADELAY))
			{
				int[] pos=new int[MAX_OCSS];
				if(state.Equals(SPY_STATE.TSA_TSADELAY))
				{
					pos[0]=(int)dll.GetMotion(0,curTime,(int)TSA_TYPE.POSITION);
					pos[1]=(int)dll.GetMotion(1,curTime,(int)TSA_TYPE.POSITION);
					pos[2]=(int)dll.GetMotion(0,curTime,this.cbTrailing.SelectedIndex);
					pos[3]=(int)dll.GetMotion(1,curTime,this.cbLeading.SelectedIndex);
				}
				if(tmrPrevPos0==pos[0] && tmrPrevPos1==pos[1])
					tmrSameCnt++;
				if(tmrSameCnt>10)
					this.tmrSimul.Enabled=false;
				tmrPrevPos0=pos[0];
				tmrPrevPos1=pos[1];
			
				int nFloor=configDs.FloorHeight.Rows.Count;
				csOcss ocssLo=(csOcss)ocssList[0];
				csOcss ocssUp=(csOcss)ocssList[1];
				csFloor flLo=(csFloor)ocssLo.floorList[0];
				csFloor flUp=(csFloor)ocssUp.floorList[nFloor-1];
				int bdHeight=(int)configDs.FloorHeight.Rows[nFloor-1][1]+(int)configDs.FloorHeight.Rows[nFloor-1][2];
				int scrHeight=flLo.picHoist.Top+flLo.picHoist.Height-flUp.picHoist.Top;
				ocssLo.picCar.Top=flLo.picHoist.Top+flLo.picHoist.Height-scrHeight*pos[0]/bdHeight-ocssLo.picCar.Height;
				ocssUp.picCar.Top=flLo.picHoist.Top+flLo.picHoist.Height-scrHeight*pos[1]/bdHeight-ocssUp.picCar.Height;
				AddData(pos);
			}
			else if(state.Equals(SPY_STATE.TSA_SELF))
			{
				ProcessTSASelf();
				double dTime=curTime/1000.0;
				this.statusBarTSA.Panels[0].Text="Time :"+dTime.ToString();
			}
			curTime+=100;
		}
		
		private void UpdateAssignedCall(int car)
		{
			//csOcss ocss_info=(csOcss)ocssList[i];
			
		}
		
		private void CheckNewCall()
		{
			double gridTime,w_t,s_t;
			int pos,dst,car;
			csOcss ocss_info;
			int checkPaxInx;
			while(true)
			{
				checkPaxInx=prevPaxInx+1;
				if(checkPaxInx>=configDs.traffic.Rows.Count)
					break;
				try
				{
					gridTime=(double)configDs.traffic.Rows[checkPaxInx][0];
					gridTime*=1000;
					pos=(int)configDs.traffic.Rows[checkPaxInx][1];
					dst=(int)configDs.traffic.Rows[checkPaxInx][2];
					car=(int)configDs.traffic.Rows[checkPaxInx][3];
					if(car<0)
					{
						prevPaxInx=checkPaxInx;
						continue;
					}
				}
				catch
				{
					prevPaxInx=checkPaxInx;
					continue;
				}
				if(gridTime>curTime)
					break;
				ocss_info=(csOcss)ocssList[car];
				ocss_info.paxList.Add(checkPaxInx);
				EnterHallCall(car,pos,dst);
				if(checkPaxInx<configDs.traffic.Rows.Count)
					this.gridTraffic.Select(checkPaxInx);
				prevPaxInx=checkPaxInx;
			}
		}
		
		public void EnterHallCall(int car,int pos,int dst)
		{
			bool bUp;
			bUp=(pos<dst) ? true : false;
			csOcss ocss_info=(csOcss)ocssList[car];
			csFloor floor=(csFloor)ocss_info.floorList[pos];
			if(bUp)
			{
				ocss_info.assigned_calls[pos] |= S_FUHC;
				floor.upHallCall=1;
			}
			else
			{
				ocss_info.assigned_calls[pos] |= S_FDHC;
				floor.dnHallCall=1;
			}
			floor.picHall.Image=this.imageList1.Images[(int)floor.hallCall+5];
		}

		private void ProcessTSASelf()
		{
			int target;
			csOcss ocss_info;
			CheckNewCall();
			//UpdateAssignedCall();
			for(int i=0;i<ocssList.Count;i++)
			{
				ocss_info=(csOcss)ocssList[i];
				target=ocss_info.GetNextTarget();
				if(ocss_info.state.Equals(CAR_STATE.CIS))
				{
					ocss_info.target_pos=target;
					dll.MotionCmd(i,ocss_info.actual_pos,ocss_info.target_pos,this.curTime,ocss_info.jerk,ocss_info.acc,ocss_info.vel);
					if(target!=ocss_info.actual_pos)
					{
						ocss_info.mov_dir=(target>ocss_info.actual_pos) ? csOcss.UP_DIR : csOcss.DOWN_DIR;
						ocss_info.state=CAR_STATE.CAS;
						ocss_info.decelTime=dll.GetCommittableTime(i,ocss_info.actual_pos,ocss_info.target_pos)+this.curTime;
						ocss_info.prevEventTime=this.curTime;
						ocss_info.prevFloor=ocss_info.actual_pos;
					}
				}
				else if(ocss_info.state.Equals(CAR_STATE.CAS))
				{
					if(this.curTime>=ocss_info.decelTime)
					{
						ocss_info.state=CAR_STATE.CAS_DECEL;
					}
					if(target!=ocss_info.target_pos && ocss_info.state!=CAR_STATE.CAS_DECEL)
					{
						ocss_info.target_pos=target;
						int acc=(int)(ushort)configDs.CarInfo.Rows[0]["maxAcceleration"];
						int jerk=(int)(ushort)configDs.CarInfo.Rows[0]["maxJerk"];
						int vel=(int)(ushort)configDs.CarInfo.Rows[0]["maxVelocity"];
						int accMod=acc;
						int jerkMod=jerk;
						int velMod=vel;

						bool bDynamicMotion=false;
						if(ocss_info.mov_dir.Equals(ocss_info.otherCar.mov_dir))
						{
							if(ocss_info.mov_dir.Equals(csOcss.UP_DIR) &&
								ocss_info.otherCar.prevFloor>ocss_info.prevFloor &&
								target>ocss_info.otherCar.prevFloor &&
								this.ckMotion.Checked)
							{
								bDynamicMotion=true;
								this.tmrSimul.Enabled=false;
								motionFm=new fmMotion(this);								
								motionFm.Show();
								motionFm.DrawGrpFrame();
								motionFm.GetProperMotion(ocss_info.otherCar.prevFloor,
														ocss_info.otherCar.target_pos,
														ocss_info.prevFloor,
														ocss_info.target_pos,
														(ocss_info.prevEventTime-ocss_info.otherCar.prevEventTime),
														int.Parse(this.txtSep.Text),
														ref jerkMod,
														ref accMod,
														ref velMod);
								dll.MotionCmd(i,ocss_info.prevFloor,target,ocss_info.prevEventTime,jerkMod,accMod,velMod);
								this.tmrSimul.Enabled=true;
								motionFm.Dispose();
							}
						}
						if(bDynamicMotion.Equals(false))
						{
							dll.MotionCmd(i,ocss_info.prevFloor,ocss_info.target_pos,ocss_info.prevEventTime,jerk,acc,vel);
						}
					}
				}
			}

			int[] pos=new int[MAX_OCSS];
			pos[0]=(int)dll.GetMotion(0,curTime,(int)TSA_TYPE.POSITION);
			pos[1]=(int)dll.GetMotion(1,curTime,(int)TSA_TYPE.POSITION);
			pos[2]=(int)dll.GetMotion(0,curTime,this.cbTrailing.SelectedIndex);
			pos[3]=(int)dll.GetMotion(1,curTime,this.cbLeading.SelectedIndex);
					
			int nFloor=configDs.FloorHeight.Rows.Count;
			csOcss ocssLo=(csOcss)ocssList[0];
			csOcss ocssUp=(csOcss)ocssList[1];
			csFloor flLo=(csFloor)ocssLo.floorList[0];
			csFloor flUp=(csFloor)ocssUp.floorList[nFloor-1];
			int bdHeight=(int)configDs.FloorHeight.Rows[nFloor-1][1]+(int)configDs.FloorHeight.Rows[nFloor-1][2];
			int scrHeight=flLo.picHoist.Top+flLo.picHoist.Height-flUp.picHoist.Top;
			ocssLo.picCar.Top=flLo.picHoist.Top+flLo.picHoist.Height-scrHeight*pos[0]/bdHeight-ocssLo.picCar.Height;
			ocssUp.picCar.Top=flLo.picHoist.Top+flLo.picHoist.Height-scrHeight*pos[1]/bdHeight-ocssUp.picCar.Height;
			AddData(pos);
		}

		private void gridFloorHeight_CurrentCellChanged(object sender, System.EventArgs e)
		{
			int nCount=configDs.FloorHeight.Rows.Count;
			int floorHeight;
			int pos;
			configDs.FloorHeight.Rows[0][2]=0;
			for(int i=0;i<nCount-1;i++)
			{
				try
				{
					floorHeight=(int)configDs.FloorHeight.Rows[i][1];
					pos=(int)configDs.FloorHeight.Rows[i][2];
					configDs.FloorHeight.Rows[i+1][2]=pos+floorHeight;
				}
				catch
				{
					continue;
				}
			}
			ConfigChanged();
		}

		private void SpyStateCheck()
		{
			if(this.chkOC.Checked && !this.chkHPE.Checked && !this.chkHP.Checked)
			{
				this.statusMode.Text="Mode : OC";
				this.state=SPY_STATE.TSA_SELF;
			}
			else if(this.chkHPE.Checked)
			{
				if(this.chkOC.Checked)
					this.chkOC.Checked=false;
				if(this.chkHP.Checked)
				{
					this.statusMode.Text="Mode : HP";
					this.state=SPY_STATE.TSA_HP;
				}
				else
				{
					this.statusMode.Text="Mode : HPE";
					this.state=SPY_STATE.TSA_HPE;
				}
			}
			else if(this.chkHP.Checked)
			{
				this.statusMode.Text="Mode : HP";
				this.state=SPY_STATE.TSA_HP;
				if(this.chkOC.Checked)
					this.chkOC.Checked=false;
				if(!this.chkHPE.Checked)
					this.chkHPE.Checked=true;
			}
		}

		private void TSAChk_Changed(object sender, System.EventArgs e)
		{
			if(!this.gridTraffic.DataSource.Equals(configDs.traffic))
				gridTraffic.DataSource=configDs.traffic;
			this.SpyStateCheck();
		}

		private void gridTraffic_CurrentCellChanged(object sender, System.EventArgs e)
		{
			return;
			double gridTime,w_t,s_t;
			int pos,dst,car;
			bool bWaitFinish,bServeFinish;

			if(gridTraffic.DataSource.Equals(configDs.TSADelay))
			{
				return;
			}
			for(int i=0;i<configDs.traffic.Rows.Count;i++)
			{
				try
				{
					gridTime=(double)configDs.traffic.Rows[i][0];
					gridTime*=1000;
					pos=(int)configDs.traffic.Rows[i][1];
					dst=(int)configDs.traffic.Rows[i][2];
					car=(int)configDs.traffic.Rows[i][3];
				}
				catch
				{
					continue;
				}
				if(curTime<gridTime)
				{
					break;
				}

				try
				{
					w_t=(double)configDs.traffic.Rows[i][4];
					bWaitFinish=true;
				}
				catch
				{
					bWaitFinish=false;
				}
				try
				{
					s_t=(double)configDs.traffic.Rows[i][5];
					bServeFinish=true;
				}
				catch
				{
					bServeFinish=false;
				}
				csOcss ocss_info=(csOcss)ocssList[car];
				csFloor floor;
				if(!bWaitFinish)
				{
					bool bUp;
					bUp=(pos<dst) ? true : false;
					floor=(csFloor)ocss_info.floorList[pos];
					if(bUp)
					{
						ocss_info.assigned_calls[pos] |= S_FUHC;
						floor.upHallCall=1;
					}
					else
					{
						ocss_info.assigned_calls[pos] |= S_FDHC;
						floor.dnHallCall=1;
					}
					floor.picHall.Image=this.imageList1.Images[(int)floor.hallCall+5];
				}
			}
		}

		private void mnuCompress_Click(object sender, System.EventArgs e)
		{
			this.gridTraffic.DataSource=configDs.TSACompress;
//			DataTable dt=new DataTable(
			MessageBox.Show(configDs.TSACompress.Rows.Count.ToString()+" cases");
		}

		private void mnuRestore_Click(object sender, System.EventArgs e)
		{
			configDs.TSARestore.Rows.Clear();
			int maxFloor=this.configDs.FloorHeight.Rows.Count;
			int orgTrail,orgLead,tgtTrail,tgtLead;
			int TDLODist,LeadTrv,TrailTrv,TSADelay;
			bool bSameExist;
			int leadDecel,trailT3;
			int minSepTime=0;

			for(orgTrail=0;orgTrail<maxFloor-2;orgTrail++)
			{
				for(orgLead=orgTrail+1;orgLead<maxFloor-1;orgLead++)
				{
					for(tgtTrail=orgTrail+1;tgtTrail<maxFloor-1;tgtTrail++)
					{
						for(tgtLead=tgtTrail+1;tgtLead<maxFloor;tgtLead++)
						{
							if(tgtLead<=orgLead)
								continue;
							if(tgtTrail<orgLead)
								continue;
							TDLODist=(int)configDs.FloorHeight.Rows[orgLead][2]-(int)configDs.FloorHeight.Rows[tgtTrail][2];
							LeadTrv=(int)configDs.FloorHeight.Rows[tgtLead][2]-(int)configDs.FloorHeight.Rows[orgLead][2];
							TrailTrv=(int)configDs.FloorHeight.Rows[tgtTrail][2]-(int)configDs.FloorHeight.Rows[orgTrail][2];
							bSameExist=false;
							for(int i=0;i<configDs.TSADelay.Rows.Count;i++)
							{
								if(TDLODist==(int)configDs.TSADelay.Rows[i][0] &&
									LeadTrv==(int)configDs.TSADelay.Rows[i][1] &&
									TrailTrv==(int)configDs.TSADelay.Rows[i][2])
								{
									bSameExist=true;
									break;
								}
							}
							if(!bSameExist)
							{
							}
						}
					}
				}
			}
		}

		private void btnClearHexec_Click(object sender, System.EventArgs e)
		{
			for(int i=0;i<MAX_OCSS;i++)
			{
				dtPlan[i].Rows.Clear();
				dtHexec[i].Rows.Clear();
			}
		}

		private void tabSimul_Click(object sender, System.EventArgs e)
		{
		
		}
		private void ConfigChanged()
		{
			bTSAConfigChanged=true;
		}

		private void ConfigChanged(object sender, System.EventArgs e)
		{
			ConfigChanged();
		}
	}
}

namespace COGSD
{
	/// <summary>
	/// fmMain에 대한 요약 설명입니다.
	/// </summary>
	public enum IMG
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
		UPDN_CARCALL,
		PAUSE
	}

	internal enum TSA_TYPE
	{
		POSITION,
		NSP,
		NSF,
		NCP,
		NCF
	}
	internal enum SPY_STATE
	{
		NONE,
		TSA_TSADELAY,
		TSA_SELF,
		TSA_HPE,
		TSA_HP
	}
	internal enum SIMUL_STATE
	{
		NONE,
		RUN,
		STOP,
		PAUSE
	}
	public enum CAR_STATE
	{
		CIS,
		CCS,
		CRS,
		CAS,
		CAS_DECEL,
		CDOS,
		CDCS,
		CHOLD,
		CTSADELAY
	}
	internal enum TYPE2DP {NONE,FLOOR,PROFILE};
}
