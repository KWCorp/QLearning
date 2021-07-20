#pragma once



namespace qln
{
#define MAX(a,b) (a > b ? a : b)


	template <const size_t Dim> struct QDLearner_M
	{
		size_t S[Dim];

		float** Q;

		float LF = 0.01, DF = 0.01;
		size_t state, action;

		QLearner_M(size_t* _state, size_t _action)
		{
			state = 1;
			for (int i = 0; i < Dim; i++)
			{
				S[i] = _state[i];
				state *= _state[i];
			}
			action = _action;
			Q = new float* [state];
			for (int i = 0; i < state; i++)
			{
				Q[i] = new float[action];
				for (int j = 0; j < action; j++)
					Q[i][j] = 0;
			}
		}

		size_t GetS(size_t* _s)
		{
			size_t s = 0;
			size_t m = 1;
			for (int i = 0; i < Dim; i++)
			{
				s += _s[i] * m;
				m *= S[i];
			}
			return s;
		}

		size_t* GetS(size_t s)
		{
			size_t* _s = new size_t[Dim];
			for (int i = 0; i < Dim; i++)
			{
				_s[i] = s % S[i];
				s /= S[i]
			}
			return _s;
		}

		size_t GetMaxA(size_t s)
		{
			float max = Q[s][0];
			for (int j = 1; j < action; j++)
				max = MAX(Q[s][j], max);
			return max;
		}

		size_t GetMaxA(size_t* _s)
		{
			return GetMaxA(GetS(_s));
		}

		void Update(size_t s, size_t a, float d)
		{
			Q[s][a] += LF * (d - Q[s][a]);
		}

		void Update(size_t* _s, size_t a, float d)
		{
			Update(GetS(_s), a, d);
		}

		~QLerner_M()
		{
			for (int i = 0; i < state; i++)
			{
				delete[] Q[i];
			}
			delete[] Q;
		}
	};

	struct QDLearner
	{
		float** Q;

		float LF = 0.01, DF = 0.01;
		size_t state, action;

		QLearner(size_t _state, size_t _action)
		{
			state = _state;
			action = _action;
			Q = new float* [state];
			for (int i = 0; i < state; i++)
			{
				Q[i] = new float[action];
				for (int j = 0; j < action; j++)
					Q[i][j] = 0;
			}
		}

		size_t GetMaxA(size_t s)
		{
			float max = Q[s][0];
			for (int j = 1; j < action; j++)
				max = MAX(Q[s][j], max);
			return max;
		}

		void Update(size_t s, size_t a, float d)
		{
			Q[s][a] += LF * (d - Q[s][a]);
		}

		~QLearner()
		{
			for (int i = 0; i < state; i++)
			{
				delete[] Q[i];
			}
			delete[] Q;
		}
	};

	struct Neuron
	{
		float(*F)(float);

		float(*DF)(float);

		uint32_t layers, * sizes, * ids, ins, outs;

		float* neurons , * err, * base, * outptr;

		float** links;

		float LF = 0.001;

		Neuron(float(*_F)(float), float(*_DF)(float), uint32_t _layers, uint32_t* _sizes)
		{
			F = _F;
			DF = _DF;
			int l = 0;
			for (; l < _layers; l++)
			{
				if (_sizes[l] == 0)
					break;
			}
			layers = l - 1;
			l = 0;
			int S = 0;
			sizes = new uint32_t[layers];
			ids = new uint32_t[layers + 1];
			links = new float* [layers - 1];
			for (; l < layers; l++)
			{
				sizes[l] = _sizes[l];
				ids[l] = S;
				S += sizes[l];
				if (l < layers - 1)
				{
					links[l] = new float[sizes[l] * sizes[l + 1]];
					for (int i = 0; i < sizes[l] * sizes[l + 1]; i++)
						links[l][i] = 0.5 - rand() / float(RAND_MAX);
				}
			}
			ids[layers] = S;
			neurons = new float[S];
			ins = sizes[0];
			outs = sizes[layers - 1];
			outptr = &neurons[ids[layers - 1]];
		}

		void Calc()
		{
			for (int l = 0; l < layers - 1; l++)
			{
				for (int o = 0; o < sizes[l + 1]; o++)
				{
					base[ids[l + 1] + o] = 0;
					for (int i = 0; i < sizes[l]; i++)
						base[ids[l + 1] + o] += neurons[ids[l] + i] * links[l][o * sizes[l] + i];
					neurons[ids[l + 1] + o] = F(base[ids[l + 1] + o]);
				}
			}
		}

		void Train(float* _err)
		{
			for (int i = 0; i < outs; i++)
				err[ids[layers - 1] +  i] = _err[i];

			for (int l = layers - 2; l >= 0; l--)
			{
				for (int i = 0; i < sizes[l]; i++)
				{
					err[ids[l] + i] = 0;
					for (int o = 0; o < sizes[l + 1]; o++)
						err[ids[l] + i] += err[ids[l + 1] + o] * links[l][o * sizes[l] + i] * DF(base[ids[l + 1] + o]);
				}

				for (int o = 0; o < sizes[l + 1]; o++)
					for (int i = 0; i < sizes[l]; i++)
						links[l][o * sizes[l] + i] -= LF * err[ids[l + 1] + o] * neurons[ids[l] + i] * DF(base[ids[l + 1] + o]);
			}
		}

		float* Get(float* input)
		{
			for (int i = 0; i < ins; i++)
				neurons[i] = input[i];

			Calc();

			float* out = new float[outs];

			for (int i = 0; i < outs; i++)
				out[i] = outptr[i];

			return out;
		}

	};
}