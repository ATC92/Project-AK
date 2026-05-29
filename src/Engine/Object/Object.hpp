// | -------------------------------
#pragma once
// | -------------------------------
#include "Game/Entity/Entity.hpp"
#include "Engine/Render/RBatch.hpp"
#include "Engine/Utils/Vector2.hpp"
#include "Engine/Component/Component.hpp"
// | -------------------------------
#include <memory>
#include <string>
#include <type_traits>
#include <vector>
// | -------------------------------

namespace ENG
{
  class Object : public APP::Entity
  {
    public:
      explicit Object(const std::string& _name);
      ~Object() = default;
      void Draw(Batcher& b) const override;
      void Update(float dt) override;

      Object& AddComponent(const std::shared_ptr<IComponents>& component);

      template <typename T> Object& CopyComponent(const T *components)
      {
        auto c = std::make_shared<T>(*components);
        _components.push_back(c);
        return *this;
      }
      template <typename T> T* GetComponent()const
      {
        static_assert(std::is_base_of<IComponents, T>::value, "Only IComponents sons");
        for(auto& c:_components)
        {
          T* comp = dynamic_cast<T*>(c.get());
          if(comp)
            return comp;
        }
        return nullptr;
      }
      template <typename T> bool HasComponent() const
      {
        static_assert(std::is_base_of<IComponents, T>::value, "Only IComponents sons");
        for(auto& c:_components)
        {
          T* comp = dynamic_cast<T*>(c.get());
          if(comp)
              return true;
        }
        return false;
      }

      static int GetCountObj()
      {
        return countObject;
      }

      std::shared_ptr<IName> GetName(void)
      {
        return this->name;
      }
   
      std::shared_ptr<ITransform>& GetTransform(void)
      {
        return this->transform;
      }

      std::shared_ptr<IStats>& GetStats(void)
      {
        return this->stats;
      }

      const bool IsAlive(void)
      {
        return stats->hp < 0;
      }

      std::vector<std::shared_ptr<IComponents>>& GetListComponents(void)
      {
        return _components;
      }
      
      void SetPosition(float x, float y);
      void SetPosition(const Vector2& new_pos);
      const Vector2& GetPosition(void) const;
      Vector2 GetSize(void) const;

    private:
      static int countObject;
    protected:
      std::shared_ptr<IName> name;
      std::shared_ptr<ITransform> transform;
      std::shared_ptr<IStats> stats;
      std::vector<std::shared_ptr<IComponents>> _components;
  };
} // namespace ENG
