#include<iostream>

class IntArray {
    int* array;

public:
    IntArray(size_t n) {
        array = new int[n];
    }

	int operator [](size_t index) const {
        return array[index];
    }
};

int main() {
    std::cout << "Hi, welcome to Game Development Programming!" << std::endl;

    return 0;
}