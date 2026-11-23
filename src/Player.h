#ifndef PLAYER_H
#define PLAYER_H

#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <set>
#include <algorithm>
#include <ctime>
#include <cstdlib>

#include "gameboard.h"

class Player
{

public:
    Player(gameboard &gameboard_play, std::string alg_name, std::string opponent_cow_name, char my_cow, char opponent_cow, int starting_pieces, std::ofstream &output_file); // algorithm constructor

    int get_Pieces_remaining(); // returns the number of pieces as a getter;
    char get_character();
    std::string get_name(); // get the characters name

    int get_max_pieces(); // indicates the number of starting pieces

    int get_pieces_placed();  // get the number of pieces placed
    void set_pieces_placed(); // get the number of pieces placed

    std::vector<gameboard::piece_action> get_move_history(); // gets the previous moves made
    void update_move_history(gameboard::piece_action &move); // updates the moves made by the player to be used in the output file

    void set_decision(); // set the terminal start for when the game is over
    bool get_decision(); // get the terminal state for when the game is over

    std::set<std::array<int, 3>> get_previous_mills();                                                    // get all the mills for the previous state
    std::vector<std::array<int, 3>> get_new_mills();                                                      // get all the news mills for the game
    void set_previous_mills(std::set<std::array<int, 3>> &new_mills);                                     // update the previous mill for the game with the current one
    void set_new_mills(std::vector<std::array<int, 3>> &new_mill);                                        // update all the new mills with the current one(this might be clear each game)
    bool mill_formed(std::set<std::array<int, 3>> &prev_mill, std::vector<std::array<int, 3>> &new_mill); // executes actionns if new mill formed

    bool heuristic_placement(); // Heuristic placment phase
    bool heuristic_movement();  // Heuristic movement phase

private:
    gameboard &m_gameboard_play; // actual gameboard used to play

    int m_piece_number_placed = 0; // number of pieces on board
    int m_piece_number_max;        // the max number of pieces the algorith will use
    int m_piece_number_remaining;  // the number of pieces remaining
    std::set<std::array<int, 3>> m_previous_state_mills;
    std::vector<std::array<int, 3>> m_new_mill;
    std::vector<gameboard::piece_action> m_move_history; // use this to print on the the correct format on the board

    char m_my_cow;
    char m_opponent_cow;
    std::string m_player_name;
    std::string m_opponent_name;
    bool m_game_decision = false;

    std::ofstream &m_output_write; // write to the output file
};

#endif // PLAYER_H
