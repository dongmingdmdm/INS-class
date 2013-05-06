/*
 
  Matrix.h
 
  This program is the copyright of Dr. Yang Gao’s Research Group 
  "Positioning and Mobile Information Systems (PMIS)" in the 
  Department of Geomatics Engineering at The University of Calgary. 
 
  Any reproduction of this program, or any part of this program without 
  the express permission of Dr. Yang Gao is forbidden.
 
  Copyright @ September 2003
 
*/


#if !defined(AFX_MATRIX_H__715F8C61_E78E_4049_AE27_109FA7857A7D__INCLUDED_)
#define AFX_MATRIX_H__715F8C61_E78E_4049_AE27_109FA7857A7D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/* modes for multiplication of matrices */
#define AA   0  /* it means matrix A times another matrix A */
#define ATA  1  /* it means the tranposed matrix A times another matrix A */
#define AAT  2  /* it means matrix A times another transposed matrix A */


class Matrix  
{
public:

	Matrix();

	Matrix(int rows, int cols);

	alloc(int rows, int cols);

	virtual ~Matrix();

	void Matrix_copy(Matrix source, int nr, int nc);

	void dealloc();

	void clear();

	static void multiply(Matrix A, int ar, int ac, Matrix B, int br, int bc, Matrix output, int mode);

	static void add(Matrix A, Matrix B, int nr, int nc, Matrix output, int mode);

	void scale(double num);

	int invert(int n);

	bool IsValid();

	bool IsNull();

	void print(ostream& out, int nr, int nc, int w, int p, int out_flag);

public:

	double** matrix;

	int nrows;

	int ncols;
};

#endif // !defined(AFX_MATRIX_H__715F8C61_E78E_4049_AE27_109FA7857A7D__INCLUDED_)
