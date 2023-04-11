#include <omp.h>

#include <bitset>
#include <iostream>
#include <list>
#include <random>
#include <vector>

union convDouble {
    double d;
    unsigned char ch[8];
};

struct DCArray {
    int size;
    convDouble *data;

    DCArray(int _size = 0) : size(_size) { data = new convDouble[size]; }

    DCArray getPart(int first, int last) {
        if ((last > size) || (first < 0)) {
            return DCArray();
        }

        int size = last - first;
        DCArray result(size);

        for (int i = 0; i < size; i++) {
            result.data[i] = data[first + i];
        }

        return result;
    }

    bool isSorted() {
        for (int i = 1; i < size; i++) {
            if (data[i - 1].d > data[i].d) return false;
        }
        return true;
    }

    void setRandValue(double lower_bound, double upper_bound) {
        for (int i = 0; i < size; i++) {
            std::random_device rd;
            std::mt19937 gen(rd());
            int r = static_cast<int>(gen());

            data[i].d =
                lower_bound + ((upper_bound - lower_bound) * r / RAND_MAX);
        }
    }

    void out() {
        std::cout << "{";
        for (int i = 0; i < size; i++) {
            std::cout << data[i].d << " ";
        }
        std::cout << "}\n";
    }

    DCArray(const DCArray &leftVal) : size(leftVal.size) {
        data = new convDouble[size];
        for (int i = 0; i < size; i++) {
            data[i] = leftVal.data[i];
        }
    }

    DCArray &operator=(const DCArray leftVal) {
        size = leftVal.size;
        data = new convDouble[size];
        for (int i = 0; i < size; i++) {
            data[i] = leftVal.data[i];
        }
        return *this;
    }

    DCArray(DCArray &&leftVal) : size(leftVal.size), data(leftVal.data) {
        leftVal.data = nullptr;
    }

    ~DCArray() { delete[] data; }
};

void RadixSort(DCArray *array) {
    std::vector<std::vector<convDouble>> cArr(256);
    int digit = 0;
    // first bait initialization
    for (int i = 0; i < array->size; i++) {
        int zn{static_cast<int>(array->data[i].ch[digit])};
        cArr.at(zn).push_back(array->data[i]);
    }

    digit++;
    for (; digit <= 7; digit++) {
        std::vector<std::vector<convDouble>> reachVector(256);
        for (int i = 0; i < 256; i++) {
            for (auto j : cArr.at(i)) {
                int zn{static_cast<int>(j.ch[digit])};
                reachVector.at(zn).push_back(j);
            }
        }
        cArr = reachVector;
    }
    std::list<convDouble> ResultList;
    for (int i = 0; i < 256; i++) {
        for (auto j : cArr.at(i)) {
            int zn = static_cast<int>(j.ch[7]);
            if (zn & 128)
                ResultList.push_front(j);
            else
                ResultList.push_back(j);
        }
    }

    // convertation
    int i = 0;
    for (auto it : ResultList) {
        array->data[i] = it;
        i++;
    }
}

DCArray Merge(const DCArray &left, const DCArray &right) {
    DCArray result(left.size + right.size);
    int i = 0;
    int j = 0;
    int k = 0;
    while (i < left.size && j < right.size) {
        if (left.data[i].d < right.data[j].d) {
            result.data[k] = left.data[i];
            i++;
        } else {
            result.data[k] = right.data[j];
            j++;
        }
        k++;
    }
    while (i < left.size) {
        result.data[k] = left.data[i];
        k++;
        i++;
    }
    while (j < right.size) {
        result.data[k] = right.data[j];
        k++;
        j++;
    }

    return result;
}

void OMPRadixSort(DCArray *array, int threads_nom) {
    int delta = array->size / threads_nom;
    int ost = array->size % threads_nom;

    omp_set_num_threads(threads_nom);

    DCArray result(0);

#pragma omp parallel
    {
        int threadNom = omp_get_thread_num();

        int leftArrBound;
        int rightArrBound;

        if (threadNom == 0) {
            leftArrBound = 0;
            rightArrBound = delta + ost;
        } else {
            leftArrBound = (delta * threadNom) + ost;
            rightArrBound = leftArrBound + delta;
        }

        DCArray local_arr = array->getPart(leftArrBound, rightArrBound);

        RadixSort(&local_arr);

#pragma omp critical
        result = Merge(result, local_arr);
    }

    for (int i = 0; i < array->size; i++) {
        array->data[i] = result.data[i];
    }
}

int main() {
    DCArray a(1000000);
    a.setRandValue(-10, 10);
    a.out();
    OMPRadixSort(&a, 8);
    a.out();
    std::cout<<a.isSorted();
}