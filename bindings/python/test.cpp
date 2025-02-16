#include <iostream>

extern "C" __declspec (dllexport) int add(int i, int j) {
    return i + j;
}