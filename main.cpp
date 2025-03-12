
#include <iostream>
#include <fstream>
#include <string>
#include "moves.cpp"
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

void print_cube(char cube[6][3][3]) {
    for (int row = 0; row < 3; ++row) {
        cout << "        ";
        for (int col = 0; col < 3; ++col) {
            cout << cube[0][row][col] << ' ';
        }
        cout << endl;
    }
    cout << endl;
    for (int row = 0; row < 3; ++row) {
        for (int face = 0; face < 4; ++face) {

            for (int col = 0; col < 3; ++col) {
                if (face == 0){
                    face = 4;
                    cout << cube[face][row][col] << ' ';
                    face = 0;
                }
                else{
                    cout << cube[face][row][col] << ' ';
                }
            }
            cout << "  ";
        }
        cout << endl;
    }
    cout << endl;
    for (int row = 0; row < 3; ++row) {
        cout << "        ";
        for (int col = 0; col < 3; ++col) {
            cout << cube[5][row][col] << ' ';
        }
        cout << endl;
    }
    cout << endl;
}

int main() {
    const string filename = "input.txt";
    static char cube[6][3][3];
    static char goal[6][3][3] = {
        {{'Y', 'Y', 'Y'}, {'Y', 'Y', 'Y'}, {'Y', 'Y', 'Y'}},
        {{'O', 'O', 'O'}, {'O', 'O', 'O'}, {'O', 'O', 'O'}},
        {{'B', 'B', 'B'}, {'B', 'B', 'B'}, {'B', 'B', 'B'}},
        {{'R', 'R', 'R'}, {'R', 'R', 'R'}, {'R', 'R', 'R'}},
        {{'G', 'G', 'G'}, {'G', 'G', 'G'}, {'G', 'G', 'G'}},
        {{'W', 'W', 'W'}, {'W', 'W', 'W'}, {'W', 'W', 'W'}}
    };
    
    load(filename, cube);
    U(cube);
    print_cube(cube);
    return 0;
}