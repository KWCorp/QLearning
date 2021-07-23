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

	virtual void SetFunc(float (*_F)(float), float (*_DF)(float));

	virtual void Calc();

	virtual void Train();
};
