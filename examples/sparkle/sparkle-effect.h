#ifndef DALI_SPARKLE_EFFECT_H
#define DALI_SPARKLE_EFFECT_H

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;
using Dali::Toolkit::ImageView;

/************************************************************/
/* Custom sparkle effect shader******************************/
/************************************************************/

namespace SparkleEffect
{
  // uniform which controls the position of particle on the path
  const std::string PERCENTAGE_UNIFORM_NAME( "uPercentage" );
  // uniform array of particle color, set their value as the PARTICLE_COLORS given below
  const std::string PARTICLE_COLOR_UNIFORM_NAME("uParticleColors[");
  // uniform array of particle opacity
  const std::string OPACITY_UNIFORM_NAME("uOpacity[");
  // uniform which offsets the path control point, with this values >=0, the paths are squeezed towards the GatheringPoint
  const std::string ACCELARATION_UNIFORM_NAME("uAcceleration");
  // uniform which indicates the ongoing tap animations
  const std::string TAP_INDICES_UNIFORM_NAME("uTapIndices");
  // uniform which controls how much the offset of the midpoints relative to the start/end points of the cubic bezier curve when the path is squeezed for tap animation
  const std::string TAP_OFFSET_UNIFORM_NAME("uTapOffset[");
  // uniform which gives the position of the tapping, in this way the particles will be pushed away from this point
  const std::string TAP_POINT_UNIFORM_NAME("uTapPoint[");
  // uniform which trigger the break animation, set to 1.0 when break animation is playing, otherwise set to 0.0
  const std::string BREAK_UNIFORM_NAME("uBreak");

  /****************particle colors******************/

  struct ParticleColor
  {
    Vector3 RGB;
    Vector2 AlphaRange;
    unsigned int numParticle;
  };

  ParticleColor PARTICLE_COLORS[]=
  {
    { Vector3( 0.f,   240.f, 255.f )/255.f, Vector2( 0.2f, 1.f ),  22 }, // 00f0ff, opacity 20%~100%
    { Vector3( 89.f,  151.f, 239.f )/255.f, Vector2( 0.2f, 0.5f ), 12 }, // 5997ef, opacity 20%~50%
    { Vector3( 181.f, 181.f, 207.f )/255.f, Vector2( 0.5f, 1.f ),  22 }, // b5b5cf, opacity 50%~100%
    { Vector3( 147.f, 147.f, 170.f )/255.f, Vector2( 0.5f, 0.5f ), 22 }, // 9393aa, opacity 50%~50%
    { Vector3( 145.f, 145.f, 201.f )/255.f, Vector2( 1.f, 1.f ),   12 }, // 91bdc9, opacity 100%~100%
    { Vector3( 145.f, 145.f, 201.f )/255.f, Vector2( 0.2f, 0.2f ), 21 }  // 91bdc9, opacity 20%~20%
  };
  const unsigned int NUM_COLOR( sizeof( PARTICLE_COLORS ) / sizeof( PARTICLE_COLORS[0] ) );

  /***************particle moving paths********************/

  typedef int MovingPath[12];

  // these paths are defined inside the circle which has the center at (250, 250) and the radius as 250
  MovingPath PATHS[]=
  { // each path is composed of two cubic b-curves: (p0, p1, p2, p3) & (p3, p4, p5, p0)
    // p0        p1        p2       p3        p4        p5
    { 280,273,  386,41,  489,141,  491,199,  494,256,  230,394  },
    { 129,226,  357,120, 150,491,  291,406,  433,320,  47,283   },
    { 96,264,   356,133, 446,196,  370,297,  294,399,  -169,384 },
    { 345,110,  359,186,  14,393,  4,247,  -6,101,  321,-28 },
    { 166,161,  128,353,  566,200,  487,304,  413,403,  203,-32 },
    { 193,286,  106,331,  206,569,  334,477,  462,385,  279,240 },
    { 336,247,  293,232,  301,465,  346,479,  390,493,  374,261 },
    { 250,72,  314,72,  332,495,  250,497,  168,499,  161,72 },
    { 48,387,  32,241,  452,558,  433,358,  411,121,  62,523 },
    { 300,32,  159,27,  442,568,  186,492,  -70,415,  551,41 },
    { 479,150,  503,203,  216,403,  163,298,  110,193,  448,78 },
    { 346,75,  311,97,  336,196,  389,160,  442,123,  383,51 },
    { 90,61,  54,96,  218,373,  294,300,  370,227,  141,11 },
    { 126,225,  240,280,  378,29,  221,16,  64,4,  11,170 },
    { 308,101,  243,22,  -10,271,  22,352,  49,422,  396,208 },
    { 193,188,  174,302,  502,389,  500,250,  498,111,  212,72 },
    { 227,3,  16,35,  577,309,  428,423,  279,537,  438,-28 },
    { 410,58,  387,18,  22,179,  154,277,  286,374,  459,142 },
    { 178,272,  109,299,  144,429,  218,396,  293,362,  221,254 },
    { 247,46,  98,5,  -91,357,  160,431,  412,505,  397,88 },
    { 41,112,  22,144,  123,273,  158,187,  192,101,  75,56 },
    { 8,300,  23,340,  267,294,  238,218,  209,142,  -20,226 },
    { 112,256,  24,270,  -1,470,  154,433,  308,396,  201,242 },
    { 212,277,  267,346,  509,202,  452,103,  398,8,  150,199 },
    { 154,205,  146,287,  496,282,  492,194,  488,107,  160,140 },
    { 281,350,  365,318,  415,476,  332,482,  248,489,  204,379 },
    { 327,23,  346,81,  154,319,  123,207,  92,95,  313,-21 },
    { 323,233,  283,307,  454,420,  478,354,  501,288,  374,136 },
    { 318,186,  311,252,  488,248,  481,168,  474,87,  328,76 },
    { 7,192,  -10,270,  249,398,  269,307,  290,216,  25,111 },
    { 148,22,  98,22,  25,458,  125,458,  225,458,  198,22 },
    { 349,32,  307,39,  492,416,  399,446,  305,477,  460,16 },
    { 147,474,  222,554,  392,154,  486,240,  581,325,  73,394 },
    { 57,186,  13,200,  51,398,  114,374,  178,349,  97,174 },
    { 257,192,  198,188,  162,345,  240,349,  319,354,  316,197 },
    { 242,4,  283,21,  30,172,  81,215,  133,257,  209,-10 },
    { 149,408,  165,442,  472,340,  444,275,  416,210,  120,348 },
    { 106,271,  136,359,  483,370,  422,186,  360,2,  76,186 },
    { 120,146,  29,224,  469,262,  346,390,  222,518,  393,-87 },
    { 318,265,  415,280,  398,537,  247,491,  96,446,  222,249 },
    { 171,275,  207,246,  274,469,  237,497,  199,525,  139,300 },
    { 196,84,  135,105,  256,510,  334,486,  412,462,  280,55 },
    { 485,314,  452,170,  158,606,  111,411,  55,179,  515,446 },
    { 134,54,  266,4,  175,607,  392,451,  609,296,  -100,144 },
    { 3,229,  -1,287,  334,383,  350,267,  366,150,  10,151 },
    { 105,115,  146,125,  154,227,  92,209,  30,192,  62,105 },
    { 343,20,  388,42,  323,357,  228,313,  132,269,  278,-10 },
    { 362,186,  271,274,  60,82,  204,19,  349,-44,  453,97 },
    { 145,128,  181,32,  501,185,  498,272,  495,347,  97,257 },
    { 286,172,  342,274,  59,463,  16,331,  -27,198,  231,69 },
    { 194,7,  404,-32,  -38,410,  140,469,  317,528,  -16,45 },
    { 39,120,  48,74,  445,109,  352,244,  259,379,  20,215 },
    { 328,247,  402,250,  411,384,  330,377,  248,370,  281,244 },
    { 189,56,  317,-31,  610,240,  396,392,  183,543,  61,144 },
    { 402,53,  430,77,  376,231,  315,161,  255,91,  351,10 },
    { 496,218,  494,260,  249,296,  251,214,  254,133,  498,139 },
    { 381,210,  469,195,  557,376,  399,391,  241,407,  292,226 },
    { 297,263,  267,346,  -8,289,  14,176,  35,69,  331,168 },
    { 329,187,  363,263,  30,371,  5,287,  -19,203,  302,128 },
    { 257,354,  168,351,  171,516,  252,496,  333,475,  340,356 },
    { 106,60,  107,121,  366,284,  359,168,  352,52,  105,14 },
    { 178,257,  240,314,  115,476,  71,421,  28,367,  98,182 },
    { 163,213,  191,273,  22,327,  3,248,  -17,170,  118,113 },
    { 459,117,  500,185,  297,390,  248,311,  199,232,  416,46 },
    { 270,3,  317,-14,  528,375,  434,407,  339,440,  223,19 },
    { 88,76,  130,68,  78,485,  176,483,  274,482,  -22,96 },
    { 422,428,  378,528,  88,205,  26,317,  -36,428,  467,328 },
    { 414,127,  460,125,  489,325,  421,322,  353,320,  372,128 },
    { 227,197,  281,174,  367,311,  294,340,  221,370,  173,220 },
    { 180,14,  147,44,  436,104,  401,161,  366,219,  207,-10 },
    { 400,367,  395,404,  71,406,  77,336,  82,265,  407,300 },
    { 396,222,  396,316,  71,439,  70,245,  68,51,  396,132 },
    { 342,109,  454,153,  49,332,  208,413,  367,494,  8,-23 },
    { 147,167,  222,137,  266,169,  231,199,  197,229,  129,178 },
    { 227,272,  310,243,  277,313,  322,266,  367,219,  207,313 },
    { 279,192,  339,233,  396,211,  367,182,  338,152,  228,194 },
    { 236,20,  283,75,  346,26,  338,71,  330,116,  207,17 },
    { 129,83,  164,23,  158,14,  179,11,  200,8,  91,78 },
    { 86,231,  129,293,  164,421,  104,348,  44,275,  66,200 },
    { 193,328,  197,278,  240,348,  276,305,  311,263,  199,354 },
    { 231,364,  241,209,  309,104,  326,236,  342,367,  225,424 },
    { 414,230,  398,328,  446,445,  467,363,  489,281,  373,254 },
    { 289,122,  332,123,  348,161,  322,158,  297,156,  275,125 },
    { 142,235,  199,308,  402,229,  283,218,  164,206,  130,206 },
    { 174,396,  210,387,  328,501,  246,455,  165,409,  138,394 },
    { 288,388,  366,357,  372,458,  393,400,  414,343,  249,431 },
    { 351,278,  409,369,  497,316,  437,288,  376,260,  351,243 },
    { 87,134,  181,77,  311,121,  206,140,  101,160,  61,159 },
    { 95,195,  126,208,  133,258,  110,236,  88,215,  95,195 },
    { 140,293,  158,330,  169,275,  184,299,  198,323,  126,313 },
    { 336,319,  383,357,  388,278,  393,333,  397,388,  311,325 },
    { 338,107,  434,209,  -37,469,  151,287,  338,104,  285,50 },
    { 403,134,  446,182,  378,318,  386,233,  393,149,  360,98 },
    { 366,82,  413,93,  416,158,  390,118,  364,78,  336,75 },
    { 448,188,  448,230,  465,269,  470,225,  474,181,  448,177 },
    { 121,398,  142,418,  126,475,  111,436,  96,396,  100,382 },
    { 40,296,  90,352,  170,310,  143,350,  116,391,  7,300 },
    { 25,203,  45,241,  70,204,  45,248,  19,293,  4,204 },
    { 243,222,  225,275,  345,256,  296,237,  247,218,  249,199 },
    { 159,149,  282,133,  284,199,  226,191,  169,184,  147,160 },
    { 149,257,  290,322,  151,374,  166,338,  182,302,  116,263 },
    { 255,285,  354,327,  234,287,  279,327,  323,367,  193,290 },
    { 188,220,  353,190,  290,354,  348,293,  407,231,  152,248 },
    { 305,122,  382,174,  402,229,  366,198,  329,167,  297,127 },
    { 378,260,  406,267,  390,330,  384,293,  377,257,  366,263 },
    { 178,396,  357,365,  273,461,  248,431,  223,401,  157,412 },
    { 180,89,  258,88,  302,94,  255,115,  207,136,  166,96 },
    { 81,197,  139,232,  39,257,  94,259,  150,261,  58,200 },
    { 314,89,  378,40,  383,38,  389,42,  395,45,  267,90 },
    { 371,141,  482,233,  508,244,  498,272,  488,299,  307,157 },
    { 339,348,  361,465,  382,477,  406,442,  430,406,  269,369 }
  };
  const unsigned int NUM_PARTICLE( sizeof( PATHS ) / sizeof( PATHS[0] ) );

  const float PARTICLE_SIZE = 13.f;

  const float ACTOR_SCALE = 0.704f; // resize 500*500 to 352*352, a bit smaller than 360*360
  const Vector3 ACTOR_POSITION( -176.f, -176.f, 1.f);

  const int MAXIMUM_ANIMATION_COUNT = 30;

  // Geometry format used by the SparkeEffect
  struct Vertex
  {
    Vertex( const Vector2& texCoord,
            const Vector2& aParticlePath0,
            const Vector2& aParticlePath1,
            const Vector2& aParticlePath2,
            const Vector2& aParticlePath3,
            const Vector2& aParticlePath4,
            const Vector2& aParticlePath5 )
    : aTexCoord( texCoord ),
      aParticlePath0( aParticlePath0 ),
      aParticlePath1( aParticlePath1 ),
      aParticlePath2( aParticlePath2 ),
      aParticlePath3( aParticlePath3 ),
      aParticlePath4( aParticlePath4 ),
      aParticlePath5( aParticlePath5 )
    {
    }

    Vector2 aTexCoord;
    Vector2 aParticlePath0;
    Vector2 aParticlePath1;
    Vector2 aParticlePath2;
    Vector2 aParticlePath3;
    Vector2 aParticlePath4;
    Vector2 aParticlePath5;
  };

  /**
   * Create a SparkleEffect object.
   * @return A handle to a newly allocated SparkleEffect
   */
  Shader New()
  {
    std::string vertexShader = DALI_COMPOSE_SHADER(
      precision highp float;\n
      \n
      attribute vec2  aTexCoord;\n
      uniform   mat4  uMvpMatrix;\n
      varying   vec2  vTexCoord;\n
      \n
      attribute vec2  aParticlePath0;\n
      attribute vec2  aParticlePath1;\n
      attribute vec2  aParticlePath2;\n
      attribute vec2  aParticlePath3;\n
      attribute vec2  aParticlePath4;\n
      attribute vec2  aParticlePath5;\n
      \n
      uniform float uPercentage;\n
      uniform float uPercentageMarked;\n
      uniform vec3  uParticleColors[NUM_COLOR];\n
      uniform float uOpacity[NUM_PARTICLE];\n
      uniform vec2  uTapIndices;
      uniform float uTapOffset[MAXIMUM_ANIMATION_COUNT];\n
      uniform vec2  uTapPoint[MAXIMUM_ANIMATION_COUNT];\n
      uniform float uAcceleration;\n
      uniform float uRadius;\n
      uniform float uScale;\n
      uniform float uBreak;\n
      \n
      varying lowp vec4 vColor;\n
      \n
      void main()\n
      {\n
        // we store the particle index inside texCoord attribute
        float idx = abs(aTexCoord.y)-1.0;\n
        \n
        // early out if the particle is invisible
        if(uOpacity[int(idx)]<1e-5)\n
        {\n
          gl_Position = vec4(0.0);\n
          vColor = vec4(0.0);\n
          return;\n
        }\n
        \n
        // As the movement along the b-curve has nonuniform speed with a uniform increasing parameter 'uPercentage'
        // we give different particles the different 'percentage' to make them looks more random
        float increment = idx / float(NUM_PARTICLE)*5.0;
        float percentage = mod(uPercentage +uAcceleration+increment, 1.0);
        \n
        vec2 p0; vec2 p1; vec2 p2; vec2 p3;
        // calculate the particle position by using the cubic b-curve equation
        if(percentage<0.5)\n // particle on the first b-curve
        {\n
          p0 = aParticlePath0;\n
          p1 = aParticlePath1;\n
          p2 = aParticlePath2;\n
          p3 = aParticlePath3;\n
        }\n
        else\n
        {\n
          p0 = aParticlePath3;\n
          p1 = aParticlePath4;\n
          p2 = aParticlePath5;\n
          p3 = aParticlePath0;\n
        }\n
        float t = mod( percentage*2.0, 1.0);\n
        vec2 position = (1.0-t)*(1.0-t)*(1.0-t)*p0 + 3.0*(1.0-t)*(1.0-t)*t*p1+3.0*(1.0-t)*t*t*p2 + t*t*t*p3;\n
        \n
        vec2 referencePoint = mix(p0,p3,0.5);\n
        float maxAnimationCount = float(MAXIMUM_ANIMATION_COUNT);\n
        \n
        for( float i=uTapIndices.x; i<uTapIndices.y; i+=1.0 )\n
        {
          int id = int( mod(i+0.5,maxAnimationCount ) );\n
          vec2 edgePoint = normalize(referencePoint-uTapPoint[id])*uRadius+vec2(uRadius);\n
          position = mix( position, edgePoint, uTapOffset[id] ) ;\n
        }\n
        \n
        position = mix( position, vec2( 250.0,250.0 ),uBreak*(1.0-uOpacity[int(idx)]) ) ;
        \n
        // vertex position on the mesh: (sign(aTexCoord.x), sign(aTexCoord.y))*PARTICLE_HALF_SIZE
        gl_Position = uMvpMatrix * vec4( position.x+sign(aTexCoord.x)*PARTICLE_HALF_SIZE/uScale,
                                         position.y+sign(aTexCoord.y)*PARTICLE_HALF_SIZE/uScale,
                                         0.0, 1.0 );\n
        \n
        // we store the color index inside texCoord attribute
        float colorIndex = abs(aTexCoord.x);
        vColor.rgb = uParticleColors[int(colorIndex)-1];\n
        vColor.a = fract(colorIndex) * uOpacity[int(idx)];\n
        \n
        // produce a 'seemingly' random fade in/out
        percentage = mod(uPercentage+increment+0.15, 1.0);\n
        float ramdomOpacity = (min(percentage, 0.25)-0.15+0.9-max(percentage,0.9))*10.0;\n
        vColor.a *=ramdomOpacity;\n
        \n
        vTexCoord = clamp(aTexCoord, 0.0, 1.0);\n
      }\n
    );

    std::string fragmentShader = DALI_COMPOSE_SHADER(
        precision highp float;\n
        uniform sampler2D sTexture;\n
        varying vec2      vTexCoord;\n
        \n
        varying lowp vec4 vColor;\n
        \n
        void main()\n
        {\n
          gl_FragColor = vColor;\n
          gl_FragColor.a *= texture2D(sTexture, vTexCoord).a;\n
        }\n
    );

    std::ostringstream vertexShaderStringStream;
    vertexShaderStringStream<< "#define NUM_COLOR "<< NUM_COLOR << "\n"
                            << "#define NUM_PARTICLE "<< NUM_PARTICLE << "\n"
                            << "#define PARTICLE_HALF_SIZE "<< PARTICLE_SIZE*ACTOR_SCALE/2.f << "\n"
                            << "#define MAXIMUM_ANIMATION_COUNT "<<MAXIMUM_ANIMATION_COUNT<<"\n"
                            << vertexShader;

    Shader handle = Shader::New( vertexShaderStringStream.str(), fragmentShader );

    // set the particle colors
    std::ostringstream oss;
    for( unsigned int i = 0; i < NUM_COLOR; i++ )
    {
      oss.str("");
      oss<< PARTICLE_COLOR_UNIFORM_NAME<< i << "]";
      handle.RegisterProperty(oss.str(), PARTICLE_COLORS[i].RGB);
    }
    handle.RegisterProperty( "uRadius", 250.f );
    handle.RegisterProperty( "uScale", ACTOR_SCALE );

    // set the initial uniform values

    for( unsigned int i = 0; i < NUM_PARTICLE; i++ )
    {
      oss.str("");
      oss<< OPACITY_UNIFORM_NAME << i << "]";
      handle.RegisterProperty(oss.str(), 1.f);
    }
    handle.RegisterProperty( PERCENTAGE_UNIFORM_NAME, 0.f );
    handle.RegisterProperty( ACCELARATION_UNIFORM_NAME, 0.f );
    handle.RegisterProperty( BREAK_UNIFORM_NAME, 0.f);
    handle.RegisterProperty( TAP_INDICES_UNIFORM_NAME, Vector2::ZERO);

    for( int i = 0; i < MAXIMUM_ANIMATION_COUNT; i++ )
    {
      oss.str("");
      oss<< TAP_OFFSET_UNIFORM_NAME << i << "]";
      handle.RegisterProperty(oss.str(), 0.f);

      oss.str("");
      oss<< TAP_POINT_UNIFORM_NAME << i << "]";
      handle.RegisterProperty(oss.str(), Vector2( 250.0f,250.0f ));
    }

    return handle;
  }

}; // namespace SparkleEffect

#endif // DALI_SPARKLE_EFFECT_H
