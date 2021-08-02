#pragma once
#include "framework.h"
class Layer :
	public INeuro
{
public:
	double*& INeuro::GetIn() override;

	double*& INeuro::GetOut() override;

	double*& INeuro::GetInErr()override;

	double*& INeuro::GetOutErr()override;

	int INeuro::GetInSize()override;

	int INeuro::GetOutSize()override;

	void INeuro::SetFunc(double (*_F)(double) , double (*_DF)(double))override;

	void INeuro::Calc()override;

	void INeuro::Train()override;

	void INeuro::ResetErr()override;

	double* in, *out, *inerr, *outerr, *outbase;

	double* links, LF = 0.01;

	int ins, outs;

	double (*F)(double);
	double (*DF)(double);

	Layer(int _ins, int _outs, double* _in = 0, double* _out = 0, double* _ine = 0, double* _oute = 0);
};

