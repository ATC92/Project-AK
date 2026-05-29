// | -------------------------------
#pragma once
// | -------------------------------
#include "Engine/Render/RAssetsManager.hpp"
// | -------------------------------


namespace ENG
{
  class Services
  {
    public:
      static AssetsManager& Assets() {return *assets;}
      static void Provide(AssetsManager* a) { assets = a; }

    private:
      static inline AssetsManager* assets = nullptr;

  };

}
