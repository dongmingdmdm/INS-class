//////////////////////////////////////////////////////////////////////
//
// MainFrm.cpp
//
// This file implements the CMainFrame class.
//
// This program is the copyright of Dr. Yang Gao’s Research Group 
// "Positioning and Mobile Information Systems (PMIS)" in the 
// Department of Geomatics Engineering at The University of Calgary. 
//
// Any reproduction of this program, or any part of this program without 
// the express permission of Dr. Yang Gao is forbidden.
//
// Copyright @ September 2003
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "io.h"
#include "ppp.h"
#include "ChildFrm.h"

#include <stdio.h>
#include <iomanip.h>

#include "Structure.h"

#include "ResultView.h"
#include "SatPicView.h"
#include "AzmElvView.h"
#include "RangeView.h"
#include "TrajView.h"
#include "MsgView.h"
#include "splitter.h"

#include "DlgConstants.h"
#include "DlgMet.h"
#include "DlgSatEx.h"
#include "DlgCoordinate.h"
#include "DlgOutput.h"
#include "DlgSppPost.h"
#include "DlgPppPost.h"

#include "DlgAntenna.h"

#ifndef POST_MISSION
#include "RT\DlgSppRT.h"
#include "RT\DlgPppRT.h"
#include "RT\SerialEventManager.h"
#include "RT\ReceiveNTRIPSocket.h"
#include "GlobalHeader.h"
#include "time.h"
#include "RT\dllentry.h"
#endif

#include "MainFrm.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)


/* ---------------------------------------------------------------------
CMainFrame Message Map
--------------------------------------------------------------------- */
BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(IDD_START, OnStart)
	ON_WM_DESTROY()
	ON_COMMAND(IDD_PAUSE, OnPause)
	ON_UPDATE_COMMAND_UI(IDD_PAUSE, OnUpdatePause)
	ON_COMMAND(IDD_STOP, OnStop)
	ON_UPDATE_COMMAND_UI(IDD_STOP, OnUpdateStop)
	ON_UPDATE_COMMAND_UI(IDD_START, OnUpdateStart)
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_COMMAND(ID_DRAW_VIEW, OnDrawView)
	ON_UPDATE_COMMAND_UI(ID_DRAW_VIEW, OnUpdateDrawView)
	ON_WM_TIMER()
	ON_COMMAND(ID_SETUP, OnSetup)
	ON_COMMAND(ID_SETTINGS, OnSettings)
	ON_COMMAND(ID_ANTENNA, OnAntenna)
	ON_COMMAND(ID_SAT_EXCLUSION, OnSatExclusion)
	ON_COMMAND(ID_METEOROLOGY, OnMeteorology)
	ON_COMMAND(ID_INITIAL_COOR, OnInitialCoor)
	ON_COMMAND(ID_PPP, OnPpp)
	ON_UPDATE_COMMAND_UI(ID_PPP, OnUpdatePpp)
	ON_COMMAND(ID_SPP, OnSpp)
	ON_UPDATE_COMMAND_UI(ID_SPP, OnUpdateSpp)
	ON_COMMAND(ID_POST_MISSION, OnPostMission)
	ON_UPDATE_COMMAND_UI(ID_POST_MISSION, OnUpdatePostMission)
	ON_UPDATE_COMMAND_UI(ID_REAL_TIME, OnUpdateRealTime)
	ON_COMMAND(ID_PROCESSING, OnProcessing)
	ON_COMMAND(ID_SYSTEM, OnSystem)
	ON_COMMAND(ID_REPORT, OnReport)
	ON_COMMAND(ID_DISPLAY, OnDisplay)
	ON_UPDATE_COMMAND_UI(ID_SYSTEM_STATUS, OnUpdateSystemStatus)
	ON_UPDATE_COMMAND_UI(ID_STATION_STATUS, OnUpdateStationStatus)
	ON_UPDATE_COMMAND_UI(ID_PROCESSING_STATUS, OnUpdateProcessingStatus)
	ON_UPDATE_COMMAND_UI(ID_ORBIT_STATUS, OnUpdateOrbitStatus)
	ON_UPDATE_COMMAND_UI(ID_CLOCK_STATUS, OnUpdateClockStatus)
	ON_UPDATE_COMMAND_UI(ID_CURRENT_EPOCH, OnUpdateCurrentEpoch)
	ON_UPDATE_COMMAND_UI(ID_FOR_LATER_USE, OnUpdateLaterUse)
	ON_COMMAND(ID_VIEW_AMBIGUITY, OnViewAmbiguity)
	ON_COMMAND(ID_VIEW_CLOCK, OnViewClock)
	ON_COMMAND(ID_VIEW_ERROR, OnViewError)
	ON_COMMAND(ID_VIEW_SATS, OnViewSats)
	ON_COMMAND(ID_VIEW_TRAJECTORY, OnViewTrajectory)
	ON_COMMAND(ID_VIEW_VEL, OnViewVel)
	ON_COMMAND(ID_VIEW_ZTD, OnViewZtd)
	ON_UPDATE_COMMAND_UI(ID_VIEW_AMBIGUITY, OnUpdateViewAmbiguity)
	ON_UPDATE_COMMAND_UI(ID_VIEW_CLOCK, OnUpdateViewClock)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ERROR, OnUpdateViewError)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SATS, OnUpdateViewSats)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TRAJECTORY, OnUpdateViewTrajectory)
	ON_UPDATE_COMMAND_UI(ID_VIEW_VEL, OnUpdateViewVel)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ZTD, OnUpdateViewZtd)
	ON_COMMAND(ID_SPLITTER5, OnSplitter5)
	ON_UPDATE_COMMAND_UI(ID_SPLITTER5, OnUpdateSplitter5)
	ON_COMMAND(ID_VIEW_GDOP, OnViewGdop)
	ON_UPDATE_COMMAND_UI(ID_VIEW_GDOP, OnUpdateViewGdop)
	ON_COMMAND(ID_VIEW_HDOP, OnViewHdop)
	ON_UPDATE_COMMAND_UI(ID_VIEW_HDOP, OnUpdateViewHdop)
	ON_COMMAND(ID_VIEW_PDOP, OnViewPdop)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PDOP, OnUpdateViewPdop)
	ON_COMMAND(ID_VIEW_TDOP, OnViewTdop)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TDOP, OnUpdateViewTdop)
	ON_COMMAND(ID_VIEW_VDOP, OnViewVdop)
	ON_UPDATE_COMMAND_UI(ID_VIEW_VDOP, OnUpdateViewVdop)
	ON_COMMAND(ID_PPP_HELP, OnPppHelp)
	ON_COMMAND(ID_REAL_TIME, OnRealTime)
	//}}AFX_MSG_MAP

#ifndef POST_MISSION
	ON_MESSAGE(WM_USER_OBS_READY, OnObsReady)
	ON_MESSAGE(WM_USER_EPH_READY, OnEphReady)
#endif

	// Global help commands
	ON_COMMAND(ID_HELP_FINDER, CMDIFrameWnd::OnHelpFinder)
	ON_COMMAND(ID_CONTEXT_HELP, CMDIFrameWnd::OnContextHelp)
	ON_COMMAND(ID_DEFAULT_HELP, CMDIFrameWnd::OnHelpFinder)

END_MESSAGE_MAP()

// add eventsink map to hook OnComm event
BEGIN_EVENTSINK_MAP(CMainFrame, CFrameWnd)
#ifndef POST_MISSION
	ON_EVENT(CMainFrame, ID_COMMGPS, 1 /* OnComm */, OnCommGPS, VTS_NONE)
#endif
END_EVENTSINK_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,
	ID_SYSTEM_STATUS,				// post-mission or real-time
	ID_STATION_STATUS,				// static or kinematic
	ID_PROCESSING_STATUS,			// SPP or PPP
	ID_ORBIT_STATUS,				// Broadcast, SP3, Broadcast+GPS*C, or Broadcast+JPL
	ID_CLOCK_STATUS,				// Broadcast, 5 Minute, SP3, Broadcast+GPS*C, or Broadcast+JPL
	ID_CURRENT_EPOCH,				// System time or GPS time during processing
	ID_FOR_LATER_USE,
};


/* ---------------------------------------------------------------------
Name:			CMainFrame

Purpose:		CMainFrame class object constructor.

History:		Name				Date		Reason for modification
                PMIS                2003        Original
--------------------------------------------------------------------- */
CMainFrame::CMainFrame()
{

	// initialize variables
	m_numEpoch =0;
	m_bLogPPP=0;
	m_bBackward=0;
	m_bViewDraw=0;

	// set all pointer to NULL at first, they will be initialized after the frame created
	m_pDrawView=NULL;
	m_pPicView=NULL;
	m_pResView=NULL;
	m_pAzmView=NULL;
	m_pRngView=NULL;
	m_pTrajView=NULL;
	m_pMsgView=NULL;
	m_p5Splitter=NULL;

	// there is no processing at beginning, so they are all false
	m_bStart = 0;
	m_bPause = 0;
	m_bStop  = 0;
	m_bClose = 0;

	// the following settings can be changed by menu, when initializing
	// only display errors (static) or trajectory (kinematic), satellites, ZTD and receiver clock
	m_bViewError=1;
	m_bViewTrajectory=1;
	m_bViewSats=1;
	m_bViewZTD=1;
	m_bViewAmb=0;
	m_bViewVel=0;
	m_bViewClock=1;

	// only display the PDOP
	m_bViewPDOP=1;
	m_bViewHDOP=0;
	m_bViewVDOP=0;
	m_bViewTDOP=0;
	m_bViewGDOP=0;

	// error (trajectory) + satellites (DOP) + ZTD + clock
	m_iDisplay=4;

	// enable all these charts at beginning, they will be double checked when opening log file 
	m_bEnableTraj=1;
	m_bEnableError=1;
	m_bEnableVel=1;
	m_bEnableAmb=1;

	// set the corrections, broadcast ephemeris to zero, 
	// which means there are no information available, because prn in these structs are also 0
	memset(m_precise_eph,0,sizeof(m_precise_eph));	
	memset(m_precise_clock,0,sizeof(m_precise_clock));	
	memset(m_gps_eph,0,sizeof(m_gps_eph));	


	strcpy(m_InitGPSFilename, "static_initalign_gps_file.txt");
	strcpy(m_InitIMUFilename, "static_initalign_imu_file.bin");
	strcpy(m_IMUFilename, "start_imu_file.bin");

	remove(m_InitGPSFilename);
	remove(m_InitIMUFilename);

	/*Bei added this part in Jan 2013*/
	m_IMURate = 100;

	time_t timer;
	time(&timer);
	struct tm * loctime;
	loctime = localtime(&timer);
	strcpy(gps_ins_solution_filename, asctime(loctime));
	gps_ins_solution_file.open(gps_ins_solution_filename, ios::out);

	solution_file.open("sat_clk.txt",ios::out);

	//m_EKF.set_control_parameter("RT_EKF_pub\test_2012.txt");
	/*Bei added this part in Jan 2013*/

#ifndef POST_MISSION
	m_iRcvType=0;		//0: Javad
	counter_RTCA = 0;
	strcpy(requestor, "kim, kzchen@ucalgary.ca");

	PPP_ESTIMATION_READY = false; 
	INIT_ALIGN_READY = false;

	for (int i = 0; i < NUM_SOLUTION_STATES; i++)
	{
		m_EKF_Estimation[i] = 0.0;
	}
	
#endif

}


/* ---------------------------------------------------------------------
Name:			~CMainFrame

Purpose:		CMainFrame class object destructor.

History:		Name				Date		Reason for modification
                PMIS                2003        Original
--------------------------------------------------------------------- */
CMainFrame::~CMainFrame()
{

	m_IMUComm.disconnect();

	fclose(m_IMUFile);

	// when the application is to be closed, close the log file if it is still opened
	if (m_ofslog.is_open())
		m_ofslog.close();
}


/* ---------------------------------------------------------------------
Name:			OnCreate

Purpose:		Create the main frame with selected styles.

History:		Name				Date		Reason for modification
                PMIS                2003        Original
--------------------------------------------------------------------- */
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	// first, call the OnCreate of the base class CMDIFrameWnd 
	// to create a basic multiple document interface (MDI) frame window
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// create toolbar
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	// create status bar
	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// set the style of the tool bar
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_GRIPPER | CBRS_BORDER_3D | CBRS_TOOLTIPS | CBRS_SIZE_DYNAMIC);

	// Set the text in the toolbar buttons
	m_wndToolBar.SetButtonText(0, "System");
	m_wndToolBar.SetButtonText(1, "Setup");
	m_wndToolBar.SetButtonText(2, "Process");

	m_wndToolBar.SetButtonText(4, "Display");
	m_wndToolBar.SetButtonText(5, "Report");

	m_wndToolBar.SetButtonText(7, "Run");
	m_wndToolBar.SetButtonText(8, "Pause");
	m_wndToolBar.SetButtonText(9, "Stop");

	m_wndToolBar.SetButtonText(11, "About");
	m_wndToolBar.SetButtonText(12, "Help");

	// set the size of tool bar button
	CRect rectToolBar;
	m_wndToolBar.GetItemRect(0, &rectToolBar);
	m_wndToolBar.SetSizes(rectToolBar.Size(), CSize(20,20));

	// set the ID, size of the progress bar on the tool bar  
	m_wndToolBar.SetButtonInfo(14, IDC_CALC_PROGRESS, TBBS_SEPARATOR, 2000);

	// create the progress bar
	CRect rect,rect1;
	int width=0;
	int left=400;
	GetWindowRect(rect);
	m_wndToolBar.GetItemRect(14, &rectToolBar);
	m_wndToolBar.GetItemRect(12, &rect1);
	rectToolBar.right=rect.right-50;
	rect.top = rectToolBar.top+5;
	rect.bottom = rectToolBar.top + 20;
	rect.left=rectToolBar.left+0;
	rect.right=rectToolBar.right-0;
	if (!m_ProgressCalc.Create(PBS_SMOOTH,rect, &m_wndToolBar, IDC_CALC_PROGRESS))
	{
		TRACE0("Failed to create combo-box\n");
		return FALSE;
	}

	// set the range the progress bar
	m_ProgressCalc.SetRange(0,100);

	// create a static box to show the time tag of the first epoch 
	rect.top = rectToolBar.top+22;
	rect.bottom = rect.top + 18;
	rect.left=rectToolBar.left+20;
	rect.right=rect.left+150;
	if (!m_stcBegin.Create("Beg",
			WS_TABSTOP,
			rect, &m_wndToolBar, IDC_CALC_BEGIN))
	{
		TRACE0("Failed to create combo-box\n");
		return FALSE;
	}

	// create a static box to show the time tag of the current epoch 
	rect.top = rectToolBar.top+22;
	rect.bottom = rect.top + 18;
	rect.left=(rectToolBar.left + rectToolBar.right)/2 - 75;
	rect.right=rect.left+150;
	if (!m_stcCur.Create("Cur",
			WS_TABSTOP|SS_CENTER,
			rect, &m_wndToolBar, IDC_CALC_CUR))
	{
		TRACE0("Failed to create combo-box\n");
		return FALSE;
	}

	// create a static box to show the time tag of the last epoch 
	rect.top = rectToolBar.top+22;
	rect.bottom = rect.top + 18;
	rect.left=rectToolBar.right-180;
	rect.right=rect.left+150;
	if (!m_stcEnd.Create("End",WS_TABSTOP|SS_RIGHT,rect, &m_wndToolBar, IDC_CALC_END))
	{
		TRACE0("Failed to create combo-box\n");
		return FALSE;
	}

	CFont* font=m_wndToolBar.GetFont();

	// set the font in the static boxes the same as the font in the tool bar
	if (font!=NULL)
	{
		m_stcBegin.SetFont(font,0);
		m_stcCur.SetFont(font,0);
		m_stcEnd.SetFont(font,0);
	}

	// initialize the parameters
	InitializeParameter();

	// obtain the settings from the registry table
	ReadOption();

	// set up a timer for some tasks which require to run periodically
	// the timer message will be activate every one second (1000 milliseconds)
	SetTimer(1,1000,NULL);

	// return 0, means the frame has been created normally
	return 0;
}


/* ---------------------------------------------------------------------
Name:			PreCreateWindow

Purpose:		to check if window creation is possible

Arguments:		CREATESTRUCT& cs -  structure containing initialization parameters 

History:		Name				Date		Reason for modification
                PMIS                2003        Original
--------------------------------------------------------------------- */
BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// some styles require to be set before the frame created, such as these two
	// create the main frame with with a double border but no title 
	cs.style |= WS_DLGFRAME;
	cs.style &= ~FWS_ADDTOTITLE;

	if( !CMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	cs.style |= WS_DLGFRAME;
	cs.style &= ~FWS_ADDTOTITLE;

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics
#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG


/* ---------------------------------------------------------------------
Name:			OnStart

Purpose:		This function is executed when the user begins processing
				by pressing the RUN button.

History:		Name				Date		Reason for modification
                PMIS                2003        Original
--------------------------------------------------------------------- */
void CMainFrame::OnStart()
{
	// if processing has already begun, do nothing
	if (m_bStart)
		return;

	// clear data associated with the 'trajectory' view; this view shows
	// how close the solution is to the approximate coordinates (in static
	// mode), or the trajectory of the solution (in kinematic mode)
	if (m_pTrajView)
		m_pTrajView->ClearData();

	m_bStart = 1;
	m_bStop = 0;

	// Performing conversion of initial coordinates depending on whether they are NAD83 or ITRF00,
	// and whether GPS*C products are used.
	Init_coord_NAD83toITRF();

	// Initializing unknowns associated with least-squares processing
	InitializeParameter();	

	// checking to make sure that the necessary parameters were read from the
	// IGS PCV file when the user chooses to use this file
	if(m_options.fromPCV == 1 && !GetPCV())
	{
		m_bStart = 0;
		return;
	}
	
	// checking if the log file already exists, so that it is not accidentally
	// overwritten
	if(m_options.log_over == 0 && !CheckForLOGOverwrite())
	{
		m_bStart = 0;
		return;
	}

	// clear all content in the message window
	if (m_pMsgView!=NULL)
		m_pMsgView->m_editMsg.SetWindowText("");

	// the command OpenLog() has been moved inside OnPostProcess(), because marker to ARP
	// parameters can be extracted from the observation file in post mission mode once
	// the observation file is known
	if (m_options.system_mode == 1)
	{
		// Bei add this in March 2013
		m_InitGPSFile.open(m_InitGPSFilename, ios::out);
		m_InitIMUFile = fopen(m_InitIMUFilename, "wb");
		//m_EKF.initialization_alignment(m_InitGPSFile, m_InitIMUFile, init_align_solution);
		
		m_InitGPSFile.close();
		fclose(m_InitIMUFile);
		// Bei add this in March 2013
		OnPostProcess();			// post-mission processing
	}

#ifndef POST_MISSION

	else
	{
		// real-time processing

		/*
		OpenLog();

		// Create the MSCOMM32 OLE Control to receive real-time GPS raw data from serial port
		if (!m_commGPS.Create(NULL,0,CRect(0,0,0,0),this,ID_COMMGPS))
		{
			TRACE0("Failed to create OLE Communications Control For GPS Receiver\n");
			return;
		}

		// Set RThreshold property to 1. The default RThreshold value (0)
		// causes the OnComm event to NOT fire when a character is detected
		// in the serial port. 
		m_commGPS.SetCommPort(m_iGPSPort);
		m_commGPS.SetRThreshold(1);
		// the raw data should be in binary, so set input mode to 1 to receive binary data
		m_commGPS.SetInputMode(1);
		// set input buffer enough big in case overflow of raw data in input buffer
		m_commGPS.SetInBufferSize(10240);
		// the output buffer is not a critical issue, 
		// because only several commands required to send to receiver
		m_commGPS.SetOutBufferSize(1024);
		// set the port settings
		char settingGPS[32];
		sprintf(settingGPS,"%d,n,8,1",m_iGPSBaudrate);
		m_commGPS.SetSettings(settingGPS);
		m_commGPS.SetRTSEnable(1);
		// finally, open the port after setting up it correctly
		m_commGPS.SetPortOpen(1);

		// send commands to receive to request data
		InitGPS();


		// clear all undecoded binary data in the buffer
		if (m_iRcvType==0)
		{
			m_jDecode.ClearData();
		}
		else if (m_iRcvType==1)
		{
			m_nDecode.ClearData();
		}

		//Bei added this IMU part in Jan 2013
		m_IMUComm.setRxSize(256);
		m_IMUComm.setManager(SerialEventManager);
		m_IMUComm.connect("COM3", 38400, SERIAL_PARITY_NONE, 8, true);
		//Bei added this IMU part in Jan 2013
		*/

		/*
		if (m_options.eph_flag == 2 || m_options.clock_flag == 3)
		{
			// if GPS*C corrections are selected, open socket to receive them

			// Bei add this in March 2013
			m_pSocketRTCA = new CReceiveSocket(); // Only use the ReceiveData function of CReceiveSocket
			NtripSocketConn();                    // Use Ntrip provided by Jian Ping Chen
			// Bei add this in March 2013
		}
		*/

		// Bei modified this in April 2013
		// Use sp3 file: igu eph and clock
		if (m_options.eph_flag == 100 || m_options.clock_flag == 100)
		{
			strcpy(m_options.peph_filename, "igu.sp3");

			if (!m_SP3Reader.Open(m_options.peph_filename))
			{
				MessageBox("Error opening input SP3 Precise Ephemeris File!","Precise Ephemeris File Error",MB_ICONSTOP);
				m_bStart = 0;
				m_bStop=0;
				m_bPause=0;
				return;
			}
		}
		// Bei modified this in April 2013

		/*
		if (m_options.eph_flag == 3 || m_options.clock_flag == 4)
		{
			// if IGDG corrections are selected, open socket to receive them

			char strerr[1024];
			struct hostent *hp;
			
			unsigned short portnum = 10041;//PORTNUMBER_JPL_MESG;
			int len;
			int send_err;
			unsigned char hi;
			unsigned char lo;
			
			int bind_err;
			int n;


			// information of requestor to be sent to the server
			char requestor[] = "kim, kzchen@ucalgary.ca";
			// the server to send the request
			char hostname[] = "64.239.128.170";

			// retrieve host information corresponding to the host name 
			if ( (hp = gethostbyname(hostname)) == NULL ) 
			{
				sprintf( strerr, "unknown host: %s\n", hostname );
				return;
			}
			// create the socket according to the port number selected
			m_pSocketRTCA = new CReceiveSocket();
			if (!m_pSocketRTCA->Create(portnum,SOCK_DGRAM))
			{
				delete m_pSocketRTCA;
				m_pSocketRTCA = NULL;
				return;
			}
			
			int to=1;
			
			// Clear the Internet name parameter. 
			memset( &name, 0, sizeof(struct sockaddr_in) );
			
			// Name the socket. 
			name.sin_family = AF_INET;   // Internet Domain /
			// Set the portnumber 

#if defined(WIN32) || defined(_linux)
			//swap bytes for the portnumber 
			hi = (portnum & 0xff00) >> 8;
			lo = (portnum & 0x00ff);
			name.sin_port = (lo << 8) + hi;
#else
			name.sin_port = portnum;
#endif
			len = sizeof(struct sockaddr_in);
			
			n = INADDR_ANY;
			memcpy( &name.sin_addr, &n, sizeof(long) );
			
			// bind the address to the socket 
			bind_err = m_pSocketRTCA->Bind((struct sockaddr *) &name, len);
			if ( bind_err < 0 ) {
				delete m_pSocketRTCA;
				m_pSocketRTCA = NULL;
				return;
			}
			
			memcpy( &name.sin_addr, hp->h_addr_list[0], hp->h_length );
	        sin_addr_now = name.sin_addr;
			
			//Send a request to start the data flow. 
			send_err = m_pSocketRTCA->SendTo(requestor, strlen(requestor),
				(struct sockaddr *) &name, len );
			
			if ( send_err < 0 ) 
			{
				delete m_pSocketRTCA;
				m_pSocketRTCA = NULL;
				return;
			}
		}
		*/
	}
#endif

}


/* ---------------------------------------------------------------------
Name:			InitializeParameter

Purpose:		To initialize unknowns for least-square processing.

History:		Name				Date		Reason for modification
                PMIS                2003        Original
--------------------------------------------------------------------- */
void CMainFrame::InitializeParameter()
{
	m_bBackward=0;
	// initialize memory 
	m_epoch.InitEpoch();
	// pass the options set by the user to the Epoch
	m_epoch.SetOptions(&m_options);

	// Convert ellipsoidal coordinates to Cartesian coordinates if necessary
	XYZ_POS	inicoor = m_options.CalRexCoor();
	m_parameter_ini.station_coor.x = inicoor.x;
	m_parameter_ini.station_coor.y = inicoor.y;
	m_parameter_ini.station_coor.z = inicoor.z;
	m_parameter_ini.clock_cor	= 0.0;

	m_parameter_ini.station_plh = m_options.station.m_stPLH;

	// calculate an initial value for zenith tropospheric delay based on met settings
	TROPOSPHERE trop=m_options.InitializeTrop();
	m_parameter_ini.wetZen	= trop.wetZen;

	// Pass the necessary data to CEpoch object m_epoch
	m_epoch.SetParameter(m_parameter_ini);
	m_epoch.SetGPSEph(m_gps_eph);
	m_epoch.SetPreciseData(m_precise_eph,m_precise_clock);

	// calculate the measurement noises of ionosphere-free combinations based on the 
	// noises of code and carrier phase measurements 
	m_options.sigmaCP			=	(m_options.sigmaP1+m_options.sigmaL1)/2;
	m_options.sigmaIonoFreeCode	=	m_options.sigmaP1*3;
	m_options.sigmaIonoFreePhase=	m_options.sigmaL1*3;

	if (m_options.ambiguity_flag==0)
	{
		if (m_options.frequency_flag!=0)
		{
			m_options.sigmaIonoFreeCode	=	m_options.sigmaP1*3;
		}
	}
	else
	{
		if (m_options.iono_free_modal==1)
		{
			m_options.sigmaIonoFreeCode	=	(m_options.sigmaP1+m_options.sigmaL1)/2;
		}
	}
}


/* ---------------------------------------------------------------------
Name:			OpenLog

Purpose:		To write header information to LOG file.

History:		Name				Date		Reason for modification
                PMIS                2003        Original
--------------------------------------------------------------------- */
void CMainFrame::OpenLog()
{
	// opening LOG file
	m_ofslog.open(m_options.logfile);

	char str[200],tstr[200];

	int width = 31;

	// output station mode
	sprintf(str,"%*s",width,"Station Mode : ");
	m_ofslog<<str;
	if (m_options.station_mode==0)
		m_ofslog<<"Static\n";
	else
		m_ofslog<<"Kinematic\n";
	
	// initial coordinate system is Cartesian
	if(m_options.ref == 0)
	{
		PLH_POS plh;
		XYZ_POS xyz;

		// storing initial Cartesian coords
		if(m_options.init_ref_sys == 2)		// NAD83 initial coords
		{
			xyz.x = m_options.origNADa;
			xyz.y = m_options.origNADb;
			xyz.z = m_options.origNADc;
		}
		else	// ITRF00 initial coords
		{
			xyz.x = m_options.direca;
			xyz.y = m_options.direcb;
			xyz.z = m_options.direcc;
		}

		// transforming to lat (rad), long (rad), height
		if(m_options.init_ref_sys == 2)
			plh = xyz.toPLH(m_options.datum[2]);
		else
			plh = xyz.toPLH(m_options.datum[0]);

		// converting latitude and longitude from radians to decimal degrees
		plh.phi *= 180/PI;
		plh.lambda *= 180/PI;

		// converting decimal degree latitude and longitude to deg, min, sec
		DMS lat, lon;
		lat.setDeg(plh.phi);
		lon.setDeg(plh.lambda);

		// output PLH coordinates
		sprintf(str,"%*s",width,"Initial PLH (DDMMSS.SS) : ");
		m_ofslog<<str;
		sprintf(str,"%.5lf  %.5lf  %.5lf\n", lat.OutputDMS(), lon.OutputDMS(), plh.h);
		m_ofslog<<str;
		
		// output XYZ coordinates
		sprintf(str,"%*s",width,"Initial XYZ (m) : ");
		m_ofslog<<str;
		if(m_options.init_ref_sys == 2) // outputting NAD83 XYZ coords
			sprintf(str,"%.5lf  %.5lf  %.5lf\n",m_options.origNADa,m_options.origNADb,m_options.origNADc);
		else // outputting ITRF00 XYZ coords
			sprintf(str,"%.5lf  %.5lf  %.5lf\n",m_options.direca,m_options.direcb,m_options.direcc);
		m_ofslog<<str;
	}

	// initial coordinate system is Ellipsoidal
	else
	{
		PLH_POS plh;
		XYZ_POS xyz;

		// converting latitude and longitude from DDMMSS.SSSSS format to deg, min, sec format
		DMS lat, lon;

		if(m_options.init_ref_sys == 2)		// NAD83 initial coords
		{
			lat.InputDMS(m_options.origNADa);
			lon.InputDMS(m_options.origNADb);
			plh.h = m_options.origNADc;
		}
		else	// ITRF00 initial coords
		{
			lat.InputDMS(m_options.direca);
			lon.InputDMS(m_options.direcb);
			plh.h = m_options.direcc;
		}

		// converting deg, min, sec format to decimal degrees and storing height
		plh.phi = lat.getDeg();
		plh.lambda = lon.getDeg();

		// converting latitude and longitude from decimal degrees to radians
		plh.phi *= PI/180;
		plh.lambda *= PI/180;

		// converting latitude (rad), longitude (rad) and height to XYZ
		if(m_options.init_ref_sys == 2)
			plh.toXYZ(xyz.x, xyz.y, xyz.z, m_options.datum[2]);
		else
			plh.toXYZ(xyz.x, xyz.y, xyz.z, m_options.datum[0]);

		// output PLH and XYZ coordinates
		sprintf(str,"%*s",width,"Initial PLH (DDMMSS.SS) : ");
		m_ofslog<<str;
		if(m_options.init_ref_sys == 2) // outputting NAD83 XYZ coords
			sprintf(str,"%.5lf  %.5lf  %.5lf\n",m_options.origNADa,m_options.origNADb,m_options.origNADc);
		else // outputting ITRF00 XYZ coords
			sprintf(str,"%.5lf  %.5lf  %.5lf\n",m_options.direca,m_options.direcb,m_options.direcc);
		m_ofslog<<str;
		
		sprintf(str,"%*s",width,"Initial XYZ (m) : ");
		m_ofslog<<str;
		sprintf(str,"%.5lf  %.5lf  %.5lf\n",xyz.x,xyz.y,xyz.z);
		m_ofslog<<str;
	}

	// output initial coordinate system
	sprintf(str,"%*s",width,"Initial Coordinate System : ");
	m_ofslog<<str;
	if (m_options.init_ref_sys==0)
		m_ofslog<<"WGS84\n";
	else if (m_options.init_ref_sys==1)
		m_ofslog<<"ITRF00\n";
	else
		m_ofslog<<"NAD83\n";

	// output the time of the initial coordinates
	sprintf(str,"%*s",width,"Coord. Date (yyyy mm dd) : ");
	m_ofslog<<str;
	m_ofslog << m_options.coord_year << " " << m_options.coord_month << " "
		<< m_options.coord_day << endl;
	
	// output system mode, either real-time or post mission
	sprintf(str,"%*s",width,"System Mode : ");
	m_ofslog<<str;
	if (m_options.system_mode==0)
		m_ofslog<<"Real Time\n";
	else
		m_ofslog<<"Post Mission\n";

	// output processing mode, either SPP or PPP
	sprintf(str,"%*s",width,"Processing Mode : ");
	m_ofslog<<str;
	if (m_options.processing_mode==0)
		m_ofslog<<"SPP\n";
	else
		m_ofslog<<"PPP\n";

	// output filter model, either Gauss-Markov or Random Walk (in kinematic mode only)
	if (m_options.station_mode==1)
	{
		sprintf(str,"%*s",width,"Filter Model : ");
		m_ofslog<<str;
		if (m_options.filter_model==0)
			m_ofslog<<"First Order Gauss_Markov\n";
		else
			m_ofslog<<"Random Walk\n";
	}

	// output satellite orbit type
	sprintf(str,"%*s",width,"Satellite Orbit : ");
	m_ofslog<<str;
	if (m_options.eph_flag==0)
		m_ofslog<<"Broadcast\n";
	else if (m_options.eph_flag==1)
		m_ofslog<<"SP3\n";
	else if (m_options.eph_flag==2)
		m_ofslog<<"GPS*C\n";
	else if (m_options.eph_flag==3)
		m_ofslog<<"JPL\n";

	// output satellite clock type
	sprintf(str,"%*s",width,"Satellite Clock : ");
	m_ofslog<<str;
	if (m_options.clock_flag==0)
		m_ofslog<<"Broadcast\n";
	else if (m_options.clock_flag==1)
		m_ofslog<<"5 Minutes\n";
	else if (m_options.clock_flag==2)
		m_ofslog<<"SP3\n";
	else if (m_options.clock_flag==3)
		m_ofslog<<"GPS*C\n";
	else if (m_options.clock_flag==4)
		m_ofslog<<"JPL\n";

	// output result coordinate system
	sprintf(str,"%*s",width,"Result Coordinate System : ");
	m_ofslog<<str;
	if (m_options.eph_flag==1 || m_options.clock_flag==1 || m_options.clock_flag==2)
		m_ofslog<<"ITRF00\n";
	else if (m_options.eph_flag==2 || m_options.clock_flag==3)
		m_ofslog<<"NAD83\n";
	else //if (m_options.eph_flag==0 && m_options.clock_flag==0)
		m_ofslog<<"WGS84\n";

	// output file names (in post mission mode only)
	if (m_options.system_mode==1)
	{
		sprintf(str,"%*s",width,"Observation File : ");
		m_ofslog<<str;
		m_ofslog<<m_options.obs_filename<<"\n";
		
		sprintf(str,"%*s",width,"Broadcast Ephemeris File : ");
		m_ofslog<<str;
		m_ofslog<<m_options.eph_filename<<"\n";
		
		if (m_options.eph_flag!=0)
		{
			sprintf(str,"%*s",width,"Precise Ephemeris File : ");
			m_ofslog<<str;
			m_ofslog<<m_options.peph_filename<<"\n";
		}

		if (m_options.clock_flag!=0 && (!(m_options.eph_flag==1 && m_options.clock_flag==2)))
		{
			sprintf(str,"%*s",width,"Precise Clock File : ");
			m_ofslog<<str;
			m_ofslog<<m_options.pclk_filename<<"\n";
		}
	}

	// output PCV filename (if used in the program)
	if (m_options.fromPCV == 1)
	{
		sprintf(str,"%*s",width,"Antenna PCV File : ");
		m_ofslog<<str;
		m_ofslog<<m_options.pcvfile<<"\n";
	}

	// output troposphere option, either model or estimate
	sprintf(str,"%*s",width,"Troposphere : ");
	m_ofslog<<str;
	if (m_options.tropo_flag==0)
		m_ofslog<<"Model\n";
	else
		m_ofslog<<"Estimate\n";

	// output estimate velocity option
	sprintf(str,"%*s",width,"Estimate Velocity : ");
	m_ofslog<<str;
	if (m_options.vel_estimate==0)
		m_ofslog<<"No\n";
	else
		m_ofslog<<"Yes\n";

	// output estimate ambiguity option
	sprintf(str,"%*s",width,"Estimate Ambiguity : ");
	m_ofslog<<str;
	if (m_options.ambiguity_flag==0)
		m_ofslog<<"No\n";
	else
		m_ofslog<<"Yes\n";

	// output ambiguity model, either Traditional or UofC
	if (m_options.ambiguity_flag==1)
	{
		sprintf(str,"%*s",width,"Ambiguity Model : ");
		m_ofslog<<str;
		if (m_options.iono_free_modal==0)
			m_ofslog<<"Traditional\n";
		else
			m_ofslog<<"UofC\n";
	}

	// output used frequencies (in SPP mode only)
	if (m_options.processing_mode==0)
	{
		sprintf(str,"%*s",width,"Frequency Used : ");
		m_ofslog<<str;
		if (m_options.frequency_flag==0)
			m_ofslog<<"L1 & L2\n";
		else
			m_ofslog<<"L1 Only\n";
	}
	
	// output phase smoothing option
	sprintf(str,"%*s",width,"Phase Smooth : ");
	m_ofslog<<str;
	if (m_options.smooth_flag==0)
		m_ofslog<<"No\n";
	else
		m_ofslog<<"Yes\n";
	
#ifndef POST_MISSION

	// output receiver type (in real-time mode only)
	if (m_options.system_mode==0)
	{
		//Javad Trimble Novatel Leica Ashtech
		sprintf(str,"%*s",width,"Receiver Type : ");
		m_ofslog<<str;
		if (m_iRcvType==0)
			m_ofslog<<"Javad\n";
		else if (m_iRcvType==1)
			m_ofslog<<"Trimble\n";
		else if (m_iRcvType==2)
			m_ofslog<<"Novatel\n";
		else if (m_iRcvType==3)
			m_ofslog<<"Leica\n";
		else if (m_iRcvType==4)
			m_ofslog<<"Ashtech\n";
		else
			m_ofslog<<"No Name\n";
	}

#endif

	// output antenna vendor and type when the PCV file is to be used
	if(m_options.fromPCV == 1)
	{
		sprintf(str,"%*s",width-1,"Antenna Vendor/Type :");
		m_ofslog<<str;
		sprintf(str," %s\n",m_options.ant_type);
		m_ofslog<<str;
	}

	// output ARP to Marker components
	sprintf(str,"%*s",width,"Marker to ARP E/N/H (m) : ");
	m_ofslog<<str;
	sprintf(str,"%.4lf    ",m_options.arp2mark_east);
	m_ofslog<<str;
	sprintf(str,"%.4lf    ",m_options.arp2mark_north);
	m_ofslog<<str;
	sprintf(str,"%.4lf\n",m_options.arp2mark_up);
	m_ofslog<<str;

	// output ARP to APC components
	sprintf(str,"%*s",width,"ARP to APC E/N/H (m) : ");
	m_ofslog<<str;
	sprintf(str,"%.4lf    ",m_options.arp2apc_east);
	m_ofslog<<str;
	sprintf(str,"%.4lf    ",m_options.arp2apc_north);
	m_ofslog<<str;
	sprintf(str,"%.4lf\n",m_options.arp2apc_up);
	m_ofslog<<str;

	// output antenna PCV corrections option if specified by the user (PPP mode only)
	if(m_options.processing_mode == 1)
	{
		sprintf(str,"%*s",width,"Antenna PCV Corrections : ");
		m_ofslog<<str;
		if (m_options.pcv_mode==0)
			m_ofslog<<"No\n";
		else
			m_ofslog<<"Yes\n";
	}

	// output GPS UTC offset
	sprintf(str,"%*s",width,"GPS UTC Offset (s) : ");
	m_ofslog<<str;
	sprintf(str,"%d\n",m_options.GPS_UTC_offset);
	m_ofslog<<str;

	// output observation interval
	sprintf(str,"%*s",width,"Observation Interval (s) : ");
	m_ofslog<<str;
	sprintf(str,"%.1lf\n",m_options.obs_interval);
	m_ofslog<<str;

	// output BLOCK II/IIA satellites
	sprintf(str,"%*s",width,"BLOCK II/IIA Satellites : ");
	m_ofslog<<str;
	tstr[0]=0;
	str[0]=0;
	for (int i=0;i<32;i++)
	{
		if (m_options.BLOCKII[i]==1)
		{
			sprintf(tstr,"%d ",i+1);
			strcat(str,tstr);
			if (i+1<10)
				strcat(str," ");
		}
	}
	m_ofslog<<str<<"\n";

	// output excluded satellites
	sprintf(str,"%*s",width,"Excluded Satellites : ");
	m_ofslog<<str;
	tstr[0]=0;
	str[0]=0;
	for (i=0;i<32;i++)
	{
		if (m_options.SatEx[i]==1)
		{
			sprintf(tstr,"%d ",i+1);
			strcat(str,tstr);
			if (i+1<10)
				strcat(str," ");
		}
	}
	m_ofslog<<str<<"\n";

	// output temperature
	sprintf(str,"%*s",width,"Temperature (degree Celsius) : ");
	m_ofslog<<str;
	sprintf(str,"%.1lf\n",m_options.temperature);
	m_ofslog<<str;

	// output pressure
	sprintf(str,"%*s",width,"Pressure (mbar) : ");
	m_ofslog<<str;
	sprintf(str,"%.1lf\n",m_options.pressure);
	m_ofslog<<str;

	// output humidity
	sprintf(str,"%*s",width,"Humidity (%) : ");
	m_ofslog<<str;
	sprintf(str,"%.1lf\n",m_options.humidity);
	m_ofslog<<str;

	// output elevation cutoff angle
	sprintf(str,"%*s",width,"Elevation Cut-Off (degree) : ");
	m_ofslog<<str;
	sprintf(str,"%.1lf\n",m_options.elev_mask);
	m_ofslog<<str;

	// output GDOP cutoff
	sprintf(str,"%*s",width,"GDOP Cut-Off : ");
	m_ofslog<<str;
	sprintf(str,"%.1lf\n",m_options.GDOP_threshold);
	m_ofslog<<str;

	// output apriori code standard deviation
	sprintf(str,"%*s",width,"Apriori Code STD (m) : ");
	m_ofslog<<str;
	if (m_options.frequency_flag!=0)
		sprintf(str,"%.3lf\n",m_options.sigmaP1);
	else
		sprintf(str,"%.3lf\n",m_options.sigmaIonoFreeCode);
	m_ofslog<<str;

	// output apriori phase standard deviation
	sprintf(str,"%*s",width,"Apriori Phase STD (m) : ");
	m_ofslog<<str;
	if (m_options.frequency_flag!=0)
		sprintf(str,"%.3lf\n",m_options.sigmaL1);
	else
		sprintf(str,"%.3lf\n",m_options.sigmaIonoFreePhase);
	m_ofslog<<str;

	// output apriori phase rate standard deviation
	sprintf(str,"%*s",width,"Apriori Phase Rate STD (m/s) : ");
	m_ofslog<<str;
	sprintf(str,"%.3lf\n",m_options.sigmaPhaseRate);
	m_ofslog<<str;

	// output backward processing option (only when SP3 orbits and either SP3 or 5 minute
	// clocks are used)
	if (m_options.eph_flag == 1 && (m_options.clock_flag == 1 || m_options.clock_flag == 2))
	{
		sprintf(str,"%*s",width,"Backward Processing : ");
		m_ofslog<<str;
		if (m_options.bBackward == 0)
			m_ofslog<<"No\n";
		else
			m_ofslog<<"Yes\n";
	}

	int width2 = 10;
	
	// output filter standard deviations and spectral densities
	sprintf(str,"%*s\n",width,"Initial STD/Spectral Density   ");
	m_ofslog<<str;
	
	// output STD and SD for horizontal component
	sprintf(str,"%*s",width,"Horizontal : ");
	m_ofslog<<str;
	sprintf(str,"%*.0e m",width2-4,m_options.init_SD_hor);
	m_ofslog<<str;
	sprintf(str,"%*.0e m^2/s\n",width2+2,m_options.spec_den_hor);
	m_ofslog<<str;
	
	// output STD and SD for vertical component
	sprintf(str,"%*s",width,"Vertical : ");
	m_ofslog<<str;
	sprintf(str,"%*.0e m",width2-4,m_options.init_SD_ver);
	m_ofslog<<str;
	sprintf(str,"%*.0e m^2/s\n",width2+2,m_options.spec_den_ver);
	m_ofslog<<str;
	
	// output STD and SD for receiver clock offset
	sprintf(str,"%*s",width,"Clock Offset : ");
	m_ofslog<<str;
	sprintf(str,"%*.0e m",width2-4,m_options.init_SD_clk);
	m_ofslog<<str;
	sprintf(str,"%*.0e m^2/s\n",width2+2,m_options.spec_den_clk);
	m_ofslog<<str;
	
	// output STD and SD for tropospheric parameter if it will be estimated
	if (m_options.tropo_flag==1)
	{
		sprintf(str,"%*s",width,"Troposphere : ");
		m_ofslog<<str;
		sprintf(str,"%*.0e m",width2-4,m_options.init_SD_trop);
		m_ofslog<<str;
		sprintf(str,"%*.0e m^2/s\n",width2+2,m_options.spec_den_trop);
		m_ofslog<<str;
	}
	
	// output STD and SD for ambiguity in PPP mode
	if (m_options.processing_mode==1)
	{
		sprintf(str,"%*s",width,"Ambiguity : ");
		m_ofslog<<str;
		sprintf(str,"%*.0e m",width2-4,m_options.init_SD_amb);
		m_ofslog<<str;
		sprintf(str,"%*.0e m^2/s\n",width2+2,m_options.spec_den_amb);
		m_ofslog<<str;
	}
	
	// output STD and SD for horizontal velocity, vertical velocity
	// and clock drift when velocity estimation has been selected
	if (m_options.vel_estimate == 1)
	{
		sprintf(str,"%*s",width,"Horizontal Velocity : ");
		m_ofslog<<str;
		sprintf(str,"%*.0e m/s",width2-4,m_options.init_SD_vh);
		m_ofslog<<str;
		sprintf(str,"%*.0e m^2/s^3\n",width2,m_options.spec_den_vh);
		m_ofslog<<str;
		
		sprintf(str,"%*s",width,"Vertical Velocity : ");
		m_ofslog<<str;
		sprintf(str,"%*.0e m/s",width2-4,m_options.init_SD_vv);
		m_ofslog<<str;
		sprintf(str,"%*.0e m^2/s^3\n",width2,m_options.spec_den_vv);
		m_ofslog<<str;
		
		sprintf(str,"%*s",width,"Clock Drift : ");
		m_ofslog<<str;
		sprintf(str,"%*.0e m/s",width2-4,m_options.init_SD_dt);
		m_ofslog<<str;
		sprintf(str,"%*.0e m^2/s^3\n",width2,m_options.spec_den_dt);
		m_ofslog<<str;
	}

	// output "END OF HEADER" string
	sprintf(str,"%*s",width+14,"END OF HEADER\n");
	m_ofslog<<str;
}


/* ---------------------------------------------------------------------
Name:			DoEvents

Purpose:		To process existing messages in the application's message
				queue. When the queue is empty, do clean up and return.

History:		Name				Date		Reason for modification
                PMIS                2003        Original
--------------------------------------------------------------------- */
void CMainFrame::DoEvents()
{
	MSG msg;

	while (::PeekMessage(&msg,NULL,0,0,PM_NOREMOVE))
	{
		if (!AfxGetThread()->PumpMessage())
			return;
	}
}


/* ---------------------------------------------------------------------
Name:			Delay

Purpose:		Delay a task for a period but do not affect message processing

Arguments:		long sec - Time for which the delay is to be implemented.

History:		Name				Date		Reason for modification
                PMIS                2003        Original
--------------------------------------------------------------------- */
void CMainFrame::Delay(long sec)
{
    CTime t;
	t = CTime::GetCurrentTime();
    do
       DoEvents();
    while ((CTime::GetCurrentTime()-t).GetTotalSeconds()<sec);
}

#ifndef POST_MISSION

/* ---------------------------------------------------------------------
Name:			InitGPS

Purpose:		Send commands to GPS receivers based on the settings

Arguments:		

History:		Name				Date		Reason for modification
                PMIS                2003        Original
--------------------------------------------------------------------- */
void CMainFrame::InitGPS() 
{
	// TODO: Add your control notification handler code here
 
	// if serial still close, open it
	if (!m_commGPS.GetPortOpen())
		m_commGPS.SetPortOpen(1);

	// if serial port can not be opened, requests can not be sent out, just return
	if (!m_commGPS.GetPortOpen())
		return;

	// for Javad receiver
	if (m_iRcvType==0)
	{
		char strCom[15][15]=
		{
			"%%dm",			 		//1
			"%%em,,jps/SI:1",		//2
			"%%em,,jps/GT:1",		//3
			"%%em,,jps/RC:1",		//4
			"%%em,,jps/R1:1",		//5
			"%%em,,jps/R2:1",		//6
			"%%em,,jps/P1:1",		//7
			"%%em,,jps/P2:1",		//8
			"%%em,,jps/D1:1",		//9		
			"%%em,,jps/D2:1",		//10		
			"%%em,,jps/GE:1",		//11
			"%%em,,jps/EE:1",		//12
		};


		CString str;

		for (int i=0;i<12;i++)
		{
			str=CString(strCom[i])+CString("\n");
			m_commGPS.SetOutput(COleVariant(str));
		}
	}	

	// for Novatel receiver
	if (m_iRcvType==1)
	{
		char strCom[5][128]=
		{
			"unlogall com1\r"					//1
			"log com1 rawephemb once\r",		//2
			"log com1 rangeb ontime 1.0\r",		//3
			"log com1 rawephemb onchanged\r",	//4
		};

		CString str;

		for (int i=0;i<4;i++)
		{
			str=CString(strCom[i]);
			m_commGPS.SetOutput(COleVariant(str));
		}
	}	

}

/* ---------------------------------------------------------------------
Name:			OnCommGPS

Purpose:		Any data arrive at or to be sent out at the serial port will activate 
				this function if the serial port is opened

Arguments:		

History:		Name				Date		Reason for modification
                PMIS                2003        Original
--------------------------------------------------------------------- */
void CMainFrame::OnCommGPS()
{
	switch(m_commGPS.GetCommEvent())
	{
		case 2: // data arrive, process the data
			ReceiveGPS(m_commGPS.GetInput());
			break;
	}

}


/* ---------------------------------------------------------------------
Name:			ReceiveGPS

Purpose:		process the received data

Arguments:		VARIANT pchData - binary data received

History:		Name				Date		Reason for modification
                PMIS                2003        Original
--------------------------------------------------------------------- */
void CMainFrame::ReceiveGPS(VARIANT pchData)
{
	HRESULT hresult;
	VARTYPE vt;
	BYTE* pbytes;
	SAFEARRAY* psa;
	long l,u;
	int m_iPos=0;


	// to determine if the data format is a binary array
	vt = pchData.vt;
	if(vt != (VT_ARRAY | VT_UI1))
	{
		return;
	}

	// get a pointer to the array
	psa = pchData.parray;

	// increments the lock count of the array, and retrieves a pointer to the array data
	SafeArrayAccessData(psa,  (void**)&pbytes); 

	// get the lower bound of the array
	hresult = SafeArrayGetLBound(psa, 1, &l);
	if(FAILED(hresult))
		return;
	// get the upper bound of the array
	hresult = SafeArrayGetUBound(psa, 1, &u);
	if(FAILED(hresult))
		return ;

	// decode data for Javad receiver
	if (m_iRcvType==0)
		m_jDecode.ReceiveData(&pbytes[l],u-l+1);
	// decode data for Novatel receiver
	else if (m_iRcvType==1)
		m_nDecode.ReceiveData(&pbytes[l],u-l+1);

	// after decoding, decrements the lock count of the array
	SafeArrayUnaccessData(psa);

}


/* ---------------------------------------------------------------------
Name:			ReceiveRTCA

Purpose:		process the real-time corrections received over the Internet

Arguments:		

History:		Name				Date		Reason for modification
                PMIS                2003        Original
--------------------------------------------------------------------- */
void CMainFrame::ReceiveRTCA()
{
	BYTE bs[256];
	memcpy(bs, RTCAbuff, RTCAdatalen*sizeof(BYTE));


	BYTE decoded_msg[1024];

	int len = Decrypt((char*)bs,RTCAdatalen, (char*)decoded_msg,1024);

	int idx = 0;

	bool isNRCan = true;

	while (idx+7 < len)
	{
		int numofmessage = 0;
		if (decoded_msg[idx] == 0x00)
			if (decoded_msg[idx+1] == 0x20)
			{
				if (decoded_msg[idx+2] == 0x00)
					if (decoded_msg[idx+3] == 0x15)
						if (decoded_msg[idx+4] == 0x00)
							if (decoded_msg[idx+5] == 0x03)
								if (decoded_msg[idx+6] == 0x00)
									if (decoded_msg[idx+7] == 0x60)
									{
										numofmessage = decoded_msg[idx+5];
									}
			}
			else if (decoded_msg[idx+1] == 0x0c)
			{
				if (decoded_msg[idx+2] == 0x00)
					if (decoded_msg[idx+3] == 0x15)
						if (decoded_msg[idx+4] == 0x00)
							if (decoded_msg[idx+5] == 0x01)
								if (decoded_msg[idx+6] == 0x00)
									if (decoded_msg[idx+7] == 0x20)
									{
										numofmessage = decoded_msg[idx+5];
									}
			}
			else if (decoded_msg[idx+1] == 0x01)
			{
				if (decoded_msg[idx+2] == 0x00)
					if (decoded_msg[idx+3] == 0x15)
						if (decoded_msg[idx+4] == 0x00)
							if (decoded_msg[idx+5] == 0x01)
								if (decoded_msg[idx+6] == 0x00)
									if (decoded_msg[idx+7] == 0x20)
									{
										numofmessage = decoded_msg[5+idx];
									}
			}
			else if (decoded_msg[idx+1] == 0x2d)
			{
				if (decoded_msg[idx+2] == 0x00)
					if (decoded_msg[idx+3] == 0x15)
						if (decoded_msg[idx+4] == 0x00)
						{
							//because the slow correction can have slightly different sizes
							//compare that the number of elements*32 bytes and the total message size in bytes are equal
							int numelems = decoded_msg[idx+5];
							int numbytes = decoded_msg[idx+6]*256+decoded_msg[idx+7];
							if (numelems*32 == numbytes)
							{
								numofmessage = decoded_msg[5+idx];
							}
						}
			}

			if (numofmessage > 0)
			{
				if (isNRCan)
					for (int i = 0; i < numofmessage; i++)
					{
						m_rDecode.ReceiveData(&decoded_msg[8+i*32+idx], 32);
					}
					idx += 10 + numofmessage*32;

			}
			else
			{
				idx++;
			}
	}

	//After decode, set precise data
	//m_epoch.SetPreciseData(eph, clk);

	/*
	BYTE bs[256];

	if (this->m_options.eph_flag==2 || this->m_options.clock_flag==3)
	{
		// for GPS*C corrections, the messages are encapsulated into a 106 bytes packet
		int rc=m_pSocketRTCA->Receive(bs,106);

		if (rc!=106)
		{
			return;
		}
		// one 106 bytes packet contains 3 32-byte messages
		// decode the messages separately
		m_rDecode.ReceiveData(&bs[8],32);	
		m_rDecode.ReceiveData(&bs[40],32);	
		m_rDecode.ReceiveData(&bs[72],32);	
	}

	if (this->m_options.eph_flag==3 || this->m_options.clock_flag==4)
	{
		// for IGDG corrections, the message is 44 bytes per packet

		int rc=m_pSocketRTCA->Receive(bs,44);

		// decode the data if the size is correct
		if (rc==44)
		{
			m_jplDecode.jplGdGPS_Decode( 44, bs);
		}

		// the server of IGDG just sends out 128 message for a request
		// send another request after receiving 127 message to avoid gap between message
		if (counter_RTCA++>100)
		{
			int send_err = m_pSocketRTCA->SendTo(requestor, strlen(requestor),
				(struct sockaddr *) &name, sizeof(struct sockaddr_in) );
			// after sending out a request, reset the counter
			counter_RTCA=0;
		}

	}
	*/
}

/* ---------------------------------------------------------------------
Name:			OnObsReady

Purpose:		this function is activated by the GPS raw data decode procedure,
				indicates a full epoch data has been decoded

Arguments:		WPARAM wParam and LPARAM lParam are used to
				store information that is important to handling the message

History:		Name				Date		Reason for modification
                PMIS                2003        Original
--------------------------------------------------------------------- */
LRESULT CMainFrame::OnObsReady(WPARAM wParam, LPARAM lParam)
{

	// if an epoch of GPS data has been decoded,  
	// process it if user has started a real-time processing
	if (!m_bPause && m_bStart && m_options.system_mode==0)
	{
		// conduct Kalman filter
		m_epoch.DoKF();

		double m_GPSTime = m_epoch.GetCGPSTime().GetGPSTime().secsOfWeek;
		double m_PPPphi  = m_epoch.GetCoor()->m_stPLH.phi;
		double m_PPPlambda = m_epoch.GetCoor()->m_stPLH.lambda;
		double m_PPPh    = m_epoch.GetCoor()->m_stPLH.h;
		double m_PPPnV   = m_epoch.GetParameter()->station_vel.phi;
		double m_PPPeV   = m_epoch.GetParameter()->station_vel.lambda;
		double m_PPPdV   = m_epoch.GetParameter()->station_vel.h;
		double m_PPPsigmaphi = m_epoch.GetParameter()->sigma_plh.phi;
		double m_PPPsigmalambda = m_epoch.GetParameter()->sigma_plh.lambda;
		double m_PPPsigmah  = m_epoch.GetParameter()->sigma_plh.h;
		double m_PPPsigmanV  = m_epoch.GetParameter()->sigma_vel.phi;
		double m_PPPsigmaeV  = m_epoch.GetParameter()->sigma_vel.lambda;
		double m_PPPsigmadV  = m_epoch.GetParameter()->sigma_vel.h;

//		PPP_ESTIMATION_READY = !m_epoch.noestimation;

		if (PPP_ESTIMATION_READY)
		{
			/******************************************************/
			/*******save static files for initial alignment********/
			/******************************************************/
			if (INIT_ALIGN_READY)
			{
				PPP_GPSTime.push_back(m_GPSTime);
				PPP_phi.push_back(m_PPPphi);
				PPP_lambda.push_back(m_PPPlambda);
				PPP_h.push_back(m_PPPh);
				PPP_nV.push_back(m_PPPnV);
				PPP_eV.push_back(m_PPPeV);
				PPP_dV.push_back(m_PPPdV);
				PPP_sigmaphi.push_back(m_PPPsigmaphi);
				PPP_sigmalambda.push_back(m_PPPsigmalambda);
				PPP_sigmah.push_back(m_PPPsigmah);
				PPP_sigmanV.push_back(m_PPPsigmanV);
				PPP_sigmaeV.push_back(m_PPPsigmaeV);
				PPP_sigmadV.push_back(m_PPPsigmadV);
			}
			else if(!INIT_ALIGN_READY && m_InitGPSFile.is_open())
			{
				double gps_data[] = {m_GPSTime, 
					m_PPPphi, m_PPPlambda, m_PPPh, 
					m_PPPsigmaphi, m_PPPsigmalambda, m_PPPsigmah, 
					m_PPPnV, m_PPPeV, m_PPPdV,
					m_PPPsigmanV, m_PPPsigmaeV, m_PPPsigmadV};

				m_InitGPSFile<<gps_data[0]<<" "<<gps_data[1]<<" "<<gps_data[2]<<" "<<gps_data[3]<<" "<<gps_data[4]<<" "<<gps_data[5]<<" "<<gps_data[6]<<" "<<gps_data[7]<<" "<<gps_data[8]<<" "<<gps_data[9]<<" "<<gps_data[10]<<" "<<gps_data[11]<<" "<<gps_data[12]<<endl;

				init_align_start_time = m_GPSTime;
			}
		}
	}

	return 0;

}

// Bei modified the IMU part in Jan 2013
void CMainFrame::Start_GPSINS_Integration(IMUDataPkg * datapkg)
{
	if (INIT_ALIGN_READY)
	{
		IMU_GPSTime.push_back(0.001 * datapkg->time);

		xRate.push_back(datapkg->gx);

		yRate.push_back(datapkg->gy);

		zRate.push_back(datapkg->gz);

		xAccel.push_back(datapkg->ax);

		yAccel.push_back(datapkg->ay);

		zAccel.push_back(datapkg->az);
		
		Roll.push_back(datapkg->roll);

		Pitch.push_back(datapkg->pitch);

		Yaw.push_back(datapkg->yaw);



		if (PPP_ESTIMATION_READY)
		{		
			// Find the closest INS epoch
			int SyncIdx = SyncGPSINS();
			
			if (SyncIdx >= 1)
			{
				// INS Mechanization: to propagate the INS states to current epoch, and delete used measurements
				if (SyncIdx > 2)
				{
					// GPS INS Integration
					double GPS[13] = {0,0,0,0,0,0,0,0,0,0,0,0,0};

					for (int i = 0; i < SyncIdx; i=i+2)
					{
						double INS_pre[7] = {IMU_GPSTime.at(0),
									xAccel.at(i), yAccel.at(0), zAccel.at(0),
									xRate.at(i), yRate.at(0), zRate.at(0)};
						double INS[7] = {IMU_GPSTime.at(1),
									xAccel.at(1), yAccel.at(1), zAccel.at(1),
									xRate.at(1), yRate.at(1), zRate.at(1)};

						double Orientation[3] = {Roll.at(1), Pitch.at(1), Yaw.at(1)};

						//m_EKF.ins_ekf_psi_bs(INS_pre, INS, GPS, m_EKF_Estimation, Orientation, false, false);

						IMU_GPSTime.erase(IMU_GPSTime.begin(), IMU_GPSTime.begin()+1);
						xRate.erase(xRate.begin(), xRate.begin()+1);
						yRate.erase(yRate.begin(), yRate.begin()+1);
						zRate.erase(zRate.begin(), zRate.begin()+1);
						xAccel.erase(xAccel.begin(), xAccel.begin()+1);
						yAccel.erase(yAccel.begin(), yAccel.begin()+1);
						zAccel.erase(zAccel.begin(), zAccel.begin()+1);
						Pitch.erase(Pitch.begin(), Pitch.begin()+1);
						Roll.erase(Roll.begin(), Roll.begin()+1);
						Yaw.erase(Yaw.begin(), Yaw.begin()+1);

					}
				}

				// GPS INS Integration
				double GPS[13] = {PPP_GPSTime.back(),
								PPP_phi.back(), PPP_lambda.back(), PPP_h.back(),
								PPP_sigmaphi.back(), PPP_sigmalambda.back(), PPP_sigmah.back(),
								PPP_nV.back(), PPP_eV.back(), PPP_dV.back(),
								PPP_sigmanV.back(), PPP_sigmaeV.back(), PPP_sigmadV.back()};
				double INS_pre[7] = {IMU_GPSTime.at(0),
								xAccel.at(0), yAccel.at(0), zAccel.at(0),
								xRate.at(0), yRate.at(0), zRate.at(0)};
				double INS[7] = {IMU_GPSTime.at(1),
								xAccel.at(1), yAccel.at(1), zAccel.at(1),
								xRate.at(1), yRate.at(1), zRate.at(1)};

				double Orientation[3] = {Roll.at(1), Pitch.at(1), Yaw.at(1)};

				//m_EKF.ins_ekf_psi_bs(INS_pre, INS, GPS, m_EKF_Estimation, Orientation, true, true);

				PPP_GPSTime.pop_back();
				PPP_phi.pop_back();
				PPP_lambda.pop_back();
				PPP_h.pop_back();
				PPP_sigmaphi.pop_back();
				PPP_sigmalambda.pop_back();
				PPP_sigmah.pop_back();
				PPP_nV.pop_back();
				PPP_eV.pop_back();
				PPP_dV.pop_back();
				PPP_sigmanV.pop_back();
				PPP_sigmaeV.pop_back();
				PPP_sigmadV.pop_back();

				IMU_GPSTime.erase(IMU_GPSTime.begin(), IMU_GPSTime.begin()+1);
				xRate.erase(xRate.begin(), xRate.begin()+1);
				yRate.erase(yRate.begin(), yRate.begin()+1);
				zRate.erase(zRate.begin(), zRate.begin()+1);
				xAccel.erase(xAccel.begin(), xAccel.begin()+1);
				yAccel.erase(yAccel.begin(), yAccel.begin()+1);
				zAccel.erase(zAccel.begin(), zAccel.begin()+1);
				Pitch.erase(Pitch.begin(), Pitch.begin()+1);
				Roll.erase(Roll.begin(), Roll.begin()+1);
				Yaw.erase(Yaw.begin(), Yaw.begin()+1);


				for (int i = 0; i < NUM_SOLUTION_STATES; i++)
				{
					gps_ins_solution_file<<m_EKF_Estimation[i]<<"    ";
				}
				gps_ins_solution_file<<endl;
			}

			PPP_ESTIMATION_READY = false;
		}
	}
	else
	{
		if (m_InitIMUFile != NULL)
		{
			double imu_data[] = {datapkg->time,
			datapkg->ax, datapkg->ay, datapkg->az,
			datapkg->gx, datapkg->gy, datapkg->gz};

			fwrite(imu_data, 1, sizeof(imu_data), m_InitIMUFile);
		}

		if (datapkg->time >= init_align_start_time + INIT_ALIGN_TIME)
		{
			double initial_alignment_result[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
			//m_EKF.initialization_alignment(static_init_gps_filename, static_init_imu_filename, 2, initial_alignment_result);

			TRACE("Initial Position: %f, %f, %f\n", initial_alignment_result[0], initial_alignment_result[1], initial_alignment_result[2]); 

			TRACE("Initial Velocity: %f, %f, %f\n", initial_alignment_result[3], initial_alignment_result[4], initial_alignment_result[5]); 

			TRACE("Pitch Roll Yaw: %f, %f, %f\n", initial_alignment_result[6], initial_alignment_result[7], initial_alignment_result[8]); 

			INIT_ALIGN_READY = true;

			m_InitGPSFile.close();
			fclose(m_InitIMUFile);
		}
	}
}

int CMainFrame::SyncGPSINS()
{
	if (IMU_GPSTime.size() <= 1)
	{
		TRACE0("Not enough IMU data!\n");
		return 0;
	}

	int idx = -1; 
	double gps_t = PPP_GPSTime.back();
	double imu_t = IMU_GPSTime.at(0);
	double t_diff_min = abs(gps_t - imu_t);
	for (int i = 1; i < IMU_GPSTime.size(); i++)
	{
		double t_diff = abs(gps_t - IMU_GPSTime.at(i));

		if (t_diff < t_diff_min)
		{
			t_diff_min = t_diff;
			imu_t = IMU_GPSTime.at(i);
			idx = i;
		}
	}

	return idx;
}
// Bei modified the IMU part in Jan 2013

/* ---------------------------------------------------------------------
Name:			OnObsReady

Purpose:		this function is activated by the GPS raw data decode procedure,
				indicates a set of broadcast ephemeris has been decoded

Arguments:		WPARAM wParam and LPARAM lParam are used to
				store information that is important to handling the message

History:		Name				Date		Reason for modification
                PMIS                2003        Original
--------------------------------------------------------------------- */
LRESULT CMainFrame::OnEphReady(WPARAM wParam, LPARAM lParam)
{

	// currently, this function does nothing, user can add commands here if he wants
	return 0;

}
#endif

/* ---------------------------------------------------------------------
Name:			OnDestroy

Purpose:		prepare to close the application.

History:		Name				Date		Reason for modification
                PMIS                2003        Original
--------------------------------------------------------------------- */
void CMainFrame::OnDestroy() 
{
	// stop the processing if it is in progress
	OnStop();

	// stop the timer
	KillTimer(1);

	CFrameWnd::OnDestroy();
	
}


/* ---------------------------------------------------------------------
Name:			ShowAzmElv

Purpose:		To show Satellites azimuth and elevation.

History:		Name				Date		Reason for modification
                PMIS                2003        Original
--------------------------------------------------------------------- */
void CMainFrame::ShowAzmElv() 
{

	// if the windows used to show the azimuth and elevation of satellite are active,
	// display the information
	if (m_pAzmView!=NULL)
	{
		m_pAzmView->ShowAzmElv();
	}
	if (m_pPicView!=NULL)
	{
		m_pPicView->ShowAzmElv();
	}
}


/* ---------------------------------------------------------------------
Name:			ShowMessage

Purpose:		To displays message.

Arguments:		CString msg - message to display
				int clear   - indicate if old message should be removed

History:		Name				Date		Reason for modification
                PMIS                2003        Original
--------------------------------------------------------------------- */
void CMainFrame::ShowMessage(CString msg, int clear) 
{
	// if the window used to show the information of Kalman filter is active,
	// display the information
	if (m_pMsgView!=NULL)
	{
		// add an "Enter" and "Newline" to each message
		char ch[3];
		ch[0]=0x0D;
		ch[1]=0x0A;
		ch[2]=0;

		m_pMsgView->ShowMessage(msg+ch,clear);
	}
}


/* ---------------------------------------------------------------------
Name:			ShowaResult

Purpose:		Show results and residuals.

History:		Name				Date		Reason for modification
                PMIS                2003        Original
--------------------------------------------------------------------- */
void CMainFrame::ShowaResult() 
{
	// if the windows are active,
	// display results
	if (m_pResView!=NULL)
	{
		m_pResView->ShowResult();
	}

	// display residuals
	if(m_pRngView!=NULL)
	{
		m_pRngView->ShowResidual();
	}
}


/* ---------------------------------------------------------------------
Name:			ShowTraj

Purpose:		To add one epoch results to trajectory.

Arguments:		double t   -  GPS sec of the week
				double p   -  phi
				double l   -  lambda
				double h   -  height
				double UTC -  UTC time

History:		Name				Date		Reason for modification
                PMIS                2003        Original
--------------------------------------------------------------------- */
void CMainFrame::ShowTraj(double t, double p, double l, double h, double UTC) 
{
	// if the window is active, display trajectory
	if (m_pTrajView!=NULL)
	{
		m_pTrajView->AddTraj(t,p,l,h,UTC);
	}
}


/* ---------------------------------------------------------------------
Name:			OnPause

Purpose:		To pause processing when the user presses the PAUSE button.

History:		Name				Date		Reason for modification
                PMIS                2003        Original
--------------------------------------------------------------------- */
void CMainFrame::OnPause() 
{
	// if user press the PAUSE button, change the state of m_bPause
	// if a processing is in progress, press the PAUSE button will pause the processing,
	// press the PAUSE again will resume the processing. 
	if(m_bStart == TRUE)
		m_bPause=!m_bPause;
	else
		m_bPause = FALSE;
}


/* ---------------------------------------------------------------------
Name:			OnUpdatePause

Purpose:		To update the pause button.

Arguments:		CCmdUI* pCmdUI - object contains information of a menu

History:		Name				Date		Reason for modification
                PMIS                2003        Original
--------------------------------------------------------------------- */
void CMainFrame::OnUpdatePause(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bPause);
}


/* ---------------------------------------------------------------------
Name:			OnStop

Purpose:		To stop the processing when a processing is in progress.

History:		Name				Date		Reason for modification
                PMIS                2003        Original
--------------------------------------------------------------------- */
void CMainFrame::OnStop() 
{

	m_bStop= 1;
	m_bStart=0;

	// close the log file
	if (m_ofslog.is_open())
	{
		m_ofslog.close();
	}

	// in post-mission mode, nothing else to do 
	if (m_options.system_mode == 1)
	{
		return;
	}

#ifndef POST_MISSION

	// in real-time mode, serial port and socket should be closed if they are open
	// close serial port
	if (m_commGPS.m_hWnd!=NULL)
	{
		if (m_commGPS.GetPortOpen())
			m_commGPS.SetPortOpen(0);
		m_commGPS.DestroyWindow();
	}
	


	// close socket
	if (m_options.eph_flag == 2 || m_options.clock_flag == 3
		|| m_options.eph_flag == 3 || m_options.clock_flag == 4)
	{
		if (m_pSocketRTCA!=NULL)
		{
			m_pSocketRTCA->Close();

			delete m_pSocketRTCA;
			m_pSocketRTCA = NULL;
		}

		CloseHandle(h_thread);
	}

#endif

}


/* ---------------------------------------------------------------------
Name:			OnUpdateStop

Arguments:		CCmdUI* pCmdUI - object contains information of a menu

History:		Name				Date		Reason for modification
                PMIS                2003        Original
--------------------------------------------------------------------- */
void CMainFrame::OnUpdateStop(CCmdUI* pCmdUI) 
{
}


/* ---------------------------------------------------------------------
Name:			OnUpdateStart

Purpose:		To update the start button.

Arguments:		CCmdUI* pCmdUI - object contains information of a menu

History:		Name				Date		Reason for modification
                PMIS                2003        Original
--------------------------------------------------------------------- */
void CMainFrame::OnUpdateStart(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bStart);
}


/* ---------------------------------------------------------------------
Name:			OnPostProcess

Purpose:		To perform post processing.

History:		Name				Date		Reason for modification
                PMIS                2003        Original
--------------------------------------------------------------------- */
void CMainFrame::OnPostProcess()
{
	// to open Rinex format observation file */
	CRinexObsFile	m_RinexObsFile;
	// to open Rinex broadcast ephemeris file */
	CGpsNavFile		m_GpsNavFile;

	// Bei add this in March 2013
	/*
	m_IMUFile = fopen(m_IMUFilename, "rb");
	double ins1[7] = {0};
	double ins[7]  = {0};
	fread(ins1, sizeof(double), 7, m_IMUFile);
	fread(ins, sizeof(double), 7, m_IMUFile);
	*/
	// Bei add this in March 2013
	 
	//[MAX_GPS_PRN-MIN_GPS_PRN+1];		// 2D array of GPS ephemerides (stored by PRN)
	CGPS_EPHList	*m_arrayGPSEph;
	CPreciseOrbitFile pof;
	CPreciseClockFile pcf;
	

	// observation file (binary input)
	if( !m_RinexObsFile.OpenFile(m_options.obs_filename))
	{
		// no observations to process, return 
		MessageBox("Error opening input Observation File!","Observation File Error!",MB_ICONSTOP);
		m_bStart = 0;
		m_bStop=0;
		m_bPause=0;
		return;
	}

	// if the user selects to get antenna information from observation file
	// getting the antenna delta h/e/n from RINEX file
	if(m_options.arp2m_mode == 1)
	{
		ifstream in_obs;

		in_obs.open(m_options.obs_filename);

		char str[200];

		// loop till the end of the file, if find the information, break the loop
		while(!in_obs.eof())
		{
			in_obs.getline(str, 200);

			// if the end of header is reached, the antenna parameters do not exist in the
			// observation file
			if(strstr(str,"END OF HEADER") != NULL)
			{
				MessageBox("Marker to ARP parameters were not found\nin the specified Observation File!","Observation File Error!",MB_ICONSTOP);
				m_bStart = 0;
				m_bStop=0;
				m_bPause=0;
				return;
			}
			else
			{
				// ignoring comment lines
				if(strstr(str,"COMMENT") != NULL)
					continue;

				// line must contain "ANTENNA: DELTA H/E/N"
				else if(strstr(str,"ANTENNA") != NULL && strstr(str,"DELTA") != NULL 
					&& strstr(str,"H/E/N") != NULL)
				{
					CString cstr = str;
					char *stop_string;

					// get antenna offset
					m_options.arp2mark_up = strtod(cstr.Left(14),&stop_string);
					m_options.arp2mark_east = strtod(cstr.Mid(15,14),&stop_string);
					m_options.arp2mark_north = strtod(cstr.Mid(29,14),&stop_string);

					in_obs.close();

					break;
				} //else if(strstr(str,"ANTENNA")
			} // else
		} // while(!in_obs.eof())					
	} // if(m_options.arp2m_mode == 1)

	// opening the log file for parameter output
	OpenLog();

	// in the following cases, broadcast ephemeris file should be used
	// if using broadcast ephemeris, GPS*C corrections, IGDG corrections
	if (m_options.eph_flag==0 || m_options.eph_flag==2 || m_options.eph_flag==3  
		|| m_options.clock_flag==0 || m_options.clock_flag==3 || m_options.clock_flag==4
		// or in SPP mode using single frequency data
		|| (m_options.processing_mode==0 && m_options.frequency_flag==1))
	{
		// if can not open broadcast ephemeris file
		if ((!m_GpsNavFile.OpenFile(m_options.eph_filename))
			&& (m_options.eph_flag==0 || m_options.eph_flag==2))  
		{
			// no broadcast ephemeris, return
			if (!(m_options.processing_mode==0 && m_options.frequency_flag==1))
			{
				MessageBox("Error opening input Broadcast Ephemeris File!","Broadcast Ephemeris File Error",MB_ICONSTOP);
				m_bStart = 0;
				m_bStop=0;
				m_bPause=0;
				return;
			}
			else
			{
				// in SPP mode using single frequency data, use default ionospheric parameters
				m_options.has_iono_para=0;
			}
		}
		else if (m_options.processing_mode==0 && m_options.frequency_flag==1)
		{
			// obtain ionospheric parameter for single frequence point positioning
			m_options.has_iono_para=m_GpsNavFile.GetIonoPara(m_options.iono_alpha,m_options.iono_beta);
		}
	}

	// get the pointer to the array of broadcast ephemeris
	m_arrayGPSEph = m_GpsNavFile.GetEphList();

	// if user selects to use SP3 format orbits or clocks, open SP3 file
	if (m_options.eph_flag==1 || m_options.clock_flag==2)
	{
		if (!m_SP3Reader.Open(m_options.peph_filename))  
		{
			// no orbits or clocks, return
			MessageBox("Error opening input SP3 Precise Ephemeris File!","Precise Ephemeris File Error",MB_ICONSTOP);
			m_bStart = 0;
			m_bStop=0;
			m_bPause=0;
			return;
		}
	}

	// if user selects to use 5-min clocks, open Rinex format 5-min clock file
	if (m_options.clock_flag==2 && m_options.eph_flag!=1)
	{
		if (!m_SP3Reader.Open(m_options.pclk_filename))  //(eph_filename)
		{
			MessageBox("Error opening input SP3 Precise Clock File!","Precise Clock File Error",MB_ICONSTOP);
			m_bStart = 0;
			m_bStop=0;
			m_bPause=0;
			return;
		}
	}

	if (m_options.clock_flag==1)
	{
		if (!m_5MinClk.Open(m_options.pclk_filename))  //(eph_filename)
		{
			// no clocks, return
			MessageBox("Error opening input 5 Minute Precise Clock File!","Precise Clock File Error",MB_ICONSTOP);
			m_bStart = 0;
			m_bStop=0;
			m_bPause=0;
			return;
		}
	}

	if (m_options.eph_flag==2 || m_options.eph_flag==3)
	{
		m_options.bBackward=0;
		if (!pof.Open(m_options.peph_filename))
		{
			MessageBox("Error opening input GPS*C Precise Ephemeris File!","Precise Ephemeris File Error",MB_ICONSTOP);
			m_bStart = 0;
			m_bStop=0;
			m_bPause=0;
			return;
		}
	}

	if (m_options.clock_flag==3 || m_options.clock_flag==4)
	{
		m_options.bBackward=0;
		if (!pcf.Open(m_options.pclk_filename))
		{
			MessageBox("Error opening input GPS*C Precise Clock File!","Precise Clock File Error",MB_ICONSTOP);
			m_bStart = 0;
			m_bStop=0;
			m_bPause=0;
			return;
		}
	}

	CString begstr,endstr;
	// obtain the timetag of the first and last epoch observation
	CGPSTime gt=m_RinexObsFile.m_tFirstObs;
	begstr.Format("%04d-%02d-%02d %02d:%02d:%04.1f",
		gt.m_iYear,gt.m_iMon,gt.m_iDay,gt.m_iHour,gt.m_iMin,gt.m_dSec);

	gt=m_RinexObsFile.m_tLastObs;
	endstr.Format("%04d-%02d-%02d %02d:%02d:%04.1f",
		gt.m_iYear,gt.m_iMon,gt.m_iDay,gt.m_iHour,gt.m_iMin,gt.m_dSec);

	// to show the progress bar, if it has been created
	if (m_ProgressCalc.m_hWnd!=NULL)
	{
		m_ProgressCalc.SetPos(0);
		m_ProgressCalc.ShowWindow(SW_SHOW);

		// display the first epoch timetag
		if (m_stcBegin.m_hWnd!=NULL)
		{
			m_stcBegin.SetWindowText(begstr);
			m_stcBegin.ShowWindow(SW_SHOW);
		}
		// still have not begun processing, empty current timetag 
		if (m_stcCur.m_hWnd!=NULL)
		{
			m_stcCur.SetWindowText("");
			m_stcCur.ShowWindow(SW_SHOW);
		}
		// display the last epoch timetag
		if (m_stcEnd.m_hWnd!=NULL)
		{
			m_stcEnd.SetWindowText(endstr);
			m_stcEnd.ShowWindow(SW_SHOW);
		}

	}	

	double tlength;

	// calculate the time span between the first and the last epoch 
	tlength=m_RinexObsFile.m_tLastObs.GetGPSTime().secsOfWeek
		-m_RinexObsFile.m_tFirstObs.GetGPSTime().secsOfWeek;

	// if the observation file crosses two sequential GPS week, plus one full week to the time span
	// assuming the observation file is shorter than one week
	if (tlength<0)
		tlength=tlength+604800.0;

	// if we obtain the orbits, clocks and observations, then we can start
	m_bStart=1;

	double mask=m_options.elev_mask;

	// clear the observation array for a new processing
	m_arrayEpoch.RemoveAll();

	// before we reach the end of the observation file, processing is forward
	m_bBackward=0;

	// start the loop till one of the following events
	// 1: arrive at the end of the file if only forward processing
	// 2: arrive at the first epoch again in backward processing
	// 3: the processing is stopped by the user
	while(1)		
	{
		//************************************************************************
		//**                      Check for user input                          **
		//************************************************************************

		// to check if user want to do other task
		DoEvents();
	
		// if the PAUSE button is held, pause the processing. But other task can still continue 
		while (m_bPause)
		{
			// to do other tasks
			DoEvents();

			// if the STOP button is clicked, the processing will halt 
			if (m_bStop)
				break;
		}
		
		// to check if user has clicked the STOP button
		if (m_bStop)
			break;

		//************************************************************************
		//**            Read one epoch of data from observation file            **
		//************************************************************************

		// get the timetag of last epoch
		GPSTime told=m_epoch.GetGPSTime();

		// used to check if the processing finishes
		bool finish=0;

		// still forwad processing
		if (!m_bBackward)
		{
			// loop to get an epoch at the time interval selected by the user
			while (!finish)
			{
				// reach the end of the file
				if (m_RinexObsFile.getNextEpoch(m_epoch)<0)	
				{
					finish=1;
					break;
				}
				// if the interval selected by the user is less than 0.1 second
				// all observations should be processed
				if (m_options.obs_interval<0.1)
					break;
				// otherwise, only process the observations at the selected time interval
				if ((int)(fmod(m_epoch.GetGPSTime().secsOfWeek+0.01,m_options.obs_interval)*10) ==0)
					break;
			}

			// if reach the end of the file
			if (finish)
			{
				// if user selects backward processing
				if (m_options.bBackward)
				{
					// the conduct the backward processing 
					m_bBackward=1;
					// remove one epoch from the array after processing
					if (m_arrayEpoch.GetSize()>1)
						m_arrayEpoch.RemoveAt(m_arrayEpoch.GetSize()-1);
					continue;
				}
				else // if only forward processing, the processing is finished
					break;
			}

			// if user selects backward processing, save the observations to the array
			// during the forward processing
			if (m_options.bBackward)
			{
				// increase the size of the array to save current data
				int epochs=m_arrayEpoch.GetSize()+1;
				m_arrayEpoch.SetSize(epochs);
				m_arrayEpoch[epochs-1].gpsTime=m_epoch.GetCGPSTime();
				m_arrayEpoch[epochs-1].num=m_epoch.GetSatNum();
				COBS* pobs=m_epoch.GetObs();
				// save observations for each satellites
				for (int i=0;i<m_arrayEpoch[epochs-1].num;i++)
				{
					GPSOBS* pgpsobs=pobs[i].GetGPSOBS();
					m_arrayEpoch[epochs-1].obsList.Add(GPSOBS(pgpsobs->prn,pgpsobs->rangeP1,
						pgpsobs->rangeP2,pgpsobs->rangeCA,pgpsobs->phaseL1,pgpsobs->phaseL2,
						pgpsobs->dopplerL1,pgpsobs->dopplerL2,pgpsobs->snrL1,pgpsobs->snrL2));
				}
			}
		}
		else
		{
			// if backward processing already, chech the size of the array
			int epochs=m_arrayEpoch.GetSize();
			// if no element in the array, finish!
			if (epochs<1)
				break;
			else
			{
				// if there are still observations in the array
				COBS* pObs = m_epoch.GetObs();
				int obses=m_arrayEpoch[epochs-1].obsList.GetSize();

				// pass the observations to the object CEpoch for processing
				for (int i=0;i<obses;i++)
				{
					pObs[i].SetGPSOBS(m_arrayEpoch[epochs-1].obsList[i]);		
				}

				m_epoch.SetTimetag(m_arrayEpoch[epochs-1].gpsTime);
				m_epoch.SetSatNum(obses);

				// decrease the size of the array
				m_arrayEpoch.RemoveAt(epochs-1);
			}
		}


		if (m_options.eph_flag==2 || m_options.eph_flag==3)
		{
			pof.GetPreciseOrbits(m_precise_eph,m_epoch.GetGPSTime());
		}

		if (m_options.clock_flag==3 || m_options.clock_flag==4)
		{
			pcf.GetPreciseClocks(m_precise_clock,m_epoch.GetGPSTime());
		}
		// if using broadcast ephemeris, GPS*C corrections, IGDG corrections,
		// pick up the nearest set of broadcast ephemeris
		if (m_options.eph_flag==0 || m_options.eph_flag==2 || m_options.eph_flag==3 || m_options.clock_flag==0 || m_options.clock_flag==3 || m_options.clock_flag==4)
		{
			PickBestGPSEphs(m_epoch.GetGPSTime().secsOfWeek,m_arrayGPSEph);
		}

		CString str;
		// display the timetag of current epoch in the status bar
		str.Format("%.1lf",m_epoch.GetGPSTime().secsOfWeek);
		m_wndStatusBar.SetPaneText(6, str);

		// orbits, clocks and observations are prepared, begin Kalman filter
		m_epoch.DoKF();/*perform kalman filtering*/


		for(int i = 0; i < m_epoch.number_sat; i++)
		{
			solution_file<<setprecision(16)<<m_epoch.GetGPSTime().secsOfWeek<<"  "<<m_epoch.observations[i].GetPrn()<<"  "<<m_epoch.observations[i].GetSatCoor()->x<<"  "<<m_epoch.observations[i].GetSatCoor()->y<<"  "<<m_epoch.observations[i].GetSatCoor()->z<<"  "<<*(m_epoch.observations[i].GetCorRange())<<"  ";
			solution_file<<m_epoch.observations[i].GetAllCorr()->clock_satellite<<"  "<<m_epoch.observations[i].GetAllCorr()->clock_receiver<<"  "<<m_epoch.GetParameter()->sigma_clock_cor<<"  "<<endl;
		}

		// Bei add this in March 2013
		/*
		double gps[13] = {0};
		double head[3] = {0};

		gps[0] = m_epoch.GetGPSTime().secsOfWeek;
		gps[1] = m_epoch.GetParameter()->station_plh.phi * 180/PI;
		gps[2] = m_epoch.GetParameter()->station_plh.lambda * 180/PI;
		gps[3] = m_epoch.GetParameter()->station_plh.h;
		gps[4] = m_epoch.GetParameter()->sigma_plh.phi;
		gps[5] = m_epoch.GetParameter()->sigma_plh.lambda;
		gps[6] = m_epoch.GetParameter()->sigma_plh.h;
		gps[7] = m_epoch.GetParameter()->station_vel.lambda;
		gps[8] = m_epoch.GetParameter()->station_vel.phi;
		gps[9] = m_epoch.GetParameter()->station_vel.h;
		gps[10]= m_epoch.GetParameter()->sigma_vel.lambda;
		gps[11]= m_epoch.GetParameter()->sigma_vel.phi;
		gps[12]= m_epoch.GetParameter()->sigma_vel.h;

		while (ins[0] <= gps[0] - 0.5 / m_IMURate)
		{
			// INS Mechanization
			//m_EKF.ins_ekf_psi_bs(ins1, ins, gps, sol, head, 0, 0);
			fread(ins1, sizeof(double), 7, m_IMUFile);
			fread(ins, sizeof(double), 7, m_IMUFile);

			// GPS INS Integration
			if (ins[0] > gps[0] - 0.5 / m_IMURate)
			{

				if (!m_epoch.noestimation)
				{
				//m_EKF.ins_ekf_psi_bs(ins1, ins, gps, sol, head, 1, 0);
				}
				else
				{
				//m_EKF.ins_ekf_psi_bs(ins1, ins, gps, sol, head, 0, 0);
				}

				fread(ins1, sizeof(double), 7, m_IMUFile);
				fread(ins, sizeof(double), 7, m_IMUFile);
				
			}

		}
		*/
		// Bei add this in March 2013

		// move the progress bar
		if (m_ProgressCalc.m_hWnd!=NULL)
		{
			// to claculate the percent processed
			double tlen=m_epoch.GetGPSTime().secsOfWeek-m_RinexObsFile.m_tFirstObs.GetGPSTime().secsOfWeek;
			if (tlen<0)
				tlen=tlen+604800.0;
			short st=(short)(tlen/tlength*100);
			if (st<0)
				st=0;
			if (st>100)
				st=100;
			// move the bar
			m_ProgressCalc.SetPos(st);

			// show timetag of current epoch in the static box
			if (m_stcCur.m_hWnd!=NULL)
			{
				gt=CGPSTime(m_epoch.GetGPSTime());
				begstr.Format("%04d-%02d-%02d %02d:%02d:%04.1f",
					gt.m_iYear,gt.m_iMon,gt.m_iDay,gt.m_iHour,gt.m_iMin,fabs(gt.m_dSec));
				m_stcCur.SetWindowText(begstr);
			} // (m_stcCur.m_hWnd!=NULL)
		} // if (m_ProgressCalc.m_hWnd!=NULL)
	}// while(1)

	// now, the processing is finished, hide some windows

	// hide the prograss bar, the first timetag, current timetag and the last timetag
	if (m_ProgressCalc.m_hWnd!=NULL)
	{
		m_ProgressCalc.ShowWindow(SW_HIDE);
		m_ProgressCalc.SetPos(0);
		if (m_stcBegin.m_hWnd!=NULL)
		{
			m_stcBegin.SetWindowText("");
			m_stcBegin.ShowWindow(SW_HIDE);
		}
		if (m_stcCur.m_hWnd!=NULL)
		{
			m_stcCur.SetWindowText("");
			m_stcCur.ShowWindow(SW_HIDE);
		}
		if (m_stcEnd.m_hWnd!=NULL)
		{
			m_stcEnd.SetWindowText("");
			m_stcEnd.ShowWindow(SW_HIDE);
		}
	}

	// clear the observation array
	m_arrayEpoch.RemoveAll();
	//close the log file
	if (m_ofslog.is_open())
	{
		m_ofslog.close();
	}

	// if user want to close the application
	if (m_bClose)
	{
		m_bStart = 0;
		m_bStop=0;
		m_bPause=0;
		//send message to close the application
		SendMessage(WM_CLOSE);
		return;
	}

	// reset all these variables
	m_bStart = 0;
	m_bStop=0;
	m_bPause=0;
}


/* ---------------------------------------------------------------------
Name:			PickBestGPSEph

Purpose:		To pick the best GPS ephemeris.

Arguments:		double ref_time - search time
				CGPS_EPHList &Eph - array of ephemerides (1D)
				int prn - satellite number

Return:			int:	the index of the best fit set of ephemeris

History:		Name				Date		Reason for modification
                PMIS                2003        Original
--------------------------------------------------------------------- */
int CMainFrame::PickBestGPSEph(double ref_time, CGPS_EPHList &Eph, int prn)
{
	int		i;							// counter
	int 	index=-1;					// index of "best" ephemeris
	double	time_diff;					// time difference between search time and TOE
	double	max_time_diff=18000.0;		// largest time diffrence (max of 5hr = 18000sec)

	// check that an ephemeris exists
	if(Eph.GetSize()<1)
		// return to calling function
		return index;

	for(i=0;i<Eph.GetSize();i++)
	{
		// find time difference
		time_diff = ref_time - Eph[i].toe;

		// check for end of week crossover
		if( time_diff < -302400.0 )
			time_diff += 604800.0;
		if( time_diff > 302400.0 )
			time_diff -= 604800.0;

		// find absolute time difference
		time_diff = fabs(time_diff);

		// check if this is the best ephemeris so far
		if(time_diff < max_time_diff)
		{
			// store time difference
			max_time_diff = time_diff;
			// store the index
			index = i;
		}
	}		//*** end of for(i=0;...) loop ***

	// return the fittest set of ephemeris
	return index;
	
}


/* ---------------------------------------------------------------------
Name:			PickBestGPSEphs

Purpose:		To pick the best GPS ephemerides.

Arguments:		double ref_time - search time
				CGPS_EPHList *Eph - array of ephemerides (1D)

History:		Name				Date		Reason for modification
                PMIS                2003        Original
--------------------------------------------------------------------- */
int CMainFrame::PickBestGPSEphs(double ref_time, CGPS_EPHList *Eph)
{
	// loop for all satellites 
	for (int i=0;i<32;i++)
	{
		// to select the best fit ephemeris of one satellite
		int ind=PickBestGPSEph(ref_time,Eph[i],i);

		// to check if the best one exists
		if (ind>=0 && ind <=Eph[i].GetSize())
				memcpy(&m_gps_eph[i],&(Eph[i][ind]),sizeof(CGPS_EPH));
	}

	return 1;
}


/* ---------------------------------------------------------------------
Name:			OnSize

Purpose:		To resize progress bar when user resizes the frame window.

Arguments:		UINT nType - type of resizing 
				int cx - new width 
				int cy - new height 

History:		Name				Date		Reason for modification
                PMIS                2003        Original
--------------------------------------------------------------------- */
void CMainFrame::OnSize(UINT nType, int cx, int cy) 
{
	CMDIFrameWnd::OnSize(nType, cx, cy);
	
	// to calculate new size for the progress bar
	CRect rect,rectToolBar;
	int width=0;
	GetWindowRect(rect);
	m_wndToolBar.GetItemRect(14, &rectToolBar);
	rectToolBar.right=rect.right-50;
	rect.top = rectToolBar.top+5;
	rect.bottom = rectToolBar.top + 20;
	rect.left=rectToolBar.left+0;
	rect.right=rectToolBar.right-0;
	
	if (m_ProgressCalc.GetStyle() & WS_VISIBLE)
	{
		// if it is showed, resize and show it
		m_ProgressCalc.SetWindowPos(NULL,rect.left,rect.top,rect.Width(),rect.Height(),
			SWP_SHOWWINDOW);
	}
	else
	{
		// if it is hiden, resize it and keep it hiden
		m_ProgressCalc.SetWindowPos(NULL,rect.left,rect.top,rect.Width(),rect.Height(),
			SWP_HIDEWINDOW);
	}

	// to calculate new size for the static box showing the beginning time tag
	rect.top = rectToolBar.top+22;
	rect.bottom = rect.top + 18;
	rect.left=rectToolBar.left+20;
	rect.right=rect.left+150;
	if (m_stcBegin.GetStyle() & WS_VISIBLE)
	{
		// if it is showed, resize and show it
		m_stcBegin.SetWindowPos(NULL,rect.left,rect.top,rect.Width(),rect.Height(),
			SWP_SHOWWINDOW);
	}
	else
	{
		// if it is hiden, resize it and keep it hiden
		m_stcBegin.SetWindowPos(NULL,rect.left,rect.top,rect.Width(),rect.Height(),
			SWP_HIDEWINDOW);
	}

	// to calculate new size for the static box showing the current time tag
	rect.top = rectToolBar.top+22;
	rect.bottom = rect.top + 18;
	rect.left=(rectToolBar.left + rectToolBar.right)/2 - 75;
	rect.right=rect.left+150;
	if (m_stcCur.GetStyle() & WS_VISIBLE)
	{
		// if it is showed, resize and show it
		m_stcCur.SetWindowPos(NULL,rect.left,rect.top,rect.Width(),rect.Height(),
			SWP_SHOWWINDOW);
	}
	else
	{
		// if it is hiden, resize it and keep it hiden
		m_stcCur.SetWindowPos(NULL,rect.left,rect.top,rect.Width(),rect.Height(),
			SWP_HIDEWINDOW);
	}

	// to calculate new size for the static box showing the end time tag
	rect.top = rectToolBar.top+22;
	rect.bottom = rect.top + 18;
	rect.left=rectToolBar.right-180;
	rect.right=rect.left+150;
	if (m_stcEnd.GetStyle() & WS_VISIBLE)
	{
		// if it is showed, resize and show it
		m_stcEnd.SetWindowPos(NULL,rect.left,rect.top,rect.Width(),rect.Height(),
			SWP_SHOWWINDOW);
	}
	else
	{
		// if it is hiden, resize it and keep it hiden
		m_stcEnd.SetWindowPos(NULL,rect.left,rect.top,rect.Width(),rect.Height(),
			SWP_HIDEWINDOW);
	}
}


/* ---------------------------------------------------------------------
Name:			OnClose

Purpose:		To close the program when the user chooses to do so.

History:		Name				Date		Reason for modification
                PMIS                2003        Original
--------------------------------------------------------------------- */
void CMainFrame::OnClose() 
{
	// if a processing is in progress, popup a message
	if (m_bStart)
	{
		// if user choose to close the quit, then stop the processing and close the application
		if (MessageBox("Processing!\nQuit anyway?","Exiting PPP",MB_OKCANCEL | MB_ICONWARNING)==IDOK)
		{
			OnStop();
			m_bStop=1;
			m_bClose=1;
			return;
		}
		else
			return;
	}

	CMDIFrameWnd::OnClose();
}


/* ---------------------------------------------------------------------
Name:			OnDrawView

History:		Name				Date		Reason for modification
                PMIS                2003        Original
--------------------------------------------------------------------- */
void CMainFrame::OnDrawView() 
{
}


/* ---------------------------------------------------------------------
Name:			OnUpdateDrawView

Purpose:		To set a check on ViewDraw.

Arguments:		CCmdUI* pCmdUI - object contains information of a menu

History:		Name				Date		Reason for modification
                PMIS                2003        Original
--------------------------------------------------------------------- */
void CMainFrame::OnUpdateDrawView(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bViewDraw);
}


/* ---------------------------------------------------------------------
Name:			OnTimer

Purpose:		To do some periodical tasks

Arguments:		UINT nIDEvent - time identifier

History:		Name				Date		Reason for modification
                PMIS                2003        Original
--------------------------------------------------------------------- */
void CMainFrame::OnTimer(UINT nIDEvent) 
{
	// if a processing in in progress, the time tag of current epoch will be shown in the staus bar
	// otherwise, show the time of the computer
	if (!m_bStart)
	{
		CTime t=CTime::GetCurrentTime();
		m_wndStatusBar.SetPaneText(6, t.Format("%H:%M:%S"));
	}

	CMDIFrameWnd::OnTimer(nIDEvent);
}


/* ---------------------------------------------------------------------
Name:			ReadOption

Purpose:		Reading PPP options from application registry.

History:		Name				Date		Reason for modification
                PMIS                2003        Original
--------------------------------------------------------------------- */
void CMainFrame::ReadOption() 
{
	int option; // for integer options
	CString setting; // for text options
	int sign; // for the sign of some options
	CWinApp* app=AfxGetApp(); // obtain the pointer to the application at first

	// for float options which are saved as integers in the registry
	double scale=1000000.0;
	int iscale=1000000;

	// get system mode: 0 - real-time, 1 - post-mission
	m_options.system_mode=app->GetProfileInt("System","Process Mode",1);
	if (m_options.system_mode>1)
		m_options.system_mode=0;

	#ifdef POST_MISSION
		m_options.system_mode=1;
	#endif
		

	// get station mode: 0 - static, 1 - kinematic
	m_options.station_mode=app->GetProfileInt("System","Station Mode",0);
	if (m_options.station_mode>1)
		m_options.station_mode=0;

	// 0=epoch by epoch, 1=sequential filter
	m_options.filter_mode=app->GetProfileInt("System","Filter Mode",1);
	// 1=Random Walk, 0=Gauss Markov
	m_options.filter_model=app->GetProfileInt("System","Processing Model",1);
	// 0=SPP, 1=PPP
	m_options.processing_mode=app->GetProfileInt("System","Processing Mode",1);

	// 0=L3 (L1+L2),1=L1,2=L2
	m_options.frequency_flag=app->GetProfileInt("Observation","Frequency",0);
	if (m_options.frequency_flag>2)
		m_options.frequency_flag=0;
	// estimate velocity? 0=no, 1=solve
	m_options.vel_estimate=app->GetProfileInt("Observation","Velocity",0);
	// smoothing reset epoch
	m_options.smooth_flag=app->GetProfileInt("Observation","Smooth",0);
	if (m_options.smooth_flag>1)
		m_options.smooth_flag=1;
	m_options.reset_epoch=app->GetProfileInt("Observation","Reset",1000);

	// getting the backward processing option
	m_options.bBackward=app->GetProfileInt("Observation","Backward",0);

	// get the list of BLOCK II/IIA satellites
	// the options of the first 31 satellites are saved in one integer 
	// each bit saves one satellite
	option=app->GetProfileInt("Satellites","BlockII < 32",2011251711);
	int l=1;
	int s=1;
	for (int k=0;k<31;k++)
	{
		l=l*2;
		m_options.BLOCKII[k]= (option % l) / s;
		s=l;
	}

	// the option of prn=32 is saved in another integer
	m_options.BLOCKII[31]=app->GetProfileInt("Satellites","BlockII = 32",0);

	// get the list of excluded satellites
	// the options of the first 31 satellites are saved in one integer 
	// each bit saves one satellite
	option=app->GetProfileInt("SatEx","PRN < 32",0);
	l=1;
	s=1;
	for (k=0;k<31;k++)
	{
		l=l*2;
		m_options.SatEx[k]= (option % l) / s;
		s=l;
	}

	// the option of prn=32 is saved in another integer
	m_options.SatEx[31]=app->GetProfileInt("SatEx","PRN = 32",0);

	// get parameters associated with the METEOROLOGY dialog
	sign=app->GetProfileInt("Meteorology","Temperature-Sign",1);
	m_options.temperature=app->GetProfileInt("Meteorology","Temperature",15*iscale)/scale;
	if (sign==0)
		m_options.temperature=-m_options.temperature;

	m_options.pressure=app->GetProfileInt("Meteorology","Pressure",1010*iscale)/scale;
	m_options.humidity=app->GetProfileInt("Meteorology","Humidity",50*iscale)/scale;

	// 0=no,1=float,2=fixed
	m_options.ambiguity_flag=app->GetProfileInt("Unknows","Ambiguity",0);
	if (m_options.ambiguity_flag>2)
		m_options.ambiguity_flag=0;

	// 0=Traditional Model,1=P1P2CP Model
	m_options.iono_free_modal=app->GetProfileInt("Unknows","Iono_Modal",0);

	m_options.obs_interval=app->GetProfileInt("Unknows","Interval",0)/10.0;

	// 0=model,1=solve
	m_options.tropo_flag=app->GetProfileInt("Corrections","Troposphere Correction",0);
	if (m_options.tropo_flag>1)
		m_options.tropo_flag=0;
	// 0=broadcast,1=sp3,2=GPS*C, 3=JPL
	m_options.eph_flag=app->GetProfileInt("Corrections","Ephemeris Correction",0);
	// 0=broadcast,1=5min,2=sp3, 3=GPS*C, 4=JPL
	m_options.clock_flag=app->GetProfileInt("Corrections","Clock Correction",0);

	// 0=WGS84, 1=ITRF00, 2=NAD83
	if (m_options.eph_flag==1 || m_options.clock_flag==1 || m_options.clock_flag==2)
		m_options.res_ref_sys=1;
	else if (m_options.eph_flag==2 || m_options.clock_flag==3)
		m_options.res_ref_sys=2;
	else 
		m_options.res_ref_sys=0;

	// 0=Cartesian; 1=Ellipsoidal
	m_options.ref=app->GetProfileInt("Coordinate","Reference",1);
	if (m_options.ref>1)
		m_options.ref=1;
	
	// 0=WGS84, 1=ITRF00, 2=NAD83
	m_options.init_ref_sys=app->GetProfileInt("Coordinate","Init Coor Ref",0);
	
	// getting ellipsoidal ITRF00 or WGS84 coordinates
	if(m_options.init_ref_sys != 2)
	{
		sign=app->GetProfileInt("Coordinate","DirecA-Sign",1);
		if (sign!=1)
			sign=-1;
		m_options.direca=app->GetProfileInt("Coordinate","DirecA-Int",510445);
		option=app->GetProfileInt("Coordinate","DirecA-Float",94184);
		m_options.direca=sign*(m_options.direca+option/scale);
		
		sign=app->GetProfileInt("Coordinate","DirecB-Sign",-1);
		if (sign!=1)
			sign=-1;
		m_options.direcb=app->GetProfileInt("Coordinate","DirecB-Int",1140757);
		option=app->GetProfileInt("Coordinate","DirecB-Float",83824);
		m_options.direcb=sign*(m_options.direcb+option/scale);
		
		sign=app->GetProfileInt("Coordinate","DirecC-Sign",1);
		if (sign!=1)
			sign=-1;
		m_options.direcc=app->GetProfileInt("Coordinate","DirecC-Int",1116);
		option=app->GetProfileInt("Coordinate","DirecC-Float",5960);
		m_options.direcc=sign*(m_options.direcc+option/scale);
	}

	// getting ellipsoidal NAD83 coordinates
	else
	{
		sign=app->GetProfileInt("NADCoordinate","DirecA-Sign",1);
		if (sign!=1)
			sign=-1;
		m_options.origNADa=app->GetProfileInt("NADCoordinate","DirecA-Int",510445);
		option=app->GetProfileInt("NADCoordinate","DirecA-Float",94184);
		m_options.origNADa=sign*(m_options.origNADa+option/scale);
		
		sign=app->GetProfileInt("NADCoordinate","DirecB-Sign",-1);
		if (sign!=1)
			sign=-1;
		m_options.origNADb=app->GetProfileInt("NADCoordinate","DirecB-Int",1140757);
		option=app->GetProfileInt("NADCoordinate","DirecB-Float",83824);
		m_options.origNADb=sign*(m_options.origNADb+option/scale);
		
		sign=app->GetProfileInt("NADCoordinate","DirecC-Sign",1);
		if (sign!=1)
			sign=-1;
		m_options.origNADc=app->GetProfileInt("NADCoordinate","DirecC-Int",1116);
		option=app->GetProfileInt("NADCoordinate","DirecC-Float",5960);
		m_options.origNADc=sign*(m_options.origNADc+option/scale);
	}
	
	// getting the date of the coordinates
	m_options.coord_year=app->GetProfileInt("Coordinate","year",2000);
	m_options.coord_month=app->GetProfileInt("Coordinate","month",1);
	m_options.coord_day=app->GetProfileInt("Coordinate","day",1);

	m_options.ant_type=app->GetProfileString("Antenna","Antenna Type","");

	// read Marker to ARP offsets
	sign=app->GetProfileInt("Antenna","M2ARPE_sign",1);
	if (sign!=1)
		sign=-1;
	m_options.arp2mark_east=app->GetProfileInt("Antenna","M2ARPE_int",0);
	option=app->GetProfileInt("Antenna","M2ARPE_float",0);
	m_options.arp2mark_east=sign*(m_options.arp2mark_east+option/scale);

	sign=app->GetProfileInt("Antenna","M2ARPN_sign",1);
	if (sign!=1)
		sign=-1;
	m_options.arp2mark_north=app->GetProfileInt("Antenna","M2ARPN_int",0);
	option=app->GetProfileInt("Antenna","M2ARPN_float",0);
	m_options.arp2mark_north=sign*(m_options.arp2mark_north+option/scale);

	sign=app->GetProfileInt("Antenna","M2ARPU_sign",1);
	if (sign!=1)
		sign=-1;
	m_options.arp2mark_up=app->GetProfileInt("Antenna","M2ARPU_int",0);
	option=app->GetProfileInt("Antenna","M2ARPU_float",0);
	m_options.arp2mark_up=sign*(m_options.arp2mark_up+option/scale);

	// read ARP to APC offsets
	sign=app->GetProfileInt("Antenna","ARP2APCE_sign",1);
	if (sign!=1)
		sign=-1;
	m_options.arp2apc_east=app->GetProfileInt("Antenna","ARP2APCE_int",0);
	option=app->GetProfileInt("Antenna","ARP2APCE_float",0);
	m_options.arp2apc_east=sign*(m_options.arp2apc_east+option/scale);

	sign=app->GetProfileInt("Antenna","ARP2APCN_sign",1);
	if (sign!=1)
		sign=-1;
	m_options.arp2apc_north=app->GetProfileInt("Antenna","ARP2APCN_int",0);
	option=app->GetProfileInt("Antenna","ARP2APCN_float",0);
	m_options.arp2apc_north=sign*(m_options.arp2apc_north+option/scale);

	sign=app->GetProfileInt("Antenna","ARP2APCU_sign",1);
	if (sign!=1)
		sign=-1;
	m_options.arp2apc_up=app->GetProfileInt("Antenna","ARP2APCU_int",0);
	option=app->GetProfileInt("Antenna","ARP2APCU_float",0);
	m_options.arp2apc_up=sign*(m_options.arp2apc_up+option/scale);
	
	// reading antenna modes
	m_options.arp2m_mode=app->GetProfileInt("Antenna","arp2m_mode",0);	
	m_options.fromPCV=app->GetProfileInt("Antenna","fromPCV",0);
	m_options.pcv_mode=app->GetProfileInt("Antenna","pcv_mode",0);

	// miscallaneous options associated with the CONSTANTS dialog
	m_options.GPS_UTC_offset=app->GetProfileInt("Miscellaneous","GPS-UTC Time Offset",13);
	m_options.elev_mask=app->GetProfileInt("Miscellaneous","Cutoff Elevation",10*iscale)/scale;
	m_options.GDOP_threshold=app->GetProfileInt("Miscellaneous","GDOP Threshold",20*iscale)/scale;
	m_options.sigmaP1=app->GetProfileInt("Miscellaneous","P1 Sigma",3*iscale/10)/scale;
	m_options.sigmaCA=app->GetProfileInt("Miscellaneous","CA Sigma",3*iscale)/scale;
	m_options.sigmaL1=app->GetProfileInt("Miscellaneous","phase Sigma",iscale*2/1000)/scale;
	m_options.sigmaPhaseRate=app->GetProfileInt("Miscellaneous","Phase Rate Sigma",iscale*2/1000)/scale;
	m_options.log_over=app->GetProfileInt("Miscellaneous","log overwrite",0);
	
	// option for overwriting the report file
	m_options.rpt_over=app->GetProfileInt("Miscellaneous","rpt overwrite",0);

	m_options.rpt_datum=app->GetProfileInt("Output","Coord System",0);

	// calculating standard deviations of observation combinations
	m_options.sigmaCP			=	(m_options.sigmaP1+m_options.sigmaL1)/2;
	m_options.sigmaIonoFreeCode	=	m_options.sigmaP1*3;
	m_options.sigmaIonoFreePhase=	m_options.sigmaL1*3;

	if (m_options.ambiguity_flag==0)
	{
		if (m_options.frequency_flag!=0)
		{
			m_options.sigmaIonoFreeCode	=	m_options.sigmaP1*1;
		}
	}
	else
	{
		if (m_options.iono_free_modal==1)
		{
			m_options.sigmaIonoFreeCode	=	(m_options.sigmaP1+m_options.sigmaL1)/2;
		}
	}

	// getting filenames
	setting=app->GetProfileString("Data Files","Observation Filename","");
	strcpy(m_options.obs_filename,LPCTSTR(setting));
	setting=app->GetProfileString("Data Files","Ephemeris Filename","");
	strcpy(m_options.eph_filename,LPCTSTR(setting));

	setting=app->GetProfileString("Data Files","Precise Ephemeris Filename","");
	strcpy(m_options.peph_filename,LPCTSTR(setting));
	setting=app->GetProfileString("Data Files","Precise Clock Filename","");
	strcpy(m_options.pclk_filename,LPCTSTR(setting));

	setting=app->GetProfileString("Data Files","Log Filename","");
	strcpy(m_options.logfile,LPCTSTR(setting));

	setting=app->GetProfileString("Data Files","PCV Filename","");
	strcpy(m_options.pcvfile,LPCTSTR(setting));

	setting=app->GetProfileString("Data Files","Report Filename","");
	strcpy(m_options.reportfile,LPCTSTR(setting));

#ifndef POST_MISSION

	// getting info associated with real-time processing
	m_iRcvType=app->GetProfileInt("Receiver","Receiver Type",0);
	m_iGPSPort=app->GetProfileInt("Receiver","Comm Port",1);
	m_iGPSBaudrate=app->GetProfileInt("Receiver","Baudrate",9600);

	m_iRTCAIP[0]=app->GetProfileInt("RTCA","IP 1",136);
	m_iRTCAIP[1]=app->GetProfileInt("RTCA","IP 2",159);
	m_iRTCAIP[2]=app->GetProfileInt("RTCA","IP 3",122);
	m_iRTCAIP[3]=app->GetProfileInt("RTCA","IP 4",202);
	m_iRTCAPort=app->GetProfileInt("RTCA","IP Port",21502);

#endif

	// getting filter parameters
	setting=app->GetProfileString("Initial SD","Horizontal","1.0e2");
	m_options.init_SD_hor=atof(setting);
	setting=app->GetProfileString("Initial SD","Vertical","1.0e2");
	m_options.init_SD_ver=atof(setting);
	setting=app->GetProfileString("Initial SD","Clock","1.0e5");
	m_options.init_SD_clk=atof(setting);
	setting=app->GetProfileString("Initial SD","Troposphere","1.0e-2");
	m_options.init_SD_trop=atof(setting);
	setting=app->GetProfileString("Initial SD","Ambiguity","1.0e2");
	m_options.init_SD_amb=atof(setting);
	setting=app->GetProfileString("Initial SD","Horizontal Vel","1.0e1");
	m_options.init_SD_vh=atof(setting);
	setting=app->GetProfileString("Initial SD","Vertical Vel","1.0e1");
	m_options.init_SD_vv=atof(setting);
	setting=app->GetProfileString("Initial SD","Clock Drift","1.0e2");
	m_options.init_SD_dt=atof(setting);

	setting=app->GetProfileString("Spectral Density","Horizontal","0.0e0");
	m_options.spec_den_hor=atof(setting);
	setting=app->GetProfileString("Spectral Density","Vertical","0.0e0");
	m_options.spec_den_ver=atof(setting);
	setting=app->GetProfileString("Spectral Density","Clock","1.0e5");
	m_options.spec_den_clk=atof(setting);
	setting=app->GetProfileString("Spectral Density","Troposphere","1.0e-9");
	m_options.spec_den_trop=atof(setting);
	setting=app->GetProfileString("Spectral Density","Ambiguity","0.0e0");
	m_options.spec_den_amb=atof(setting);
	m_options.spec_den_amb=0;
	setting=app->GetProfileString("Spectral Density","Horizontal Vel","1.0e1");
	m_options.spec_den_vh=atof(setting);
	setting=app->GetProfileString("Spectral Density","Vertical Vel","1.0e1");
	m_options.spec_den_vv=atof(setting);
	setting=app->GetProfileString("Spectral Density","Clock Drift","1.0e2");
	m_options.spec_den_dt=atof(setting);

	m_options.corr_length_hor=app->GetProfileInt("Correlation Time","Horizontal",600);
	m_options.corr_length_ver=app->GetProfileInt("Correlation Time","Vertical",600);
	m_options.corr_length_clk=app->GetProfileInt("Correlation Time","Clock",10);

	// results presenting in the report file
	m_options.outputTypes=app->GetProfileInt("Output","Types",0);

	for (l=0;l<m_options.outputTypes;l++)
	{
		setting.Format("Type %d",l+1);
		setting=app->GetProfileString("Output",setting,"GPS Time");
		strcpy(m_options.outputType[l],(LPCTSTR)setting);
	}
}


/* ---------------------------------------------------------------------
Name:			SaveOption

Purpose:		Saving PPP options to application registry.

History:		Name				Date		Reason for modification
                PMIS                2003        Original
--------------------------------------------------------------------- */
void CMainFrame::SaveOption() 
{
	int option;					// for integer options
	CString setting;			// for text options
	CWinApp* app=AfxGetApp();	// obtain the pointer to the application at first

	// for float options which are saved as integers in the registry
	double scale=1000000.0;
	double fscale=0.1;
	int iscale=1000000;
 
	// the following write settings to the the registry,
	app->WriteProfileInt("System","Process Mode",m_options.system_mode);
	app->WriteProfileInt("System","Station Mode",m_options.station_mode);
	app->WriteProfileInt("System","Filter Mode",m_options.filter_mode);
	app->WriteProfileInt("System","Processing Model",m_options.filter_model);
	app->WriteProfileInt("System","Processing Mode",m_options.processing_mode);

	app->WriteProfileInt("Observation","Frequency",m_options.frequency_flag);
	app->WriteProfileInt("Observation","Velocity",m_options.vel_estimate);
	app->WriteProfileInt("Observation","Smooth",m_options.smooth_flag);
	app->WriteProfileInt("Observation","Reset",m_options.reset_epoch);
	app->WriteProfileInt("Observation","Backward",m_options.bBackward);

	option=m_options.BLOCKII[0];
	int l=1;
	for (int k=1;k<31;k++)
	{
		l=l*2;
		option=option+m_options.BLOCKII[k]*l;
	}
	app->WriteProfileInt("Satellites","BlockII < 32",option);
	app->WriteProfileInt("Satellites","BlockII = 32",m_options.BLOCKII[31]);

	option=m_options.SatEx[0];
	l=1;
	for (k=1;k<31;k++)
	{
		l=l*2;
		option=option+m_options.SatEx[k]*l;
	}
	app->WriteProfileInt("SatEx","PRN < 32",option);
	app->WriteProfileInt("SatEx","PRN = 32",m_options.SatEx[31]);

	app->WriteProfileInt("Meteorology","Temperature",(int)(fabs(m_options.temperature)*iscale+fscale));
	if (m_options.temperature<0)
		app->WriteProfileInt("Meteorology","Temperature-Sign",0);
	else
		app->WriteProfileInt("Meteorology","Temperature-Sign",1);

	app->WriteProfileInt("Meteorology","Pressure",(int)(m_options.pressure*iscale+fscale));
	app->WriteProfileInt("Meteorology","Humidity",(int)(m_options.humidity*iscale+fscale));

	app->WriteProfileInt("Unknows","Ambiguity",m_options.ambiguity_flag);
	app->WriteProfileInt("Unknows","Interval",(int)(m_options.obs_interval*10+fscale));
	app->WriteProfileInt("Unknows","Iono_Modal",(int)(m_options.iono_free_modal));

	app->WriteProfileInt("Corrections","Troposphere Correction",m_options.tropo_flag);
	app->WriteProfileInt("Corrections","Ephemeris Correction",m_options.eph_flag);
	app->WriteProfileInt("Corrections","Clock Correction",m_options.clock_flag);

	app->WriteProfileInt("Coordinate","Reference",m_options.ref);

	app->WriteProfileInt("Coordinate","Init Coor Ref",m_options.init_ref_sys);
	
	// save ellipsoidal ITRF00 or WGS84 coordinates
	if(m_options.init_ref_sys != 2)
	{
		double td=fabs(m_options.direca);
		int sign=1;
		if (m_options.direca<0)
			sign=0;
		app->WriteProfileInt("Coordinate","DirecA-Sign",sign);
		app->WriteProfileInt("Coordinate","DirecA-Int",(int)td);
		option=(int)((td-(int)td)*iscale+fscale);
		app->WriteProfileInt("Coordinate","DirecA-Float",option);
		
		td=fabs(m_options.direcb);
		sign=1;
		if (m_options.direcb<0)
			sign=0;
		app->WriteProfileInt("Coordinate","DirecB-Sign",sign);
		app->WriteProfileInt("Coordinate","DirecB-Int",(int)td);
		option=(int)((td-(int)td)*iscale+fscale);
		app->WriteProfileInt("Coordinate","DirecB-Float",option);
		
		td=fabs(m_options.direcc);
		sign=1;
		if (m_options.direcc<0)
			sign=0;
		app->WriteProfileInt("Coordinate","DirecC-Sign",sign);
		app->WriteProfileInt("Coordinate","DirecC-Int",(int)td);
		option=(int)((td-(int)td)*iscale+fscale);
		app->WriteProfileInt("Coordinate","DirecC-Float",option);
	}
	
	// saving ellipsoidal NAD83 coordinates
	else
	{
		double td=fabs(m_options.origNADa);
		int sign=1;
		if (m_options.origNADa<0)
			sign=0;
		app->WriteProfileInt("NADCoordinate","DirecA-Sign",sign);
		app->WriteProfileInt("NADCoordinate","DirecA-Int",(int)td);
		option=(int)((td-(int)td)*iscale+fscale);
		app->WriteProfileInt("NADCoordinate","DirecA-Float",option);
		
		td=fabs(m_options.origNADb);
		sign=1;
		if (m_options.origNADb<0)
			sign=0;
		app->WriteProfileInt("NADCoordinate","DirecB-Sign",sign);
		app->WriteProfileInt("NADCoordinate","DirecB-Int",(int)td);
		option=(int)((td-(int)td)*iscale+fscale);
		app->WriteProfileInt("NADCoordinate","DirecB-Float",option);
		
		td=fabs(m_options.origNADc);
		sign=1;
		if (m_options.origNADc<0)
			sign=0;
		app->WriteProfileInt("NADCoordinate","DirecC-Sign",sign);
		app->WriteProfileInt("NADCoordinate","DirecC-Int",(int)td);
		option=(int)((td-(int)td)*iscale+fscale);
		app->WriteProfileInt("NADCoordinate","DirecC-Float",option);
	}

	app->WriteProfileInt("Coordinate","year",m_options.coord_year);
	app->WriteProfileInt("Coordinate","month",m_options.coord_month);
	app->WriteProfileInt("Coordinate","day",m_options.coord_day);

	app->WriteProfileString("Antenna","Antenna Type",m_options.ant_type);

	// writing Marker to ARP offsets
	double td=fabs(m_options.arp2mark_east);
	int sign=1;
	if (m_options.arp2mark_east<0)
		sign=0;
	app->WriteProfileInt("Antenna","M2ARPE_sign",sign);
	app->WriteProfileInt("Antenna","M2ARPE_int",(int)td);
	option=(int)((td-(int)td)*iscale+fscale);
	app->WriteProfileInt("Antenna","M2ARPE_float",option);

	td=fabs(m_options.arp2mark_north);
	sign=1;
	if (m_options.arp2mark_north<0)
		sign=0;
	app->WriteProfileInt("Antenna","M2ARPN_sign",sign);
	app->WriteProfileInt("Antenna","M2ARPN_int",(int)td);
	option=(int)((td-(int)td)*iscale+fscale);
	app->WriteProfileInt("Antenna","M2ARPN_float",option);

	td=fabs(m_options.arp2mark_up);
	sign=1;
	if (m_options.arp2mark_up<0)
		sign=0;
	app->WriteProfileInt("Antenna","M2ARPU_sign",sign);
	app->WriteProfileInt("Antenna","M2ARPU_int",(int)td);
	option=(int)((td-(int)td)*iscale+fscale);
	app->WriteProfileInt("Antenna","M2ARPU_float",option);

	// writing ARP to APC offsets
	td=fabs(m_options.arp2apc_east);
	sign=1;
	if (m_options.arp2apc_east<0)
		sign=0;
	app->WriteProfileInt("Antenna","ARP2APCE_sign",sign);
	app->WriteProfileInt("Antenna","ARP2APCE_int",(int)td);
	option=(int)((td-(int)td)*iscale+fscale);
	app->WriteProfileInt("Antenna","ARP2APCE_float",option);

	td=fabs(m_options.arp2apc_north);
	sign=1;
	if (m_options.arp2apc_north<0)
		sign=0;
	app->WriteProfileInt("Antenna","ARP2APCN_sign",sign);
	app->WriteProfileInt("Antenna","ARP2APCN_int",(int)td);
	option=(int)((td-(int)td)*iscale+fscale);
	app->WriteProfileInt("Antenna","ARP2APCN_float",option);

	td=fabs(m_options.arp2apc_up);
	sign=1;
	if (m_options.arp2apc_up<0)
		sign=0;
	app->WriteProfileInt("Antenna","ARP2APCU_sign",sign);
	app->WriteProfileInt("Antenna","ARP2APCU_int",(int)td);
	option=(int)((td-(int)td)*iscale+fscale);
	app->WriteProfileInt("Antenna","ARP2APCU_float",option);

	// writing antenna modes
	app->WriteProfileInt("Antenna","arp2m_mode",m_options.arp2m_mode);	
	app->WriteProfileInt("Antenna","fromPCV",m_options.fromPCV);
	app->WriteProfileInt("Antenna","pcv_mode",m_options.pcv_mode);

	app->WriteProfileInt("Miscellaneous","GPS-UTC Time Offset",m_options.GPS_UTC_offset);
	app->WriteProfileInt("Miscellaneous","Cutoff Elevation",(int)(m_options.elev_mask*iscale+fscale));
	app->WriteProfileInt("Miscellaneous","GDOP Threshold",(int)(m_options.GDOP_threshold*iscale+fscale));
	app->WriteProfileInt("Miscellaneous","P1 Sigma",(int)(m_options.sigmaP1*iscale+fscale));
	app->WriteProfileInt("Miscellaneous","CA Sigma",(int)(m_options.sigmaCA*iscale+fscale));
	app->WriteProfileInt("Miscellaneous","Phase Sigma",(int)(m_options.sigmaL1*iscale+fscale));
	app->WriteProfileInt("Miscellaneous","Phase Rate Sigma",(int)(m_options.sigmaPhaseRate*iscale+fscale));
	app->WriteProfileInt("Miscellaneous","log overwrite",m_options.log_over);
	app->WriteProfileInt("Miscellaneous","rpt overwrite",m_options.rpt_over);

	app->WriteProfileString("Data Files","Observation Filename",m_options.obs_filename);
	app->WriteProfileString("Data Files","Ephemeris Filename",m_options.eph_filename);
	app->WriteProfileString("Data Files","Precise Ephemeris Filename",m_options.peph_filename);
	app->WriteProfileString("Data Files","Precise Clock Filename",m_options.pclk_filename);
	app->WriteProfileString("Data Files","Log Filename",m_options.logfile);
	app->WriteProfileString("Data Files","Report Filename",m_options.reportfile);
	app->WriteProfileString("Data Files","PCV Filename",m_options.pcvfile);

	app->WriteProfileInt("Output","Coord System",m_options.rpt_datum);

	char str[128];
	sprintf(str,"%.1E",m_options.init_SD_hor);
	app->WriteProfileString("Initial SD","Horizontal",str);
	sprintf(str,"%.1E",m_options.init_SD_ver);
	app->WriteProfileString("Initial SD","Vertical",str);
	sprintf(str,"%.1E",m_options.init_SD_clk);
	app->WriteProfileString("Initial SD","Clock",str);
	sprintf(str,"%.1E",m_options.init_SD_trop);
	app->WriteProfileString("Initial SD","Troposphere",str);
	sprintf(str,"%.1E",m_options.init_SD_amb);
	app->WriteProfileString("Initial SD","Ambiguity",str);
	sprintf(str,"%.1E",m_options.init_SD_vh);
	app->WriteProfileString("Initial SD","Horizontal Vel",str);
	sprintf(str,"%.1E",m_options.init_SD_vv);
	app->WriteProfileString("Initial SD","Vertical Vel",str);
	sprintf(str,"%.1E",m_options.init_SD_dt);
	app->WriteProfileString("Initial SD","Clock Drift",str);

	sprintf(str,"%.1E",m_options.spec_den_hor);
	app->WriteProfileString("Spectral Density","Horizontal",str);
	sprintf(str,"%.1E",m_options.spec_den_ver);
	app->WriteProfileString("Spectral Density","Vertical",str);
	sprintf(str,"%.1E",m_options.spec_den_clk);
	app->WriteProfileString("Spectral Density","Clock",str);
	sprintf(str,"%.1E",m_options.spec_den_trop);
	app->WriteProfileString("Spectral Density","Troposphere",str);
	sprintf(str,"%.1E",m_options.spec_den_amb);
	app->WriteProfileString("Spectral Density","Ambiguity",str);
	sprintf(str,"%.1E",m_options.spec_den_vh);
	app->WriteProfileString("Spectral Density","Horizontal Vel",str);
	sprintf(str,"%.1E",m_options.spec_den_vv);
	app->WriteProfileString("Spectral Density","Vertical Vel",str);
	sprintf(str,"%.1E",m_options.spec_den_dt);
	app->WriteProfileString("Spectral Density","Clock Drift",str);

	app->WriteProfileInt("Correlation Time","Horizontal",(int)m_options.corr_length_hor);
	app->WriteProfileInt("Correlation Time","Vertical",(int)m_options.corr_length_ver);
	app->WriteProfileInt("Correlation Time","Clock",(int)m_options.corr_length_clk);
}


/* ---------------------------------------------------------------------
Name:			OnSetup

Purpose:		Implementation of SETUP dialog, along with all of its
				associated tabs.  The tabs are: Settings,
				Antenna, Initial Coordinates, Meteorology, and Satellite
				Exclusion.

History:		Name				Date		Reason for modification
                PMIS                2003        Original
--------------------------------------------------------------------- */
void CMainFrame::OnSetup() 
{
	// user attempts to access SETUP parameters during processing
	if (m_bStart)
	{
		MessageBox("You cannot change parameters during processing!","Setup Parameters",MB_ICONINFORMATION);
		return;
	}

	CPropertySheet sheet;

	// dialog objects
	DlgConstants	dlgConstants;	// constants including measurements noise, cutoff, etc
	DlgAntenna		dlgAnt;			// antenna settings
	DlgMet			dlgMet;			// meteorological parameters
	DlgCoordinate	dlgCoordinate;	// initial coordinates
	CDlgSatEx		dlgSatEx;		// excluded satellites

	// Setting the parameters in the SETUP dialog based on what was contained in the
	// application's registry.  The registry values were read in the ReadOption() function.

	// first, pass all settings in the option struct to the dialog
	dlgConstants.m_dElemask			=	m_options.elev_mask;
	dlgConstants.m_dGdopmask		=	m_options.GDOP_threshold;
	dlgConstants.m_dSigmaCA			=	m_options.sigmaCA;
	dlgConstants.m_dSigmaP1			=	m_options.sigmaP1;
	dlgConstants.m_dSigmaPhase		=	m_options.sigmaL1;
	dlgConstants.m_dSigmaPhaseRate	=	m_options.sigmaPhaseRate;
	dlgConstants.m_dInterval		=	m_options.obs_interval;
	dlgConstants.m_iOffset			=	m_options.GPS_UTC_offset;
	dlgConstants.appPath			=	m_strAppPath;

	dlgConstants.m_strLogPath	=	m_options.logfile;
	dlgConstants.m_bLOG			=	m_options.log_over;

	dlgAnt.m_dARP2M_east	=	m_options.arp2mark_east;
	dlgAnt.m_dARP2M_north	=	m_options.arp2mark_north;
	dlgAnt.m_dARP2M_up		=	m_options.arp2mark_up;
	dlgAnt.m_bARP2M_Mode	=	m_options.arp2m_mode;
	dlgAnt.m_strAntType		=	m_options.ant_type;
	dlgAnt.m_strPCVpath		=	m_options.pcvfile;
	dlgAnt.m_bPCV_Mode		=	m_options.pcv_mode;
	dlgAnt.appPath			=	m_strAppPath;
	dlgAnt.m_dARP2APCe		=	m_options.arp2apc_east;
	dlgAnt.m_dARP2APCn		=	m_options.arp2apc_north;
	dlgAnt.m_dARP2APCu		=	m_options.arp2apc_up;
	dlgAnt.m_bFromPCV		=	m_options.fromPCV;

	dlgMet.m_fHumidity	=	m_options.humidity;
	dlgMet.m_fPressure	=	m_options.pressure;
	dlgMet.m_fTemp		=	m_options.temperature;

	dlgCoordinate.m_iCoorSystem = m_options.ref;
	dlgCoordinate.m_iDatum = m_options.init_ref_sys;
	if(m_options.ref<0 || m_options.ref>1)
		dlgCoordinate.m_iCoorSystem=0;

	dlgCoordinate.m_iYear = m_options.coord_year;
	dlgCoordinate.m_iMonth = m_options.coord_month;
	dlgCoordinate.m_iDay = m_options.coord_day;
	
	// Cartesian coordinates
	if (m_options.ref==0)
	{
		if(m_options.init_ref_sys != 2)
		{
			// WGS84, ITRF
			dlgCoordinate.m_dCoorx	=	m_options.direca;
			dlgCoordinate.m_dCoory	=	m_options.direcb;
			dlgCoordinate.m_dCoorz	=	m_options.direcc;
		}
		else
		{
			// NAD83
			dlgCoordinate.m_dCoorx	=	m_options.origNADa;
			dlgCoordinate.m_dCoory	=	m_options.origNADb;
			dlgCoordinate.m_dCoorz	=	m_options.origNADc;
		}
	}

	// Ellipsoidal coordinates
	else
	{
		if(m_options.init_ref_sys != 2)
		{
			// WGS84, ITRF
			double deg=fabs(m_options.direca);
			dlgCoordinate.m_iLatDeg=(int)(deg/10000);
			dlgCoordinate.m_iLatMin=(int)((deg/100-dlgCoordinate.m_iLatDeg*100));
			dlgCoordinate.m_dLatSec=((deg-dlgCoordinate.m_iLatDeg*10000)-dlgCoordinate.m_iLatMin*100);
			if (m_options.direca<0)
				dlgCoordinate.m_iLatDeg=-dlgCoordinate.m_iLatDeg;
			
			deg=fabs(m_options.direcb);
			dlgCoordinate.m_iLonDeg=(int)(deg/10000);
			dlgCoordinate.m_iLonMin=(int)((deg/100-dlgCoordinate.m_iLonDeg*100));
			dlgCoordinate.m_dLonSec=((deg-dlgCoordinate.m_iLonDeg*10000)-dlgCoordinate.m_iLonMin*100);
			if (m_options.direcb<0)
				dlgCoordinate.m_iLonDeg=-dlgCoordinate.m_iLonDeg;
			
			dlgCoordinate.m_dHeight	=	m_options.direcc;
		}
		else
		{
			// NAD83
			double deg=fabs(m_options.origNADa);
			dlgCoordinate.m_iLatDeg=(int)(deg/10000);
			dlgCoordinate.m_iLatMin=(int)((deg/100-dlgCoordinate.m_iLatDeg*100));
			dlgCoordinate.m_dLatSec=((deg-dlgCoordinate.m_iLatDeg*10000)-dlgCoordinate.m_iLatMin*100);
			if (m_options.origNADa<0)
				dlgCoordinate.m_iLatDeg=-dlgCoordinate.m_iLatDeg;
			
			deg=fabs(m_options.origNADb);
			dlgCoordinate.m_iLonDeg=(int)(deg/10000);
			dlgCoordinate.m_iLonMin=(int)((deg/100-dlgCoordinate.m_iLonDeg*100));
			dlgCoordinate.m_dLonSec=((deg-dlgCoordinate.m_iLonDeg*10000)-dlgCoordinate.m_iLonMin*100);
			if (m_options.origNADb<0)
				dlgCoordinate.m_iLonDeg=-dlgCoordinate.m_iLonDeg;
			
			dlgCoordinate.m_dHeight	=	m_options.origNADc;
		}
	}
	
	for (int i=0;i<32;i++)
		dlgSatEx.m_iSats[i]=m_options.SatEx[i];
	
	// add all dialog to the property sheet
	sheet.AddPage((CPropertyPage*)&dlgConstants);
	sheet.AddPage((CPropertyPage*)&dlgAnt);
	sheet.AddPage((CPropertyPage*)&dlgCoordinate);
	sheet.AddPage((CPropertyPage*)&dlgMet);
	sheet.AddPage((CPropertyPage*)&dlgSatEx);

	// set the style and title of the sheet
	UINT u=PSH_HASHELP;
	u=~PSH_HASHELP;
	sheet.m_psh.dwFlags |= PSH_NOAPPLYNOW;
	sheet.m_psh.pszCaption = "Setup";
	sheet.m_psh.nStartPage = 0;

	// Once the user click's "OK", the parameters contained in the dialogs are saved to the
	// COPTION object "m_options" and to the application's registry with the function SaveOption().
	if (sheet.DoModal()==IDOK)
	{
		m_options.elev_mask			=	dlgConstants.m_dElemask;
		m_options.GDOP_threshold	=	dlgConstants.m_dGdopmask;
		m_options.sigmaCA			=	dlgConstants.m_dSigmaCA;
		m_options.sigmaP1			=	dlgConstants.m_dSigmaP1;
		m_options.sigmaL1			=	dlgConstants.m_dSigmaPhase;
		m_options.sigmaPhaseRate	=	dlgConstants.m_dSigmaPhaseRate;
		m_options.obs_interval		=	dlgConstants.m_dInterval;
		m_options.GPS_UTC_offset	=	dlgConstants.m_iOffset;
		
		strcpy(m_options.logfile,LPCTSTR(dlgConstants.m_strLogPath));

		// adding ".log" extension if not provided by the user
		CString file = CString(m_options.logfile);
		CString ext = file.Right(4);
		if(ext != ".log")
			file += ".log";
		
		strcpy(m_options.logfile,LPCTSTR(file));		
		
		m_options.log_over			=	dlgConstants.m_bLOG;

		m_options.sigmaCP			=	(m_options.sigmaP1+m_options.sigmaL1)/2;
		m_options.sigmaIonoFreeCode	=	m_options.sigmaP1*3;
		m_options.sigmaIonoFreePhase=	m_options.sigmaL1*3;

		m_options.arp2mark_east		=	dlgAnt.m_dARP2M_east;
		m_options.arp2mark_north	=	dlgAnt.m_dARP2M_north;
		m_options.arp2mark_up		=	dlgAnt.m_dARP2M_up;
		m_options.arp2m_mode		=	dlgAnt.m_bARP2M_Mode;
		m_options.ant_type			=	dlgAnt.m_strAntVendor+dlgAnt.m_strAntType;
		m_options.pcv_mode			=	dlgAnt.m_bPCV_Mode;
		m_options.arp2apc_east		=	dlgAnt.m_dARP2APCe;
		m_options.arp2apc_north		=	dlgAnt.m_dARP2APCn;
		m_options.arp2apc_up		=	dlgAnt.m_dARP2APCu;
		m_options.fromPCV			=	dlgAnt.m_bFromPCV;

		strcpy(m_options.pcvfile,LPCTSTR(dlgAnt.m_strPCVpath));

		m_options.humidity	=	dlgMet.m_fHumidity;
		m_options.pressure	=	dlgMet.m_fPressure;
		m_options.temperature	=	dlgMet.m_fTemp;

		m_options.ref=dlgCoordinate.m_iCoorSystem;
		if(m_options.ref<0 || m_options.ref>1)
			m_options.ref = 0;
		m_options.init_ref_sys=dlgCoordinate.m_iDatum;

		m_options.coord_year = dlgCoordinate.m_iYear;
		m_options.coord_month = dlgCoordinate.m_iMonth;
		m_options.coord_day = dlgCoordinate.m_iDay;
		
		// Cartesian coordinate system
		if (m_options.ref==0)
		{
			if(m_options.init_ref_sys != 2)
			{
				m_options.direca = dlgCoordinate.m_dCoorx;
				m_options.direcb = dlgCoordinate.m_dCoory;
				m_options.direcc = dlgCoordinate.m_dCoorz;
			}

			if(m_options.init_ref_sys == 2)
			{
				m_options.origNADa = dlgCoordinate.m_dCoorx;
				m_options.origNADb = dlgCoordinate.m_dCoory;
				m_options.origNADc = dlgCoordinate.m_dCoorz;
			}
		}
		
		// Ellipsoidal coordinate system
		if (m_options.ref == 1)
		{
			if(m_options.init_ref_sys != 2)
			{
				double deg=fabs(dlgCoordinate.m_iLatDeg*10000)+fabs(dlgCoordinate.m_iLatMin*100.0)
					+fabs(dlgCoordinate.m_dLatSec);
				if (dlgCoordinate.m_iLatDeg<0)
					deg=-deg;
				m_options.direca=deg;	// latitude in DDMMSS.SSSSS format
				
				deg=fabs(dlgCoordinate.m_iLonDeg*10000)+fabs(dlgCoordinate.m_iLonMin*100.0)
					+fabs(dlgCoordinate.m_dLonSec);
				if (dlgCoordinate.m_iLonDeg<0)
					deg=-deg;
				m_options.direcb=deg;	// longitude in DDMMSS.SSSSS format
				
				m_options.direcc=dlgCoordinate.m_dHeight;	// height
			}
			
			if(m_options.init_ref_sys == 2)
			{
				double deg=fabs(dlgCoordinate.m_iLatDeg*10000)+fabs(dlgCoordinate.m_iLatMin*100.0)
					+fabs(dlgCoordinate.m_dLatSec);
				if (dlgCoordinate.m_iLatDeg<0)
					deg=-deg;
				m_options.origNADa=deg;	// latitude in DDMMSS.SSSSS format
				
				deg=fabs(dlgCoordinate.m_iLonDeg*10000)+fabs(dlgCoordinate.m_iLonMin*100.0)
					+fabs(dlgCoordinate.m_dLonSec);
				if (dlgCoordinate.m_iLonDeg<0)
					deg=-deg;
				m_options.origNADb=deg;	// longitude in DDMMSS.SSSSS format
				
				m_options.origNADc=dlgCoordinate.m_dHeight;	// height
			}
		}		

		for (i=0;i<32;i++)
			m_options.SatEx[i]=dlgSatEx.m_iSats[i];

		// if user accepts the changes, update the registry
		SaveOption();

		// some changes of setting may change the layout of some sub-windows
		// for example, the cutoff 
		if (m_pPicView)
		{
			m_pPicView->Invalidate();
		}

		if (m_pResView)
		{
			m_pResView->Invalidate();
		}
	}
}


/* -------------------------------------------------------------------------------
Name:			OnSettings

Purpose:		Implementation of SETTINGS dialog, which is a part of
				the SETUP dialog.  This function is called when the user
				accesses only the SETTINGS dialog from the SETUP menu.  When
				the SETUP button is pressed, or the SETUP option accessed from
				the SETUP menu, then the function OnSetup() is called instead.

History:		Name				Date		Reason for modification
                PMIS                2003        Original
------------------------------------------------------------------------------- */
void CMainFrame::OnSettings() 
{
	// setting can not be changed during processing
	if (m_bStart)
	{
		MessageBox("You cannot change parameters during processing!","Setup Parameters",MB_ICONINFORMATION);
		return;
	}

	CPropertySheet sheet;

	// pass setting from the option struct to the dialog
	DlgConstants dlgConstants;
	dlgConstants.m_dElemask		=	m_options.elev_mask;
	dlgConstants.m_dGdopmask	=	m_options.GDOP_threshold;
	dlgConstants.m_dSigmaCA		=	m_options.sigmaCA;
	dlgConstants.m_dSigmaP1		=	m_options.sigmaP1;
	dlgConstants.m_dSigmaPhase	=	m_options.sigmaL1;
	dlgConstants.m_dSigmaPhaseRate	=	m_options.sigmaPhaseRate;
	dlgConstants.m_dInterval	=	m_options.obs_interval;
	dlgConstants.m_iOffset		=	m_options.GPS_UTC_offset;
	dlgConstants.appPath		=	m_strAppPath;

	dlgConstants.m_strLogPath	=	m_options.logfile;
	dlgConstants.m_bLOG			=	m_options.log_over;

	// only one dialog is added to the sheet
	sheet.AddPage((CPropertyPage*)&dlgConstants);

	// sheet style and title
	sheet.m_psh.dwFlags |= PSH_NOAPPLYNOW | PSH_DEFAULT;
	sheet.m_psh.pszCaption = "Setup";
	sheet.m_psh.nStartPage = 0;

	if (sheet.DoModal()==IDOK)
	{
		// if user click OK, save settings to the option struct
		m_options.elev_mask			=	dlgConstants.m_dElemask;
		m_options.GDOP_threshold	=	dlgConstants.m_dGdopmask;
		m_options.sigmaCA			=	dlgConstants.m_dSigmaCA;
		m_options.sigmaP1			=	dlgConstants.m_dSigmaP1;
		m_options.sigmaL1			=	dlgConstants.m_dSigmaPhase;
		m_options.sigmaPhaseRate	=	dlgConstants.m_dSigmaPhaseRate;
		m_options.obs_interval		=	dlgConstants.m_dInterval;
		m_options.GPS_UTC_offset	=	dlgConstants.m_iOffset;

		strcpy(m_options.logfile,LPCTSTR(dlgConstants.m_strLogPath));

		// adding ".log" extension if not provided by the user
		CString file = CString(m_options.logfile);
		CString ext = file.Right(4);
		if(ext != ".log")
			file += ".log";

		strcpy(m_options.logfile,LPCTSTR(file));

		m_options.log_over			=	dlgConstants.m_bLOG;

		m_options.sigmaCP			=	(m_options.sigmaP1+m_options.sigmaL1)/2;
		m_options.sigmaIonoFreeCode	=	m_options.sigmaP1*3;
		m_options.sigmaIonoFreePhase=	m_options.sigmaL1*3;

		// update registry
		SaveOption();

		// update the cutoff angle
		if (m_pPicView)
		{
			m_pPicView->Invalidate();
		}
		if (m_pResView)
		{
			m_pResView->Invalidate();
		}
	}	
}


/* -------------------------------------------------------------------------------
Name:			OnAntenna

Purpose:		Implementation of ANTENNA dialog, which is a part of
				the SETUP dialog.  This function is called when the user
				accesses only the ANTENNA dialog from the SETUP menu.  When
				the SETUP button is pressed, or the SETUP option accessed from
				the SETUP menu, then the function OnSetup() is called instead.

History:		Name				Date		Reason for modification
                PMIS                2003        Original
------------------------------------------------------------------------------- */
void CMainFrame::OnAntenna()
{
	// setting can not be changed during processing
	if (m_bStart)
	{
		MessageBox("You cannot change parameters during processing!","Setup Parameters",MB_ICONINFORMATION);
		return;
	}

	CPropertySheet sheet;

	DlgAntenna dlgAnt;

	// pass setting from the option struct to the dialog
	dlgAnt.m_dARP2M_east	=	m_options.arp2mark_east;
	dlgAnt.m_dARP2M_north	=	m_options.arp2mark_north;
	dlgAnt.m_dARP2M_up		=	m_options.arp2mark_up;
	dlgAnt.m_bARP2M_Mode	=	m_options.arp2m_mode;
	dlgAnt.m_strAntType		=	m_options.ant_type;
	dlgAnt.m_bPCV_Mode		=	m_options.pcv_mode;
	dlgAnt.m_strPCVpath		=	m_options.pcvfile;
	dlgAnt.appPath			=	m_strAppPath;
	dlgAnt.m_dARP2APCe		=	m_options.arp2apc_east;
	dlgAnt.m_dARP2APCn		=	m_options.arp2apc_north;
	dlgAnt.m_dARP2APCu		=	m_options.arp2apc_up;
	dlgAnt.m_bFromPCV		=	m_options.fromPCV;

	// only one dialog is added to the sheet
	sheet.AddPage((CPropertyPage*)&dlgAnt);

	// sheet style and title
	sheet.m_psh.dwFlags |= PSH_NOAPPLYNOW | PSH_DEFAULT;
	sheet.m_psh.pszCaption = "Setup";
	sheet.m_psh.nStartPage = 0;

	if (sheet.DoModal()==IDOK)
	{
		// if user click OK, save settings to the option struct
		m_options.arp2mark_east		=	dlgAnt.m_dARP2M_east;
		m_options.arp2mark_north	=	dlgAnt.m_dARP2M_north;
		m_options.arp2mark_up		=	dlgAnt.m_dARP2M_up;
		m_options.arp2m_mode		=	dlgAnt.m_bARP2M_Mode;
		m_options.ant_type			=	dlgAnt.m_strAntVendor+dlgAnt.m_strAntType;
		m_options.pcv_mode			=	dlgAnt.m_bPCV_Mode;
		m_options.arp2apc_east		=	dlgAnt.m_dARP2APCe;
		m_options.arp2apc_north		=	dlgAnt.m_dARP2APCn;
		m_options.arp2apc_up		=	dlgAnt.m_dARP2APCu;
		m_options.fromPCV			=	dlgAnt.m_bFromPCV;

		strcpy(m_options.pcvfile,LPCTSTR(dlgAnt.m_strPCVpath));

		// update registry
		SaveOption();

	}

	return;
}


/* -------------------------------------------------------------------------------
Name:			OnSatExclusion

Purpose:		Implementation of SATELLITES EXCLUSION dialog, which is a part of
				the SETUP dialog.  This function is called when the user
				accesses only the SATELLITES EXCLUSION dialog from the SETUP menu. 
				When the SETUP button is pressed, or the SETUP option accessed from
				the SETUP menu, then the function OnSetup() is called instead.

History:		Name				Date		Reason for modification
                PMIS                2003        Original
------------------------------------------------------------------------------- */
void CMainFrame::OnSatExclusion() 
{
	// setting can not be changed during processing
	if (m_bStart)
	{
		MessageBox("You cannot change parameters during processing!","Setup Parameters",MB_ICONINFORMATION);
		return;
	}

	CPropertySheet sheet;

	CDlgSatEx		dlgSatEx;

	// pass setting from the option struct to the dialog
	for (int i=0;i<32;i++)
		dlgSatEx.m_iSats[i]=m_options.SatEx[i];

	// only one dialog is added to the sheet
	sheet.AddPage((CPropertyPage*)&dlgSatEx);

	// sheet style and title
	sheet.m_psh.dwFlags |= PSH_NOAPPLYNOW | PSH_DEFAULT;
	sheet.m_psh.pszCaption = "Setup";
	sheet.m_psh.nStartPage = 0;

	CString str,str1;
	if (sheet.DoModal()==IDOK)
	{
		// if user click OK, save settings to the option struct
		for (i=0;i<32;i++)
			m_options.SatEx[i]=dlgSatEx.m_iSats[i];

		// update registry
		SaveOption();
	}	
}


/* -------------------------------------------------------------------------------
Name:			OnMeteorology

Purpose:		Implementation of METEOROLOGY dialog, which is a part of
				the SETUP dialog.  This function is called when the user
				accesses only the METEOROLOGY dialog from the SETUP menu. 
				When the SETUP button is pressed, or the SETUP option accessed from
				the SETUP menu, then the function OnSetup() is called instead.

History:		Name				Date		Reason for modification
                PMIS                2003        Original
------------------------------------------------------------------------------- */
void CMainFrame::OnMeteorology() 
{
	// setting can not be changed during processing
	if (m_bStart)
	{
		MessageBox("You cannot change parameters during processing!","Setup Parameters",MB_ICONINFORMATION);
		return;
	}

	CPropertySheet sheet;

	DlgMet			dlgMet;

	// pass setting from the option struct to the dialog
	dlgMet.m_fHumidity	=	m_options.humidity;
	dlgMet.m_fPressure	=	m_options.pressure;
	dlgMet.m_fTemp		=	m_options.temperature;

	// only one dialog is added to the sheet
	sheet.AddPage((CPropertyPage*)&dlgMet);
	
	// sheet style and title
	sheet.m_psh.dwFlags |= PSH_NOAPPLYNOW | PSH_DEFAULT;
	sheet.m_psh.pszCaption = "Setup";
	sheet.m_psh.nStartPage = 0;

	CString str,str1;
	if (sheet.DoModal()==IDOK)
	{
		// if user click OK, save settings to the option struct
		m_options.humidity	=	dlgMet.m_fHumidity;
		m_options.pressure	=	dlgMet.m_fPressure;
		m_options.temperature		=	dlgMet.m_fTemp;

		// update registry
		SaveOption();
	}
}


/* -------------------------------------------------------------------------------
Name:			OnInitialCoor

Purpose:		Implementation of INITIAL COORDINATES dialog, which is a part of
				the SETUP dialog.  This function is called when the user
				accesses only the INITIAL COORDINATES dialog from the SETUP menu. 
				When the SETUP button is pressed, or the SETUP option accessed from
				the SETUP menu, then the function OnSetup() is called instead.

History:		Name				Date		Reason for modification
                PMIS                2003        Original
------------------------------------------------------------------------------- */
void CMainFrame::OnInitialCoor()
{
	// setting can not be changed during processing
	if (m_bStart)
	{
		MessageBox("You cannot change parameters during processing!","Setup Parameters",MB_ICONINFORMATION);
		return;
	}

	CPropertySheet sheet;

	DlgCoordinate	dlgCoordinate;

	// pass setting from the option struct to the dialog
	dlgCoordinate.m_iCoorSystem = m_options.ref;
	dlgCoordinate.m_iDatum = m_options.init_ref_sys;
	if(m_options.ref<0 || m_options.ref>1)
		dlgCoordinate.m_iCoorSystem=0;
	
	dlgCoordinate.m_iYear = m_options.coord_year;
	dlgCoordinate.m_iMonth = m_options.coord_month;
	dlgCoordinate.m_iDay = m_options.coord_day;
	
	// Cartesian coordinates
	if (m_options.ref==0)
	{
		if(m_options.init_ref_sys != 2)
		{
			dlgCoordinate.m_dCoorx	=	m_options.direca;
			dlgCoordinate.m_dCoory	=	m_options.direcb;
			dlgCoordinate.m_dCoorz	=	m_options.direcc;
		}
		else
		{
			dlgCoordinate.m_dCoorx	=	m_options.origNADa;
			dlgCoordinate.m_dCoory	=	m_options.origNADb;
			dlgCoordinate.m_dCoorz	=	m_options.origNADc;
		}
	}

	// Ellipsoidal coordinates
	else
	{
		if(m_options.init_ref_sys != 2)
		{
			double deg=fabs(m_options.direca);
			dlgCoordinate.m_iLatDeg=(int)(deg/10000);
			dlgCoordinate.m_iLatMin=(int)((deg/100-dlgCoordinate.m_iLatDeg*100));
			dlgCoordinate.m_dLatSec=((deg-dlgCoordinate.m_iLatDeg*10000)-dlgCoordinate.m_iLatMin*100);
			if (m_options.direca<0)
				dlgCoordinate.m_iLatDeg=-dlgCoordinate.m_iLatDeg;
			
			deg=fabs(m_options.direcb);
			dlgCoordinate.m_iLonDeg=(int)(deg/10000);
			dlgCoordinate.m_iLonMin=(int)((deg/100-dlgCoordinate.m_iLonDeg*100));
			dlgCoordinate.m_dLonSec=((deg-dlgCoordinate.m_iLonDeg*10000)-dlgCoordinate.m_iLonMin*100);
			if (m_options.direcb<0)
				dlgCoordinate.m_iLonDeg=-dlgCoordinate.m_iLonDeg;
			
			dlgCoordinate.m_dHeight	=	m_options.direcc;
		}
		else
		{
			double deg=fabs(m_options.origNADa);
			dlgCoordinate.m_iLatDeg=(int)(deg/10000);
			dlgCoordinate.m_iLatMin=(int)((deg/100-dlgCoordinate.m_iLatDeg*100));
			dlgCoordinate.m_dLatSec=((deg-dlgCoordinate.m_iLatDeg*10000)-dlgCoordinate.m_iLatMin*100);
			if (m_options.origNADa<0)
				dlgCoordinate.m_iLatDeg=-dlgCoordinate.m_iLatDeg;
			
			deg=fabs(m_options.origNADb);
			dlgCoordinate.m_iLonDeg=(int)(deg/10000);
			dlgCoordinate.m_iLonMin=(int)((deg/100-dlgCoordinate.m_iLonDeg*100));
			dlgCoordinate.m_dLonSec=((deg-dlgCoordinate.m_iLonDeg*10000)-dlgCoordinate.m_iLonMin*100);
			if (m_options.origNADb<0)
				dlgCoordinate.m_iLonDeg=-dlgCoordinate.m_iLonDeg;
			
			dlgCoordinate.m_dHeight	=	m_options.origNADc;
		}
	}
	
	// only one dialog is added to the sheet
	sheet.AddPage((CPropertyPage*)&dlgCoordinate);

	// sheet style and title
	sheet.m_psh.dwFlags |= PSH_NOAPPLYNOW | PSH_DEFAULT;
	sheet.m_psh.pszCaption = "Setup";
	sheet.m_psh.nStartPage = 0;

	if (sheet.DoModal()==IDOK)
	{
		// if user click OK, save settings to the option struct
		// Cartesian: m_iCoorSystem = 0; Ellipsoidal: m_iCoorSystem = 1
		m_options.ref=dlgCoordinate.m_iCoorSystem;
		if(m_options.ref<0 || m_options.ref>1)
			m_options.ref = 0;

		// WGS84: m_iDatum = 0; ITRF00: m_iDatum = 1; NAD83: m_iDatum = 2
		m_options.init_ref_sys=dlgCoordinate.m_iDatum;
		
		m_options.coord_year = dlgCoordinate.m_iYear;
		m_options.coord_month = dlgCoordinate.m_iMonth;
		m_options.coord_day = dlgCoordinate.m_iDay;
		
		// Cartesian coordinate system
		if (m_options.ref==0)
		{
			if(m_options.init_ref_sys != 2)
			{
				m_options.direca = dlgCoordinate.m_dCoorx;
				m_options.direcb = dlgCoordinate.m_dCoory;
				m_options.direcc = dlgCoordinate.m_dCoorz;
			}
			else
			{
				m_options.origNADa = dlgCoordinate.m_dCoorx;
				m_options.origNADb = dlgCoordinate.m_dCoory;
				m_options.origNADc = dlgCoordinate.m_dCoorz;
			}
		}
		
		// Ellipsoidal coordinate system
		if (m_options.ref == 1)
		{
			if(m_options.init_ref_sys != 2)
			{
				double deg=fabs(dlgCoordinate.m_iLatDeg*10000)+fabs(dlgCoordinate.m_iLatMin*100.0)
					+fabs(dlgCoordinate.m_dLatSec);
				if (dlgCoordinate.m_iLatDeg<0)
					deg=-deg;
				m_options.direca=deg;	// latitude in DDMMSS.SSSSS format
				
				deg=fabs(dlgCoordinate.m_iLonDeg*10000)+fabs(dlgCoordinate.m_iLonMin*100.0)
					+fabs(dlgCoordinate.m_dLonSec);
				if (dlgCoordinate.m_iLonDeg<0)
					deg=-deg;
				m_options.direcb=deg;	// longitude in DDMMSS.SSSSS format
				
				m_options.direcc=dlgCoordinate.m_dHeight;	// height
			}
			else
			{
				double deg=fabs(dlgCoordinate.m_iLatDeg*10000)+fabs(dlgCoordinate.m_iLatMin*100.0)
					+fabs(dlgCoordinate.m_dLatSec);
				if (dlgCoordinate.m_iLatDeg<0)
					deg=-deg;
				m_options.origNADa=deg;	// latitude in DDMMSS.SSSSS format
				
				deg=fabs(dlgCoordinate.m_iLonDeg*10000)+fabs(dlgCoordinate.m_iLonMin*100.0)
					+fabs(dlgCoordinate.m_dLonSec);
				if (dlgCoordinate.m_iLonDeg<0)
					deg=-deg;
				m_options.origNADb=deg;	// longitude in DDMMSS.SSSSS format
				
				m_options.origNADc=dlgCoordinate.m_dHeight;	// height
			}
		}		

		// update registry
		SaveOption();
	}	
}


/* -------------------------------------------------------------------------------
Name:			OnPpp

Purpose:		Implementation of PPP dialog, either in post-mission or real-time
				mode. Setup for PPP processing

History:		Name				Date		Reason for modification
                PMIS                2003        Original
------------------------------------------------------------------------------- */
void CMainFrame::OnPpp() 
{
	// prevent from changing during processing
	if (m_bStart)
	{
		MessageBox("You cannot change parameters during processing!","Process Parameters",MB_ICONINFORMATION);
		return;
	}

	// PPP dialog in post-mission processing
	if (m_options.system_mode==1)
	{
		CDlgPppPost dlg(this);

		// pass setting from the option struct to the dialog
		dlg.m_bBackward	= m_options.bBackward;
		dlg.m_iOrbit	= m_options.eph_flag-1;
		dlg.m_iClock	= m_options.clock_flag-1;
		if (dlg.m_iOrbit<0)
			dlg.m_iOrbit=0;
		if (dlg.m_iClock<0)
			dlg.m_iClock=0;

		// Block II satellites
		for (int i=0;i<32;i++)
			dlg.BLOCKII[i]=m_options.BLOCKII[i];

		// observation, ephemeris files
		dlg.m_strObsPath=m_options.obs_filename;
		dlg.m_strEphPath=m_options.eph_filename;
		dlg.m_strPEphPath=m_options.peph_filename;
		dlg.m_strPClkPath=m_options.pclk_filename;

		// ionosphere-free combinations
		dlg.m_iModel = m_options.iono_free_modal;
		// estimate velocity or not
		dlg.m_bVel = m_options.vel_estimate;
		// estimate tropospheric delay or model it
		dlg.m_bTrop = m_options.tropo_flag;
		// smooth code with carrier phase or not
		dlg.m_bSmoothing = m_options.smooth_flag;

		// kinematic or static
		dlg.m_iStationMode=m_options.station_mode;

		// initial uncertainties of unknowns
		dlg.m_dStdHor=m_options.init_SD_hor;
		dlg.m_dStdVer=m_options.init_SD_ver;
		dlg.m_dStdClk=m_options.init_SD_clk;
		dlg.m_dStdTrop=m_options.init_SD_trop;
		dlg.m_dStdAmb=m_options.init_SD_amb;
		dlg.m_dStdHorVel=m_options.init_SD_vh;
		dlg.m_dStdVerVel=m_options.init_SD_vv;
		dlg.m_dStdClkVel=m_options.init_SD_dt;

		// spectral density of unknowns
		dlg.m_dSdHor=m_options.spec_den_hor;
		dlg.m_dSdVer=m_options.spec_den_ver;
		dlg.m_dSdClk=m_options.spec_den_clk;
		dlg.m_dSdTrop=m_options.spec_den_trop;
		dlg.m_dSdAmb=m_options.spec_den_amb;
		dlg.m_dSdHorVel=m_options.spec_den_vh;
		dlg.m_dSdVerVel=m_options.spec_den_vv;
		dlg.m_dSdClkVel=m_options.spec_den_dt;

		// correlation time
		dlg.m_iCT_Hor=m_options.corr_length_hor;
		dlg.m_iCT_Ver=m_options.corr_length_ver;
		dlg.m_iCT_Clk=m_options.corr_length_clk;

		// 1=Random Walk, 0=Gauss Markov
		dlg.m_iFilter=m_options.filter_model;

		if (dlg.DoModal()==IDOK)
		{ 
			// if user accepts, save the settings to the option
			m_options.eph_flag=dlg.m_iOrbit+1;
			m_options.clock_flag=dlg.m_iClock+1;

			for (int i=0;i<32;i++)
				m_options.BLOCKII[i]=dlg.BLOCKII[i];

			// observation, ephemeris files
			strcpy(m_options.peph_filename,LPCTSTR(dlg.m_strPEphPath));
			strcpy(m_options.eph_filename,LPCTSTR(dlg.m_strEphPath));
			strcpy(m_options.pclk_filename,LPCTSTR(dlg.m_strPClkPath));
			strcpy(m_options.obs_filename,LPCTSTR(dlg.m_strObsPath));

			// ionosphere-free combinations
			m_options.iono_free_modal=dlg.m_iModel;
			// estimate velocity or not
			m_options.vel_estimate = dlg.m_bVel;
			// estimate tropospheric delay or model it
			m_options.tropo_flag=dlg.m_bTrop;
			// smooth code with carrier phase or not
			m_options.smooth_flag=dlg.m_bSmoothing;

			// kinematic or static
			m_options.station_mode=dlg.m_iStationMode;
			if (m_options.station_mode==0)
				m_options.vel_estimate=0;

			//PPP
			m_options.processing_mode=1;

			m_epoch.WorkingMode = PUREPPPMODE;

			// using dual-frequency data
			m_options.frequency_flag = 0;

			// esimate ambiguity
			m_options.ambiguity_flag=1;

			// initial uncertainties of unknowns
			m_options.init_SD_hor=dlg.m_dStdHor;
			m_options.init_SD_ver=dlg.m_dStdVer;
			m_options.init_SD_clk=dlg.m_dStdClk;
			m_options.init_SD_trop=dlg.m_dStdTrop;
			m_options.init_SD_amb=dlg.m_dStdAmb;
			m_options.init_SD_vh=dlg.m_dStdHorVel;
			m_options.init_SD_vv=dlg.m_dStdVerVel;
			m_options.init_SD_dt=dlg.m_dStdClkVel;

			// spectral density of unknowns
			m_options.spec_den_hor=dlg.m_dSdHor;
			m_options.spec_den_ver=dlg.m_dSdVer;
			m_options.spec_den_clk=dlg.m_dSdClk;
			m_options.spec_den_trop=dlg.m_dSdTrop;
			m_options.spec_den_amb=dlg.m_dSdAmb;
			m_options.spec_den_vh=dlg.m_dSdHorVel;
			m_options.spec_den_vv=dlg.m_dSdVerVel;
			m_options.spec_den_dt=dlg.m_dSdClkVel;

			// correlation time
			m_options.corr_length_hor=dlg.m_iCT_Hor;
			m_options.corr_length_ver=dlg.m_iCT_Ver;
			m_options.corr_length_clk=dlg.m_iCT_Clk;

			// 1=Random Walk, 0=Gauss Markov
			m_options.filter_model=dlg.m_iFilter;

			// the coordinate system of the results are depending on the orbit and clock products
			if (m_options.eph_flag==1 || m_options.clock_flag==1 || m_options.clock_flag==2)
				m_options.res_ref_sys=1;
			else if (m_options.eph_flag==2 || m_options.clock_flag==3)
				m_options.res_ref_sys=2;
			else 
				m_options.res_ref_sys=0;

			m_options.bBackward=dlg.m_bBackward;

			// show positioning errors in static mode and trajectory in kinematic
			// and clear all saved data
			if (m_pTrajView)
			{
				m_pTrajView->ClearData();
			}

			// the number of residuals for each satellite maybe changed
			if (m_pRngView)
			{
				m_pRngView->ClearData();
			}

			// update registry
			SaveOption();

			m_epoch.WorkingMode = PUREPPPMODE;
		}
	}

#ifndef POST_MISSION

	// PPP dialog in real-time processing
	else
	{
		CDlgPppRT dlg(this);

		// only two types of precise orbit and clock can be used in real-time PPP
		/*
		if (m_options.eph_flag==3)
			dlg.m_iOrbitClock=1;	// GPS*C
		else
			dlg.m_iOrbitClock=0;	// IGDG
			*/

		// Bei modified in April 2013
		// Use sp3 file: igu eph and clock
		m_options.eph_flag = 100;
		m_options.clock_flag = 100;
		// Bei modified in April 2013

		// serial port settings
		dlg.m_iBaudrate=m_iGPSBaudrate;
		dlg.m_iPort=m_iGPSPort-1;
		dlg.m_iReceiver=m_iRcvType;

		// socket settings
		for (int i=0;i<4;i++)
			dlg.m_iIP[i]=m_iRTCAIP[i];
		dlg.m_iIPPort=m_iRTCAPort;


		// ionosphere-free combinations
		dlg.m_iModel = m_options.iono_free_modal;
		// estimate velocity or not
		dlg.m_bVel = m_options.vel_estimate;
		// estimate tropospheric delay or model it
		dlg.m_bTrop = m_options.tropo_flag;
		// smooth code with carrier phase or not
		dlg.m_bSmoothing = m_options.smooth_flag;

		// kinematic or static
		dlg.m_iStationMode=m_options.station_mode;

		// initial uncertainties of unknowns
		dlg.m_dStdHor=m_options.init_SD_hor;
		dlg.m_dStdVer=m_options.init_SD_ver;
		dlg.m_dStdClk=m_options.init_SD_clk;
		dlg.m_dStdTrop=m_options.init_SD_trop;
		dlg.m_dStdAmb=m_options.init_SD_amb;
		dlg.m_dStdHorVel=m_options.init_SD_vh;
		dlg.m_dStdVerVel=m_options.init_SD_vv;
		dlg.m_dStdClkVel=m_options.init_SD_dt;

		// spectral density of unknowns
		dlg.m_dSdHor=m_options.spec_den_hor;
		dlg.m_dSdVer=m_options.spec_den_ver;
		dlg.m_dSdClk=m_options.spec_den_clk;
		dlg.m_dSdTrop=m_options.spec_den_trop;
		dlg.m_dSdAmb=m_options.spec_den_amb;
		dlg.m_dSdHorVel=m_options.spec_den_vh;
		dlg.m_dSdVerVel=m_options.spec_den_vv;
		dlg.m_dSdClkVel=m_options.spec_den_dt;

		// correlation time
		dlg.m_iCT_Hor=m_options.corr_length_hor;
		dlg.m_iCT_Ver=m_options.corr_length_ver;
		dlg.m_iCT_Clk=m_options.corr_length_clk;

		// 1=Random Walk, 0=Gauss Markov
		dlg.m_iFilter=m_options.filter_model;

		if (dlg.DoModal()==IDOK)
		{ 
			if (dlg.m_iOrbitClock==0)
			{
				// GPS*C corrections
				m_options.eph_flag=2;
				m_options.clock_flag=3;
			}
			else if (dlg.m_iOrbitClock==1)
			{
				// IGDG corrections
				m_options.eph_flag=3;
				m_options.clock_flag=4;
			}

			// serial port settings
			m_iRcvType=dlg.m_iReceiver;
			m_iGPSPort=dlg.m_iPort+1;
			m_iGPSBaudrate=dlg.m_iBaudrate;

			// socket settings
			for (i=0;i<4;i++)
				m_iRTCAIP[i]=dlg.m_iIP[i];
			m_iRTCAPort=dlg.m_iIPPort;

			// using dual-frequency data
			m_options.frequency_flag = 0;

			// ionosphere-free combinations
			m_options.iono_free_modal=dlg.m_iModel;
			// estimate velocity or not
			m_options.vel_estimate = dlg.m_bVel;
			// estimate tropospheric delay or model it
			m_options.tropo_flag=dlg.m_bTrop;
			// smooth code with carrier phase or not
			m_options.smooth_flag=dlg.m_bSmoothing;

			// kinematic or static
			m_options.station_mode=dlg.m_iStationMode;

			// PPP
			m_options.processing_mode=1;

			// estimate ambiguity
			m_options.ambiguity_flag=1;

			// initial uncertainties of unknowns
			m_options.init_SD_hor=dlg.m_dStdHor;
			m_options.init_SD_ver=dlg.m_dStdVer;
			m_options.init_SD_clk=dlg.m_dStdClk;
			m_options.init_SD_trop=dlg.m_dStdTrop;
			m_options.init_SD_amb=dlg.m_dStdAmb;
			m_options.init_SD_vh=dlg.m_dStdHorVel;
			m_options.init_SD_vv=dlg.m_dStdVerVel;
			m_options.init_SD_dt=dlg.m_dStdClkVel;

			// spectral density of unknowns
			m_options.spec_den_hor=dlg.m_dSdHor;
			m_options.spec_den_ver=dlg.m_dSdVer;
			m_options.spec_den_clk=dlg.m_dSdClk;
			m_options.spec_den_trop=dlg.m_dSdTrop;
			m_options.spec_den_amb=dlg.m_dSdAmb;
			m_options.spec_den_vh=dlg.m_dSdHorVel;
			m_options.spec_den_vv=dlg.m_dSdVerVel;
			m_options.spec_den_dt=dlg.m_dSdClkVel;

			// correlation time
			m_options.corr_length_hor=dlg.m_iCT_Hor;
			m_options.corr_length_ver=dlg.m_iCT_Ver;
			m_options.corr_length_clk=dlg.m_iCT_Clk;

			// 1=Random Walk, 0=Gauss Markov
			m_options.filter_model=dlg.m_iFilter;

			// the coordinate system of the results are depending on the orbit and clock products
			if (m_options.eph_flag==1 || m_options.clock_flag==1 || m_options.clock_flag==2)
				m_options.res_ref_sys=1;
			else if (m_options.eph_flag==2 || m_options.clock_flag==3)
				m_options.res_ref_sys=2;
			else 
				m_options.res_ref_sys=0;

			// show positioning errors in static mode and trajectory in kinematic
			// and clear all saved data
			if (m_pTrajView)
			{
				m_pTrajView->ClearData();
			}

			// the number of residuals for each satellite maybe changed
			if (m_pRngView)
			{
				m_pRngView->ClearData();
			}

			// update registry
			SaveOption();

			m_epoch.WorkingMode = PUREPPPMODE;

			CWinApp* app=AfxGetApp();

			// save some options only relevant to real-time processing
			// receiver settings
			app->WriteProfileInt("Receiver","Receiver Type",m_iRcvType);
			app->WriteProfileInt("Receiver","Comm Port",m_iGPSPort);
			app->WriteProfileInt("Receiver","Baudrate",m_iGPSBaudrate);

			// socket settings
			app->WriteProfileInt("RTCA","IP 1",m_iRTCAIP[0]);
			app->WriteProfileInt("RTCA","IP 2",m_iRTCAIP[1]);
			app->WriteProfileInt("RTCA","IP 3",m_iRTCAIP[2]);
			app->WriteProfileInt("RTCA","IP 4",m_iRTCAIP[3]);
			app->WriteProfileInt("RTCA","IP Port",m_iRTCAPort);
		}
	}	

#endif

}


/* -------------------------------------------------------------------------------
Name:			OnUpdatePpp

Arguments:		CCmdUI* pCmdUI - object contains information of a menu

History:		Name				Date		Reason for modification
                PMIS                2003        Original
------------------------------------------------------------------------------- */
void CMainFrame::OnUpdatePpp(CCmdUI* pCmdUI) 
{
}


/* -------------------------------------------------------------------------------
Name:			OnSpp

Purpose:		Implementation of SPP dialog, either in post-mission or real-time
				mode.

History:		Name				Date		Reason for modification
                PMIS                2003        Original
------------------------------------------------------------------------------- */
void CMainFrame::OnSpp() 
{
	// prevent from changing during processing
	if (m_bStart)
	{
		MessageBox("You cannot change parameters during processing!","Process Parameters",MB_ICONINFORMATION);
		return;
	}

	// SPP dialog in post-mission mode
	if (m_options.system_mode==1)
	{
		CDlgSppPost dlg(this);

		// pass setting from the option struct to the dialog
		dlg.m_iOrbit	= m_options.eph_flag;
		dlg.m_iClock	= m_options.clock_flag;

		// Block II satellites
		for (int i=0;i<32;i++)
			dlg.BLOCKII[i]=m_options.BLOCKII[i];

		// observation, ephemeris files
		dlg.m_strObsPath=m_options.obs_filename;
		dlg.m_strEphPath=m_options.eph_filename;
		dlg.m_strPEphPath=m_options.peph_filename;
		dlg.m_strPClkPath=m_options.pclk_filename;

		// L1 only or L1&L2
		if(m_options.frequency_flag == 1)
			dlg.m_iFrequency = 0;
		else if(m_options.frequency_flag == 0)
			dlg.m_iFrequency = 1;

		// smooth code with carrier phase or not
		dlg.m_bSmoothing = m_options.smooth_flag;

		// kinematic or static
		dlg.m_iStationMode=m_options.station_mode;

		// initial uncertainties of unknowns
		dlg.m_dStdHor=m_options.init_SD_hor;
		dlg.m_dStdVer=m_options.init_SD_ver;
		dlg.m_dStdClk=m_options.init_SD_clk;
		dlg.m_dStdTrop=m_options.init_SD_trop;
		dlg.m_dStdAmb=m_options.init_SD_amb;
		dlg.m_dStdHorVel=m_options.init_SD_vh;
		dlg.m_dStdVerVel=m_options.init_SD_vv;
		dlg.m_dStdClkVel=m_options.init_SD_dt;

		// spectral density of unknowns
		dlg.m_dSdHor=m_options.spec_den_hor;
		dlg.m_dSdVer=m_options.spec_den_ver;
		dlg.m_dSdClk=m_options.spec_den_clk;
		dlg.m_dSdTrop=m_options.spec_den_trop;
		dlg.m_dSdAmb=m_options.spec_den_amb;
		dlg.m_dSdHorVel=m_options.spec_den_vh;
		dlg.m_dSdVerVel=m_options.spec_den_vv;
		dlg.m_dSdClkVel=m_options.spec_den_dt;

		// correlation time
		dlg.m_iCT_Hor=m_options.corr_length_hor;
		dlg.m_iCT_Ver=m_options.corr_length_ver;
		dlg.m_iCT_Clk=m_options.corr_length_clk;

		// 1=Random Walk, 0=Gauss Markov
		dlg.m_iFilter=m_options.filter_model;

		if (dlg.DoModal()==IDOK)
		{ 
			// if user accepts, save the settings to the option
			m_options.eph_flag=dlg.m_iOrbit;
			m_options.clock_flag=dlg.m_iClock;
			for (int i=0;i<32;i++)
				m_options.BLOCKII[i]=dlg.BLOCKII[i];
			// observation, ephemeris files
			strcpy(m_options.peph_filename,LPCTSTR(dlg.m_strPEphPath));
			strcpy(m_options.eph_filename,LPCTSTR(dlg.m_strEphPath));
			strcpy(m_options.pclk_filename,LPCTSTR(dlg.m_strPClkPath));
			strcpy(m_options.obs_filename,LPCTSTR(dlg.m_strObsPath));

			// L1 only or L1&L2
			if(dlg.m_iFrequency == 0)
				m_options.frequency_flag = 1;
			else if(dlg.m_iFrequency == 1)
				m_options.frequency_flag = 0;

			// smooth code with carrier phase or not
			m_options.smooth_flag=dlg.m_bSmoothing;

			// kinematic or static
			m_options.station_mode=dlg.m_iStationMode;

			// SPP
			m_options.processing_mode=0;

			m_epoch.WorkingMode = 0;

			// do not estimate ambiguity
			m_options.ambiguity_flag=0;
			// do not estimate tropospheric delay
			m_options.tropo_flag=0;
			// do not estimate velocity
			m_options.vel_estimate=0;

			// initial uncertainties of unknowns
			m_options.init_SD_hor=dlg.m_dStdHor;
			m_options.init_SD_ver=dlg.m_dStdVer;
			m_options.init_SD_clk=dlg.m_dStdClk;
			m_options.init_SD_trop=dlg.m_dStdTrop;
			m_options.init_SD_amb=dlg.m_dStdAmb;
			m_options.init_SD_vh=dlg.m_dStdHorVel;
			m_options.init_SD_vv=dlg.m_dStdVerVel;
			m_options.init_SD_dt=dlg.m_dStdClkVel;

			// spectral density of unknowns
			m_options.spec_den_hor=dlg.m_dSdHor;
			m_options.spec_den_ver=dlg.m_dSdVer;
			m_options.spec_den_clk=dlg.m_dSdClk;
			m_options.spec_den_trop=dlg.m_dSdTrop;
			m_options.spec_den_amb=dlg.m_dSdAmb;
			m_options.spec_den_vh=dlg.m_dSdHorVel;
			m_options.spec_den_vv=dlg.m_dSdVerVel;
			m_options.spec_den_dt=dlg.m_dSdClkVel;

			// correlation time
			m_options.corr_length_hor=dlg.m_iCT_Hor;
			m_options.corr_length_ver=dlg.m_iCT_Ver;
			m_options.corr_length_clk=dlg.m_iCT_Clk;

			// 1=Random Walk, 0=Gauss Markov
			m_options.filter_model=dlg.m_iFilter;

			// the coordinate system of the results are depending on the orbit and clock products
			if (m_options.eph_flag==1 || m_options.clock_flag==1 || m_options.clock_flag==2)
				m_options.res_ref_sys=1;
			else if (m_options.eph_flag==2 || m_options.clock_flag==3)
				m_options.res_ref_sys=2;
			else 
				m_options.res_ref_sys=0;

			// show positioning errors in static mode and trajectory in kinematic
			// and clear all saved data
			if (m_pTrajView)
			{
				m_pTrajView->ClearData();
			}

			// the number of residuals for each satellite maybe changed
			if (m_pRngView)
			{
				m_pRngView->ClearData();
			}

			// update registry
			SaveOption();
		}
	}

#ifndef POST_MISSION

	// SPP dialog in real-time mode
	else
	{

		CDlgSppRT dlg(this);

		// there are three types of orbit and clock products can be used in real-time PPP
		if (m_options.eph_flag==2 || m_options.eph_flag ==3)
			dlg.m_iOrbitClock=m_options.eph_flag-1;	// GPS*C or IGDG
		else
			dlg.m_iOrbitClock=0;	// broadcast

		// serial port settings
		dlg.m_iBaudrate=m_iGPSBaudrate;
		dlg.m_iPort=m_iGPSPort-1;
		dlg.m_iReceiver=m_iRcvType;

		// socket settings
		for (int i=0;i<4;i++)
			dlg.m_iIP[i]=m_iRTCAIP[i];
		dlg.m_iIPPort=m_iRTCAPort;

		// L1 only or L1&L2
		if(m_options.frequency_flag == 1)
			dlg.m_iFrequency = 0;
		else if(m_options.frequency_flag == 0)
			dlg.m_iFrequency = 1;
		// smooth code with carrier phase or not
		dlg.m_bSmoothing = m_options.smooth_flag;

		// kinematic or static
		dlg.m_iStationMode=m_options.station_mode;

		// initial uncertainties of unknowns
		dlg.m_dStdHor=m_options.init_SD_hor;
		dlg.m_dStdVer=m_options.init_SD_ver;
		dlg.m_dStdClk=m_options.init_SD_clk;
		dlg.m_dStdTrop=m_options.init_SD_trop;
		dlg.m_dStdAmb=m_options.init_SD_amb;
		dlg.m_dStdHorVel=m_options.init_SD_vh;
		dlg.m_dStdVerVel=m_options.init_SD_vv;
		dlg.m_dStdClkVel=m_options.init_SD_dt;

		// spectral density of unknowns
		dlg.m_dSdHor=m_options.spec_den_hor;
		dlg.m_dSdVer=m_options.spec_den_ver;
		dlg.m_dSdClk=m_options.spec_den_clk;
		dlg.m_dSdTrop=m_options.spec_den_trop;
		dlg.m_dSdAmb=m_options.spec_den_amb;
		dlg.m_dSdHorVel=m_options.spec_den_vh;
		dlg.m_dSdVerVel=m_options.spec_den_vv;
		dlg.m_dSdClkVel=m_options.spec_den_dt;

		// correlation time
		dlg.m_iCT_Hor=m_options.corr_length_hor;
		dlg.m_iCT_Ver=m_options.corr_length_ver;
		dlg.m_iCT_Clk=m_options.corr_length_clk;

		// 1=Random Walk, 0=Gauss Markov
		dlg.m_iFilter=m_options.filter_model;

		if (dlg.DoModal()==IDOK)
		{ 
			if (dlg.m_iOrbitClock==1)
			{
				// GPS*C corrections
				m_options.eph_flag=2;
				m_options.clock_flag=3;
			}
			else if (dlg.m_iOrbitClock==2)
			{
				// IGDG corrections
				m_options.eph_flag=3;
				m_options.clock_flag=4;
			}
			else
			{
				// broadcast
				m_options.eph_flag=0;
				m_options.clock_flag=0;
			}

			// serial port settings
			m_iRcvType=dlg.m_iReceiver;
			m_iGPSPort=dlg.m_iPort+1;
			m_iGPSBaudrate=dlg.m_iBaudrate;

			// socket settings
			for (i=0;i<4;i++)
				m_iRTCAIP[i]=dlg.m_iIP[i];
			m_iRTCAPort=dlg.m_iIPPort;

			// L1 only or L1&L2
			if(dlg.m_iFrequency == 0)
				m_options.frequency_flag = 1;
			else if(dlg.m_iFrequency == 1)
				m_options.frequency_flag = 0;

			// smooth code with carrier phase or not
			m_options.smooth_flag=dlg.m_bSmoothing;

			// kinematic or static
			m_options.station_mode=dlg.m_iStationMode;

			// SPP
			m_options.processing_mode=0;

			// do not estimate ambiguity
			m_options.ambiguity_flag=0;
			// do not estimate tropospheric delay
			m_options.tropo_flag=0;
			// do not estimate velocity
			m_options.vel_estimate=0;

			// initial uncertainties of unknowns
			m_options.init_SD_hor=dlg.m_dStdHor;
			m_options.init_SD_ver=dlg.m_dStdVer;
			m_options.init_SD_clk=dlg.m_dStdClk;
			m_options.init_SD_trop=dlg.m_dStdTrop;
			m_options.init_SD_amb=dlg.m_dStdAmb;
			m_options.init_SD_vh=dlg.m_dStdHorVel;
			m_options.init_SD_vv=dlg.m_dStdVerVel;
			m_options.init_SD_dt=dlg.m_dStdClkVel;

			// spectral density of unknowns
			m_options.spec_den_hor=dlg.m_dSdHor;
			m_options.spec_den_ver=dlg.m_dSdVer;
			m_options.spec_den_clk=dlg.m_dSdClk;
			m_options.spec_den_trop=dlg.m_dSdTrop;
			m_options.spec_den_amb=dlg.m_dSdAmb;
			m_options.spec_den_vh=dlg.m_dSdHorVel;
			m_options.spec_den_vv=dlg.m_dSdVerVel;
			m_options.spec_den_dt=dlg.m_dSdClkVel;

			// correlation time
			m_options.corr_length_hor=dlg.m_iCT_Hor;
			m_options.corr_length_ver=dlg.m_iCT_Ver;
			m_options.corr_length_clk=dlg.m_iCT_Clk;

			// 1=Random Walk, 0=Gauss Markov
			m_options.filter_model=dlg.m_iFilter;

			// the coordinate system of the results are depending on the orbit and clock products
			if (m_options.eph_flag==1 || m_options.clock_flag==1 || m_options.clock_flag==2)
				m_options.res_ref_sys=1;
			else if (m_options.eph_flag==2 || m_options.clock_flag==3)
				m_options.res_ref_sys=2;
			else 
				m_options.res_ref_sys=0;

			// show positioning errors in static mode and trajectory in kinematic
			// and clear all saved data
			if (m_pTrajView)
			{
				m_pTrajView->ClearData();
			}

			// the number of residuals for each satellite maybe changed
			if (m_pRngView)
			{
				m_pRngView->ClearData();
			}
			// update registry
			SaveOption();

			CWinApp* app=AfxGetApp();
	
			// save some options only relevant to real-time processing
			// receiver settings
			app->WriteProfileInt("Receiver","Receiver Type",m_iRcvType);
			app->WriteProfileInt("Receiver","Comm Port",m_iGPSPort);
			app->WriteProfileInt("Receiver","Baudrate",m_iGPSBaudrate);

			// socket settings
			app->WriteProfileInt("RTCA","IP 1",m_iRTCAIP[0]);
			app->WriteProfileInt("RTCA","IP 2",m_iRTCAIP[1]);
			app->WriteProfileInt("RTCA","IP 3",m_iRTCAIP[2]);
			app->WriteProfileInt("RTCA","IP 4",m_iRTCAIP[3]);
			app->WriteProfileInt("RTCA","IP Port",m_iRTCAPort);
		}
	}

#endif

}


/* -------------------------------------------------------------------------------
Name:			OnUpdateSpp

Arguments:		CCmdUI* pCmdUI - object contains information of a menu

History:		Name				Date		Reason for modification
                PMIS                2003        Original
------------------------------------------------------------------------------- */
void CMainFrame::OnUpdateSpp(CCmdUI* pCmdUI) 
{
}


/* -------------------------------------------------------------------------------
Name:			OnPostMission

Purpose:		To set the system mode flag to post-mission when post-mission is
				selected by the user.

History:		Name				Date		Reason for modification
                PMIS                2003        Original
------------------------------------------------------------------------------- */
void CMainFrame::OnPostMission() 
{
	// prevent from changing during processing
	if (m_bStart)
	{
		MessageBox("You cannot change the System Mode during processing!","System Mode",MB_ICONINFORMATION);
		return;
	}

	// post-mission
	m_options.system_mode=1;
	// update registry
	SaveOption();
}


/* -------------------------------------------------------------------------------
Name:			OnUpdatePostMission

Arguments:		CCmdUI* pCmdUI - object contains information of a menu

History:		Name				Date		Reason for modification
                PMIS                2003        Original
------------------------------------------------------------------------------- */
void CMainFrame::OnUpdatePostMission(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_options.system_mode==1);
}


/* -------------------------------------------------------------------------------
Name:			OnRealTime

Purpose:		To set the system mode flag to real-time when real-time is
				selected by the user.

History:		Name				Date		Reason for modification
                PMIS                2003        Original
------------------------------------------------------------------------------- */

void CMainFrame::OnRealTime() 
{
	// prevent from changing during processing
	if (m_bStart)
	{
		MessageBox("You cannot change the System Mode during processing!","System Mode",MB_ICONINFORMATION);
		return;
	}	

	// real-time
	m_options.system_mode=0;

	// update registry
	SaveOption();
}


/* -------------------------------------------------------------------------------
Name:			OnUpdateRealTime

Purpose:		To check system mode button.

Arguments:		CCmdUI* pCmdUI - object contains information of a menu

History:		Name				Date		Reason for modification
                PMIS                2003        Original
------------------------------------------------------------------------------- */

void CMainFrame::OnUpdateRealTime(CCmdUI* pCmdUI) 
{
#ifdef POST_MISSION
	pCmdUI->Enable(0);
	return;
#endif
	pCmdUI->SetCheck(m_options.system_mode==0);
}


/* -------------------------------------------------------------------------------
Name:			OnProcessing

Purpose:		To load processing menu. (SPP or PPP)

History:		Name				Date		Reason for modification
                PMIS                2003        Original
------------------------------------------------------------------------------- */
void CMainFrame::OnProcessing() 
{
	CRect rect;

	CMenu menu;
	if (!menu.LoadMenu(IDR_POPUP))
	{
		return;
	}

	int menuitem=5;

	// translate the current toolbar item rectangle into screen coordinates
	// so that we'll know where to pop up the menu
	m_wndToolBar.GetToolBarCtrl().GetRect(ID_PROCESSING, &rect);
	rect.top = rect.bottom;
	m_wndToolBar.ClientToScreen(&rect.TopLeft());
	CMenu* pPopup;
	CString str;

	for (int i=0;i<menuitem;i++)
	{
		menu.GetMenuString(i, str, MF_BYPOSITION);
		if (str.Find("Processing",0)!=-1)
			break;
	}
	if (i<menuitem)
	{
		pPopup = menu.GetSubMenu(i);
		ASSERT(pPopup != NULL);
		pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, rect.left, rect.top + 1, AfxGetMainWnd());
	}
}


/* -------------------------------------------------------------------------------
Name:			OnSystem

Purpose:		For system selection (real-time or post-mission processing).

History:		Name				Date		Reason for modification
                PMIS                2003        Original
------------------------------------------------------------------------------- */
void CMainFrame::OnSystem() 
{
	CRect rect;

	CMenu menu;
	if (!menu.LoadMenu(IDR_POPUP))
	{
		return;
	}

	int menuitem=5;

	// translate the current toolbar item rectangle into screen coordinates
	// so that we'll know where to pop up the menu
	m_wndToolBar.GetToolBarCtrl().GetRect(ID_SYSTEM, &rect);
	rect.top = rect.bottom;
	m_wndToolBar.ClientToScreen(&rect.TopLeft());
	CMenu* pPopup;
	CString str;

	for (int i=0;i<menuitem;i++)
	{
		menu.GetMenuString(i, str, MF_BYPOSITION);
		if (str.Find("System",0)!=-1)
			break;
	}
	if (i<menuitem)
	{
		pPopup = menu.GetSubMenu(i);
		ASSERT(pPopup != NULL);
		pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, rect.left, rect.top + 1, AfxGetMainWnd());
	}
}


/* -------------------------------------------------------------------------------
Name:			OnReport

Purpose:		Implementation of the REPORT dialog.

History:		Name				Date		Reason for modification
                PMIS                2003        Original
------------------------------------------------------------------------------- */
void CMainFrame::OnReport() 
{
	// prevent from changing during processing
	if (m_bStart)
	{
		MessageBox("You cannot generate a report file during processing!","Report",MB_ICONINFORMATION);
		return;
	}

	// opening LOG file, from which the information will be extracted and used
	// in the creation of the report output file
	ifstream is(m_options.logfile,ios::nocreate);

	// if the LOG file does not exist, the user can specify a different LOG file
	// this is especially useful when the user wants to disply/report data from a LOG
	// file that was created in the past
	if (!is.good())
	{
		// User wants to specify alternate LOG file
		if(MessageBox("The LOG file was not found!\nA report file cannot be created.\n\nWould you like to browse for a LOG file?","LOG File Does Not Exist",MB_ICONQUESTION | MB_YESNO) == IDYES)
		{
			CString path;
			path = m_options.logfile;
			if (path.IsEmpty())
			{
				char tch[MAX_PATH];
				GetCurrentDirectory(MAX_PATH,tch);
				path=tch;
			}
			else
			{
				int ind=path.ReverseFind('\\');
				if (ind>=0)
					path=path.Left(ind);
			}

			path=path+"\\";
			
			// use file dialog to choose output path
			CFileDialog m_filedlg(1,NULL, NULL ,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,NULL, NULL);
			
			m_filedlg.m_ofn.hwndOwner=NULL; 
			m_filedlg.m_ofn.lpstrInitialDir=path;
			m_filedlg.m_ofn.lpstrTitle="Log File Path and Name..."; 
			m_filedlg.m_ofn.lpstrFilter="Log File (*.log)\0*.log\0All Files (*.*)\0*.*\0\0"; 
			
			UpdateData(1);
			
			if (m_filedlg.DoModal()==IDOK)
			{
				strcpy(m_options.logfile,LPCTSTR(m_filedlg.GetPathName()));
				
				// adding ".log" extension if not provided by the user
				CString file = CString(m_options.logfile);
				CString ext = file.Right(4);
				if(ext != ".log")
					file += ".log";
				
				strcpy(m_options.logfile,LPCTSTR(file));
				
				if(_access(m_options.logfile, 00))
				{
					CString temp = m_options.logfile;
					MessageBox(temp + " could not be found!","LOG File Does Not Exist",MB_ICONEXCLAMATION);
					return;
				}	// if(_access(m_options.logfile, 00))

			}	// if (m_filedlg.DoModal()==IDOK)
			else
				return;
		}	// if(MessageBox)
		else
			return;
	}	// if (!is.good())

	// use the output dialog to create report file
	CDlgOutput dlgOutput;

	// pass output types to the dialog
	dlgOutput.outputTypes=m_options.outputTypes;
	for (int i=0;i<m_options.outputTypes;i++)
		strcpy(dlgOutput.outputType[i],m_options.outputType[i]);
	dlgOutput.m_strPath = m_options.reportfile;

	dlgOutput.m_bRPT	= m_options.rpt_over;
	dlgOutput.m_iDatum	= m_options.rpt_datum;

	if (dlgOutput.DoModal()==IDOK)
	{
		// adding ".txt" extension if not provided by the user
		CString file = CString(m_options.reportfile);
		CString ext = file.Right(4);
		if(ext != ".txt")
			file += ".txt";

		strcpy(m_options.reportfile,LPCTSTR(file));

		m_options.outputTypes=dlgOutput.outputTypes;
		for (int i=0;i<m_options.outputTypes;i++)
			strcpy(m_options.outputType[i],dlgOutput.outputType[i]);

		CWinApp* app=AfxGetApp();
		app->WriteProfileInt("Output","Types",m_options.outputTypes);

		char str[20];
		for (i=0;i<m_options.outputTypes;i++)
		{
			sprintf(str,"Type %d",i+1);
			app->WriteProfileString("Output",str,m_options.outputType[i]);
		}

		m_options.rpt_over = dlgOutput.m_bRPT;
		m_options.rpt_datum = dlgOutput.m_iDatum;

		// update registry
		SaveOption();
	}
}


/* -------------------------------------------------------------------------------
Name:			OnDisplay

Purpose:		to display the charts

History:		Name				Date		Reason for modification
                PMIS                2003        Original
------------------------------------------------------------------------------- */
void CMainFrame::OnDisplay() 
{
	// prevent from displaying charts during processing
	if (m_bStart)
	{
		MessageBox("You cannot activate the Display View during processing!","Display",MB_ICONINFORMATION);
		return;
	}

	// if the charts already exist, update them only
	if (m_pDrawView!=NULL)
	{
		m_pDrawView->UpdateDraw();
		m_pDrawView->GetParent()->ShowWindow(SW_SHOWMAXIMIZED);
		return;
	}

	// otherwise, create the window

	ifstream is(m_options.logfile,ios::nocreate);
	bool isdone=0;

	if (!is.good())
	{
		// User wants to specify alternate LOG file
		if(MessageBox("The LOG file was not found!\nNo data will be displayed.\n\nWould you like to browse for a LOG file?","LOG File Does Not Exist",MB_ICONQUESTION | MB_YESNO) == IDYES)
		{
			CString path;
			path = m_options.logfile;
			if (path.IsEmpty())
			{
				char tch[MAX_PATH];
				GetCurrentDirectory(MAX_PATH,tch);
				path=tch;
			}
			else
			{
				int ind=path.ReverseFind('\\');
				if (ind>=0)
					path=path.Left(ind);
			}

			path=path+"\\";
			
			CFileDialog m_filedlg(1,NULL, NULL ,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,NULL, NULL);
			
			m_filedlg.m_ofn.hwndOwner=NULL; 
			m_filedlg.m_ofn.lpstrInitialDir=path;
			m_filedlg.m_ofn.lpstrTitle="Log File Path and Name..."; 
			m_filedlg.m_ofn.lpstrFilter="Log File (*.log)\0*.log\0All Files (*.*)\0*.*\0\0"; 
			
			UpdateData(1);
			
			if (m_filedlg.DoModal()==IDOK)
			{
				strcpy(m_options.logfile,LPCTSTR(m_filedlg.GetPathName()));

				// adding ".log" extension if not provided by the user
				CString file = CString(m_options.logfile);
				CString ext = file.Right(4);
				if(ext != ".log")
					file += ".log";
				
				strcpy(m_options.logfile,LPCTSTR(file));
				
				if(_access(m_options.logfile, 00))
				{
					CString temp = m_options.logfile;
					MessageBox(temp + " could not be found!","LOG File Does Not Exist",MB_ICONEXCLAMATION);
					return;
				}	// if(_access(m_options.logfile, 00))
				
			}	// if (m_filedlg.DoModal()==IDOK)
			else
				return;
		}	// if(MessageBox(
		else
			return;
	}	// if (!is.good())

	CWinApp *pApp=AfxGetApp();
	POSITION curTemplatePos = pApp->GetFirstDocTemplatePosition();
		
	// to create the window
	while(curTemplatePos != NULL)
	{
		CDocTemplate* curTemplate = 
			pApp->GetNextDocTemplate(curTemplatePos);
		CString str;
		curTemplate->GetDocString(str, CDocTemplate::docName);
		if(str == _T("POS"))
		{
			if (curTemplate->OpenDocumentFile(m_options.logfile))
			{
				CChildFrame* m_pLineWnd=(CChildFrame*)GetActiveFrame();
				m_pDrawView=(CDrawView*)m_pLineWnd->GetActiveView();
				m_pLineWnd->GetActiveDocument()->SetTitle("Display View");
				m_pLineWnd->SetWindowText("Display View");
				return;
			}
		}
	}
}


/* -------------------------------------------------------------------------------
Name:			OnUpdateSystemStatus

Purpose:		To update the system option in the status bar, either real-time 
				or post-mission based on the user's selection.

Arguments:		CCmdUI* pCmdUI - object contains information of a menu

History:		Name				Date		Reason for modification
                PMIS                2003        Original
------------------------------------------------------------------------------- */
void CMainFrame::OnUpdateSystemStatus(CCmdUI* pCmdUI) 
{
	if (m_options.system_mode==0)
		m_wndStatusBar.SetPaneText(1, "Real Time");
	else
		m_wndStatusBar.SetPaneText(1, "Post Mission");
}


/* -------------------------------------------------------------------------------
Name:			OnUpdateStationStatus

Purpose:		To update the station option in the status bar, either static
				or kinematic based on the user's selection.

Arguments:		CCmdUI* pCmdUI - object contains information of a menu

History:		Name				Date		Reason for modification
                PMIS                2003        Original
------------------------------------------------------------------------------- */
void CMainFrame::OnUpdateStationStatus(CCmdUI* pCmdUI) 
{
	if (m_options.station_mode==0)
		m_wndStatusBar.SetPaneText(2, "Static");
	else
		m_wndStatusBar.SetPaneText(2, "Kinematic");
}


/* -------------------------------------------------------------------------------
Name:			OnUpdateProcessingStatus

Purpose:		To update the processing option in the status bar, either SPP or PPP
				based on the user's selection.

Arguments:		CCmdUI* pCmdUI - object contains information of a menu

History:		Name				Date		Reason for modification
                PMIS                2003        Original
------------------------------------------------------------------------------- */
void CMainFrame::OnUpdateProcessingStatus(CCmdUI* pCmdUI) 
{
	if (m_options.processing_mode==0)
		m_wndStatusBar.SetPaneText(3, "SPP");
	else
		m_wndStatusBar.SetPaneText(3, "PPP");
}


/* -------------------------------------------------------------------------------
Name:			OnUpdateOrbitStatus

Purpose:		To update the orbit option in the status bar.  Possible options for
				the orbit are broadcast, SP3, broadcast+JPL and broadcast+GPS*C.

Arguments:		CCmdUI* pCmdUI - object contains information of a menu

History:		Name				Date		Reason for modification
                PMIS                2003        Original
------------------------------------------------------------------------------- */
void CMainFrame::OnUpdateOrbitStatus(CCmdUI* pCmdUI) 
{
	switch (m_options.eph_flag)
	{
	case 0:
		m_wndStatusBar.SetPaneText(4, "Orbit: Broadcast");
		break;
	case 1:
		m_wndStatusBar.SetPaneText(4, "Orbit: SP3");
		break;
	case 2:
		m_wndStatusBar.SetPaneText(4, "Orbit: Broadcast+GPS*C");
		break;
	case 3:
		m_wndStatusBar.SetPaneText(4, "Orbit: Broadcast+JPL");
		break;
	}
}


/* -------------------------------------------------------------------------------
Name:			OnUpdateClockStatus

Purpose:		To update the clock option in the status bar.  Possible options for
				the clock are broadcast, SP3, 5 minutes, broadcast+JPL and
				broadcast+GPS*C.

Arguments:		CCmdUI* pCmdUI - object contains information of a menu

History:		Name				Date		Reason for modification
                PMIS                2003        Original
------------------------------------------------------------------------------- */
void CMainFrame::OnUpdateClockStatus(CCmdUI* pCmdUI) 
{
	switch (m_options.clock_flag)
	{
	case 0:
		m_wndStatusBar.SetPaneText(5, "Clock: Broadcast");
		break;
	case 1:
		m_wndStatusBar.SetPaneText(5, "Clock: 5 Minutes");
		break;
	case 2:
		m_wndStatusBar.SetPaneText(5, "Clock: SP3");
		break;
	case 3:
		m_wndStatusBar.SetPaneText(5, "Clock: Broadcast+GPS*C");
		break;
	case 4:
		m_wndStatusBar.SetPaneText(5, "Clock: Broadcast+JPL");
		break;
	}
}


/* -------------------------------------------------------------------------------
Name:			OnUpdateCurrentEpoch

Arguments:		CCmdUI* pCmdUI - object contains information of a menu

History:		Name				Date		Reason for modification
                PMIS                2003        Original
------------------------------------------------------------------------------- */
void CMainFrame::OnUpdateCurrentEpoch(CCmdUI* pCmdUI) 
{
}


/* -------------------------------------------------------------------------------
Name:			OnUpdateLaterUse

Arguments:		CCmdUI* pCmdUI - object contains information of a menu

History:		Name				Date		Reason for modification
                PMIS                2003        Original
------------------------------------------------------------------------------- */
void CMainFrame::OnUpdateLaterUse(CCmdUI* pCmdUI) 
{
}


/* -------------------------------------------------------------------------------
Name:			OnViewAmbiguity

Purpose:		To display ambiguity graph.

History:		Name				Date		Reason for modification
                PMIS                2003        Original
------------------------------------------------------------------------------- */
void CMainFrame::OnViewAmbiguity() 
{
	// update the number of charts displaying
	// at most 4 charts can be shown in the display window at the same time
	if (!m_bViewAmb)
	{
		if (m_iDisplay>3)
			return;
		else
			m_iDisplay++;
	}
	else
		m_iDisplay--;

	// toggle the switch 
	m_bViewAmb=!m_bViewAmb;

	// after number of charts to be shown has been changed, update the window
	if (m_pDrawView!=NULL)
		m_pDrawView->Invalidate(0);
}


/* -------------------------------------------------------------------------------
Name:			OnViewClock

Purpose:		To view receiver clock error graph.

History:		Name				Date		Reason for modification
                PMIS                2003        Original
------------------------------------------------------------------------------- */
void CMainFrame::OnViewClock() 
{
	// update the number of charts displaying
	// at most 4 charts can be shown in the display window at the same time
	if (!m_bViewClock)
	{
		if (m_iDisplay>3)
			return;
		else
			m_iDisplay++;
	}
	else
		m_iDisplay--;

	// toggle the switch 
	m_bViewClock=!m_bViewClock;

	// after number of charts to be shown has been changed, update the window
	if (m_pDrawView!=NULL)
		m_pDrawView->Invalidate(0);
}


/* -------------------------------------------------------------------------------
Name:			OnViewError

Purpose:		To view position error graph.

History:		Name				Date		Reason for modification
                PMIS                2003        Original
------------------------------------------------------------------------------- */
void CMainFrame::OnViewError() 
{
	// update the number of charts displaying
	// at most 4 charts can be shown in the display window at the same time
	if (!m_bViewError)
	{
		if (m_iDisplay>3)
			return;
		else
			m_iDisplay++;
	}
	else
		m_iDisplay--;

	// toggle the switch 
	m_bViewError=!m_bViewError;
	if (m_bViewError)
		m_bViewTrajectory=0;

	// after number of charts to be shown has been changed, update the window
	if (m_pDrawView!=NULL)
		m_pDrawView->Invalidate(0);
}


/* -------------------------------------------------------------------------------
Name:			OnViewSats

Purpose:		To view the number of satellites graph.

History:		Name				Date		Reason for modification
                PMIS                2003        Original
------------------------------------------------------------------------------- */
void CMainFrame::OnViewSats() 
{
	// update the number of charts displaying
	// at most 4 charts can be shown in the display window at the same time
	if (!m_bViewSats)
	{
		if (m_iDisplay>3 && (!(m_bViewPDOP || m_bViewGDOP || m_bViewVDOP || m_bViewTDOP || m_bViewHDOP)))
			return;
		else if (!(m_bViewPDOP || m_bViewHDOP || m_bViewVDOP || m_bViewTDOP || m_bViewHDOP))
			m_iDisplay++;
	}
	else
		m_iDisplay--;

	// toggle the switch 
	m_bViewSats=!m_bViewSats;

	// after number of charts to be shown has been changed, update the window
	if (m_pDrawView!=NULL)
		m_pDrawView->Invalidate(0);
}


/* -------------------------------------------------------------------------------
Name:			OnViewTrajectory

Purpose:		To view the trajectory graph.

History:		Name				Date		Reason for modification
                PMIS                2003        Original
------------------------------------------------------------------------------- */
void CMainFrame::OnViewTrajectory() 
{
	// update the number of charts displaying
	// at most 4 charts can be shown in the display window at the same time
	if (!m_bViewTrajectory)
	{
		if (m_iDisplay>3)
			return;
		else
			m_iDisplay++;
	}
	else
		m_iDisplay--;

	// toggle the switch 
	m_bViewTrajectory=!m_bViewTrajectory;

	// display trajectory or positioning errors
	if (m_bViewTrajectory)
		m_bViewError=0;

	// after number of charts to be shown has been changed, update the window
	if (m_pDrawView!=NULL)
		m_pDrawView->Invalidate(0);
}


/* -------------------------------------------------------------------------------
Name:			OnViewVel

Purpose:		To view the velocity graph.

History:		Name				Date		Reason for modification
                PMIS                2003        Original
------------------------------------------------------------------------------- */
void CMainFrame::OnViewVel() 
{
	// update the number of charts displaying
	// at most 4 charts can be shown in the display window at the same time
	if (!m_bViewVel)
	{
		if (m_iDisplay>3)
			return;
		else
			m_iDisplay++;
	}
	else
		m_iDisplay--;

	// toggle the switch 
	m_bViewVel=!m_bViewVel;

	// after number of charts to be shown has been changed, update the window
	if (m_pDrawView!=NULL)
		m_pDrawView->Invalidate(0);
}


/* -------------------------------------------------------------------------------
Name:			OnViewZtd

Purpose:		Show/hide zenith tropospheric delay graph.

History:		Name				Date		Reason for modification
                PMIS                2003        Original
------------------------------------------------------------------------------- */
void CMainFrame::OnViewZtd() 
{
	// update the number of charts displaying
	// at most 4 charts can be shown in the display window at the same time
	if (!m_bViewZTD)
	{
		if (m_iDisplay>3)
			return;
		else
			m_iDisplay++;
	}
	else
		m_iDisplay--;
	
	// toggle the switch 
	m_bViewZTD=!m_bViewZTD;

	// after number of charts to be shown has been changed, update the window
	if (m_pDrawView!=NULL)
		m_pDrawView->Invalidate(0);
}


/* -------------------------------------------------------------------------------
Name:			OnUpdateViewAmbiguity

Purpose:		To set the check next to the ambiguity graph based on whether the
				ambiguity graph is currently shown in the DISPLAY view.

Arguments:		CCmdUI* pCmdUI - object contains information of a menu

History:		Name				Date		Reason for modification
                PMIS                2003        Original
------------------------------------------------------------------------------- */
void CMainFrame::OnUpdateViewAmbiguity(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bViewAmb);
	pCmdUI->Enable(m_bEnableAmb);
}


/* -------------------------------------------------------------------------------
Name:			OnUpdateViewClock

Purpose:		To set the check next to the clock graph based on whether the
				clock graph is currently shown in the DISPLAY view.

Arguments:		CCmdUI* pCmdUI - object contains information of a menu

History:		Name				Date		Reason for modification
                PMIS                2003        Original
------------------------------------------------------------------------------- */
void CMainFrame::OnUpdateViewClock(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bViewClock);
}


/* -------------------------------------------------------------------------------
Name:			OnUpdateViewError

Purpose:		To determine if to show trajectory or position error.

Arguments:		CCmdUI* pCmdUI - object contains information of a menu

History:		Name				Date		Reason for modification
                PMIS                2003        Original
------------------------------------------------------------------------------- */
void CMainFrame::OnUpdateViewError(CCmdUI* pCmdUI) 
{
	// open the log file
	ifstream is(m_options.logfile,ios::nocreate);

	bool isMode=0;	// if we got the mode from the file or not
	bool isVel=0;	// if we got the velocity estimation from the file or not
	bool isAmb=0;	// if we got the ambiguity estimation from the file or not

	// if the file exists
	if (is.good())
	{
		char str[128];
		is.getline(str,128);
		// loop till find the information or reach the end of the file
		while (!is.eof())
		{
			if (strstr(str,"END OF HEADER"))
				break;
			if (strstr(str,"Station Mode"))
			{
				if (strstr(str,"Static"))
				{
					// display positioning error instead trajectory in static mode
					if (m_bViewTrajectory==1)
					{
						m_bViewError=1;
						m_bViewTrajectory=0;
					}
					m_bEnableTraj=0;
					m_bEnableError=1;
				}
				else
				{
					// display trajectory instead positioning error in kinematic mode
					if (m_bViewError==1)
					{
						m_bViewError=0;
						m_bViewTrajectory=1;
					}

					m_bEnableTraj=1;
					m_bEnableError=0;
				}
				// we got the mode from the file
				isMode=1;
			}

			if (strstr(str,"Estimate Velocity"))
			{
				// if velocity has not been estimated, disable the menu
				if (strstr(str,"No"))
				{
					m_bEnableVel=0;
					m_bViewVel=0;
				}
				else
				{
					// otherwise, enable the menu
					m_bEnableVel=1;
				}
				// we got the velocity estimation from the file
				isVel=1;
			}

			if (strstr(str,"Estimate Ambiguity"))
			{
				// if ambiguity has not been estimated, disable the menu
				if (strstr(str,"No"))
				{
					m_bViewAmb=0;
					m_bEnableAmb=0;
				}
				else
				{
					// otherwise, enable the menu
					m_bEnableAmb=1;
				}
				// we got the ambiguity estimation from the file
				isAmb=1;
			}
			is.getline(str,128);
		}
	}

	// if we can not determine the mode from the file, use the settings in the options
	if (!isMode)
	{
		if (m_options.station_mode==1)
		{
			m_bEnableTraj=1;
			m_bEnableError=0;
			m_bViewError=0;
		}
		else
		{
			m_bViewTrajectory=0;
			m_bEnableTraj=0;
			m_bEnableError=1;
		}
	}
	// if we can not determine the velocity estimation from the file, use the settings in the options
	if (!isVel)
	{
		if (m_options.vel_estimate==1)
		{
			m_bEnableVel=1;
		}
		else
		{
			m_bEnableVel=0;
			m_bViewVel=0;
		}
	}
	// if we can not determine the ambiguity estimation from the file, use the settings in the options
	if (!isAmb)
	{
		if (m_options.ambiguity_flag==1)
		{
			m_bEnableAmb=1;
		}
		else
		{
			m_bViewAmb=0;
			m_bEnableAmb=0;
		}
	}

	// to determine how many charts we have chosen to display
	m_iDisplay=0;
	if (m_bViewTrajectory)
		m_iDisplay++;
	if (m_bViewError)
		m_iDisplay++;
	if (m_bViewClock)
		m_iDisplay++;
	if (m_bViewZTD)
		m_iDisplay++;
	if (m_bViewSats || m_bViewPDOP || m_bViewHDOP || m_bViewVDOP || m_bViewTDOP || m_bViewGDOP)
		m_iDisplay++;
	if (m_bViewAmb)
		m_iDisplay++;
	if (m_bViewVel)
		m_iDisplay++;

	// at most 4 charts can be displayed in the window at the same time
	if (m_iDisplay>4)
	{
		// if more than 4 has been selected, remove the velocity chart
		if (m_bViewVel)
		{
			m_bViewVel=0;
			m_iDisplay--;
		}
	}

	if (m_iDisplay>4)
	{
		// if still more than 4 has been selected, remove the ambiguity chart
		m_bViewAmb=0;
		m_iDisplay--;
	}

	// if we select to display positioning errors, check the menu
	pCmdUI->SetCheck(m_bViewError);
	// if in static mode, enable the positioning error menu
	pCmdUI->Enable(m_bEnableError);

	CMenu* menu=GetMenu();
	if (!menu)
	{
		return;
	}

	int menuitem=5;

	// translate the current toolbar item rectangle into screen coordinates
	// so that we'll know where to pop up the menu
	CMenu* pPopup;
	CString str;
	for (int i=0;i<menuitem;i++)
	{
		menu->GetMenuString(i, str, MF_BYPOSITION);
		if (str.Find("Display",0)!=-1)
			break;
	}
	if (i<menuitem)
	{
		pPopup = menu->GetSubMenu(i);
		ASSERT(pPopup != NULL);
		menuitem=8;
		for (i=0;i<menuitem;i++)
		{
			pPopup->GetMenuString(i, str, MF_BYPOSITION);
			if (str.Find("DOP",0)!=-1)
				break;
		}

		if (i<menuitem)
		{
			if (m_bViewPDOP || m_bViewHDOP || m_bViewVDOP || m_bViewTDOP || m_bViewGDOP)
				pPopup->CheckMenuItem(i,MF_CHECKED|MF_BYPOSITION);
			else
				pPopup->CheckMenuItem(i,MF_UNCHECKED|MF_BYPOSITION);
		}
	}
}


/* -------------------------------------------------------------------------------
Name:			OnUpdateViewSats

Purpose:		To set a check next to the number of satellites graph when this
				graph is currently being displayed.

Arguments:		CCmdUI* pCmdUI - object contains information of a menu

History:		Name				Date		Reason for modification
                PMIS                2003        Original
------------------------------------------------------------------------------- */
void CMainFrame::OnUpdateViewSats(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bViewSats);
}


/* -------------------------------------------------------------------------------
Name:			OnUpdateViewTrajectory

Purpose:		To set a check next to the trajectory graph when this
				graph is currently being displayed.

Arguments:		CCmdUI* pCmdUI - object contains information of a menu

History:		Name				Date		Reason for modification
                PMIS                2003        Original
------------------------------------------------------------------------------- */
void CMainFrame::OnUpdateViewTrajectory(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bViewTrajectory);
	pCmdUI->Enable(m_bEnableTraj);
}


/* -------------------------------------------------------------------------------
Name:			OnUpdateViewVel

Purpose:		To set a check next to the velocity graph when this
				graph is currently being displayed.

Arguments:		CCmdUI* pCmdUI - object contains information of a menu

History:		Name				Date		Reason for modification
                PMIS                2003        Original
------------------------------------------------------------------------------- */
void CMainFrame::OnUpdateViewVel(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bViewVel);
	pCmdUI->Enable(m_bEnableVel);
}


/* -------------------------------------------------------------------------------
Name:			OnUpdateViewZtd

Purpose:		To set a check next to the zenith troposphere graph when this
				graph is currently being displayed.

Arguments:		CCmdUI* pCmdUI - object contains information of a menu

History:		Name				Date		Reason for modification
                PMIS                2003        Original
------------------------------------------------------------------------------- */
void CMainFrame::OnUpdateViewZtd(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bViewZTD);
}


/* -------------------------------------------------------------------------------
Name:			OnSplitter5

Purpose:		To split the windows into several windows.

History:		Name				Date		Reason for modification
                PMIS                2003        Original
------------------------------------------------------------------------------- */
void CMainFrame::OnSplitter5() 
{
	// if the 5-way splitted window already exists, just maximize and show it
	if (m_p5Splitter!=NULL)
	{
		m_p5Splitter->ShowWindow(SW_SHOWMAXIMIZED);
		return;
	}

	// otherwise, create the 5-way splitted window and show it

	CWinApp *pApp=AfxGetApp();
	BOOL found=0;
	POSITION curTemplatePos = pApp->GetFirstDocTemplatePosition();
	CDocTemplate* curTemplate = NULL;
	// find the templete for the 5-way splitted window
	while(curTemplatePos != NULL)
	{
		curTemplate = 
			pApp->GetNextDocTemplate(curTemplatePos);
		CString str;
		curTemplate->GetDocString(str, CDocTemplate::docName);
		if(str == _T("SPLITTER5"))
		{
			found=1;
			break;
		}
	}

	if (!found || curTemplate == NULL)
		return;

	if (curTemplate->OpenDocumentFile(NULL))
	{
		m_p5Splitter=GetActiveFrame();
		m_p5Splitter->GetActiveDocument()->SetTitle("Picture View");
		m_p5Splitter->SetWindowText("Picture View");
		m_p5Splitter->ShowWindow(SW_SHOWMAXIMIZED);
	}

	// save a pointer to the sub-windows for displaying
	if (m_p5Splitter !=NULL && m_p5Splitter->IsKindOf( RUNTIME_CLASS( C5WaySplitterFrame )))
	{
		m_pResView=(CResultView*)((C5WaySplitterFrame*)m_p5Splitter)->m_pResView;
		m_pRngView=(CRangeView*)((C5WaySplitterFrame*)m_p5Splitter)->m_pRngView;
		m_pPicView=(CSatPicView*)((C5WaySplitterFrame*)m_p5Splitter)->m_pPicView;
		m_pAzmView=(CAzmElvView*)((C5WaySplitterFrame*)m_p5Splitter)->m_pSatView;
		m_pTrajView=(CTrajView*)((C5WaySplitterFrame*)m_p5Splitter)->m_pTrajView;
		m_pMsgView=(CMsgView*)((C5WaySplitterFrame*)m_p5Splitter)->m_pMsgView;
	}	
}


/* -------------------------------------------------------------------------------
Name:			OnUpdateSplitter5

Purpose:		Update splitter.

Arguments:		CCmdUI* pCmdUI - object contains information of a menu

History:		Name				Date		Reason for modification
                PMIS                2003        Original
------------------------------------------------------------------------------- */
void CMainFrame::OnUpdateSplitter5(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_p5Splitter ==NULL);
}


/* -------------------------------------------------------------------------------
Name:			OnViewGdop

Purpose:		To display the GDOP graph.

History:		Name				Date		Reason for modification
                PMIS                2003        Original
------------------------------------------------------------------------------- */
void CMainFrame::OnViewGdop() 
{
	// update the number of charts displaying
	// at most 4 charts can be shown in the display window at the same time
	if (!m_bViewGDOP)
	{
		if (m_iDisplay>3 && (!(m_bViewPDOP || m_bViewHDOP || m_bViewVDOP || m_bViewTDOP || m_bViewSats)))
			return;
		else if (!(m_bViewPDOP || m_bViewHDOP || m_bViewVDOP || m_bViewTDOP || m_bViewSats))
			m_iDisplay++;
	}
	else
		m_iDisplay--;
	
	// toggle the switch 
	m_bViewGDOP=!m_bViewGDOP;

	// after number of charts to be shown has been changed, update the window
	if (m_pDrawView!=NULL)
		m_pDrawView->Invalidate(0);	
}


/* -------------------------------------------------------------------------------
Name:			OnUpdateViewGdop

Purpose:		To set a check next to GDOP when the GDOP is currently being
				displayed.

Arguments:		CCmdUI* pCmdUI - object contains information of a menu

History:		Name				Date		Reason for modification
                PMIS                2003        Original
------------------------------------------------------------------------------- */
void CMainFrame::OnUpdateViewGdop(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bViewGDOP);
}


/* -------------------------------------------------------------------------------
Name:			OnViewHdop

Purpose:		To display the HDOP graph.

History:		Name				Date		Reason for modification
                PMIS                2003        Original
------------------------------------------------------------------------------- */
void CMainFrame::OnViewHdop() 
{
	// update the number of charts displaying
	// at most 4 charts can be shown in the display window at the same time
	if (!m_bViewHDOP)
	{
		if (m_iDisplay>3 && (!(m_bViewPDOP || m_bViewGDOP || m_bViewVDOP || m_bViewTDOP || m_bViewSats)))
			return;
		else if (!(m_bViewPDOP || m_bViewHDOP || m_bViewVDOP || m_bViewTDOP || m_bViewSats))
			m_iDisplay++;
	}
	else
		m_iDisplay--;

	// toggle the switch 
	m_bViewHDOP=!m_bViewHDOP;

	// after number of charts to be shown has been changed, update the window
	if (m_pDrawView!=NULL)
		m_pDrawView->Invalidate(0);
	
}


/* -------------------------------------------------------------------------------
Name:			OnUpdateViewHdop

Purpose:		To set a check next to HDOP when the HDOP is currently being
				displayed.

Arguments:		CCmdUI* pCmdUI - object contains information of a menu

History:		Name				Date		Reason for modification
                PMIS                2003        Original
------------------------------------------------------------------------------- */
void CMainFrame::OnUpdateViewHdop(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bViewHDOP);	
}


/* -------------------------------------------------------------------------------
Name:			OnViewPdop

Purpose:		To display the PDOP graph.

History:		Name				Date		Reason for modification
                PMIS                2003        Original
------------------------------------------------------------------------------- */
void CMainFrame::OnViewPdop() 
{
	// update the number of charts displaying
	// at most 4 charts can be shown in the display window at the same time
	if (!m_bViewPDOP)
	{
		if (m_iDisplay>3 && (!(m_bViewHDOP || m_bViewGDOP || m_bViewVDOP || m_bViewTDOP || m_bViewSats)))
			return;
		else if (!(m_bViewPDOP || m_bViewHDOP || m_bViewVDOP || m_bViewTDOP || m_bViewSats))
			m_iDisplay++;
	}
	else
		m_iDisplay--;

	// toggle the switch 
	m_bViewPDOP=!m_bViewPDOP;

	// after number of charts to be shown has been changed, update the window
	if (m_pDrawView!=NULL)
		m_pDrawView->Invalidate(0);	
}


/* -------------------------------------------------------------------------------
Name:			OnUpdateViewPdop

Purpose:		To set a check next to PDOP when the PDOP is currently being
				displayed.

Arguments:		CCmdUI* pCmdUI - object contains information of a menu

History:		Name				Date		Reason for modification
                PMIS                2003        Original
------------------------------------------------------------------------------- */
void CMainFrame::OnUpdateViewPdop(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bViewPDOP);	
}


/* -------------------------------------------------------------------------------
Name:			OnViewTdop

Purpose:		To display the TDOP graph.

History:		Name				Date		Reason for modification
                PMIS                2003        Original
------------------------------------------------------------------------------- */
void CMainFrame::OnViewTdop() 
{
	// update the number of charts displaying
	// at most 4 charts can be shown in the display window at the same time
	if (!m_bViewTDOP)
	{
		if (m_iDisplay>3 && (!(m_bViewPDOP || m_bViewGDOP || m_bViewVDOP || m_bViewHDOP || m_bViewSats)))
			return;
		else if (!(m_bViewPDOP || m_bViewHDOP || m_bViewVDOP || m_bViewTDOP || m_bViewSats))
			m_iDisplay++;
	}
	else
		m_iDisplay--;

	// toggle the switch 
	m_bViewTDOP=!m_bViewTDOP;

	// after number of charts to be shown has been changed, update the window
	if (m_pDrawView!=NULL)
		m_pDrawView->Invalidate(0);	
}


/* -------------------------------------------------------------------------------
Name:			OnUpdateViewTdop

Purpose:		To set a check next to TDOP when the TDOP is currently being
				displayed.

Arguments:		CCmdUI* pCmdUI - object contains information of a menu

History:		Name				Date		Reason for modification
                PMIS                2003        Original
------------------------------------------------------------------------------- */
void CMainFrame::OnUpdateViewTdop(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bViewTDOP);	
}


/* -------------------------------------------------------------------------------
Name:			OnViewVdop

Purpose:		To display the VDOP graph.

History:		Name				Date		Reason for modification
                PMIS                2003        Original
------------------------------------------------------------------------------- */
void CMainFrame::OnViewVdop() 
{
	// update the number of charts displaying
	// at most 4 charts can be shown in the display window at the same time
	if (!m_bViewVDOP)
	{
		if (m_iDisplay>3 && (!(m_bViewPDOP || m_bViewGDOP || m_bViewHDOP || m_bViewTDOP || m_bViewSats)))
			return;
		else if (!(m_bViewPDOP || m_bViewHDOP || m_bViewVDOP || m_bViewTDOP || m_bViewSats))
			m_iDisplay++;
	}
	else
		m_iDisplay--;

	// toggle the switch 
	m_bViewVDOP=!m_bViewVDOP;

	// after number of charts to be shown has been changed, update the window
	if (m_pDrawView!=NULL)
		m_pDrawView->Invalidate(0);	
}


/* -------------------------------------------------------------------------------
Name:			OnUpdateViewVdop

Purpose:		To set a check next to VDOP when the VDOP is currently being
				displayed.

Arguments:		CCmdUI* pCmdUI - object contains information of a menu

History:		Name				Date		Reason for modification
                PMIS                2003        Original
------------------------------------------------------------------------------- */
void CMainFrame::OnUpdateViewVdop(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bViewVDOP);	
}


/* -------------------------------------------------------------------------------
Name:			OnPppHelp

Purpose:		To display the manual associated with the PPP software.  If the
				file is not found, a dialog will appear prompting the user to
				locate the file.

History:		Name				Date		Reason for modification
                PMIS                2003        Original
------------------------------------------------------------------------------- */
void CMainFrame::OnPppHelp() 
{
	CString tstr=m_strAppPath+"\\manual.doc";
	CString str1, str2;

	CString office1 = CString("C:\\Program Files\\Microsoft Office\\Office\\winword.exe ");
	CString office2 = CString("C:\\Program Files\\Microsoft Office\\Office10\\winword.exe ");

	CFileFind ff;

	if (!ff.FindFile(tstr))
	{
		ff.Close();

		// browse for manual
		CFileDialog m_filedlg(1,NULL, NULL ,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,NULL, NULL);
		
		m_filedlg.m_ofn.hwndOwner=NULL; 
		m_filedlg.m_ofn.lpstrInitialDir=m_strAppPath;
		m_filedlg.m_ofn.lpstrTitle="Select PPP Manual Path and Name..."; 
		m_filedlg.m_ofn.lpstrFilter="Word Document (*.doc)\0*.doc\0All Files (*.*)\0*.*\0\0";

		UpdateData(1);
			
		// user selects a different filename and clicks 'OK'
		if (m_filedlg.DoModal()==IDOK)
			tstr = m_filedlg.GetPathName();
		else
			return;
	}

	str1=office1+tstr;
	str2=office2+tstr;

	// use winword to open the manual
	if(WinExec(str1,SW_SHOWMAXIMIZED) > 31)
		return;
	else if(WinExec(str2,SW_SHOWMAXIMIZED) > 31)
		return;
	else
		MessageBox("Error opening PPP manual!","PPP Manual Error",MB_ICONINFORMATION);
}


/* -------------------------------------------------------------------------------
Name:			CheckForLOGOverwrite

Purpose:		To check if the LOG file already exists.  If it exists, the user
				will be asked if they want to overwrite this file, unless the
				user selected the option for overwriting the LOG file in the
				CONSTANTS dialog (part of Setup).

				When the user is asked to overwrite the file, he/she can respond "yes"
				and the file will be overwritten, or he/she can respond "no" at which
				time a dialog will appear for the user to specify an alternate filename.

History:		Name				Date		Reason for modification
                PMIS                2003        Original
------------------------------------------------------------------------------- */
bool CMainFrame::CheckForLOGOverwrite()
{
	// checking if a file path and name has been specified
	if (m_options.logfile[0] == ' ' || m_options.logfile[0] == NULL)
		MessageBox("A LOG file has not been specified!\nYou will not be able to display graphs or generate a report file.","LOG File Does Not Exist",MB_ICONEXCLAMATION);

	// checking to see if the file already exists
	if(!_access(m_options.logfile, 00))
	{
		// user chooses NOT to overwrite the existing file
		if(MessageBox("The specified LOG file already exists!\n\nOverwrite?",
			"LOG File Overwrite",MB_ICONQUESTION | MB_YESNO) == IDNO)
		{
			CString path;
			path = m_options.logfile;
			if (path.IsEmpty())
			{
				char tch[MAX_PATH];
				GetCurrentDirectory(MAX_PATH,tch);
				path=tch;
			}
			else
			{
				int ind=path.ReverseFind('\\');
				if (ind>=0)
					path=path.Left(ind);
			}
			
			path=path+"\\";
			
			CFileDialog m_filedlg(1,NULL, NULL ,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,NULL, NULL);
			
			m_filedlg.m_ofn.hwndOwner=NULL; 
			m_filedlg.m_ofn.lpstrInitialDir=path;
			m_filedlg.m_ofn.lpstrTitle="Log File Path and Name..."; 
			m_filedlg.m_ofn.lpstrFilter="Log File (*.log)\0*.log\0All Files (*.*)\0*.*\0\0"; 
			
			UpdateData(1);
			
			// user selects a different filename and clicks 'OK'
			if (m_filedlg.DoModal()==IDOK)
			{
				strcpy(m_options.logfile,LPCTSTR(m_filedlg.GetPathName()));

				// adding ".log" if not provided by the user
				CString file = CString(m_options.logfile);
				CString ext = file.Right(4);
				if(ext != ".log")
					file += ".log";

				strcpy(m_options.logfile,LPCTSTR(file));
				
				UpdateData(0);
				SaveOption();
				return TRUE;
			}

			// user clicks on 'CANCEL' in the dialog for selecting a different filename
			else
				return FALSE;
		}

		// user chooses to overwrite the existing file
		else
			return TRUE;
	}

	else
		return TRUE;
}


/* -------------------------------------------------------------------------------
Name:			GetPCV

Purpose:		To get the antenna phase center offsets and antenna phase center
				variations for a specified antenna vendor and type from the IGS
				PCV file.  A message is displayed if the values for the specified
				antenna vendor and type were not read properly.

History:		Name				Date		Reason for modification
                PMIS                2003        Original
------------------------------------------------------------------------------- */
bool CMainFrame::GetPCV()
{
	// Check to see if a valid PCV file has been specified.

	// At least one antenna entry must be present for the PCV file to be 
	// considered valid:
	// 
	// TURBOROGUE     DORNE MARGOLIN T          OLD NAME              (  0) 96/06/30
    //        0.0       0.0     110.0
    //    0.0   0.0   0.0   0.0   0.0   0.0   0.0   0.0   0.0   0.0
    //    0.0   0.0   0.0   0.0   0.0   0.0   0.0   0.0   0.0
    //        0.0       0.0     128.0
    //    0.0   0.0   0.0   0.0   0.0   0.0   0.0   0.0   0.0   0.0
    //    0.0   0.0   0.0   0.0   0.0   0.0   0.0   0.0   0.0

	int i, j;

	ifstream is;
	is.open(m_options.pcvfile, ios::nocreate);

	// the PCV file could not be opened
	if(is.fail())
	{
		MessageBox("The specified PCV File could not be opened!","File Input Error",MB_ICONSTOP);
		return FALSE;
	}

	char str[200];

	// initializing all L1 and L2 APC offsets and PCV values to -99999
	for(i = 0; i < 3; i++)
	{
		L1apc[i] = -99999;
		L2apc[i] = -99999;
	}

	for(i = 0; i < 19; i++)
	{
		L1pcv[i] = -99999;
		L2pcv[i] = -99999;
	}

	int count = 0;

	while(!is.eof())
	{
		is.getline(str,200);

		if (strstr(str, m_options.ant_type) != NULL)
		{
			// inputting L1 apc, L1 pcv, L2 apc and L2 pcv for specified antenna vendor
			// and type
			for(i = 0; i < 3; i++)
				is >> L1apc[i];
			for(i = 18; i >= 0; i--)
				is >> L1pcv[i];
			for(i = 0; i < 3; i++)
				is >> L2apc[i];
			for(i = 18; i >= 0; i--)
				is >> L2pcv[i];

			// checking to see if all values have been properly initialized
			for(i = 0; i < 3; i++)
			{
				if(L1apc[i] != -99999 && L2apc[i] != -99999)
					count += 2;
			}

			for(i = 0; i < 19; i++)
			{
				if(L1pcv[i] != -99999 && L2pcv[i] != -99999)
					count += 2;
			}

			break;
		}
		else
			continue;
	}

	// there should be a total of 44 values for each antenna... if this is not the case,
	// an error message is displayed and processing does not take place
	if(count != 44)
	{
		// PCV file does not contain any data
		if(m_options.ant_type == "")
			MessageBox("The PCV File does not contain valid data!","File Input Error",MB_ICONSTOP);

		// the values for the specified antenna vendor and type are incorrect
		else
		{			
			// separating the vendor and type
			CString v(m_options.ant_type.Left(15));
			CString t(m_options.ant_type.Mid(15, 22));
			
			LPTSTR lpv = new TCHAR[v.GetLength()+1];
			LPTSTR lpt = new TCHAR[t.GetLength()+1];
			_tcscpy(lpv, v);
			_tcscpy(lpt, t);
			
			for(i = 0; i < v.GetLength()+1 && lpv[i] != char(32); i++);
			char *vendor = new char[i];
			for(j = 0; j < i; j++)
				vendor[j] = lpv[j];
			vendor[j] = NULL;
			
			for(i = 0; i < t.GetLength()+1 && lpt[i] != char(32); i++);
			char *type = new char[i];
			for(j = 0; j < i; j++)
				type[j] = lpt[j];
			type[j] = NULL;
			
			char string[200];
			sprintf(string, "The antenna vendor %s\nand antenna type %s\nparameters are incorrect!",vendor, type);
			
			MessageBox(string,"File Input Error",MB_ICONSTOP);
		}
		
		return FALSE;
	}

	// when all 44 values are successfully read for the specified antenna vendor and type
	else
	{
		// converting all values to metres
		for(i = 0; i < 3; i++)
		{
			L1apc[i] /= 1000.0;
			L2apc[i] /= 1000.0;
		}

		for(i = 0; i < 19; i++)
		{
			L1pcv[i] /= 1000.0;
			L2pcv[i] /= 1000.0;
		}

		double c1 = pow(FREQ_L1,2)/(pow(FREQ_L1,2) - pow(FREQ_L2,2));	// 2.546
		double c2 = pow(FREQ_L2,2)/(pow(FREQ_L2,2) - pow(FREQ_L1,2));	// -1.546

		// computing the antenna phase offsets for east, north and height
		m_options.arp2apc_east = c1 * L1apc[0] + c2 * L2apc[0];
		m_options.arp2apc_north = c1 * L1apc[1] + c2 * L2apc[1];
		m_options.arp2apc_up = c1 * L1apc[2] + c2 * L2apc[2];

		return TRUE;
	}
}


/* -------------------------------------------------------------------------------
Name:			Init_coord_NAD83toITRF

Purpose:		To convert initial coordinates from NAD83 to ITRF00 when initial
				coordinates are specified as NAD83.

History:		Name				Date		Reason for modification
                PMIS                2003        Original
------------------------------------------------------------------------------- */
void CMainFrame::Init_coord_NAD83toITRF()
{
	// NAD83 to ITRF00 conversion for Cartesian coordinates when GPS*C products are not used.
	if (m_options.ref == 0 && m_options.init_ref_sys == 2
		&& m_options.eph_flag != 2 && m_options.clock_flag != 3)
	{
		double Xitrf, Yitrf, Zitrf;
		
		// converting initial XYZ NAD83 coordinates to ITRF00 (*8*)
		Coor_Trans coor_trans;
		coor_trans.NAD83toITRFXYZ(m_options.origNADa,m_options.origNADb,m_options.origNADc,
			Xitrf,Yitrf,Zitrf,m_options.coord_year*10000 + m_options.coord_month*100
			+ m_options.coord_day);

		m_options.direca = Xitrf;
		m_options.direcb = Yitrf;
		m_options.direcc = Zitrf;
	}
	
	// NAD83 to ITRF00 conversion for Ellipsoidal coordinates when GPS*C products are not used.
	if (m_options.ref == 1 && m_options.init_ref_sys == 2
		&& m_options.eph_flag != 2 && m_options.clock_flag != 3)
	{
		double LATitrf, LONGitrf, Hitrf;
		
		DMS lat, lon;
		lat.InputDMS(m_options.origNADa);
		lon.InputDMS(m_options.origNADb);
		
		// converting initial XYZ NAD83 coordinates to ITRF00 (*8*)
		Coor_Trans coor_trans;
		coor_trans.NAD83toITRF(lat.deg+lat.min/60.0+lat.sec/3600.0,
			lon.deg+lon.min/60.0+lon.sec/3600.0,
			m_options.origNADc,
			LATitrf,LONGitrf,Hitrf,
			m_options.coord_year*10000 + m_options.coord_month*100
			+ m_options.coord_day);
		
		DMS latdms;
		latdms.setDeg(LATitrf);

		double deg=fabs(latdms.deg*10000)+fabs(latdms.min*100.0)
			+fabs(latdms.sec);
		if (latdms.deg<0)
			deg=-deg;
		m_options.direca=deg;	// latitude in DDMMSS.SSSSS format
		
		DMS londms;
		londms.setDeg(LONGitrf);
		
		deg=fabs(londms.deg*10000)+fabs(londms.min*100.0)
			+fabs(londms.sec);
		if (londms.deg<0)
			deg=-deg;
		m_options.direcb=deg;	// longitude in DDMMSS.SSSSS format
		
		m_options.direcc = Hitrf;	// height
	}

	// ITRF00 to NAD83 Cartesian
	if (m_options.ref == 0 && m_options.init_ref_sys != 2
		&& m_options.eph_flag == 2 && m_options.clock_flag == 3)
	{
		double Xnad, Ynad, Znad;
		
		// converting initial XYZ ITRF00 coordinates to NAD83 when using GPS*C (*1*)
		Coor_Trans coor_trans;
		coor_trans.ITRFtoNAD83XYZ(m_options.direca,m_options.direcb,m_options.direcc,
			Xnad,Ynad,Znad,m_options.coord_year*10000 + m_options.coord_month*100
			+ m_options.coord_day);

		m_options.origNADa = Xnad;
		m_options.origNADb = Ynad;
		m_options.origNADc = Znad;
	}

	// ITRF00 to NAD83 Ellipsoidal
	if (m_options.ref == 1 && m_options.init_ref_sys != 2
		&& m_options.eph_flag == 2 && m_options.clock_flag == 3)
	{
		double LATnad, LONGnad, Hnad;
		
		DMS lat, lon;
		lat.InputDMS(m_options.direca);
		lon.InputDMS(m_options.direcb);
		
		// converting initial XYZ ITRF00 coordinates to NAD83 when using GPS*C (*1*)
		Coor_Trans coor_trans;
		coor_trans.ITRFtoNAD83(lat.deg+lat.min/60.0+lat.sec/3600.0,
			lon.deg+lon.min/60.0+lon.sec/3600.0,
			m_options.direcc,
			LATnad,LONGnad,Hnad,
			m_options.coord_year*10000 + m_options.coord_month*100
			+ m_options.coord_day);
		
		DMS latdms;
		latdms.setDeg(LATnad);

		double deg=fabs(latdms.deg*10000)+fabs(latdms.min*100.0)
			+fabs(latdms.sec);
		if (latdms.deg<0)
			deg=-deg;
		m_options.origNADa=deg;	// latitude in DDMMSS.SSSSS format
		
		DMS londms;
		londms.setDeg(LONGnad);
		
		deg=fabs(londms.deg*10000)+fabs(londms.min*100.0)
			+fabs(londms.sec);
		if (londms.deg<0)
			deg=-deg;
		m_options.origNADb=deg;	// longitude in DDMMSS.SSSSS format
		
		m_options.origNADc = Hnad;	// height
	}
}

