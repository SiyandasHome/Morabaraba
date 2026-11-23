
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

// optimal depth
#define placement_depth 4
#define movement_depth 4
#define remove_depth 4

Min_max::Min_max(gameboard &game_play, Player &maximizer, Player &minimizer, std::ofstream &output_file)
    : m_gameboard_play(game_play), m_maximizer(maximizer), m_minimizer(minimizer), m_output(output_file)
{
}

int Min_max::min_max(gameboard &gameboard_play, Player &Maximizer, Player &Minimizer, gameboard::piece_action &move, int depth, bool maximizer, int max_pieces) // minmax decision algorithm
{
    int state_eval;
    std::string Maximizer_s = "Maximizer";
    std::string Minimizer_s = "Minimizer";
    bool draw = false;
    int count = 0;

    if (depth <= 0 || Min_max::is_game_over(m_gameboard_play, Maximizer, Minimizer, draw, count)) // when out of depth or at the end conditions
    {
        state_eval = gameboard_play.points_state(Maximizer.get_character(), Minimizer.get_character(), Maximizer.get_pieces_placed(), Maximizer.get_max_pieces(), Maximizer.get_previous_mills(), Maximizer.get_new_mills(), m_maximizer.get_decision()) - gameboard_play.points_state(Minimizer.get_character(), Maximizer.get_character(), Minimizer.get_pieces_placed(), Minimizer.get_max_pieces(), Minimizer.get_previous_mills(), Minimizer.get_new_mills(), m_minimizer.get_decision());
        return state_eval;
    }

    gameboard sim_Board = (gameboard_play); // copy the board
    Player my_sim = (Maximizer);            // copy of player of interest
    Player opponent_sim = (Minimizer);      // copy of opposing play

    if (maximizer) // maximizer turn
    {
        if (!sim_Board.apply_move(move, Maximizer.get_character(), Minimizer.get_character()) && (move.action_type != gameboard::piece_action::remove)) // if apply moves fails stop minmax
        {
            std::cout << "Error(Max) With Movement in minmax class\n";
            return -1000; // debug error
        }

        if (move.action_type == gameboard::piece_action::place) // if in the placement phase, update pieces placed
        {
            my_sim.set_pieces_placed();
        }

        auto opponent_mills = sim_Board.mill_available(opponent_sim.get_character());
        auto my_previous_mills = my_sim.get_previous_mills();
        auto my_new_mills = my_sim.get_new_mills();

        if (sim_Board.has_formed_mill(my_sim.get_character(), my_previous_mills, my_new_mills)) // if the simulated move forms a mill, proceed with the correct move logic
        {
            my_sim.set_previous_mills(my_previous_mills); // update mills
            my_sim.set_new_mills(my_new_mills);           // update mills

            int max_eval = -10000;
            auto remove_pieces = sim_Board.get_valid_moves(my_sim.get_character(), opponent_sim.get_character(), opponent_mills, my_sim.get_pieces_placed(), my_sim.get_max_pieces());

            // simualate a move ahead

            for (auto next_moves : remove_pieces)
            {
                if (next_moves.action_type == gameboard::piece_action::remove) // Filter out to only removes
                {
                    int eval = min_max(sim_Board, my_sim, opponent_sim, next_moves, depth - 1, true, max_pieces);
                    max_eval = std::max(max_eval, eval);
                }
            }

            move.score = max_eval;
            return max_eval;
        }

        else // if no mill has been formed.
        {
            int max_eval = -10000;

            auto my_moves = sim_Board.get_valid_moves(my_sim.get_character(), opponent_sim.get_character(), opponent_mills, my_sim.get_pieces_placed(), max_pieces);

            for (auto &next_move : my_moves)
            {
                if (next_move.action_type == gameboard::piece_action::remove) // preventing accidental removes
                {
                    continue;
                }

                int eval = min_max(sim_Board, my_sim, opponent_sim, next_move, depth - 1, false, max_pieces);
                max_eval = std::max(max_eval, eval);
            }

            move.score = max_eval;
            return max_eval;
        }
    }

    else // minimzer turn
    {
        if (!sim_Board.apply_move(move, Minimizer.get_character(), Maximizer.get_character()) && move.action_type != gameboard::piece_action::remove)
        {

            return 1000;
        }

        if (move.action_type == gameboard::piece_action::place) // if in the placement phase, update pieces placed
        {
            opponent_sim.set_pieces_placed();
        }

        auto my_mills = sim_Board.mill_available(opponent_sim.get_character());
        auto my_previous_mills = opponent_sim.get_previous_mills();
        auto my_new_mills = opponent_sim.get_new_mills();

        if (sim_Board.has_formed_mill(opponent_sim.get_character(), my_previous_mills, my_new_mills))
        {
            opponent_sim.set_new_mills(my_new_mills);
            opponent_sim.set_previous_mills(my_previous_mills);

            int min_eval = 10000;
            auto opponent_mills = sim_Board.mill_available(Maximizer.get_character()); // get maximixer as opposing character
            auto remove_pieces = sim_Board.get_valid_moves(opponent_sim.get_character(), my_sim.get_character(), opponent_mills, opponent_sim.get_pieces_placed(), opponent_sim.get_max_pieces());

            // simualate a move ahead

            for (auto next_moves : remove_pieces)
            {
                if (next_moves.action_type == gameboard::piece_action::remove) // Filter out to only removes
                {

                    int eval = min_max(sim_Board, my_sim, opponent_sim, next_moves, depth - 1, false, max_pieces);
                    min_eval = std::min(min_eval, eval);
                }
            }

            move.score = min_eval;
            return min_eval;
        }

        else
        {

            int min_eval = 10000;
            auto my_mills = sim_Board.mill_available(my_sim.get_character());
            auto opp_moves = sim_Board.get_valid_moves(opponent_sim.get_character(), my_sim.get_character(), my_mills, opponent_sim.get_pieces_placed(), max_pieces);

            for (auto &next_move : opp_moves)
            {
                if (next_move.action_type == gameboard::piece_action::remove) // preventing accidental removes
                {
                    continue;
                }

                int eval = min_max(sim_Board, my_sim, opponent_sim, next_move, depth - 1, true, max_pieces);
                min_eval = std::min(min_eval, eval);
            }

            move.score = min_eval;
            return min_eval;
        }
    }
}

bool Min_max::min_max_placement()
{
    auto opponent_mills = m_gameboard_play.mill_available(m_minimizer.get_character());
    auto all_moves = m_gameboard_play.get_valid_moves(m_maximizer.get_character(), m_minimizer.get_character(), opponent_mills, m_maximizer.get_pieces_placed(), m_minimizer.get_max_pieces());
    gameboard::piece_action best_move;

    if (m_gameboard_play.common_slots(m_maximizer.get_character()).size() == 0) // initial placement
    {
        do
        {
            int random_slot = rand() % (all_moves.size()); // random number between 0 and 8
            best_move = all_moves[random_slot];

        } while (best_move.action_type != gameboard::piece_action::place);

        if (!m_gameboard_play.apply_move(best_move, m_maximizer.get_character(), m_minimizer.get_character()))
        {
            return false;
        }

        m_maximizer.set_pieces_placed();
        m_maximizer.update_move_history(best_move); // update the move that has just occured

        m_output << m_maximizer.get_name() << ' ' << "00-" << m_gameboard_play.player_position_write(best_move.to) << '\n';
        return true;
    }

    else
    {

        for (auto &possible_move : all_moves)
        {
            if (possible_move.action_type == gameboard::piece_action::place)
            {
                int max_score = -10000;
                int min_max_eval = min_max(m_gameboard_play, m_maximizer, m_minimizer, possible_move, placement_depth, true, m_maximizer.get_max_pieces());
                auto max_mill_move = m_gameboard_play.best_position(m_maximizer.get_character());
                auto min_mills_move = m_gameboard_play.best_position(m_minimizer.get_character());

                // guiding maximizer to making the best decision
                if (max_mill_move.size() > 0) // make a mill when possible
                {
                    best_move.to = max_mill_move[0];
                    best_move.action_type = gameboard::piece_action::place;
                    break;
                }

                else if (min_mills_move.size() > 0) // block their best position
                {
                    best_move.to = min_mills_move[0]; // assume first best move is invalid
                    best_move.action_type = gameboard::piece_action::place;
                    break;
                }

                if (min_max_eval == -1000)
                {
                    return false;
                }

                if (min_max_eval > max_score)
                {
                    best_move = possible_move;
                }

                else // if now best move evaluated, make a random
                {
                    best_move = all_moves.at(rand() % all_moves.size());
                }
            }
        }

        if (!m_gameboard_play.apply_move(best_move, m_maximizer.get_character(), m_minimizer.get_character()))
        {
            return false;
        }

        else
        {
            m_output << m_maximizer.get_name() << ' ' << "00-" << m_gameboard_play.player_position_write(best_move.to) << ' ';
            m_maximizer.set_pieces_placed();
        }

        auto prev_mill = m_maximizer.get_previous_mills();
        auto new_mill = m_maximizer.get_new_mills();

        if (m_gameboard_play.has_formed_mill(m_maximizer.get_character(), prev_mill, new_mill)) // bug here as the mill is being stored
        {
            auto max_get_removes = m_gameboard_play.get_valid_moves(m_maximizer.get_character(), m_minimizer.get_character(), opponent_mills, m_maximizer.get_pieces_placed(), m_maximizer.get_max_pieces());
            gameboard::piece_action best_remove;
            best_remove.action_type = gameboard::piece_action::remove;

            best_remove.score = -1000;
            auto mill_update = m_gameboard_play.mill_available(m_maximizer.get_character());
            std::vector<gameboard::piece_action> remove_option;

            m_output << "(mill = ";

            for (int m = 0; m < new_mill.size(); ++m)
            {
                const auto &mill = new_mill[m];

                for (int i = 0; i < mill.size(); ++i)
                {
                    m_output << m_gameboard_play.mill_formed_write(mill.at(i));

                    if (i < mill.size() - 1)
                    {
                        m_output << ','; // comma between elements
                    }
                }

                if (m < new_mill.size() - 1)
                {
                    m_output << "; "; // semicolon between mills
                }
            }

            m_output << ") ";

            auto mill_updated = convert_vector_to_set(mill_update);
            m_maximizer.set_previous_mills(mill_updated);
            m_maximizer.set_new_mills(new_mill);

            for (auto &remove_piece_option : max_get_removes)
            {
                if (remove_piece_option.action_type == gameboard::piece_action::remove)
                {
                    remove_option.push_back(remove_piece_option);
                }
            }

            if (remove_option.size() == 0)
            {

                auto opponent_slots = m_gameboard_play.common_slots(m_minimizer.get_character());
                gameboard::piece_action temp_remove;
                temp_remove.action_type = gameboard::piece_action::remove;
                temp_remove.score;

                for (auto &slot : opponent_slots)
                {
                    temp_remove.from = slot;

                    if (min_max(m_gameboard_play, m_maximizer, m_minimizer, temp_remove, remove_depth, true, m_maximizer.get_max_pieces()) > best_remove.score)
                    {
                        best_remove = temp_remove;
                    }
                }

                if (!m_gameboard_play.apply_move(best_remove, m_maximizer.get_character(), m_minimizer.get_character()))
                {

                    return false;
                }

                m_output << m_minimizer.get_name() << " looses cow (" << m_gameboard_play.player_position_write(best_remove.from) << ") | " << m_gameboard_play.common_slots(m_minimizer.get_character()).size() << " left\n";
                m_maximizer.update_move_history(best_remove);
            }

            else
            {
                if (max_get_removes.size() > 0)
                {
                    for (auto &piece_remove : max_get_removes)
                    {
                        if (piece_remove.action_type == gameboard::piece_action::remove)
                        {
                            if (min_max(m_gameboard_play, m_maximizer, m_minimizer, piece_remove, 3, true, m_maximizer.get_max_pieces()) > best_remove.score)
                            {
                                best_remove = piece_remove;
                            }
                        }
                    }
                }

                else
                {
                    auto opponent_slots = m_gameboard_play.common_slots(m_minimizer.get_character());
                    gameboard::piece_action temp_remove;
                    temp_remove.action_type = gameboard::piece_action::remove;
                    temp_remove.score;

                    for (auto &slot : opponent_slots)
                    {
                        temp_remove.to = slot;

                        if (min_max(m_gameboard_play, m_maximizer, m_minimizer, temp_remove, remove_depth, true, m_maximizer.get_max_pieces()) > best_remove.score)
                        {
                            best_remove = temp_remove;
                        }
                    }
                }
            }

            if (!m_gameboard_play.apply_move(best_remove, m_maximizer.get_character(), m_minimizer.get_character()))
            {
                return false;
            }

            m_output << m_minimizer.get_name() << " looses cow (" << m_gameboard_play.player_position_write(best_remove.from) << ") | " << m_gameboard_play.common_slots(m_minimizer.get_character()).size() << " left\n";
            m_maximizer.update_move_history(best_remove);
            return true;
        }

        else
        {
            m_maximizer.set_previous_mills(prev_mill);
            m_maximizer.set_new_mills(new_mill);
            m_output << '\n';
        }
    }

    return true;
}

bool Min_max::min_max_movement()
{
    auto opponent_mills = m_gameboard_play.mill_available(m_minimizer.get_character());
    auto all_moves = m_gameboard_play.get_valid_moves(
        m_maximizer.get_character(),
        m_minimizer.get_character(),
        opponent_mills,
        m_maximizer.get_pieces_placed(),
        m_maximizer.get_max_pieces());

    gameboard::piece_action best_move;
    best_move.score = -1000; // initialize worst case

    if (check_repetitions(all_moves, best_move))
    {
        if (!m_gameboard_play.apply_move(best_move, m_maximizer.get_character(), m_minimizer.get_character()))
        {
            return false;
        }
        m_maximizer.update_move_history(best_move);
        m_output << m_maximizer.get_name() << ' '
                 << m_gameboard_play.player_position_write(best_move.from) << '-'
                 << m_gameboard_play.player_position_write(best_move.to) << ' ';
    }

    else
    {

        auto best_positions = m_gameboard_play.best_position(m_maximizer.get_character());
        if (!best_positions.empty())
        {
            for (auto &possible : all_moves)
            {
                if (possible.to == best_positions.at(0))
                {
                    best_move = possible;
                    break;
                }
            }

            if (!m_gameboard_play.apply_move(best_move, m_maximizer.get_character(), m_minimizer.get_character()))
            {
                return false;
            }
            m_maximizer.update_move_history(best_move);
            m_output << m_maximizer.get_name() << ' '
                     << m_gameboard_play.player_position_write(best_move.from) << '-'
                     << m_gameboard_play.player_position_write(best_move.to) << ' ';
        }
        else
        {

            for (auto &moves : all_moves)
            {
                if (moves.action_type == gameboard::piece_action::move)
                {
                    int eval = min_max(m_gameboard_play, m_maximizer, m_minimizer, moves, movement_depth, true, m_maximizer.get_max_pieces());
                    moves.score = eval; // keep score consistent
                    if (eval > best_move.score)
                    {
                        best_move = moves;
                    }

                    else // if no best move evaluated, make a random
                    {
                        do
                        {
                            best_move = all_moves.at(rand() % all_moves.size());

                        } while (best_move.action_type != gameboard::piece_action::move);
                    }
                }
            }

            if (!m_gameboard_play.apply_move(best_move, m_maximizer.get_character(), m_minimizer.get_character()))
            {

                return false;
            }
            m_maximizer.update_move_history(best_move);
            m_output << m_maximizer.get_name() << ' '
                     << m_gameboard_play.player_position_write(best_move.from) << '-'
                     << m_gameboard_play.player_position_write(best_move.to) << ' ';
        }
    }

    // updating previous mill state
    auto prev_mill = m_maximizer.get_previous_mills();
    auto new_mill = m_maximizer.get_new_mills();

    if (m_gameboard_play.has_formed_mill(m_maximizer.get_character(), prev_mill, new_mill))
    {
        auto max_get_removes = m_gameboard_play.get_valid_moves(m_maximizer.get_character(), m_minimizer.get_character(), opponent_mills, m_maximizer.get_pieces_placed(), m_maximizer.get_max_pieces());
        gameboard::piece_action best_remove;
        best_remove.action_type = gameboard::piece_action::remove;

        best_remove.score = -1000;
        auto mill_update = m_gameboard_play.mill_available(m_maximizer.get_character());
        std::vector<gameboard::piece_action> remove_option;

        m_output << "(mill = ";

        for (int m = 0; m < new_mill.size(); ++m)
        {
            const auto &mill = new_mill[m];

            for (int i = 0; i < mill.size(); ++i)
            {
                m_output << m_gameboard_play.mill_formed_write(mill.at(i));

                if (i < mill.size() - 1)
                {
                    m_output << ','; // comma between elements
                }
            }

            if (m < new_mill.size() - 1)
            {
                m_output << "; "; // semicolon between mills
            }
        }

        m_output << ") ";

        auto mill_updated = convert_vector_to_set(mill_update);

        // update mill state for current state
        m_maximizer.set_previous_mills(mill_updated);
        m_maximizer.set_new_mills(new_mill);

        for (auto &remove_piece_option : max_get_removes)
        {
            if (remove_piece_option.action_type == gameboard::piece_action::remove)
            {
                remove_option.push_back(remove_piece_option);
            }
        }

        if (remove_option.size() == 0)
        {

            auto opponent_slots = m_gameboard_play.common_slots(m_minimizer.get_character());
            gameboard::piece_action force_remove;
            force_remove.action_type = gameboard::piece_action::remove;
            force_remove.score;

            for (auto &slot : opponent_slots)
            {
                force_remove.from = slot;

                if (min_max(m_gameboard_play, m_maximizer, m_minimizer, force_remove, remove_depth, true, m_maximizer.get_max_pieces()) > best_remove.score)
                {
                    best_remove = force_remove;
                }
            }

            if (!m_gameboard_play.apply_move(best_remove, m_maximizer.get_character(), m_minimizer.get_character()))
            {
                return false;
            }

            m_output << m_minimizer.get_name() << " losses cow (" << m_gameboard_play.player_position_write(best_remove.from) << ") | "
                     << m_gameboard_play.common_slots(m_minimizer.get_character()).size() << " left\n";
            m_maximizer.update_move_history(best_remove);

            return true;
        }

        else
        {
            for (auto &piece_remove : max_get_removes)
            {
                if (piece_remove.action_type == gameboard::piece_action::remove)
                {
                    if (min_max(m_gameboard_play, m_maximizer, m_minimizer, piece_remove, 3, true, m_maximizer.get_max_pieces()) > best_remove.score)
                    {
                        best_remove = piece_remove;
                    }
                }
            }
        }

        if (!m_gameboard_play.apply_move(best_remove, m_maximizer.get_character(), m_minimizer.get_character()))
        {
            return false;
        }

        m_output << m_minimizer.get_name() << " losses cow (" << m_gameboard_play.player_position_write(best_remove.from) << ") | " << m_gameboard_play.common_slots(m_minimizer.get_character()).size() << " left\n";
        m_maximizer.update_move_history(best_remove);
        return true;
    }

    else
    {
        m_maximizer.set_previous_mills(prev_mill);
        m_maximizer.set_new_mills(new_mill);
        m_output << '\n';
    }

    return true;
}

std::set<std::array<int, 3>> Min_max::convert_vector_to_set(std::vector<std::array<int, 3>> vector)
{
    std::set<std::array<int, 3>> converted;

    for (auto to_set : vector)
    {
        converted.emplace(to_set); // place the new mills into the new vector
    }

    return converted;
}

bool Min_max::check_repetitions(std::vector<gameboard::piece_action> &my_moves, gameboard::piece_action &best_move)
{
    auto move_history = m_maximizer.get_move_history();
    int s = move_history.size();

    if (s >= 4)
    {
        if (move_history.at(s - 1).from == move_history.at(s - 3).from && move_history.at(s - 1).to == move_history.at(s - 3).to && move_history.at(s - 1).action_type == move_history.at(s - 3).action_type &&
            move_history.at(s - 2).from == move_history.at(s - 4).from && move_history.at(s - 2).to == move_history.at(s - 4).to && move_history.at(s - 2).action_type == move_history.at(s - 4).action_type)
        {

            do
            {
                int r = rand() % my_moves.size();
                best_move = my_moves.at(r); // make a random moves

            } while (best_move.action_type != gameboard::piece_action::move);

            return true;
        }
    }

    return false;
}

bool Min_max::is_game_over(gameboard &board, Player &player1, Player &player2, bool &draw, int &mill_formed)
{

    // iniatialzing player information
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

    if (player2_moves.size() == 0 && player1_moves.size() == 0)
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

    if (mill_formed >= 250)
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