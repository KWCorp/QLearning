#pragma once



namespace qln
{
#define MAX(a,b) (a > b ? a : b)
	struct QLerner
	{	
		float ** Q;
		float LF = 0.01, DF = 0.01;
		size_t state, action;

		QLerner(size_t _state, size_t _action)
		{
			state = _state;
			action = _action;
			Q = new float *[state];
			for(int i = 0; i < state; i++)
			{
				Q[i] = new float[action];
				for(int j = 0; j < action; j++)
					Q[i][j] = 0;
			}
		}

		size_t GetMaxA(size_t s)
		{
			float max = Q[s][0];
			for(int j = 1; j < action; j++)
				max = MAX(Q[s][j], max);
			return max;
		}

		void Update(size_t s, size_t a, float d)
		{
			d *= 0.5;
			Q[s][a] += LF * d;
			if(action <= 1) return;
			for(int j = 0; j < action; j++)
				if(j != a) 
					Q[s][j] -= LF * d;
		}

		~QLerner()
		{
			for(int i = 0; i < state; i++)
			{
				delete[] Q[i];
			}
			delete[] Q;
		}
	};

	template <const size_t Dim> struct QLearner_M
	{
		size_t S[Dim];
		
		float ** Q;

		float LF = 0.01, DF = 0.01;
		size_t state, action;

		QLearner_M(size_t * _state, size_t _action)
		{
			state = 1;
			for(int i = 0; i < Dim; i++)
			{
				S[i] = _state[i];
				state *= _state[i];
			}
			action = _action;
			Q = new float *[state];
			for(int i = 0; i < state; i++)
			{
				Q[i] = new float[action];
				for(int j = 0; j < action; j++)
					Q[i][j] = 0;
			}
		}

		size_t GetS(size_t* _s)
		{
			size_t s = 0;
			size_t m = 1;
			for(int i = 0; i < Dim; i++)
			{
				s += _s[i] * m;
				m *= S[i];
			}
			return s;
		}

		size_t* GetS(size_t s)
		{
			size_t *_s = new size_t[Dim];
			for(int i = 0; i < Dim; i++)
			{
				_s[i] = s % S[i];
				s /= S[i]
			}
			return _s;
		}

		size_t GetMaxA(size_t s)
		{
			float max = Q[s][0];
			for(int j = 1; j < action; j++)
				max = MAX(Q[s][j], max);
			return max;
		}

		size_t GetMaxA(size_t *_s)
		{
			return GetMaxA(GetS(_s));
		}

		void Update(size_t s, size_t a, float d)
		{
			d *= 0.5;
			Q[s][a] += LF * d;
			if(action <= 1) return;
			for(int j = 0; j < action; j++)
				if(j != a) 
					Q[s][j] -= LF * d;
		}

		void Update(size_t *_s, size_t a, float d)
		{
			Update(GetS(_s), a, d);
		}

		~QLerner_M()
		{
			for(int i = 0; i < state; i++)
			{
				delete[] Q[i];
			}
			delete[] Q;
		}
	};
}