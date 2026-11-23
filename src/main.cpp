
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
#include "Min_max.h"

using namespace std;

bool is_game_over(gameboard &board, Player &player1, Player &player2, bool &draw, int &max_moves); // returns the terminal state of the game

int main()
{
    srand(static_cast<unsigned>(time(nullptr)));

    // open files
    std::ifstream input("input.txt");
    std::ofstream output("Morabaraba_Results.txt");

    if (!input.is_open()) // checks to see if file exists
    {
        std::cout << "File not located.\n";
        return -1;
    }

    char Alg_1 = 'A'; // first algorithm
    std::string Alg1_string = "Alg_1";

    char Alg_2 = 'B'; // second algorithm
    std::string Alg2_string = "Alg_2";

    int starting_pieces = 0;
    int test_count = 0;

    while (input >> starting_pieces) // game loop
    {

        gameboard board_play;        // initialize board game board
        bool draw_condition = false; // checks for a draw conditon
        int max_move_number = 0;

        Player Heuristic(board_play, Alg1_string, Alg2_string, Alg_1, Alg_2, starting_pieces, output);
        Player Minmax(board_play, Alg2_string, Alg1_string, Alg_2, Alg_1, starting_pieces, output);

        Min_max Min_max_player(board_play, Minmax, Heuristic, output);

        output << starting_pieces << '\n';

        while (Minmax.get_pieces_placed() < starting_pieces && Heuristic.get_pieces_placed() < starting_pieces) // Placement phase
        {
            Min_max_player.min_max_placement();

            Heuristic.heuristic_placement();
        }

        while (!is_game_over(board_play, Heuristic, Minmax, draw_condition, max_move_number)) // Movement Phase
        {
            Min_max_player.min_max_movement();

            Heuristic.heuristic_movement();

            max_move_number++;
        }

        // terminal states
        if (Minmax.get_decision())
        {
            output << '\n'
                   << Minmax.get_name() << " wins\n"
                   << '\n';
        }

        else if (Heuristic.get_decision())
        {
            output << '\n'
                   << Heuristic.get_name() << " wins\n"
                   << '\n';
        }

        else if (draw_condition)
        {
            output << "\nIt is a draw\n\n";
        }
    }

    input.close();
    output.close();

    return 0;
}

bool is_game_over(gameboard &board, Player &player1, Player &player2, bool &draw, int &max_moves)
{

    // Initialzing player information
    char player1_cow = player1.get_character();
    char player2_cow = player2.get_character();
    int player1_number_cow = board.common_slots(player1_cow).size();
    int player2_number_cow = board.common_slots(player2_cow).size();
    bool player1_decision = player1.get_decision();
    bool player2_decision = player2.get_decision();
    auto player1_mills = board.mill_available(player1_cow);
    auto player2_mills = board.mill_available(player2_cow);
    auto player1_moves = board.get_valid_moves(player1_cow, player2_cow, player2_mills, player1.get_pieces_placed(), player1.get_max_pieces());
    auto player2_moves = board.get_valid_moves(player2_cow, player1_cow, player1_mills, player2.get_pieces_placed(), player2.get_max_pieces());

    if (player2_moves.size() <= 0 && player1_moves.size() <= 0)
    {
        draw = true;
        return true;
    }

    if (player2_moves.size() > 0 && player1_moves.size() <= 0)
    {
        player2.set_decision();
        return true;
    }

    if (player2_moves.size() <= 0 && player1_moves.size() > 0)
    {
        player1.set_decision();
        return true;
    }

    if (max_moves >= 250)
    {
        draw = true;
        return true;
    }

    if (player1_number_cow <= 2 && player2_number_cow <= 2)
    {
        draw = true;
        return true;
    }

    else if (player1_number_cow >= 3 && player2_number_cow <= 2)
    {
        player1.set_decision();
        return true;
    }

    else if (player2_number_cow >= 3 && player1_number_cow <= 2)
    {
        player2.set_decision();
        return true;
    }

    return false;
}
