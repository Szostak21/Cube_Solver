using namespace std;

void U(char cube[6][3][3]){
    char temp[3];
    for (int i = 0; i < 3; ++i) {
        temp[i] = cube[0][0][i];
    }
    for (int i = 0; i < 3; ++i) {
        cube[0][0][i] = cube[0][2 - i][0];
    }
    for (int i = 0; i < 3; ++i) {
        cube[0][i][0] = cube[0][2][i];
    }
    for (int i = 0; i < 3; ++i) {
        cube[0][2][i] = cube[0][2 - i][2];
    }
    for (int i = 0; i < 3; ++i) {
        cube[0][i][2] = temp[i];
    }

    for (int i = 0; i < 3; ++i) {
        temp[i] = cube[1][0][i];
    }
    for (int i = 0; i < 3; ++i) {
        cube[1][0][i] = cube[2][0][i];
    }
    for (int i = 0; i < 3; ++i) {
        cube[2][0][i] = cube[3][0][i];
    }
    for (int i = 0; i < 3; ++i) {
        cube[3][0][i] = cube[4][0][i];
    }
    for (int i = 0; i < 3; ++i) {
        cube[4][0][i] = temp[i];
    }
}