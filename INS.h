//
//  INS.h
//  
//
//  Created by Bei Huang on 2013-05-06.
//
//

#ifndef ____INS__
#define ____INS__

#include "Matrix.h"
#include <iostream>

class INS
{
public:
    INS();
    ~INS();
    
    
    Matrix P(21, 21);
    Matrix Q(21, 21);
    Matrix R(21, 21);
    
    Matrix F(21, 21);
    Matrix H(6, 21);
    Matrix Z(6, 1);
    Matrix X(21, 1);
    
    double pos[3];
    double pos_std[3];
    double vel[3];
    double vel_std[3];
    Matrix dcmCb2n(3, 3);
    double pitch;
    double pitch_std[3];
    double roll;
    double roll_std[3];
    double azimuth;
    double azimuth_std[3];
    double accel_bias[3];
    double accel_SF[3];
    double gyro_bias[3];
    double gyro_SF[3];
    
    double deltaVn[3];      // delta velocity in navigation frame
    double velocityInc[2];
    double angularInc[3];    // angular increment
    double qCb2n[4];         // quaternion form of Cb2n
    double fN[3];
    
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
}

#endif /* defined(____INS__) */
