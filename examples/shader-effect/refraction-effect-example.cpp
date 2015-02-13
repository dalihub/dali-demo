/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include "../shared/view.h"

#include <fstream>
#include <sstream>

using namespace Dali;

namespace
{
const char * const APPLICATION_TITLE( "Refraction Effect" );
const char * const TOOLBAR_IMAGE( DALI_IMAGE_DIR "top-bar.png" );
const char * const CHANGE_TEXTURE_ICON( DALI_IMAGE_DIR "icon-change.png" );
const char * const CHANGE_MESH_ICON( DALI_IMAGE_DIR "icon-replace.png" );

const char* MESH_FILES[] =
{
 DALI_MODEL_DIR "surface_pattern_v01.obj",
 DALI_MODEL_DIR "surface_pattern_v02.obj"
};
const unsigned int NUM_MESH_FILES( sizeof( MESH_FILES ) / sizeof( MESH_FILES[0] ) );

const char* TEXTURE_IMAGES[]=
{
  DALI_IMAGE_DIR "background-1.jpg",
  DALI_IMAGE_DIR "background-2.jpg",
  DALI_IMAGE_DIR "background-3.jpg",
  DALI_IMAGE_DIR "background-4.jpg"
};
const unsigned int NUM_TEXTURE_IMAGES( sizeof( TEXTURE_IMAGES ) / sizeof( TEXTURE_IMAGES[0] ) );

#define MAKE_SHADER(A)#A

struct LightOffsetConstraint
{
  LightOffsetConstraint( float radius )
  : mRadius( radius )
  {
  }

  Vector2 operator()( const Vector2& current, const PropertyInput& spinAngleProperty)
  {
    float spinAngle = spinAngleProperty.GetFloat();
    return Vector2( cos(spinAngle ), sin( spinAngle ) ) * mRadius;
  }

  float mRadius;
};

} // namespace

/************************************************************************************************
 *** This shader is used when the MeshActor is not touched***
 ************************************************************************************************/
class NoEffect : public ShaderEffect
{
public:
  /**
   * Create an empty handle.
   */
  NoEffect()
  {
  }

  /**
   * Virtual destructor
   */
  virtual ~NoEffect()
  {
  }

  /**
   * Create a NoEffect object.
   * @return A handle to a newly allocated NoEffect
   */
  static NoEffect New()
  {
    std::string vertexShader = MAKE_SHADER(
        precision mediump float;\n
        uniform mediump vec4 uTextureRect;\n
        void main()\n
        {\n
          gl_Position = uMvpMatrix * vec4( aPosition.xy, 0.0, 1.0 );\n
          vTexCoord = aTexCoord.xy;\n
        }\n
    );
    std::string fragmentShader = MAKE_SHADER(
        precision mediump float;\n
        void main()\n
        {\n
          gl_FragColor = texture2D( sTexture, vTexCoord ) * uColor;\n
        }\n
    );
    ShaderEffect shaderEffect = ShaderEffect::New( vertexShader, fragmentShader,
                                                   GeometryType( GEOMETRY_TYPE_TEXTURED_MESH),
                                                   ShaderEffect::GeometryHints( ShaderEffect::HINT_NONE ) );
    NoEffect handle( shaderEffect );
    return handle;
  }

private:
  /**
   * Helper for New()
   */
  NoEffect( ShaderEffect handle )
  : ShaderEffect( handle )
  {
  }
};

/************************************************************/
/* Custom refraction effect shader******************************/
/************************************************************/

class RefractionEffect : public ShaderEffect
{
public:

  /**
   * Create an empty RefractionEffect handle.
   */
  RefractionEffect()
  {
  }

  /**
   * Virtual destructor
   */
  virtual ~RefractionEffect()
  {
  }

  /**
   * Create a RefractionEffect object.
   * @return A handle to a newly allocated RefractionEffect
   */
  static RefractionEffect New()
  {
    std::string vertexShader = MAKE_SHADER(
      precision mediump float;\n
      varying mediump vec2 vTextureOffset;\n
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

    std::string fragmentShader = MAKE_SHADER(
      precision mediump float;\n
      uniform mediump float uEffectStrength;\n
      uniform mediump vec3 uLightPosition;\n
      uniform mediump vec2 uLightXYOffset;\n
      uniform mediump vec2 uLightSpinOffset;\n
      uniform mediump float uLightIntensity;\n
      varying mediump vec2 vTextureOffset;\n

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

      vec3 hsl2rgb( vec3 hsl )
      {
        // pure hue->RGB
        vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);\n
        vec3 p = abs(fract(hsl.xxx + K.xyz) * 6.0 - K.www);\n
        vec3 RGB = clamp(p - K.xxx, 0.0, 1.0);\n
        \n
        float chroma = ( 1.0 - abs( hsl.z*2.0-1.0 ) ) * hsl.y;\n
        return ( RGB - 0.5 ) * chroma + hsl.z;
      }

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

    ShaderEffect shaderEffect = ShaderEffect::New( vertexShader, fragmentShader,
                                                   GeometryType( GEOMETRY_TYPE_TEXTURED_MESH),
                                                   ShaderEffect::GeometryHints( ShaderEffect::HINT_BLENDING ) );
    RefractionEffect handle( shaderEffect );

    Vector2 stageSize = Stage::GetCurrent().GetSize();
    handle.SetLightPosition( Vector2(stageSize.x, 0.f) );
    handle.SetUniform( "uLightXYOffset",  Vector2::ZERO );
    handle.SetUniform( "uLightSpinOffset",  Vector2::ZERO );
    handle.SetUniform( "uEffectStrength", 0.f );
    handle.SetUniform( "uLightIntensity",  2.5f );

    Property::Index index = handle.RegisterProperty( "uSpinAngle", 0.f );
    Constraint constraint = Constraint::New<Vector2>( handle.GetPropertyIndex("uLightSpinOffset"),
                                                      LocalSource(index),
                                                      LightOffsetConstraint(stageSize.x*0.1f));
    handle.ApplyConstraint( constraint );

    return handle;
  }

  void SetLightPosition( const Vector2& position )
  {
    Vector2 stageHalfSize = Stage::GetCurrent().GetSize() * 0.5f;
    SetUniform( "uLightPosition", Vector3( position.x - stageHalfSize.x, position.y - stageHalfSize.y, stageHalfSize.x ) );
  }

  void SetLightXYOffset( const Vector2& offset )
  {
    SetUniform( "uLightXYOffset",  offset );
  }

  void SetEffectStrength( float strength )
  {
    SetUniform( "uEffectStrength", strength );
  }

  void SetLightIntensity( float intensity )
  {
    SetUniform( "uLightIntensity", intensity );
  }

private:
  /**
   * Helper for New()
   */
  RefractionEffect( ShaderEffect handle )
  : ShaderEffect( handle )
  {
  }
};

/*************************************************/
/*Demo using RefractionEffect*****************/
/*************************************************/
class RefractionEffectExample : public ConnectionTracker
{
public:
  RefractionEffectExample( Application &application )
  : mApplication( application ),
    mIsDown( false ),
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
    mStageHalfSize = stage.GetSize() * 0.5f;

    stage.KeyEventSignal().Connect(this, &RefractionEffectExample::OnKeyEvent);

    // Creates a default view with a default tool bar.
    // The view is added to the stage.
    Toolkit::ToolBar toolBar;
    Toolkit::View    view;
    mContent = DemoHelper::CreateView( application,
        view,
        toolBar,
        "",
        TOOLBAR_IMAGE,
        APPLICATION_TITLE );

    // Add a button to change background. (right of toolbar)
    mChangeTextureButton = Toolkit::PushButton::New();
    mChangeTextureButton.SetBackgroundImage( ResourceImage::New( CHANGE_TEXTURE_ICON ) );
    mChangeTextureButton.ClickedSignal().Connect( this, &RefractionEffectExample::OnChangeTexture );
    toolBar.AddControl( mChangeTextureButton,
                        DemoHelper::DEFAULT_VIEW_STYLE.mToolBarButtonPercentage,
                        Toolkit::Alignment::HorizontalRight,
                        DemoHelper::DEFAULT_MODE_SWITCH_PADDING  );
    // Add a button to change mesh pattern. ( left of bar )
    mChangeMeshButton = Toolkit::PushButton::New();
    mChangeMeshButton.SetBackgroundImage( ResourceImage::New( CHANGE_MESH_ICON ) );
    mChangeMeshButton.ClickedSignal().Connect( this, &RefractionEffectExample::OnChangeMesh );
    toolBar.AddControl( mChangeMeshButton,
                        DemoHelper::DEFAULT_VIEW_STYLE.mToolBarButtonPercentage,
                        Toolkit::Alignment::HorizontalLeft,
                        DemoHelper::DEFAULT_MODE_SWITCH_PADDING  );

    // creates the shader effects applied on the mesh actor
    mRefractionEffect = RefractionEffect::New(); // used when the finger is touching the screen
    mNoEffect = NoEffect::New(); // used in the other situations, basic render shader
    // Create the mesh from the obj file and add to stage
    mMaterial =  Material::New( "Material" ) ;
    mMaterial.SetDiffuseTexture(ResourceImage::New(TEXTURE_IMAGES[mCurrentTextureId]));
    CreateSurface( MESH_FILES[mCurrentMeshId] );

    // Connect the callback to the touch signal on the mesh actor
    mContent.TouchedSignal().Connect( this, &RefractionEffectExample::OnTouch );

    // the animation which spin the light around the finger touch position
    mLightPosition = Vector2( mStageHalfSize.x*2.f, 0.f);
    mLightAnimation = Animation::New(2.f);
    mLightAnimation.AnimateTo( Property( mRefractionEffect, "uSpinAngle" ), Math::PI*2.f );
    mLightAnimation.SetLooping( true );
    mLightAnimation.Pause();
  }

  /**
   * Create a mesh actor with different geometry to replace the current one
   */
  bool OnChangeMesh( Toolkit::Button button  )
  {
    if( mMeshActor )
    {
      UnparentAndReset( mMeshActor );
    }

    mCurrentMeshId = ( mCurrentMeshId + 1 ) % NUM_MESH_FILES;
    CreateSurface( MESH_FILES[mCurrentMeshId] );

    return true;
  }

  bool OnChangeTexture( Toolkit::Button button )
  {
    mCurrentTextureId = ( mCurrentTextureId + 1 ) % NUM_TEXTURE_IMAGES;
    mMaterial.SetDiffuseTexture(ResourceImage::New(TEXTURE_IMAGES[mCurrentTextureId]));

    return true;
  }

  bool OnTouch( Actor actor , const TouchEvent& event )
  {
    const TouchPoint &point = event.GetPoint(0);

    switch(point.state)
    {
      case TouchPoint::Down:
      {
        mIsDown = true;
        mDownPosition = point.screen;

        mLightAnimation.Play();

        if( mStrenghAnimation )
        {
          mStrenghAnimation.Clear();
        }

        mRefractionEffect.SetLightXYOffset( point.screen - mLightPosition );
        mMeshActor.SetShaderEffect( mRefractionEffect );
        mStrenghAnimation= Animation::New(0.5f);
        mStrenghAnimation.AnimateTo( Property( mRefractionEffect, "uEffectStrength" ), 1.f );
        mStrenghAnimation.Play();

        break;
      }
      case TouchPoint::Motion:
      {
        if(mIsDown)
        {
          // make the light position following the finger movement
          mRefractionEffect.SetLightXYOffset( point.screen - mLightPosition );
        }
        break;
      }
      case TouchPoint::Up:
      case TouchPoint::Leave:
      case TouchPoint::Interrupted:
      {
        if(mIsDown)
        {
          mLightAnimation.Pause();

          if( mStrenghAnimation )
          {
            mStrenghAnimation.Clear();
          }
          mStrenghAnimation = Animation::New(0.5f);
          mStrenghAnimation.AnimateTo( Property( mRefractionEffect, "uEffectStrength" ), 0.f );
          mStrenghAnimation.FinishedSignal().Connect( this, &RefractionEffectExample::OnTouchFinished );
          mStrenghAnimation.Play();
        }

        mIsDown = false;
        break;
      }
      case TouchPoint::Stationary:
      case TouchPoint::Last:
      default:
      {
        break;
      }
    }
    return true;
  }

  void OnTouchFinished( Animation& source )
  {
    mMeshActor.SetShaderEffect( mNoEffect );
    mRefractionEffect.SetLightXYOffset( Vector2::ZERO );
  }

  void CreateSurface( const std::string& objFileName )
  {
    MeshData::VertexContainer    vertices;
    MeshData::FaceIndices        faces;
    MeshData                     meshData;

    std::vector<float> boundingBox;
    std::vector<Vector3> vertexPositions;
    std::vector<int> faceIndices;
    // read the vertice and faces from the .obj file, and record the bounding box
    ReadObjFile( objFileName, boundingBox, vertexPositions, faceIndices );

    std::vector<Vector2> textureCoordinates;
    // align the mesh, scale it to fit the screen size, and calculate the texture coordinate for each vertex
    ShapeResizeAndTexureCoordinateCalculation( boundingBox, vertexPositions, textureCoordinates );

    // re-organize the mesh, the vertices are duplicated, each vertex only belongs to one triangle.
    // Without sharing vertex between triangle, so we can manipulate the texture offset on each triangle conveniently.
    for( std::size_t i=0; i<faceIndices.size(); i=i+3 )
    {
      Vector3 edge1 = vertexPositions[ faceIndices[i+2] ] - vertexPositions[ faceIndices[i] ];
      Vector3 edge2 = vertexPositions[ faceIndices[i+1] ] - vertexPositions[ faceIndices[i] ];
      Vector3 normal = edge1.Cross(edge2);
      normal.Normalize();

      if( normal.z > 0 )
      {
        faces.push_back( i );
        faces.push_back( i+1 );
        faces.push_back( i+2 );
      }
      else
      {
        normal *= -1.f;
        faces.push_back( i );
        faces.push_back( i+2 );
        faces.push_back( i+1 );
      }

      vertices.push_back( MeshData::Vertex( vertexPositions[ faceIndices[i] ], textureCoordinates[ faceIndices[i] ], normal ) );
      vertices.push_back( MeshData::Vertex( vertexPositions[ faceIndices[i+1] ], textureCoordinates[ faceIndices[i+1] ], normal ) );
      vertices.push_back( MeshData::Vertex( vertexPositions[ faceIndices[i+2] ], textureCoordinates[ faceIndices[i+2] ], normal ) );

    }

    // Now ready to construct the mesh actor
    meshData.SetMaterial( mMaterial );
    meshData.SetVertices( vertices );
    meshData.SetFaceIndices( faces );
    meshData.SetHasTextureCoords(true);
    meshData.SetHasNormals(true);
    mMeshActor = MeshActor::New( Mesh::New( meshData ) );
    mMeshActor.SetParentOrigin(ParentOrigin::CENTER);
    mMeshActor.SetAffectedByLighting( false );
    mMeshActor.SetShaderEffect( mNoEffect );
    mContent.Add( mMeshActor );
  }

  void ReadObjFile( const std::string& objFileName,
      std::vector<float>& boundingBox,
      std::vector<Vector3>& vertexPositions,
      std::vector<int>& faceIndices)
  {
    std::ifstream ifs( objFileName.c_str(), std::ios::in );

    boundingBox.resize( 6 );
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
        int indices[ numOfInt ];
        unsigned int i=0;
        while( iss >> indices[i++] && i < numOfInt);
        unsigned int step = (i+1) / 3;
        faceIndices.push_back( indices[0]-1 );
        faceIndices.push_back( indices[step]-1 );
        faceIndices.push_back( indices[2*step]-1 );
      }
    }

    ifs.close();
  }

  void ShapeResizeAndTexureCoordinateCalculation( const std::vector<float>& boundingBox,
      std::vector<Vector3>& vertexPositions,
      std::vector<Vector2>& textureCoordinates)
  {
    Vector3 bBoxSize( boundingBox[1] - boundingBox[0], boundingBox[3] - boundingBox[2], boundingBox[5] - boundingBox[4]);
    Vector3 bBoxMinCorner( boundingBox[0], boundingBox[2], boundingBox[4] );

    Vector2 stageSize = Stage::GetCurrent().GetSize();
    Vector3 scale( stageSize.x / bBoxSize.x, stageSize.y / bBoxSize.y, 1.f );
    scale.z = (scale.x + scale.y)/2.f;

    for( std::vector<Vector3>::iterator iter = vertexPositions.begin(); iter != vertexPositions.end(); iter++ )
    {
      Vector3 newPosition(  (*iter) - bBoxMinCorner ) ;

      Vector2 textureCoord( newPosition.x / bBoxSize.x, newPosition.y / bBoxSize.y );
      textureCoordinates.push_back( textureCoord );

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

  bool           mIsDown;
  Vector2        mDownPosition;
  Vector2        mLightPosition;
  Vector2        mStageHalfSize;

  Material       mMaterial;
  MeshActor      mMeshActor;

  RefractionEffect  mRefractionEffect;
  NoEffect          mNoEffect;
  Animation         mLightAnimation;
  Animation         mStrenghAnimation;

  Toolkit::PushButton        mChangeTextureButton;
  Toolkit::PushButton        mChangeMeshButton;
  unsigned int               mCurrentTextureId;
  unsigned int               mCurrentMeshId;
};

/*****************************************************************************/

static void
RunTest(Application& app)
{
  RefractionEffectExample theApp(app);
  app.MainLoop();
}

/*****************************************************************************/

int
main(int argc, char **argv)
{
  Application app = Application::New(&argc, &argv);

  RunTest(app);

  return 0;
}
