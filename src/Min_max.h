#ifndef MIN_MAX_H
#define MIN_MAX_H

#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <set>
#include <algorithm>
#include <ctime>
#include <cstdlib>

#include "gameboard.h"
#include "Player.h"

class Min_max
{
public:
    Min_max(gameboard &game_play, Player &maximizer, Player &minimizer, std::ofstream &output_file);

    int min_max(gameboard &gameboard_play, Player &Maximizer, Player &Minimizer, gameboard::piece_action &move, int depth, bool maximizer, int max_pieces); // minmax decision algorithm

    bool min_max_placement(); // minmax placement logic

    bool min_max_movement(); // minmax movement logic

    std::set<std::array<int, 3>> convert_vector_to_set(std::vector<std::array<int, 3>> vector); // convert the vector to a set

    bool check_repetitions(std::vector<gameboard::piece_action> &my_moves, gameboard::piece_action &best_move); // make a random move when there are repetitions
    bool is_game_over(gameboard &board, Player &player1, Player &player2, bool &draw, int &mill_formed);        // checks if state has terminated

private:
    Player &m_maximizer;
    Player &m_minimizer;

    gameboard &m_gameboard_play;
    std::ofstream &m_output;
};

#endif // MIN_MAX_H*/
