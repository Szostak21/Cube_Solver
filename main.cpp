
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

void rotateClockwise(char cube[6][3][3], int face) {
    char temp[3];
    
    for (int i = 0; i < 3; ++i) {
        temp[i] = cube[face][0][i];
    }
    
    for (int i = 0; i < 3; ++i) {
        cube[face][0][i] = cube[face][2 - i][0];
    }
    
    for (int i = 0; i < 3; ++i) {
        cube[face][2 - i][0] = cube[face][2][2 - i];
    }
    
    for (int i = 0; i < 3; ++i) {
        cube[face][2][2 - i] = cube[face][i][2];
    }
    
    for (int i = 0; i < 3; ++i) {
        cube[face][i][2] = temp[i];
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
    char cube[6][3][3];
    
    load(filename, cube);
    rotateClockwise(cube, 0);
    print_cube(cube);
    return 0;
}