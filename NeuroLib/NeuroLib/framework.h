#pragma once

#include <cmath>

double F_RELU(double x);

double DF_RELU(double x);

double F_GATE(double x);

double DF_GATE(double x);

double F_SGATE(double x);

double DF_SGATE(double x);


class INeuro
{
public:
	virtual double*& GetIn() = 0;

	virtual double*& GetOut() = 0;

	virtual double*& GetInErr() = 0;

	virtual double*& GetOutErr() = 0;

	virtual int GetInSize() = 0;

	virtual int GetOutSize() = 0;

	virtual void SetFunc(double (*_F)(double), double (*_DF)(double)) = 0;

	virtual void Calc() = 0;

	virtual void Train() = 0;
};
