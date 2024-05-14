//
// Created by anton on 5/13/24.
//

#ifndef ENGINE_GAME_H
#define ENGINE_GAME_H

#include <memory>
#include <string>

namespace sf
{
class RenderWindow;
} // namespace sf

class Game
{
public:
  Game();
  ~Game();

  void run();

private:
  std::unique_ptr<sf::RenderWindow> Window;

  std::string Title = "Game";
};

#endif // ENGINE_GAME_H
