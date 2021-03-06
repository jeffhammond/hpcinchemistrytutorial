#ifndef MATRIX_UTIL_H
#define MATRIX_UTIL_H

typedef long integer;           // C type of Fortran integers

// Prints a matrix[n,m] in a vaguely human-readable form
void print(int n, int m, const double* s);

// Prints a vector[n] in a vaguely human-readable form
void print(int n, const double* s);

// Makes a new copy of a matrix[n,m] .. result will be allocated with new double[n*m]
double* copy(int n, int m, const double* a);



/// Real symmetric generalized diagonalization using dsygv() ... solves F C = S C E

/// n = matrix dimension
///
/// F[n*n] = input matrix ... unchanged on output
///
/// S[n*n] = input matrix ... unchanged on output
///
/// C[n*n] = output matrix ... i'th row of C contains i'th eigenvector
///
/// E[n] = output vector ... E[i] = i'th eigenvalue
///
/// Solution satisfies   sum[k] F[i,k] C[j,k] = sum[k] S[i,k] C[j,k] E[j]
///
void real_sym_gen_diag(int n, const double* F, const double* S, double* C, double* E);


/// Real symmetric diagonalization using dsyev() ... solves A C = C E

/// n = matrix dimension
///
/// A[n*n] = input matrix ... unchanged on output
///
/// C[n*n] = output matrix ... i'th row of C contains i'th eigenvector
///
/// E[n] = output vector ... E[i] = i'th eigenvalue
///
/// Solution satisfies   sum[k] A[i,k] C[j,k] = C[i,j] E[j]
///
void real_sym_diag(int n, const double* A, double* C, double* E);


/// Matrix * matrix transpose 

/// The C[i,j] = sum(k) A[i,k] * B[j,k]
void mxmT(int dimi, int dimj, int dimk, const double* A, const double* B, double* C);


/// Matrix transpose * matrix

/// The C[i,j] = sum(k) A[k,i] * B[k,j]
void mTxm(int dimi, int dimj, int dimk, const double* A, const double* B, double* C);


/// Fills vector with value
void fill(int n, double s, double* v);

/// Computes inner or dot product between two vectors
double dot_product(int n, const double* a, const double* b);

/// Symmetrizes a square matrix
void symmetrize(int n, double* a);

#endif
