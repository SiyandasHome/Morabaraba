#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <set>
#include <algorithm>
#include <iomanip>

#include "gameboard.h"

gameboard::gameboard()
    : m_gameboard_play()
{
    for (int i = 0; i < 49; i++)
    {
        m_gameboard_play[i] = m_gameboard_layout[i];
    }
}

void gameboard::print_board()
{
    for (int i = 0; i < 49; i++)
    {
        if (i % 7 == 0 && i != 0)
            std::cout << '\n';
        std::cout << m_gameboard_play[i] << ' ';
    }
    std::cout << '\n';
}

std::vector<char> gameboard::get_board()
{
    std::vector<char> board_copy(49);

    for (int i = 0; i < 49; i++)
    {
        board_copy[i] = m_gameboard_play[i];
    }

    return board_copy;
}

std::vector<std::array<int, 3>> gameboard::mill_available(const char &cow)
{
    std::vector<std::array<int, 3>> available_m_mills_possible;

    for (auto &m : m_mills_possible)
    {
        if (m_gameboard_play[m[0]] == cow && m_gameboard_play[m[1]] == cow && m_gameboard_play[m[2]] == cow)
        {
            available_m_mills_possible.emplace_back(m); // gives all mill positions occupied
        }
    }

    return available_m_mills_possible;
}

bool gameboard::has_formed_mill(char cow, std::set<std::array<int, 3>> &previous_mills_possible, std::vector<std::array<int, 3>> &new_mills_possible)
{
    auto current_m_mills_possible = mill_available(cow); // displays mill of current state

    new_mills_possible.clear(); // clears all mills, to replace with new mills
    for (auto &m : current_m_mills_possible)
    {
        if (previous_mills_possible.find(m) == previous_mills_possible.end()) // finds new mill in previous mill, if not present it puts it in new m_mills_possible to use
        {
            new_mills_possible.push_back(m); // when m not found, place it in this array
        }
    }

    // updates previous m_mills_possible formed with new m_mills_possible.
    previous_mills_possible.clear();
    previous_mills_possible.insert(current_m_mills_possible.begin(), current_m_mills_possible.end());

    return !new_mills_possible.empty(); // checks to see if the new mill has formed
}

std::vector<std::array<int, 2>> gameboard::potential_mill_available(char &cow)
{
    std::vector<std::array<int, 2>> potential_available_mill;

    for (auto &m : m_mills_possible) // looks at all all the possible mills, then places into the potential mill vector all the slots that can form a mill in the next turn
    {
        if (m_gameboard_play[m[0]] == cow && m_gameboard_play[m[1]] == cow && m_gameboard_play[m[2]] == '-')
        {
            potential_available_mill.emplace_back(std::array<int, 2>{m[0], m[1]});
        }

        else if (m_gameboard_play[m[0]] == cow && m_gameboard_play[m[1]] == '-' && m_gameboard_play[m[2]] == cow)
        {
            potential_available_mill.emplace_back(std::array<int, 2>{m[0], m[2]}); // positions in this array
        }

        else if (m_gameboard_play[m[0]] == '-' && m_gameboard_play[m[1]] == cow && m_gameboard_play[m[2]] == cow)
        {

            potential_available_mill.emplace_back(std::array<int, 2>{m[1], m[2]}); // positions in this array
        }
    }

    return potential_available_mill;
}

bool gameboard::has_potential_mill(char &cow)
{
    for (auto &m : m_mills_possible) // creates finds any two pieces that can form a new mill, then returns true
    {
        if (m_gameboard_play[m[0]] == cow && m_gameboard_play[m[1]] == cow && m_gameboard_play[m[2]] == '-')
        {
            return true;
        }

        else if (m_gameboard_play[m[0]] == cow && m_gameboard_play[m[1]] == '-' && m_gameboard_play[m[2]] == cow)
        {
            return true;
        }

        else if (m_gameboard_play[m[0]] == '-' && m_gameboard_play[m[1]] == cow && m_gameboard_play[m[2]] == cow)
        {
            return true;
        }
    }

    return false;
}

std::vector<int> gameboard::best_position(char cow)
{
    std::set<int> position; // to return unique values,only individual copies needed

    std::vector<std::array<int, 2>> potential_mills_possible = potential_mill_available(cow);

    for (int i = 0; i < m_mills_possible.size(); i++) // finding all the losts on the board that can be used to form a mill in the next players turn
    {
        for (int j = 0; j < potential_mills_possible.size(); j++)
        {
            if (potential_mills_possible[j][0] == m_mills_possible[i][0] && potential_mills_possible[j][1] == m_mills_possible[i][1] && m_gameboard_play[potential_mills_possible[j][0]] == cow)
            {
                position.insert(m_mills_possible[i][2]);
            }
            else if (potential_mills_possible[j][0] == m_mills_possible[i][1] && potential_mills_possible[j][1] == m_mills_possible[i][2] && m_gameboard_play[potential_mills_possible[j][1]] == cow)
            {
                position.insert(m_mills_possible[i][0]);
            }
            else if (potential_mills_possible[j][0] == m_mills_possible[i][0] && potential_mills_possible[j][1] == m_mills_possible[i][2] && m_gameboard_play[potential_mills_possible[j][0]] == cow)
            {
                position.insert(m_mills_possible[i][1]);
            }
            else // if(potential_m_mills_possible[][0] != m_mills_possible[][0]  && potential_m_mills_possible[][1] != m_mills_possible[][1] && potential_m_mills_possible[][0] != m_mills_possible[][2] && potential_m_mills_possible[][1] != mill[][2])
            {
                continue;
            }
        }
    }

    std::vector<int> best_pos(position.begin(), position.end()); // return all the values needed into the vectors
    return best_pos;
}

std::vector<int> gameboard::common_slots(char slot)
{
    int gameboard_size = 49;
    std::vector<int> slots;

    for (int i = 0; i < gameboard_size; i++) // returns slots of the same character
    {
        if (m_gameboard_play[i] == slot)
        {
            slots.push_back(i);
        }
    }

    return slots;
}

bool gameboard::remove_cow(int cow_slot, char &opponent_cow)
{
    if (m_gameboard_play[cow_slot] == opponent_cow)
    {
        if (all_cows_in_mill(opponent_cow) || !is_in_mill(cow_slot, opponent_cow)) // cow must not be in a mill, unless all cows in mill
        {
            m_gameboard_play[cow_slot] = '-';
            return true;
        }
    }
    return false;
}

bool gameboard::place_cow(int cow_slot, char &cow)
{
    if (m_gameboard_play[cow_slot] == '-') // slot must be empty for placement to occur
    {
        m_gameboard_play[cow_slot] = cow;
        return true;
    }

    return false;
}

bool gameboard::move_cow(int current_pos, int next_pos, char &cow)
{
    std::vector<int> valid_move_positions = get_empty_neighbor(current_pos); // initializes all possible moves of current position
    bool move_not_possible = false;                                          // assume next move is false

    if (valid_move_positions.size() == 0)
    {
        return move_not_possible;
    } // when size is zero, the is no valid move for that current piece

    for (int i = 0; i < valid_move_positions.size(); i++) // checks and find whether the next move if possible
    {
        if (next_pos == valid_move_positions[i])
        {
            m_gameboard_play[current_pos] = '-'; // change piece back to empty slot
            return place_cow(next_pos, cow);
        }
    }

    return move_not_possible;
}

bool gameboard::is_slot_empty(int slot)
{
    if (m_gameboard_play[slot] == '-')
        return true;

    return false;
}

std::vector<int> gameboard::get_empty_neighbor(int cow_position)
{

    std::vector<int> empty_neighbor; // checks each slot and its neigbor to verify if move is possible

    switch (cow_position)
    {
    case 0:
        empty_neighbor = {21, 8, 3};
        break;

    case 3:
        empty_neighbor = {0, 6, 10};
        break;

    case 6:
        empty_neighbor = {27, 3, 12};
        break;

    case 8:
        empty_neighbor = {0, 22, 16, 10};
        break;

    case 10:
        empty_neighbor = {3, 8, 12, 17};
        break;

    case 12:
        empty_neighbor = {10, 6, 18, 26};
        break;

    case 16:
        empty_neighbor = {8, 23, 17};
        break;

    case 17:
        empty_neighbor = {10, 16, 18};
        break;

    case 18:
        empty_neighbor = {12, 17, 25};
        break;

    case 21:
        empty_neighbor = {0, 42, 22};
        break;

    case 22:
        empty_neighbor = {8, 36, 21, 23};
        break;

    case 23:
        empty_neighbor = {16, 30, 22};
        break;

    case 25:
        empty_neighbor = {26, 18, 32};
        break;

    case 26:
        empty_neighbor = {25, 12, 40, 27};
        break;

    case 27:
        empty_neighbor = {6, 48, 26};
        break;

    case 30:
        empty_neighbor = {31, 23, 36};
        break;

    case 31:
        empty_neighbor = {30, 32, 38};
        break;

    case 32:
        empty_neighbor = {25, 31, 40};
        break;

    case 36:
        empty_neighbor = {30, 42, 38, 22};
        break;

    case 38:
        empty_neighbor = {31, 45, 36, 40};
        break;

    case 40:
        empty_neighbor = {48, 32, 26, 38};
        break;

    case 42:
        empty_neighbor = {36, 21, 45};
        break;

    case 45:
        empty_neighbor = {42, 48, 38};
        break;

    case 48:
        empty_neighbor = {27, 40, 45};
        break;
    }

    // adding empty slots only
    std::vector<int> free_neighbor;

    for (int slot : empty_neighbor) // finds all empty neighbors and makes them available for movement phase
    {

        if (is_slot_empty(slot))
        {

            free_neighbor.push_back(slot);
        }
    }

    return free_neighbor;
}

std::string gameboard::mill_formed_write(int slot)
{

    switch (slot)
    {

    case 0:
        return "S2i6";
        break;

    case 3:
        return "S2i7";
        break;

    case 6:
        return "S2i0";
        break;

    case 8:
        return "S1i6";
        break;

    case 10:
        return "S1i7";
        break;

    case 12:
        return "S1i0";
        break;

    case 16:
        return "S0i6";
        break;

    case 17:
        return "S0i7";
        break;

    case 18:
        return "S0i0";
        break;

    case 21:
        return "S2i5";
        break;

    case 22:
        return "S1i5";
        break;

    case 23:
        return "S0i5";
        break;

    case 25:
        return "S0i1";
        break;

    case 26:
        return "S1i1";
        break;

    case 27:
        return "S2i1";
        break;

    case 30:
        return "S0i4";
        break;

    case 31:
        return "S0i3";
        break;

    case 32:
        return "S0i2";
        break;

    case 36:
        return "S1i4";
        break;

    case 38:
        return "S1i3";
        break;

    case 40:
        return "S1i2";
        break;

    case 42:
        return "S2i4";
        break;

    case 45:
        return "S2i3";
        break;

    case 48:
        return "S2i2";
        break;

    default:
        return " "; // return this error when slot is not available
    }
}

std::string gameboard::player_position_write(int slot)
{

    switch (slot)
    {

    case 0:
        return "S2,i6";
        break;

    case 3:
        return "S2,i7";
        break;

    case 6:
        return "S2,i0";
        break;

    case 8:
        return "S1,i6";
        break;

    case 10:
        return "S1,i7";
        break;

    case 12:
        return "S1,i0";
        break;

    case 16:
        return "S0,i6";
        break;

    case 17:
        return "S0,i7";
        break;

    case 18:
        return "S0,i0";
        break;

    case 21:
        return "S2,i5";
        break;

    case 22:
        return "S1,i5";
        break;

    case 23:
        return "S0,i5";
        break;

    case 25:
        return "S0,i1";
        break;

    case 26:
        return "S1,i1";
        break;

    case 27:
        return "S2,i1";
        break;

    case 30:
        return "S0,i4";
        break;

    case 31:
        return "S0,i3";
        break;

    case 32:
        return "S0,i2";
        break;

    case 36:
        return "S1,i4";
        break;

    case 38:
        return "S1,i3";
        break;

    case 40:
        return "S1,i2";
        break;

    case 42:
        return "S2,i4";
        break;

    case 45:
        return "S2,i3";
        break;

    case 48:
        return "S2,i2";
        break;

    default:
        return " ";
    }
}

std::vector<std::array<int, 3>> gameboard::new_mills_available(char cow, std::set<std::array<int, 3>> &previous_mills_possible)
{
    auto current_m_mills_possible = mill_available(cow);
    std::vector<std::array<int, 3>> new_mills_possible;

    for (auto m : current_m_mills_possible)
    {
        if (std::find(previous_mills_possible.begin(), previous_mills_possible.end(), m) == previous_mills_possible.end()) // finding the new m_mills_possible formed, by comparing to previous state
        {
            new_mills_possible.push_back(m);
        }
    }

    return new_mills_possible;
}

int gameboard::points_state(char cow, char opponent, int pieces_placed, int max_pieces, std::set<std::array<int, 3>> prev_mill, std::vector<std::array<int, 3>> new_mill, bool win_state)
{
    int mill_points = mill_available(cow).size();
    int pieces_points = common_slots(cow).size();
    int potential_points = potential_mill_available(cow).size();
    int intersection_pref = intesection_preference(cow);
    int block_points = blocking_score(cow, opponent);
    int best_form_bias = best_position(cow).size();

    if (win_state == true)
    {
        return 10000; // win condition
    }

    // Placement phase
    if (pieces_placed < max_pieces)
    { // biases moves that get the pieces on the board, and getting the pieces to the slot with more intersections

        return 50 * mill_points + 50 * pieces_points + 25 * potential_points + 15 * intersection_pref + 15 * block_points + 25 * best_form_bias;
    }

    // Removal phase (after forming a mill)
    if (has_formed_mill(cow, prev_mill, new_mill))
    {
        return 1 * mill_points + 100 * pieces_points + 50 * potential_points + 1 * intersection_pref + 1 * block_points + 25 * best_form_bias;
    }

    // movement phase
    return 70 * mill_points + 50 * pieces_points + 35 * potential_points + 1 * intersection_pref + 15 * block_points + 20 * best_form_bias;
}

std::vector<gameboard::piece_action> gameboard::get_valid_moves(char my_cow, char opponent_cow, std::vector<std::array<int, 3>> &opponent_mills, int pieces_placed, int max_pieces)
{
    std::vector<gameboard::piece_action> moves;

    { // remove piece
        // filter cows not in a mill, or if all cows are in a mill, grant the ability to take

        std::vector<int> opponent_cow_slots = common_slots(opponent_cow);
        gameboard::piece_action move;

        for (int i = 0; i < opponent_cow_slots.size(); i++)
        {
            bool in_mill = false;

            for (int j = 0; j < opponent_mills.size(); j++)
            {
                for (int k = 0; k < opponent_mills[j].size(); k++)
                {
                    if (opponent_cow_slots[i] == opponent_mills[j][k] && !all_cows_in_mill(opponent_cow))
                    {
                        in_mill = true;
                        break;
                    }
                }

                if (in_mill)
                    break;
            }

            if (in_mill)
                continue;

            move.action_type = gameboard::piece_action::remove;
            move.from = opponent_cow_slots[i]; // from the slot position
            move.to = -1;                      // off the booard
            moves.push_back(move);
        }
    }

    if (pieces_placed < max_pieces) // placement phase
    {

        for (int i = 0; i < 49; i++) // finding valid move
        {
            gameboard::piece_action move;
            if (is_slot_empty(i))
            {
                move.action_type = piece_action::place; // name this a place action
                move.from = -1;                         // from off the board
                move.to = i;                            // to i slot on the board
                moves.push_back(move);                  // add to vector moves
            }
        }
    }

    else // movement phase
    {
        std::vector<int> my_cow_slots = common_slots(my_cow); // get all my cow positions

        for (int i = 0; i < my_cow_slots.size(); i++)
        {
            std::vector<int> next_move = get_empty_neighbor(my_cow_slots[i]); // getting all possible moves for that cow
            piece_action move;

            for (int j = 0; j < next_move.size(); j++)
            {
                move.action_type = piece_action::move;
                move.from = my_cow_slots[i];
                move.to = next_move[j];
                moves.push_back(move);
            }
        }
    }

    return moves;
}

bool gameboard::apply_move(gameboard::piece_action play_move, char my_cow, char opponent_cow)
{
    // applies when move is invalid
    if (play_move.action_type == piece_action::place)
    {
        if (!place_cow(play_move.to, my_cow))
        {
            return false;
        }

        return true;
    }

    else if (play_move.action_type == piece_action::move)
    {
        if (!move_cow(play_move.from, play_move.to, my_cow))
        {
            return false;
        }

        return true;
    }

    else if (play_move.action_type == piece_action::remove)
    {
        if (!remove_cow(play_move.from, opponent_cow))
        {
            return false;
        }

        return true;
    }

    return false;
}

int gameboard::intesection_preference(char cow)
{
    auto cow_slots = common_slots(cow);
    int slot_ranking = 0;

    for (auto &slot : cow_slots)
    {
        slot_ranking += get_empty_neighbor(slot).size();
    }

    return slot_ranking;
}

int gameboard::blocking_score(char my_cow, char opponent_cow)
{
    int score = 0;

    // Get all potential mills for the opponent
    auto opponent_potentials = potential_mill_available(opponent_cow);

    for (auto &mill : opponent_potentials)
    {
        // Each "mill" is a set of 2 occupied slots + 1 empty slot
        for (int slot : mill)
        {
            if (m_gameboard_play[slot] == my_cow)
            {
                score += 3; // weight for blocking
            }
        }
    }

    return score;
}

bool gameboard::all_cows_in_mill(char cow)
{
    auto cow_slots = common_slots(cow);
    if (cow_slots.size() == 0) // if there are no cows , then not all in mills
    {
        return false;
    }

    auto mills = mill_available(cow);

    std::set<int> mill_positions;
    for (auto &m : mills) // insert all the cows slots in a mill to another position vector
    {
        for (int pos : m)
        {
            mill_positions.insert(pos);
        }
    }

    for (int slot : cow_slots) // use all the mills available to find all the slots, then check if they are in a mill
    {
        if (mill_positions.find(slot) == mill_positions.end())
        {
            return false;
        }
    }

    return true;
}

bool gameboard::is_in_mill(int slot, char cow)
{
    auto mills = mill_available(cow);
    for (auto &m : mills)
    {
        if (std::find(m.begin(), m.end(), slot) != m.end())
        {
            return true;
        }
    }

    return false;
}
