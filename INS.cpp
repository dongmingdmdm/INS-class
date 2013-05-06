//
//  INS.cpp
//  
//
//  Created by Bei Huang on 2013-05-06.
//
//

#include "INS.h"
#include <stdio.h>
#include <string.h>
#include <cmath.h>

using namespace std;

INS::INS()
{
    memset(pos, 0.0, 3*sizeof(double));
    memset(pos_std, 0.0, 3*sizeof(double));
    memset(vel, 0.0, 3*sizeof(double));
    memset(vel_std, 0.0, 3*sizeof(double));
    pitch   = 0.0;
    pitch_std = 0.0;
    roll    = 0.0;
    roll_std = 0.0;
    azimuth = 0.0;
    azimuth_std = 0.0;
    memset(accel_bias, 0.0, 3*sizeof(double));
    memset(accel_SF, 0.0, 3*sizeof(double));
    memset(gyro_bias, 0.0, 3*sizeof(double));
    memset(gyro_SF, 0.0, 3*sizeof(double));
    
    numofstates = 0;
    
    init_start_time = 0.0;
    run_start_time  = 0.0;
    sample_rate     = 50.0;
    
    memset(accel_bias_std, 0.0, 3*sizeof(double));
    memset(accel_bias_GM_frqspctrl, 0.0, 3*sizeof(double));
    memset(accel_bias_GM_corrtime, 0.0, 3*sizeof(double));
    
    memset(gyro_bias_std, 0.0, 3*sizeof(double));
    memset(gyro_bias_GM_frqspctrl, 0.0, 3*sizeof(double));
    memset(gyro_bias_GM_corrtime, 0.0, 3*sizeof(double));
    
    EstimateScaleFactor = false;
    
    memset(accel_SF_std, 0.0, 3*sizeof(double));
    memset(accel_SF_GM_frqspctrl, 0.0, 3*sizeof(double));
    memset(accel_SF_GM_corrtime, 0.0, 3*sizeof(double));
    
    memset(gyro_SF_std, 0.0, 3*sizeof(double));
    memset(gyro_SF_GM_frqspctrl, 0.0, 3*sizeof(double));
    memset(gyro_SF_GM_corrtime, 0.0, 3*sizeof(double));
    
    gravity = 9.8;
    M       = 6378137.0;
    N       = 6378137.0;
    
    T = 1/sample_rate;
    memset(deltaVn, 0.0, 3*sizeof(double));
    memset(angularInc, 0.0, 3*sizeof(double));
    memset(velocityInc, 0.0, 3*sizeof(double));
    memset(qCb2n, 0.0, 4*sizeof(double));
}

INS::~INS()
{

}

void INS::initSetting()
{
    
    init_start_time = 50000.0;
    run_start_time  = 50000.0;
    sample_rate     = 50.0;
    
    
    if (EstimateScaleFactor)
    {
        numofstates = 21;
    }
    else
    {
        numofstates = 15;
    }
    
    
}

void INS::coraseAlignment()
{
    
}

void insMechnization(const double * INS_pre, const double * INS)
{
    CalGravity();
    CalMN();
    
    double prePos[3] = {0.0};
    memcpy(prePos, pos, 3*sizeof(double));
    double preVel[3] = {0.0};
    memcpy(preVel, vel, 3*sizeof(double));
    double preDeltaVn[3] = {0.0};
    memcpy(preDeltaVn, INS_pre, 3*sizeof(double));
    
    double gN[3] = {0.0, 0.0, -gravity};
    double we    = 0.00007292115;
    /*******************/
    /* Attitude Update */
    /*******************/
    // earth rotation rate in navigation frame
    double WieN[3] = {0.0, we*cos(prePos[0]), we*sin(prePos[0])};
    
    // transport rate
    double WenN[3] = {-preVel[1]/(M+prePos[2]), preVel[0]/(N+prePos[1]), preVel[0]*tan(prePos[0]/(N+prePos[2]))}; 
    
    // rotation
    double WinN[3] = {0.0};
    WinN[0] = WieN[0] + WenN[0];
    WinN[1] = WieN[1] + WenN[1];
    WinN[2] = WieN[2] + WenN[2];
    
    double WinB[3] = {0.0};
    
    Matrix dcmCn2b(3, 3);
    dcmCn2b.Matrix_copy(dcmCb2n, 3, 3);
    dcmCn2b.invert(3);
    WinB[0] = dcmCn2b.matrix[0][0] * WinN[0] + dcmCn2b.matrix[0][1] * WinN[1] + dcmCn2b.matrix[0][2] * WinN[2];
    WinB[1] = dcmCn2b.matrix[1][0] * WinN[0] + dcmCn2b.matrix[1][1] * WinN[1] + dcmCn2b.matrix[1][2] * WinN[2];
    WinB[2] = dcmCn2b.matrix[2][0] * WinN[0] + dcmCn2b.matrix[2][1] * WinN[1] + dcmCn2b.matrix[2][2] * WinN[2];
    
    double Theta[3] = {0.0};
    Theta[0] = angularInc[0] - WinB[0] * T;
    Theta[1] = angularInc[1] - WinB[1] * T;
    Theta[2] = angularInc[2] - WinB[2] * T;
    
    // Attitude update
    double modTheta = sqrt(Theta[0]*Theta[0] + Theta[1]*Theta[1] + Theta[2]*Theta[2]);
    double s        = sin(modTheta/2) / modTheta;
    double c        = cos(modTheta/2);
    double A[16]     = {c, -s*Theta[0], -s*Theta[1], -s*Theta[2],
                        s*Theta[0],  c,  s*Theta[2],  s*Theta[1],
                        s*Theta[1], -s*Theta[2],  c,  s*Theta[0],
                        s*Theta[2],  s*Theta[1], -s*Theta[0],  c};
    
    double qOut[4]  = {A[0]*qCb2n[0]+A[1]*qCb2n[1]+A[2]*qCb2n[2]+A[3]*qCb2n[3],
                       A[4]*qCb2n[0]+A[5]*qCb2n[1]+A[6]*qCb2n[2]+A[7]*qCb2n[3],
                       A[8]*qCb2n[0]+A[9]*qCb2n[1]+A[10]*qCb2n[2]+A[11]*qCb2n[3],
                       A[12]*qCb2n[0]+A[13]*qCb2n[1]+A[14]*qCb2n[2]+A[15]*qCb2n[3]};
    
    quatnormalize(qOut);
    
    quat2dcm(qOut);
    
    /*****************/
    /*Velocity Update*/
    /*****************/
    double corialisAngle[3] = {0.0};
    corialisAngle[0] = 2*WieN[0] + WenN[0];
    corialisAngle[1] = 2*WieN[1] + WenN[1];
    corialisAngle[2] = 2*WieN[2] + WenN[2];
    
    double corialisOmega[9] = {0.0, -corialisAngle[2],  corialisAngle[1],
                               corialisAngle[2],  0.0, -corialisAngle[0],
                              -corialisAngle[1],  corialisAngle[0],  0.0};
    
    double Av[9]   = {1.0, -0.5*Theta[2],  0.5*Theta[1],
                      0.5*Theta[2],  1.0, -0.5*Theta[0],
                      -0.5*Theta[1], 0.5*Theta[0], 1.0};
    
    double VfN[3]  = {Av[0]*velocityInc[0]+Av[1]*velocityInc[1]+Av[2]*velocityInc[2],
                      Av[3]*velocityInc[0]+Av[4]*velocityInc[1]+Av[5]*velocityInc[2],
                      Av[6]*velocityInc[0]+Av[7]*velocityInc[1]+Av[8]*velocityInc[2]};
    
    VfN[0] = dcmCb2n.matrix[0][0] * VfN[0] + dcmCb2n.matrix[0][1] * VfN[1] + dcmCb2n.matrix[0][2] * VfN[2];
    VfN[1] = dcmCb2n.matrix[1][0] * VfN[0] + dcmCb2n.matrix[1][1] * VfN[1] + dcmCb2n.matrix[1][2] * VfN[2];
    VfN[2] = dcmCb2n.matrix[2][0] * VfN[0] + dcmCb2n.matrix[2][1] * VfN[1] + dcmCb2n.matrix[2][2] * VfN[2];
    
    deltaVn[0] = VfN[0] + (gN[0] - corialisOmega[0]*preVel[0]+corialisOmega[1]*preVel[1]+corialisOmega[2]*preVel[2]) * T;
    deltaVn[1] = VfN[1] + (gN[1] - corialisOmega[3]*preVel[0]+corialisOmega[4]*preVel[1]+corialisOmega[5]*preVel[2]) * T;
    deltaVn[2] = VfN[2] + (gN[2] - corialisOmega[6]*preVel[0]+corialisOmega[7]*preVel[1]+corialisOmega[8]*preVel[2]) * T;
    
    vel[0] = preVel[0] + 0.5*(deltaVn[0] + preDeltaVn[0]);
    vel[1] = preVel[1] + 0.5*(deltaVn[1] + preDeltaVn[1]);
    vel[2] = preVel[2] + 0.5*(deltaVn[2] + preDeltaVn[2]);
    
    fN[0] = VfN[0] / T;
    fN[1] = VfN[1] / T;
    fN[2] = VfN[2] / T;
    
    /*****************/
    /*Position Update*/
    /*****************/
    pos[2] = prePos[2] + 0.5*(vel[2] + preVel[2]) * T;
    pos[0] = prePos[0] + 0.5*(vel[1] + preVel[1])/(M+pos[2]) * T;
    pos[1] = prePos[1] + 0.5*(vel[0] + preVel[0])/(N+pos[2])/cos(pos[0]) * T;
}

void insPropagation()
{
    double we = 0.00007292115;
    
    double T  = currentTime - filterTime;
    filterTime = currentTime;
    
    double Ve = vel[0];
    double Vn = vel[1];
    double Vu = vel[2];
    
    double lat = pos[0];
    double h   = pos[2];
    
    double fe = fN[0];
    double fn = fN[1];
    double fu = fN[2];
    
    double MplusH = M + pos[2];
    double NplusH = N + pos[2];
    
    double latDot = Vn/MplusH;
    double lonDot = Ve/NplusH/cos(lat);
    
    /////////////////////////////
    // Build Transformation Matrix
    /////////////////////////////
    /********************/
    /*Pos err vs pos err*/
    /********************/
    F.matrix[0][2] = -latDot/MplusH;
    F.matrix[1][0] = lonDot/tan(lat);
    F.matrix[1][2] = -lonDot/NplusH;
    
    /********************/
    /*Pos err vs vel err*/
    /********************/
    F.matrix[0][4] = 1/MplusH;
    F.matrix[1][3] = 1/NplusH/cos(lat);
    F.matrix[2][5] = 1.0;
    
    /********************/
    /*Vel err vs pos err*/
    /********************/
    F.matrix[3][0] = 2*we*(Vu*sin(lat) + Vn*cos(lat)) + Vn*lonDot/cos(lat);
    F.matrix[4][0] = -2*we*Ve*cos(lat) - Ve*lonDot/cos(lat);
    F.matrix[5][0] = -2*we*Ve*sin(lat);
    F.matrix[5][2] = -2*gN[2]/sqrt(M*N);
    
    /********************/
    /*Vel err vs vel err*/
    /********************/
    F.matrix[3][3] = -Vu/NplusH + Vn*tan(lat)/NplusH;
    F.matrix[3][4] = (2*we+lonDot)*sin(lat);
    F.matrix[3][5] = -(2*we+lonDot)*cos(lat);;
    F.matrix[4][3] = -F.matrix[3][4];
    F.matrix[4][4] = -Vu/MplusH;
    F.matrix[4][5] = -latDot;
    F.matrix[5][3] = -F.matrix[3][5];
    F.matrix[5][4] = 2*latDot;
    
    /********************/
    /*Vel err vs att err*/
    /********************/
    F.matrix[3][7] = fu;
    F.matrix[3][8] = -fn;
    F.matrix[4][6] = -fu;
    F.matrix[4][8] = fe;
    F.matrix[5][6] = fn'
    F.matrix[5][7] = -fe;
    
    /********************/
    /*Vel err vs accel bias*/
    /********************/
    F.matrix[3][9] = dcmCb2n.matrix[0][0]; F.matrix[3][10] = dcmCb2n.matrix[0][1]; F.matrix[3][11] = dcmCb2n.matrix[0][2];
    F.matrix[4][9] = dcmCb2n.matrix[1][0]; F.matrix[4][10] = dcmCb2n.matrix[1][1]; F.matrix[4][11] = dcmCb2n.matrix[1][2];
    F.matrix[5][9] = dcmCb2n.matrix[2][0]; F.matrix[5][10] = dcmCb2n.matrix[2][1]; F.matrix[5][11] = dcmCb2n.matrix[2][2];
    
    if (EstimateScaleFactor) {
        F.matrix[3][15] = dcmCb2n.matrix[0][0]; F.matrix[3][16] = dcmCb2n.matrix[0][1]; F.matrix[3][17] = dcmCb2n.matrix[0][2];
        F.matrix[4][15] = dcmCb2n.matrix[1][0]; F.matrix[4][16] = dcmCb2n.matrix[1][1]; F.matrix[4][17] = dcmCb2n.matrix[1][2];
        F.matrix[5][15] = dcmCb2n.matrix[2][0]; F.matrix[5][16] = dcmCb2n.matrix[2][1]; F.matrix[5][17] = dcmCb2n.matrix[2][2];
    }
    
    
    /********************/
    /*Att err vs vel err*/
    /********************/
    F.matrix[6][4] = 1/MplusH;
    F.matrix[7][3] = -1/NplusH;
    F.matrix[8][3] = -tan(lat)/NplusH;
    
    /********************/
    /*Att err vs att err*/
    /********************/
    F.matrix[6][7] = (we + lonDot)*sin(lat);
    F.matrix[6][8] = -(we + lonDot)*cos(lat);
    F.matrix[7][6] = -F.matrix[6][7];
    F.matrix[7][8] = -latDot;
    F.matrix[8][6] = -F.matrix[6][8];
    F.matrix[8][7] = -F.matrix[7][8];
    
    /********************/
    /*Att err vs gyro bias*/
    /********************/
    F.matrix[6][12] = dcmCb2n.matrix[0][0]; F.matrix[6][13] = dcmCb2n.matrix[0][1]; F.matrix[6][14] = dcmCb2n.matrix[0][2];
    F.matrix[7][12] = dcmCb2n.matrix[1][0]; F.matrix[7][13] = dcmCb2n.matrix[1][1]; F.matrix[7][14] = dcmCb2n.matrix[1][2];
    F.matrix[8][12] = dcmCb2n.matrix[2][0]; F.matrix[8][13] = dcmCb2n.matrix[2][1]; F.matrix[8][14] = dcmCb2n.matrix[2][2];
    
    if (EstimateScaleFactor) {
        F.matrix[6][18] = dcmCb2n.matrix[0][0]; F.matrix[6][19] = dcmCb2n.matrix[0][1]; F.matrix[6][20] = dcmCb2n.matrix[0][2];
        F.matrix[7][18] = dcmCb2n.matrix[1][0]; F.matrix[7][19] = dcmCb2n.matrix[1][1]; F.matrix[7][20] = dcmCb2n.matrix[1][2];
        F.matrix[8][18] = dcmCb2n.matrix[2][0]; F.matrix[8][19] = dcmCb2n.matrix[2][1]; F.matrix[8][20] = dcmCb2n.matrix[2][2];
    }
    
    /******************/
    /*accel bias      */
    /******************/
    F.matrix[9][9] = -accel_bias_GM_corrtime[0];
    F.matrix[10][10] = -accel_bias_GM_corrtime[1];
    F.matrix[11][11] = -accel_bias_GM_corrtime[2];
    
    /******************/
    /* gyro bias      */
    /******************/
    F.matrix[12][12] = -gyro_bias_GM_corrtime[0];
    F.matrix[13][13] = -gyro_bias_GM_corrtime[1];
    F.matrix[14][14] = -gyro_bias_GM_corrtime[2];
    
    //////////////////////////
    // Build Shape Matrix
    //////////////////////////
    
    
    //////////////////////////
    // Kalman Filter
    //////////////////////////
    Matrix eye(numofstates, numofstates);
    Matrix dTrans(numofstates, numofstates);
    for (int i = 0; i < numofstates; i++)
    {
        for (int j = 0; j < numofstates; j++)
        {
            dTrans.matrix[i][j] = eye.matrix[i][j] + F.matrix[i][j]*T;
        }
    }
    
    
}

void integration(const double * GPS)
{

}

void CalGravity(double * pos)
{

}

void CalMN(double * pos)
{

}

void quatnormalize(double * q)
{

}

void quat2dcm(double * q)
{

}

