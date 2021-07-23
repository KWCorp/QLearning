// DicretAnts.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <string>
#include <thread>
#include <ctime> 

#ifdef _WIN32

#include "Windows.h"

#endif

void Print(int mx, int my, char** buff);

const float PI = 3.1415926535897932384626433832795;

struct Neuron
{
	float(*F)(float);

	float(*DF)(float);

	uint32_t layers, * sizes, * ids, ins, outs;

	float* neurons, * err, * base, * outptr;

	float** links;

	float LF = 0.001;

	Neuron(float _F(float x), float _DF(float x), uint32_t _layers, uint32_t* _sizes)
	{
		F = _F;
		DF = _DF;
		int l = 0;
		for (; l < _layers; l++)
		{
			if (_sizes[l] == 0)
				break;
		}
		layers = l;
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
			if (l != 0)
			{
				links[l - 1] = new float[sizes[l] * (sizes[l - 1] + 1)];
				for (int i = 0; i < sizes[l] * (sizes[l - 1] + 1); i++)
					links[l - 1][i] = 1 - rand() / float(RAND_MAX) * 2;
			}
		}
		ids[layers] = S;
		neurons = new float[S];
		base = new float[S];
		err = new float[S];
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
				base[ids[l + 1] + o] = links[l][o + sizes[l] * sizes[l + 1]];
				for (int i = 0; i < sizes[l]; i++)
					base[ids[l + 1] + o] += neurons[ids[l] + i] * links[l][o + i * sizes[l + 1]];
				neurons[ids[l + 1] + o] = F(base[ids[l + 1] + o]);
			}
		}
	}

	void Train(float* _err)
	{
		for (int i = 0; i < outs; i++)
			err[ids[layers - 1] + i] = _err[i];

		for (int l = layers - 2; l >= 0; l--)
		{
			for (int i = 0; i < sizes[l]; i++)
			{
				err[ids[l] + i] = 0;
				for (int o = 0; o < sizes[l + 1]; o++)
					err[ids[l] + i] += err[ids[l + 1] + o] * links[l][o + i * sizes[l + 1]] * DF(base[ids[l + 1] + o]);
			}

			for (int o = 0; o < sizes[l + 1]; o++)
			{
				for (int i = 0; i < sizes[l]; i++)
					links[l][o + i * sizes[l + 1]] += LF * err[ids[l + 1] + o] * neurons[ids[l] + i] * DF(base[ids[l + 1] + o]);
				links[l][o + sizes[l] * sizes[l + 1]] += LF * err[ids[l + 1] + o] * DF(base[ids[l + 1] + o]);
			}
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

struct MedianGradientTriner
{
	Neuron* base = 0;

	float** deltas = 0;

	float C = 0;

	MedianGradientTriner(Neuron* _base)
	{
		Init(_base);
	}

	void Init(Neuron* _base)
	{
		if (base != 0)
		{
			for (int l = 0; l < base->layers - 1; l++)
			{
				delete[] deltas[l];
			}
			delete[] deltas;
		}
		C = 0;
		base = _base;
		deltas = new float* [base->layers - 1];
		for (int l = 0; l < base->layers; l++)
		{
			if (l != 0)
			{
				deltas[l - 1] = new float[base->sizes[l] * (base->sizes[l - 1] + 1)];
				for (int i = 0; i < base->sizes[l] * (base->sizes[l - 1] + 1); i++)
					deltas[l - 1][i] = 0;
			}
		}
	} 

	void Accum(float* _err)
	{
		for (int i = 0; i < base->outs; i++)
			base->err[base->ids[base->layers - 1] + i] = _err[i];

		for (int l = base->layers - 2; l >= 0; l--)
		{
			for (int i = 0; i < base->sizes[l]; i++)
			{
				base->err[base->ids[l] + i] = 0;
				for (int o = 0; o < base->sizes[l + 1]; o++)
					base->err[base->ids[l] + i] += base->err[base->ids[l + 1] + o] * base->links[l][o + i * base->sizes[l + 1]] * base->DF(base->base[base->ids[l + 1] + o]);
			}

			for (int o = 0; o < base->sizes[l + 1]; o++)
			{
				for (int i = 0; i < base->sizes[l]; i++)
					deltas[l][o + i * base->sizes[l + 1]] += base->LF * base->err[base->ids[l + 1] + o] * base->neurons[base->ids[l] + i] * base->DF(base->base[base->ids[l + 1] + o]);
				deltas[l][o + base->sizes[l] * base->sizes[l + 1]] += base->LF * base->err[base->ids[l + 1] + o] * base->DF(base->base[base->ids[l + 1] + o]);
			}
		}
		C++;
	}

	void Train()
	{
		for (int l = base->layers - 2; l >= 0; l--)
		{
			for (int o = 0; o < base->sizes[l + 1]; o++)
			{
				for (int i = 0; i < base->sizes[l]; i++)
					base->links[l][o + i * base->sizes[l + 1]] += deltas[l][o + i * base->sizes[l + 1]] / C;
				base->links[l][o + base->sizes[l] * base->sizes[l + 1]] += deltas[l][o + base->sizes[l] * base->sizes[l + 1]] / C;

				for (int i = 0; i < base->sizes[l]; i++)
					deltas[l][o + i * base->sizes[l + 1]] = 0;
				deltas[l][o + base->sizes[l] * base->sizes[l + 1]] = 0;
			}
		}
		C = 0;
	}
};



template<const size_t States, const size_t Actions> struct QDiscret
{
	float Q[States][Actions];

	float LF = 0.1, DF = 0.01;

	QDiscret()
	{
		for (size_t s = 0; s < States; s++)
			for (size_t a = 0; a < Actions; a++)
			{
				Q[s][a] = rand() / float(RAND_MAX);
			}
	}

	float MaxInS(size_t s)
	{
		float m = Q[s][0];
		for (int a = 1; a < Actions; a++)
			if (Q[s][a] > m) m = Q[s][a];
		return m;
	}

	size_t MaxA(size_t s)
	{
		size_t m = 0;
		for (int a = 1; a < Actions; a++)
			if (Q[s][a] > Q[s][m]) m = a;
		return m;
	}

	size_t SelectA(size_t s, size_t n)
	{
		size_t m = 0, om = -1;
		for (int i = 0; i <= n; i++)
		{
			m = -1;
			for (int a = 0; a < Actions; a++)
				if ((m == -1 || Q[s][a] > Q[s][m]) && (om == -1 || Q[s][a] < Q[s][om])) m = a;
			om = m;
		}
		return m;
	}

	void Update(size_t s, size_t a, float d)
	{
		Q[s][a] += LF * (d - Q[s][a]);
	}

	void Update(size_t s, size_t a, size_t ns, float d)
	{
		Q[s][a] += LF * (d + DF * MaxInS(ns) - Q[s][a]);
	}

	void Normalize(float summ)
	{
		for (size_t s = 0; s < States; s++)
		{
			float e = 0;
			for (size_t a = 0; a < Actions; a++)
			{
				e += Q[s][a];
			}
			e /= summ;
			for (size_t a = 0; a < Actions; a++)
			{
				Q[s][a] /= e;
			}
		}
	}

	void Break(float _max)
	{
		for (size_t s = 0; s < States; s++)
		{
			float e = Q[s][0];
			for (size_t a = 1; a < Actions; a++)
			{
				e = max(Q[s][a], e);
			}
			e /= _max;
			for (size_t a = 0; a < Actions; a++)
			{
				Q[s][a] /= e;
			}
		}
	}
};

float F_LRELU(float x)
{
	return (x > 0 ? x : 0.01 * x);
}

float DF_LRELU(float x)
{
	return (x > 0 ? 1 : 0.01);
}

float F_GATE(float x)
{
	return (x >= 0 ? x + 1 : 1 / (1.0 - x));
}

float DF_GATE(float x)
{
	return (x >= 0 ? 1 : 1 / ((1.0 - x) * (1.0 - x)));
}

int main()
{
	const char wall = char(219);
	const char space = ' ';
	const char ant = char(253);
	const char target = 'X';
	int mx = 50;
	int my = 20;
	char** buff = new char* [mx];
	for (int x = 0; x < mx; x++)
		buff[x] = new char[my];

	uint16_t** dist = new uint16_t * [mx];
	for (int x = 0; x < mx; x++)
		dist[x] = new uint16_t[my];

	setlocale(LC_ALL, "utf-8");
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);	// Получение дескриптора устройства стандартного вывода

	QDiscret<size_t(4 * 8 * 2 * 2 * 2 * 2), 4> Move;

	for (int y = 0; y < my; y++)
		for (int x = 0; x < mx; x++)
			buff[x][y] = x == 0 || y == 0 || x == mx - 1 || y == my - 1 ? wall : space;

	int ax = 1, ay = 1;
	int tx = mx, ty = 0;
	size_t __K__ = 1e4;
	srand(static_cast<unsigned int>(time(NULL)));
	int k = 0;
	for (size_t __T__ = 0, __Y__ = 0, __W__ = 0;;)
	{
		float KK = 10;
		Move.LF = 0.001;

		if (__T__ % __K__ == 0)
		{
			__Y__++;
			__T__ = 0;
			for (int y = 0; y < my; y++)
				for (int x = 0; x < mx; x++)
					buff[x][y] = x == 0 || y == 0 || x == mx - 1 || y == my - 1 ? wall : space;
			int x = rand() % mx, y = rand() % my, k = -1;

			for (int i = 0, g = 0; i < (__Y__ < 30 ? 100 : (__Y__ < 50 ? 200 : 1000)); i++)
			{
				g++;
				if (buff[x][y] == space)
				{
					int f = 0;

					f = 0;

					f = 0;

					{
						for (int a = -1; a < 2; a++)
							for (int b = -1; b < 2; b++)
								if ((a != 0 || b != 0))
									if (
										buff[x + a][y + b] == space
										)
									{
										f += 4;
									}
									else if (
										x + a == 0 ||
										x + a == mx - 1 ||
										y + b == 0 ||
										y + b == my - 1
										)
									{
										f += 2;
									}
					}
					if (f > 27)
						buff[x][y] = wall;

					if (x > 1 && y > 1 && x < mx - 2 && y < my - 2)
					{
						f = 0;
						{
							for (int a = -2; a < 3; a++)
								for (int b = -2; b < 3; b++)
									if ((a != 0 || b != 0))
										if (
											buff[x + a][y + b] == space
											)
										{
											f += 4;
										}
										else if (
											x + a == 0 ||
											x + a == mx - 1 ||
											y + b == 0 ||
											y + b == my - 1
											)
										{
											f += 4;
										}
						}
						if (f > 80)
							buff[x][y] = wall;
					}

					if (buff[x][y] == wall)
					{
						//Print(mx, my, buff);
						g = 0;
					}

				}
				k = (k == -1 || rand() % 2 == 0) ? rand() % 4 : k;
				switch (k)
				{
				case 0:
					x++;
					break;
				case 1:
					x--;
					break;
				case 2:
					y++;
					break;
				case 3:
					y--;
					break;



				default:
					break;
				}

				x = max(1, min(mx - 2, x));
				y = max(1, min(my - 2, y));

				if (g == 5000)
					i = 1e10;
			}
			tx = rand() % mx, ty = rand() % my;
			while (buff[tx][ty] != space)
			{
				tx = rand() % mx; ty = rand() % my;
			}
			buff[tx][ty] = target;
			buff[ax][ay] = space;
			ax = rand() % mx; ay = rand() % my;
			while (buff[ax][ay] != space)
			{
				ax = rand() % mx; ay = rand() % my;
			}
			buff[ax][ay] = ant;

			for (int y = 0; y < my; y++)
				for (int x = 0; x < mx; x++)
					dist[x][y] = -1;
			dist[tx][ty] = 0;
			{
				size_t q = 1;
				while (q > 0)
				{
					int qo = q;
					q = 0;
					for (int y = 1; y < my - 1; y++)
						for (int x = 1; x < mx - 1; x++)
						{
							if (buff[x][y] == space && dist[x][y] == uint16_t(-1))
							{
								dist[x][y] = min(dist[x][y], dist[x + 1][y]);
								dist[x][y] = min(dist[x][y], dist[x - 1][y]);
								dist[x][y] = min(dist[x][y], dist[x][y + 1]);
								dist[x][y] = min(dist[x][y], dist[x][y - 1]);
								if (dist[x][y] != uint16_t(-1))
									dist[x][y]++;
							}
							if (buff[x][y] == space && dist[x][y] == uint16_t(-1))
								q++;
						}
					if (q == qo)
						q = 0;
				}
			}
		}
		__T__++;
		buff[ax][ay] = space;
		float r = (ax - tx) * (ax - tx) + (ay - ty) * (ay - ty);

		float Al = acos((ax - tx) / sqrtf(r));
		Al = (ay - ty >= 0 ? Al : 2 * PI - Al) * 4 / PI;
		Al = (Al > 7 ? Al - 8 : Al);
		Al += 1;

		int ko = k;
		int d = Al, t1 = 0, t2 = 0,
			s =
			ko * 16 * 8 +
			d * 16 +
			(buff[ax + 1][ay] != wall) * 8 +
			(buff[ax - 1][ay] != wall) * 4 +
			(buff[ax][ay + 1] != wall) * 2 +
			(buff[ax][ay - 1] != wall) * 1;

		if (d == 0)
		{
			Al = Al;
		}

		float A[4];
		for (int a = 0; a < 4; a++)
			A[a] = Move.Q[s][a];
		{
			float Amin = 10000;
			for (int a = 0; a < 4; a++) if (A[a] > -100)
				Amin = min(Amin, A[a]);

			for (int a = 0; a < 4; a++) if (A[a] > -100)
				A[a] = sqrtf(A[a] - Amin + 0.1);
			else
				A[a] = 0;

			float S = 0;
			for (int a = 0; a < 4; a++) if (A[a] > -100)
				S += A[a];

			for (int a = 0; a < 4; a++) if (A[a] > -100)
				A[a] /= S;
		}

		{
			k = 3;
			float rnd = rand() / float(RAND_MAX);
			for (int a = 0; a < 4; a++) if (A[a] > -100)
				if (A[a] > rnd || a == 4 - 1)
				{
					k = a;
					break;
				}
				else
				{
					rnd -= A[a];
				}

		}

		//k = rand() % 3 != 0 ? Move.MaxA(s) : k;
		if (__Y__ < 8)
		{
			s =
				d * 16 +
				(__T__ % 16);
		}
		if (__Y__ < 10)
		{
			//k = rand() % 4;
		}
		bool iswal = true, istarg = false;
		uint16_t dio = dist[ax][ay];
		switch (k)
		{
		case 0:
			iswal = buff[ax + 1][ay] == wall;
			if (!iswal)ax++;
			break;
		case 1:
			iswal = buff[ax - 1][ay] == wall;
			if (!iswal)ax--;
			break;
		case 2:
			iswal = buff[ax][ay + 1] == wall;
			if (!iswal)ay++;
			break;
		case 3:
			iswal = buff[ax][ay - 1] == wall;
			if (!iswal)ay--;
			break;
		default:
			break;
		}

		switch (k)
		{
		case 0:
			istarg = buff[ax + 1][ay] == target;
			break;
		case 1:
			istarg = buff[ax - 1][ay] == target;
			break;
		case 2:
			istarg = buff[ax][ay + 1] == target;
			break;
		case 3:
			istarg = buff[ax][ay - 1] == target;
			break;
		default:
			break;
		}
		float nr = (ax - tx) * (ax - tx) + (ay - ty) * (ay - ty);
		if (istarg || r < 1 || nr < 1 || ax < 0 || ay < 0 || ax >= mx || ay >= my)
		{
			__T__ = 0;
			continue;
		}
		r = sqrtf(r);
		nr = sqrtf(nr);

		int nd = abs(ax - tx) > abs(ay - ty) ? (ax - tx > 0 ? 0 : 1) : (ax - tx > 0 ? 2 : 3),
			ns =
			k * 16 * 8 +
			d * 16 +
			(buff[ax + 1][ay] == space) * 8 +
			(buff[ax - 1][ay] == space) * 4 +
			(buff[ax][ay + 1] == space) * 2 +
			(buff[ax][ay - 1] == space) * 1;

		if (iswal)
		{
			if (__Y__ > 20)
				Move.Q[s][k] = -1000;
		}
		else if (nr < r)
		{
			//Move.Update(s, k, 0);
		}
		//else
			//Move.Update(s, k, 0);

		if (abs(dio - dist[ax][ay]) == 1)
		{
			Move.Update(s, k, (r - nr) * KK);
			Move.Update(s, k, (dio - dist[ax][ay]) * KK);
		}


		buff[ax][ay] = ant;
		if (!iswal && __T__ % 1 == 0 && __Y__ > 1000)
		{
			Print(mx, my, buff);
			COORD position;
			CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
			GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
			std::string cl = "";
			for (int w = mx; w < consoleInfo.srWindow.Right; w++)
			{
				cl += ' ';
			}
			for (int q = 0; q < 10; q++)
			{
				position.X = mx;
				position.Y = q;
				SetConsoleCursorPosition(hConsole, position);
				std::cout << cl;
			}

			position.X = mx;
			position.Y = 0;
			SetConsoleCursorPosition(hConsole, position);
			std::cout << "dir " << d << " state " << s << " Score " << __Y__ << ' ' << __T__ / float(__K__);
			position.X = mx;
			position.Y = 1;
			SetConsoleCursorPosition(hConsole, position);
			switch (k)
			{
			case 0:
				std::cout << "right ";
				break;
			case 1:
				std::cout << "left ";
				break;
			case 2:
				std::cout << "down ";
				break;
			case 3:
				std::cout << "up ";
				break;
			default:
				break;
			}
			for (int q = 0; q < 4; q++)
			{
				position.X = mx;
				position.Y = q + 2;
				SetConsoleCursorPosition(hConsole, position);
				switch (q)
				{
				case 0:
					std::cout << "right ";
					break;
				case 1:
					std::cout << "left ";
					break;
				case 2:
					std::cout << "down ";
					break;
				case 3:
					std::cout << "up ";
					break;
				default:
					break;
				}
				std::cout << A[q] << ' ';
			}
			std::cout << std::endl;
			std::this_thread::sleep_for(std::chrono::milliseconds(5));

		}
	}


}


void Print(int mx, int my, char** buff)
{
	if (buff == 0) return;
	COORD position;
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	position.X = 0;
	position.Y = 0;
	SetConsoleCursorPosition(hConsole, position);
	std::string Sbuff = "";
	for (int y = 0; y < my; y++)
	{
		for (int x = 0; x < mx; x++)
		{
			if (buff[x] != 0)
			{
				if (buff[x][y] != '\n')
					Sbuff += buff[x][y];
				else
					Sbuff += ' ';
			}
			else
				Sbuff += ' ';
		}
		Sbuff += '\n';
	}
	//system("cls");
	puts(Sbuff.c_str());
	position.X = 0;									// Установка координаты X
	position.Y = 0;
	SetConsoleCursorPosition(hConsole, position);		// Перемещение каретки по заданным координатам
}
