#pragma once

namespace ENG
{ 
  class IUpdatable
  {
    public:
      virtual void Update(float dt) = 0;
  };
}
