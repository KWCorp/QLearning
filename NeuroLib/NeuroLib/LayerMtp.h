#pragma once
#include "framework.h"
class LayerMtp :
    public INeuro
{
public:
	float*& GetIn();

	float*& GetOut();

	float*& GetInErr();

	float*& GetOutErr();

	int GetInSize();

	int GetOutSize();

	void SetFunc(float (*_F)(float), float (*_DF)(float));

	void SetFunc1(float (*_F)(float), float (*_DF)(float));

	void SetFunc2(float (*_F)(float), float (*_DF)(float));

	void Calc();

	void Train();

	float* in, * out, * inerr, * outerr, * outbase1, * outbase2;

	float* links1, *links2, LF = 0.01;

	int ins, outs;

	float (*F1)(float);
	float (*DF1)(float);

	float (*F2)(float);
	float (*DF2)(float);

	LayerMtp(int _ins, int _outs, float* _in = 0, float* _out = 0, float* _ine = 0, float* _oute = 0);
};

