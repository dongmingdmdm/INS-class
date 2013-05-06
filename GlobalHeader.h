// Bei add this file in March 2013

#ifndef _GLOBALHEADER_H_

#define _GLOBALHEADER_H_

#pragma comment(lib,"Ws2_32.lib")

#include <stdlib.h>
#include <conio.h>
#include <stdio.h>
#include <windows.h>
#include <vector>
#include "RT\RDecode.h"

#ifndef _WINDOWS_
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <process.h>
#include <exception>
#include <wchar.h>
#undef WIN32_LEAN_AND_MEAN
#endif

int   erreur;
 
static bool  PPP_ESTIMATION_READY;
static bool  INIT_ALIGN_READY;

static std::vector<double> xRate;
static std::vector<double> yRate;
static std::vector<double> zRate;

static std::vector<double> xAccel;
static std::vector<double> yAccel;
static std::vector<double> zAccel;

static std::vector<double> Pitch;
static std::vector<double> Roll;
static std::vector<double> Yaw;

static std::vector<double> PPP_phi;
static std::vector<double> PPP_lambda;
static std::vector<double> PPP_h;

static std::vector<double> PPP_sigmaphi;
static std::vector<double> PPP_sigmalambda;
static std::vector<double> PPP_sigmah;

static std::vector<double> PPP_nV;
static std::vector<double> PPP_eV;
static std::vector<double> PPP_dV;

static std::vector<double> PPP_sigmanV;
static std::vector<double> PPP_sigmaeV;
static std::vector<double> PPP_sigmadV;

static std::vector<double> PPP_GPSTime;

static std::vector<double> IMU_phi;
static std::vector<double> IMU_lambda;
static std::vector<double> IMU_h;

static std::vector<double> IMU_nV;
static std::vector<double> IMU_eV;
static std::vector<double> IMU_dV;

static std::vector<double> IMU_GPSTime;

CRDecode m_rDecode;
SOCKET m_NTRIPSocket;
HANDLE h_thread;
DWORD  dwThreadId;
char   RTCAbuff[1024];
int    RTCAdatalen;

ofstream solution_file;

#endif