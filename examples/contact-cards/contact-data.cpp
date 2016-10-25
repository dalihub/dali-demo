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

// HEADER
#include "contact-data.h"

namespace ContactData
{

const Item TABLE[] =
{
  { "Shelia Ramos",       "19 Wormley Ct\nWinters Way\nWaltham Abbey\nEN9 3HW", DEMO_IMAGE_DIR "gallery-small-19.jpg" },
  { "Walter Jensen",      "32 Upper Fant Rd\nMaidstone\nME16 8DN",              DEMO_IMAGE_DIR "gallery-small-2.jpg" },
  { "Randal Parks",       "8 Rymill St\nLondon\nE16 2JF",                       DEMO_IMAGE_DIR "gallery-small-3.jpg" },
  { "Tasha Cooper",       "2 Kyles View\nColintraive\nPA22 3AS",                DEMO_IMAGE_DIR "gallery-small-4.jpg" },
  { "Domingo Lynch",      "Red Lion Farm\nWatlington\nOX49 5LG",                DEMO_IMAGE_DIR "gallery-small-5.jpg" },
  { "Dan Haynes",         "239 Whitefield Dr\nLiverpool\nL32 0RD",              DEMO_IMAGE_DIR "gallery-small-6.jpg" },
  { "Leslie Wong",        "1 Tullyvar Rd\nAughnacloy\nBT69 6BQ",                DEMO_IMAGE_DIR "gallery-small-7.jpg" },
  { "Mable Hodges",       "5 Mortimer Rd\nGrazeley\nReading\nRG7 1LA",          DEMO_IMAGE_DIR "gallery-small-8.jpg" },
  { "Kristi Riley",       "10 Jura Dr\nOld Kilpatrick\nGlasgow\nG60 5EH",       DEMO_IMAGE_DIR "gallery-small-17.jpg" },
  { "Terry Clark",        "142 Raeberry St\nGlasgow\nG20 6EA",                  DEMO_IMAGE_DIR "gallery-small-18.jpg" },
  { "Horace Bailey",      "11 Assembly St\nNormanton\nWF6 2DB",                 DEMO_IMAGE_DIR "gallery-small-11.jpg" },
  { "Suzanne Delgado",    "6 Grange Rd\nDownpatrick\nBT30 7DB",                 DEMO_IMAGE_DIR "gallery-small-12.jpg" },
  { "Jamie Bennett",      "117 Potter St\nNorthwood\nHA6 1QF",                  DEMO_IMAGE_DIR "gallery-small-13.jpg" },
  { "Emmett Yates",       "18 Renfrew Way\nBletchley\nMilton Keynes\nMK3 7NY",  DEMO_IMAGE_DIR "gallery-small-14.jpg" },
  { "Glen Vaughn",        "5 Hasman Terrace\nCove Bay\nAberdeen\nAB12 3GD",     DEMO_IMAGE_DIR "gallery-small-15.jpg" },
};
const size_t TABLE_SIZE = sizeof( TABLE ) / sizeof( TABLE[ 0 ] );

} // namespace ContactData
