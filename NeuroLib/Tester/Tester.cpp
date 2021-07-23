// Tester.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>

#include "../NeuroLib/framework.h"

#include <vector>

#include <thread>

#include "../NeuroLib/LayerMtp.h"

#include "../NeuroLib/Layer.h"

int main()
{
    std::vector<INeuro*> neuro;

    double neurons[100];
    double err[100];

    neuro.push_back(new Layer(3, 10, &neurons[0], &neurons[3], &err[0], &err[3]));
    neuro.push_back(new LayerMtp(10, 1, &neurons[3], &neurons[13], &err[3], &err[13]));
    auto L1 = ((Layer*)neuro[0]); L1 ->LF = 0.0001;
    auto L2 = ((LayerMtp*)neuro[1]); L2->LF = 0.0001;
    L1->SetFunc(F_SGATE, DF_SGATE);
    double serr = 0, C = 0;

    double train[1000][4];
    for (int i = 0; i < 1000; i++)
    {
        double v = rand() / double(RAND_MAX) * 4 - 2;
        double x1 = rand() / double(RAND_MAX) * 20 - 10;
        double dt = rand() / double(RAND_MAX) * 0.99 + 0.01;
        double x2 = x1 + v * dt;
        train[i][0] = x1;
        train[i][1] = x2;
        train[i][2] = dt;
        train[i][3] = v;

    }

    for (int i = 0; i < 10000000; i++)
    {
        double v = rand() / double(RAND_MAX) * 4 - 2;
        double x1 = rand() / double(RAND_MAX) * 20 - 10;
        double dt = rand() / double(RAND_MAX) * 0.99 + 0.01;
        double x2 = x1 + v * dt;

        if (i % 200000 < 100000)
        {
            x1 = train[i % 1000][0];
            x2 = train[i % 1000][1];
            dt = train[i % 1000][2];
            v  = train[i % 1000][3];
        }

        neurons[0] = x1;
        neurons[1] = x2;
        neurons[2] = dt;
        for (int i = 0; i < neuro.size(); i++)
        {
            neuro[i]->Calc();
        }
        double terr = v - neurons[13];
        err[13] = terr;
        for (int i = neuro.size() - 1; i >= 0 ; i--)
        {
            neuro[i]->Train();
        }
        serr += terr * terr;
        C++;
        if (i % 100 == 0)
        {
            system("cls");

            for (int i = 0; i < L1->ins + 1; i++, std::cout << '\n')
            {
                std::cout << (i < L1->ins ? L1->in[i] : 1) << " : ";
                for (int o = 0; o < L1->outs; o++)
                {
                    std::cout << L1->links[o + i * L1->outs] << ' ';
                }
            }
            std::cout << '\n';

            for (int i = 0; i < L2->ins + 1; i++, std::cout << '\n')
            {
                std::cout << (i < L2->ins ? L2->in[i] : 1) << " : ";
                for (int o = 0; o < L2->outs; o++)
                {
                    std::cout << L2->links1[o + i * L2->outs] << ' ';
                }
            }
            std::cout << '\n';

            for (int i = 0; i < L2->ins + 1; i++, std::cout << '\n')
            {
                std::cout << (i < L2->ins ? L2->in[i] : 1) << " : ";
                for (int o = 0; o < L2->outs; o++)
                {
                    std::cout << L2->links2[o + i * L2->outs] << ' ';
                }
            }
            std::cout << '\n';

            std::cout << serr / C << '\n';

            C = 0;
            serr = 0;
            system("pause");
            //std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }

    }
}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
