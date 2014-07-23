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

#include <sstream>
#include <iomanip>

#include "../shared/view.h"
#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;
using namespace Dali::Toolkit;
using namespace std;
using namespace DemoHelper;

namespace // unnamed namespace
{

const char * const BACKGROUND_IMAGE( DALI_IMAGE_DIR "background-default.png" );
const char * const TOOLBAR_IMAGE( DALI_IMAGE_DIR "top-bar.png" );
const char * const APPLICATION_TITLE( "Clusters" );
const char * const LAYOUT_NONE_IMAGE( DALI_IMAGE_DIR "icon-cluster-none.png" );
const char * const LAYOUT_WOBBLE_IMAGE( DALI_IMAGE_DIR "icon-cluster-wobble.png" );
const char * const LAYOUT_CAROUSEL_IMAGE( DALI_IMAGE_DIR "icon-cluster-carousel.png" );
const char * const LAYOUT_SPHERE_IMAGE( DALI_IMAGE_DIR "icon-cluster-sphere.png" );

enum ClusterType
{
  PEOPLE,
  TODAY,
  PHONE,
  PICTURES,
  MUSIC,
  MAGAZINE,

  CLUSTER_COUNT
};

const char* PEOPLE_IMAGE_PATHS[] =   { DALI_IMAGE_DIR "people-medium-1.jpg",
                                       DALI_IMAGE_DIR "people-medium-2.jpg",
                                       DALI_IMAGE_DIR "people-medium-3.jpg",
                                       DALI_IMAGE_DIR "people-medium-4.jpg",
                                       DALI_IMAGE_DIR "people-medium-5.jpg",
                                       DALI_IMAGE_DIR "people-medium-6.jpg",
                                       DALI_IMAGE_DIR "people-medium-7.jpg",
                                       DALI_IMAGE_DIR "people-medium-8.jpg",
                                       NULL };

const char* TODAY_IMAGE_PATHS[] =    { DALI_IMAGE_DIR "gallery-medium-1.jpg",
                                       DALI_IMAGE_DIR "gallery-medium-2.jpg",
                                       DALI_IMAGE_DIR "gallery-medium-3.jpg",
                                       DALI_IMAGE_DIR "gallery-medium-4.jpg",
                                       DALI_IMAGE_DIR "gallery-medium-5.jpg",
                                       DALI_IMAGE_DIR "gallery-medium-6.jpg",
                                       NULL };

const char* PHONE_IMAGE_PATHS[] =    { DALI_IMAGE_DIR "gallery-medium-7.jpg",
                                       DALI_IMAGE_DIR "gallery-medium-8.jpg",
                                       DALI_IMAGE_DIR "gallery-medium-9.jpg",
                                       DALI_IMAGE_DIR "gallery-medium-10.jpg",
                                       DALI_IMAGE_DIR "gallery-medium-11.jpg",
                                       DALI_IMAGE_DIR "gallery-medium-12.jpg",
                                       NULL };

const char* PICTURES_IMAGE_PATHS[] = { DALI_IMAGE_DIR "gallery-medium-13.jpg",
                                       DALI_IMAGE_DIR "gallery-medium-14.jpg",
                                       DALI_IMAGE_DIR "gallery-medium-15.jpg",
                                       DALI_IMAGE_DIR "gallery-medium-16.jpg",
                                       DALI_IMAGE_DIR "gallery-medium-17.jpg",
                                       DALI_IMAGE_DIR "gallery-medium-18.jpg",
                                       NULL };

const char* MUSIC_IMAGE_PATHS[] =    { DALI_IMAGE_DIR "gallery-medium-19.jpg",
                                       DALI_IMAGE_DIR "gallery-medium-20.jpg",
                                       DALI_IMAGE_DIR "gallery-medium-21.jpg",
                                       DALI_IMAGE_DIR "gallery-medium-22.jpg",
                                       DALI_IMAGE_DIR "gallery-medium-23.jpg",
                                       DALI_IMAGE_DIR "gallery-medium-24.jpg",
                                       NULL };

const char* MAGAZINE_IMAGE_PATHS[] = { DALI_IMAGE_DIR "gallery-medium-25.jpg",
                                       DALI_IMAGE_DIR "gallery-medium-26.jpg",
                                       DALI_IMAGE_DIR "gallery-medium-27.jpg",
                                       DALI_IMAGE_DIR "gallery-medium-28.jpg",
                                       DALI_IMAGE_DIR "gallery-medium-29.jpg",
                                       DALI_IMAGE_DIR "gallery-medium-30.jpg",
                                       NULL };

const char **IMAGE_GROUPS[] = {PEOPLE_IMAGE_PATHS,
                               TODAY_IMAGE_PATHS,
                               PHONE_IMAGE_PATHS,
                               PICTURES_IMAGE_PATHS,
                               MUSIC_IMAGE_PATHS,
                               MAGAZINE_IMAGE_PATHS,
                               NULL};

const float CLUSTER_IMAGE_THUMBNAIL_WIDTH  = 256.0f;
const float CLUSTER_IMAGE_THUMBNAIL_HEIGHT = 256.0f;

const char* CLUSTER_SHADOW_IMAGE_PATH = DALI_IMAGE_DIR "cluster-image-shadow.png";
const char* CLUSTER_BORDER_IMAGE_PATH = DALI_IMAGE_DIR "cluster-image-frame.png";
const char* CLUSTER_BACKGROUND_IMAGE_PATH = DALI_IMAGE_DIR "cluster-background.png";

const float CLUSTER_IMAGE_BORDER_INDENT = 14.0f;            ///< Indent of border in pixels.
const float CLUSTER_IMAGE_BORDER_WIDTH = 128;               ///< Width of border in pixels.
const float CLUSTER_IMAGE_BORDER_HEIGHT = 128;              ///< Height of border in pixels.

const Vector4 CLUSTER_IMAGE_BORDER_ABSOLUTE( 16.0f, 16.0f, 16.0f, 16.0f ); // Border dimensions in absolute pixel coordinates.

// These values depend on the border image
const float CLUSTER_RELATIVE_SIZE = 0.65f;                  ///< Cluster size relative to screen width

const float CLUSTER_GROUP_DELAY_TOP = 0.25f;                ///< Delay for top Clusters in seconds.
const float CLUSTER_GROUP_DELAY_BOTTOM = 0.0f;              ///< Delay for bottom Clusters in seconds.

const float CLUSTER_COLUMN_INDENT = 0.1f;                   ///< Left Indentation in screen coordinates.
const float CLUSTER_ROW_INDENT = 0.13f;                     ///< Top Indentation in screen coordinates.

const Vector3 SHEAR_EFFECT_ANCHOR_POINT(0.5f, 1.0f, 0.5f);  ///< Anchor Point used for the shear effect (extends outside of Cluster)
const float SHEAR_EFFECT_MAX_OVERSHOOT = 30.0f;             ///< Max Overshoot for shear effect (in degrees).

const float UI_MARGIN = 10.0f;                              ///< Screen Margin for placement of UI buttons

const float CAROUSEL_EFFECT_RADIUS = 500.0f;                ///< In Carousel Effect mode: Radius of carousel (Z peak depth)
const float CAROUSEL_EFFECT_ANGLE_SWEEP = 90.0f;            ///< In Carousel Effect mode: Angle sweep from left to right of screen
const float SPHERE_EFFECT_RADIUS = 1000.0f;                 ///< In Sphere Effect mode: Radius of sphere carousel (Z peak depth)
const float SPHERE_EFFECT_POSITION_Z = -700.0f;             ///< In Sphere Effect mode: Z position alter (as carousel is coming out to screen we move back)
const float SPHERE_EFFECT_ANGLE_SWEEP = 90.0f;              ///< In Sphere Effect mode: Angle sweep from edge to opposite side of circle.
const float SPHERE_EFFECT_VERTICAL_DOMAIN = 0.15f;          ///< In Sphere Effect mode: How much the user can pan in the vertical axis. (in stageHeights)

/**
 * List of effect types that user can select through.
 */
enum ExampleEffectType
{
  NO_EFFECT,
  WOBBLE_EFFECT,
  CAROUSEL_EFFECT,
  SPHERE_EFFECT,
  TOTAL_EFFECTS
};

/**
 * List of effect type names that appear on the Effect button.
 */
const char* EXAMPLE_EFFECT_LABEL[] = { "NONE",
                                       "WOBBLE",
                                       "CAROUSEL",
                                       "SPHERE",
                                     };

/**
 * CarouselEffectOrientationConstraint
 * Based on the View orientation i.e. portrait (0 degrees), landscape (90 degrees) etc.
 * carousel shader effect should bend differently (as a function of this orientation),
 * as shader effect is applied to the screen coordinates.
 */
struct CarouselEffectOrientationConstraint
{
  /**
   * Constructor
   * @param[in] angleSweep The amount of degrees to rotate by per pixel.
   */
  CarouselEffectOrientationConstraint( const Vector2 angleSweep )
  : mAngleSweep( angleSweep )
  {
  }

  /**
   * @param[in] current The object's current property value
   * @return The object's new property value
   */
  Vector2 operator()(const Vector2& current,
                     const PropertyInput& propertyOrientation)
  {
    Vector3 axis;
    float angle;
    propertyOrientation.GetQuaternion().ToAxisAngle( axis, angle );
    Vector2 direction( cosf(angle), sinf(angle) );

    return mAngleSweep * direction;
  }

  Vector2 mAngleSweep;

};

/**
 * RescaleConstraint
 * Rescales the inputer scale by the ratio of the height:width of it's parent.
 */
struct RescaleConstraint
{
  /**
   * Constructor
   * @param[in] value0 Constant multiplication operand (K).
   */
  RescaleConstraint()
  {
  }

  /**
   * @param[in] current The object's current property value
   * @param[in] property0 The first property to multiplication operand
   * @return The object's new property value
   */
  Vector3 operator()(const Vector3&    current,
                     const PropertyInput& property0)
  {
    return current * Vector3( property0.GetVector3().y / property0.GetVector3().x, 1.0f, 1.0f );
  }

};

/**
 * ClusterImageBorderSizeConstraint
 */
struct ClusterImageBorderSizeConstraint
{
  ClusterImageBorderSizeConstraint()
  : mSizeOffset(Vector3(CLUSTER_IMAGE_BORDER_INDENT - 1, CLUSTER_IMAGE_BORDER_INDENT - 1, 0.0f) * 2.0f)
  {
  }

  Vector3 operator()(const Vector3&    current,
                     const PropertyInput& referenceSizeProperty)
  {
    const Vector3& referenceSize = referenceSizeProperty.GetVector3();

    return referenceSize + mSizeOffset;
  }

  Vector3 mSizeOffset;                        ///< The amount to offset the size from referenceSize
};

/**
 * ShearEffectConstraint
 *
 * Constrains ShearEffect's tilt to be a function of scrollview's
 * horizontal overshoot amount.
 */
struct ShearEffectConstraint
{
  /**
   * @param[in] stageSize The stage size (not subject to orientation)
   * @param[in] maxOvershoot Maximum amount overshoot can affect shear.
   * @param[in] componentMask Whether constraint should take the X shear
   * or the Y shear component.
   */
  ShearEffectConstraint(Vector2 stageSize, float maxOvershoot, Vector2 componentMask)
  : mStageSize(stageSize),
    mMaxOvershoot(maxOvershoot),
    mComponentMask(componentMask)
  {
  }

  /**
   * @param[in] current The current shear effect Angle.
   * @param[in] scrollOvershootProperty The overshoot property from ScrollView
   * @param[in] propertyViewOrientation The orientation of the view e.g. Portrait, Landscape.
   * @return angle to provide ShearShaderEffect
   */
  float operator()(const float&    current,
                   const PropertyInput& scrollOvershootProperty,
                   const PropertyInput& propertyViewOrientation)
  {
    float f = scrollOvershootProperty.GetVector3().x;

    float mag = fabsf(f);
    float halfWidth = mStageSize.x * 0.5f;

    // inverse exponential tail-off
    float overshoot = 1.0f - halfWidth / (halfWidth + mag);
    if (f > 0.0f)
    {
      overshoot = -overshoot;
    }

    // Channel this shear value into either the X or Y axis depending on
    // the component mask passed in.
    Vector3 axis;
    float angle;
    propertyViewOrientation.GetQuaternion().ToAxisAngle( axis, angle );
    Vector2 direction( cosf(angle), sinf(angle) );
    float yield = direction.x * mComponentMask.x + direction.y * mComponentMask.y;

    return overshoot * mMaxOvershoot * yield;
  }

  Vector2 mStageSize;
  float mMaxOvershoot;
  Vector2 mComponentMask;
};

/**
 * ShearEffectCenterConstraint
 *
 * Sets ShearEffect's center to be a function of the
 * screen orientation (portrait or landscape).
 */
struct ShearEffectCenterConstraint
{
  /**
   * @param[in] stageSize The stage size (not subject to orientation)
   * @param[in] center Shear Center position based on initial orientation.
   */
  ShearEffectCenterConstraint(Vector2 stageSize, Vector2 center)
  : mStageSize(stageSize),
    mCenter(center)
  {
  }

  /**
   * @param[in] current The current center
   * @param[in] propertyViewSize The current view size
   * @return vector to provide ShearShaderEffect
   */
  Vector2 operator()(const Vector2&    current,
                     const PropertyInput& propertyViewSize)
  {
    float f = propertyViewSize.GetVector3().width / mStageSize.width;
    return Vector2( f * mCenter.x, mCenter.y );
  }

  Vector2 mStageSize;
  Vector2 mCenter;
};

/**
 * SphereEffectOffsetConstraint
 *
 * Sets SphereEffect's center to be a function of the
 * screen orientation (portrait or landscape).
 */
struct SphereEffectOffsetConstraint
{
  /**
   * @param[in] stageSize The stage size (not subject to orientation)
   * @param[in] center Shear Center position based on initial orientation.
   */
  SphereEffectOffsetConstraint(float offset)
  : mOffset(offset)
  {
  }

  /**
   * @param[in] current The current center
   * @param[in] propertyViewSize The current view size
   * @return vector to provide SphereShaderEffect
   */
  float operator()(const float& current)
  {
    return current + mOffset;
  }

  float mOffset;
};

/**
 * ClusterInfo struct
 *
 * Contains information about each cluster in mClusterInfo list.
 */
struct ClusterInfo
{
  /**
   * Constructor
   *
   * @param[in] cluster The cluster instance
   * @param[in] index The cluster's index (starting from 0 for the first cluster)
   * @param[in] position The cluster's original position
   * @param[in] size The cluster's original size
   */
  ClusterInfo(Cluster cluster, int index, const Vector3& position, const Vector3& size)
  : mCluster(cluster),
    mIndex(index),
    mPosition(position),
    mSize(size)
  {

  }

  /**
   * Copy constructor
   *
   * @param[in] rhs The ClusterInfo struct to be copied.
   */
  ClusterInfo( const ClusterInfo& rhs )
  : mCluster(rhs.mCluster),
    mIndex(rhs.mIndex),
    mPosition(rhs.mPosition),
    mSize(rhs.mSize)
  {

  }

  Cluster mCluster;                   ///< Cluster instance
  int mIndex;                         ///< Cluster index
  Vector3 mPosition;                  ///< Cluster original position
  Vector3 mSize;                      ///< Cluster original size
  ActiveConstraint mEffectConstraint; ///< Cluster constraint
};

/**
 * Shrinks Actor based on parent's aspect ratio.
 */
struct ShrinkConstraint
{
  /**
   * Constructor
   */
  ShrinkConstraint()
  {
  }

  /**
   * @param[in] current The object's current scale value
   * @param[in] parentScale The parent's scale
   * @return The object's new scale value
   */
  Vector3 operator()(const Vector3&    current,
                     const PropertyInput& parentScale)
  {
    return Vector3( parentScale.GetVector3().x / parentScale.GetVector3().y, 1.0f, 1.0f );
  }
};

} // unnamed namespace

/**
 * Custom position and size of shadow image
 */
namespace ShadowProperty
{
const Vector3 ANCHOR_POINT      (0.54f, 0.6f, 0.5f);
const Vector3 RELATIVE_POSITION (0.0f, 0.0f, -0.1f);
const Vector3 SIZE_SCALE        (1.4f, 1.4f, 1.0f);
}

// This example shows how to use Cluster UI control
//
class ClusterController : public ConnectionTracker
{
public:

  /**
   * Constructor
   * @param application class, stored as reference
   */
  ClusterController(Application &app)
  : mApplication(app),
    mClusterCount(0),
    mExampleEffect(NO_EFFECT)
  {
    // Connect to the Application's Init signal
    app.InitSignal().Connect(this, &ClusterController::Create);
  }

  ~ClusterController()
  {
    // Nothing to do here; everything gets deleted automatically
  }

  /**
   * This method gets called once the main loop of application is up and running
   */
  void Create(Application& application)
  {
    Stage::GetCurrent().KeyEventSignal().Connect(this, &ClusterController::OnKeyEvent);

    Vector2 stageSize = Stage::GetCurrent().GetSize();

    // The Init signal is received once (only) during the Application lifetime

    // Hide the indicator bar
    application.GetWindow().ShowIndicator( false );

    // Creates a default view with a default tool bar.
    // The view is added to the stage.
    mContentLayer = DemoHelper::CreateView( application,
                                            mView,
                                            mToolBar,
                                            BACKGROUND_IMAGE,
                                            TOOLBAR_IMAGE,
                                            "" );

    // Create a effect toggle button. (right of toolbar)
    mLayoutButtonImages[ NO_EFFECT ] = Image::New( LAYOUT_NONE_IMAGE );
    mLayoutButtonImages[ WOBBLE_EFFECT ] = Image::New( LAYOUT_WOBBLE_IMAGE );
    mLayoutButtonImages[ CAROUSEL_EFFECT ] = Image::New( LAYOUT_CAROUSEL_IMAGE );
    mLayoutButtonImages[ SPHERE_EFFECT ] = Image::New( LAYOUT_SPHERE_IMAGE );

    mLayoutButton = Toolkit::PushButton::New();
    mLayoutButton.ClickedSignal().Connect( this, &ClusterController::OnEffectTouched );
    mToolBar.AddControl( mLayoutButton, DemoHelper::DEFAULT_VIEW_STYLE.mToolBarButtonPercentage, Toolkit::Alignment::HorizontalRight, DemoHelper::DEFAULT_MODE_SWITCH_PADDING  );

    // create and setup the scroll view...
    mScrollView = ScrollView::New();
    mScrollView.SetSize(stageSize);

    // attach Wobble Effect to ScrollView
    mScrollViewEffect = ScrollViewWobbleEffect::New();
    mScrollView.ApplyEffect(mScrollViewEffect);

    // anchor the scroll view from its center point to the middle of its parent
    mScrollView.SetAnchorPoint(AnchorPoint::CENTER);
    mScrollView.SetParentOrigin(ParentOrigin::CENTER);

    // Scale ScrollView to fit within parent (mContentLayer)
    Constraint constraint = Constraint::New<Vector3>( Actor::SCALE,
                                                    LocalSource( Actor::SIZE ),
                                                    ParentSource( Actor::SIZE ),
                                                    ScaleToFitConstraint() );
    mScrollView.ApplyConstraint(constraint);

    // Add the scroll view to the content layer
    mContentLayer.Add(mScrollView);

    // Create the image border shared by all the cluster image actors
    mClusterBorderImage = Image::New(CLUSTER_BORDER_IMAGE_PATH);

    AddCluster( PEOPLE,   ClusterStyleStandard::New(ClusterStyleStandard::ClusterStyle1) );
    AddCluster( TODAY,    ClusterStyleStandard::New(ClusterStyleStandard::ClusterStyle2) );
    AddCluster( PHONE,    ClusterStyleStandard::New(ClusterStyleStandard::ClusterStyle3) );
    AddCluster( PICTURES, ClusterStyleStandard::New(ClusterStyleStandard::ClusterStyle4) );
    AddCluster( MUSIC,    ClusterStyleStandard::New(ClusterStyleStandard::ClusterStyle2) );
    AddCluster( MAGAZINE, ClusterStyleStandard::New(ClusterStyleStandard::ClusterStyle3) );

    SetEffect(WOBBLE_EFFECT);
  }

  /**
   * Helper to create the cluster actors
   */
  Cluster CreateClusterActor(ClusterType clusterType, ClusterStyle style)
  {
    // Create the cluster actor with the given cluster style
    Cluster clusterActor = Cluster::New(style);
    clusterActor.SetParentOrigin(ParentOrigin::CENTER);
    clusterActor.SetAnchorPoint(AnchorPoint::CENTER);

    Vector2 stageSize = Dali::Stage::GetCurrent().GetSize();
    float minStageDimension = min(stageSize.x, stageSize.y);
    clusterActor.SetSize(minStageDimension * CLUSTER_RELATIVE_SIZE, minStageDimension * CLUSTER_RELATIVE_SIZE, 0.0f);

    DALI_ASSERT_ALWAYS(clusterType < CLUSTER_COUNT);
    const char **paths = IMAGE_GROUPS[clusterType];
    DALI_ASSERT_ALWAYS(paths);

    // Add a background image to the cluster
    Image bg = Image::New( CLUSTER_BACKGROUND_IMAGE_PATH );
    ImageActor image = ImageActor::New(bg);
    clusterActor.SetBackgroundImage(image);

    // Add actors (pictures) as the children of the cluster
    for (unsigned int i = 0; (i < style.GetMaximumNumberOfChildren()) && (*paths); i++, paths++)
    {
      clusterActor.AddChild(CreateClusterPictureActor(clusterType, *paths), i);
    }

    return clusterActor;
  }

  /**
   * Helper to create the picture actors in the cluster
   */
  Actor CreateClusterPictureActor(ClusterType clusterType, const std::string& imagePath)
  {
    // Create a picture for this cluster image
    // actor|->shadow
    //      |->image
    //      |->border
    Actor actor = Actor::New();
    actor.SetSize(CLUSTER_IMAGE_THUMBNAIL_WIDTH, CLUSTER_IMAGE_THUMBNAIL_HEIGHT);
    actor.SetParentOrigin( ParentOrigin::CENTER );
    actor.SetAnchorPoint( AnchorPoint::CENTER );

    // Load the thumbnail
    ImageAttributes attribs = ImageAttributes::New();
    attribs.SetSize(CLUSTER_IMAGE_THUMBNAIL_WIDTH, CLUSTER_IMAGE_THUMBNAIL_HEIGHT);
    attribs.SetScalingMode(Dali::ImageAttributes::ShrinkToFit);
    attribs.SetPixelFormat( Pixel::RGB888  );

    // Add a shadow image child actor
    Image shadowImage = Image::New( CLUSTER_SHADOW_IMAGE_PATH, attribs );
    ImageActor shadowActor = ImageActor::New(shadowImage);

    // Shadow is not exactly located on the center of the image, so it is moved to a little
    // upper-left side of the image relatively using customised AnchorPoint.
    shadowActor.SetParentOrigin(ShadowProperty::ANCHOR_POINT);
    shadowActor.SetAnchorPoint(AnchorPoint::CENTER);
    shadowActor.SetPosition(Vector3(0.0f, 0.0f, -1.0f));

    // Apply size constraint to the image shadow
    shadowActor.ApplyConstraint( Constraint::New<Vector3>( Actor::SIZE, ParentSource( Actor::SIZE ), RelativeToConstraint( ShadowProperty::SIZE_SCALE ) ) );
    actor.Add(shadowActor);

    // Add a picture image actor to actor.
    Image image = Image::New( imagePath, attribs );
    ImageActor imageActor = ImageActor::New(image);
    imageActor.SetParentOrigin( ParentOrigin::CENTER );
    imageActor.SetAnchorPoint( AnchorPoint::CENTER );
    imageActor.ApplyConstraint( Constraint::New<Vector3>( Actor::SIZE, ParentSource( Actor::SIZE ), EqualToConstraint() ) );
    actor.Add(imageActor);

    // Add a border image child actor
    ImageActor borderActor = ImageActor::New(mClusterBorderImage);
    borderActor.SetParentOrigin( ParentOrigin::CENTER );
    borderActor.SetAnchorPoint( AnchorPoint::CENTER );
    borderActor.SetStyle( ImageActor::STYLE_NINE_PATCH );
    borderActor.SetNinePatchBorder( CLUSTER_IMAGE_BORDER_ABSOLUTE );
    borderActor.SetPosition(Vector3(0.0f, 0.0f, 1.0f));

    // Apply size constraint to the image border
    Constraint constraint = Constraint::New<Vector3>(Actor::SIZE,
                                                     ParentSource(Actor::SIZE),
                                                     ClusterImageBorderSizeConstraint());
    borderActor.ApplyConstraint(constraint);
    actor.Add(borderActor);

    return actor;
  }


  /**
   * Adds a Cluster to the ScrollView
   *
   * @param[in] clusterType The type of cluster (determines the image content)
   * @param[in] style The style to be used for this cluster.
   */
  void AddCluster(ClusterType clusterType, ClusterStyle style)
  {
    Vector2 stageSize = Dali::Stage::GetCurrent().GetSize();

    int column = mClusterCount>>1;
    int row = mClusterCount&1;

    float minStageDimension = min(stageSize.x, stageSize.y);
    float clusterRightShift = 1.0f - CLUSTER_COLUMN_INDENT * 2.0f;
    Vector3 clusterPosition = Vector3(CLUSTER_COLUMN_INDENT * stageSize.width + row * (clusterRightShift * stageSize.width - minStageDimension * CLUSTER_RELATIVE_SIZE),
                                      CLUSTER_ROW_INDENT * stageSize.height + row * (clusterRightShift * stageSize.height - minStageDimension * CLUSTER_RELATIVE_SIZE), 0.0f);

    Actor pageView = Actor::New();
    mScrollView.Add(pageView);
    pageView.SetParentOrigin(ParentOrigin::CENTER);
    pageView.SetPosition(Vector3(stageSize.width * column, 0.0f, 0.0f));
    pageView.SetSize(stageSize);

    // Resize pageView (which contains a Cluster)
    Constraint constraintScale = Constraint::New<Vector3>( Actor::SCALE,
                                                           ParentSource( Actor::SCALE ),
                                                           RescaleConstraint() );
    constraintScale.SetRemoveAction(Constraint::Discard);
    pageView.ApplyConstraint(constraintScale);

    // Create cluster actors, add them to scroll view, and set the shear effect with the given center point.
    Cluster cluster = CreateClusterActor(clusterType, style);
    cluster.SetParentOrigin(ParentOrigin::TOP_LEFT);
    cluster.SetAnchorPoint(AnchorPoint::TOP_LEFT);
    cluster.SetPosition( clusterPosition );

    pageView.Add(cluster);
    Vector3 clusterSize = cluster.GetCurrentSize();

    mClusterInfo.push_back(ClusterInfo( cluster, mClusterCount, clusterPosition, clusterSize ));

    mClusterCount++;
  }

  /**
   * Resets ScrollView and Clusters settings
   * to reflect the new ExampleEffectType
   *
   * TODO: Add animation transition to fade out of existing effect,
   * and into new effect.
   *
   * @param[in] type The desired effect to switch to.
   */
  void SetEffect(ExampleEffectType type)
  {
    Vector2 stageSize(Dali::Stage::GetCurrent().GetSize());

    mExampleEffect = type;

    std::stringstream ss(APPLICATION_TITLE);
    ss << APPLICATION_TITLE << ": " << EXAMPLE_EFFECT_LABEL[mExampleEffect];
    SetTitle(ss.str());

    // Set up default ruler settings (fixed in horizontal, disabled in vertical)
    RulerPtr rulerX;
    rulerX = new FixedRuler(stageSize.x);
    int columns = (mClusterCount + 1) >> 1;
    rulerX->SetDomain(RulerDomain(0.0f, stageSize.x * columns, true));
    mScrollView.SetRulerX(rulerX);

    RulerPtr rulerY = new DefaultRuler();
    rulerY->Disable();
    mScrollView.SetRulerY(rulerY);

    mScrollView.SetActorAutoSnap(false);

    // Remove all shader-effects from mScrollView and it's children (the clusters)
    mScrollView.RemoveShaderEffect();
    mScrollView.SetPosition(Vector3::ZERO);

    mLayoutButton.SetBackgroundImage( mLayoutButtonImages[ type ] );

    for( vector<ClusterInfo>::iterator i = mClusterInfo.begin(); i != mClusterInfo.end(); ++i )
    {
      Cluster cluster = i->mCluster;
      cluster.RemoveShaderEffect();
      if( i->mEffectConstraint )
      {
        cluster.RemoveConstraint(i->mEffectConstraint);
        i->mEffectConstraint = 0;
      }
    }

    // Apply new shader-effects.
    // Move Y to origin incase we came from an effect where user could free pan in y axis.
    const Vector3 currentScrollPosition(mScrollView.GetCurrentScrollPosition());
    mScrollView.ScrollTo(Vector3(currentScrollPosition.x, 0.0f, 0.0f));

    switch(type)
    {
      case NO_EFFECT:
      {
        break;
      }

      case WOBBLE_EFFECT:
      {
        for( vector<ClusterInfo>::iterator i = mClusterInfo.begin(); i != mClusterInfo.end(); ++i )
        {
          Cluster cluster = i->mCluster;
          Vector3 position = i->mPosition;
          Vector3 size = i->mSize;

          ShearEffect shaderEffect = ShearEffect::New();
          Vector3 shearAnchor = SHEAR_EFFECT_ANCHOR_POINT;

          Vector2 shearCenter( Vector2(position.x + size.width * shearAnchor.x, position.y + size.height * shearAnchor.y) );
          Property::Index centerProperty = shaderEffect.GetPropertyIndex(shaderEffect.GetCenterPropertyName());
          Constraint constraint = Constraint::New<Vector2>( centerProperty,
                                                            Source(mView, Actor::SIZE),
                                                            ShearEffectCenterConstraint(stageSize, shearCenter) );
          shaderEffect.ApplyConstraint(constraint);

          cluster.SetShaderEffect(shaderEffect);

          // Apply Constraint to Shader Effect
          Property::Index scrollOvershootProperty = /*targetGroup*/mScrollView.GetPropertyIndex(ScrollViewWobbleEffect::EFFECT_OVERSHOOT);
          Property::Index angleXAxisProperty = shaderEffect.GetPropertyIndex(shaderEffect.GetAngleXAxisPropertyName());
          Property::Index angleYAxisProperty = shaderEffect.GetPropertyIndex(shaderEffect.GetAngleYAxisPropertyName());

          constraint = Constraint::New<float>( angleXAxisProperty,
                                               Source(mScrollView, scrollOvershootProperty),
                                               Source(mView, Actor::ROTATION),
                                               ShearEffectConstraint(stageSize, SHEAR_EFFECT_MAX_OVERSHOOT, Vector2::XAXIS) );
          shaderEffect.ApplyConstraint(constraint);
          constraint = Constraint::New<float>( angleYAxisProperty,
                                               Source(mScrollView, scrollOvershootProperty),
                                               Source(mView, Actor::ROTATION),
                                               ShearEffectConstraint(stageSize, SHEAR_EFFECT_MAX_OVERSHOOT, Vector2::YAXIS) );
          shaderEffect.ApplyConstraint(constraint);


        }
        break;
      }

      case CAROUSEL_EFFECT:
      {
        // Apply Carousel Shader Effect to scrollView
        CarouselEffect shaderEffect = CarouselEffect::New();
        shaderEffect.SetRadius( -CAROUSEL_EFFECT_RADIUS );
        mScrollView.SetShaderEffect( shaderEffect );
        mScrollView.SetPosition( Vector3( 0.0f, 0.0f, CAROUSEL_EFFECT_RADIUS ) );

        const Vector2 angleSweep( CAROUSEL_EFFECT_ANGLE_SWEEP / stageSize.width,
                                  CAROUSEL_EFFECT_ANGLE_SWEEP / stageSize.width );

        Property::Index anglePerUnit = shaderEffect.GetPropertyIndex( shaderEffect.GetAnglePerUnitPropertyName() );
        shaderEffect.ApplyConstraint( Constraint::New<Vector2>( anglePerUnit,
                                                                Source(mView, Actor::ROTATION),
                                                                CarouselEffectOrientationConstraint( angleSweep ) ) );

        break;
      }

      case SPHERE_EFFECT:
      {
        // Change ruler to free panning...
        RulerPtr rulerX;
        rulerX = new DefaultRuler();
        int columns = (mClusterCount + 1) >> 1;
        rulerX->SetDomain(RulerDomain(0.0f, stageSize.x * columns, true));
        mScrollView.SetRulerX(rulerX);

        RulerPtr rulerY = new DefaultRuler();
        rulerY->SetDomain(RulerDomain( -stageSize.y * SPHERE_EFFECT_VERTICAL_DOMAIN, stageSize.y * (1.0f + SPHERE_EFFECT_VERTICAL_DOMAIN), true));
        mScrollView.SetRulerY(rulerY);

        // Apply Carousel Shader Effect to scrollView (Spherical style)
        CarouselEffect shaderEffect = CarouselEffect::New();

        shaderEffect.SetRadius( SPHERE_EFFECT_RADIUS );
        shaderEffect.SetAnglePerUnit( Vector2( SPHERE_EFFECT_ANGLE_SWEEP / stageSize.y, SPHERE_EFFECT_ANGLE_SWEEP / stageSize.y ) );
        mScrollView.SetShaderEffect( shaderEffect );
        for( vector<ClusterInfo>::iterator i = mClusterInfo.begin(); i != mClusterInfo.end(); ++i )
        {
          Constraint constraint = Constraint::New<float>(Actor::POSITION_Z, SphereEffectOffsetConstraint(SPHERE_EFFECT_POSITION_Z));
          constraint.SetRemoveAction(Constraint::Discard);
          Cluster cluster = i->mCluster;
          i->mEffectConstraint = cluster.ApplyConstraint(constraint);
        }
        break;
      }

      default:
        break;
    }

  }

  /**
   * Signal handler, called when quit button is pressed
   */
  bool OnEffectTouched( Toolkit::Button button )
  {
    // quit the application
    SetEffect(static_cast<ExampleEffectType>( (mExampleEffect + 1) % TOTAL_EFFECTS) );
    return true;
  }

  /**
   * Sets/Updates the title of the View
   * @param[in] title The new title for the view.
   */
  void SetTitle(const std::string& title)
  {
    if(!mTitleActor)
    {
      mTitleActor = TextView::New();
      // Add title to the tool bar.
      mToolBar.AddControl( mTitleActor, DemoHelper::DEFAULT_VIEW_STYLE.mToolBarTitlePercentage, Alignment::HorizontalCenter );
    }

    Font font = Font::New();
    mTitleActor.SetText( title );
    mTitleActor.SetSize( font.MeasureText( title ) );
    mTitleActor.SetStyleToCurrentText(DemoHelper::GetDefaultTextStyle());
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

  Application&               mApplication;                       ///< Application instance
  Toolkit::View              mView;                              ///< The View instance.
  Toolkit::ToolBar           mToolBar;                           ///< The View's Toolbar.
  TextView                   mTitleActor;                        ///< The Toolbar's Title.

  Layer                      mContentLayer;                      ///< Content layer (scrolling cluster content)

  ScrollView                 mScrollView;                        ///< The ScrollView container for all clusters
  ScrollViewWobbleEffect     mScrollViewEffect;                  ///< ScrollView Wobble effect
  Image                      mClusterBorderImage;                ///< The border frame that appears on each image

  vector<ClusterInfo>        mClusterInfo;                       ///< Keeps track of each cluster's information.
  int                        mClusterCount;                      ///< Current number of clusters in use
  ExampleEffectType          mExampleEffect;                     ///< Current example effect.

  Toolkit::PushButton        mLayoutButton;                      ///< The layout button
  Image                      mLayoutButtonImages[TOTAL_EFFECTS]; ///< Image when no layout
};

void RunTest(Application& app)
{
  ClusterController test(app);

  app.MainLoop();
}

// Entry point for Linux & SLP applications
//
int main(int argc, char **argv)
{
  Application app = Application::New(&argc, &argv);

  RunTest(app);

  return 0;
}
