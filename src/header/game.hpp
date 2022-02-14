#ifndef GAME_H
#define GAME_H

namespace Game 
{
    struct GameBoard;

    enum class PlayGameFlag 
    { 
        BrandNewGame, 
        ContinuePreviousGame
    };

    void playGame(PlayGameFlag flag, GameBoard gb, 
            unsigned long long boardSize = 1);

    void startGame();
    void continueGame();

}; // namespace Game

#endif // !GAME_H