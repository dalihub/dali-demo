/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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

// CLASS HEADER
#include "clipping-item-factory.h"

// EXTERNAL INCLUDES
#include <dali/public-api/object/property-map.h>
#include <dali-toolkit/public-api/controls/image-view/image-view.h>
#include <dali-toolkit/public-api/visuals/border-visual-properties.h>
#include <dali-toolkit/public-api/visuals/visual-properties.h>
#include <dali-toolkit/public-api/visuals/image-visual-properties.h>
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
const char * IMAGE_PATHS[] = {
  DEMO_IMAGE_DIR "gallery-medium-1.jpg",
  DEMO_IMAGE_DIR "gallery-medium-2.jpg",
  DEMO_IMAGE_DIR "gallery-medium-3.jpg",
  DEMO_IMAGE_DIR "gallery-medium-4.jpg",
  DEMO_IMAGE_DIR "gallery-medium-5.jpg",
  DEMO_IMAGE_DIR "gallery-medium-6.jpg",
  DEMO_IMAGE_DIR "gallery-medium-7.jpg",
  DEMO_IMAGE_DIR "gallery-medium-8.jpg",
  DEMO_IMAGE_DIR "gallery-medium-9.jpg",
  DEMO_IMAGE_DIR "gallery-medium-10.jpg",
  DEMO_IMAGE_DIR "gallery-medium-11.jpg",
  DEMO_IMAGE_DIR "gallery-medium-12.jpg",
  DEMO_IMAGE_DIR "gallery-medium-13.jpg",
  DEMO_IMAGE_DIR "gallery-medium-14.jpg",
  DEMO_IMAGE_DIR "gallery-medium-15.jpg",
  DEMO_IMAGE_DIR "gallery-medium-16.jpg",
  DEMO_IMAGE_DIR "gallery-medium-17.jpg",
  DEMO_IMAGE_DIR "gallery-medium-18.jpg",
  DEMO_IMAGE_DIR "gallery-medium-19.jpg",
  DEMO_IMAGE_DIR "gallery-medium-20.jpg",
  DEMO_IMAGE_DIR "gallery-medium-21.jpg",
  DEMO_IMAGE_DIR "gallery-medium-22.jpg",
  DEMO_IMAGE_DIR "gallery-medium-23.jpg",
  DEMO_IMAGE_DIR "gallery-medium-24.jpg",
  DEMO_IMAGE_DIR "gallery-medium-25.jpg",
  DEMO_IMAGE_DIR "gallery-medium-26.jpg",
  DEMO_IMAGE_DIR "gallery-medium-27.jpg",
  DEMO_IMAGE_DIR "gallery-medium-28.jpg",
  DEMO_IMAGE_DIR "gallery-medium-29.jpg",
  DEMO_IMAGE_DIR "gallery-medium-30.jpg",
  DEMO_IMAGE_DIR "gallery-medium-31.jpg",
  DEMO_IMAGE_DIR "gallery-medium-32.jpg",
  DEMO_IMAGE_DIR "gallery-medium-33.jpg",
  DEMO_IMAGE_DIR "gallery-medium-34.jpg",
  DEMO_IMAGE_DIR "gallery-medium-35.jpg",
  DEMO_IMAGE_DIR "gallery-medium-36.jpg",
  DEMO_IMAGE_DIR "gallery-medium-37.jpg",
  DEMO_IMAGE_DIR "gallery-medium-38.jpg",
  DEMO_IMAGE_DIR "gallery-medium-39.jpg",
  DEMO_IMAGE_DIR "gallery-medium-40.jpg",
  DEMO_IMAGE_DIR "gallery-medium-41.jpg",
  DEMO_IMAGE_DIR "gallery-medium-42.jpg",
  DEMO_IMAGE_DIR "gallery-medium-43.jpg",
  DEMO_IMAGE_DIR "gallery-medium-44.jpg",
  DEMO_IMAGE_DIR "gallery-medium-45.jpg",
  DEMO_IMAGE_DIR "gallery-medium-46.jpg",
  DEMO_IMAGE_DIR "gallery-medium-47.jpg",
  DEMO_IMAGE_DIR "gallery-medium-48.jpg",
  DEMO_IMAGE_DIR "gallery-medium-49.jpg",
  DEMO_IMAGE_DIR "gallery-medium-50.jpg",
  DEMO_IMAGE_DIR "gallery-medium-51.jpg",
  DEMO_IMAGE_DIR "gallery-medium-52.jpg",
  DEMO_IMAGE_DIR "gallery-medium-53.jpg",
};
const unsigned int NUM_IMAGES = sizeof( IMAGE_PATHS ) / sizeof( char * );
const unsigned int NUM_IMAGES_MULTIPLIER = 10;

const float ITEM_BORDER_SIZE = 2.0f;
} // unnamed namespace

ClippingItemFactory::ClippingItemFactory()
{
}

unsigned int ClippingItemFactory::GetNumberOfItems()
{
  return NUM_IMAGES * NUM_IMAGES_MULTIPLIER;
}

Actor ClippingItemFactory::NewItem( unsigned int itemId )
{
  // Create an image view for this item
  Property::Map propertyMap;
  propertyMap.Insert(Visual::Property::TYPE,  Visual::IMAGE);
  propertyMap.Insert(ImageVisual::Property::URL, IMAGE_PATHS[ itemId % NUM_IMAGES ] );
  propertyMap.Insert(DevelVisual::Property::VISUAL_FITTING_MODE, DevelVisual::FILL);
  ImageView actor = ImageView::New();
  actor.SetProperty(Toolkit::ImageView::Property::IMAGE, propertyMap);

  // Add a border image child actor
  ImageView borderActor = ImageView::New();
  borderActor.SetParentOrigin( ParentOrigin::CENTER );
  borderActor.SetAnchorPoint( AnchorPoint::CENTER );
  borderActor.SetResizePolicy( ResizePolicy::SIZE_FIXED_OFFSET_FROM_PARENT, Dimension::ALL_DIMENSIONS );
  borderActor.SetSizeModeFactor( Vector3( 2.0f * ITEM_BORDER_SIZE, 2.0f * ITEM_BORDER_SIZE, 0.0f ) );
  borderActor.SetColorMode( USE_PARENT_COLOR );
  borderActor.SetProperty( ImageView::Property::IMAGE,
                           Property::Map().Add( Toolkit::Visual::Property::TYPE, Visual::BORDER )
                                          .Add( BorderVisual::Property::COLOR, Color::WHITE )
                                          .Add( BorderVisual::Property::SIZE, ITEM_BORDER_SIZE )
                                          .Add( BorderVisual::Property::ANTI_ALIASING, true ) );
  actor.Add(borderActor);

  return actor;
}
