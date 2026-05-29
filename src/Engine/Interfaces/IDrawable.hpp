#pragma once

namespace ENG
{
  class IDrawable
  {
    public:
      virtual ~IDrawable() = default;
      virtual void Draw(void) const = 0;
  };
}
