#ifndef __DALI_DEMO_GRID_FLAGS_H_
#define __DALI_DEMO_GRID_FLAGS_H_
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
#include <algorithm>
#include <cassert>
#include <dali/dali.h>

/** Controls the output of application logging. */
//#define DEBUG_PRINT_GRID_DIAGNOSTICS

namespace Dali
{
namespace Demo
{
/**
 * @brief A 2D grid of booleans, settable and gettable via integer (x,y) coordinates.
 * */
class GridFlags
{
public:
  /**
   * Create grid of specified dimensions.
   */
  GridFlags( unsigned width, unsigned height ) :  mCells( width * height ), mWidth( width ), mHeight( height ), mHighestUsedRow( 0 )
  {
#ifdef DEBUG_PRINT_GRID_DIAGNOSTICS
      fprintf(stderr, "Grid created with dimensions: (%u, %u).\n", mWidth, mHeight );
#endif
  }

  void Set( const unsigned x, const unsigned y )
  {
    const unsigned index = CellIndex( x, y );
    mCells[index] += 1u; ///< += To allow a debug check of the number of times a cell is set.
    mHighestUsedRow = std::max( mHighestUsedRow, y );
  }

  bool Get( unsigned x, unsigned y ) const
  {
    return mCells[ CellIndex( x, y ) ] != 0;
  }

  unsigned GetHighestUsedRow() const
  {
    return mHighestUsedRow;
  }

  /**
   * @brief Try to find space in a grid of cells for the region requested.
   * @return true if any region (not necessarily an exact match) was found, else false.
   * @param[in] region The rectangular region requested
   * @param[out] outCellX The X coordinate of the region allocated, if any.
   * @param[out] outCellY The Y coordinate of the region allocated, if any.
   * @param[out] outRegion The rectangle actually found: the lowest-Y exact match region
   *             or the largest area rectangular region no greater than the requested
   *             region in x or y. Undefined if false is returned.
   */
  bool AllocateRegion( const Vector2& region, unsigned& outCellX, unsigned& outCellY, Vector2& outRegion )
  {
    const unsigned regionWidth = (region.x + 0.5f);
    const unsigned regionHeight = (region.y + 0.5f);
#ifdef DEBUG_PRINT_GRID_DIAGNOSTICS
      fprintf( stderr, "Allocation requested for region (%u, %u). Result: ", regionWidth, regionHeight );
#endif
    unsigned bestRegionWidth = 0;
    unsigned bestRegionHeight = 0;
    unsigned bestCellX = 0;
    unsigned bestCellY = 0;

    // Look for a non-set cell:
    for( unsigned y = 0; y < mHeight; ++y )
    {
      for( unsigned x = 0; x < mWidth; ++x )
      {
        if ( !Get( x, y) )
        {
          // Look for clear grid cells under the desired region:

          const unsigned clampedRegionHeight = std::min( regionHeight, mHeight - y);
          const unsigned clampedRegionWidth = std::min( regionWidth, mWidth - x);
          const unsigned regionLimitY = y + clampedRegionHeight;
          const unsigned regionLimitX = x + clampedRegionWidth;

          for( unsigned regionY = y; regionY < regionLimitY; ++regionY )
          {
            for( unsigned regionX = x; regionX < regionLimitX; ++regionX )
            {
              if( Get( regionX, regionY ) )
              {
                // The region of clear cells is not big enough but remember it
                // anyway in case there is no region that fits:
                const unsigned clearRegionWidth = regionX - x;
                const unsigned clearRegionHeight = (regionY + 1) - y;
                if( clearRegionWidth * clearRegionHeight > bestRegionWidth * bestRegionHeight )
                {
                  bestCellX = x;
                  bestCellY = y;
                  bestRegionWidth = clearRegionWidth;
                  bestRegionHeight = clearRegionHeight;
                }
                goto whole_region_not_found;
              }
            }
          }

          // Every cell in the region is clear so check if it is the best one yet:
          if( clampedRegionWidth * clampedRegionHeight > bestRegionWidth * bestRegionHeight )
          {
            bestCellX = x;
            bestCellY = y;
            bestRegionWidth = clampedRegionWidth;
            bestRegionHeight = clampedRegionHeight;
          }

          // If a big-enough region was found, end the search early and greedily allocate it:
          if( clampedRegionHeight == regionHeight && clampedRegionWidth == regionWidth )
          {
            x = mWidth;
            y = mHeight;
          }
whole_region_not_found:
          continue;
        }
      }
    }

    // Allocate and return the best cell region found:

    if( bestRegionWidth == 0 || bestRegionHeight == 0 )
    {
#ifdef DEBUG_PRINT_GRID_DIAGNOSTICS
        fputs( "false.\n", stderr );
#endif
      return false;
    }

    // Allocate the found region:
#ifdef DEBUG_PRINT_GRID_DIAGNOSTICS
      fprintf( stderr, " - bestCellX = %u, bestCellY = %u, bestRegionWidth = %u, bestRegionHeight = %u - ", bestCellX, bestCellY, bestRegionWidth, bestRegionHeight );
#endif
    for( unsigned y = bestCellY; y < bestCellY + bestRegionHeight; ++y )
    {
      for( unsigned x = bestCellX; x < bestCellX + bestRegionWidth; ++x )
      {
        Set( x, y );
      }
    }

    outCellX = bestCellX;
    outCellY = bestCellY;
    outRegion = Vector2( bestRegionWidth, bestRegionHeight );
#ifdef DEBUG_PRINT_GRID_DIAGNOSTICS
      fputs( "true.\n", stderr );
#endif
    return true;
  }

  /** @return True if every cell was set one or zero times, else false. */
  bool DebugCheckGridValid()
  {
    for( unsigned cell = 0; cell < mWidth * mHeight; ++cell )
    {
      if( mCells[cell] > 1 )
      {
        return false;
      }
    }
    return true;
  }

private:
  unsigned CellIndex( unsigned x, unsigned y ) const
  {
    const unsigned offset = mWidth * y + x;
    assert( offset < mCells.size() && "Out of range access to grid." );
    return offset;
  }

  std::vector<unsigned char> mCells;
  const unsigned mWidth;
  const unsigned mHeight;
  unsigned mHighestUsedRow;
};

} /* namespace Demo */
} /* namespace Dali */

#endif /* __DALI_DEMO_GRID_FLAGS_H_ */
