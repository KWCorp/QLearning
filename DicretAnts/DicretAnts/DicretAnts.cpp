﻿// DicretAnts.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <string>
#include <thread>
#include <ctime> 

#ifdef _WIN32

#include "Windows.h"

#endif

void Print(int mx, int my, char** buff);


template<const size_t States, const size_t Actions> struct QDiscret
{
	float Q[States][Actions];

	float LF = 0.1, DF = 0.01;

	QDiscret()
	{
		srand(static_cast<unsigned int>(time(0)));
		size_t Seed = 0;
		{
			for(int i = 0; i < 100; i++)
			{
				uint8_t *T = new uint8_t;
				Seed += *T;
				delete T;
			}
		}
		Seed = Seed % 16127;
		for(size_t s = 0; s < States; s++)
			for(size_t a = 0; a < Actions; a++)
			{
				Q[s][a] = 0;
			}
	}

	float MaxInS(size_t s)
	{
		float m = Q[s][0];
		for(int a = 1; a < Actions; a++)
			if(Q[s][a] > m) m = Q[s][a];
		return m;
	}

	size_t MaxA(size_t s)
	{
		size_t m = 0;
		for(int a = 1; a < Actions; a++)
			if(Q[s][a] > Q[s][m]) m = a;
		return m;
	}

	void Update(size_t s, size_t a, float d)
	{
		Q[s][a] += LF * (d);
	}

	void Update(size_t s, size_t a, size_t ns, float d)
	{
		Q[s][a] += LF * (d + DF * (MaxInS(ns)- Q[s][a]));
	}

	void Normalize(float summ)
	{
		for(size_t s = 0; s < States; s++)
		{
			float e = 0;
			for(size_t a = 0; a < Actions; a++)
			{
				e += Q[s][a];
			}
			e /= summ;
			for(size_t a = 0; a < Actions; a++)
			{
				Q[s][a] /= e;
			}
		}
	}

	void Break(float _max)
	{
		for(size_t s = 0; s < States; s++)
		{
			float e = Q[s][0];
			for(size_t a = 1; a < Actions; a++)
			{
				e = max(Q[s][a],e);
			}
			e /= _max;
			for(size_t a = 0; a < Actions; a++)
			{
				Q[s][a] /= e;
			}
		}
	}
};

template<const size_t Vars, const size_t Actions, size_t InterpolationCount = 100> struct QInterpol
{
	float Q[Actions][Vars][InterpolationCount], B[Vars][2];

	float LF = 0.01, DF = 0.01;

	QInterpol(float * mins, float * maxs)
	{
		size_t Seed = 0;
		{
			for(int i = 0; i < 100; i++)
			{
				uint8_t *T = new uint8_t;
				Seed += *T;
				delete T;
			}
		}
		Seed = Seed % 16127;
		for(size_t a = 0; a < Actions; a++)
			for(size_t v = 0; v < Vars; v++)
				for(size_t i = 0; i < InterpolationCount; i++)
				{
					
					Q[a][v][i] = ((a * 1931 + 1291 * v + i * 3329 + Seed * 317) % 100) * 0.01;
				}
	}

	float MaxInS(size_t v)
	{
		float m = Q[0][v][0];
		for(size_t a = 0; a < Actions; a++)
			for(size_t i = 0; i < InterpolationCount; i++)
				if(Q[a][v][i] > m) m = Q[a][v][i];
		return m;
	}

	size_t MaxA(size_t v)
	{
		size_t m1 = 0, m2 = 0;
		for(size_t a = 0; a < Actions; a++)
				for(size_t i = 0; i < InterpolationCount; i++)
					if(Q[a][v][i] > Q[m1][v][m2]) { m1 = a; m2 = i; }
		return m1;
	}

	void Update(size_t s, size_t a, float d)
	{
		Q[s][a] += LF * (d);
	}

	void Update(size_t s, size_t a, size_t ns, float d)
	{
		Q[s][a] += LF * (d + DF * (MaxInS(ns)- Q[s][a]));
	}
};


int main()
{
	//for(int i = 0; i < 256; i++)
	//	std::cout << i << " : " << char(i) << '\n';
	//system("pause");
	//return 0;

	const char wall = char(219);
	const char space = ' ';
	const char ant = char(253);
	const char target = 'X';
	int mx = 50;
	int my = 20;
	char** buff = new char*[mx];
	for(int x = 0; x < mx; x++)
		buff[x] = new char[my];
	setlocale(LC_ALL, "utf-8");
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);	// Получение дескриптора устройства стандартного вывода

	QDiscret<size_t(4 * 4 * 2 * 2 * 2 * 2), 4> Move;

	for(int y = 0; y < my; y++)
		for(int x = 0; x < mx; x++)
			buff[x][y] = x == 0 || y == 0 || x == mx - 1 || y == my - 1 ? wall : space;
	
	int ax = 1, ay = 1;
	int tx = mx, ty = 0;
	size_t __K__ = 1e4;
	srand(static_cast<unsigned int>(time(NULL)));
	int k = 0;
	for(size_t __T__ = 0, __Y__ = 0;;)
	{
		if(__T__ % __K__ == 0)
		{		
			__Y__++;
			__T__ = 0;
			for(int y = 0; y < my; y++)
				for(int x = 0; x < mx; x++)
					buff[x][y] = x == 0 || y == 0 || x == mx - 1 || y == my - 1 ? wall : space;
			int x = rand() % mx, y = rand() % my, k = -1;
			
			for(int i = 0, g = 0; i < (__Y__ < 10 ? 0 : (__Y__ < 20 ? 1000 : 10000)); i++)
			{
				g++;
				if(buff[x][y] == space)
				{
					int f = 0;

					f = 0;
					{
						for(int a = -1; a < 2; a++)
							for(int b = -1; b < 2; b++)
								if((a != 0 || b != 0) && a * b == 0)
									if(
										buff[x + a][y + b] == space
										)
									{
										f += 4;
									}
									else if(
										x + a == 0 ||
										x + a == mx - 1 ||
										y + b == 0 ||
										y + b == my - 1
										)
									{
										f += 4;
									}
					}
					if(f > 16)
						buff[x][y] = wall;

					f = 0;

					{
						for(int a = -1; a < 2; a++)
							for(int b = -1; b < 2; b++)
								if((a != 0 || b != 0))
									if(
										buff[x + a][y + b] == space
										)
									{
										f+=4;
									}
									else if( 
										x + a == 0 || 
										x + a == mx - 1 || 
										y + b == 0 || 
										y + b == my - 1
										)
									{
										f+=2;
									}
					}
					if(f > 25)
						buff[x][y] = wall;

					if(x > 1 && y > 1 && x < mx - 2 && y < my - 2)
					{
						f = 0;
						{
							for(int a = -2; a < 3; a++)
								for(int b = -2; b < 3; b++)
									if((a != 0 || b != 0))
										if(
											buff[x + a][y + b] == space
											)
										{
											f += 4;
										}
										else if(
											x + a == 0 ||
											x + a == mx - 1 ||
											y + b == 0 ||
											y + b == my - 1
											)
										{
											f += 4;
										}
						}
						if(f > 90)
							buff[x][y] = wall;
					}

					if(buff[x][y] == wall)
					{
						//Print(mx, my, buff);
						g = 0;
					}

				}
				k = (k == -1 || rand() % 2 == 0) ? rand() % 4 : k;
				switch(k)
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

				if(g == 5000)
					i = 1e10;
			}
			tx = rand() % mx, ty = rand() % my;
			while(buff[tx][ty] != space)
			{
				tx = rand() % mx; ty = rand() % my;
			}
			buff[tx][ty] = target;
			buff[ax][ay] = space;
			ax = rand() % mx; ay = rand() % my;
			while(buff[ax][ay] != space)
			{
				ax = rand() % mx; ay = rand() % my;
			}
			buff[ax][ay] = ant;
		}
		__T__++;
		buff[ax][ay] = space;
		int d = abs(ax - tx) > abs(ay - ty) ? (ax - tx > 0 ? 0 : 1) : (ay - ty > 0 ? 2 : 3),
			t1 = (ax - tx > 0 ? 0 : 1),
			t2 = (ay - ty > 0 ? 0 : 1),
			s =
				t1 * 64 +
				t2 * 32 +
				d * 16 +
				(buff[ax + 1][ay] == space) * 8 +
				(buff[ax - 1][ay] == space) * 4 +
				(buff[ax][ay + 1] == space) * 2 +
				(buff[ax][ay - 1] == space) * 1;
		float r = (ax - tx) * (ax - tx) + (ay - ty) * (ay - ty);		
		k = rand() % 2 == 0 ? Move.MaxA(s) : k;
		if(__Y__ < 8)
		{
			s = 
				t1 * 64 +
				t2 * 32 +
				d * 16 +
				(rand() % 16);
		}
		if(__Y__ < 8)
		{
			k = rand() % 4;
		}
		bool iswal = true, istarg = false;

		switch(k)
		{
			case 0:
				iswal = buff[ax + 1][ay] == wall;
				if(!iswal)ax++;
				break;
			case 1:
				iswal = buff[ax -1][ay] == wall;
				if(!iswal)ax--;
				break;
			case 2:
				iswal = buff[ax][ay + 1] == wall;
				if(!iswal)ay++;
				break;
			case 3:
				iswal = buff[ax][ay - 1] == wall;
				if(!iswal)ay--;
				break;
			default:
				break;
		}

		switch(k)
		{
			case 0:
				istarg = buff[ax + 1][ay] == target;
				break;
			case 1:
				istarg = buff[ax -1][ay] == target;
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
		if(istarg || r < 1 || nr < 1)
		{
			__T__ = 0;
			continue;
		}
		r = sqrtf(r);
		nr = sqrtf(nr);

		int nd = abs(ax - tx) > abs(ay - ty) ? (ax - tx > 0 ? 0 : 1) : (ax - tx > 0 ? 2 : 3),
			ns =
			d * 16 +
			(buff[ax + 1][ay] == space) * 8 +
			(buff[ax - 1][ay] == space) * 4 +
			(buff[ax][ay + 1] == space) * 2 +
			(buff[ax][ay - 1] == space) * 1;

		if(iswal)
		{
			if(__Y__ > 5)
			if(Move.Q[s][k] > -1000)
				Move.Update(s, k, -1000000);
		}
		else if(nr < r)
		{
			Move.Update(s, k, 0);
		}
		else
			Move.Update(s, k, 0);

		Move.Update(s, k, r - nr);

		
		buff[ax][ay] = ant;
		if(!iswal)
		{
			COORD position;
			CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
			GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
			std::string cl = "";
			for(int w = mx; w < consoleInfo.srWindow.Right; w++)
			{
				cl += ' ';
			}
			for(int q = 0; q < 10; q++)
			{
				position.X = mx;
				position.Y = q;
				SetConsoleCursorPosition(hConsole, position);
				std::cout << cl;
			}

			position.X = mx;
			position.Y = 0;
			SetConsoleCursorPosition(hConsole, position);
			std::cout << "dir " << k << " state " << s << ' ' << __T__ / float(__K__);
			position.X = mx;
			position.Y = 1;
			SetConsoleCursorPosition(hConsole, position);
			switch(k)
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
			for(int q = 0; q < 4; q++)
			{
				position.X = mx;
				position.Y = q + 2;
				SetConsoleCursorPosition(hConsole, position);
				switch(q)
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
				std::cout << Move.Q[s][q] << ' ';
			}
			//std::this_thread::sleep_for(std::chrono::milliseconds(10));

			Print(mx, my, buff);
		}
	}


}


void Print(int mx, int my, char** buff)
{
	if(buff == 0) return;
	COORD position;										// Объявление необходимой структуры
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);	// Получение дескриптора устройства стандартного вывода
	position.X = 0;									// Установка координаты X
	position.Y = 0;									// Установка координаты Y
	SetConsoleCursorPosition(hConsole, position);		// Перемещение каретки по заданным координатам
	std::string Sbuff = "";
	for(int y = 0; y < my; y++)
	{
		for(int x = 0; x < mx; x++)
		{
			if(buff[x] != 0)
			{
				if(buff[x][y] != '\n')
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
