#include <iostream>
#include <fstream>

#include <vector>
#include <string>

#include <algorithm>

// Основная идея программы заключается в полном переборе
// вариантов расстановки слов.
//
// Под свободными словами подразумеваются слова,
// которые ещё не были добавлены на кроссворд.
//
// Под занятыми словами подразумеваются слова,
// которые уже были доабвлены на кроссворд.

using namespace std;

const int wordCount = 4;

// Словарь.
vector<string> words;

// Перечисление, обозначающее направление: V - вертикальное, H - горизонтальное.
enum Dir { V, H };

// Описатель занятого слова. Он хранит,
// где начинается слово на поле, его индекс в словаре,
// и ориентацию.
struct Descriptor {
    int row, col, ind;                                      // строка, столбец, индекс в словаре.
    Dir dir;                                                // направление.
};

// Функция сравнения длины двух слов.
bool lessThan(const string& lhs, const string& rhs) {
    return lhs.length() < rhs.length();
}

// Функция проверки, можно ли поместить слово word
// по координатам [row; col], направлению dir в поле field.
bool canPlace(string &word, int row, int col, Dir dir, vector<vector<char>> &field) {
    int &inc = dir == V ? row : col;                        // Ссылка на увеличивающуюся координату.
                                                            // Если мы пытаемся расположить слово
                                                            // вертикально, то нужно увеличивать ряд,
                                                            // а если горизонтально, то колонку.
    for (unsigned i = 0; i < word.length(); ++i, ++inc) {
        if (field[row][col] != ' ' && word[i] != field[row][col]) { // Если клетка поля занята и
                                                                    // буква, которая там стоит
                                                                    // не совпадает с i-ым символом слова,
            return false;                                           // то мы не можем поместить это слово.
        }
    }

    return true;
}

// Функция, которая помещает слово word
// по координатам [row; col], направлению dir в поле field.
void placeWord(string &word, int row, int col, Dir dir, vector<vector<char>> &field) {
    int &inc = dir == V ? row : col;
    for (unsigned i = 0; i < word.length(); ++i, ++inc) {
        field[row][col] = word[i];
    }
}

// Функция, которая находит пересечения двух слов.
vector<pair<int, int>> getIntersections(string &lhv, string &rhv) {
    vector<pair<int, int>> intersections;                   // Массив пересечений.
    for (unsigned i = 0; i < lhv.length(); ++i) {
        for (unsigned j = 0; j < rhv.length(); ++j) {
            if (rhv[j] == lhv[i]) {
                intersections.push_back({ i, j });
            }
        }
    }

    return intersections;
}

// Функция, которая печатает кроссворд на экране.
void printCrossword(vector<vector<char>> &field) {
    for (unsigned i = 0; i < field.size(); ++i) {
        for (unsigned j = 0; j < field[i].size(); ++j) {
            cout << field[i][j];
        }
        cout << endl;
    }
}

// Рекурсивная функция, которая возвращает указатель на кроссворд.
vector<vector<char>> *buildCrossword(vector<vector<char>> field,
                                     vector<int> freeWords, vector<Descriptor> usedWords) {
//    printCrossword(field);
    for (unsigned freeInd = 0; freeInd < freeWords.size(); ++freeInd) {         // Перебираем все свободные
        for (unsigned usedInd = 0; usedInd < usedWords.size(); ++usedInd) {     // и занятые слова.
            // Первое значение - индексе в занятом слове,
            // а второе - в свободном слове
            vector<pair<int, int>> intersections
                    = getIntersections(words[usedWords[usedInd].ind], words[freeWords[freeInd]]);

            Dir dir = usedWords[usedInd].dir == V ? H : V;                      // Определяем потенциальное
                                                                                // добавляемого слова.

            for (unsigned i = 0; i < intersections.size(); ++i) {                    // Перебираем все пересечения.
                // Вычисляем координаты, где находится пересечение.
                int row, col;
                if (dir == V) {
                    // Если ориентация вертикальна, то определяем строку как строку уже использованного
                    // слова минус отступ до пересечения внутри занятого слова; а колонку - как колонку
                    // уже использованного слова плюс отступ до пересечения в свободном слове.
                    row = usedWords[usedInd].row - intersections[i].second;
                    col = usedWords[usedInd].col + intersections[i].first;
                } else {
                    // Если ориентация горизонтальна, то определяем строку как строку уже использованного
                    // слова плюс отступ до пересечения в свободном слове; а колонку - как колонку
                    // уже использованного слова минус отступ до пересечения внутри занятого слова.
                    row = usedWords[usedInd].row + intersections[i].first;
                    col = usedWords[usedInd].col - intersections[i].second;
                }

                // Проверяем, можем ли мы поместить свободное слово по найденным ранее координатам.
                if (canPlace(words[freeWords[freeInd]], row, col, dir, field)) {
                    // Если можем, то создаём новое поле из старого и
                    // помещаем на него свободное слово.
                    vector<vector<char>> newField(field);
                    placeWord(words[freeWords[freeInd]], row, col, dir, newField);

                    // Создаём новый массив занятых слов и помещаем в него
                    // добавленное в кроссворд слово.
                    vector<Descriptor> newUsedWords(usedWords);
                    newUsedWords.push_back({ row, col, freeWords[freeInd], dir });

                    // Создаём новый массив свободных слов, убирая при этом
                    // только что добавленное в кроссворд слово.
                    vector<int> newFreeWords;
                    for (unsigned i = 0; i < freeWords.size(); ++i) {
                        if (i != freeInd) { newFreeWords.push_back(freeWords[i]); }
                    }

                    // Если новый массив пустых слов оказывается пуст, то
                    // это значит, что мы поместили все слова на кроссворд,
                    // а наш ответ содержится в newField.
                    if (newFreeWords.empty()) {
                        // Возвращаем указатель на копию newField.
                        return new vector<vector<char>>(newField);
                    } else {
                        // Если свободные слова ещё есть, то вызываем рекурсивно эту же функцию
                        // с новыми данными.
                        vector<vector<char>> *result = buildCrossword(newField, newFreeWords, newUsedWords);
                        if (result) {                                       // Если указатель не нулевой,
                            return result;                                  // то возвращаем его.
                        }
                    }
                }
            }
        }
    }

    // Возвращаем нулевой указатель, если решение не было построено.
    return nullptr;
}

// Функция считывания слов из файла.
void readData() {
    // Создаём поток ввода.
    ifstream input("words.txt");

    while (!input.eof()) {                                  // Пока не конец файла,
        string word;
        getline(input, word);                               // считываем строку из него и
        words.push_back(word);                              // кладём слово в конец словаря.
    }

    input.close();                                          // Заркываем файл.
}

int main() {
    setlocale(LC_ALL, "Russian");

    readData();

    sort(words.begin(), words.end(), lessThan);             // Сортируем массив слов по неубыванию их длин.

    // Размер изначального поля рассчитывается
    // как сумма длин всех слов.
    int fieldSize = 0;
    for (int i = 0; i < wordCount; ++i) {
        fieldSize += words[i].length();
    }

    int row, col;
    row = col = fieldSize / 2 - words[0].length() / 2;      // Координаты для первого слова.

    vector<int> freeWords;                                  // Массив свободных слов.
    for (unsigned i = 1; i < words.size(); ++i) {
        freeWords.push_back(i);
    }

    vector<Descriptor> usedWords;                           // Массив использованных слов.
    usedWords.push_back({ row, col, 0, H });                // "используем" первое слово.

    vector<vector<char>> startField(fieldSize, vector<char>(fieldSize, ' ')); // Начальное пустое поле.
    placeWord(words[0], row, col, H, startField);           // Помещаем первое слово на поле.

    // Строим кроссворд.
    vector<vector<char>> *result = buildCrossword(startField, freeWords, usedWords);

    if (result) {                                           // Если это удалось,
        printCrossword(*result);                            // выводим результат,
    } else {                                                // иначе
        cout << "Решение не найдено";                       // выводим текст.
    }

    system("pause");
    return 0;
}
