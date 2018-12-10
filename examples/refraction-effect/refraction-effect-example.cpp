/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/buttons/button-devel.h>

#include <fstream>
#include <sstream>
#include <limits>

// INTERNAL INCLUDES
#include "shared/view.h"
#include "shared/utility.h"

using namespace Dali;

namespace
{
const char * const APPLICATION_TITLE( "Refraction Effect" );
const char * const TOOLBAR_IMAGE( DEMO_IMAGE_DIR "top-bar.png" );
const char * const CHANGE_TEXTURE_ICON( DEMO_IMAGE_DIR "icon-change.png" );
const char * const CHANGE_TEXTURE_ICON_SELECTED( DEMO_IMAGE_DIR "icon-change-selected.png" );
const char * const CHANGE_MESH_ICON( DEMO_IMAGE_DIR "icon-replace.png" );
const char * const CHANGE_MESH_ICON_SELECTED( DEMO_IMAGE_DIR "icon-replace-selected.png" );

const char* MESH_FILES[] =
{
 DEMO_MODEL_DIR "surface_pattern_v01.obj",
 DEMO_MODEL_DIR "surface_pattern_v02.obj"
};
const unsigned int NUM_MESH_FILES( sizeof( MESH_FILES ) / sizeof( MESH_FILES[0] ) );

const char* TEXTURE_IMAGES[]=
{
  DEMO_IMAGE_DIR "background-1.jpg",
  DEMO_IMAGE_DIR "background-2.jpg",
  DEMO_IMAGE_DIR "background-3.jpg",
  DEMO_IMAGE_DIR "background-4.jpg"
};
const unsigned int NUM_TEXTURE_IMAGES( sizeof( TEXTURE_IMAGES ) / sizeof( TEXTURE_IMAGES[0] ) );

struct LightOffsetConstraint
{
  LightOffsetConstraint( float radius )
  : mRadius( radius )
  {
  }

  void operator()( Vector2& current, const PropertyInputContainer& inputs )
  {
    float spinAngle = inputs[0]->GetFloat();
    current.x = cos( spinAngle );
    current.y = sin( spinAngle );

    current *= mRadius;
  }

  float mRadius;
};

/**
 * structure of the vertex in the mesh
 */
struct Vertex
{
  Vector3 position;
  Vector3 normal;
  Vector2 textureCoord;

  Vertex()
  {}

  Vertex( const Vector3& position, const Vector3& normal, const Vector2& textureCoord )
  : position( position ), normal( normal ), textureCoord( textureCoord )
  {}
};

/************************************************************************************************
 *** The shader source is used when the MeshActor is not touched***
 ************************************************************************************************/
const char* VERTEX_SHADER_FLAT = DALI_COMPOSE_SHADER(
attribute mediump vec3    aPosition;\n
attribute mediump vec3    aNormal;\n
attribute highp   vec2    aTexCoord;\n
uniform   mediump mat4    uMvpMatrix;\n
varying   mediump vec2    vTexCoord;\n
void main()\n
{\n
  gl_Position = uMvpMatrix * vec4( aPosition.xy, 0.0, 1.0 );\n
  vTexCoord = aTexCoord.xy;\n
}\n
);

const char* FRAGMENT_SHADER_FLAT = DALI_COMPOSE_SHADER(
uniform lowp    vec4  uColor;\n
uniform sampler2D     sTexture;\n
varying mediump vec2  vTexCoord;\n
void main()\n
{\n
  gl_FragColor = texture2D( sTexture, vTexCoord ) * uColor;\n
}\n
);

/************************************************************
 ** Custom refraction effect shader***************************
 ************************************************************/
const char* VERTEX_SHADER_REFRACTION = DALI_COMPOSE_SHADER(
attribute mediump vec3    aPosition;\n
attribute mediump vec3    aNormal;\n
attribute highp   vec2    aTexCoord;\n
uniform   mediump mat4    uMvpMatrix;\n
varying   mediump vec4    vVertex;\n
varying   mediump vec3    vNormal;\n
varying   mediump vec2    vTexCoord;\n
varying   mediump vec2    vTextureOffset;\n
void main()\n
{\n
  gl_Position = uMvpMatrix * vec4( aPosition.xy, 0.0, 1.0 );\n
  vTexCoord = aTexCoord.xy;\n

  vNormal = aNormal;\n
  vVertex = vec4( aPosition, 1.0 );\n
  float length = max(0.01, length(aNormal.xy)) * 40.0;\n
  vTextureOffset = aNormal.xy / length;\n
}\n
);

const char* FRAGMENT_SHADER_REFRACTION = DALI_COMPOSE_SHADER(
precision mediump float;\n
uniform   mediump float  uEffectStrength;\n
uniform   mediump vec3   uLightPosition;\n
uniform   mediump vec2   uLightXYOffset;\n
uniform   mediump vec2   uLightSpinOffset;\n
uniform   mediump float  uLightIntensity;\n
uniform   lowp    vec4   uColor;\n
uniform   sampler2D      sTexture;\n
varying   mediump vec4   vVertex;\n
varying   mediump vec3   vNormal;\n
varying   mediump vec2   vTexCoord;\n
varying   mediump vec2   vTextureOffset;\n

vec3 rgb2hsl(vec3 rgb)\n
{\n
  float epsilon = 1.0e-10;\n
  vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);\n
  vec4 P = mix(vec4(rgb.bg, K.wz), vec4(rgb.gb, K.xy), step(rgb.b, rgb.g));\n
  vec4 Q = mix(vec4(P.xyw, rgb.r), vec4(rgb.r, P.yzx), step(P.x, rgb.r));\n
  \n
  // RGB -> HCV
  float value = Q.x;\n
  float chroma = Q.x - min(Q.w, Q.y);\n
  float hue = abs(Q.z + (Q.w-Q.y) / (6.0*chroma+epsilon));\n
  // HCV -> HSL
  float lightness = value - chroma*0.5;\n
  return vec3( hue, chroma/max( 1.0-abs(lightness*2.0-1.0), 1.0e-1 ), lightness );\n
}\n

vec3 hsl2rgb( vec3 hsl )\n
{\n
  // pure hue->RGB
  vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);\n
  vec3 p = abs(fract(hsl.xxx + K.xyz) * 6.0 - K.www);\n
  vec3 RGB = clamp(p - K.xxx, 0.0, 1.0);\n
  \n
  float chroma = ( 1.0 - abs( hsl.z*2.0-1.0 ) ) * hsl.y;\n
  return ( RGB - 0.5 ) * chroma + hsl.z;\n
}\n

void main()\n
{\n
  vec3 normal = normalize( vNormal);\n

  vec3 lightPosition = uLightPosition + vec3(uLightXYOffset+uLightSpinOffset, 0.0);\n
  mediump vec3 vecToLight = normalize( (lightPosition - vVertex.xyz) * 0.01 );\n
  mediump float spotEffect = pow( max(0.05, vecToLight.z ) - 0.05, 8.0);\n

  spotEffect = spotEffect * uEffectStrength;\n
  mediump float lightDiffuse = ( ( dot( vecToLight, normal )-0.75 ) *uLightIntensity  ) * spotEffect;\n

  lowp vec4 color = texture2D( sTexture, vTexCoord + vTextureOffset * spotEffect );\n
  vec3 lightedColor =  hsl2rgb( rgb2hsl(color.rgb) + vec3(0.0,0.0,lightDiffuse) );\n

  gl_FragColor = vec4( lightedColor, color.a ) * uColor;\n
}\n
);

} // namespace


/*************************************************/
/*Demo using RefractionEffect*****************/
/*************************************************/
class RefractionEffectExample : public ConnectionTracker
{
public:
  RefractionEffectExample( Application &application )
  : mApplication( application ),
    mContent(),
    mTextureSet(),
    mGeometry(),
    mRenderer(),
    mMeshActor(),
    mShaderFlat(),
    mShaderRefraction(),
    mLightAnimation(),
    mStrenghAnimation(),
    mLightXYOffsetIndex( Property::INVALID_INDEX ),
    mSpinAngleIndex( Property::INVALID_INDEX ),
    mLightIntensityIndex( Property::INVALID_INDEX ),
    mEffectStrengthIndex( Property::INVALID_INDEX ),
    mChangeTextureButton(),
    mChangeMeshButton(),
    mCurrentTextureId( 1 ),
    mCurrentMeshId( 0 )
  {
    // Connect to the Application's Init signal
    application.InitSignal().Connect(this, &RefractionEffectExample::Create);
  }

  ~RefractionEffectExample()
  {
  }

private:

  // The Init signal is received once (only) during the Application lifetime
  void Create(Application& application)
  {
    Stage stage = Stage::GetCurrent();
    Vector2 stageSize = stage.GetSize();

    stage.KeyEventSignal().Connect(this, &RefractionEffectExample::OnKeyEvent);

    // Creates a default view with a default tool bar.
    // The view is added to the stage.
    Toolkit::ToolBar toolBar;
    Toolkit::Control    view;
    mContent = DemoHelper::CreateView( application,
        view,
        toolBar,
        "",
        TOOLBAR_IMAGE,
        APPLICATION_TITLE );

    // Add a button to change background. (right of toolbar)
    mChangeTextureButton = Toolkit::PushButton::New();
    mChangeTextureButton.SetProperty( Toolkit::DevelButton::Property::UNSELECTED_BACKGROUND_VISUAL, CHANGE_TEXTURE_ICON );
    mChangeTextureButton.SetProperty( Toolkit::DevelButton::Property::SELECTED_BACKGROUND_VISUAL, CHANGE_TEXTURE_ICON_SELECTED );
    mChangeTextureButton.ClickedSignal().Connect( this, &RefractionEffectExample::OnChangeTexture );
    toolBar.AddControl( mChangeTextureButton,
                        DemoHelper::DEFAULT_VIEW_STYLE.mToolBarButtonPercentage,
                        Toolkit::Alignment::HorizontalRight,
                        DemoHelper::DEFAULT_MODE_SWITCH_PADDING  );
    // Add a button to change mesh pattern. ( left of bar )
    mChangeMeshButton = Toolkit::PushButton::New();
    mChangeMeshButton.SetProperty( Toolkit::DevelButton::Property::UNSELECTED_BACKGROUND_VISUAL, CHANGE_MESH_ICON );
    mChangeMeshButton.SetProperty( Toolkit::DevelButton::Property::SELECTED_BACKGROUND_VISUAL, CHANGE_MESH_ICON_SELECTED );
    mChangeMeshButton.ClickedSignal().Connect( this, &RefractionEffectExample::OnChangeMesh );
    toolBar.AddControl( mChangeMeshButton,
                        DemoHelper::DEFAULT_VIEW_STYLE.mToolBarButtonPercentage,
                        Toolkit::Alignment::HorizontalLeft,
                        DemoHelper::DEFAULT_MODE_SWITCH_PADDING  );



    // shader used when the screen is not touched, render a flat surface
    mShaderFlat = Shader::New( VERTEX_SHADER_FLAT, FRAGMENT_SHADER_FLAT );
    mGeometry = CreateGeometry( MESH_FILES[mCurrentMeshId] );

    Texture texture = DemoHelper::LoadStageFillingTexture( TEXTURE_IMAGES[mCurrentTextureId] );
    mTextureSet = TextureSet::New();
    mTextureSet.SetTexture( 0u, texture );

    mRenderer = Renderer::New( mGeometry, mShaderFlat );
    mRenderer.SetTextures( mTextureSet );

    mMeshActor = Actor::New();
    mMeshActor.AddRenderer( mRenderer );
    mMeshActor.SetSize( stageSize );
    mMeshActor.SetParentOrigin(ParentOrigin::CENTER);
    mContent.Add( mMeshActor );

    // Connect the callback to the touch signal on the mesh actor
    mContent.TouchSignal().Connect( this, &RefractionEffectExample::OnTouch );

    // shader used when the finger is touching the screen. render refraction effect
    mShaderRefraction = Shader::New( VERTEX_SHADER_REFRACTION, FRAGMENT_SHADER_REFRACTION );

    // register uniforms
    mLightXYOffsetIndex = mMeshActor.RegisterProperty( "uLightXYOffset", Vector2::ZERO );

    mLightIntensityIndex = mMeshActor.RegisterProperty( "uLightIntensity", 2.5f );

    mEffectStrengthIndex = mMeshActor.RegisterProperty( "uEffectStrength",  0.f );

    Vector3 lightPosition( -stageSize.x*0.5f, -stageSize.y*0.5f, stageSize.x*0.5f ); // top_left
    mMeshActor.RegisterProperty( "uLightPosition", lightPosition );

    Property::Index lightSpinOffsetIndex = mMeshActor.RegisterProperty( "uLightSpinOffset", Vector2::ZERO );

    mSpinAngleIndex = mMeshActor.RegisterProperty("uSpinAngle", 0.f );
    Constraint constraint = Constraint::New<Vector2>( mMeshActor, lightSpinOffsetIndex, LightOffsetConstraint(stageSize.x*0.1f) );
    constraint.AddSource( LocalSource(mSpinAngleIndex) );
    constraint.Apply();

    // the animation which spin the light around the finger touch position
    mLightAnimation = Animation::New(2.f);
    mLightAnimation.AnimateTo( Property( mMeshActor, mSpinAngleIndex ), Math::PI*2.f );
    mLightAnimation.SetLooping( true );
    mLightAnimation.Pause();
  }

  void SetLightXYOffset( const Vector2& offset )
  {
    mMeshActor.SetProperty( mLightXYOffsetIndex,  offset );
  }

  /**
   * Create a mesh actor with different geometry to replace the current one
   */
  bool OnChangeMesh( Toolkit::Button button  )
  {
    mCurrentMeshId = ( mCurrentMeshId + 1 ) % NUM_MESH_FILES;
    mGeometry = CreateGeometry( MESH_FILES[mCurrentMeshId] );
    mRenderer.SetGeometry( mGeometry );

    return true;
  }

  bool OnChangeTexture( Toolkit::Button button )
  {
    mCurrentTextureId = ( mCurrentTextureId + 1 ) % NUM_TEXTURE_IMAGES;
    Texture texture = DemoHelper::LoadStageFillingTexture( TEXTURE_IMAGES[mCurrentTextureId] );
    mTextureSet.SetTexture( 0u, texture );
    return true;
  }

  bool OnTouch( Actor actor, const TouchData& event )
  {
    switch( event.GetState( 0 ) )
    {
      case PointState::DOWN:
      {
        mRenderer.SetShader( mShaderRefraction );

        SetLightXYOffset( event.GetScreenPosition( 0 ) );

        mLightAnimation.Play();

        if( mStrenghAnimation )
        {
          mStrenghAnimation.Clear();
        }

        mStrenghAnimation= Animation::New(0.5f);
        mStrenghAnimation.AnimateTo( Property( mMeshActor, mEffectStrengthIndex ), 1.f );
        mStrenghAnimation.Play();

        break;
      }
      case PointState::MOTION:
      {
        // make the light position following the finger movement
        SetLightXYOffset( event.GetScreenPosition( 0 ) );
        break;
      }
      case PointState::UP:
      case PointState::LEAVE:
      case PointState::INTERRUPTED:
      {
        mLightAnimation.Pause();

        if( mStrenghAnimation )
        {
          mStrenghAnimation.Clear();
        }
        mStrenghAnimation = Animation::New(0.5f);
        mStrenghAnimation.AnimateTo( Property( mMeshActor, mEffectStrengthIndex ), 0.f );
        mStrenghAnimation.FinishedSignal().Connect( this, &RefractionEffectExample::OnTouchFinished );
        mStrenghAnimation.Play();
        break;
      }
      case PointState::STATIONARY:
      {
        break;
      }
    }

    return true;
  }

  void OnTouchFinished( Animation& source )
  {
    mRenderer.SetShader( mShaderFlat );
    SetLightXYOffset( Vector2::ZERO );
  }

  Geometry CreateGeometry(const std::string& objFileName)
  {
    std::vector<Vector3> vertexPositions;
    Vector<unsigned int> faceIndices;
    Vector<float> boundingBox;
    // read the vertice and faces from the .obj file, and record the bounding box
    ReadObjFile( objFileName, boundingBox, vertexPositions, faceIndices );

    std::vector<Vector2> textureCoordinates;
    // align the mesh, scale it to fit the screen size, and calculate the texture coordinate for each vertex
    ShapeResizeAndTexureCoordinateCalculation( boundingBox, vertexPositions, textureCoordinates );

    // re-organize the mesh, the vertices are duplicated, each vertex only belongs to one triangle.
    // Without sharing vertex between triangle, so we can manipulate the texture offset on each triangle conveniently.
    std::vector<Vertex> vertices;

    std::size_t size = faceIndices.Size();
    vertices.reserve( size );

    for( std::size_t i=0; i<size; i=i+3 )
    {
      Vector3 edge1 = vertexPositions[ faceIndices[i+2] ] - vertexPositions[ faceIndices[i] ];
      Vector3 edge2 = vertexPositions[ faceIndices[i+1] ] - vertexPositions[ faceIndices[i] ];
      Vector3 normal = edge1.Cross(edge2);
      normal.Normalize();

      // make sure all the faces are front-facing
      if( normal.z > 0 )
      {
        vertices.push_back( Vertex( vertexPositions[ faceIndices[i] ], normal, textureCoordinates[ faceIndices[i] ] ) );
        vertices.push_back( Vertex( vertexPositions[ faceIndices[i+1] ], normal, textureCoordinates[ faceIndices[i+1] ] ) );
        vertices.push_back( Vertex( vertexPositions[ faceIndices[i+2] ], normal, textureCoordinates[ faceIndices[i+2] ] ) );
      }
      else
      {
        normal *= -1.f;
        vertices.push_back( Vertex( vertexPositions[ faceIndices[i] ], normal, textureCoordinates[ faceIndices[i] ] ) );
        vertices.push_back( Vertex( vertexPositions[ faceIndices[i+2] ], normal, textureCoordinates[ faceIndices[i+2] ] ) );
        vertices.push_back( Vertex( vertexPositions[ faceIndices[i+1] ], normal, textureCoordinates[ faceIndices[i+1] ] ) );
      }
    }

    Property::Map vertexFormat;
    vertexFormat["aPosition"] = Property::VECTOR3;
    vertexFormat["aNormal"] = Property::VECTOR3;
    vertexFormat["aTexCoord"] = Property::VECTOR2;
    PropertyBuffer surfaceVertices = PropertyBuffer::New( vertexFormat );
    surfaceVertices.SetData( &vertices[0], vertices.size() );

    Geometry surface = Geometry::New();
    surface.AddVertexBuffer( surfaceVertices );

    return surface;
  }

  void ReadObjFile( const std::string& objFileName,
      Vector<float>& boundingBox,
      std::vector<Vector3>& vertexPositions,
      Vector<unsigned int>& faceIndices)
  {
    std::ifstream ifs( objFileName.c_str(), std::ios::in );

    boundingBox.Resize( 6 );
    boundingBox[0]=boundingBox[2]=boundingBox[4] = std::numeric_limits<float>::max();
    boundingBox[1]=boundingBox[3]=boundingBox[5] = -std::numeric_limits<float>::max();

    std::string line;
    while( std::getline( ifs, line ) )
    {
      if( line[0] == 'v' && std::isspace(line[1]))  // vertex
      {
        std::istringstream iss(line.substr(2), std::istringstream::in);
        unsigned int i = 0;
        Vector3 vertex;
        while( iss >> vertex[i++] && i < 3);
        if( vertex.x < boundingBox[0] )  boundingBox[0] = vertex.x;
        if( vertex.x > boundingBox[1] )  boundingBox[1] = vertex.x;
        if( vertex.y < boundingBox[2] )  boundingBox[2] = vertex.y;
        if( vertex.y > boundingBox[3] )  boundingBox[3] = vertex.y;
        if( vertex.z < boundingBox[4] )  boundingBox[4] = vertex.z;
        if( vertex.z > boundingBox[5] )  boundingBox[5] = vertex.z;
        vertexPositions.push_back( vertex );
      }
      else if( line[0] == 'f' ) //face
      {
        unsigned int numOfInt = 3;
        while( true )
        {
          std::size_t found  = line.find('/');
          if( found == std::string::npos )
          {
            break;
          }
          line[found] = ' ';
          numOfInt++;
        }

        std::istringstream iss(line.substr(2), std::istringstream::in);

        std::vector<unsigned int> indices( numOfInt );

        unsigned int i=0;
        while( iss >> indices[i++] && i < numOfInt);
        unsigned int step = (i+1) / 3;
        faceIndices.PushBack( indices[0]-1 );
        faceIndices.PushBack( indices[step]-1 );
        faceIndices.PushBack( indices[2*step]-1 );
      }
    }

    ifs.close();
  }

  void ShapeResizeAndTexureCoordinateCalculation( const Vector<float>& boundingBox,
      std::vector<Vector3>& vertexPositions,
      std::vector<Vector2>& textureCoordinates)
  {
    Vector3 bBoxSize( boundingBox[1] - boundingBox[0], boundingBox[3] - boundingBox[2], boundingBox[5] - boundingBox[4]);
    Vector3 bBoxMinCorner( boundingBox[0], boundingBox[2], boundingBox[4] );

    Vector2 stageSize = Stage::GetCurrent().GetSize();
    Vector3 scale( stageSize.x / bBoxSize.x, stageSize.y / bBoxSize.y, 1.f );
    scale.z = (scale.x + scale.y)/2.f;

    textureCoordinates.reserve(vertexPositions.size());

    for( std::vector<Vector3>::iterator iter = vertexPositions.begin(); iter != vertexPositions.end(); iter++ )
    {
      Vector3 newPosition(  (*iter) - bBoxMinCorner ) ;

     textureCoordinates.push_back( Vector2( newPosition.x / bBoxSize.x, newPosition.y / bBoxSize.y ) );

      newPosition -= bBoxSize * 0.5f;
      (*iter) = newPosition * scale;
    }
  }

  /**
   * Main key event handler
   */
  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.state == KeyEvent::Down)
    {
      if( IsKey( event, Dali::DALI_KEY_ESCAPE) || IsKey( event, Dali::DALI_KEY_BACK) )
      {
        mApplication.Quit();
      }
    }
  }

private:

  Application&   mApplication;
  Layer          mContent;
  TextureSet     mTextureSet;
  Geometry       mGeometry;
  Renderer       mRenderer;
  Actor          mMeshActor;

  Shader         mShaderFlat;
  Shader         mShaderRefraction;

  Animation      mLightAnimation;
  Animation      mStrenghAnimation;

  Property::Index mLightXYOffsetIndex;
  Property::Index mSpinAngleIndex;
  Property::Index mLightIntensityIndex;
  Property::Index mEffectStrengthIndex;

  Toolkit::PushButton        mChangeTextureButton;
  Toolkit::PushButton        mChangeMeshButton;
  unsigned int               mCurrentTextureId;
  unsigned int               mCurrentMeshId;
};

/*****************************************************************************/

int DALI_EXPORT_API main(int argc, char **argv)
{
  Application app = Application::New(&argc, &argv, DEMO_THEME_PATH);
  RefractionEffectExample theApp(app);
  app.MainLoop();
  return 0;
}
