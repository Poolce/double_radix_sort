#include <iostream>
#include "main.h"
#include <vector>
#include <list>
#include <random>
#include <bitset>

convDouble *SeqRadixSort(convDouble *arr, int size)
{
    std::vector<std::vector<convDouble>> cArr(256);
    int digit = 0;
    // first bait initialization
    for (int i = 0; i < size; i++)
    {
        int zn{static_cast<int>(arr[i].ch[digit])};
        cArr.at(zn).push_back(arr[i]);
    }
    digit++;

    // 1-7 bait
    for (; digit <= 7; digit++)
    {
        std::vector<std::vector<convDouble>> reachVector(256);
        for (int i = 0; i < 256; i++)
        {
            for (auto j : cArr.at(i))
            {
                int zn{static_cast<int>(j.ch[digit])};
                reachVector.at(zn).push_back(j);
            }
        }
        cArr = reachVector;
        std::cout<<digit<<"\n";
    }
    std::list<convDouble> resVec;
    for (int i = 0; i < 256; i++)
    {
        for (auto j : cArr.at(i))
        {
            int zn = static_cast<int>(j.ch[7]);
            if (zn & 128)
                resVec.push_front(j);
            else
                resVec.push_back(j);
        }
    }
    // convertation
    convDouble *res = new convDouble[size];
    int i = 0;
    for (auto it : resVec)
    {
        res[i] = it;
        i++;
    }
    std::cout<<digit<<"res\n";
    return res;
}

void RadixSort(double *arr, int size)
{
    convDouble *sortArr = new convDouble[size];
    for (int i = 0; i < size; i++)
        sortArr[i].d = arr[i];
    sortArr = SeqRadixSort(sortArr, size);
    for (int i = 0; i < size; i++)
        arr[i] = sortArr[i].d;
}

bool isSorted(double *arr, int size)
{
    for (int i = 1; i < size; i++)
        if (arr[i - 1] > arr[i])
        return false;
    return true;
}

void randDoubleArr(double *arr, int size, double lower_bound, double upper_bound)
{
    for (int i = 0; i < size; i++)
        arr[i] = lower_bound + (upper_bound - lower_bound) * (random() % RAND_MAX) / RAND_MAX;
}

void outArr(double *arr, int size)
{
    for (int i = 0; i < size; i++)
        std::cout << arr[i] << " ";
    std::cout << std::endl;
}

int main()
{
    int size = 60000000;
    double *arr = new double[size];
    randDoubleArr(arr, size, -200, 200);
    // outArr(arr, size);
    std::cout << "\n";
    RadixSort(arr, size);
    // outArr(arr, size);
    std::cout << "\n\n\n"<<(256&128);

    isSorted(arr, size)?std::cout<<"Right sorted\n":std::cout<<"ERROR not sorted\n";
}