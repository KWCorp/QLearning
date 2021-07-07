// DicretAnts.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <string>
#include <thread>

void Print(int mx, int my, char** buff);


int main()
{
	int mx = 50;
	int my = mx;
	char** buff = new char*[mx];
	for(int x = 0; x < mx; x++)
		buff[x] = new char[my];
	for(int y = 0; y < my; y++)
		for(int x = 0; x < mx; x++)
			buff[x][y] = ' ';
	
}


void Print(int mx, int my, char** buff)
{
	if(buff == 0) return;
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
	system("cls");
	std::cout << Sbuff;
}
