#pragma once
#include "framework.h"
class NeuralNetwork :
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

	INeuro** elements;


};

