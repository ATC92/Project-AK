// | -------------------------------
#include "Engine.hpp"
// | -------------------------------
#include "Engine/Render/RAssetsManager.hpp"
#include "Engine/Services/Services.hpp"
#include "Engine/Utils/Color.hpp"
#include "Engine/Utils/Config.hpp"
#include "Engine/Render/Render.hpp"
#include "Engine/Layer/GameLayer.hpp"
#include "Engine/PollEvent/PollEvent.hpp"
// | -------------------------------
#include "Engine/Utils/Log.hpp"
#include "SDL3/SDL_error.h"
// | -------------------------------
#include <chrono>
#include <memory>
#include <string>
#include <utility>
// | -------------------------------

namespace ENG
{
  Engine::Engine(const EngineConfig& config, std::unique_ptr<GameLayer> layer)
    : eConfig(std::move(config)), game(std::move(layer)) {}

  bool Engine::OnInit(void)
  {  
    auto& r = Render::Get();
    if(r.InitWindowSDLContext(this->eConfig.title, this->eConfig.vW, this->eConfig.vH) == 0)
    {
      LOG_INFO(" | << SDL InitWindows Context Error :" + static_cast<std::string>(SDL_GetError()));
      return false;
    }
    if(!r.InitGLContext())
      LOG_INFO(" | << OpenGL Context NOT created succesfully" + static_cast<std::string>(SDL_GetError()));
    LOG_INFO(" | << SDL InitWindows Context created succesfully" + static_cast<std::string>(SDL_GetError()));
    LOG_INFO(" | << OpenGL Context created succesfully");
    LOG_INFO(" | << PollEvent Engine created succesfully");
    PollEvent::Get().ClearPollEvent();
    LOG_INFO(" | << OpenGL Context created succesfully");
    LOG_INFO(" | << Render Engine created succesfully");

    LOG_INFO(" | << Creating BatchingContext");
    // Init Batcher and RenderContext
    r.InitRenderContext();
    r.SetScreenSize(this->eConfig.vW, this->eConfig.vH);

    LOG_INFO(" | << AssertsManager Services created");
    AssetsManager amgr;
    Services::Provide(&amgr);

    game->OnInit();


    return true;
  }

  bool Engine::OnRun(void)
  {
    LOG_INFO(" | << GameEngine Application is Running");

    // Create DeltaTime
    auto frame_ant = std::chrono::high_resolution_clock::now();
    float dt = 0.16666f; 
    
    while (game->IsRunning())
    {
      PollEvent::Get().ProcessPollEvents();
      game->OnInputs(dt);
      game->OnUpdate(dt);
      auto& r = Render::Get();
      
      // Update Render
      r.UpdateRender();
      // Start Rendering
      r.StartDraw();
      {
        ENG::Render::Get().RenderColor(ENG::Color::Gray); 
        game->OnRender(dt);
      }
      r.EndDraw();
      // Clear Render
      r.ClearRender();
       

      PollEvent::Get().ResetPollEvent();
      

      // Update DeltaTime
      auto frame_act = std::chrono::high_resolution_clock::now();
      dt = std::chrono::duration_cast<std::chrono::duration<float>>(frame_act-frame_ant).count();
      frame_ant = frame_act;

    }

    LOG_INFO(" | << Application next to close, waiting...");
    PollEvent::Get().ClearPollEvent();
    game->OnDestroy();
    return true;
  }

  void Engine::OnDestroy(void)
  {
    LOG_INFO(" | << Destroying Engine, waiting...");
    Render::Get().DestroyBatch();
    Render::Get().DestroyWindowSDLContext();
    PollEvent::Get().ClearPollEvent();
    PollEvent::Get().DestroyPollEvent();
  }
}
