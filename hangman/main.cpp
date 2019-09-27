#include <iostream>
#include <vector>
#include <fstream>

using namespace std;

// as the game goes on, we need different people
constexpr struct {

    const char* empty = 
    "\n\n"
    "    ======\n"
    "    |  |  \n"
    "    |  O  \n"
    "    |     \n"
    "    |     \n"
    "    |     \n"
    "    /------\\\n";

    const char* one = 
    "\n\n"
    "    ======\n"
    "    |  |  \n"
    "    | (\") \n"
    "    |     \n"
    "    |     \n"
    "    |     \n"
    "    /------\\\n";

    const char* two = 
    "\n\n"
    "    ====== \n"
    "    |  |   \n"
    "    | (\") \n"
    "    |  #   \n"
    "    |      \n"
    "    |      \n"
    "    /------\\\n";

    const char* three = 
    "\n\n"
    "    ====== \n"
    "    |  |   \n"
    "    | (\") \n"
    "    | /#   \n"
    "    |      \n"
    "    |      \n"
    "    /------\\\n";

    const char* four = 
    "\n\n"
    "    ====== \n"
    "    |  |   \n"
    "    | (\") \n"
    "    | /#\\ \n"
    "    |      \n"
    "    |      \n"
    "    /------\\\n";

    const char* five =
    "\n\n"
    "    ====== \n"
    "    |  |   \n"
    "    | (\") \n"
    "    | /#\\ \n"
    "    | /    \n"
    "    |      \n"
    "    /------\\\n";

    const char* dead = 
    "\n\n"
    "    ====== \n"
    "    |  |   \n"
    "    | (\") \n"
    "    | /#\\ \n"
    "    | / \\ \n"
    "    |      \n"
    "    /------\\\n";

} reps;

// level is from 0-6 inclusively
void print_character(int level) {

    switch(level) {
        case 0: cout << reps.empty; break;
        case 1: cout << reps.one; break;
        case 2: cout << reps.two; break;
        case 3: cout << reps.three; break;
        case 4: cout << reps.four; break;
        case 5: cout << reps.five; break;
        case 6: cout << reps.dead; break;
        default: throw runtime_error("unknown representation");
    }

}

int main(int argc, char* argv[]) {

    string input_str;

    if(argc == 2) {
        input_str = argv[1];
    }
    else if(argc == 1) {
        // read a random word
        srand(time(NULL));
        int ch = rand() % 370102; // text file located in this directory

        ifstream in("words_alpha.txt");

        for(int i = 0; i < ch-1; i++)
            in >> input_str;

        in >> input_str;

    }
    else {
        cout << "Usage:\n\t" << argv[0] << " <word to play>\n";
        return 1;
    }

    vector<char> char_vec, guessed_chars;

    while(char_vec.size() < input_str.size())
        char_vec.push_back('_');

    // now we have the character, see where it matches
    auto char_guessed = [&guessed_chars](char c) -> bool {
        for(char f : guessed_chars)
            if(f == c)
                return true;
        return false;
    };

    auto print_current_string = [&char_vec]() -> void {
        cout << "string:  ";
        for(char c : char_vec)
            cout << c << ' ';
        cout << endl;
    };

    auto print_guessed_chars = [&guessed_chars]() -> void {
        cout << "guesses: ";
        for(char c : guessed_chars)
            cout << c << ' ';
        cout << endl;
    };

    auto did_win = [&char_vec]() -> bool {
        for(char c : char_vec)
            if(c == '_')
                return false;

        return true;
    };

    // return 1 if matches any chars in input string
    auto match_chars = [&](char c) -> int {
        int matched = 0;

        for(int i = 0; i < input_str.size(); i++) {
            if(c == input_str[i]) {
                char_vec[i] = c;
                matched = 1;
            }
        }

        return matched;
    };

    int level = 0;
    bool running = true;
    while(running) {

        // print the current board
        (void)system("clear");
        print_current_string();
        print_guessed_chars();

        print_character(level);
        cout << "\n\n>> ";
        char c;
        cin >> c;

        if(char_guessed(c))
            continue;

        guessed_chars.push_back(c);
        level += (1-match_chars(c));

        if(did_win()) {
            cout << "\nYou WIN!!!!!\n";
            cout << "The word was '" << input_str << "'\n";
            return 0;
        }
        else if(level > 6) {
            cout << "\nYou lose!\n";
            cout << "The word was '" << input_str << "'\n";
            return 0;
        }

    }

    return 0;
}


