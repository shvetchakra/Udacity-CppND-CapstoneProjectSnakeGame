#include "game.h"
#include <iostream>
#include <algorithm>
#include <memory>
#include "SDL.h"
#include "snake.h"


Game::Game(std::size_t grid_width, std::size_t grid_height)
    : grid_width(grid_width), grid_height(grid_height),
      snake(std::make_shared <Snake>(grid_width, grid_height, Snake::SnakeType::User)),
      engine(dev()),
      random_w(0, static_cast<int>(grid_width)),
      random_h(0, static_cast<int>(grid_height)) {
  
 PlaceFood();
 snake_vector.emplace_back(snake); //Pushing User snake pointer in snake vector 
 }

void Game::Run(Controller const &controller, Renderer &renderer,
               std::size_t target_frame_duration) {
  Uint32 title_timestamp = SDL_GetTicks();
  Uint32 frame_start;
  Uint32 frame_end;
  Uint32 frame_duration;
  Uint32 food_duration;
  int frame_count = 0;
  bool running = true;
  Uint32 food_placement_time = SDL_GetTicks();
  
  while (running) {
    frame_start = SDL_GetTicks();

    // Input, Update, Render - the main game loop.
    controller.HandleInput(running, snake);
    
    SDL_Point prev_food = food_vector[0];
    Update();
    renderer.Render(snake_vector, food_vector);
    //renderer.Render(compSnake, food_vector);

    frame_end = SDL_GetTicks();
    food_duration = frame_end - food_placement_time; //to get the food duration from the last food placement
    if(food_vector.size() == 0)
      PlaceFood();
    if(food_duration >= 10000 && food_vector.size()>0) //if more than 10 sec place more food and create computer snake
    {
      PlaceFood();
      food_placement_time = SDL_GetTicks();
      
      //computer snake initiated
      if(!compSnake){
        compSnake = std::make_shared< Snake>(grid_width,grid_height, Snake::SnakeType::Computer);
        snake_vector.emplace_back(compSnake);
      }
      else if(!compSnake->alive) //computer snake reinitiated after death
      {
        Snake snake = Snake(grid_width,grid_height, Snake::SnakeType::Computer);
        compSnake.reset( &snake);
        snake_vector.emplace_back(compSnake);
        compSnake->alive = true;
      }
      
    }
    // Keep track of how long each loop through the input/update/render cycle takes.
    frame_count++;
    frame_duration = frame_end - frame_start;

    // After every second, update the window title.
    if (frame_end - title_timestamp >= 1000) {
      renderer.UpdateWindowTitle(score, frame_count);
      frame_count = 0;
      title_timestamp = frame_end;
    }

    // If the time for this frame is too small (i.e. frame_duration is
    // smaller than the target ms_per_frame), delay the loop to
    // achieve the correct frame rate.
    if (frame_duration < target_frame_duration) {
      SDL_Delay(target_frame_duration - frame_duration);
    }
  }
}

void Game::PlaceFood() {
  int x, y;
  while (true) {
    x = random_w(engine);
    y = random_h(engine);
    // Check that the location is not occupied by a snake item before placing
    // food.
    if (!snake->SnakeCell(x, y) && food_vector.size() < 5) { 
      SDL_Point food;
      food.x = x;
      food.y = y;
      food_vector.emplace_back(food);
      return;
    }
    else if(food_vector.size()>=5) //if there are five food , donot place new food
      return;

  }
}
//To change the direction while checking the opposite direction for computer snake
//For the computer snake to change the direction 
void Game::ChangeDirection(std::shared_ptr<Snake> snake, Snake::Direction input,
                                 Snake::Direction opposite) const {
  if (snake->direction != opposite || snake->size == 1) snake->direction = input;
  return;
}

//Get the direction to reach to food for computer snake using manhattan distance to find closest food
void Game::GetFoodDirection(std::shared_ptr<Snake> snake){
  int new_x = static_cast<int>(compSnake->head_x);
  int new_y = static_cast<int>(compSnake->head_y);
  std::sort(food_vector.begin(),food_vector.end(),[&new_x, &new_y](const auto& a, const auto& b)   {
                                        return ((abs(a.x - new_x) + abs(a.y - new_y)) < (abs(b.x - new_x) + abs(b.y - new_y)));
                                      });
  if (!food_vector.empty())                                    
    { 
      SDL_Point food_position = food_vector.front();
  if(new_x != food_position.x)
    {
        if(new_x > food_position.x) ChangeDirection(snake, Snake::Direction::kLeft, Snake::Direction::kRight);
        else
        ChangeDirection(snake, Snake::Direction::kRight, Snake::Direction::kLeft) ;
        
    }
  if(new_y != food_position.y)
  {
    if (new_y > food_position.y) ChangeDirection(snake, Snake::Direction::kUp, Snake::Direction::kDown);
    else 
    ChangeDirection(snake, Snake::Direction::kDown, Snake::Direction::kUp);
  }
  
    return ;
  
  
  }
}

void Game::Update() {
  
    
    if(snake != nullptr && compSnake != nullptr){

      SnakeSize(snake->size);
      compSnake->Killed(snake);//computer Snake killed user snake
    }
    if (!snake->alive) return;
    
    if(snake != nullptr && compSnake != nullptr)
     {
       snake->Killed(compSnake);//user snake killer computer snake
        int compSnakeSize = compSnake->size;
        if(!compSnake->alive)
          UpdateScore(compSnakeSize);
     }
  if (compSnake != nullptr && !compSnake->alive) { // if User snake has killed Computer snake reset computer snake pointer
    
    snake_vector.pop_back();
    Snake *snake = nullptr;
    compSnake.reset( snake);
   
  }
  snake->Update(); //update the position of user snake
  if(compSnake != nullptr){
    GetFoodDirection(compSnake);
    compSnake->Update(); //update the position of computer snake if that's alive
    }
  

  // Check if there's food over here 
  // Check which snake has eaten the food.
  std::for_each(snake_vector.begin(), snake_vector.end(),  [&](std::shared_ptr<Snake> &snake){
 
    int new_x = static_cast<int>(snake->head_x);
    int new_y = static_cast<int>(snake->head_y);
  for (auto food = food_vector.begin(); food != food_vector.end(); ++food){
    if (food->x == new_x && food->y == new_y) {
      score++;
      std::vector<SDL_Point>::iterator it = std::find_if(food_vector.begin(),food_vector.end(),[&food](const auto& a)   {
                                        return (a.x == food->x && a.y == food->y);
                                      });
      food_vector.erase(it);
      PlaceFood();
      // Grow snake and increase speed.
      snake->GrowBody();
      if(snake->snake_type == Snake::SnakeType::Computer && snake->speed < .4)//max speed of computer snake
        snake->speed += 0.005; //lesser increment in speed of computer snake
      else
        if(snake->speed < .6) //max speed of user snake
          snake->speed += 0.02;  //increment in snake speed
    }
  }
  });//snake loop close
}

int Game::GetScore() const { return score; }
int Game::GetSize() const { return userSnakeSize; }