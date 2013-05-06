//////////////////////////////////////////////////////////////////////
//
// MainFrm.h
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


#if !defined(AFX_MAINFRM_H__3460F6BB_1F0F_11D6_A315_0050DAD66AD4__INCLUDED_)
#define AFX_MAINFRM_H__3460F6BB_1F0F_11D6_A315_0050DAD66AD4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Constant.h"

#include "obs.h"
#include "Epoch.h"
#include "Ephemeris.h"
#include "Output.h"
#include "Structure.h"
#include "rinex.h"

/*Bei add this in March 2013*/
#include "PreciseClockFile.h"
#include "PreciseOrbitFile.h"
/*Bei add this in March 2013*/

#ifndef POST_MISSION
#include "RT\mscomm.h"
#include "RT\ReceiveSocket.h"
#include "RT\JPLDecode.h"
#include "RT\NDecode.h"
#include "RT\JDecode.h"
#include "RT\IMUComm.h"
#include "RT_EKF_pub\RT_EKF.h"

#endif

#include "drawDoc.h"
#include "drawView.h"

#include "SP3File.h"
#include "5MinClk.h"

// this is the message ID indicates a serial port event for GPS
#define ID_COMMGPS  WM_USER+2001 

/* Bei modified the IMUComm part in Jan 2013*/
#define INIT_ALIGN_TIME 300
#define IMU_DATARATE 50
#define NUM_SOLUTION_STATES 43
typedef struct IMU_DATAPKG {
	bool  echo;
	double ax;
	double ay;
	double az;
	double gx;
	double gy;
	double gz;
	double roll;
	double pitch;
	double yaw;
	double latitude;
	double longitude;
	double altitude;
	double temp;
	unsigned int time;
} IMUDataPkg;
/* Bei modified the IMUComm part in Jan 2013*/

// the header of the following classes are not included in this header
// but they are used in the class CMainFrame, declare them first
class CSatPicView;			// class to display satellite azi & ele in a picture
class CAzmElvView;			// class to display satellite azi & ele in a table
class CResultView;			// class to display processing results 
class CRangeView;			// class to display residuals
class CMsgView;				// class to display processing information
class CTrajView;			// class to display trajectory (kinematic mode) or errors (static mode)

// this is the main frame and interface of this application   
class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame) 

public:
	// construction
	CMainFrame();

// Attributes

private:

	CPRECISE_EPH	m_precise_eph[MAX_GPS_PRN];		// GPS precise orbit correction list
	CPRECISE_CLOCK	m_precise_clock[MAX_GPS_PRN];	// GPS precise clock correction list
	CGPS_EPH		m_gps_eph[MAX_GPS_PRN];			// broadcast GPS ephemeris list
	CSP3Reader		m_SP3Reader;					// open and organize sp3 format precise ephemeris
	C5MinClk		m_5MinClk;						// open and organize RINEX format precise clocks
	CEpochList		m_arrayEpoch;					// to save observations for backward processing
	
	CEpoch			m_epoch;		// organize all information in an epoch from observations, unknowns, and processing information
	int				m_numEpoch;		// number of epochs processed
	bool			m_bStart;		// indicate if the processing is in progress, 0:no, 1:yes
	bool			m_bPause;		// indicate if the processing is paused, 0:no, 1:yes
	bool			m_bStop;		// indicate if the processing is stoped, 0:no, 1:yes
	bool			m_bClose;		// indicate if the user has choosen to close the application, 0:no, 1:yes
	int				m_iDisplay;		// indicate the number of charts to be displayed on the screen
	
	CPARAMETER		m_parameter_ini;// initial parameters
	COPTION			m_options;		// save the settings user selected

	CString			m_strAppPath;	// the path of the application

	CDrawView*		m_pDrawView;	// pointer to the window displaying various charts of the results
	CSatPicView*	m_pPicView;		// pointer to the window displaying satellite azi & ele in a picture
	CAzmElvView*	m_pAzmView;		// pointer to the window displaying satellite azi & ele in a table
	CResultView*	m_pResView;		// pointer to the window displaying processing results of one epoch
	CRangeView*		m_pRngView;		// pointer to the window displaying residuals
	CTrajView*		m_pTrajView;	// pointer to the window displaying trajectory (kinematic mode) or errors (static mode)	
	CMsgView*		m_pMsgView;		// pointer to the window displaying processing information
	CFrameWnd*		m_p5Splitter;	// pointer to the 5-way splitted window

	CProgressCtrl	m_ProgressCalc;	// a bar indicates the progress of processing
	CStatic			m_stcBegin,m_stcEnd,m_stcCur;	// display the time tag of the first epoch, last epoch and current epoch

	CString	m_strAntType;			// contain the information of the antenna name

	/* Bei added this IMU part in Jan 2013*/
	char m_InitGPSFilename[500];
	char m_InitIMUFilename[500];
	char m_GPSFilename[500];
	char m_IMUFilename[500];
	char gps_ins_solution_filename[500];
	ofstream gps_ins_solution_file;
	ofstream m_InitGPSFile;
	FILE * m_InitIMUFile;
	FILE * m_IMUFile;
	double m_IMURate;
	/* Bei added this IMU part in Jan 2013*/

#ifndef POST_MISSION
	JDecode			m_jDecode;		// to decode raw date from Javad receiver
	NDecode			m_nDecode;		// to decode raw date from Novatel receiver
	//CRDecode		m_rDecode;		// to decode RTCA format real-time precise corrections
	JPLDecode		m_jplDecode;	// to decode IGDG format real-time precise corrections
	CMSComm			m_commGPS;		// to receive GPS raw data from serial port
	CReceiveSocket* m_pSocketRTCA;	// to receive real-time precise corrections over Internet

	/* Bei added this IMU part in Jan 2013*/
	IMUComm m_IMUComm;
	//EKF m_EKF;
	double m_EKF_Estimation[NUM_SOLUTION_STATES];
	double init_align_start_time;
	/* Bei added this IMU part in Jan 2013*/

	int		m_iGPSPort;				// the serial port to receive GPS data
	int		m_iGPSBaudrate;			// the baudrate of the serial port
	int		m_iRTCAIP[4];			// the IP address of the server to broadcast GPS corrections
	int		m_iRTCAPort;			// the port of the server to broadcast GPS corrections

	struct	in_addr sin_addr_now;	// the Internet address to request real-time corrections
	int		counter_RTCA;			// number of RTCA message received
	struct	sockaddr_in name;		// the port number to receive real-time corrections
	char	requestor[128];			// the contact information sent to the server to request the real-time correction
	int		m_iRcvType;				// the receiver type for raw data decode, 0: Javad, 1: Novatel
#endif


public:		

	bool		m_bViewDraw;		// indicate if the window m_pDrawView is shown or not

	bool			m_bBackward;	// indicate if a backward processing will be conducted, 0:no, 1:yes
	Matrix		PxForward,PxBackward, Pxsmooth,StateForward,StateBackward,Statesmooth;

	ofstream	m_ofslog;			// the output stream for log file
	bool		m_bLogPPP;			// log processing results or not
	bool		m_bViewError;		// show the positioning errors or not in the m_pDrawView
	bool		m_bViewTrajectory;	// show the trajectory or not in the m_pDrawView
	bool		m_bViewSats;		// show the number of satellites or not in the m_pDrawView
	bool		m_bViewZTD;			// show the Zenith Tropospheric Delay or not in the m_pDrawView
	bool		m_bViewAmb;			// show the ambiguities or not in the m_pDrawView
	bool		m_bViewVel;			// show the velocity or not in the m_pDrawView
	bool		m_bViewClock;		// show the receiver clock offset or not in the m_pDrawView
	bool		m_bViewPDOP;		// show the PDOP value or not in the m_pDrawView
	bool		m_bViewHDOP;		// show the HDOP value or not in the m_pDrawView
	bool		m_bViewVDOP;		// show the VDOP value or not in the m_pDrawView
	bool		m_bViewTDOP;		// show the TDOP value or not in the m_pDrawView
	bool		m_bViewGDOP;		// show the GDOP value or not in the m_pDrawView
	bool		m_bEnableTraj;		// indicate if the log file contains trajectory information
	bool		m_bEnableError;		// indicate if the log file contains positioning error information
	bool		m_bEnableVel;		// indicate if the log file contains velocity information
	bool		m_bEnableAmb;		// indicate if the log file contains ambiguity information

	double		L1apc[3];			// antenna phase center offset for L1 signal
	double		L1pcv[19];			// antenna phase center variations for L1 signal
	double		L2apc[3];			// antenna phase center offset for L2 signal
	double		L2pcv[19];			// antenna phase center variations for L2 signal


// Operations
private:	

#ifndef POST_MISSION
	void ReceiveGPS(VARIANT pchData);	// to receive GPS raw data from serial port
	void InitGPS();						// send commands to receiver to request observations

#endif

	void ReadOption();					// obtain settings from registry table
	void SaveOption();					// save settings to registry table
	void OnPostProcess();				// start post processing
	void OpenLog();						// open the log file

public:

	CEpoch*			GetEpoch() { return &m_epoch; };				// return a pointer to the current Epoch
	CGPS_EPH*		GetEph() { return m_gps_eph; };					// return a pointer to the broadcast ephemeris
	COPTION*		GetOption() { return &m_options; };				// return a pointer to the current settings
	CSP3Reader*		GetSP3Reader() { return	&m_SP3Reader;};			// return a pointer to the SP3 format ephemeris
	C5MinClk*		Get5MinClk() { return	&m_5MinClk;};			// return a pointer to the 5-minute clock
	CString			GetAntType() { return	m_strAntType;};			// return antenna name
	bool			GetDirection() { return m_bBackward; };			// return the backward processing setting
	CPRECISE_CLOCK* GetPriciseClock() { return m_precise_clock; };	// return a pointer to the current precise clocks
	CPRECISE_EPH*	GetPriciseEph() { return m_precise_eph; };		// return a pointer to the current precise orbits
	int				GetNumEpoch() {return m_numEpoch;};				// return how many epochs have been processed
	CSatPicView*	GetPicView()  { return m_pPicView;  };			// return the window displaying satellite information 
	CResultView*	GetResView()  { return m_pResView;  };			// return the window displaying processing results 
	CAzmElvView*	GetAzmView()  { return m_pAzmView;  };			// return the window displaying satellite Azi & Ele 
	CRangeView*		GetRngView()  { return m_pRngView;  };			// return the window displaying residuals 
	CTrajView*		GetTrajView()  { return m_pTrajView;  };		// return the window displaying trajectory 
	CMsgView*		GetMsgView()  { return m_pMsgView;  };			// return the window displaying processing information 
	CDrawView*		GetDrawView()  { return m_pDrawView;  };		// return the window displaying charts 
	CString			GetAppPath()  { return m_strAppPath; };			// return the path of the application 
	bool			GetStart() { return m_bStart; };				// return if a processing is in progress 

	void DoEvents();		// to Peek Message
	void Delay(long sec);	// delay a task for a period

	void ShowAzmElv();													// update satellite Azi & Ele
	void ShowMessage(CString msg, int clear);							// update processing information
	void ShowTraj(double t, double p, double l, double h, double UTC);	// update trajectory
	void ShowaResult();													// update processing results

	void SetAppPath(CString str)  { m_strAppPath=str; };					// set the path of the application
	// the following functions assign the pointers to the activated windows
	void SetDrawView(CView *pView) { m_pDrawView=(CDrawView*)pView;  };		
	void SetSatPicView(CView *pView) { m_pPicView=(CSatPicView*)pView;  };
	void SetResultView(CView *pView) { m_pResView=(CResultView*)pView;  };
	void SetAzmElvView(CView *pView) { m_pAzmView=(CAzmElvView*)pView;  };
	void SetRangeView(CView *pView) { m_pRngView=(CRangeView*)pView;  };
	void SetTrajView(CView *pView) { m_pTrajView=(CTrajView*)pView;  };
	void SetMsgView(CView *pView) { m_pMsgView=(CMsgView*)pView;  };
	void Set5Splitter(CFrameWnd *pWnd) { m_p5Splitter=pWnd;  };

	int  PickBestGPSEph( double		ref_time,						// to pick a best fit set of ephemeris for all satellites
					    CGPS_EPHList	&Eph, int prn=1);			
	int  PickBestGPSEphs( double	ref_time,CGPS_EPHList	*Eph);	// to pick a best fit set of ephemeris for a satellite

	void InitializeParameter();		// initialize the parameters, unknowns 
	bool CheckForLOGOverwrite();	// check if a file exist with the path user selected for the log file
	bool GetPCV();					// obtain antenna phase center center offset and variations
	void Init_coord_NAD83toITRF();	// convert initial coordinates from NAD83 to ITRF00 when initial coordinates are specified as NAD83.

#ifndef POST_MISSION
	int	GetRcvType() { return m_iRcvType; };			// return receiver type
	void ReceiveRTCA();				// receive real-time corrections over Internet

	/* Bei added this IMU part in Jan 2013*/
	int SyncGPSINS();
	void Start_GPSINS_Integration(IMUDataPkg * datapkg);
	/* Bei added this IMU part in Jan 2013*/
#endif
	
// the following part is generated by VC or in ClassWizard, do not change it manually

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;		
	CToolBar    m_wndToolBar;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnStart();
	afx_msg void OnDestroy();
	afx_msg void OnPause();
	afx_msg void OnUpdatePause(CCmdUI* pCmdUI);
	afx_msg void OnStop();
	afx_msg void OnUpdateStop(CCmdUI* pCmdUI);
	afx_msg void OnUpdateStart(CCmdUI* pCmdUI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();
	afx_msg void OnDrawView();
	afx_msg void OnUpdateDrawView(CCmdUI* pCmdUI);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSetup();
	afx_msg void OnSettings();
	afx_msg void OnAntenna();
	afx_msg void OnSatExclusion();
	afx_msg void OnMeteorology();
	afx_msg void OnInitialCoor();
	afx_msg void OnPpp();
	afx_msg void OnUpdatePpp(CCmdUI* pCmdUI);
	afx_msg void OnSpp();
	afx_msg void OnUpdateSpp(CCmdUI* pCmdUI);
	afx_msg void OnPostMission();
	afx_msg void OnUpdatePostMission(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRealTime(CCmdUI* pCmdUI);
	afx_msg void OnProcessing();
	afx_msg void OnSystem();
	afx_msg void OnReport();
	afx_msg void OnDisplay();
	afx_msg void OnUpdateSystemStatus(CCmdUI* pCmdUI);
	afx_msg void OnUpdateStationStatus(CCmdUI* pCmdUI);
	afx_msg void OnUpdateProcessingStatus(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOrbitStatus(CCmdUI* pCmdUI);
	afx_msg void OnUpdateClockStatus(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCurrentEpoch(CCmdUI* pCmdUI);
	afx_msg void OnUpdateLaterUse(CCmdUI* pCmdUI);
	afx_msg void OnViewAmbiguity();
	afx_msg void OnViewClock();
	afx_msg void OnViewError();
	afx_msg void OnViewSats();
	afx_msg void OnViewTrajectory();
	afx_msg void OnViewVel();
	afx_msg void OnViewZtd();
	afx_msg void OnUpdateViewAmbiguity(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewClock(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewError(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewSats(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewTrajectory(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewVel(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewZtd(CCmdUI* pCmdUI);
	afx_msg void OnSplitter5();
	afx_msg void OnUpdateSplitter5(CCmdUI* pCmdUI);
	afx_msg void OnViewGdop();
	afx_msg void OnUpdateViewGdop(CCmdUI* pCmdUI);
	afx_msg void OnViewHdop();
	afx_msg void OnUpdateViewHdop(CCmdUI* pCmdUI);
	afx_msg void OnViewPdop();
	afx_msg void OnUpdateViewPdop(CCmdUI* pCmdUI);
	afx_msg void OnViewTdop();
	afx_msg void OnUpdateViewTdop(CCmdUI* pCmdUI);
	afx_msg void OnViewVdop();
	afx_msg void OnUpdateViewVdop(CCmdUI* pCmdUI);
	afx_msg void OnPppHelp();
	afx_msg void OnRealTime();
	//}}AFX_MSG
#ifndef POST_MISSION
	afx_msg LRESULT OnObsReady(WPARAM wParam = 0, LPARAM lParam = 0);
	afx_msg LRESULT OnEphReady(WPARAM wParam = 0, LPARAM lParam = 0);
	afx_msg void OnCommGPS();
#endif

	DECLARE_EVENTSINK_MAP()

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__3460F6BB_1F0F_11D6_A315_0050DAD66AD4__INCLUDED_)
