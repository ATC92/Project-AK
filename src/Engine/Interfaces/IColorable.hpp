#pragma once

#include "Engine/Utils/Color.hpp"

namespace ENG
{
  class IColorable
  {
    public:
      virtual void SetColor(const Color& color) = 0;
      virtual void SetOutlineColor(const Color& color) = 0;
  };
}
