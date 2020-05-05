#include "snake.h"
#include <cmath>
#include <iostream>
#include <functional>


void Snake::Update() {
  SDL_Point prev_cell{
      static_cast<int>(head_x),
      static_cast<int>(head_y)};  // We first capture the head's cell before updating.
  UpdateHead();
  SDL_Point current_cell{
      static_cast<int>(head_x),
      static_cast<int>(head_y)};  // Capture the head's cell after updating.

  // Update all of the body vector items if the snake head has moved to a new
  // cell.
  if (current_cell.x != prev_cell.x || current_cell.y != prev_cell.y) {
    UpdateBody(current_cell, prev_cell);
  }
}

void Snake::UpdateHead() {
  switch (direction) {
    case Direction::kUp:
      head_y -= speed;
      break;

    case Direction::kDown:
      head_y += speed;
      break;

    case Direction::kLeft:
      head_x -= speed;
      break;

    case Direction::kRight:
      head_x += speed;
      break;
  }

  // Wrap the Snake around to the beginning if going off of the screen.
  head_x = fmod(head_x + grid_width, grid_width);
  head_y = fmod(head_y + grid_height, grid_height);
}

void Snake::UpdateBody(SDL_Point &current_head_cell, SDL_Point &prev_head_cell) {
  // Add previous head location to vector
  body.push_back(prev_head_cell);

  if (!growing) {
    // Remove the tail from the vector.
    body.erase(body.begin());
  } else {
    growing = false;
    size++;
  }

  // Check if the snake has died.
  for (auto const &item : body) {
    if (current_head_cell.x == item.x && current_head_cell.y == item.y) {
      alive = false;
    }
  }
}

void Snake::GrowBody() { growing = true; }

// Inefficient method to check if cell is occupied by snake.
bool Snake::SnakeCell(int x, int y) {
  if (x == static_cast<int>(head_x) && y == static_cast<int>(head_y)) {
    return true;
  }
  for (auto const &item : body) {
    if (x == item.x && y == item.y) {
      return true;
    }
  }
  return false;
}
//Update as per the victim snake's properties
void Snake::EatSnake(std::shared_ptr<Snake> snake){
  std::shared_ptr<Snake> this_snake = this->get_shared_this();
  this_snake->size += snake->size;
  
  while(snake->size--){ //eat the body of another snake
    this_snake->Update();
    this_snake->GrowBody();

  }
  if(this_snake->speed < .6 && this_snake->snake_type == SnakeType::User)
    this_snake->speed += 0.02;
  else if(this_snake->speed < .4)
    this_snake->speed += 0.005;

  snake->size = 1;
  snake->alive = false;
  if(this_snake->snake_type == SnakeType::User)
    std::cout<< "User snake has killed Computer snake \n";
  else
    std::cout<< "Computer snake has killed User snake \n";
  return;
}
void Snake::Killed(std::shared_ptr<Snake> snake){
  std::shared_ptr<Snake> this_snake = this->get_shared_this();
  //If head on collision between snakes
  if( abs(this_snake->head_x - snake->head_x) < 1 && abs(this_snake->head_y - snake->head_y) < 1){ 
        if(this_snake->size > snake->size){ //check who is bigger
          this_snake->EatSnake(snake);  return;
          }
          else if(this_snake->size == snake->size) //if equal preference to user snake
          {
            if (this_snake->snake_type == SnakeType::Computer) //protect the User snake if size are same
              {
                snake->EatSnake(this_snake);  return;
                
              }
              else
              {
                this_snake->EatSnake(snake);  return;
              }
              
          }
          else
          {
            snake->EatSnake(get_shared_this()); return;
          }
          
      }
  if( snake->SnakeCell(static_cast<int> (head_x), static_cast<int> (head_y))) //bite on the body of the snake
  {
    this_snake->EatSnake(snake);
    return;
  }
  return ; //return without Killing anyone
}