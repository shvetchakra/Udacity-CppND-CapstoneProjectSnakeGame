#ifndef SNAKE_H
#define SNAKE_H

#include <vector>
#include <iostream>
#include <memory>
#include "SDL.h"

class Snake : public std::enable_shared_from_this<Snake>{ //to pass the this pointer of snake's shared pointer
 public:
  enum class Direction { kUp, kDown, kLeft, kRight };
  enum class SnakeType { User, Computer };
  Snake(int grid_width, int grid_height, SnakeType snaketype)
      : grid_width(grid_width),
        grid_height(grid_height),
        head_x(grid_width / 2),
        head_y(grid_height / 2),
        snake_type(snaketype) {std::cout<< "Snake Param Constructor\n";}
  Snake()
  {
    std::cout<< "Snake Default Constructor\n";
  }
  void Update();

  void GrowBody();
  bool SnakeCell(int x, int y);
  void Killed(std::shared_ptr<Snake> snake); // 'thisSnake' has Killed 'snake'
  void EatSnake(std::shared_ptr<Snake> snake); //eat the body of the snake
  std::shared_ptr<Snake> get_shared_this() { return shared_from_this(); }

  Direction direction = Direction::kUp;
  SnakeType snake_type = SnakeType::Computer;
  float speed{0.1f};
  int size{1};
  bool alive{true};
  float head_x;
  float head_y;
  std::vector<SDL_Point> body;

 private:
  void UpdateHead();
  void UpdateBody(SDL_Point &current_cell, SDL_Point &prev_cell);

  bool growing{false};
  int grid_width;
  int grid_height;
};

#endif