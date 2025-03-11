
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

void load(const string& input, char cube[6][3][3]) {
    ifstream File(input);
    
    if (File.is_open()) {
        for (int face = 0; face < 6; ++face) {
            for (int row = 0; row < 3; ++row) {
                for (int col = 0; col < 3; ++col) {
                    File >> cube[face][row][col];
                }
            }
        }
        File.close();
    } else {
        cout << "Unable to open file";
    }
}

int main() {
    const string filename = "input.txt";
    char cube[6][3][3];
    
    load(filename, cube);
    
    for (int face = 0; face < 6; ++face) {
        for (int row = 0; row < 3; ++row) {
            for (int col = 0; col < 3; ++col) {
                cout << cube[face][row][col] << ' ';
            }
            cout << endl;
        }
        cout << endl;
    }
    
    return 0;
}
