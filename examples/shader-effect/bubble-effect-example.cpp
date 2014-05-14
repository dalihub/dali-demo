//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <sstream>
#include <dali/dali.h>
#include "dali-toolkit/dali-toolkit.h"

#include "../shared/view.h"

using namespace Dali;

using Dali::Material;

namespace
{
const char * const TEXTURE_IMAGE ( DALI_IMAGE_DIR "bubble-effect-texture-border.png" );
const char * const TOOLBAR_IMAGE( DALI_IMAGE_DIR "top-bar.png" );
const char * const APPLICATION_TITLE( "Bubble Effect" );
const char * const CHANGE_IMAGE_ICON( DALI_IMAGE_DIR "icon_mode.png" );

const char* BACKGROUND_IMAGES[]=
{
  DALI_IMAGE_DIR "background-1.jpg",
  DALI_IMAGE_DIR "background-2.jpg",
  DALI_IMAGE_DIR "background-3.jpg",
  DALI_IMAGE_DIR "background-4.jpg",
};
const unsigned int NUM_BACKGROUND_IMAGES( sizeof(BACKGROUND_IMAGES) / sizeof(BACKGROUND_IMAGES[0]) );

const float MIN_DISTANCE( 2.f);
const float MAX_DISTANCE( 200.f);
const float BUBBLE_SIZE( 40.f );
const float BUBBLE_MIN_SIZE( 5.f );
const unsigned long MAX_TIME_INTERVAL(100);
const unsigned int NUMBER_OF_BUBBLES( 100 );  //this value cannot be bigger than 100;
const unsigned int NUMBER_OF_GROUP( 7 );
const unsigned int TOTAL_NUMBER_OF_BUBBLES( NUMBER_OF_BUBBLES * NUMBER_OF_GROUP );
} // unnamed namespace

class BubbleEffect : public ShaderEffect
{
public:

  BubbleEffect()
  {}

  static BubbleEffect New( unsigned int numberOfBubble )
  {
    std::ostringstream meshVertexStringStream;
    meshVertexStringStream << "#define NUMBER_OF_BUBBLE "<< numberOfBubble << "\n";
    std::string meshVertex(
    "  uniform float uGravity; \n"
    "  uniform vec4 uStartAndEndPos[NUMBER_OF_BUBBLE];\n"
    "  uniform float uEasing[NUMBER_OF_BUBBLE];\n"
    "  uniform float uPercentage[NUMBER_OF_BUBBLE];\n"
    "  uniform vec2 uStageSize; \n"
    "  uniform vec2 offset[9]; \n"
    "  varying float vPercentage;\n"
    "  varying vec2  vEffectTexCoord;\n"
    "  void main()\n"
    "  {\n"
    "    mediump vec4 position = vec4( aPosition.x, aPosition.y, 0.0, 1.0 );\n"
    "    int zCoord = int(aPosition.z); \n"
    "    int idx = int(mod(aPosition.z, float(NUMBER_OF_BUBBLE)));\n"
    "    int groupIdx = (zCoord - idx) / NUMBER_OF_BUBBLE;\n"
    "    vec4 startAndEnd = uStartAndEndPos[idx]; \n"
    "    startAndEnd.zw += offset[groupIdx];\n"
    "\n"
    "    if( uPercentage[idx] < 1.0 )\n"
    "    {\n"
    "      float positionDelta = uPercentage[idx];\n"
    "      if (mod(uEasing[idx], 4.0) > 1.5) { positionDelta = pow(uPercentage[idx], 2.0); } \n"
    "      else if (mod(uEasing[idx], 8.0) > 3.5) { positionDelta = 1.0 - pow((1.0-uPercentage[idx]),2.0); }\n"
    "      else if (mod(uEasing[idx], 16.0) > 7.5) { positionDelta = pow(2.0, 10.0 * (uPercentage[idx] - 1.0)) - 0.001; }\n"
    "      else if (mod(uEasing[idx], 32.0) > 15.5) { positionDelta = 1.001 * (-pow(2.0, -10.0 * uPercentage[idx]) + 1.0); }\n"

    "      position.xy = position.xy + startAndEnd.xy + startAndEnd.zw * positionDelta; \n"
    "      if (mod(uEasing[idx],64.0) > 31.5) { position.y = position.y - (0.5*uGravity * pow(uPercentage[idx]+0.1, 2.0)); }\n"
    "    }\n"
    "    gl_Position = uMvpMatrix * position;\n"
    "\n"
    "    mediump vec2 start = uCustomTextureCoords.xy;\n"
    "    mediump vec2 scale = uCustomTextureCoords.zw;\n"
    "    vTexCoord = vec2( start.x + aTexCoord.x * scale.x, start.y + aTexCoord.y * scale.y );\n"
    "    vPercentage = uPercentage[idx];\n"
    "    vEffectTexCoord = startAndEnd.xy / uStageSize; \n"
    "  }\n" );
    meshVertexStringStream << meshVertex;

    std::string meshFragment(
    "  varying float vPercentage;\n"
    "  varying vec2  vEffectTexCoord;\n"
    "\n"
    "  void main()\n"
    "  {\n"
    "    vec4 fragColor = texture2D(sEffect, vEffectTexCoord);\n"
    "    fragColor.rgb *= 1.2; \n"
    "    fragColor *= uColor;\n"
    "    fragColor.a  *= texture2D(sTexture, vTexCoord).a * ( 1.0-vPercentage*vPercentage );\n"
    "    gl_FragColor = fragColor;\n"
    "  }\n");

    ShaderEffect shaderEffect = ShaderEffect::New( meshVertexStringStream.str(), meshFragment,
                                                   GeometryType( GEOMETRY_TYPE_TEXTURED_MESH),
                                                   ShaderEffect::GeometryHints( ShaderEffect::HINT_BLENDING ) );
    BubbleEffect handle( shaderEffect );
    handle.mNumOfBubbles = numberOfBubble;
    handle.SetUniform( "uGravity", 100.f );
    handle.SetUniform( "uStageSize", Stage::GetCurrent().GetSize() );
    for( unsigned int i=0; i<numberOfBubble; i++ )
    {
      handle.SetPercentage( i, 1.f);
    }

    srand(time(NULL));

    return handle;
  }

  void SetBubbleParameter(unsigned int idx, const Vector2& emitPosition, Vector2 direction)
  {
     Vector2 randomVec(rand()%400-200, rand()%400-200);
     float length = randomVec.Length();
     randomVec /= length;
     direction.Normalize();
     Vector2 endPos = (randomVec -  direction*4.f);
     endPos.Normalize();
     Vector4 startAndEndPos( emitPosition.x, emitPosition.y, endPos.x*length, endPos.y*length );
     SetStartAndEndPos( idx, startAndEndPos );

     float easing = pow( 2, rand()%5-1 ) + ( rand()%2-1 )*32;
     SetEasing( idx, easing );

     SetPercentage( idx, 0.f);

     float offset = 100.f;
     SetUniform("offset[0]", Vector2(0.f,0.f));
     SetUniform("offset[1]", Vector2(offset,0.f));
     SetUniform("offset[2]", Vector2(-offset,0.f));
     SetUniform("offset[3]", Vector2(0.f,offset));
     SetUniform("offset[4]", Vector2(0.f,-offset));
     SetUniform("offset[5]", Vector2(offset,offset)*0.707f );
     SetUniform("offset[6]", Vector2(offset,-offset)*0.707f);
     SetUniform("offset[7]", Vector2(-offset,offset)*0.707f);
     SetUniform("offset[8]", Vector2(-offset,-offset)*0.707f);
  }

  std::string GetPercentagePropertyName( unsigned int idx ) const
  {
    assert( idx < mNumOfBubbles );
    std::ostringstream oss;
    oss<< "uPercentage["<< idx << "]";
    return oss.str();
  }

private:
  // Helper for New()
  BubbleEffect( ShaderEffect handle )
  : ShaderEffect( handle )
  {}

  void SetStartAndEndPos( unsigned int idx, const Vector4& startAndEndPos )
  {
    assert( idx < mNumOfBubbles );
    std::ostringstream oss;
    oss<< "uStartAndEndPos["<< idx << "]";
    SetUniform( oss.str(), startAndEndPos );
  }

  void SetEasing( unsigned int idx, float easing )
  {
    assert( idx < mNumOfBubbles );
    std::ostringstream oss;
    oss<< "uEasing["<< idx << "]";
    SetUniform( oss.str(), easing );
  }

  void SetPercentage( unsigned int idx, float percentage )
  {
    assert( idx < mNumOfBubbles );
    std::ostringstream oss;
    oss<< "uPercentage["<< idx << "]";
    SetUniform( oss.str(), percentage );
  }

private:

  unsigned int mNumOfBubbles;
};

class BubbleEffectExample : public ConnectionTracker
{
public:
  BubbleEffectExample(Application &app)
  : mApp(app),
    mCurrentUniform( 0 ),
    mTouchable( false ),
    mLastTouchTime( 0 ),
    mRescoucesCleared( false ),
    mCurrentBackgroundImageId( 0 )
  {
    app.InitSignal().Connect(this, &BubbleEffectExample::Create);
  }

  ~BubbleEffectExample()
  {
  }

public:

  void Create(Application& app)
  {
    Stage stage = Stage::GetCurrent();
    Vector2 size = stage.GetSize();

    // Creates a default view with a default tool bar.
    // The view is added to the stage.
    Toolkit::ToolBar toolBar;
    mContent = DemoHelper::CreateView( app,
                                       mView,
                                       toolBar,
                                       "",
                                       TOOLBAR_IMAGE,
                                       APPLICATION_TITLE );

    // Create a effect toggle button. (right of toolbar)
    Image imageLayout = Image::New( CHANGE_IMAGE_ICON );
    Toolkit::PushButton layoutButton = Toolkit::PushButton::New();
    layoutButton.SetBackgroundImage(imageLayout);
    layoutButton.ClickedSignal().Connect( this, &BubbleEffectExample::OnImageChageIconClicked );
    toolBar.AddControl( layoutButton, DemoHelper::DEFAULT_VIEW_STYLE.mToolBarButtonPercentage, Toolkit::Alignment::HorizontalRight, DemoHelper::DEFAULT_MODE_SWITCH_PADDING  );

    mRoot = Actor::New();
    mRoot.SetParentOrigin( ParentOrigin::CENTER );
    stage.Add(mRoot);

    mFrameBufferImage = FrameBufferImage::New( size.width/4.f, size.height/4.f, Pixel::RGBA8888, Dali::Image::Unused );
    mTimer = Timer::New( 1000 );
    mTimer.TickSignal().Connect( this, &BubbleEffectExample::ClearBlurResource );

    SetImage( BACKGROUND_IMAGES[ mCurrentBackgroundImageId ] );

    GenMaterial();
    MeshData meshData;
    ConstructBubbleMesh( meshData, NUMBER_OF_BUBBLES*9, BUBBLE_SIZE);
    for(unsigned int i=0; i < NUMBER_OF_GROUP; i++ )
    {
      mMesh[i] = Mesh::New( meshData );
      mMeshActor[i] = MeshActor::New( mMesh[i] );
      mMeshActor[i].SetAffectedByLighting( false );
      mMeshActor[i].SetParentOrigin(ParentOrigin::TOP_LEFT);
      stage.Add( mMeshActor[i] );
      mEffect[i] = BubbleEffect::New( NUMBER_OF_BUBBLES );
      mEffect[i].SetEffectImage( mFrameBufferImage );
      mMeshActor[i].SetShaderEffect( mEffect[i] );
    }

    Stage::GetCurrent().KeyEventSignal().Connect(this, &BubbleEffectExample::OnKeyEvent);
  }

  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.state == KeyEvent::Down)
    {
      if( IsKey( event, Dali::DALI_KEY_ESCAPE) || IsKey( event, Dali::DALI_KEY_BACK) )
      {
        mApp.Quit();
      }
    }
  }

  void SetImage( const std::string& imagePath )
  {
    mTouchable = false;

    if( mScreen )
    {
      mScreen.TouchedSignal().Disconnect( this, &BubbleEffectExample::OnTouch );
      mRoot.Remove( mScreen );
      ClearBlurResource();
    }

    Stage stage = Stage::GetCurrent();

    Image image = Image::New( imagePath );
    mScreen = ImageActor::New( image );
    mScreen.SetSize( stage.GetSize() );
    mScreen.SetZ( -1.f );
    mScreen.SetParentOrigin(ParentOrigin::CENTER);
    mRoot.Add(mScreen);
    mScreen.TouchedSignal().Connect( this, &BubbleEffectExample::OnTouch );

    mGaussianBlurView = Toolkit::GaussianBlurView::New( 7, 2.5f, Pixel::RGBA8888, 0.25f, 0.25f, true );
    mGaussianBlurView.SetParentOrigin(ParentOrigin::CENTER);
    mGaussianBlurView.SetSize(stage.GetSize());
    mGaussianBlurView.SetUserImageAndOutputRenderTarget(  image, mFrameBufferImage );
    stage.Add( mGaussianBlurView );
    mGaussianBlurView.Activate();

    mRescoucesCleared = false;
    mTimer.Start();

    mTouchable = true;
  }

  bool ClearBlurResource()
  {
    if( !mRescoucesCleared )
    {
      Stage::GetCurrent().Remove( mGaussianBlurView );
      mGaussianBlurView.Deactivate();
      mGaussianBlurView.Reset();
      mRescoucesCleared = true;
    }
    return false;
 }

  void GenMaterial()
  {
    mCustomMaterial = Material::New("CustomMaterial");
    mCustomMaterial.SetOpacity(1.0f);
    mCustomMaterial.SetDiffuseColor(Color::WHITE);
    mCustomMaterial.SetAmbientColor(Vector4(0.0, 0.1, 0.1, 1.0));
    mCustomMaterial.SetMapU( Material::MAPPING_MODE_WRAP );
    mCustomMaterial.SetMapV( Material::MAPPING_MODE_WRAP );
    mCustomMaterial.SetDiffuseTexture( Image::New( TEXTURE_IMAGE ) );
  }

  void AddVertex(MeshData::VertexContainer& vertices, Vector3 V, Vector2 UV)
  {
    MeshData::Vertex meshVertex;
    meshVertex.x = V.x;
    meshVertex.y = V.y;
    meshVertex.z = V.z;
    meshVertex.u = UV.x;
    meshVertex.v = UV.y;
    vertices.push_back(meshVertex);
  }

  void AddTriangle(MeshData::FaceIndices& faces,
                   size_t v0, size_t v1, size_t v2)
  {
    faces.push_back(v0);
    faces.push_back(v1);
    faces.push_back(v2);
  }

  void ConstructBubbleMesh( MeshData& meshData, unsigned int numOfBubble, int size )
  {
    MeshData::VertexContainer    vertices;
    MeshData::FaceIndices        faces;
    BoneContainer                bones(0);

    for(unsigned int index = 0; index < numOfBubble; index ++)
    {
      float curSize =  static_cast<float>( rand()%size + BUBBLE_MIN_SIZE);
      float depth = static_cast<float>( index );
      AddVertex( vertices, Vector3(0.f,0.f,depth), Vector2(0.f,0.f) );
      AddVertex( vertices, Vector3(0.f,curSize,depth), Vector2( 0.f,1.f ));
      AddVertex( vertices, Vector3(curSize,curSize,depth), Vector2(1.f,1.f) );
      AddVertex( vertices, Vector3(curSize,0.f,depth), Vector2(1.f,0.f) );

      unsigned int idx = index * 4;
      AddTriangle( faces, idx, idx+1, idx+2);
      AddTriangle( faces, idx, idx+2, idx+3);
    }

    meshData.SetData(vertices, faces, bones, mCustomMaterial);
    meshData.SetHasColor(false);
    meshData.SetHasNormals(true);
    meshData.SetHasTextureCoords(true);
  }

  void SetUpAnimation( Vector2 emitPosition, Vector2 direction )
  {
    unsigned int curUniform = mCurrentUniform  % NUMBER_OF_BUBBLES;
    unsigned int groupIdx = mCurrentUniform / NUMBER_OF_BUBBLES;
    mEffect[groupIdx].SetBubbleParameter(curUniform, emitPosition, direction);
    float duration = RandomRange( 1.f, 2.f );
    mAnimation[mCurrentUniform] = Animation::New( duration );
    mAnimationIndexPair[mAnimation[mCurrentUniform]] = mCurrentUniform;
    mAnimation[mCurrentUniform].AnimateTo( Property( mEffect[groupIdx], mEffect[groupIdx].GetPercentagePropertyName(curUniform) ),
                                           1.f, AlphaFunctions::DoubleEaseInOutSine60 );
    mAnimation[mCurrentUniform].Play();
    mAnimation[mCurrentUniform].FinishedSignal().Connect(this, &BubbleEffectExample::OnAnimationFinished);

    mCurrentUniform = (mCurrentUniform + 1) % (TOTAL_NUMBER_OF_BUBBLES);
  }

  void OnAnimationFinished( Animation& source )
  {
    mAnimation[mAnimationIndexPair[source]].Reset();
    mAnimationIndexPair.erase( source );
  }

  float RandomRange(float f0, float f1)
  {
    return f0 + (rand() & 0xfff) * (f1-f0) * (1.0f/4095.0f);
  }

  bool OnTouch(Dali::Actor actor, const Dali::TouchEvent& event)
  {
    if(!mTouchable)
    {
      return false;
    }

    if ( event.GetPointCount() > 0 )
    {
      const TouchPoint &point = event.GetPoint(0);
      float distance = hypotf( point.local.x - mLastTouchPosition.x, point.local.y - mLastTouchPosition.y );
      if ( distance > MIN_DISTANCE )
      {
        // when the finger moves fast, interpolate linearly between two touch points
        if(event.time - mLastTouchTime < MAX_TIME_INTERVAL  && distance < MAX_DISTANCE)
        {
          float num = floor(distance / MIN_DISTANCE);
          unsigned int numStep = static_cast<unsigned int>( num );
          Vector2 step = ( point.screen - mLastTouchPosition ) * MIN_DISTANCE / distance;
          for(unsigned int i = 0; i<numStep; i++)
          {
            SetUpAnimation( mLastTouchPosition + step*i, step );
          }
        }
        else
        {
          SetUpAnimation( point.screen, point.screen-mLastTouchPosition );
        }

        mLastTouchTime = event.time;
        mLastTouchPosition = point.screen;
      }
    }
    return true;
  }

  bool OnImageChageIconClicked( Toolkit::Button button )
  {
    mCurrentBackgroundImageId = (mCurrentBackgroundImageId + 1 ) % NUM_BACKGROUND_IMAGES;
    SetImage( BACKGROUND_IMAGES[ mCurrentBackgroundImageId ] );
    return true;
  }

private:
  Application&                      mApp;
  Actor                             mRoot;
  ImageActor                        mScreen;
  Mesh                              mMesh[NUMBER_OF_GROUP];
  MeshActor                         mMeshActor[NUMBER_OF_GROUP];
  Material                          mCustomMaterial;
  BubbleEffect                      mEffect[NUMBER_OF_GROUP];
  unsigned int                      mCurrentUniform;
  Animation                         mAnimation[TOTAL_NUMBER_OF_BUBBLES];
  bool                              mTouchable;
  std::map<Animation, unsigned int> mAnimationIndexPair;
  unsigned long                     mLastTouchTime;
  Vector2                           mLastTouchPosition;

  Toolkit::GaussianBlurView         mGaussianBlurView;
  FrameBufferImage                  mFrameBufferImage;
  Timer                             mTimer;
  bool                              mRescoucesCleared;

  Layer                             mContent;
  Toolkit::View                     mView;
  unsigned int                      mCurrentBackgroundImageId;
};

/*****************************************************************************/

static void
RunTest(Application& app)
{
  BubbleEffectExample theApp(app);
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



