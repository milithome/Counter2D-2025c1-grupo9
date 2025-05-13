#ifndef GAME_H
#define GAME_H

class Game {
<<<<<<< Updated upstream
public:
    Game() {}
=======
private:
  std::vector<Player> players;
  Team team1;
  Team team2;
  int map_width;
  int map_height;
  Player &findPlayerById(uint id);
  bool running = true;

public:
  Game(int width, int height) : map_width(width), map_height(height) {}
  bool addPlayer(const std::string &name, const int player_id);
  void movePlayer(const uint player_id, int x, int y, float deltaTime);
  std::vector<Entity> getState();
  bool isRunning();
  void stop();
>>>>>>> Stashed changes
};

#endif // GAME_H