#pragma once

#include "Engine/Layer/GameLayer.hpp"
#include "Engine/Object/Object.hpp"
#include "Engine/Utils/Color.hpp"
#include "Game/Entity/Entity.hpp"
#include <memory>
#include <vector>

namespace APP
{
  class Game: public ENG::GameLayer
  {
    public:
      explicit Game();
      ~Game()override{};
      void OnInit(void) override;
      void OnInputs(float dt ) override;
      void OnDestroy(void) override;
      bool IsRunning(void) override;
      void OnUpdate(float dt) override;
      void OnRender(float dt) override;
    private:
      std::vector<std::unique_ptr<Entity>> pool;
    private:
      bool IsAppEnd;
      std::shared_ptr<ENG::Object> player;

    protected:
      ENG::Color bgColor;
      float changer = 0.0;
  };
}
