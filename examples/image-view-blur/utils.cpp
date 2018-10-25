#include "utils.h"

#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/image-loader/texture-manager.h>
#include <dali-toolkit/devel-api/builder/base64-encoding.h>
#include <iostream>


using namespace Dali;
using namespace Dali::Toolkit;

namespace Utils
{

std::string CreateEffectPassTexture( std::string inputUrl, std::vector<uint32_t>& fragShader, Vector2 targetSize, Direction direction )
{
  auto outputTexture = Texture::New( TextureType::TEXTURE_2D,
                                     Pixel::RGBA8888,
                                     unsigned(targetSize.width),
                                     unsigned(targetSize.height) );

  auto framebuffer = FrameBuffer::New(targetSize.width, targetSize.height, Pixel::RGB888);
  framebuffer.AttachColorTexture( outputTexture );

  Actor image = CreateEffectPassTexture( inputUrl, fragShader, targetSize, direction, framebuffer );

  auto url = Dali::Toolkit::TextureManager::AddTexture( outputTexture );
  return url;
}


Actor CreateEffectPassTexture( std::string inputUrl, std::vector<uint32_t>& fragShader, Vector2 targetSize, Direction direction, FrameBuffer fb )
{
  auto image = ImageView::New( inputUrl );

  if( fragShader.size() > 0 )
  {
    Property::Map visualMap;
    Property::Map customShader;
    Property::Value value;
    Toolkit::EncodeBase64PropertyData( value, fragShader );
    customShader[Toolkit::Visual::Shader::Property::FRAGMENT_SHADER] = value;
    visualMap[Toolkit::Visual::Property::SHADER] = customShader;
    image.SetProperty(Toolkit::ImageView::Property::IMAGE, visualMap);
    SetShaderConstants( image, direction, 5, targetSize.x, targetSize.y );
  }

  image.SetParentOrigin( ParentOrigin::CENTER );
  image.SetSize( targetSize );

  Stage::GetCurrent().Add(image);

  Utils::CreateRenderTask( image, targetSize, fb );
  return image;
}

Actor CreatePreview( const std::string& url,
                     Vector2 targetSize,
                     Vector3 parentOrigin, Vector3 labelOrigin,
                     const std::string& label, bool copy )
{
  Actor previewActor;
  if( copy )
  {
    previewActor = CreatePreviewRenderTask( url, targetSize );
  }
  else
  {
    previewActor = ImageView::New( url );
  }

  previewActor.SetParentOrigin( parentOrigin );
  previewActor.SetAnchorPoint( parentOrigin );
  previewActor.SetSize( targetSize * 0.8f );

  auto labelActor = TextLabel::New(label);
  labelActor.SetResizePolicy(ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS);
  labelActor.SetParentOrigin( labelOrigin );
  labelActor.SetAnchorPoint( parentOrigin );
  labelActor.SetProperty(TextLabel::Property::TEXT_COLOR, Color::WHITE);
  previewActor.Add( labelActor );

  return previewActor;
}

/**
 * @param[in] url The url of the texture to copy
 * @return An image view containing a copy of the texture
 */
Actor CreatePreviewRenderTask( std::string url, Vector2 targetSize )
{
  // Image for re-rendering to a second FB
  auto image = ImageView::New( url );
  image.SetSize( targetSize );
  image.SetParentOrigin( ParentOrigin::CENTER );
  image.SetAnchorPoint( AnchorPoint::CENTER );
  Stage::GetCurrent().Add( image );

  Texture previewOutput = Utils::CreateRenderTask( image, targetSize ); // Exclusive
  auto previewUrl = Dali::Toolkit::TextureManager::AddTexture( previewOutput );

  auto previewImage = ImageView::New( previewUrl );
  previewImage.SetSize( targetSize * 0.4f );
  return previewImage;
}

Texture CreateRenderTask(Actor inputActor, Vector2 targetSize)
{
  auto outputTexture = Texture::New( TextureType::TEXTURE_2D,
                                     Pixel::RGBA8888,
                                     unsigned(targetSize.width),
                                     unsigned(targetSize.height) );

  auto framebuffer = FrameBuffer::New(targetSize.width, targetSize.height, Pixel::RGBA8888);
  framebuffer.AttachColorTexture( outputTexture );

  Utils::CreateRenderTask( inputActor, targetSize, framebuffer );

  return outputTexture;
}

void CreateRenderTask(Actor inputActor, Vector2 targetSize, FrameBuffer fbo )
{
  auto rootActor = Stage::GetCurrent().GetRootLayer();

  auto cameraActor = CameraActor::New(targetSize);
  float fov = Math::PI * 0.25f;
  cameraActor.SetFieldOfView( fov );
  cameraActor.SetNearClippingPlane( 1.0f );
  cameraActor.SetAspectRatio( targetSize.width / targetSize.height );
  cameraActor.SetType( Dali::Camera::FREE_LOOK );
  cameraActor.SetPosition( 0.0f, 0.0f, ((targetSize.height * 0.5f) / tanf( fov * 0.5f ) ) );
  cameraActor.SetParentOrigin(ParentOrigin::CENTER);
  rootActor.Add(cameraActor);

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();

  auto renderTask = taskList.CreateTask();
  renderTask.SetRefreshRate(RenderTask::REFRESH_ONCE);
  renderTask.SetSourceActor(inputActor);
  renderTask.SetExclusive(true);

  renderTask.SetInputEnabled(false);
  renderTask.SetClearColor(Vector4(1.,0.,0.,1.));
  renderTask.SetClearEnabled(true);
  renderTask.SetCameraActor(cameraActor);

  renderTask.SetFrameBuffer( fbo );
}


float CalcGaussianWeight(float x)
{
  const float BELL_CURVE_WIDTH( 1.5f );

  return (1.0f / sqrt(2.0f * Math::PI * BELL_CURVE_WIDTH)) * exp(-(x * x) / (2.0f * BELL_CURVE_WIDTH * BELL_CURVE_WIDTH));
}

std::string GetSampleOffsetsPropertyName( unsigned int index )
{
  std::ostringstream oss;
  oss << "uSampleOffsets[" << index << "]";
  return oss.str();
}

std::string GetSampleWeightsPropertyName( unsigned int index )
{
  std::ostringstream oss;
  oss << "uSampleWeights[" << index << "]";
  return oss.str();
}


void SetShaderConstants(Actor actor, Direction direction, uint32_t numSamples, float downsampledWidth, float downsampledHeight)
{
  Vector2 *uvOffsets;
  float ofs;
  float *weights;
  float w, totalWeights;
  unsigned int i;

  uvOffsets = new Vector2[numSamples + 1];
  weights = new float[numSamples + 1];

  totalWeights = weights[0] = CalcGaussianWeight(0);
  uvOffsets[0].x = 0.0f;
  uvOffsets[0].y = 0.0f;

  for(i=0; i<numSamples >> 1; i++)
  {
    w = CalcGaussianWeight((float)(i + 1));
    weights[(i << 1) + 1] = w;
    weights[(i << 1) + 2] = w;
    totalWeights += w * 2.0f;

    // offset texture lookup to between texels, that way the bilinear filter in the texture hardware will average two samples with one lookup
    ofs = ((float)(i << 1)) + 1.5f;

    // get offsets from units of pixels into uv coordinates in [0..1]
    float ofsX = ofs / downsampledWidth;
    float ofsY = ofs / downsampledHeight;
    uvOffsets[(i << 1) + 1].x = ofsX;
    uvOffsets[(i << 1) + 1].y = ofsY;

    uvOffsets[(i << 1) + 2].x = -ofsX;
    uvOffsets[(i << 1) + 2].y = -ofsY;
  }

  for(i=0; i<numSamples; i++)
  {
    weights[i] /= totalWeights;
  }

  // set shader constants
  Vector2 xAxis(1.0f, 0.0f);
  Vector2 yAxis(0.0f, 1.0f);
  for (i = 0; i < numSamples; ++i )
  {
    switch( direction )
    {
      case Direction::HORIZONTAL:
      {
        actor.RegisterProperty( GetSampleOffsetsPropertyName( i ), uvOffsets[ i ] * xAxis );
        actor.RegisterProperty( GetSampleWeightsPropertyName( i ), weights[ i ] );
        break;
      }
      case Direction::VERTICAL:
      {
        actor.RegisterProperty( GetSampleOffsetsPropertyName( i ), uvOffsets[ i ] * yAxis );
        actor.RegisterProperty( GetSampleWeightsPropertyName( i ), weights[ i ] );
        break;
      }
    }
  }

  delete[] uvOffsets;
  delete[] weights;
}

} // namespace Util
