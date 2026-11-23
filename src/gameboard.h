#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <set>
#include <algorithm>

class gameboard
{
public:
    gameboard(); // constructor that iniaitlizes the gameboard

    std::vector<char> get_board(); // get copy of board

    struct piece_action // data type stores possible piece moves
    {
        enum action // label used to indicate the phase for which the move is valid
        {
            place,
            move,
            remove
        } action_type;

        int to = -1;
        int from = -1;
        int score = 0;
    };

    void print_board();                                                                                          // Debug: Print the board
    std::vector<std::array<int, 3>> mill_available(const char &cow);                                             // provides all the current mills in the available for the player
    std::vector<std::array<int, 3>> new_mills_available(char cow, std::set<std::array<int, 3>> &previous_mills); // checks if a player has formed a new mill
    std::vector<std::array<int, 2>> potential_mill_available(char &cow);                                         // Provids all the slots filled with two ajacent cows that can form a mill

    std::vector<int> best_position(char cow);                                                                                                                      // returns the slots were a mill is likely to form
    std::vector<int> common_slots(char slot);                                                                                                                      // returns all the slots with the same character
    std::vector<int> get_empty_neighbor(int cow_position);                                                                                                         // returns all neigboring slots of the a given piece
    std::vector<piece_action> get_valid_moves(char my_cow, char opponent_cow, std::vector<std::array<int, 3>> &opponent_mills, int pieces_placed, int max_pieces); // all valid move for a cow in a given slot,

    std::string mill_formed_write(int slot);     // writes down in the "formed mill"  format(Sx,iy0)
    std::string player_position_write(int slot); // writes down in the format (Sxiy)

    bool remove_cow(int cow_slot, char &opponent_cow);                  // removing cow off the board
    bool place_cow(int cow_slot, char &cow);                            // placing cow on board
    bool move_cow(int current_pos, int next_pos, char &cow);            // moving cow on the board
    bool apply_move(piece_action move, char my_cow, char oppenent_cow); // apply move onto board. Move is false when invalid

    void is_position_valid(char &turn, int &position, bool &game_cont); //  checks if placement is validn(used for debugging)

    bool has_formed_mill(char cow, std::set<std::array<int, 3>> &previous_mills, std::vector<std::array<int, 3>> &new_mills_out); // returns a boolean when the newly formed mills
    bool has_potential_mill(char &cow);                                                                                           // checks if two pieces can form a mill
    bool all_cows_in_mill(char cow);                                                                                              // Check if all cows of a given player are in mills
    bool is_slot_empty(int slot);
    bool is_in_mill(int slot, char cow); // check if slot is empty

    // heuristics functions
    int points_state(char cow, char opponent, int pieces_placed, int max_pieces, std::set<std::array<int, 3>> prev_mill, std::vector<std::array<int, 3>> new_mill, bool win_state);
    int intesection_preference(char cow);               // returns the score of the cows with the highest neigbor count
    int blocking_score(char my_cow, char opponent_cow); // provides points that block the opponents

private:
    const char m_gameboard_layout[49] = // board layout to be used when called
        {
            '-', 'o', 'o', '-', 'o', 'o', '-',
            'o', '-', 'o', '-', 'o', '-', 'o',
            'o', 'o', '-', '-', '-', 'o', 'o',
            '-', '-', '-', 'o', '-', '-', '-',
            'o', 'o', '-', '-', '-', 'o', 'o',
            'o', '-', 'o', '-', 'o', '-', 'o',
            '-', 'o', 'o', '-', 'o', 'o', '-'};

    char m_gameboard_play[49]; // where the game will be played

    std::vector<std::array<int, 3>> m_mills_possible = // all the possible mills in the game
        {
            // horizontal mills
            {0, 3, 6},
            {8, 10, 12},
            {16, 17, 18},
            {21, 22, 23},
            {25, 26, 27},
            {30, 31, 32},
            {36, 38, 40},
            {42, 45, 48},

            // vectical mills
            {0, 21, 42},
            {8, 22, 36},
            {16, 23, 30},
            {3, 10, 17},
            {31, 38, 45},
            {18, 25, 32},
            {12, 26, 40},
            {6, 27, 48},

            // diagonal mills
            {0, 8, 16},
            {6, 12, 18},
            {30, 36, 42},
            {32, 40, 48}

    };
};

#endif // GAMEBOARD_H
