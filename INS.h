// INS.h: interface for the INS class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INS_H__33EDBF07_0DA4_41C5_B245_F302ECDA364D__INCLUDED_)
#define AFX_INS_H__33EDBF07_0DA4_41C5_B245_F302ECDA364D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Matrix.h"

class INS  
{
public:
	INS();
	virtual ~INS();

	Matrix P;
	Matrix G;
	Matrix R;
	Matrix F;
	Matrix H;
	Matrix Z;
	Matrix X;

	double pos[3];
    double pos_std[3];
    double vel[3];
    double vel_std[3];
    Matrix dcmCb2n;
    double pitch;
    double pitch_std;
    double roll;
    double roll_std;
    double azimuth;
    double azimuth_std;
    double accel_bias[3];
    double accel_SF[3];
    double gyro_bias[3];
    double gyro_SF[3];
    
    double deltaVn[3];      // delta velocity in navigation frame
    double velocityInc[2];
    double angularInc[3];    // angular increment
    double qCb2n[4];         // quaternion form of Cb2n
    double fN[3];
    
	int numofobs;
    int numofstates;
    
    // Settings for INS class
    double init_start_time;     // initial alignment start time
    double run_start_time;      // run start time, initial alignment end time as well
    double sample_rate;         // IMU sample rate
    bool   EstimateScaleFactor;
    double currentTime;
    double filterTime;

private:
    
    // Bias specifications for accelerometers and gyros
    double accel_bias_std[3];
    double accel_bias_GM_frqspctrl[3];
    double accel_bias_GM_corrtime[3];
    
    double gyro_bias_std[3];
    double gyro_bias_GM_frqspctrl[3];
    double gyro_bias_GM_corrtime[3];
    
    // Scale factor specifications for accelerometers and gyros
    double accel_SF_std[3];
    double accel_SF_GM_frqspctrl[3];
    double accel_SF_GM_corrtime[3];
    
    double gyro_SF_std[3];
    double gyro_SF_GM_frqspctrl[3];
    double gyro_SF_GM_corrtime[3];
    
    // Constants used in INS
    double gravity;
    double M;
    double N;
    
    double T;

public:
    void initSetting();     // set parameters and error specifications
    
    void coarseAlignment(); // coarse alignment = accelerometer leveling + gyro compassing
    
    void insMechnization(const double * INS_pre, const double * INS); // ins mechanization equations
    
	void insPropagation();  // ins propagation
    
    void integration(const double * GPS);   // integration (loose couple)
    
private:
    void CalGravity();
    
    void CalMN();
    
    void quatnormalize(double * q);
    
    void quat2dcm(double * q);

};

#endif // !defined(AFX_INS_H__33EDBF07_0DA4_41C5_B245_F302ECDA364D__INCLUDED_)
