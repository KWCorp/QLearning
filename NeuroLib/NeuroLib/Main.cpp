#include "framework.h"

double F_RELU(double x)
{
	return 0.99 * x * (x > 0) + 0.01 * x;
}

double DF_RELU(double x)
{
	return 0.99 + (x > 0) + 0.01;
}

double F_GATE(double x)
{
	return x + 1 + (1 / (1 - x) - x - 1) * (x < 0);
}

double DF_GATE(double x)
{
	return 1 + (1 / (1 - 2 * x + x * x) - 1) * (x < 0);
}

double F_SGATE(double x)
{
	return x / (1 + abs(x));
}

double DF_SGATE(double x)
{
	return x > 0 ? 1 / (1 + 2 * x + x * x) : 1 / (1 + 2 * x + x * x);
}