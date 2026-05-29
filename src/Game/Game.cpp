/// | ------------------------------------ |
#include "Game.hpp"
/// | ------------------------------------ |
#include "Engine/Services/Services.hpp"
#include "Engine/Utils/Log.hpp"
#include "Engine/Utils/Color.hpp"
#include "Engine/Render/Render.hpp"
#include "Engine/Object/Object.hpp"
#include "Engine/Utils/Geometry.hpp"
#include "Engine/Component/Component.hpp"
#include "Engine/PollEvent/PollEvent.hpp"
/// | ------------------------------------ |
#include "SDL3/SDL_scancode.h"
/// | ------------------------------------ |
#include <memory>
#include <string>
#include <utility>
/// | ------------------------------------ |

namespace APP
{
  Game::Game()
  {
    IsAppEnd = false;
  }

  void Game::OnInit(void)
  {
    LOG_INFO(" | << Game application Init Succesfully");
      

    /// Player creation [0]
    auto player = std::make_unique<ENG::Object>("Player");
    player->GetStats()->hp = 100;
    player->GetTransform()->position = {100,100};
    player->GetTransform()->velocity = {150,150};
    ENG::Services::Assets().Load(ASSET_PATH"Player/Player.png", "Player");

    player->AddComponent(std::make_shared<ENG::ISprite>("Player",1.0f));

    pool.push_back(std::move(player));
    
    pool.push_back(std::make_unique<ENG::Rectangle>(0,0,300,100,ENG::Color::Green,ENG::Color::Red));
    // pool.push_back(std::make_unique<ENG::Line>(200,200,500,500, ENG::Color::White));
  }

  void Game::OnDestroy(void)
  {
    IsAppEnd = true;
    pool.clear();
  }

  bool Game::IsRunning(void)
  {
    return !IsAppEnd;
  }

  void Game::OnInputs(float dt)
  {
    auto& pollEvent = ENG::PollEvent::Get();
    auto player = dynamic_cast<ENG::Object*>(pool.at(0).get());

    if(pollEvent.IsKeyPress(SDL_SCANCODE_ESCAPE))
    {
      IsAppEnd = true;
    }

    if(pollEvent.IsKeyPress(SDL_SCANCODE_F1))
    {
      auto& tF = ENG::Render::Get()._toggleFullscreen;
      tF = !tF;
    }

    if(pollEvent.IsKeyDown(SDL_SCANCODE_D))
    {
      player->GetTransform()->position.x += player->GetTransform()->velocity.x * dt;
    }
    if(pollEvent.IsKeyDown(SDL_SCANCODE_W))
    {
      player->GetTransform()->position.y -= player->GetTransform()->velocity.y * dt;
    }
    if(pollEvent.IsKeyDown(SDL_SCANCODE_A))
    {
      player->GetTransform()->position.x -= (player->GetTransform()->velocity.x * dt);
    }
    if(pollEvent.IsKeyDown(SDL_SCANCODE_S))
    {
      player->GetTransform()->position.y += player->GetTransform()->velocity.y * dt;
    }
  }

  void Game::OnUpdate(float dt)
  {
    
    
  auto player = dynamic_cast<ENG::Object*>(pool.at(0).get());
  player->GetTransform()->prev_position = player->GetTransform()->position;
  }

  void Game::OnRender(float dt)
  {
    auto& b = ENG::Render::Get().GetBatcher();
    for(auto& o : pool)
    {
      o->Draw(b);
    }
  }

}
