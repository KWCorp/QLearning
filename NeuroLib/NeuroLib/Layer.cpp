#include "Layer.h"

float F_RELU(float x)
{
	return 0.99 * x * (x > 0) + 0.01 * x;
}

float DF_RELU(float x)
{
	return 0.99 + (x > 0) + 0.01;
}

Layer::Layer(int _ins, int _outs, float* _in, float* _out, float* _ine, float* _oute)
{
	F = F_RELU;
	DF = DF_RELU;
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

	outbase = new float[outs];

	links = new float[(ins + 1) * outs];
}

float*& Layer::GetIn()
{
	return in;
}

float*& Layer::GetOut()
{
	return out;
}

float*& Layer::GetInErr()
{
	return inerr;
}

float*& Layer::GetOutErr()
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

void Layer::SetFunc(float (*_F)(float), float (*_DF)(float))
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

void Layer::Train()
{
	for (int i = 0; i < ins; i++)
	{
		inerr[i] = 0;
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
