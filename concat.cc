#include <iostream>
#include <vector>
using namespace std;

vector<int> concatenateVectors(const vector<int>& vec1, const vector<int>& vec2, const vector<int>& vec3) {
    // Calculate the size of the resulting vector
    size_t resultSize = vec1.size() + vec2.size() + vec3.size();

    // Create a new vector with the size of the concatenation
    vector<int> resultVector(resultSize);

    // Copy elements from the first vector
    copy(vec1.begin(), vec1.end(), resultVector.begin());

    // Copy elements from the second vector
    copy(vec2.begin(), vec2.end(), resultVector.begin() + vec1.size());

    // Copy elements from the third vector
    copy(vec3.begin(), vec3.end(), resultVector.begin() + vec1.size() + vec2.size());

    return resultVector;
}

int main() {
    // Example usage
    vector<int> vec1 = {1, 2, 3, 4, 5, 6};
    vector<int> vec2 = {4, 5, 6, 7, 8, 9};
    vector<int> vec3 = {7, 8, 9, 10, 11, 12};

    vector<int> result = concatenateVectors(vec1, vec2, vec3);

    // Print the result
    cout << "Concatenated vector: ";
    for (int num : result) {
        cout << num << " ";
    }

    return 0;
}
