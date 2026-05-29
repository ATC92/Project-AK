/// | ------------------------------------ |
#include "RBatch.hpp"
/// | ------------------------------------ |
#include "Engine/Render/RImage.hpp"
#include "Engine/Render/RVertex.hpp"
#include "Engine/Render/RShader.hpp"
#include "Engine/Render/Render.hpp"
#include "Engine/Utils/Geometry.hpp"
#include "Engine/Utils/Path.hpp"
#include "Engine/Utils/Vector2.hpp"
/// | ------------------------------------ |
//#include <cstdio>
#include <glm/detail/qualifier.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float4.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/type_ptr.hpp>
/// | ------------------------------------ |
#include <cstddef>
#include <cstdint>
#include <cmath>
#include <memory>
/// | ------------------------------------ |

namespace ENG
{
  void Batcher::Init()
  {
    /// Starting buffer on CPU
    m_VertexBufferBase = new Vertex[MAX_VERTS];

    // EBO
    glGenBuffers(1, &m_EBO);

    // VAO
    glGenVertexArrays(1,&m_VAO);
    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    
    // VBO Dinamyc -> Each frame change
    glGenBuffers(1,&m_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, MAX_VERTS * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

    // Vertex attributes
    // | -> Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(Vertex), (void*)offsetof(Vertex, Position));

    // | -> Color
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,Color));

    // | -> TextCord
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCord));

    // | -> TexIndex
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexIndex));

    /// EBO -> Static index (QUAD always same patron)
    uint32_t* index = new uint32_t[MAX_INDICES];
    uint32_t offset = 0;
    for (uint32_t i=0; i<MAX_INDICES; i+=6)
    {
      index[i + 0] = offset + 0;
      index[i + 1] = offset + 1;
      index[i + 2] = offset + 2;
      index[i + 3] = offset + 2;
      index[i + 4] = offset + 3;
      index[i + 5] = offset + 0;
      offset += 4;
    }

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, MAX_INDICES * sizeof(uint32_t), index, GL_STATIC_DRAW);
    delete [] index;

    glBindVertexArray(0);

    // White Texture 1x1
    glGenTextures(1, &m_WhiteTexture);
    glBindTexture(GL_TEXTURE_2D, m_WhiteTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    uint32_t white = 0xFFFFFFFF;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, &white);
    glBindTexture(GL_TEXTURE_2D,0);
    m_TextureSlots[0] = m_WhiteTexture;


    // Shader
    auto& path = Path::Get();
    

    auto _vPath = path.ReadFile(SHADER_PATH "texture.vs" );
    auto _fPath = path.ReadFile(SHADER_PATH "texture.fs" );

    this->shader = Shader( _vPath, _fPath);
    glUseProgram(this->shader.GetProgram());

    // Asign samplers one time
    int samplers[MAX_TEXTURES];
    for (int i=0; i < MAX_TEXTURES; i++)
      samplers[i] = i;
    glUniform1iv(glGetUniformLocation(this->shader.GetProgram(), "u_Textures"), MAX_TEXTURES, samplers);

  }

  void Batcher::Shutdown()
  {
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_EBO);
    glDeleteTextures(1, &m_WhiteTexture);
    glDeleteProgram(this->shader.GetProgram());
    delete[] m_VertexBufferBase;
  }

  void Batcher::Begin()
  {
    Vector2 wS = Render::Get().GetScreenSize();
    //printf("ScreenSize: %.1f %.1f\n", wS.x, wS.y);
    glm::mat4 proj = glm::ortho(0.0f, wS.x,wS.y,0.0f, -1.0f, 1.0f);
    glUniformMatrix4fv(
      glGetUniformLocation(shader.GetProgram(), "u_ViewProjection"),
    1 , GL_FALSE, &proj[0][0]
    );

    StartBatch();
  }

  void Batcher::End()
  {
    Flush();
  }

  void Batcher::StartBatch()
  {
    m_VertexBufferPtr = m_VertexBufferBase;
    m_IndexCount = 0;
    m_TextureSlotIndex = 1;
  }

  void Batcher::Flush()
  {
    //glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    if(m_IndexCount == 0) return;

    // Push vertex to VRam
    uint32_t dataSize = (uint32_t)((uint8_t*)m_VertexBufferPtr - (uint8_t*)m_VertexBufferBase);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, dataSize, m_VertexBufferBase);
    //printf("After bufferSubData: %d\n", glGetError());

    for( uint32_t i=0; i<m_TextureSlotIndex; i++)
    // Bind textures on slots
    {
      glActiveTexture(GL_TEXTURE0 + i);
      glBindTexture(GL_TEXTURE_2D, m_TextureSlots[i]);
    }
    //printf("After textures: %d\n", glGetError());
    //printf("IndexCount: %d | DataSize: %d | TexSlots: %d\n", m_IndexCount, dataSize, m_TextureSlotIndex);
    
    glUseProgram(shader.GetProgram());
    glBindVertexArray(m_VAO);
    //printf("After VAO: %d\n", glGetError());
    glDrawElements(GL_TRIANGLES, m_IndexCount, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
    //printf("After draw: %d\n", glGetError());

    StartBatch();  /// Reset
  }

  float Batcher::GetTextureIndex(const RImage& texture)
  {
    GLuint id = texture.GetID();
  
    // Search if exist on index;
    for(uint32_t i = 1; i< m_TextureSlotIndex; i++)
    {
      if(m_TextureSlots[i] == id)
        return (float)i;
    }

    // if Slots full -> Flush and reset
    if(m_TextureSlotIndex >= MAX_TEXTURES)
      Flush();

    m_TextureSlots[m_TextureSlotIndex] = id;
    return (float)m_TextureSlotIndex++;
  }

  void Batcher::DrawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color)
  {
    if (m_IndexCount >= MAX_INDICES)
    {
      Flush();
      StartBatch();
    }


    // Bottom-left
    m_VertexBufferPtr->Position  = {pos.x, pos.y, 0.0f};
    m_VertexBufferPtr->Color     = color;
    m_VertexBufferPtr->TexCord   = {0.0f, 0.0f};
    m_VertexBufferPtr->TexIndex  = 0.0f;
    m_VertexBufferPtr++;

    // Bottom-right
    m_VertexBufferPtr->Position  = {pos.x + size.x, pos.y, 0.0f};
    m_VertexBufferPtr->Color     = color;
    m_VertexBufferPtr->TexCord   = {1.0f, 0.0f};
    m_VertexBufferPtr->TexIndex  = 0.0f;
    m_VertexBufferPtr++;

    // Top-right
    m_VertexBufferPtr->Position  = {pos.x + size.x, pos.y + size.y, 0.0f};
    m_VertexBufferPtr->Color     = color;
    m_VertexBufferPtr->TexCord   = {1.0f, 1.0f};
    m_VertexBufferPtr->TexIndex  = 0.0f;
    m_VertexBufferPtr++;

    // Top-left
    m_VertexBufferPtr->Position  = {pos.x, pos.y + size.y, 0.0f};
    m_VertexBufferPtr->Color     = color;
    m_VertexBufferPtr->TexCord   = {0.0f, 1.0f};
    m_VertexBufferPtr->TexIndex  = 0.0f;
    m_VertexBufferPtr++;

    m_IndexCount += 6;
  }

  void Batcher::DrawTexture(const glm::vec2& pos, const glm::vec2& size, std::shared_ptr<RImage> texture, const glm::vec4& tint)
  {
    if (m_IndexCount >= MAX_INDICES)
    {
      Flush();
      StartBatch();
    }

    const RImage* tex = texture.get();
    float textureIndex = GetTextureIndex(*tex);

    // Bottom-left
    m_VertexBufferPtr->Position  = {pos.x, pos.y, 0.0f};
    m_VertexBufferPtr->Color     = tint;
    m_VertexBufferPtr->TexCord   = {0.0f, 0.0f};
    m_VertexBufferPtr->TexIndex  = textureIndex;
    m_VertexBufferPtr++;

    // Bottom-right
    m_VertexBufferPtr->Position  = {pos.x + size.x, pos.y, 0.0f};
    m_VertexBufferPtr->Color     = tint;
    m_VertexBufferPtr->TexCord   = {1.0f, 0.0f};
    m_VertexBufferPtr->TexIndex  = textureIndex;
    m_VertexBufferPtr++;

    // Top-right
    m_VertexBufferPtr->Position  = {pos.x + size.x, pos.y + size.y, 0.0f};
    m_VertexBufferPtr->Color     = tint;
    m_VertexBufferPtr->TexCord   = {1.0f, 1.0f};
    m_VertexBufferPtr->TexIndex  = textureIndex;
    m_VertexBufferPtr++;

    // Top-left
    m_VertexBufferPtr->Position  = {pos.x, pos.y + size.y, 0.0f};
    m_VertexBufferPtr->Color     = tint;
    m_VertexBufferPtr->TexCord   = {0.0f, 1.0f};
    m_VertexBufferPtr->TexIndex  = textureIndex;
    m_VertexBufferPtr++;

    m_IndexCount += 6;
  }

  void Batcher::DrawCircle(const glm::vec2& center, float radius, const glm::vec4& color, uint32_t segments)
  {
    if (m_IndexCount >= MAX_INDICES)
    {
      Flush();
      StartBatch();
    }

    float angleIncrement = 2.0f * glm::pi<float>() / static_cast<float>(segments);
    float textureIndex = 0.0f; // White Texture

    for (uint32_t i = 0; i < segments; ++i)
    {
      float angle1 = static_cast<float>(i) * angleIncrement;
      float angle2 = static_cast<float>(i + 1) * angleIncrement;

      glm::vec2 p1 = {center.x + radius * std::cos(angle1), center.y + radius * std::sin(angle1)};
      glm::vec2 p2 = {center.x + radius * std::cos(angle2), center.y + radius * std::sin(angle2)};

      // Center vertex
      m_VertexBufferPtr->Position = {center.x, center.y, 0.0f};
      m_VertexBufferPtr->Color    = color;
      m_VertexBufferPtr->TexCord  = {0.5f, 0.5f}; // Center of a dummy texture
      m_VertexBufferPtr->TexIndex = textureIndex;
      m_VertexBufferPtr++;

      // Point 1 on circumference
      m_VertexBufferPtr->Position = {p1.x, p1.y, 0.0f};
      m_VertexBufferPtr->Color    = color;
      m_VertexBufferPtr->TexCord  = {0.5f + 0.5f * std::cos(angle1), 0.5f + 0.5f * std::sin(angle1)};
      m_VertexBufferPtr->TexIndex = textureIndex;
      m_VertexBufferPtr++;

      // Point 2 on circumference
      m_VertexBufferPtr->Position = {p2.x, p2.y, 0.0f};
      m_VertexBufferPtr->Color    = color;
      m_VertexBufferPtr->TexCord  = {0.5f + 0.5f * std::cos(angle2), 0.5f + 0.5f * std::sin(angle2)};
      m_VertexBufferPtr->TexIndex = textureIndex;
      m_VertexBufferPtr++;

      m_IndexCount += 3; // Each segment adds one triangle

      if (m_IndexCount >= MAX_INDICES)
      {
        Flush();
        StartBatch();
      }
    }
  }

  void Batcher::DrawCircleOutLine(const glm::vec2& center, float radius, const glm::vec4& outcolor)
  {
    uint32_t segments = 32;
    float angleIncrement = 2.0f * glm::pi<float>() / static_cast<float>(segments);
    float thickness = 2.0f; // Default thickness for outline

    for (uint32_t i = 0; i < segments; ++i)
    {
      float angle1 = static_cast<float>(i) * angleIncrement;
      float angle2 = static_cast<float>(i + 1) * angleIncrement;

      glm::vec2 p1 = {center.x + radius * std::cos(angle1), center.y + radius * std::sin(angle1)};
      glm::vec2 p2 = {center.x + radius * std::cos(angle2), center.y + radius * std::sin(angle2)};

      DrawLine(p1, p2, thickness, outcolor);
    }
  }

  void Batcher::DrawTriangle(const glm::vec2& pointA, const glm::vec2& pointB, const glm::vec2& pointC, const glm::vec4& color)
  {
    if (m_IndexCount >= MAX_INDICES)
    {
      Flush();
      StartBatch();
    }

    float textureIndex = 0.0f; // White Texture

    // Point A
    m_VertexBufferPtr->Position  = {pointA.x, pointA.y, 0.0f};
    m_VertexBufferPtr->Color     = color;
    m_VertexBufferPtr->TexCord   = {0.0f, 0.0f};
    m_VertexBufferPtr->TexIndex  = textureIndex;
    m_VertexBufferPtr++;

    // Point B
    m_VertexBufferPtr->Position  = {pointB.x, pointB.y, 0.0f};
    m_VertexBufferPtr->Color     = color;
    m_VertexBufferPtr->TexCord   = {0.5f, 1.0f};
    m_VertexBufferPtr->TexIndex  = textureIndex;
    m_VertexBufferPtr++;

    // Point C
    m_VertexBufferPtr->Position  = {pointC.x, pointC.y, 0.0f};
    m_VertexBufferPtr->Color     = color;
    m_VertexBufferPtr->TexCord   = {1.0f, 0.0f};
    m_VertexBufferPtr->TexIndex  = textureIndex;
    m_VertexBufferPtr++;

    m_IndexCount += 3;
  }

  void Batcher::DrawLine(const glm::vec2& a, const glm::vec2& b, float thickness, const glm::vec4& color)
  {
    if (m_IndexCount >= MAX_INDICES)
    {
      Flush();
      StartBatch();
    }

    Vector2 v = {a.x, a.y};
    Vector2 vb = {b.x,b.y};
    Vector2 r = v-vb;
    r.Normalize();

    glm::vec2 direction = {r.x,r.y};
    glm::vec2 normal    = glm::vec2(-direction.y, direction.x) * (thickness * 0.5f);

    // Vertices for the thick line quad
    glm::vec2 p1 = a - normal;
    glm::vec2 p2 = b - normal;
    glm::vec2 p3 = b + normal;
    glm::vec2 p4 = a + normal;

    float textureIndex = 0.0f; // White Texture

    // Bottom-left (p1)
    m_VertexBufferPtr->Position  = {p1.x, p1.y, 0.0f};
    m_VertexBufferPtr->Color     = color;
    m_VertexBufferPtr->TexCord   = {0.0f, 0.0f};
    m_VertexBufferPtr->TexIndex  = textureIndex;
    m_VertexBufferPtr++;

    // Bottom-right (p2)
    m_VertexBufferPtr->Position  = {p2.x, p2.y, 0.0f};
    m_VertexBufferPtr->Color     = color;
    m_VertexBufferPtr->TexCord   = {1.0f, 0.0f};
    m_VertexBufferPtr->TexIndex  = textureIndex;
    m_VertexBufferPtr++;

    // Top-right (p3)
    m_VertexBufferPtr->Position  = {p3.x, p3.y, 0.0f};
    m_VertexBufferPtr->Color     = color;
    m_VertexBufferPtr->TexCord   = {1.0f, 1.0f};
    m_VertexBufferPtr->TexIndex  = textureIndex;
    m_VertexBufferPtr++;

    // Top-left (p4)
    m_VertexBufferPtr->Position  = {p4.x, p4.y, 0.0f};
    m_VertexBufferPtr->Color     = color;
    m_VertexBufferPtr->TexCord   = {0.0f, 1.0f};
    m_VertexBufferPtr->TexIndex  = textureIndex;
    m_VertexBufferPtr++;

    m_IndexCount += 6;
  }
}

