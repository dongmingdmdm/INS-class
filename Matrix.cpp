/*
 
  Matrix.cpp
 
  This program is the copyright of Dr. Yang Gao’s Research Group 
  "Positioning and Mobile Information Systems (PMIS)" in the 
  Department of Geomatics Engineering at The University of Calgary. 
 
  Any reproduction of this program, or any part of this program without 
  the express permission of Dr. Yang Gao is forbidden.
 
  Copyright @ September 2003
 
*/


#include "StdAfx.h"
#include "Matrix.h"
#include <iomanip.h>

#ifdef _DEBUG
#undef THIS_FILE
char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


/*-------------------------------------------------------------------------------
Name:			Matrix

Purpose:		Matrix class constructor.

				Creates a NULL matrix with zero rows and columns.
				
History:		Name				Date		Reason for modification
                PMIS                2003        Original
------------------------------------------------------------------------------- */
Matrix::Matrix()
{
	matrix = NULL;
	nrows = 0;
	ncols = 0;

	return;
}


/*-------------------------------------------------------------------------------
Name:			Matrix

Purpose:		Matrix class constructor. 
				
				Calls the function 'alloc' which allocates space for a 
				'rows' by 'cols' double matrix.

Arguments:		int rows - number of rows (must be > 0)
				int cols - number of columns (must be > 0)

History:		Name				Date		Reason for modification
                PMIS                2003        Original
------------------------------------------------------------------------------- */
Matrix::Matrix(int rows, int cols)
{
	alloc(rows, cols);
}


/*-------------------------------------------------------------------------------
Name:			alloc

Purpose:		To allocate space for a 'rows' by 'cols' double matrix.

Arguments:		int rows - number of rows (must be > 0)
				int cols - number of columns (must be > 0)
				
History:		Name				Date		Reason for modification
                PMIS                2003        Original
------------------------------------------------------------------------------- */
Matrix::alloc(int rows, int cols)
{
	int i, j;
	
	/* checking 'rows' and 'cols' values (must be positive non-zero) */
	if(rows < 0 || cols < 0)
	{
		AfxMessageBox("Cannot have negative number of rows and/or columns!");
		exit(1);
	}
	
	/* 'matrix' is a pointer to pointer to double */
	matrix = new double* [rows];
	
	/* 'matrix' will be NULL if insufficient memory exists for allocation */
	if(matrix == NULL)
	{
		AfxMessageBox("Insufficient memory for matrix allocation!");
		exit(1);
	}
	
	for(i = 0; i < rows; i++)
	{
		matrix[i] = new double [cols];
		
		/* 'matrix' will be NULL if insufficient memory exists for allocation */
		if(matrix[i] == NULL)
		{
			AfxMessageBox("Insufficient memory for matrix allocation!");
			exit(1);
		}
	}
	
	/* setting number of rows and columns */
	nrows = rows;
	ncols = cols;
	
	/* setting all elements to zero */
	for(i = 0; i < nrows; i++)
		for(j = 0; j < ncols; j++)
			matrix[i][j] = 0.0;
}


/*-------------------------------------------------------------------------------
Name:			~Matrix

Purpose:		Matrix class destructor.

History:		Name				Date		Reason for modification
                PMIS                2003        Original
------------------------------------------------------------------------------- */
Matrix::~Matrix()
{
}


/*-------------------------------------------------------------------------------
Name:			copy

Purpose:		To copy a source matrix to a destination matrix.

Arguments:		Matrix source - source matrix that will be copied to the matrix
				                that calls this function.
				int nr - the number of rows (starting with the first) to copy.
				int nc - the number of columns (starting with the first) to copy.

				Note that 'nr' and 'nc' have to be smaller than or equal to the
				number of rows and columns, respectively, that the source and 
				destination matrices consist of. In other words, you can copy
				a part of the source matrix to the destination matrix.

History:		Name				Date		Reason for modification
                PMIS                2003        Original
------------------------------------------------------------------------------- */
void Matrix::Matrix_copy(Matrix source, int nr, int nc)
{
	int i, j;

	/* checking to see if the source matrix has valid row and column values and if
	   the specified copying boundaries positive */
	if(!source.IsValid() || nr < 0 || nc < 0)
	{
		AfxMessageBox("Cannot have negative number of rows and/or columns!");
		exit(1);
	}

	/* checking to see if the specified copying boundaries are within the boundaries
	   of the source and destination matrices */
	if(nr > nrows || nc > ncols || nr > source.nrows || nc > source.ncols)
	{
		AfxMessageBox("Copying boundaries are beyond matrix size!");
		exit(1);
	}

	/* nothing is done if the source matrix is null */
	if(source.IsNull())
		return;
	/* passing source information to destination matrix */
	else
	{
		for(i = 0; i < nr; i++)
			for(j = 0; j < nc; j++)
				matrix[i][j] = source.matrix[i][j];
	}
}


/*-------------------------------------------------------------------------------
Name:			dealloc

Purpose:		Releases memory allocated to a double matrix.

History:		Name				Date		Reason for modification
                PMIS                2003        Original
------------------------------------------------------------------------------- */
void Matrix::dealloc()
{
	int i;

	if(!IsValid())
	{
		AfxMessageBox("Cannot have negative number of rows and/or columns!");
		exit(1);
	}

	if(!IsNull())
	{
		for(i = 0; i < nrows; i++)
			delete[] matrix[i];
		
		delete[] matrix;
		
		matrix = NULL;
		
		nrows = 0;
		ncols = 0;
	}
}


/*-------------------------------------------------------------------------------
Name:			clear

Purpose:		Sets all elements in a double matrix to zero.

				When a matrix is cleared, only the cleared matrix is maintained. The
				original matrix before the 'clear' operation no longer exists.

History:		Name				Date		Reason for modification
                PMIS                2003        Original
------------------------------------------------------------------------------- */
void Matrix::clear()
{
	int i, j;

	if(!IsValid())
	{
		AfxMessageBox("Cannot have negative number of rows and/or columns!");
		exit(1);
	}

	if(!IsNull())
	{
		for(i = 0; i < nrows; i++)
			for(j = 0; j < ncols; j++)
				matrix[i][j] = 0.0;
	}
}


/*-------------------------------------------------------------------------------
Name:			multiply

Purpose:		To multiply two matrices of type double.

Arguments:		Matrix A - first input matrix of type double
				int ar - row subset for Matrix A
				int ac - column subset for Matrix A
				Matrix B - second input matrix of type double
				int br - row subset for Matrix B
				int bc - column subset for Matrix B
				Matrix output - output matrix of type double
				int mode - multiplication mode

				Note that 'ar' and 'ac' must be smaller than or equal to the number
				of rows and columns, respectively, that matrix A is composed of.
				Similarly, 'br' and 'bc' must be smaller than or equal to the number
				of rows and columns, respectively, that matrix B is composed of. These
				dimensions apply to the matrices before any tranpose operation is
				applied. For example:

				Matrix::multiply(A, 2, 4, B, 2, 5, C, ATA);

					A		B              AT
				****ooo  *****oo	      **o
				****ooo  *****oo          **o
				ooooooo  ooooooo          **o
				         ooooooo          **o
						 ooooooo          ooo
						                  ooo
						                  ooo

				From this example, it can be seen that 'ar', 'ac', 'br' and 'bc' do
				not have to be equivalent to the number of rows and columns that
				the matrices consist of. Also, the values of 'ar' and 'ac' above were
				chosen before the matrix is transposed internally within the function.

History:		Name				Date		Reason for modification
                PMIS                2003        Original
------------------------------------------------------------------------------- */
void Matrix::multiply(Matrix A, int ar, int ac, Matrix B, int br, int bc, Matrix output, int mode)
{
	int i, j, k;

	if(!A.IsValid() || !B.IsValid() || ar < 0 || ac < 0 || br < 0 || bc < 0)
	{
		AfxMessageBox("Cannot have negative number of rows and/or columns!");		
		exit(1);
	}

	if(A.IsNull() || B.IsNull())
	{
		AfxMessageBox("Cannot have a NULL matrix for multiplication!");
		exit(1);
	}

	if(ar > A.nrows || ac > A.ncols || br > B.nrows || bc > B.ncols)
	{
		AfxMessageBox("Multiplication boundaries are beyond matrix size!");
		exit(1);
	}
	
	/* condition when matrices are not compatible for multiplication */
	if(mode == 0 && ac != br
		|| mode == 1 && ar != br
		|| mode == 2 && ac != bc)
	{
		AfxMessageBox("Matrices are not compatible for multiplication!");
		exit(1);
	}

	/* checking if output matrix is of sufficient size to store the multiplication result */
	if(mode == 0 && (ar > output.nrows || bc > output.ncols)
		|| mode == 1 && (ac > output.nrows || bc > output.ncols)
		|| mode == 2 && (ar > output.nrows || br > output.ncols))
	{
		AfxMessageBox("Output matrix not large enough to hold multiplication result!");
		exit(1);
	}

	/* clearing output matrix */
	output.clear();

	/* performing multiplication based on the specified mode */
	if(mode == 0) /* Output.matrix = A*B */
	{
		for(i = 0; i < ar; i++)
			for(j = 0; j < bc; j++)
				for(k = 0; k < ac; k++)
					output.matrix[i][j] += A.matrix[i][k] * B.matrix[k][j];
	}
	else if(mode == 1)  /* Output.matrix = A' * B */
	{
		for(i = 0; i < ac; i++)
			for(j = 0; j < bc; j++)
				for(k = 0; k < ar; k++)
					output.matrix[i][j] += A.matrix[k][i] * B.matrix[k][j];
	}
	else if(mode == 2)  /* Output.matrix = A * B' */
	{
		for(i = 0; i < ar; i++)
			for(j = 0; j < br; j++)
				for(k = 0; k < ac; k++)
					output.matrix[i][j] += A.matrix[i][k] * B.matrix[j][k];
	}
	else
	{
		AfxMessageBox("Invalid multiplication mode!");
		exit(1);
	}
}


/*-------------------------------------------------------------------------------
Name:			add

Purpose:		To add two matrices of type double.

Arguments:		Matrix A - first input matrix of type double
				Matrix B - second input matrix of type double
				int nr - row subset
				int nc - column subset
				Matrix output - output matrix of type double
				int mode - addition mode

				Note that 'nr' and 'nc' must be smaller than or equal to the number
				of rows and columns, respectively, that matrix A and B is composed of.
				These dimensions apply to the matrices before any tranpose operation is
				applied. For example:

				Matrix::add(A, 2, 4, B, 4, 2, C, AAT);

					A		B              BT
				****ooo  **ooooo	      ****o
				****ooo  **ooooo          ****o
				ooooooo  **ooooo          ooooo
				         **ooooo          ooooo
						 ooooooo          ooooo
						                  ooooo
						                  ooooo

				From this example, it can be seen that 'nr' and 'nc' do
				not have to be equivalent to the number of rows and columns that
				the matrices consist of. Also, the values of 'nr' and 'nc' above were
				chosen before the matrix is transposed internally within the function.

History:		Name				Date		Reason for modification
                PMIS                2003        Original
------------------------------------------------------------------------------- */
void Matrix::add(Matrix A, Matrix B, int nr, int nc, Matrix output, int mode)
{
	int i, j;

	if(!A.IsValid() || !B.IsValid() || nr < 0 || nc < 0)
	{
		AfxMessageBox("Cannot have negative number of rows and/or columns!");		
		exit(1);
	}

	if(A.IsNull() || B.IsNull())
	{
		AfxMessageBox("Cannot have a NULL matrix for addition!");
		exit(1);
	}

	if(nr > A.nrows || nc > A.ncols || nr > B.nrows || nc > B.ncols)
	{
		AfxMessageBox("Addition boundaries are beyond matrix size!");
		exit(1);
	}

	/* checking if output matrix is of sufficient size to store the addition result */
	if(nr > output.nrows || nc > output.ncols)
	{
		AfxMessageBox("Output matrix not large enough to hold addition result!");
		exit(1);
	}

	/* performing addition based on the specified mode */
	if(mode == 0) /* Output.matrix = A + B */
	{
		for(i = 0; i < nr; i++)
			for(j = 0; j < nc; j++)
				output.matrix[i][j] = A.matrix[i][j] + B.matrix[i][j];
	}
	else if(mode == 1)  /* Output.matrix = A' + B */
	{
		for(i = 0; i < nr; i++)
			for(j = 0; j < nc; j++)
				output.matrix[i][j] = A.matrix[j][i] + B.matrix[i][j];
	}
	else if(mode == 2)   /* Output.matrix = A + B' */
	{
		for(i = 0; i < nr; i++)
			for(j = 0; j < nc; j++)
				output.matrix[i][j] = A.matrix[i][j] + B.matrix[j][i];
	}
	else
	{
		AfxMessageBox("Invalid addition mode!");
		exit(1);
	}
}


/*-------------------------------------------------------------------------------
Name:			scale

Purpose:		To scale a matrix of type double.

Arguments:		double num - value that matrix will be scaled by

				When a matrix is scaled, only the scaled matrix is maintained. The
				original matrix before the 'scale' operation no longer exists.

History:		Name				Date		Reason for modification
                PMIS                2003        Original
------------------------------------------------------------------------------- */
void Matrix::scale(double num)
{
	int i, j;

	if(!IsValid())
	{
		AfxMessageBox("Cannot have negative number of rows and/or columns!");
		exit(1);
	}

	if(!IsNull())
	{
		for(i = 0; i < nrows; i++)
			for(j = 0; j < ncols; j++)
				matrix[i][j] *= num;
	}
}


/*-------------------------------------------------------------------------------
Name:			invert

Purpose:		To invert a matrix of type double using Cholesky decomposition.
				The matrix that calls the 'invert' function must be positive definite.

				When a matrix is inverted, only the inverted matrix is maintained. The
				original matrix before the 'invert' operation no longer exists.

				The 'invert' function returns one of the following values:

				0 - successful inversion
				1 - invalid matrix dimension
				2 - matrix not positive definite
				3 - matrix is singular
				4 - square root of a negative number encountered
				5 - division by zero encountered

History:		Name				Date		Reason for modification
                PMIS                2003        Original
------------------------------------------------------------------------------- */
int Matrix::invert(int n)
{
	int i, j, k;
	
	if(!IsValid() || nrows != ncols || n > nrows || n <= 0)
		return 1;
	
	if(!IsNull())
	{		
		/* Check if matrix is positive definite */
		for(i = 0; i < n; i++)
		{
			if(matrix[i][i] < 0.0)
				return 2;
			
			if(fabs(matrix[i][i]) < 1.0e-15)
				return 3;
		}
		
		/* Perform Cholesky decomposition */
		for(j = 0; j < n; j++)
		{
			for(k = 0; k < j; k++)
				matrix[j][j] -= matrix[j][k] * matrix[j][k];
			
			if (matrix[j][j] < 0.0)
				return 4;
			
			matrix[j][j] = sqrt(matrix[j][j]);
			
			for(i = j+1; i < n; i++)
			{
				for(k = 0; k < j; k++)
					matrix[i][j] -= matrix[i][k] * matrix[j][k];
				
				if(fabs(matrix[j][j]) < 1.0e-12)
					return 5;
				
				matrix[i][j] /= matrix[j][j];
			}
		}
		
		/* Inversion of lower triangular matrix */
		for(j = 0; j < n; j++)
		{
			matrix[j][j] = 1.0 / matrix[j][j];
			
			for(i = j+1; i < n; i++)
			{
				matrix[i][j] *= (-1) * matrix[j][j] / matrix[i][i];
				
				for(k = j+1; k < i; k++)
					matrix[i][j] -= matrix[i][k] * matrix[k][j] / matrix[i][i];
			}
		}
		
		/* Construction of lower triangular inverse matrix */
		for(j = 0; j < n; j++)
		{
			for(i = j; i < n; i++)
			{
				matrix[i][j] *= matrix[i][i];
				
				for(k = i+1; k < n; k++)
					matrix[i][j] += matrix[k][i] * matrix[k][j];
			}
		}
		
		/* fill upper diagonal */
		for(i = 1; i < n; i++)
			for(j = 0; j < i; j++)
				matrix[j][i] = matrix[i][j];
	}
	else
		return 1;

	return 0;
}


/*-------------------------------------------------------------------------------
Name:			IsValid

Purpose:		To check if a matrix is valid. A matrix is not valid if its number
				of rows or columns is a negative value.

History:		Name				Date		Reason for modification
                PMIS                2003        Original
------------------------------------------------------------------------------- */
bool Matrix::IsValid()
{
	if(nrows < 0 || ncols < 0)
		return FALSE;
	else
		return TRUE;
}


/*-------------------------------------------------------------------------------
Name:			IsNull

Purpose:		To check if a matrix is null. A matrix is null if the number of
				rows or columns is zero, or if the 'matrix' variable (pointer to
				pointer to double) is null.

History:		Name				Date		Reason for modification
                PMIS                2003        Original
------------------------------------------------------------------------------- */
bool Matrix::IsNull()
{
	/* 'matrix' is not NULL but one of the dimensions is zero */
	if((nrows == 0 || ncols == 0) && matrix != NULL)
	{
		nrows = 0;
		ncols = 0;
		dealloc();
		return TRUE;
	}
	/* 'matrix' is NULL and at least one of its dimensions is zero */
	else if((nrows == 0 || ncols == 0) && matrix == NULL)
	{
		nrows = 0;
		ncols = 0;
		return TRUE;
	}
	/* matrix dimensions are non-zero, but the matrix itself is NULL */
	else if(nrows != 0 && ncols != 0 && matrix == NULL)
	{
		nrows = 0;
		ncols = 0;
		return TRUE;
	}
	else
		return FALSE;
}


/*-------------------------------------------------------------------------------
Name:			print

Purpose:		To print the matrix of type double to an output stream.

Arguments:		ofstream& out - output stream
				int nr - number of rows (starting with the first) to print
				int nc - number of columns (starting with the first) to print
				int w - width between matrix elements
				int p - precision of output elements
				int out_flag - if out_flag = 0, the output will be in fixed format
							   if out_flag = 1, the output will be in scientific format

History:		Name				Date		Reason for modification
                PMIS                2003        Original
------------------------------------------------------------------------------- */
void Matrix::print(ostream& out, int nr, int nc, int w, int p, int out_flag)
{
	int i, j;

	if(!IsValid())
	{
		AfxMessageBox("Cannot have negative number of rows and/or columns!");
		exit(1);
	}

	if(IsNull())
	{
		AfxMessageBox("Cannot print NULL matrix!");
		exit(1);
	}

	if(nr > nrows || nc > ncols)
	{
		AfxMessageBox("Printing boundaries beyond matrix size!");
		exit(1);
	}

	out.setf(ios::showpoint);

	if(out_flag == 0)
		out.setf(ios::fixed);
	else
		out.unsetf(ios::fixed);
	
	out.precision(p);
	
	for(i = 0; i < nr; i++)
	{
		for(j = 0; j < nc; j++)
			out << matrix[i][j] << setw(w);
		
		out << endl;
	}
}
