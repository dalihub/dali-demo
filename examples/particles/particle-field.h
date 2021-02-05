#ifndef PARTICLES_PARTICLE_FIELD_H_
#define PARTICLES_PARTICLE_FIELD_H_
/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include <vector>
#include "dali/public-api/math/vector2.h"
#include "dali/public-api/math/vector3.h"
#include "dali/public-api/math/vector4.h"
#include "dali/public-api/rendering/geometry.h"
#include "float-rand.h"
#include "utils.h"

struct ParticleField
{
  float         mSize;
  Dali::Vector3 mBoxSize;
  Dali::Vector3 mParticlesPerAxis;
  float         mSizeVariance;
  float         mNoiseAmount; // affects color, motion (phase), twinkle (frequency, phase, size, opacity),
  float         mDisperse;
  float         mMotionScale;
  float         mMotionCycleLength; // seconds
  float         mTwinkleFrequency;  // per motion cycle
  float         mTwinkleSizeScale;
  float         mTwinkleOpacityWeight;

  Dali::Vector3 GetParticlesPerAxisSafe() const
  {
    using namespace Dali;
    return Vector3(std::max(1.f, FastFloor(mParticlesPerAxis.x)),
                   std::max(1.f, FastFloor(mParticlesPerAxis.y)),
                   std::max(1.f, FastFloor(mParticlesPerAxis.z)));
  }

  Dali::Geometry MakeGeometry() const
  {
    using namespace Dali;
    FloatRand frandPath;
    FloatRand frandSeed;
    FloatRand frandPos;
    FloatRand frandDisperse;
    FloatRand frandSize;

    struct Vertex
    {
      Vector3 aPosition;
      float   aSeed;
      Vector4 aPath;
      Vector2 aSubPosition;
      float   aSize;
    };

    const int numPatternVertices                = 6;
    Vector2   vertexPattern[numPatternVertices] = {
      Vector2(-1.f, 1.f),
      Vector2(-1.f, -1.f),
      Vector2(1.f, 1.f),
      Vector2(1.f, 1.f),
      Vector2(-1.f, -1.f),
      Vector2(1.f, -1.f),
    };

    Vector3 particlesPerAxis = GetParticlesPerAxisSafe();
    auto    numParticles     = particlesPerAxis.x * particlesPerAxis.y * particlesPerAxis.z;

    std::vector<Vertex> vertices;
    vertices.reserve(numParticles * numPatternVertices);

    Vector3 invBoxSize(1. / std::max(mBoxSize.x, 1.f),
                       1. / std::max(mBoxSize.y, 1.f),
                       1. / std::max(mBoxSize.z, 1.f));
    Vector3 spacing(mBoxSize.x / particlesPerAxis.x,
                    mBoxSize.y / particlesPerAxis.y,
                    mBoxSize.z / particlesPerAxis.z);
    auto    offset = (mBoxSize - spacing) * .5;
    int     nx     = particlesPerAxis.x;
    int     ny     = particlesPerAxis.y;
    int     nxy    = nx * ny;
    for(size_t i = 0; i < numParticles; ++i)
    {
      Vertex v;
      float  x    = float(i % nx);
      float  y    = float((i / nx) % ny);
      float  z    = float(i / nxy);
      v.aPosition = Vector3(x, y, z) * spacing - offset;

      Vector3 disperseDir(frandDisperse() - .5, frandDisperse() - .5, frandDisperse() - .5);
      disperseDir.Normalize();

      v.aPosition += disperseDir * (frandDisperse() * mDisperse);
      v.aPosition *= invBoxSize;

      v.aSeed = frandSeed() * mNoiseAmount;
      v.aPath = Vector4(frandPath() - .5, frandPath() - .5, frandPath() - .5, frandPath() - .5) * mMotionScale;

      const float size = mSize * ((1.f + (frandSize() - .5) * mSizeVariance) * .5f);
      for(int j = 0; j < numPatternVertices; ++j)
      {
        v.aSubPosition = vertexPattern[j];
        v.aSize        = size;
        vertices.push_back(v);
      }
    }

    VertexBuffer vertexBuffer = VertexBuffer::New(Property::Map()
                                                    .Add("aPosition", Property::VECTOR3)
                                                    .Add("aSeed", Property::FLOAT)
                                                    .Add("aPath", Property::VECTOR4)
                                                    .Add("aSubPosition", Property::VECTOR2)
                                                    .Add("aSize", Property::FLOAT));
    vertexBuffer.SetData(vertices.data(), vertices.size());

    Geometry geometry = Geometry::New();
    geometry.AddVertexBuffer(vertexBuffer);
    geometry.SetType(Geometry::TRIANGLES);
    return geometry;
  }
};

#endif //PARTICLES_PARTICLE_FIELD_H_
