// DicretAnts.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <string>
#include <thread>

#ifdef _WIN32

#include "Windows.h"

#endif

void Print(int mx, int my, char** buff);



int main()
{
	int mx = 50;
	int my = mx;
	char** buff = new char*[mx];
	for(int x = 0; x < mx; x++)
		buff[x] = new char[my];
	

	while(true)
	{
		for(int y = 0; y < my; y++)
			for(int x = 0; x < mx; x++)
				buff[x][y] = ' ';
		buff[mx - 1][my - 1] = '*';
		for(int y = 0; y < my; y++)
			for(int x = 0; x < mx; x++)
			{
				buff[x][y] = '0';
				Print(mx, my, buff);
				std::this_thread::sleep_for(std::chrono::microseconds(10));
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
	SetConsoleCursorPosition(hConsole, position);		// Перемещение каретки по заданным координатам
}
