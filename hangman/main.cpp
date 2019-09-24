#include <iostream>
#include <vector>

using namespace std;

// as the game goes on, we need different people
constexpr struct {

    const char* empty = 
    "======\n"
    "|  |  \n"
    "|  O  \n"
    "|     \n"
    "|     \n"
    "|     \n"
    "/------\\\n";

    const char* one = 
    "======\n"
    "|  |  \n"
    "| (\") \n"
    "|     \n"
    "|     \n"
    "|     \n"
    "/------\\\n";

    const char* two = 
    "====== \n"
    "|  |   \n"
    "| (\") \n"
    "|  #   \n"
    "|      \n"
    "|      \n"
    "/------\\\n";

    const char* three = 
    "====== \n"
    "|  |   \n"
    "| (\") \n"
    "| /#   \n"
    "|      \n"
    "|      \n"
    "/------\\\n";

    const char* four = 
    "====== \n"
    "|  |   \n"
    "| (\") \n"
    "| /#\\ \n"
    "|      \n"
    "|      \n"
    "/------\\\n";

    const char* five =
    "====== \n"
    "|  |   \n"
    "| (\") \n"
    "| /#\\ \n"
    "| /    \n"
    "|      \n"
    "/------\\\n";

    const char* dead = 
    "====== \n"
    "|  |   \n"
    "| (\") \n"
    "| /#\\ \n"
    "| / \\ \n"
    "|      \n"
    "/------\\\n";

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

    if(argc != 2) {
        cout << "Usage:\n\t" << argv[0] << " <word to play>\n";
        return 1;
    }

    string input_str = argv[1];
    vector<char> char_vec, guessed_chars;

    while(char_vec.size() < input_str.size())
        char_vec.push_back('_');

    int level = 0;
    bool running = true;
    while(running) {

        // print the current board
        system("clear");
        print_character(level);
        cout << "\n\n>> ";
        char c;
        cin >> c;

        // now we have the character, see where it matches
        auto char_guessed = [&guessed_chars](char c) -> bool {
            for(char f : guessed_chars)
                if(f == c)
                    return true;
            return false;
        };

        auto print_current_string = [&char_vec]() -> void {
            for(char c : char_vec)
                cout << c << ' ';
            cout << endl;
        };

    }

    return 0;
}


