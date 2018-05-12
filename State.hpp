#ifndef BOT_STATE_HPP
#define BOT_STATE_HPP

#include <array>

using PlayerState = std::array<int, 7>;

enum class GameStatus : uint32_t {
    Won,
    Lost,
    Draw,
    Ongoing
};

struct BoardState {
    size_t player_turn;
    std::array<PlayerState, 2> state;

    GameStatus game_status() const;
};

enum class Action : uint32_t {
    M1 = 1,
    M2 = 2,
    M3 = 3,
    M4 = 4,
    M5 = 5,
    M6 = 6,
    M_NONE = 7
};

constexpr size_t n_actions = 7;

BoardState operator+(BoardState b, Action a);

#endif // BOT_STATE_HPP
