// | -------------------------------
#include "Geometry.hpp"
// | -------------------------------
#include "Engine/Render/RBatch.hpp"
#include "Engine/Utils/Color.hpp"
#include "Engine/Utils/Log.hpp"
#include "Engine/Utils/Vector2.hpp"
// | -------------------------------
#include "SDL3/SDL_rect.h"
// | -------------------------------
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float4.hpp>
// | -------------------------------
#include <cstddef>
#include <string>
// | -------------------------------

namespace ENG
{
  int Geometry::amountGeometries = 0;
  /// Line Part
  Line::Line(float x, float y, float xw, float yh, const Color& c)
    : Geometry{}, pStart(x, y), pEnd(xw, yh), color(c)
  {
    amountGeometries ++;
    LOG_INFO(" | << Object: Line created, AmountGeometries " + std::to_string(amountGeometries));
  }

  Line::Line(const Vector2& start, const Vector2& end, const Color& c)
    : Geometry{}, pStart(start), pEnd(end), color(c)
  {
    amountGeometries ++;
    LOG_INFO(" | << Object: Line created, AmountGeometries " + std::to_string(amountGeometries));
  }

  void Line::Draw(Batcher& b) const
  {
    glm::vec2 pointA = {pStart.x, pStart.y};
    glm::vec2 pointB = {pEnd.x, pEnd.y};
    glm::vec4 c = {color.r, color.g, color.b, color.a};

    b.DrawLine(pointA, pointB, 1.0f, c);
    
  }
  
  void Line::Update(float dt)
  {
    (void)dt;
  }

  void Line::SetColor(const Color& c)
  {
    this->color = c;
  }

  void Line::SetPosition(float x, float y)
  {
    this->pStart = {x,y};
  }

  void Line::SetPosition(float x, float y, float xw, float yh)
  {
    this->pStart.x = x;
    this->pStart.y = y;
    this->pEnd.x = xw;
    this->pEnd.y = yh;
  }

  void Line::SetPosition(const Vector2& start, const Vector2& end)
  {
    this->pStart = start;
    this->pEnd = end;
  }
  const Vector2& Line::GetPosition(void) const
  {
    return this->pStart;
  }

  Vector2 Line::GetCenter(void)
  {
    Vector2 rVector;
    rVector.x   = (pStart.x + pEnd.x) * 0.5;
    rVector.y   = (pStart.y + pEnd.y) * 0.5;

    return rVector;
  }
  /// Rectangle part
  Rectangle::Rectangle(float x, float y, float w, float h, const Color& c)
    : Geometry{} ,rect(x,y,w,h),position(x,y),color(c), line_color(c)
  { 
    amountGeometries ++;
    LOG_INFO(" | << Object: Rectangle created, AmountGeometries " + std::to_string(amountGeometries));
  }
  Rectangle::Rectangle(float x, float y, float w, float h, const Color& c, const Color& outColor)
    : Geometry{} ,rect(x,y,w,h),position(x,y),color(c), line_color(outColor)
  { 
    amountGeometries ++;
    LOG_INFO(" | << Object: Rectangle created, AmountGeometries " + std::to_string(amountGeometries));
  }
  Rectangle::Rectangle(float xy, float wh, const Color& c)
    : Geometry{}, rect(xy,xy, wh,wh), position(xy,xy), color(c), line_color(c)
  {
    amountGeometries ++;
    LOG_INFO(" | << Object: Rectangle created, AmountGeometries " + std::to_string(amountGeometries));
  }

  Rectangle::Rectangle(float xy, float wh, const Color& c, const Color& outColor)
    : Geometry{}, rect(xy,xy, wh,wh), position(xy,xy), color(c), line_color(outColor)
  {
    amountGeometries ++;
    LOG_INFO(" | << Object: Rectangle created, AmountGeometries " + std::to_string(amountGeometries));
  }
  Rectangle::Rectangle(const Vector2& position, const Vector2& size, const Color& c)
    : Geometry{}, rect{position.x,position.y, size.x ,size.y}, color{c}, line_color{c} 
  {
    amountGeometries ++;
    LOG_INFO(" | << Object: Rectangle created, AmountGeometries " + std::to_string(amountGeometries));
  }

  Rectangle::Rectangle(const Vector2& position, const Vector2& size, const Color& c, const Color& outColor)
    : Geometry{}, rect{position.x,position.y, size.y ,size.y}, color{c}, line_color{outColor} 
  {
    amountGeometries ++;
    LOG_INFO(" | << Object: Rectangle created, AmountGeometries " + std::to_string(amountGeometries));
  }

  void Rectangle::Draw(Batcher& b) const
  {
    // Fill
    glm::vec4 fillColor = {color.r, color.g, color.b, color.a};
    glm::vec2 pos = {position.x, position.y};
    glm::vec2 size = {rect.w,rect.h};

    b.DrawQuad(pos,size,fillColor);

    // Outline
    if(line_color.a > 0)
    {
      float x = position.x, y = position.y;
      float w = rect.w, h = rect.h;
      
      glm::vec4 outColor = {line_color.r, line_color.g, line_color.b, line_color.a};
      b.DrawLine({x, y},  {x+w, y},  1.0f, outColor);
      b.DrawLine({x + w,  y},  {x+w, y + h}, 1.0f, outColor);
      b.DrawLine({x + w, y + h},  {x, y + h}, 1.0f, outColor);
      b.DrawLine({x, y + h},  {x, y}, 1.0f, outColor);
    }
  }

  void Rectangle::Update(float dt)
  {
    // Create Lines for OutlineColor
    this->points[0] = {rect.x,rect.y};
    this->points[1] = {rect.x + rect.w, rect.y};
    this->points[2] = {rect.x + rect.w, rect.y + rect.h};
    this->points[3] = {rect.x, rect.y + rect.h};
    this->points[4] = {rect.x,rect.y};
  }
  
  void Rectangle::SetSize(float w, float h)
  {
    this->rect.x = w,
    this->rect.y = h;
  }
  
  void Rectangle::SetColor(const Color& c)
  {
    this->color = c;
  }

  void Rectangle::SetOutlineColor(const Color& c)
  {
    this->line_color = c;
  }
  
  void Rectangle::SetPosition(float x, float y)
  {
    this->position = {x,y};
    this->rect.x = x;
    this->rect.y = y;
  }

  void Rectangle::SetPosition(const Vector2& v)
  {
    this->position = v;
    this->rect.x = v.x;
    this->rect.y = v.y;
  }
  
  const Vector2& Rectangle::GetPosition(void) const
  {
    return position;
  }
  
  Vector2 Rectangle::GetCenter(void)
  {
    Vector2 v;
    v.x = (rect.x + rect.w) / 2;
    v.y = (rect.y + rect.h) / 2;
    return v;
  }

  /// Circle part
  Circle::Circle(float x, float y, float diameter, const Color& color)
    :Geometry{}, position(x, y), diameter(diameter), color(color)
  {
    amountGeometries ++;
    LOG_INFO(" | << Object: Circle created, AmountGeometries " + std::to_string(amountGeometries));
  }

  void Circle::Draw(Batcher& b) const
  {
    float rad = diameter * 0.5f;

    glm::vec2 pos= {position.x, position.y};
    glm::vec4 c = {color.r,color.g, color.b, color.a};

    b.DrawCircle(pos, rad, c);

    glm::vec4 oColor = {line_color.r, line_color.g, line_color.b, line_color.a};
    if(line_color.a > 0)
      b.DrawCircleOutLine(pos, rad, oColor);
  }

  void Circle::Update(float dt)
  {
    (void)dt;
  }

  void Circle::SetSize(float diameter)
  {
    this->diameter = diameter;
  }
  
  void Circle::SetColor(const Color& c)
  {
    this->color = c;
  }

  void Circle::SetOutlineColor(const Color& c)
  {
    this->color = c;
  }

  void Circle::SetPosition(float x, float y)
  {
    this->position.x = x;
    this->position.y = y;
  }

  void Circle::SetPosition( const Vector2& v)
  {
    this->position = v;
  }

  const Vector2& Circle::GetPosition(void) const
  {
    return this->position;
  }
    
  Vector2 Circle::GetCenter(void)
  {
    return position;
  }

  /// Triangle part
  Triangle::Triangle(const Vector2& a, const Vector2& b, const Vector2& c, const Color& color)
      : Geometry{}, color(color) 
  {
    points[0] = a;
    points[1] = b;
    points[2] = c;

    amountGeometries ++;
    LOG_INFO(" | << Object: Triangle created, AmountGeometries " + std::to_string(amountGeometries));
  }
  
  void Triangle::Draw(Batcher& b) const 
  {
    // Points[0 .. 2]
    glm::vec4 c = {color.r, color.g, color.b, color.a};
    glm::vec2 point1 = {points[0].x,points[0].y};
    glm::vec2 point2 = {points[1].x, points[1].y};
    glm::vec2 point3 = {points[2].y, points[2].y};

    b.DrawTriangle(point1, point2, point3, c);

    if(line_color.a > 0)
    {
      glm::vec4 outColor = {line_color.r, line_color.g, line_color.b, line_color.a};
      
      b.DrawLine(point1, point2, 1.0f, outColor);
      b.DrawLine(point2, point3, 1.0f, outColor);
      b.DrawLine(point3, point1, 1.0f, outColor);
    }
  }

  void Triangle::Update(float dt)
  {
    (void)dt;
  }

  void Triangle::SetSize(const Vector2& a, const Vector2& b, const Vector2& c)
  {
    points[0] = a;
    points[1] = b;
    points[2] = c;
  }

  void Triangle::SetColor(const Color& c)
  {
    this->color = c;
  }

  void Triangle::SetOutlineColor(const Color& c)
  {
    this->line_color = c;
  }

  void Triangle::SetPosition(float x, float y)
  {
    this->position = {x,y};
  }

  void Triangle::SetPosition(const Vector2& new_pos)
  {
    this->position = new_pos;
  }

  const Vector2& Triangle::GetPosition(void) const
  {
    return this->position;
  }

  Vector2 Triangle::GetCenter(void)
  {
    return { (points[0].x + points[1].x + points[2].x) / 3.0f,
             (points[0].y + points[1].y + points[2].y) / 3.0f }; 
  }

  // Polygon part
  Polygon::Polygon(float x, float y, int sides, const Color& c)
    : Geometry{}, position(x,y), sides(sides), color(c)
  {
    if(sides < 0)
    {
      sides = 0;
    }
    poolPoints.reserve(sides); 

    amountGeometries ++;
    LOG_INFO(" | << Object: Polygon created, AmountGeometries " + std::to_string(amountGeometries));
  }

  void Polygon::Draw(Batcher& b) const
  {
    if(poolPoints.size() < 3) return;

    glm::vec4 c = {color.r, color.g, color.b, color.a};

    // FanTriangulation from center
    for (size_t i=0; i<poolPoints.size(); i++) 
    {
      size_t next = (i + 1) % poolPoints.size();

      glm::vec2 pos = {position.x , position.y};
      glm::vec2 point1 = {poolPoints[i].x, poolPoints[i].y};
      glm::vec2 pointNext = {poolPoints[next].x, poolPoints[next].y};

      b.DrawTriangle(pos, point1, pointNext,c);
    

      if(line_color.a > 0)
      {
        glm::vec4 outColor = {line_color.r, line_color.g, line_color.b, line_color.a};
        b.DrawLine(point1, pointNext, 1.0f, outColor);
      }
    }
  }
  
  void Polygon::Update(float dt)
  {
    (void)dt;
  }

  void Polygon::SetSize(int _sides)
  {
    if(_sides > this->sides)
      poolPoints.resize(_sides);
  }

  void Polygon::SetColor(const Color& c)
  {
    this->color = c;
  }

  void Polygon::SetOutlineColor(const Color& c)
  {
    this->line_color = c;
  }

  void Polygon::SetPosition(float x, float y)
  {
    this->position.x = x;
    this->position.y = y;
  }

  void Polygon::SetPosition(const Vector2& new_pos)
  {
    this->position = new_pos;
  }

  const Vector2& Polygon::GetPosition(void) const
  {
    return this->position;
  }

  Vector2 Polygon::GetCenter(void) 
  {
    Vector2 pCenter;
    float x, y;
    for(auto& p : poolPoints)
    {
      x += p.x;
      y += p.y;
    }

    pCenter.x = x / this->sides;
    pCenter.y = y / this->sides;

    return pCenter;
  }
}
