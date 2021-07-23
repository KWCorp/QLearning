#pragma once
#include "framework.h"
class Layer :
	public INeuro
{
public:
	float*& GetIn();

	float*& GetOut();

	float*& GetInErr();

	float*& GetOutErr();

	int GetInSize();

	int GetOutSize();

	void SetFunc(float (*_F)(float) , float (*_DF)(float));

	void Calc();

	void Train();

	float* in, *out, *inerr, *outerr, *outbase;

	float* links, LF = 0.01;

	int ins, outs;

	float (*F)(float);
	float (*DF)(float);

	Layer(int _ins, int _outs, float* _in = 0, float* _out = 0, float* _ine = 0, float* _oute = 0);
};

