// | -------------------------------
#include "Object.hpp"
// | -------------------------------
#include "Game/Entity/Entity.hpp"
// | -------------------------------
#include "Engine/Utils/Log.hpp"
#include "Engine/Render/RBatch.hpp"
#include "Engine/Utils/Vector2.hpp"
#include "Engine/Component/Component.hpp"
// | -------------------------------
#include <glm/ext/vector_float2.hpp>
// | -------------------------------
#include <glm/ext/vector_float4.hpp>
#include <memory>
#include <string>
#include <xcb/xproto.h>
// | -------------------------------

namespace ENG
{
  int Object::countObject = 0;

  Object::Object(const std::string& _name)
    : APP::Entity{}
  {
    name        = std::make_shared<IName>(_name + " Entity: "+ std::to_string(Object::countObject + 1));
    transform   = std::make_shared<ITransform>();
    stats       = std::make_shared<IStats>();
    LOG_INFO(" | << Object: " + name->name);
    Object::countObject ++;
  }
  
  void Object::Draw(Batcher& b) const
  {
    glm::vec2 pos = {this->GetPosition().x, this->GetPosition().y};
    glm::vec2 size = {this->GetSize().x * 6 , this->GetSize().y * 6};
    auto img = this->GetComponent<ISprite>()->image;
    glm::vec4 color = {1,1,1,1};
    b.DrawTexture(pos, size, img, color);
  }

  void Object::Update(float dt)
  {
    (void)dt;
  }

  void Object::SetPosition(float x, float y)
  {
    transform->prev_position = transform->position;
    transform->position.x = x;
    transform->position.y = y;
  }

  void Object::SetPosition(const Vector2& new_pos)
  {
    this->transform->position = new_pos;
  }

  const Vector2& Object::GetPosition(void) const
  {
    return this->transform->position;
  }

  Vector2 Object::GetSize() const
  {
    auto s = GetComponent<ISprite>()->image;
    return {(float)s->GetWidth(), (float)s->GetHeight()};
  }

  Object& Object::AddComponent(const std::shared_ptr<IComponents>& component)
  {
    _components.push_back(component);
    return *this;
  }
}
