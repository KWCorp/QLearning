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
    for (int i = 0; i < 100; i++)
    {
        neurons[i] = 0;
    }
    //neuro.push_back(new LayerMtp(3, 10, &neurons[0], &neurons[3], &err[0], &err[3]));
    //neuro.push_back(new Layer(10, 10, &neurons[3], &neurons[13], &err[3], &err[13]));
    //neuro.push_back(new Layer(10, 1, &neurons[13], &neurons[23], &err[13], &err[23]));

    int inp = 0, inc = 3, outp = inp + inc, outc = 1;

    int c = 10, co = inc, start = outp + outc;
    neuro.push_back(new Layer(co, c, &neurons[inp], &neurons[start], &err[inp], &err[start]));
    co = c;
    c = 5;
    neuro.push_back(new Layer(co, c, &neurons[start], &neurons[start + co], &err[start], &err[start + co]));
    neuro.push_back(new LayerMtp(co, c, &neurons[start], &neurons[start + co + c], &err[start], &err[start + co + c]));

    start += co;
    co = c * 2;
    c = 5;

    neuro.push_back(new Layer(co, c, &neurons[start], &neurons[start + co], &err[start], &err[start + co]));
    neuro.push_back(new LayerMtp(co, c, &neurons[start], &neurons[start + co + c], &err[start], &err[start + co + c]));

    start += co;
    co = c * 2;
    c = outc;

    neuro.push_back(new Layer(co, c, &neurons[start], &neurons[outp], &err[start], &err[outp]));

    auto L1 = ((Layer*)neuro[0]); L1 ->LF = 0.0001;
    auto L2 = ((Layer*)neuro[1]); L2->LF = 0.0001;
    auto L3 = ((LayerMtp*)neuro[2]); L3->LF = 0.000001;
    auto L4 = ((Layer*)neuro[3]); L4->LF = 0.00001;
    auto L5 = ((LayerMtp*)neuro[4]); L5->LF = 0.000001;
    auto L6 = ((Layer*)neuro[5]); L6->LF = 0.00001;

    L3->SetFunc(F_GATE, DF_GATE);
    L5->SetFunc(F_GATE, DF_GATE);
    double serr = 0, C = 0;

    double train[8000][4];
    auto t_start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 8000; i++)
    {
        train[i][0] = (i % 20) / 20.0 * 20 - 10;
        train[i][1] = ((i / 20) % 20) / 20.0 * 20 - 10;
        train[i][2] = ((i / 400) % 20) / 20.0 * 4 + 1;
        train[i][3] = 1 * exp(- (train[i][0] * train[i][0] + train[i][1] * train[i][1]) / (train[i][2] * train[i][2]));

    }

    for (int i = 0; i < 100000000; i++)
    {
        double v = rand() / double(RAND_MAX) * 4 - 2;
        double x1 = rand() / double(RAND_MAX) * 20 - 10;
        double dt = rand() / double(RAND_MAX) * 0.99 + 0.01;
        double x2 = x1 + v * dt;

        if(1)
        {
            int j = i % 8000;
            x1 = train[j][0];
            x2 = train[j][1];
            dt = train[j][2];
            v = train[j][3];
        }

        neurons[0] = x1;
        neurons[1] = x2;
        neurons[2] = dt;
        for (int i = 0; i < neuro.size(); i++)
            neuro[i]->Calc();

        double terr = v - neurons[outp];
        err[outp] = terr;

        for (int i = 0; i < neuro.size(); i++)
            neuro[i]->ResetErr();

        for (int i = neuro.size() - 1; i >= 0 ; i--)
            neuro[i]->Train();


        C++;
        if (i % 100000 == 0)
        {
            auto t_end = std::chrono::high_resolution_clock::now();
            //system("cls");
            std::cout << std::scientific;

            //for (int i = 0; i < L1->ins + 1; i++, std::cout << '\n')
            //{
            //    std::cout << (i < L1->ins ? L1->in[i] : 1) << " : ";
            //    for (int o = 0; o < L1->outs; o++)
            //    {
            //        std::cout << L1->links1[o + i * L1->outs] << ' ';
            //    }
            //}
            //std::cout << '\n';
            //for (int i = 0; i < L1->ins + 1; i++, std::cout << '\n')
            //{
            //    std::cout << (i < L1->ins ? L1->in[i] : 1) << " : ";
            //    for (int o = 0; o < L1->outs; o++)
            //    {
            //        std::cout << L1->links2[o + i * L1->outs] << ' ';
            //    }
            //}
            //std::cout << '\n';
            //for (int i = 0; i < L2->ins + 1; i++, std::cout << '\n')
            //{
            //    std::cout << (i < L2->ins ? L2->in[i] : 1) << " : ";
            //    for (int o = 0; o < L2->outs; o++)
            //    {
            //        std::cout << L2->links[o + i * L2->outs] << ' ';
            //    }
            //}
            //std::cout << '\n';
            serr = 0;
            float KK = 0;
            for (int j = 0; j < 8000; j++)
            {
                {
                    x1 = train[j][0];
                    x2 = train[j][1];
                    dt = train[j][2];
                    v = train[j][3];
                }

                neurons[0] = x1;
                neurons[1] = x2;
                neurons[2] = dt;
                for (int i = 0; i < neuro.size(); i++)
                    neuro[i]->Calc();

                double terr = v - neurons[outp];
                err[outp] = terr;
                serr += terr * terr;
                KK++;
            }
            std::cout << serr / KK;
            std::cout << "," << std::chrono::duration<double, std::milli>(t_end - t_start).count() / C << ":ms" << std::endl;
            C = 0;
            serr = 0;
            //system("pause");
            //std::this_thread::sleep_for(std::chrono::milliseconds(200));
            t_start = std::chrono::high_resolution_clock::now();
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
