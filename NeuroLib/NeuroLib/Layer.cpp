#include "Layer.h"

Layer::Layer(int _ins, int _outs, float* _in, float* _out)
{
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

	links = new float[(ins + 1) * outs];
}
