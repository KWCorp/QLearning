#pragma once
#include "framework.h"
class LayerMtp :
    public INeuro
{
public:
	double*& GetIn()override;

	double*& GetOut()override;

	double*& GetInErr()override;

	double*& GetOutErr()override;

	int GetInSize()override;

	int GetOutSize() override;

	void SetFunc(double (*_F)(double), double (*_DF)(double))override;

	void SetFunc1(double (*_F)(double), double (*_DF)(double));

	void SetFunc2(double (*_F)(double), double (*_DF)(double));

	void Calc()override;

	void Train()override;

	double* in, * out, * inerr, * outerr, * outbase1, * outbase2;

	double* links1, *links2, LF = 0.01;

	int ins, outs;

	double (*F1)(double);
	double (*DF1)(double);

	double (*F2)(double);
	double (*DF2)(double);

	LayerMtp(int _ins, int _outs, double* _in = 0, double* _out = 0, double* _ine = 0, double* _oute = 0);
};

