// | -------------------------------
#pragma once
// | -------------------------------
#include "Engine/Component/Component.hpp"
#include "Game/Entity/Entity.hpp"
#include "Engine/Utils/Color.hpp"
#include "Engine/Utils/Vector2.hpp"
// | -------------------------------
#include "SDL3/SDL_rect.h"
// | -------------------------------
#include <memory>
#include <vector>
// | -------------------------------
#define RECTANGLE_SIZE_LINES 4
#define TRIANGLE_SIZE_LINES 3
// | -------------------------------

namespace ENG
{
  // Forward
  class Batcher;

  class Geometry : public APP::Entity
  {
    public:
      virtual void SetPosition(float x, float y) = 0;
      void Draw(Batcher& b) const override = 0;
      void Update(float dt) override = 0;
    public:
      static int amountGeometries;
  };
  
  class Line : public Geometry
  {
    public:
      Line(float x, float y,float xw, float yh, const Color& c);
      Line(const Vector2& start, const Vector2& end, const Color& c);
      ~Line() override{}

      void Draw(Batcher& batching) const override;
      void Update(float dt) override;
    
      void SetColor(const Color& c);
      void SetPosition(float x, float y) override;
      void SetPosition(float x, float y, float xw, float yh);
      void SetPosition(const Vector2& start, const Vector2& end);
      const Vector2& GetPosition(void) const;
      Vector2 GetCenter(void);
    private:
      std::shared_ptr<ENG::IName> name;
      Vector2 pStart;
      Vector2 pEnd;
      Color color;
  };

  class Rectangle : public Geometry
  {
    public:
      Rectangle(float x, float y, float w, float h, const Color& c);
      Rectangle(float x, float y, float w, float h, const Color& c, const Color& outColor);
      Rectangle(float xy, float wh, const Color& c);
      Rectangle(float xy, float wh, const Color& c, const Color& outColor);
      Rectangle(const Vector2& position, const Vector2& size, const Color& c);
      Rectangle(const Vector2& position, const Vector2& size, const Color& c, const Color& outColor);
      ~Rectangle() {}
  
      void Draw(Batcher& batching) const override;
      void Update(float dt) override;

      void SetSize(float width, float height);
      void SetColor(const Color& c);
      void SetOutlineColor(const Color& c);
      void SetPosition(float x, float y) override;
      void SetPosition(const Vector2& new_pos);
      const Vector2& GetPosition() const;
      Vector2 GetCenter(void);
    private:
      SDL_FRect rect;
      SDL_FPoint points[RECTANGLE_SIZE_LINES + 1];
      Vector2 position;
      Color color;
      Color line_color;
  };

  class Circle : public Geometry
  {
    public:
      Circle(float x, float y, float diameter, const Color& c);
      ~Circle() {}

      void Draw(Batcher& batching) const override;
      void Update(float dt) override;

      void SetSize(float diameter);
      void SetColor(const Color& c) ;
      void SetOutlineColor(const Color& c) ;
      void SetPosition(float x, float y) override; 
      void SetPosition(const Vector2& new_pos) ;
      const Vector2& GetPosition() const ;
      Vector2 GetCenter(void) ;

    private:
      float diameter = 0;
      Vector2 position;
      Color color;
      Color line_color;
  };

  class Triangle : public Geometry
  {
    public:
      Triangle(const Vector2& a, const Vector2& b, const Vector2& c, const Color& color);
      ~Triangle() {}

      void Draw(Batcher& batching) const override;
      void Update(float dt) override;
    private:
      void SetSize(const Vector2& a, const Vector2& b, const Vector2& c);
      void SetColor(const Color& c) ;
      void SetOutlineColor(const Color& c) ;
      void SetPosition(float x, float y) override; 
      void SetPosition(const Vector2& new_pos) ;
      const Vector2& GetPosition() const ;
      Vector2 GetCenter(void) ;

    private:
      Vector2 points[4];
      Vector2 position;
      Color color;
      Color line_color;
  };

  class Polygon : public Geometry
  {
    public:
      Polygon(float x, float y, int sides, const Color& color);
      ~Polygon() {}
    
      void Draw(Batcher& batching) const override;
      void Update(float dt) override;
      
      void AddLine(float x, float y);
      void AddLine(const Vector2& point);

      void SetSize(int new_sides);
      void SetColor(const Color& c) ;
      void SetOutlineColor(const Color& color) ;
      void SetPosition(float x, float y) override; 
      void SetPosition(const Vector2& new_pos) ;
      const Vector2& GetPosition() const ;
      Vector2 GetCenter(void) ;
    
    private:
      Vector2 position;
      int sides;
      std::vector<Vector2> poolPoints;
      Color color;
      Color line_color;
  };
  // Class multi size, like 5 lados.
}
