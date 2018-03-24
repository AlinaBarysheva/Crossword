#include <iostream>
#include <fstream>

#include <vector>
#include <string>

#include <algorithm>

// �������� ���� ��������� ����������� � ������ ��������
// ��������� ����������� ����.
//
// ��� ���������� ������� ��������������� �����,
// ������� ��� �� ���� ��������� �� ���������.
//
// ��� �������� ������� ��������������� �����,
// ������� ��� ���� ��������� �� ���������.

using namespace std;

const int wordCount = 4;

// �������.
vector<string> words;

// ������������, ������������ �����������: V - ������������, H - ��������������.
enum Dir { V, H };

// ��������� �������� �����. �� ������,
// ��� ���������� ����� �� ����, ��� ������ � �������,
// � ����������.
struct Descriptor {
    int row, col, ind;                                      // ������, �������, ������ � �������.
    Dir dir;                                                // �����������.
};

// ������� ��������� ����� ���� ����.
bool lessThan(const string& lhs, const string& rhs) {
    return lhs.length() < rhs.length();
}

// ������� ��������, ����� �� ��������� ����� word
// �� ����������� [row; col], ����������� dir � ���� field.
bool canPlace(string &word, int row, int col, Dir dir, vector<vector<char>> &field) {
    int &inc = dir == V ? row : col;                        // ������ �� ��������������� ����������.
                                                            // ���� �� �������� ����������� �����
                                                            // �����������, �� ����� ����������� ���,
                                                            // � ���� �������������, �� �������.
    for (unsigned i = 0; i < word.length(); ++i, ++inc) {
        if (field[row][col] != ' ' && word[i] != field[row][col]) { // ���� ������ ���� ������ �
                                                                    // �����, ������� ��� �����
                                                                    // �� ��������� � i-�� �������� �����,
            return false;                                           // �� �� �� ����� ��������� ��� �����.
        }
    }

    return true;
}

// �������, ������� �������� ����� word
// �� ����������� [row; col], ����������� dir � ���� field.
void placeWord(string &word, int row, int col, Dir dir, vector<vector<char>> &field) {
    int &inc = dir == V ? row : col;
    for (unsigned i = 0; i < word.length(); ++i, ++inc) {
        field[row][col] = word[i];
    }
}

// �������, ������� ������� ����������� ���� ����.
vector<pair<int, int>> getIntersections(string &lhv, string &rhv) {
    vector<pair<int, int>> intersections;                   // ������ �����������.
    for (unsigned i = 0; i < lhv.length(); ++i) {
        for (unsigned j = 0; j < rhv.length(); ++j) {
            if (rhv[j] == lhv[i]) {
                intersections.push_back({ i, j });
            }
        }
    }

    return intersections;
}

// �������, ������� �������� ��������� �� ������.
void printCrossword(vector<vector<char>> &field) {
    for (unsigned i = 0; i < field.size(); ++i) {
        for (unsigned j = 0; j < field[i].size(); ++j) {
            cout << field[i][j];
        }
        cout << endl;
    }
}

// ����������� �������, ������� ���������� ��������� �� ���������.
vector<vector<char>> *buildCrossword(vector<vector<char>> field,
                                     vector<int> freeWords, vector<Descriptor> usedWords) {
//    printCrossword(field);
    for (unsigned freeInd = 0; freeInd < freeWords.size(); ++freeInd) {         // ���������� ��� ���������
        for (unsigned usedInd = 0; usedInd < usedWords.size(); ++usedInd) {     // � ������� �����.
            // ������ �������� - ������� � ������� �����,
            // � ������ - � ��������� �����
            vector<pair<int, int>> intersections
                    = getIntersections(words[usedWords[usedInd].ind], words[freeWords[freeInd]]);

            Dir dir = usedWords[usedInd].dir == V ? H : V;                      // ���������� �������������
                                                                                // ������������ �����.

            for (unsigned i = 0; i < intersections.size(); ++i) {                    // ���������� ��� �����������.
                // ��������� ����������, ��� ��������� �����������.
                int row, col;
                if (dir == V) {
                    // ���� ���������� �����������, �� ���������� ������ ��� ������ ��� ���������������
                    // ����� ����� ������ �� ����������� ������ �������� �����; � ������� - ��� �������
                    // ��� ��������������� ����� ���� ������ �� ����������� � ��������� �����.
                    row = usedWords[usedInd].row - intersections[i].second;
                    col = usedWords[usedInd].col + intersections[i].first;
                } else {
                    // ���� ���������� �������������, �� ���������� ������ ��� ������ ��� ���������������
                    // ����� ���� ������ �� ����������� � ��������� �����; � ������� - ��� �������
                    // ��� ��������������� ����� ����� ������ �� ����������� ������ �������� �����.
                    row = usedWords[usedInd].row + intersections[i].first;
                    col = usedWords[usedInd].col - intersections[i].second;
                }

                // ���������, ����� �� �� ��������� ��������� ����� �� ��������� ����� �����������.
                if (canPlace(words[freeWords[freeInd]], row, col, dir, field)) {
                    // ���� �����, �� ������ ����� ���� �� ������� �
                    // �������� �� ���� ��������� �����.
                    vector<vector<char>> newField(field);
                    placeWord(words[freeWords[freeInd]], row, col, dir, newField);

                    // ������ ����� ������ ������� ���� � �������� � ����
                    // ����������� � ��������� �����.
                    vector<Descriptor> newUsedWords(usedWords);
                    newUsedWords.push_back({ row, col, freeWords[freeInd], dir });

                    // ������ ����� ������ ��������� ����, ������ ��� ����
                    // ������ ��� ����������� � ��������� �����.
                    vector<int> newFreeWords;
                    for (unsigned i = 0; i < freeWords.size(); ++i) {
                        if (i != freeInd) { newFreeWords.push_back(freeWords[i]); }
                    }

                    // ���� ����� ������ ������ ���� ����������� ����, ��
                    // ��� ������, ��� �� ��������� ��� ����� �� ���������,
                    // � ��� ����� ���������� � newField.
                    if (newFreeWords.empty()) {
                        // ���������� ��������� �� ����� newField.
                        return new vector<vector<char>>(newField);
                    } else {
                        // ���� ��������� ����� ��� ����, �� �������� ���������� ��� �� �������
                        // � ������ �������.
                        vector<vector<char>> *result = buildCrossword(newField, newFreeWords, newUsedWords);
                        if (result) {                                       // ���� ��������� �� �������,
                            return result;                                  // �� ���������� ���.
                        }
                    }
                }
            }
        }
    }

    // ���������� ������� ���������, ���� ������� �� ���� ���������.
    return nullptr;
}

// ������� ���������� ���� �� �����.
void readData() {
    // ������ ����� �����.
    ifstream input("words.txt");

    while (!input.eof()) {                                  // ���� �� ����� �����,
        string word;
        getline(input, word);                               // ��������� ������ �� ���� �
        words.push_back(word);                              // ����� ����� � ����� �������.
    }

    input.close();                                          // ��������� ����.
}

int main() {
    setlocale(LC_ALL, "Russian");

    readData();

    sort(words.begin(), words.end(), lessThan);             // ��������� ������ ���� �� ���������� �� ����.

    // ������ ������������ ���� ��������������
    // ��� ����� ���� ���� ����.
    int fieldSize = 0;
    for (int i = 0; i < wordCount; ++i) {
        fieldSize += words[i].length();
    }

    int row, col;
    row = col = fieldSize / 2 - words[0].length() / 2;      // ���������� ��� ������� �����.

    vector<int> freeWords;                                  // ������ ��������� ����.
    for (unsigned i = 1; i < words.size(); ++i) {
        freeWords.push_back(i);
    }

    vector<Descriptor> usedWords;                           // ������ �������������� ����.
    usedWords.push_back({ row, col, 0, H });                // "����������" ������ �����.

    vector<vector<char>> startField(fieldSize, vector<char>(fieldSize, ' ')); // ��������� ������ ����.
    placeWord(words[0], row, col, H, startField);           // �������� ������ ����� �� ����.

    // ������ ���������.
    vector<vector<char>> *result = buildCrossword(startField, freeWords, usedWords);

    if (result) {                                           // ���� ��� �������,
        printCrossword(*result);                            // ������� ���������,
    } else {                                                // �����
        cout << "������� �� �������";                       // ������� �����.
    }

    system("pause");
    return 0;
}
