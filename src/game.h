#ifndef GAME_H
#define GAME_H

#include <random>
#include <vector>
#include <memory>
#include "SDL.h"
#include "controller.h"
#include "renderer.h"
#include "snake.h"

class Game {
 public:
  Game(std::size_t grid_width, std::size_t grid_height);
  void Run(Controller const &controller, Renderer &renderer,
           std::size_t target_frame_duration);
  int GetScore() const;
  int GetSize() const;
  void UpdateScore(int size){ score += size; }// to update the score if eating a computer snake
  void SnakeSize(int size){ userSnakeSize = size > userSnakeSize ? size : userSnakeSize; } //keep update of snake size before he dies

 private:
  std::shared_ptr<Snake> snake;
  std::shared_ptr<Snake> compSnake;

  std::vector<std::shared_ptr <Snake> > snake_vector; //snake vector
  std::vector<SDL_Point> food_vector; //Food vector
  std::random_device dev;
  std::mt19937 engine;
  std::uniform_int_distribution<int> random_w;
  std::uniform_int_distribution<int> random_h;

  int userSnakeSize{1};
  int score{0};
  std::size_t grid_width;
  std::size_t grid_height;
  void PlaceFood();
  void Update();
  void GetFoodDirection(std::shared_ptr<Snake> snake);
  void ChangeDirection(std::shared_ptr<Snake> snake, Snake::Direction input, Snake::Direction opposite) const;
};

#endif