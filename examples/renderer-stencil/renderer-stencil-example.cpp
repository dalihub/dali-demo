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
#include <dali-toolkit/dali-toolkit.h>

// INTERNAL INCLUDES
#include "renderer-stencil-shaders.h"
#include "shared/view.h"
#include "shared/utility.h"

using namespace Dali;

namespace
{

// Constants:

// Application constants:
const char * const APPLICATION_TITLE( "Renderer Stencil API Demo" );
const char * const BACKGROUND_IMAGE( DEMO_IMAGE_DIR "background-gradient.jpg" );

// Texture filenames:
const char * const CUBE_TEXTURE( DEMO_IMAGE_DIR "people-medium-1.jpg" );
const char * const FLOOR_TEXTURE( DEMO_IMAGE_DIR "wood.png" );

// Scale dimensions: These values are relative to the stage size. EG. width = 0.32f * stageSize.
const float   CUBE_WIDTH_SCALE( 0.32f );                   ///< The width (and height + depth) of the main and reflection cubes.
const Vector2 FLOOR_DIMENSION_SCALE( 0.67f, 0.017f );      ///< The width and height of the floor object.

// Configurable animation characteristics:
const float ANIMATION_ROTATION_DURATION( 10.0f );          ///< Time in seconds to rotate the scene 360 degrees around Y.
const float ANIMATION_BOUNCE_TOTAL_TIME( 1.6f );           ///< Time in seconds to perform 1 full bounce animation cycle.
const float ANIMATION_BOUNCE_DEFORMATION_TIME( 0.4f );     ///< Time in seconds that the cube deformation animation will occur for (on contact with the floor).
const float ANIMATION_BOUNCE_DEFORMATION_PERCENT( 20.0f ); ///< Percentage (of the cube's size) to deform the cube by (on contact with floor).
const float ANIMATION_BOUNCE_HEIGHT_PERCENT( 40.0f );      ///< Percentage (of the cube's size) to bounce up in to the air by.

// Base colors for the objects:
const Vector4 TEXT_COLOR( 1.0f, 1.0f, 1.0f, 1.0f );        ///< White.
const Vector4 CUBE_COLOR( 1.0f, 1.0f, 1.0f, 1.0f );        ///< White.
const Vector4 FLOOR_COLOR( 1.0f, 1.0f, 1.0f, 1.0f );       ///< White.
const Vector4 REFLECTION_COLOR( 0.6f, 0.6f, 0.6f, 0.6f );  ///< Note that alpha is not 1.0f, to make the blend more photo-realistic.

// We need to control the draw order as we are controlling both the stencil and depth buffer per renderer.
const int DEPTH_INDEX_GRANULARITY( 10000 );                ///< This value is the gap in depth-index in-between each renderer.

} // Anonymous namespace

/**
 * @brief This example shows how to manipulate stencil and depth buffer properties within the Renderer API.
 */
class RendererStencilExample : public ConnectionTracker
{
public:

  /**
   * @brief Constructor.
   * @param[in] application The DALi application object
   */
  RendererStencilExample( Application& application )
  : mApplication( application )
  {
    // Connect to the Application's Init signal.
    mApplication.InitSignal().Connect( this, &RendererStencilExample::Create );
  }

  /**
   * @brief Destructor (non-virtual).
   */
  ~RendererStencilExample()
  {
  }

private:

  /**
   * @brief Enum to facilitate more readable use of the cube array.
   */
  enum CubeType
  {
    MAIN_CUBE,      ///< The main cube that bounces above the floor object.
    REFLECTION_CUBE ///< The reflected cube object.
  };

  /**
   * @brief Struct to store the position, normal and texture coordinates of a single vertex.
   */
  struct TexturedVertex
  {
    Vector3 position;
    Vector3 normal;
    Vector2 textureCoord;
  };

  /**
   * @brief This is the main scene setup method for this demo.
   * This is called via the Init signal which is received once (only) during the Application lifetime.
   * @param[in] application The DALi application object
   */
  void Create( Application& application )
  {
    Stage stage = Stage::GetCurrent();

    // Hide the indicator bar
    application.GetWindow().ShowIndicator( Dali::Window::INVISIBLE );

    // Use a gradient visual to render the background gradient.
    Toolkit::Control background = Dali::Toolkit::Control::New();
    background.SetAnchorPoint( Dali::AnchorPoint::CENTER );
    background.SetParentOrigin( Dali::ParentOrigin::CENTER );
    background.SetResizePolicy( Dali::ResizePolicy::FILL_TO_PARENT, Dali::Dimension::ALL_DIMENSIONS );

    // Set up the background gradient.
    Property::Array stopOffsets;
    stopOffsets.PushBack( 0.0f );
    stopOffsets.PushBack( 1.0f );
    Property::Array stopColors;
    stopColors.PushBack( Vector4( 0.17f, 0.24f, 0.35f, 1.0f ) ); // Dark, medium saturated blue  ( top   of screen)
    stopColors.PushBack( Vector4( 0.45f, 0.70f, 0.80f, 1.0f ) ); // Medium bright, pastel blue   (bottom of screen)
    const float percentageStageHeight = stage.GetSize().height * 0.7f;

    background.SetProperty( Toolkit::Control::Property::BACKGROUND, Dali::Property::Map()
      .Add( Toolkit::Visual::Property::TYPE, Dali::Toolkit::Visual::GRADIENT )
      .Add( Toolkit::GradientVisual::Property::STOP_OFFSET, stopOffsets )
      .Add( Toolkit::GradientVisual::Property::STOP_COLOR, stopColors )
      .Add( Toolkit::GradientVisual::Property::START_POSITION, Vector2( 0.0f, -percentageStageHeight ) )
      .Add( Toolkit::GradientVisual::Property::END_POSITION, Vector2( 0.0f, percentageStageHeight ) )
      .Add( Toolkit::GradientVisual::Property::UNITS, Toolkit::GradientVisual::Units::USER_SPACE ) );

    stage.Add( background );

    // Create a TextLabel for the application title.
    Toolkit::TextLabel label = Toolkit::TextLabel::New( APPLICATION_TITLE );
    label.SetAnchorPoint( AnchorPoint::TOP_CENTER );
    // Set the parent origin to a small percentage below the top (so the demo will scale for different resolutions).
    label.SetParentOrigin( Vector3( 0.5f, 0.03f, 0.5f ) );
    label.SetProperty( Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
    label.SetProperty( Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );
    label.SetProperty( Toolkit::TextLabel::Property::TEXT_COLOR, TEXT_COLOR );
    stage.Add( label );

    // Layer to hold the 3D scene.
    Layer layer = Layer::New();
    layer.SetAnchorPoint( AnchorPoint::CENTER );
    // Set the parent origin to a small percentage below the center (so the demo will scale for different resolutions).
    layer.SetParentOrigin( Vector3( 0.5f, 0.58f, 0.5f ) );
    layer.SetBehavior( Layer::LAYER_2D );
    layer.SetDepthTestDisabled( false );
    stage.Add( layer );

    // Main cube:
    // Make the demo scalable with different resolutions by basing
    // the cube size on a percentage of the stage size.
    float scaleSize( std::min( stage.GetSize().width, stage.GetSize().height ) );
    float cubeWidth( scaleSize * CUBE_WIDTH_SCALE );
    Vector3 cubeSize( cubeWidth, cubeWidth, cubeWidth );
    // Create the geometry for the cube, and the texture.
    Geometry cubeGeometry = CreateCubeVertices( Vector3::ONE, false );
    TextureSet cubeTextureSet = CreateTextureSet( CUBE_TEXTURE );
    // Create the cube object and add it.
    // Note: The cube is anchored around its base for animation purposes, so the position can be zero.
    mCubes[ MAIN_CUBE ] = CreateMainCubeObject( cubeGeometry, cubeSize, cubeTextureSet );
    layer.Add( mCubes[ MAIN_CUBE ] );

    // Floor:
    float floorWidth( scaleSize * FLOOR_DIMENSION_SCALE.x );
    Vector3 floorSize( floorWidth, scaleSize * FLOOR_DIMENSION_SCALE.y, floorWidth );
    // Create the floor object using the cube geometry with a new size, and add it.
    Actor floorObject( CreateFloorObject( cubeGeometry, floorSize ) );
    layer.Add( floorObject );

    // Stencil:
    Vector3 planeSize( floorWidth, floorWidth, 0.0f );
    // Create the stencil plane object, and add it.
    Actor stencilPlaneObject( CreateStencilPlaneObject( planeSize ) );
    layer.Add( stencilPlaneObject );

    // Reflection cube:
    // Create the reflection cube object and add it.
    // Note: The cube is anchored around its base for animation purposes, so the position can be zero.
    mCubes[ REFLECTION_CUBE ] = CreateReflectionCubeObject( cubeSize, cubeTextureSet );
    layer.Add( mCubes[ REFLECTION_CUBE ] );

    // Rotate the layer so we can see some of the top of the cube for a more 3D effect.
    layer.SetProperty( Actor::Property::ORIENTATION, Quaternion( Degree( -24.0f ), Degree( 0.0f ), Degree( 0.0f ) ) );

    // Set up the rotation on the Y axis.
    mRotationAnimation = Animation::New( ANIMATION_ROTATION_DURATION );
    float fullRotation = 360.0f;
    mRotationAnimation.AnimateBy( Property( mCubes[ MAIN_CUBE ], Actor::Property::ORIENTATION ),
                                 Quaternion( Degree( 0.0f ), Degree( fullRotation ), Degree( 0.0f ) ) );
    mRotationAnimation.AnimateBy( Property( floorObject, Actor::Property::ORIENTATION ),
                                 Quaternion( Degree( 0.0f ), Degree( fullRotation ), Degree( 0.0f ) ) );
    // Note the stencil is pre-rotated by 90 degrees on X, so we rotate relatively on its Z axis for an equivalent Y rotation.
    mRotationAnimation.AnimateBy( Property( stencilPlaneObject, Actor::Property::ORIENTATION ),
                                 Quaternion( Degree( 0.0f ), Degree( 0.0f ), Degree( fullRotation ) ) );
    mRotationAnimation.AnimateBy( Property( mCubes[ REFLECTION_CUBE ], Actor::Property::ORIENTATION ),
                                 Quaternion( Degree( 0.0f ), Degree( fullRotation ), Degree( 0.0f ) ) );
    mRotationAnimation.SetLooping( true );

    // Set up the cube bouncing animation.
    float totalTime = ANIMATION_BOUNCE_TOTAL_TIME;
    float deformationTime = ANIMATION_BOUNCE_DEFORMATION_TIME;
    // Percentage based amounts allows the bounce and deformation to scale for different resolution screens.
    float deformationAmount = ANIMATION_BOUNCE_DEFORMATION_PERCENT / 100.0f;
    float heightChange = ( cubeSize.y * ANIMATION_BOUNCE_HEIGHT_PERCENT ) / 100.0f;

    // Animation pre-calculations:
    float halfTime = totalTime / 2.0f;
    float halfDeformationTime = deformationTime / 2.0f;

    // First position the cubes at the top of the animation cycle.
    mCubes[ MAIN_CUBE ].SetProperty(       Actor::Property::POSITION_Y, -heightChange );
    mCubes[ REFLECTION_CUBE ].SetProperty( Actor::Property::POSITION_Y,  heightChange );

    mBounceAnimation = Animation::New( totalTime );

    // The animations for the main and reflected cubes are almost identical, so we combine the code to do both.
    for( int cube = 0; cube < 2; ++cube )
    {
      // If iterating on the reflection cube, adjust the heightChange variable so the below code can be reused.
      if( cube == 1 )
      {
        heightChange = -heightChange;
      }

      // 1st TimePeriod: Start moving down with increasing speed, until it is time to distort the cube due to impact.
      mBounceAnimation.AnimateBy( Property( mCubes[ cube ], Actor::Property::POSITION_Y ),  heightChange, AlphaFunction::EASE_IN_SQUARE, TimePeriod( 0.0f, halfTime - halfDeformationTime ) );

      // 2nd TimePeriod: The cube is touching the floor, start deforming it - then un-deform it again.
      mBounceAnimation.AnimateBy( Property( mCubes[ cube ], Actor::Property::SCALE_X ),  deformationAmount, AlphaFunction::BOUNCE, TimePeriod( halfTime - halfDeformationTime, deformationTime ) );
      mBounceAnimation.AnimateBy( Property( mCubes[ cube ], Actor::Property::SCALE_Z ),  deformationAmount, AlphaFunction::BOUNCE, TimePeriod( halfTime - halfDeformationTime, deformationTime ) );
      mBounceAnimation.AnimateBy( Property( mCubes[ cube ], Actor::Property::SCALE_Y ), -deformationAmount, AlphaFunction::BOUNCE, TimePeriod( halfTime - halfDeformationTime, deformationTime ) );

      // 3rd TimePeriod: Start moving up with decreasing speed, until at the apex of the animation.
      mBounceAnimation.AnimateBy( Property( mCubes[ cube ], Actor::Property::POSITION_Y ), -heightChange, AlphaFunction::EASE_OUT_SQUARE, TimePeriod( halfTime + halfDeformationTime, halfTime - halfDeformationTime ) );
    }

    mBounceAnimation.SetLooping( true );

    // Start the animations.
    mRotationAnimation.Play();
    mBounceAnimation.Play();

    // Respond to a click anywhere on the stage
    stage.GetRootLayer().TouchSignal().Connect( this, &RendererStencilExample::OnTouch );
    // Connect signals to allow Back and Escape to exit.
    stage.KeyEventSignal().Connect( this, &RendererStencilExample::OnKeyEvent );
  }

private:

  // Methods to setup each component of the 3D scene:

  /**
   * @brief Creates the Main cube object.
   * This creates the renderer from existing geometry (as the cubes geometry is shared).
   * The texture is set and all relevant renderer properties are set-up.
   * @param[in] geometry Pre-calculated cube geometry
   * @param[in] size The desired cube size
   * @param[in] textureSet A pre-existing TextureSet with a texture set up, to be applied to the cube
   * @return An actor set-up containing the main cube object
   */
  Actor CreateMainCubeObject( Geometry& geometry, Vector3 size, TextureSet& textureSet )
  {
    Toolkit::Control container = Toolkit::Control::New();
    container.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
    container.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
    container.SetSize( size );
    container.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );

    // Create a renderer from the geometry and add the texture.
    Renderer renderer = CreateRenderer( geometry, size, true, CUBE_COLOR );
    renderer.SetTextures( textureSet );

    // Setup the renderer properties:
    // We are writing to the color buffer & culling back faces (no stencil is used for the main cube).
    renderer.SetProperty( Renderer::Property::RENDER_MODE, RenderMode::COLOR );
    renderer.SetProperty( Renderer::Property::FACE_CULLING_MODE, FaceCullingMode::BACK );

    // We do need to write to the depth buffer as other objects need to appear underneath this cube.
    renderer.SetProperty( Renderer::Property::DEPTH_WRITE_MODE, DepthWriteMode::ON );
    // We do not need to test the depth buffer as we are culling the back faces.
    renderer.SetProperty( Renderer::Property::DEPTH_TEST_MODE, DepthTestMode::OFF );

    // This object must be rendered 1st.
    renderer.SetProperty( Renderer::Property::DEPTH_INDEX, 0 * DEPTH_INDEX_GRANULARITY );

    container.AddRenderer( renderer );
    return container;
  }

  /**
   * @brief Creates the Floor object.
   * This creates the renderer from existing geometry (as the cube geometry can be re-used).
   * The texture is created and set and all relevant renderer properties are set-up.
   * @param[in] geometry Pre-calculated cube geometry
   * @param[in] size The desired floor size
   * @return An actor set-up containing the floor object
   */
  Actor CreateFloorObject( Geometry& geometry, Vector3 size )
  {
    Toolkit::Control container = Toolkit::Control::New();
    container.SetAnchorPoint( AnchorPoint::TOP_CENTER );
    container.SetParentOrigin( ParentOrigin::TOP_CENTER );
    container.SetSize( size );
    container.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );

    // Create a renderer from the geometry and add the texture.
    TextureSet planeTextureSet = CreateTextureSet( FLOOR_TEXTURE );
    Renderer renderer = CreateRenderer( geometry, size, true, FLOOR_COLOR );
    renderer.SetTextures( planeTextureSet );

    // Setup the renderer properties:
    // We are writing to the color buffer & culling back faces as we are NOT doing depth write (no stencil is used for the floor).
    renderer.SetProperty( Renderer::Property::RENDER_MODE, RenderMode::COLOR );
    renderer.SetProperty( Renderer::Property::FACE_CULLING_MODE, FaceCullingMode::BACK );

    // We do not write to the depth buffer as its not needed.
    renderer.SetProperty( Renderer::Property::DEPTH_WRITE_MODE, DepthWriteMode::OFF );
    // We do need to test the depth buffer as we need the floor to be underneath the cube.
    renderer.SetProperty( Renderer::Property::DEPTH_TEST_MODE, DepthTestMode::ON );

    // This object must be rendered 2nd.
    renderer.SetProperty( Renderer::Property::DEPTH_INDEX, 1 * DEPTH_INDEX_GRANULARITY );

    container.AddRenderer( renderer );
    return container;
  }

  /**
   * @brief Creates the Stencil-Plane object.
   * This is places on the floor object to allow the reflection to be drawn on to the floor.
   * This creates the geometry and renderer.
   * All relevant renderer properties are set-up.
   * @param[in] size The desired plane size
   * @return An actor set-up containing the stencil-plane object
   */
  Actor CreateStencilPlaneObject( Vector3 size )
  {
    Toolkit::Control container = Toolkit::Control::New();
    container.SetAnchorPoint( AnchorPoint::CENTER );
    container.SetParentOrigin( ParentOrigin::CENTER );
    container.SetSize( size );
    container.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );

    // We rotate the plane as the geometry is created flat in X & Y. We want it to span X & Z axis.
    container.SetProperty( Actor::Property::ORIENTATION, Quaternion( Degree( -90.0f ), Degree( 0.0f ), Degree( 0.0f ) ) );

    // Create geometry for a flat plane.
    Geometry planeGeometry = CreatePlaneVertices( Vector2::ONE );
    // Create a renderer from the geometry.
    Renderer renderer = CreateRenderer( planeGeometry, size, false, Vector4::ONE );

    // Setup the renderer properties:
    // The stencil plane is only for stencilling.
    renderer.SetProperty( Renderer::Property::RENDER_MODE, RenderMode::STENCIL );

    renderer.SetProperty( Renderer::Property::STENCIL_FUNCTION, StencilFunction::ALWAYS );
    renderer.SetProperty( Renderer::Property::STENCIL_FUNCTION_REFERENCE, 1 );
    renderer.SetProperty( Renderer::Property::STENCIL_FUNCTION_MASK, 0xFF );
    renderer.SetProperty( Renderer::Property::STENCIL_OPERATION_ON_FAIL, StencilOperation::KEEP );
    renderer.SetProperty( Renderer::Property::STENCIL_OPERATION_ON_Z_FAIL, StencilOperation::KEEP );
    renderer.SetProperty( Renderer::Property::STENCIL_OPERATION_ON_Z_PASS, StencilOperation::REPLACE );
    renderer.SetProperty( Renderer::Property::STENCIL_MASK, 0xFF );

    // We don't want to write to the depth buffer, as this would block the reflection being drawn.
    renderer.SetProperty( Renderer::Property::DEPTH_WRITE_MODE, DepthWriteMode::OFF );
    // We test the depth buffer as we want the stencil to only exist underneath the cube.
    renderer.SetProperty( Renderer::Property::DEPTH_TEST_MODE, DepthTestMode::ON );

    // This object must be rendered 3rd.
    renderer.SetProperty( Renderer::Property::DEPTH_INDEX, 2 * DEPTH_INDEX_GRANULARITY );

    container.AddRenderer( renderer );
    return container;
  }

  /**
   * @brief Creates the Reflection cube object.
   * This creates new geometry (as the texture UVs are different to the main cube).
   * The renderer is then created.
   * The texture is set and all relevant renderer properties are set-up.
   * @param[in] size The desired cube size
   * @param[in] textureSet A pre-existing TextureSet with a texture set up, to be applied to the cube
   * @return An actor set-up containing the reflection cube object
   */
  Actor CreateReflectionCubeObject( Vector3 size, TextureSet& textureSet )
  {
    Toolkit::Control container = Toolkit::Control::New();
    container.SetAnchorPoint( AnchorPoint::TOP_CENTER );
    container.SetParentOrigin( ParentOrigin::TOP_CENTER );
    container.SetSize( size );
    container.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );

    // Create the cube geometry of unity size.
    // The "true" specifies we want the texture UVs flipped vertically as this is the reflection cube.
    Geometry reflectedCubeGeometry = CreateCubeVertices( Vector3::ONE, true );
    // Create a renderer from the geometry and add the texture.
    Renderer renderer = CreateRenderer( reflectedCubeGeometry, size, true, REFLECTION_COLOR );
    renderer.SetTextures( textureSet );

    // Setup the renderer properties:
    // Write to color buffer so reflection is visible.
    // Also enable the stencil buffer, as we will be testing against it to only draw to areas within the stencil.
    renderer.SetProperty( Renderer::Property::RENDER_MODE, RenderMode::COLOR_STENCIL );
    // We cull to skip drawing the back faces.
    renderer.SetProperty( Renderer::Property::FACE_CULLING_MODE, FaceCullingMode::BACK );

    // We use blending to blend the reflection with the floor texture.
    renderer.SetProperty( Renderer::Property::BLEND_MODE, BlendMode::ON );
    renderer.SetProperty( Renderer::Property::BLEND_EQUATION_RGB, BlendEquation::ADD );
    renderer.SetProperty( Renderer::Property::BLEND_EQUATION_ALPHA, BlendEquation::ADD );
    renderer.SetProperty( Renderer::Property::BLEND_FACTOR_DEST_RGB, BlendFactor::ONE );

    // Enable stencil. Here we only draw to areas within the stencil.
    renderer.SetProperty( Renderer::Property::STENCIL_FUNCTION, StencilFunction::EQUAL );
    renderer.SetProperty( Renderer::Property::STENCIL_FUNCTION_REFERENCE, 1 );
    renderer.SetProperty( Renderer::Property::STENCIL_FUNCTION_MASK, 0xff );
    // Don't write to the stencil.
    renderer.SetProperty( Renderer::Property::STENCIL_MASK, 0x00 );

    // We don't need to write to the depth buffer, as we are culling.
    renderer.SetProperty( Renderer::Property::DEPTH_WRITE_MODE, DepthWriteMode::OFF );
    // We need to test the depth buffer as we need the reflection to be underneath the cube.
    renderer.SetProperty( Renderer::Property::DEPTH_TEST_MODE, DepthTestMode::ON );

    // This object must be rendered last.
    renderer.SetProperty( Renderer::Property::DEPTH_INDEX, 3 * DEPTH_INDEX_GRANULARITY );

    container.AddRenderer( renderer );
    return container;
  }

  // Methods:

  /**
   * @brief Creates a geometry object from vertices and indices.
   * @param[in] vertices The object vertices
   * @param[in] indices The object indices
   * @return A geometry object
   */
  Geometry CreateTexturedGeometry( Vector<TexturedVertex>& vertices, Vector<unsigned short>& indices )
  {
    // Vertices
    Property::Map vertexFormat;
    vertexFormat[POSITION] = Property::VECTOR3;
    vertexFormat[NORMAL] =   Property::VECTOR3;
    vertexFormat[TEXTURE] =  Property::VECTOR2;

    PropertyBuffer surfaceVertices = PropertyBuffer::New( vertexFormat );
    surfaceVertices.SetData( &vertices[0u], vertices.Size() );

    Geometry geometry = Geometry::New();
    geometry.AddVertexBuffer( surfaceVertices );

    // Indices for triangle formulation
    geometry.SetIndexBuffer( &indices[0u], indices.Size() );
    return geometry;
  }

  /**
   * @brief Creates a renderer from a geometry object.
   * @param[in] geometry The geometry to use
   * @param[in] dimensions The dimensions (will be passed in to the shader)
   * @param[in] textured Set to true to use the texture versions of the shaders
   * @param[in] color The base color for the renderer
   * @return A renderer object
   */
  Renderer CreateRenderer( Geometry geometry, Vector3 dimensions, bool textured, Vector4 color )
  {
    Stage stage = Stage::GetCurrent();
    Shader shader;

    if( textured )
    {
      shader = Shader::New( VERTEX_SHADER_TEXTURED, FRAGMENT_SHADER_TEXTURED );
    }
    else
    {
      shader = Shader::New( VERTEX_SHADER, FRAGMENT_SHADER );
    }

    // Here we modify the light position based on half the stage size as a pre-calculation step.
    // This avoids the work having to be done in the shader.
    shader.RegisterProperty( LIGHT_POSITION_UNIFORM_NAME, Vector3( -stage.GetSize().width / 2.0f, -stage.GetSize().width / 2.0f, 1000.0f ) );
    shader.RegisterProperty( COLOR_UNIFORM_NAME, color );
    shader.RegisterProperty( OBJECT_DIMENSIONS_UNIFORM_NAME, dimensions );

    return Renderer::New( geometry, shader );
  }

  /**
   * @brief Helper method to create a TextureSet from an image URL.
   * @param[in] url An image URL
   * @return A TextureSet object
   */
  TextureSet CreateTextureSet( const char* url )
  {
    TextureSet textureSet = TextureSet::New();

    if( textureSet )
    {
      Texture texture = DemoHelper::LoadTexture( url );
      if( texture )
      {
        textureSet.SetTexture( 0u, texture );
      }
    }

    return textureSet;
  }

  // Geometry Creation:

  /**
   * @brief Creates a geometry object for a flat plane.
   * The plane is oriented in X & Y axis (Z is 0).
   * @param[in] dimensions The desired plane dimensions
   * @return A Geometry object
   */
  Geometry CreatePlaneVertices( Vector2 dimensions )
  {
    Vector<TexturedVertex> vertices;
    Vector<unsigned short> indices;
    vertices.Resize( 4u );
    indices.Resize( 6u );

    float scaledX = 0.5f * dimensions.x;
    float scaledY = 0.5f * dimensions.y;

    vertices[0].position     = Vector3( -scaledX, -scaledY, 0.0f );
    vertices[0].textureCoord = Vector2( 0.0, 0.0f );
    vertices[1].position     = Vector3(  scaledX, -scaledY, 0.0f );
    vertices[1].textureCoord = Vector2( 1.0, 0.0f );
    vertices[2].position     = Vector3(  scaledX,  scaledY, 0.0f );
    vertices[2].textureCoord = Vector2( 1.0, 1.0f );
    vertices[3].position     = Vector3( -scaledX,  scaledY, 0.0f );
    vertices[3].textureCoord = Vector2( 0.0, 1.0f );

    // All vertices have the same normal.
    for( int i = 0; i < 4; ++i )
    {
      vertices[i].normal = Vector3( 0.0f, 0.0f, -1.0f );
    }

    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;
    indices[3] = 2;
    indices[4] = 3;
    indices[5] = 0;

    // Use the helper method to create the geometry object.
    return CreateTexturedGeometry( vertices, indices );
  }

  /**
   * @brief Creates a geometry object for a cube (or cuboid).
   * @param[in] dimensions The desired cube dimensions
   * @param[in] reflectVerticalUVs Set to True to force the UVs to be vertically flipped
   * @return A Geometry object
   */
  Geometry CreateCubeVertices( Vector3 dimensions, bool reflectVerticalUVs )
  {
    Vector<TexturedVertex> vertices;
    Vector<unsigned short> indices;
    int vertexIndex = 0u; // Tracks progress through vertices.
    float scaledX = 0.5f * dimensions.x;
    float scaledY = 0.5f * dimensions.y;
    float scaledZ = 0.5f * dimensions.z;
    float verticalTextureCoord = reflectVerticalUVs ? 0.0f : 1.0f;

    vertices.Resize( 4u * 6u ); // 4 vertices x 6 faces

    Vector<Vector3> positions;  // Stores vertex positions, which are shared between vertexes at the same position but with a different normal.
    positions.Resize( 8u );
    Vector<Vector3> normals;    // Stores normals, which are shared between vertexes of the same face.
    normals.Resize( 6u );

    positions[0] = Vector3( -scaledX,  scaledY, -scaledZ );
    positions[1] = Vector3(  scaledX,  scaledY, -scaledZ );
    positions[2] = Vector3(  scaledX,  scaledY,  scaledZ );
    positions[3] = Vector3( -scaledX,  scaledY,  scaledZ );
    positions[4] = Vector3( -scaledX, -scaledY, -scaledZ );
    positions[5] = Vector3(  scaledX, -scaledY, -scaledZ );
    positions[6] = Vector3(  scaledX, -scaledY,  scaledZ );
    positions[7] = Vector3( -scaledX, -scaledY,  scaledZ );

    normals[0] = Vector3(  0,  1,  0 );
    normals[1] = Vector3(  0,  0, -1 );
    normals[2] = Vector3(  1,  0,  0 );
    normals[3] = Vector3(  0,  0,  1 );
    normals[4] = Vector3( -1,  0,  0 );
    normals[5] = Vector3(  0, -1,  0 );

    // Top face, upward normals.
    for( int i = 0; i < 4; ++i, ++vertexIndex )
    {
      vertices[vertexIndex].position = positions[i];
      vertices[vertexIndex].normal = normals[0];
      // The below logic forms the correct U/V pairs for a quad when "i" goes from 0 to 3.
      vertices[vertexIndex].textureCoord = Vector2( ( i == 1 || i == 2 ) ? 1.0f : 0.0f, ( i == 2 || i == 3 ) ? 1.0f : 0.0f );
    }

    // Top face, outward normals.
    for( int i = 0; i < 4; ++i, vertexIndex += 2 )
    {
      vertices[vertexIndex].position = positions[i];
      vertices[vertexIndex].normal = normals[i + 1];

      if( i == 3 )
      {
        // End, so loop around.
        vertices[vertexIndex + 1].position = positions[0];
      }
      else
      {
        vertices[vertexIndex + 1].position = positions[i + 1];
      }
      vertices[vertexIndex + 1].normal = normals[i + 1];

      vertices[vertexIndex].textureCoord = Vector2( 0.0f, verticalTextureCoord );
      vertices[vertexIndex+1].textureCoord = Vector2( 1.0f, verticalTextureCoord );
    }

    // Flip the vertical texture coord for the UV values of the bottom points.
    verticalTextureCoord = 1.0f - verticalTextureCoord;

    // Bottom face, outward normals.
    for( int i = 0; i < 4; ++i, vertexIndex += 2 )
    {
      vertices[vertexIndex].position = positions[i + 4];
      vertices[vertexIndex].normal = normals[i + 1];

      if( i == 3 )
      {
        // End, so loop around.
        vertices[vertexIndex + 1].position = positions[4];
      }
      else
      {
        vertices[vertexIndex + 1].position = positions[i + 5];
      }
      vertices[vertexIndex + 1].normal = normals[i + 1];

      vertices[vertexIndex].textureCoord = Vector2( 0.0f, verticalTextureCoord );
      vertices[vertexIndex+1].textureCoord = Vector2( 1.0f, verticalTextureCoord );
    }

    // Bottom face, downward normals.
    for( int i = 0; i < 4; ++i, ++vertexIndex )
    {
      // Reverse positions for bottom face to keep triangles clockwise (for culling).
      vertices[vertexIndex].position = positions[ 7 - i ];
      vertices[vertexIndex].normal = normals[5];
      // The below logic forms the correct U/V pairs for a quad when "i" goes from 0 to 3.
      vertices[vertexIndex].textureCoord = Vector2( ( i == 1 || i == 2 ) ? 1.0f : 0.0f, ( i == 2 || i == 3 ) ? 1.0f : 0.0f );
    }

    // Create cube indices.
    int triangleIndex = 0u;     //Track progress through indices.
    indices.Resize( 3u * 12u ); // 3 points x 12 triangles.

    // Top face.
    indices[triangleIndex] =     0;
    indices[triangleIndex + 1] = 1;
    indices[triangleIndex + 2] = 2;
    indices[triangleIndex + 3] = 2;
    indices[triangleIndex + 4] = 3;
    indices[triangleIndex + 5] = 0;
    triangleIndex += 6;

    int topFaceStart = 4u;
    int bottomFaceStart = topFaceStart + 8u;

    // Side faces.
    for( int i = 0; i < 8; i += 2, triangleIndex += 6 )
    {
      indices[triangleIndex    ] = i + topFaceStart;
      indices[triangleIndex + 1] = i + bottomFaceStart + 1;
      indices[triangleIndex + 2] = i + topFaceStart + 1;
      indices[triangleIndex + 3] = i + topFaceStart;
      indices[triangleIndex + 4] = i + bottomFaceStart;
      indices[triangleIndex + 5] = i + bottomFaceStart + 1;
    }

    // Bottom face.
    indices[triangleIndex] =     20;
    indices[triangleIndex + 1] = 21;
    indices[triangleIndex + 2] = 22;
    indices[triangleIndex + 3] = 22;
    indices[triangleIndex + 4] = 23;
    indices[triangleIndex + 5] = 20;

    // Use the helper method to create the geometry object.
    return CreateTexturedGeometry( vertices, indices );
  }

  // Signal handlers:

  /**
   * @brief OnTouch signal handler.
   * @param[in] actor The actor that has been touched
   * @param[in] touch The touch information
   * @return True if the event has been handled
   */
  bool OnTouch( Actor actor, const TouchData& touch )
  {
    // Quit the application.
    mApplication.Quit();
    return true;
  }

  /**
   * @brief OnKeyEvent signal handler.
   * @param[in] event The key event information
   */
  void OnKeyEvent( const KeyEvent& event )
  {
    if( event.state == KeyEvent::Down )
    {
      if ( IsKey( event, Dali::DALI_KEY_ESCAPE ) || IsKey( event, Dali::DALI_KEY_BACK ) )
      {
        mApplication.Quit();
      }
    }
  }

private:

  // Member variables:

  Application&     mApplication;       ///< The DALi application object
  Toolkit::Control mView;              ///< The view used to show the background

  Animation        mRotationAnimation; ///< The animation to spin the cube & floor
  Animation        mBounceAnimation;   ///< The animation to bounce the cube
  Actor            mCubes[2];          ///< The cube object containers
};

int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );
  RendererStencilExample example( application );
  application.MainLoop();
  return 0;
}
