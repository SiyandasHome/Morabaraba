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

Player::Player(gameboard &gameboard_play, std::string alg_name, std::string opponent_cow_name, char my_cow, char opponent_cow, int starting_pieces, std::ofstream &output_file)
    : m_gameboard_play(gameboard_play),
      m_player_name(alg_name),
      m_opponent_name(opponent_cow_name),
      m_my_cow(my_cow),
      m_opponent_cow(opponent_cow),
      m_piece_number_max(starting_pieces),
      m_piece_number_placed(0),
      m_piece_number_remaining(starting_pieces),
      m_previous_state_mills(),
      m_new_mill(),
      m_move_history(),
      m_output_write(output_file),
      m_game_decision()

{
}

char Player::get_character()
{
    return m_my_cow;
}

std::string Player::get_name()
{

    return m_player_name;
}

int Player::get_max_pieces()
{
    return m_piece_number_max;
}

int Player::get_Pieces_remaining()
{
    m_piece_number_remaining = static_cast<int>(m_gameboard_play.common_slots(m_my_cow).size());
    return m_piece_number_remaining;
}

int Player::get_pieces_placed()
{
    return m_piece_number_placed;
}

void Player::set_pieces_placed()
{
    if (m_piece_number_placed < m_piece_number_max)
    {
        ++m_piece_number_placed;
    }
}

std::set<std::array<int, 3>> Player::get_previous_mills()
{
    return m_previous_state_mills;
}

std::vector<std::array<int, 3>> Player::get_new_mills()
{
    return m_new_mill;
}

void Player::set_previous_mills(std::set<std::array<int, 3>> &new_current_mill)
{
    m_previous_state_mills = new_current_mill;
}

void Player::set_new_mills(std::vector<std::array<int, 3>> &new_mill)
{
    m_new_mill = new_mill;
}

void Player::set_decision()
{
    m_game_decision = true;
}

bool Player::get_decision()
{
    return m_game_decision;
}

std::vector<gameboard::piece_action> Player::get_move_history()
{
    return m_move_history;
}

void Player::update_move_history(gameboard::piece_action &move)
{
    m_move_history.push_back(move); // update the move
}

bool Player::mill_formed(std::set<std::array<int, 3>> &prev_mill, std::vector<std::array<int, 3>> &new_mill) // checks whether player has made a new game
{
    if (m_gameboard_play.has_formed_mill(m_my_cow, prev_mill, new_mill))
    {
        auto opponent_mills = m_gameboard_play.mill_available(m_opponent_cow);

        auto max_get_removes = m_gameboard_play.get_valid_moves(m_my_cow, m_opponent_cow, opponent_mills, get_pieces_placed(), get_max_pieces());
        std::vector<gameboard::piece_action> best_remove_option;
        gameboard::piece_action best_remove;
        best_remove.score = -1000;

        m_output_write << " (mill = ";

        for (int m = 0; m < new_mill.size(); ++m) // writing to output file with the correct format
        {
            const auto &mill = new_mill[m];

            for (int i = 0; i < mill.size(); ++i)
            {
                m_output_write << m_gameboard_play.mill_formed_write(mill.at(i));

                if (i < mill.size() - 1)
                {
                    m_output_write << ','; // comma between elements
                }
            }

            if (m < new_mill.size() - 1)
            {
                m_output_write << "; "; // semicolon between mills
            }
        }

        m_output_write << ") ";

        set_previous_mills(prev_mill);
        set_new_mills(new_mill);

        for (auto &piece_remove : max_get_removes)
        {
            if (piece_remove.action_type == gameboard::piece_action::remove)
            {
                best_remove_option.push_back(piece_remove);
            }
        }

        if (best_remove_option.size() == 0)
        {

            int r = rand() % m_gameboard_play.common_slots(m_opponent_cow).size();
            best_remove.from = m_gameboard_play.common_slots(m_opponent_cow).at(r);

            best_remove.to = -1;
            best_remove.action_type = gameboard::piece_action::remove;

            if (!m_gameboard_play.apply_move(best_remove, m_my_cow, m_opponent_cow))
            {
                std::cout << "Player want to remove from " << best_remove.from << " to " << best_remove.to << '\n';
                std::cout << "Player: remove attempt failed\n";
                return false;
            }

            m_output_write << m_opponent_name << " losses cow (" << m_gameboard_play.player_position_write(best_remove.from) << ") | " << m_gameboard_play.common_slots(m_opponent_cow).size() << " left";
            update_move_history(best_remove);
            return true;
        }

        else
        {
            // apply random remove
            int r = rand() % best_remove_option.size();
            best_remove = best_remove_option[r];

            if (!m_gameboard_play.apply_move(best_remove, m_my_cow, m_opponent_cow))
            {
                std::cout << "Player want to remove from " << best_remove.from << " to " << best_remove.to << '\n';
                std::cout << "Player: remove attempt failed\n";
                return false;
            }
        }

        m_output_write << m_opponent_name << " looses cow (" << m_gameboard_play.player_position_write(best_remove.from) << ") | " << m_gameboard_play.common_slots(m_opponent_cow).size() << " left";
    }

    return true;
}

bool Player::heuristic_placement()
{
    // Stop if all pieces are already placed
    if (m_piece_number_placed >= m_piece_number_max)
    {
        return false;
    }

    auto opponentMills = m_gameboard_play.mill_available(m_opponent_cow);
    auto all_moves = m_gameboard_play.get_valid_moves(
        m_my_cow, m_opponent_cow, opponentMills,
        m_piece_number_placed, m_piece_number_max);

    std::vector<gameboard::piece_action> place_moves;
    for (auto &m : all_moves)
    {
        if (m.action_type == gameboard::piece_action::place)
        {
            place_moves.push_back(m);
        }
    }

    // --- Case 1: First placement (random) ---
    if (m_gameboard_play.common_slots(m_my_cow).empty())
    {
        if (place_moves.empty())
            return false;

        int r = rand() % place_moves.size();
        if (!m_gameboard_play.apply_move(place_moves[r], m_my_cow, m_opponent_cow))
        {
            std::cout << "heuristic_placement failed (first move)\n";
            return false;
        }

        set_pieces_placed();
        m_output_write << m_player_name << " 00-"
                       << m_gameboard_play.player_position_write(place_moves[r].to);

        set_new_mills(m_new_mill); // update mill state

        if (!mill_formed(m_previous_state_mills, m_new_mill))
        {
            std::cout << "remove attempt failed\n";
            return false;
        }

        else
        {
            m_output_write << '\n';
        }
        return true;
    }

    auto mill_positions = m_gameboard_play.best_position(m_my_cow);
    if (!mill_positions.empty())
    {
        gameboard::piece_action best_move;
        best_move.action_type = gameboard::piece_action::place;
        best_move.to = mill_positions[rand() % mill_positions.size()];

        if (!m_gameboard_play.apply_move(best_move, m_my_cow, m_opponent_cow))
        {
            std::cout << "heuristic placement failed (mill)\n";
            return false;
        }

        set_pieces_placed();
        m_output_write << m_player_name << " 00-"
                       << m_gameboard_play.player_position_write(best_move.to);

        set_new_mills(m_new_mill);
        if (!mill_formed(m_previous_state_mills, m_new_mill))
        {
            std::cout << "remove attempt failed\n";
        }
        else
        {
            m_output_write << '\n';
        }
        return true;
    }

    if (!m_gameboard_play.potential_mill_available(m_my_cow).empty())
    {
        auto my_common_slots = m_gameboard_play.common_slots(m_my_cow);
        std::vector<gameboard::piece_action> cow_to_place;

        for (auto &slot : my_common_slots)
        {
            auto neighbors = m_gameboard_play.get_empty_neighbor(slot);
            for (auto &potential_place : neighbors)
            {
                gameboard::piece_action move_to_place;
                move_to_place.action_type = gameboard::piece_action::place;
                move_to_place.to = potential_place;
                cow_to_place.push_back(move_to_place);
            }
        }

        if (cow_to_place.empty())
            return false;

        gameboard::piece_action move = cow_to_place[rand() % cow_to_place.size()];
        if (!m_gameboard_play.apply_move(move, m_my_cow, m_opponent_cow))
        {
            std::cout << "heuristic placement failed (group of two)\n";
            return false;
        }

        set_pieces_placed();
        m_output_write << m_player_name << " 00-" << m_gameboard_play.player_position_write(move.to);

        set_new_mills(m_new_mill);

        if (!mill_formed(m_previous_state_mills, m_new_mill))
        {
            std::cout << "remove attempt failed\n";
            return false;
        }

        else
        {
            m_output_write << '\n';
        }
        return true;
    }

    if (!place_moves.empty())
    {
        int r = rand() % place_moves.size();
        if (!m_gameboard_play.apply_move(place_moves[r], m_my_cow, m_opponent_cow))
        {
            std::cout << "heuristic placement failed (random fallback)\n";
            return false;
        }

        set_pieces_placed();
        m_output_write << m_player_name << " 00-" << m_gameboard_play.player_position_write(place_moves[r].to);

        set_new_mills(m_new_mill);
        if (!mill_formed(m_previous_state_mills, m_new_mill))
        {
            std::cout << "remove attempt failed\n";
            return false;
        }

        else
        {
            m_output_write << '\n';
        }
        return true;
    }

    // --- No moves available ---
    return false;
}

bool Player::heuristic_movement()
{
    int MAX_PIECES = get_max_pieces();
    auto opponent_mills = m_gameboard_play.mill_available(m_opponent_cow);
    auto moves = m_gameboard_play.get_valid_moves(m_my_cow, m_opponent_cow,
                                                  opponent_mills, m_piece_number_placed, MAX_PIECES);

    std::vector<gameboard::piece_action> move_actions;
    for (auto &m : moves)
        if (m.action_type == gameboard::piece_action::move)
            move_actions.push_back(m);

    if (move_actions.empty())
        return false; // truly no move

    gameboard::piece_action best_move;
    best_move.from = best_move.to = -1;

    //Try to find a move that creates a potential mill (only if we don't already have one)
    if (!m_gameboard_play.has_potential_mill(m_my_cow))
    {
        for (auto &m : move_actions)
        {
            gameboard temp = m_gameboard_play;
            temp.apply_move(m, m_my_cow, m_opponent_cow);
            if (temp.has_potential_mill(m_my_cow))
            {
                best_move = m;
                break;
            }
        }
    }

    //Otherwise score every move and pick the best
    if (best_move.from == -1)
    {
        int bestScore = -1000;
        for (auto &m : move_actions)
        {
            gameboard temp = m_gameboard_play;
            temp.apply_move(m, m_my_cow, m_opponent_cow);
            int s = temp.points_state(m_my_cow, m_opponent_cow,
                                      m_piece_number_placed, m_piece_number_max,
                                      m_previous_state_mills, m_new_mill, false);
            if (s > bestScore)
            {
                bestScore = s;
                best_move = m;
            }
        }
    }

    //Last resort — random
    if (best_move.from == -1)
        best_move = move_actions[rand() % move_actions.size()];

    if (!m_gameboard_play.apply_move(best_move, m_my_cow, m_opponent_cow))
        return false;

    m_output_write << m_player_name << ' '
                   << m_gameboard_play.player_position_write(best_move.from) << '-'
                   << m_gameboard_play.player_position_write(best_move.to);
    mill_formed(m_previous_state_mills, m_new_mill);
    m_output_write << '\n';
    
    return true;
}
