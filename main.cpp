#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>
#include <raylib.h>
#include <string>
#include <utility>
#include <vector>

using namespace std;

const int rows = 15;
const int cols = 10;
const int cell_size = 30;
int offset = 75;

int screen_width = (cell_size * cols) + (2 * offset) + 50;
int screen_height = (cell_size * rows) + (2 * offset) + 100;

const int alphabet_size = 30;
string alphabet[alphabet_size] = {"A", "B", "C", "D", "E", "F", "G", "H", "I", 
                                  "J", "K", "L", "M", "N", "O", "P", "Q", "R", 
                                  "S", "T", "U", "V", "W", "X", "Y", "Z", 
                                  "Ä", "Ö", "Ü", "ß"};

Color black = Color{0, 0, 0, 255};
Color white = Color{255, 255, 255, 255};

typedef struct {
    string letter;
    bool updated;
} letter_type;

class Board
{

public:
    letter_type board[rows][cols];
    vector<pair<int, int>> directions = {{-1, -1}, {0, -1}, {1, -1}, 
                                         {-1, 0},           {1, 0},
                                         {-1, 1},  {0, 1},  {1, 1}};

    void fill_in_board(vector<string>& word_list)
    {
        // set up all board
        for (int i = 0; i < rows; i++) 
        {
            for (int j = 0; j < cols; j++)
            {
                letter_type& cur = board[i][j];
                // cur.letter = alphabet[GetRandomValue(0, alphabet_size - 1)];
                cur.letter = "";
                cur.updated = false;
            }
        }

        // add words
        int size = word_list.size();
        for (int w = 0; w < size; w++) 
        {
            string curr_word = word_list.at(w);
            int len_word = curr_word.length();

            int row = 0;
            int col = 0;

            vector<pair<int, int>> possible_directions;

            int dir = 0;
            bool valid = false;
            pair<int,int> direction;
            int attempts = 0;

            do
            {
                row = GetRandomValue(0, rows-1);
                col = GetRandomValue(0, cols-1);
                possible_directions = find_directions(size, row, col);
                dir = 0;

                if (possible_directions.empty()) continue;
                
                do 
                {
                    direction = possible_directions.at(dir);
                    valid = is_valid_space(curr_word, len_word, row, col, direction);
                    dir++;
                } while ((!valid && dir < possible_directions.size()));

                attempts++;
            }
            while (!valid && attempts < 100);

            if (attempts >= 100) 
            {
                word_list.erase(word_list.begin() + w);
                size = word_list.size();
            } 
            else 
            {
                // move in correct direction
                for (int v = 0; v < len_word; v++) 
                {
                    letter_type& cur = board[row + (v * direction.first)][col + (v * direction.second)];
                    cur.letter = curr_word[v];
                    cur.updated = true;
                }

                cout << curr_word << endl;
            }
        }
    }

    bool is_valid_space(string word, int len, int row, int col, pair<int,int> direction)
    {

        // loop through location of word
        for (int i = 0; i < len; i++)
        {
            // check if board access is out bounds bounds
            int r = row + (i * direction.first);
            int c = col + (i * direction.second);
            if (r < 0 || r >= rows) return false;
            if (c < 0 || c >= cols) return false;

            // check if location has updated = true
            letter_type& cur = board[r][c];
            if (cur.updated) 
            {
                // check if letters are the same
                if (cur.letter[0] != word[i]) 
                {
                    return false;
                }
            }
        }

        return true;
    }

    vector<pair<int, int>> find_directions(int len, int row, int col)
    {
        vector<pair<int, int>> possible_direcs = directions;

        int end_row_pos = row + len;
        int end_col_pos = col + len;
        
        int end_row_neg = row - len;
        int end_col_neg = col - len;
        
        if (end_row_pos >= rows) // past bottom wall boundary 
        {
            possible_direcs.erase(
                remove(possible_direcs.begin(), possible_direcs.end(), pair<int, int>{1, 0}), 
                possible_direcs.end());
        }
        if (end_row_neg <= 0) // past top wall boundary
        {
            possible_direcs.erase(
                remove(possible_direcs.begin(), possible_direcs.end(), pair<int, int>{-1, 0}), 
                possible_direcs.end());
        }
        if (end_col_pos >= cols) // past right wall boundary 
        {
            possible_direcs.erase(
                remove(possible_direcs.begin(), possible_direcs.end(), pair<int, int>{0, 1}), 
                possible_direcs.end());
        }
        if (end_col_neg <= 0) // past left wall boundary 
        {
            possible_direcs.erase(
                remove(possible_direcs.begin(), possible_direcs.end(), pair<int, int>{0, -1}), 
                possible_direcs.end());
        }
        
        if (end_row_pos >= rows && end_col_pos >= cols) // past bottom right corner
        {
            possible_direcs.erase(
                remove(possible_direcs.begin(), possible_direcs.end(), pair<int, int>{1, 1}), 
                possible_direcs.end());
        }
        if (end_row_pos >= rows && end_col_neg <= 0) // past bottom left corner
        {
            possible_direcs.erase(
                remove(possible_direcs.begin(), possible_direcs.end(), pair<int, int>{1, -1}), 
                possible_direcs.end());
        }
        if (end_row_neg <= 0 && end_col_pos >= cols) // past top right corner
        {
            possible_direcs.erase(
                remove(possible_direcs.begin(), possible_direcs.end(), pair<int, int>{-1, 1}), 
                possible_direcs.end());
        }
        if (end_row_neg <= 0 && end_col_neg <= 0) // past top left corner
        {
            possible_direcs.erase(
                remove(possible_direcs.begin(), possible_direcs.end(), pair<int, int>{-1, -1}), 
                possible_direcs.end());
        }
        
        shuffle(possible_direcs.begin(), possible_direcs.end(), 
                                         mt19937(random_device{}()));
        return possible_direcs;
    }

    void draw()
    {
        for (int i = 0; i < rows; i++) 
        {
            for (int j = 0; j < cols; j++)
            {
                letter_type cur = board[i][j];
                DrawText(cur.letter.c_str(), 
                         (j * cell_size) + offset + 25, 
                         (i * cell_size) + offset + 75, 
                         cell_size - 5, black);
            }
        }
    }
};


class WordsToFind
{

public:

    void draw(vector<string>& word_list)
    {
        int len = word_list.size();
        int offset_width = 0;
        int offset_height = 0;
        int size = cell_size - 10;

        for (int w = 0; w < len; w++)
        {
            int width = 100 + offset_width;
            int height = 50 + offset_height;
            
            string curr = word_list[w];
            DrawText(curr.c_str(), width, height, size, black);

            offset_width += (curr.length() * (cell_size/2)); 
            if (width >= screen_width - 25 - size - offset_width) 
            {
                offset_height += size + 5;
                offset_width = 0;
            }
        }
    }

};



vector<string> read_word_list(string filename)
{
    string word;
    vector<string> word_list;

    ifstream word_list_file(filename);

    if (!word_list_file.is_open()) {
        cerr << "Error: Could not open the file." << std::endl;
        return {};
    }

    while (getline(word_list_file, word)) {
        word_list.push_back(word);
    }

    word_list_file.close();

    return word_list;
}

vector<string> select_words(vector<string> word_list) {
    int max_size = GetRandomValue(5, min(8, (int)word_list.size()));

    shuffle(word_list.begin(), word_list.end(), mt19937(random_device{}()));

    return vector<string>(word_list.begin(), word_list.begin() + max_size);
}


int main()
{

    vector<string> full_word_list = read_word_list("word_list.txt");
    if (full_word_list.empty()) {
        return 1;
    }
    
    vector<string> word_list = select_words(full_word_list);
    cout << "------------" << endl;
    for (int i = 0; i < word_list.size(); i++) {
        cout << word_list.at(i) << endl;
    }
    cout << "------------" << endl;

    InitWindow(screen_width, screen_height, "Word Search");
    SetTargetFPS(60);

    Board board = Board();
    board.fill_in_board(word_list);
    WordsToFind words_to_find = WordsToFind();

    while (WindowShouldClose() == false) 
    {
        BeginDrawing();

        ClearBackground(white);

        board.draw();
        words_to_find.draw(word_list);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
