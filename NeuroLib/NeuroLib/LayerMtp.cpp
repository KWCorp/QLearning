#include "LayerMtp.h"

float F_RELU(float x)
{
	return 0.99 * x * (x > 0) + 0.01 * x;
}

float DF_RELU(float x)
{
	return 0.99 + (x > 0) + 0.01;
}

float F_GATE(float x)
{
	return x + 1 + (1 / (1 - x) - x - 1) * (x < 0);
}

float DF_GATE(float x)
{
	return 1 + (1 / (1 - 2 * x + x * x) - 1) * (x < 0);
}

LayerMtp::LayerMtp(int _ins, int _outs, float* _in, float* _out, float* _ine, float* _oute)
{
	F1 = F_RELU;
	DF1 = DF_RELU;
	F2 = F_GATE;
	DF2 = DF_GATE;
	ins = _ins;
	outs = _outs;
	if (_in == 0)
		in = new float[ins];
	else
		in = _in;

	if (_out == 0)
		out = new float[outs];
	else
		out = _out;

	if (_ine == 0)
		inerr = new float[ins];
	else
		inerr = _ine;

	if (_oute == 0)
		outerr = new float[outs];
	else
		outerr = _oute;

	outbase1 = new float[outs];
	outbase2 = new float[outs];

	links1 = new float[(ins + 1) * outs];
	links2 = new float[(ins + 1) * outs];
}

float*& LayerMtp::GetIn()
{
	return in;
}

float*& LayerMtp::GetOut()
{
	return out;
}

float*& LayerMtp::GetInErr()
{
	return inerr;
}

float*& LayerMtp::GetOutErr()
{
	return outerr;
}

int LayerMtp::GetInSize()
{
	return ins;
}

int LayerMtp::GetOutSize()
{
	return outs;
}

void LayerMtp::SetFunc(float (*_F)(float), float (*_DF)(float))
{
	F1 = _F;
	DF1 = _DF;
	F2 = _F;
	DF2 = _DF;
}

void LayerMtp::SetFunc1(float (*_F)(float), float (*_DF)(float))
{
	F1 = _F;
	DF1 = _DF;
}

void LayerMtp::SetFunc2(float (*_F)(float), float (*_DF)(float))
{
	F2 = _F;
	DF2 = _DF;
}

void LayerMtp::Calc()
{
	for (int o = 0; o < outs; o++)
	{
		outbase1[o] = links1[o];
		outbase2[o] = links2[o];
		for (int i = 0; i < ins; i++)
		{
			outbase1[o] += in[i] * links1[o + (i + 1) * outs];
			outbase2[o] += in[i] * links2[o + (i + 1) * outs];
		}
		out[o] = F1(outbase1[o]) * F2(outbase2[o]);
	}
}

void LayerMtp::Train()
{
	for (int i = 0; i < ins; i++)
	{
		inerr[i] = 0;
		for (int o = 0; o < outs; o++)
			inerr[i] += outerr[o] * (
			links1[o + (i + 1) * outs] * DF1(outbase1[o]) * F2(outbase2[o]) + 
			links2[o + (i + 1) * outs] * DF2(outbase2[o]) * F1(outbase1[o])) ;
	}

	for (int o = 0; o < outs; o++)
	{
		links1[o] += outerr[o] * DF1(outbase1[o]) * F2(outbase2[o]) * LF;
		links2[o] += outerr[o] * DF2(outbase2[o]) * F1(outbase1[o]) * LF;
		for (int i = 0; i < ins; i++)
		{
			links1[o + (i + 1) * outs] += outerr[o] * DF1(outbase1[o]) * F2(outbase2[o]) * in[i] * LF;
			links2[o + (i + 1) * outs] += outerr[o] * DF2(outbase2[o]) * F1(outbase1[o]) * in[i] * LF;
		}
	}

}
