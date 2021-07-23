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

	void SetFunc(float* (float));

	void Calc();

	void Train();

	float* in, *out, *inerr, *outerr;

	float* links;

	int ins, outs;

	Layer(int _ins, int _outs, float* _in = 0, float* _out = 0);
};

