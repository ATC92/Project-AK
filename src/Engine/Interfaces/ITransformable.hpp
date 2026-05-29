#pragma once

#include "Engine/Utils/Vector2.hpp"
namespace ENG
{
  class ITransformable
  {
    public:
      virtual void SetPosition(float x, float y) = 0;
      virtual void SetPosition(const Vector2& new_pos) = 0;
      virtual const Vector2& GetPosition(void) const = 0;
  };
}
