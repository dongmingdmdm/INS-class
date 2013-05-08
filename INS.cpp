// INS.cpp: implementation of the INS class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "INS.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

INS::INS()
{
	memset(pos, 0, 3*sizeof(double));
    memset(pos_std, 0, 3*sizeof(double));
    memset(vel, 0, 3*sizeof(double));
    memset(vel_std, 0, 3*sizeof(double));
    pitch   = 0.0;
    pitch_std = 0.0;
    roll    = 0.0;
    roll_std = 0.0;
    azimuth = 0.0;
    azimuth_std = 0.0;
    memset(accel_bias, 0, 3*sizeof(double));
    memset(accel_SF, 0, 3*sizeof(double));
    memset(gyro_bias, 0, 3*sizeof(double));
    memset(gyro_SF, 0, 3*sizeof(double));
    
    numofstates = 0;
    numofobs = 0;

    init_start_time = 0.0;
    run_start_time  = 0.0;
    sample_rate     = 50.0;
    
    memset(accel_bias_std, 0, 3*sizeof(double));
    memset(accel_bias_GM_frqspctrl, 0, 3*sizeof(double));
    memset(accel_bias_GM_corrtime, 0, 3*sizeof(double));
    
    memset(gyro_bias_std, 0, 3*sizeof(double));
    memset(gyro_bias_GM_frqspctrl, 0, 3*sizeof(double));
    memset(gyro_bias_GM_corrtime, 0, 3*sizeof(double));
    
    EstimateScaleFactor = false;
    
    memset(accel_SF_std, 0, 3*sizeof(double));
    memset(accel_SF_GM_frqspctrl, 0, 3*sizeof(double));
    memset(accel_SF_GM_corrtime, 0, 3*sizeof(double));
    
    memset(gyro_SF_std, 0, 3*sizeof(double));
    memset(gyro_SF_GM_frqspctrl, 0, 3*sizeof(double));
    memset(gyro_SF_GM_corrtime, 0, 3*sizeof(double));
    
    memset(sumVel, 0, 3*sizeof(double));
    memset(sumAng, 0, 3*sizeof(double));
    
    gravity = 9.8;
    M       = 6378137.0;
    N       = 6378137.0;
    
    T = 1/sample_rate;
    memset(deltaVn, 0, 3*sizeof(double));
    memset(angularInc, 0, 3*sizeof(double));
    memset(velocityInc, 0, 3*sizeof(double));
    memset(qCb2n, 0, 4*sizeof(double));
}

INS::~INS()
{
    P.dealloc();
    G.dealloc();
    R.dealloc();
    H.dealloc();
    Z.dealloc();
    X.dealloc();
}

/*****************************************/
/* Configure the initial settings for INS*/
/*****************************************/
void INS::initSetting()
{
	init_start_time = 50000.0;
    run_start_time  = 50000.0;
    sample_rate     = 50.0;
    
	numofobs = 6;
    
    if (EstimateScaleFactor)
    {
        numofstates = 21;
    }
    else
    {
        numofstates = 15;
    }

	P.alloc(numofstates, numofstates);
	G.alloc(numofstates, numofstates);
	R.alloc(numofobs, numofobs);
	H.alloc(numofobs, numofstates);
	Z.alloc(numofobs, 1);
	X.alloc(numofstates, 1);

    // Kalman filter settings
    P.matrix[0][0] = pos_std[0]*pos_std[0];
    P.matrix[1][1] = pos_std[1]*pos_std[1];
    P.matrix[2][2] = pos_std[2]*pos_std[2];
    
    P.matrix[3][3] = vel_std[0]*vel_std[0];
    P.matrix[4][4] = vel_std[1]*vel_std[1];
    P.matrix[5][5] = vel_std[2]*vel_std[2];
    
    P.matrix[6][6] = pitch_std * pitch_std;
    P.matrix[7][7] = roll_std  * roll_std;
    P.matrix[8][8] = azimuth_std * azimuth_std;
    
    if (EstimateScaleFactor)
    {
        P.matrix[15][15] = 1.0;
        P.matrix[16][16] = 1.0;
        P.matrix[17][17] = 1.0;
        P.matrix[18][18] = 1.0;
        P.matrix[19][19] = 1.0;
        P.matrix[20][20] = 1.0;
    }
}

/*****************************************/
/* Coarse Alignment to get initial attitude*/
/*****************************************/
void INS::coarseAlignment(double *INS)
{
    double endCoarseTime = run_start_time;
    
    currentTime = INS[0];
    
    if (currentTime < endCoarseTime && currentTime > init_start_time)
    {
        double ax = INS[1];
        double ay = INS[2];
        double az = INS[3];
        double gx = INS[4];
        double gy = INS[5];
        double gz = INS[6];
        double magx = INS[7];
        double magy = INS[8];
        double magz = INS[9];
        
        velocityInc[0] = ax * T;
        velocityInc[1] = ay * T;
        velocityInc[2] = az * T;
        
        angularInc[0]  = gx * T;
        angularInc[1]  = gy * T;
        angularInc[2]  = gz * T;
        
        sumVel[0]     += velocityInc[0];
        sumVel[1]     += velocityInc[1];
        sumVel[2]     += velocityInc[2];
        
        sumAng[0]     += angularInc[0];
        sumAng[1]     += angularInc[1];
        sumAng[2]     += angularInc[2];
        
        sumAtt[0]     += magx;
        sumAtt[1]     += magy;
        sumAtt[2]     += magz;
    }
    else if (currentTime > endCoarseTime)
    {
        double summingTime = endCoarseTime - init_start_time;
        
        double aveVel[3] = {sumVel[0]/summingTime, sumVel[1]/summingTime, sumVel[2]/summingTime};
        double aveAng[3] = {sumAng[0]/summingTime, sumAng[1]/summingTime, sumAng[2]/summingTime};
        
        roll    = - asin(aveVel[0]/gravity);
        pitch   =   asin(aveVel[1]/gravity);
        azimuth =   sumAtt[2]/summingTime;
        
        Matrix *R1_pitch  = R1(-pitch);
        Matrix *R2_roll	 = R2(-roll);
        Matrix *R3_azi	 = R3(azimuth);
        
        Matrix temp;
        temp.alloc(3,3);
        Matrix::multiply(*R1_pitch, 3, 3, *R2_roll, 3, 3, temp, AB);
        Matrix::multiply(*R3_azi,   3, 3, temp,    3, 3, dcmCb2n, AB);
        
        dcm2quat();
        quatnormalize(qCb2n);
        
        delete R1_pitch;
        delete R2_roll;
        delete R3_azi;
        temp.dealloc();
    }
}

/*****************************************/
/* INS mechanization to integrate raw measurement*/
/*****************************************/
void INS::insMechnization(const double *INS_pre, const double *INS)
{
    currentTime = INS[0];

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

	Matrix dcmCn2b;
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
    
    quat2dcm(qCb2n);
    
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

/*****************************************/
/* INS Propagation to update KF          */
/*****************************************/
void INS::insPropagation()
{
	double we = 0.00007292115;
	double gN[3] = {0.0, 0.0, gravity};
    
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
    F.matrix[5][6] = fn;
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
    G.matrix[3][3] = dcmCb2n.matrix[0][0] * accel_bias_std[0];
    G.matrix[3][4] = dcmCb2n.matrix[0][1] * accel_bias_std[1];
    G.matrix[3][5] = dcmCb2n.matrix[0][2] * accel_bias_std[2];
    
    G.matrix[4][3] = dcmCb2n.matrix[1][0] * accel_bias_std[0];
    G.matrix[4][4] = dcmCb2n.matrix[1][1] * accel_bias_std[1];
    G.matrix[4][5] = dcmCb2n.matrix[1][2] * accel_bias_std[2];
    
    G.matrix[5][5] = dcmCb2n.matrix[2][0] * accel_bias_std[0];
    G.matrix[5][6] = dcmCb2n.matrix[2][1] * accel_bias_std[1];
    G.matrix[5][7] = dcmCb2n.matrix[2][2] * accel_bias_std[2];
    
    G.matrix[6][6] = dcmCb2n.matrix[0][0] * gyro_bias_std[0] *M_PI/180.0/3600.0;
    G.matrix[6][7] = dcmCb2n.matrix[0][1] * gyro_bias_std[1] *M_PI/180.0/3600.0;
    G.matrix[6][8] = dcmCb2n.matrix[0][2] * gyro_bias_std[2] *M_PI/180.0/3600.0;
    
    G.matrix[7][6] = dcmCb2n.matrix[1][0] * gyro_bias_std[0] *M_PI/180.0/3600.0;
    G.matrix[7][7] = dcmCb2n.matrix[1][1] * gyro_bias_std[1] *M_PI/180.0/3600.0;
    G.matrix[7][8] = dcmCb2n.matrix[1][2] * gyro_bias_std[2] *M_PI/180.0/3600.0;
    
    G.matrix[8][6] = dcmCb2n.matrix[2][0] * gyro_bias_std[0] *M_PI/180.0/3600.0;
    G.matrix[8][7] = dcmCb2n.matrix[2][1] * gyro_bias_std[1] *M_PI/180.0/3600.0;
    G.matrix[8][8] = dcmCb2n.matrix[2][2] * gyro_bias_std[2] *M_PI/180.0/3600.0;
    
    G.matrix[9][9]   = accel_bias_GM_frqspctrl[0] * sqrt(2.0/accel_bias_GM_corrtime[0]);
    G.matrix[10][10] = accel_bias_GM_frqspctrl[1] * sqrt(2.0/accel_bias_GM_corrtime[1]);
    G.matrix[11][11] = accel_bias_GM_frqspctrl[2] * sqrt(2.0/accel_bias_GM_corrtime[2]);
    
    G.matrix[12][12] = gyro_bias_GM_frqspctrl[0] * sqrt(2.0/gyro_bias_GM_corrtime[0]);
    G.matrix[13][13] = gyro_bias_GM_frqspctrl[1] * sqrt(2.0/gyro_bias_GM_corrtime[0]);
    G.matrix[14][14] = gyro_bias_GM_frqspctrl[2] * sqrt(2.0/gyro_bias_GM_corrtime[0]);


	//////////////////////////
    // Kalman Filter
    //////////////////////////
    Matrix eye;
	eye.alloc(numofstates, numofstates);
    Matrix dTrans;
	dTrans.alloc(numofstates, numofstates);
    for (int i = 0; i < numofstates; i++)
    {
        for (int j = 0; j < numofstates; j++)
        {
            dTrans.matrix[i][j] = eye.matrix[i][j] + F.matrix[i][j]*T;
        }
    }
    
    
    // temp1 = dTrans * G
    Matrix temp1;
    temp1.alloc(numofstates, numofstates);
    Matrix::multiply(dTrans, numofstates, numofstates, G, numofstates, numofstates, temp1, AB);
    // temp2 = dTrans * G * G' * dTrans'
    Matrix temp2;
    temp2.alloc(numofstates, numofstates);
    Matrix::multiply(temp1, numofstates, numofstates, temp1, numofstates, numofstates, temp2, ABT);
    // temp3 = G * G';
    Matrix temp3;
    temp3.alloc(numofstates, numofstates);
    Matrix::multiply(G, numofstates, numofstates, G, numofstates, numofstates, temp3, ABT);
    //temp4 = dTrans * G * G' * dTrans' + G * G'
    Matrix temp4;
    temp4.alloc(numofstates, numofstates);
    Matrix::add(temp2, temp3, numofstates, numofstates, temp4, AB);
    // Qk = (dTrans * G * G' * dTrans' + G * G') * (T/2)
    Matrix Qk;
    Qk.alloc(numofstates, numofstates);
    Qk.scale(T/2);
    
    // temp5 = P * dTrans'
    Matrix temp5;
    temp5.alloc(numofstates, numofstates);
    Matrix::multiply(P, numofstates, numofstates, dTrans, numofstates, numofstates, temp5, ABT);
    // temp6 = dTrans * P * dTrans'
    Matrix temp6;
    temp6.alloc(numofstates, numofstates);
    Matrix::multiply(dTrans, numofstates, numofstates, temp5, numofstates, numofstates, temp6, AB);
    // P = dTrans * P * dTrans' + Qk
    Matrix::add(temp6, Qk, numofstates, numofstates, P, AB);
    
    temp1.dealloc();
    temp2.dealloc();
    temp3.dealloc();
    temp4.dealloc();
    temp5.dealloc();
    temp6.dealloc();
}

/*****************************************/
/* Integration to update KF and estimation with GPS*/
/*****************************************/
void INS::integration(const double *GPS)
{
    
    double we = 0.00007292115;
	double gN[3] = {0.0, 0.0, gravity};
    
    currentTime = GPS[0];
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
    
    double gpsPos[3]     = {GPS[1],  GPS[2],  GPS[3]};
    double gpsPos_std[3] = {GPS[4],  GPS[5],  GPS[6]};
    double gpsVel[3]     = {GPS[7],  GPS[8],  GPS[9]};
    double gpsVel_std[3] = {GPS[10], GPS[11], GPS[12]};
    
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
    F.matrix[5][6] = fn;
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
    G.matrix[3][3] = dcmCb2n.matrix[0][0] * accel_bias_std[0];
    G.matrix[3][4] = dcmCb2n.matrix[0][1] * accel_bias_std[1];
    G.matrix[3][5] = dcmCb2n.matrix[0][2] * accel_bias_std[2];
    
    G.matrix[4][3] = dcmCb2n.matrix[1][0] * accel_bias_std[0];
    G.matrix[4][4] = dcmCb2n.matrix[1][1] * accel_bias_std[1];
    G.matrix[4][5] = dcmCb2n.matrix[1][2] * accel_bias_std[2];
    
    G.matrix[5][5] = dcmCb2n.matrix[2][0] * accel_bias_std[0];
    G.matrix[5][6] = dcmCb2n.matrix[2][1] * accel_bias_std[1];
    G.matrix[5][7] = dcmCb2n.matrix[2][2] * accel_bias_std[2];
    
    G.matrix[6][6] = dcmCb2n.matrix[0][0] * gyro_bias_std[0] *M_PI/180.0/3600.0;
    G.matrix[6][7] = dcmCb2n.matrix[0][1] * gyro_bias_std[1] *M_PI/180.0/3600.0;
    G.matrix[6][8] = dcmCb2n.matrix[0][2] * gyro_bias_std[2] *M_PI/180.0/3600.0;
    
    G.matrix[7][6] = dcmCb2n.matrix[1][0] * gyro_bias_std[0] *M_PI/180.0/3600.0;
    G.matrix[7][7] = dcmCb2n.matrix[1][1] * gyro_bias_std[1] *M_PI/180.0/3600.0;
    G.matrix[7][8] = dcmCb2n.matrix[1][2] * gyro_bias_std[2] *M_PI/180.0/3600.0;
    
    G.matrix[8][6] = dcmCb2n.matrix[2][0] * gyro_bias_std[0] *M_PI/180.0/3600.0;
    G.matrix[8][7] = dcmCb2n.matrix[2][1] * gyro_bias_std[1] *M_PI/180.0/3600.0;
    G.matrix[8][8] = dcmCb2n.matrix[2][2] * gyro_bias_std[2] *M_PI/180.0/3600.0;
    
    G.matrix[9][9]   = accel_bias_GM_frqspctrl[0] * sqrt(2.0/accel_bias_GM_corrtime[0]);
    G.matrix[10][10] = accel_bias_GM_frqspctrl[1] * sqrt(2.0/accel_bias_GM_corrtime[1]);
    G.matrix[11][11] = accel_bias_GM_frqspctrl[2] * sqrt(2.0/accel_bias_GM_corrtime[2]);
    
    G.matrix[12][12] = gyro_bias_GM_frqspctrl[0] * sqrt(2.0/gyro_bias_GM_corrtime[0]);
    G.matrix[13][13] = gyro_bias_GM_frqspctrl[1] * sqrt(2.0/gyro_bias_GM_corrtime[0]);
    G.matrix[14][14] = gyro_bias_GM_frqspctrl[2] * sqrt(2.0/gyro_bias_GM_corrtime[0]);
    
    /****************************/
    /*   Misclosure             */
    /****************************/
    double insPos[3] = {0.0};
    insPos[0] = pos[0] + latDot*T;
    insPos[1] = pos[1] + lonDot*T;
    insPos[2] = pos[2] + Vu*T;
    
    Z.matrix[0][0] = insPos[0] - gpsPos[0];
    Z.matrix[1][0] = insPos[1] - gpsPos[1];
    Z.matrix[2][0] = insPos[2] - gpsPos[2];
    
    /************************/
    /* Build design matrix  */
    /************************/
    H.matrix[0][0] = 1;
    H.matrix[1][1] = 1;
    H.matrix[2][2] = 1;
    
    /************************************/
    /* Build observation weight matrix  */
    /************************************/
    R.matrix[0][0] = gpsPos_std[0];
    R.matrix[1][1] = gpsPos_std[1];
    R.matrix[2][2] = gpsPos_std[2];
    
    /**********************/
    /* Kalman Filter      */
    /**********************/
	Matrix eye;
	eye.alloc(numofstates, numofstates);
    Matrix dTrans;
	dTrans.alloc(numofstates, numofstates);
    for (int i = 0; i < numofstates; i++)
    {
        for (int j = 0; j < numofstates; j++)
        {
            dTrans.matrix[i][j] = eye.matrix[i][j] + F.matrix[i][j]*T;
        }
    }

    // temp1 = dTrans * G
    Matrix temp1;
    temp1.alloc(numofstates, numofstates);
    Matrix::multiply(dTrans, numofstates, numofstates, G, numofstates, numofstates, temp1, AB);
    // temp2 = dTrans * G * G' * dTrans'
    Matrix temp2;
    temp2.alloc(numofstates, numofstates);
    Matrix::multiply(temp1, numofstates, numofstates, temp1, numofstates, numofstates, temp2, ABT);
    // temp3 = G * G';
    Matrix temp3;
    temp3.alloc(numofstates, numofstates);
    Matrix::multiply(G, numofstates, numofstates, G, numofstates, numofstates, temp3, ABT);
    //temp4 = dTrans * G * G' * dTrans' + G * G'
    Matrix temp4;
    temp4.alloc(numofstates, numofstates);
    Matrix::add(temp2, temp3, numofstates, numofstates, temp4, AB);
    // Qk = (dTrans * G * G' * dTrans' + G * G') * (T/2)
    Matrix Qk;
    Qk.alloc(numofstates, numofstates);
    Qk.scale(T/2);
    
    // temp5 = P * dTrans'
    Matrix temp5;
    temp5.alloc(numofstates, numofstates);
    Matrix::multiply(P, numofstates, numofstates, dTrans, numofstates, numofstates, temp5, ABT);
    // temp6 = dTrans * P * dTrans'
    Matrix temp6;
    temp6.alloc(numofstates, numofstates);
    Matrix::multiply(dTrans, numofstates, numofstates, temp5, numofstates, numofstates, temp6, AB);
    // P = dTrans * P * dTrans' + Qk
    Matrix::add(temp6, Qk, numofstates, numofstates, P, AB);
    
    // temp7 = P * H'
    Matrix temp7;
    temp7.alloc(numofstates, numofobs);
    Matrix::multiply(P, numofstates, numofstates, H, numofobs, numofstates, temp7, ABT);
    // temp8 = H * P * H'
    Matrix temp8;
    temp8.alloc(numofobs, numofobs);
    Matrix::multiply(H, numofobs, numofstates, temp7, numofstates, numofobs, temp8, AB);
    // temp9 = inv(H * P * H' + R)
    Matrix temp9;
    temp9.alloc(numofobs, numofobs);
    Matrix::add(temp8, R, numofobs, numofobs, temp9, AB);
    temp9.invert(numofobs);
    // K = P * H' * inv(H * P * H' + R)
    Matrix K;
    K.alloc(numofstates, numofobs);
    Matrix::multiply(temp7, numofstates, numofobs, temp9, numofobs, numofobs, K, AB);
    // X = K * Z
    Matrix::multiply(K, numofstates, numofobs, Z, numofobs, 1, X, AB);
    // temp10 = K * H
    Matrix temp10;
    temp10.alloc(numofstates, numofstates);
    Matrix::multiply(K, numofstates, numofobs, H, numofobs, numofstates, temp10, AB);
    // temp11 = - K * H * P
    Matrix temp11;
    temp11.alloc(numofstates, numofstates);
    Matrix::multiply(temp10, numofstates, numofstates, P, numofstates, numofstates, temp11, AB);
    temp11.scale(-1.0);
    
    Matrix::add(P, temp11, numofstates, numofstates, P, AB);
    
    
    temp1.dealloc();
    temp2.dealloc();
    temp3.dealloc();
    temp4.dealloc();
    temp5.dealloc();
    temp6.dealloc();
    temp7.dealloc();
    temp8.dealloc();
    temp9.dealloc();
    temp10.dealloc();
    temp11.dealloc();

}


/*****************************************/
/* Calculate local gravity               */
/*****************************************/
void INS::CalGravity()
{
    double a1 = 9.7803267715;
    double a2 = 0.0052790414;
    double a3 = 0.0000232718;
    double a4 = -0.0000030876910891;
    double a5 = 0.0000000043977311;
    double a6 = 0.0000000000007211;
    
    gravity = a1 * (1 + a2 * sin(pos[0])*sin(pos[0]) + a3 * sin(pos[0])*sin(pos[0])*sin(pos[0])*sin(pos[0])) + (a4 + a5 * sin(pos[0])*sin(pos[0])) * pos[2] + a6 * pos[2] * pos[2];
}


/*****************************************/
/* Calculate local earth radius, M and N */
/*****************************************/
void INS::CalMN()
{
    double a  = 6378137.0;
    double e2 = 0.00669437999019758;
    double W  = sqrt(1.0 - e2 * sin(pos[0]) * sin(pos[0]));
    N = a / W;
    M = (a * (1.0 - e2)) /W /W /W;
}

/*****************************************/
/* Normalize quaternion                  */
/*****************************************/
void INS::quatnormalize(double *q)
{
    double err = q[0]*q[0] + q[1]*q[1] + q[2]*q[2] + q[3]*q[3];
    
    if (err < 1.0000001 && err > 0.999999)
    {
    
    }
    else
    {
        qCb2n[0] = (1.5 - err/2) * q[0];
        qCb2n[1] = (1.5 - err/2) * q[1];
        qCb2n[2] = (1.5 - err/2) * q[2];
        qCb2n[3] = (1.5 - err/2) * q[3];
    }
}

/*****************************************/
/* Calculate dcm from quaternion */
/*****************************************/
void INS::quat2dcm(double *q)
{
    dcmCb2n.matrix[0][0] = q[0]*q[0] + q[1]*q[1] - q[2]*q[2] - q[3]*q[3];
    dcmCb2n.matrix[0][1] = 2 * (q[1]*q[2] - q[0]*q[3]);
    dcmCb2n.matrix[0][2] = 2 * (q[1]*q[3] + q[0]*q[2]);
    
    dcmCb2n.matrix[1][0] = 2 * (q[1]*q[2] + q[0]*q[3]);
    dcmCb2n.matrix[1][1] = q[0]*q[0] - q[1]*q[1] + q[2]*q[2] - q[3]*q[3];
    dcmCb2n.matrix[1][2] = 2 * (q[2]*q[3] - q[0]*q[1]);
    
    dcmCb2n.matrix[2][0] = 2 * (q[1]*q[3] - q[0]*q[2]);
    dcmCb2n.matrix[2][1] = 2 * (q[0]*q[1] + q[2]*q[3]);
    dcmCb2n.matrix[2][2] = q[0]*q[0] - q[1]*q[1] - q[2]*q[2] + q[3]*q[3];
}

/***********************************/
/* Calculate quaternion from dcm   */
/***********************************/
void INS::dcm2quat()
{
    double trace = dcmCb2n.matrix[0][0] + dcmCb2n.matrix[1][1] + dcmCb2n.matrix[2][2];
    double Pa    = 1 + trace;
    double Pb    = 1 + 2 * dcmCb2n.matrix[0][0] - trace;
    double Pc    = 1 + 2 * dcmCb2n.matrix[1][1] - trace;
    double Pd    = 1 + 2 * dcmCb2n.matrix[2][2] - trace;
    
    double a, b, c, d;
    
    if (Pa > Pb && Pa > Pc && Pa > Pd)
    {
        a = 0.5 * sqrt(Pa);
        b = (dcmCb2n.matrix[2][1] - dcmCb2n.matrix[1][2]) / (4*a);
        c = (dcmCb2n.matrix[0][2] - dcmCb2n.matrix[2][0]) / (4*a);
        d = (dcmCb2n.matrix[1][0] - dcmCb2n.matrix[0][1]) / (4*a);
    }
    else if (Pb > Pa && Pb > Pc && Pb > Pd)
    {
        b = 0.5 * sqrt(Pb);
        c = (dcmCb2n.matrix[1][0] - dcmCb2n.matrix[0][1]) / (4*b);
        d = (dcmCb2n.matrix[0][2] - dcmCb2n.matrix[2][0]) / (4*b);
        a = (dcmCb2n.matrix[2][1] - dcmCb2n.matrix[1][2]) / (4*b);
    }
    else if (Pc > Pa && Pc > Pb && Pc > Pd)
    {
        c = 0.5 * sqrt(Pc);
        d = (dcmCb2n.matrix[2][1] - dcmCb2n.matrix[1][2]) / (4*c);
        a = (dcmCb2n.matrix[0][2] - dcmCb2n.matrix[2][0]) / (4*c);
        b = (dcmCb2n.matrix[1][0] - dcmCb2n.matrix[0][1]) / (4*c);
    }
    else if (Pd > Pa && Pd > Pb && Pd > Pc)
    {
        d = 0.5 * sqrt(Pd);
        a = (dcmCb2n.matrix[1][0] - dcmCb2n.matrix[0][1]) / (4*d);
        b = (dcmCb2n.matrix[0][2] - dcmCb2n.matrix[2][0]) / (4*d);
        c = (dcmCb2n.matrix[2][1] - dcmCb2n.matrix[1][2]) / (4*d);
    }
    else
    {
    
    }
    
    if (a < 0)
    {
        a = -a;
        b = -b;
        c = -c;
        d = -d;
    }
    
    qCb2n[0] = a; qCb2n[1] = b; qCb2n[2] = c; qCb2n[3] = d;
}

Matrix * INS::R1(double p)
{
    Matrix *R = new Matrix(3, 3);
    
    R->matrix[0][0] = 1.0;
    R->matrix[1][1] = 1.0;
    R->matrix[2][2] = 1.0;
    
    R->matrix[1][1] = cos(p);
    R->matrix[1][2] = sin(p);
    
    R->matrix[2][2] = cos(p);
    R->matrix[2][1] = -sin(p);
    
    return R;
}

Matrix * INS::R2(double r)
{

    Matrix *R = new Matrix(3, 3);
    
    R->matrix[0][0] = 1.0;
    R->matrix[1][1] = 1.0;
    R->matrix[2][2] = 1.0;
    
    R->matrix[0][0] = cos(r);
    R->matrix[0][2] = -sin(r);
    
    R->matrix[2][2] = cos(r);
    R->matrix[2][0] = sin(r);
    
    return R;
}

Matrix * INS::R3(double a)
{
    
    Matrix *R = new Matrix(3, 3);
    
    R->matrix[0][0] = 1.0;
    R->matrix[1][1] = 1.0;
    R->matrix[2][2] = 1.0;
    
    R->matrix[0][0] = cos(a);
    R->matrix[0][1] = sin(a);
    
    R->matrix[1][1] = cos(a);
    R->matrix[1][0] = -sin(a);
    
    return R;
}
