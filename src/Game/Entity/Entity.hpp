// | -------------------------------
#pragma once
// | -------------------------------
#include "Engine/Render/RBatch.hpp"
// | -------------------------------

namespace APP
{
  class Entity
  {
    public:
      Entity() = default;
      virtual ~Entity() = default;
      virtual void Draw(ENG::Batcher& b) const = 0;
      virtual void Update(float dt) = 0;
  };
}
