#include "LayerMtp.h"

LayerMtp::LayerMtp(int _ins, int _outs, double* _in, double* _out, double* _ine, double* _oute)
{
	F1 = F_SGATE;
	DF1 = DF_SGATE;
	F2 = F_GATE;
	DF2 = DF_GATE;
	ins = _ins;
	outs = _outs;
	if (_in == 0)
		in = new double[ins];
	else
		in = _in;

	if (_out == 0)
		out = new double[outs];
	else
		out = _out;

	if (_ine == 0)
		inerr = new double[ins];
	else
		inerr = _ine;

	if (_oute == 0)
		outerr = new double[outs];
	else
		outerr = _oute;

	outbase1 = new double[outs];
	outbase2 = new double[outs];

	links1 = new double[(ins + 1) * outs];
	links2 = new double[(ins + 1) * outs];
	for (int l = 0; l < (ins + 1) * outs; l++)
	{
		links1[l] = rand() / double(RAND_MAX) * 0.2 - 0.1;
		links2[l] = rand() / double(RAND_MAX) * 0.2 - 0.1;
	}
}

double*& LayerMtp::GetIn()
{
	return in;
}

double*& LayerMtp::GetOut()
{
	return out;
}

double*& LayerMtp::GetInErr()
{
	return inerr;
}

double*& LayerMtp::GetOutErr()
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

void LayerMtp::SetFunc(double (*_F)(double), double (*_DF)(double))
{
	F1 = _F;
	DF1 = _DF;
	F2 = _F;
	DF2 = _DF;
}

void LayerMtp::SetFunc1(double (*_F)(double), double (*_DF)(double))
{
	F1 = _F;
	DF1 = _DF;
}

void LayerMtp::SetFunc2(double (*_F)(double), double (*_DF)(double))
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

void LayerMtp::ResetErr()
{
	for (int i = 0; i < ins; i++)
	{
		inerr[i] = 0;
	}
}


void LayerMtp::Train()
{
	for (int i = 0; i < ins; i++)
	{
		for (int o = 0; o < outs; o++)
			inerr[i] += outerr[o] * (
			links1[o + (i + 1) * outs] * DF1(outbase1[o]) * F2(outbase2[o]) + 
			links2[o + (i + 1) * outs] * DF2(outbase2[o]) * F1(outbase1[o])) ;
	}


	float max = 0;

	for (int o = 0; o < outs; o++)
	{
		links1[o] += outerr[o] * DF1(outbase1[o]) * F2(outbase2[o]) * LF;
		links2[o] += outerr[o] * DF2(outbase2[o]) * F1(outbase1[o]) * LF;
		for (int i = 0; i < ins; i++)
		{
			links1[o + (i + 1) * outs] += outerr[o] * DF1(outbase1[o]) * F2(outbase2[o]) * in[i] * LF;
			links2[o + (i + 1) * outs] += outerr[o] * DF2(outbase2[o]) * F1(outbase1[o]) * in[i] * LF;
			max = fmax(max, abs(links1[o + (i + 1) * outs]));
			max = fmax(max, abs(links2[o + (i + 1) * outs]));
		}
	}

	for (int o = 0; o < outs; o++)
		for (int i = 0; i < ins; i++)
		{
			links1[o + (i + 1) * outs] = abs(links1[o + (i + 1) * outs]) < max * 200 ? links1[o + (i + 1) * outs] * 0.9 : links1[o + (i + 1) * outs];
			links2[o + (i + 1) * outs] = abs(links2[o + (i + 1) * outs]) < max * 200 ? links2[o + (i + 1) * outs] * 0.9 : links2[o + (i + 1) * outs];
		}

}
