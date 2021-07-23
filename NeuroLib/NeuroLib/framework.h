#pragma once

class INeuro
{
public:
	virtual float*& GetIn();

	virtual float*& GetOut();

	virtual float*& GetInErr();

	virtual float*& GetOutErr();

	virtual int GetInSize();

	virtual int GetOutSize();

	virtual void SetFunc(float* (float));

	virtual void Calc();

	virtual void Train();
};
