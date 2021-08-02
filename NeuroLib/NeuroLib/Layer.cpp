#include "Layer.h"



Layer::Layer(int _ins, int _outs, double* _in, double* _out, double* _ine, double* _oute)
{
	F = F_RELU;
	DF = DF_RELU;
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

	outbase = new double[outs];

	links = new double[(ins + 1) * outs];

	for (int l = 0; l < (ins + 1) * outs; l++)
	{
		links[l] = rand() / double(RAND_MAX) * 2 - 1;
	}
}

double*& Layer::GetIn()
{
	return in;
}

double*& Layer::GetOut()
{
	return out;
}

double*& Layer::GetInErr()
{
	return inerr;
}

double*& Layer::GetOutErr()
{
	return outerr;
}

int Layer::GetInSize()
{
	return ins;
}

int Layer::GetOutSize()
{
	return outs;
}

void Layer::SetFunc(double (*_F)(double), double (*_DF)(double))
{
	F = _F;
	DF = _DF;
}

void Layer::Calc()
{
	for (int o = 0; o < outs; o++)
	{
		outbase[o] = links[o];
		for (int i = 0; i < ins; i++)
		{
			outbase[o] += in[i] * links[o + (i + 1) * outs];
		}
		out[o] = F(outbase[o]);
	}
}

void Layer::ResetErr()
{
	for (int i = 0; i < ins; i++)
	{
		inerr[i] = 0;
	}
}

void Layer::Train()
{
	for (int i = 0; i < ins; i++)
	{
		for (int o = 0; o < outs; o++)
			inerr[i] += outerr[o] * links[o + (i + 1) * outs] * DF(outbase[o]);
	}

	for (int o = 0; o < outs; o++)
	{
		links[o] += outerr[o] * DF(outbase[o]) * LF;
		for (int i = 0; i < ins; i++)
			links[o + (i + 1) * outs] += outerr[o] * DF(outbase[o]) * in[i] * LF;
	}

}
