/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

/* This header file includes all multi language strings which need display */
#ifndef __DALI_DEMO_STRINGS_H__
#define __DALI_DEMO_STRINGS_H__

#include <libintl.h>

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#define DALI_DEMO_DOMAIN_LOCAL "dali-demo"

#define DALI_DEMO_STR_EMPTY _("")

#ifdef INTERNATIONALIZATION_ENABLED

#define DALI_DEMO_STR_TITLE_BUBBLES dgettext(DALI_DEMO_DOMAIN_LOCAL, "DALI_DEMO_STR_TITLE_BUBBLES")
#define DALI_DEMO_STR_TITLE_BLOCKS dgettext(DALI_DEMO_DOMAIN_LOCAL, "DALI_DEMO_STR_TITLE_BLOCKS")
#define DALI_DEMO_STR_TITLE_CLUSTER dgettext(DALI_DEMO_DOMAIN_LOCAL, "DALI_DEMO_STR_TITLE_CLUSTER")
#define DALI_DEMO_STR_TITLE_CUBE_TRANSITION dgettext(DALI_DEMO_DOMAIN_LOCAL, "DALI_DEMO_STR_TITLE_CUBE_TRANSITION")
#define DALI_DEMO_STR_TITLE_DISSOLVE_TRANSITION dgettext(DALI_DEMO_DOMAIN_LOCAL, "DALI_DEMO_STR_TITLE_DISSOLVE_TRANSITION")
#define DALI_DEMO_STR_TITLE_ITEM_VIEW dgettext(DALI_DEMO_DOMAIN_LOCAL, "DALI_DEMO_STR_TITLE_ITEM_VIEW")
#define DALI_DEMO_STR_TITLE_MAGNIFIER dgettext(DALI_DEMO_DOMAIN_LOCAL, "DALI_DEMO_STR_TITLE_MAGNIFIER")
#define DALI_DEMO_STR_TITLE_MOTION_BLUR dgettext(DALI_DEMO_DOMAIN_LOCAL, "DALI_DEMO_STR_TITLE_MOTION_BLUR")
#define DALI_DEMO_STR_TITLE_MOTION_STRETCH dgettext(DALI_DEMO_DOMAIN_LOCAL, "DALI_DEMO_STR_TITLE_MOTION_STRETCH")
#define DALI_DEMO_STR_TITLE_PAGE_TURN_VIEW dgettext(DALI_DEMO_DOMAIN_LOCAL, "DALI_DEMO_STR_TITLE_PAGE_TURN_VIEW")
#define DALI_DEMO_STR_TITLE_RADIAL_MENU dgettext(DALI_DEMO_DOMAIN_LOCAL, "DALI_DEMO_STR_TITLE_RADIAL_MENU")
#define DALI_DEMO_STR_TITLE_REFRACTION dgettext(DALI_DEMO_DOMAIN_LOCAL, "DALI_DEMO_STR_TITLE_REFRACTION")
#define DALI_DEMO_STR_TITLE_SCROLL_VIEW dgettext(DALI_DEMO_DOMAIN_LOCAL, "DALI_DEMO_STR_TITLE_SCROLL_VIEW")
#define DALI_DEMO_STR_TITLE_LIGHTS_AND_SHADOWS dgettext(DALI_DEMO_DOMAIN_LOCAL, "DALI_DEMO_STR_TITLE_LIGHTS_AND_SHADOWS")
#define DALI_DEMO_STR_TITLE_SCRIPT_BASED_UI dgettext(DALI_DEMO_DOMAIN_LOCAL, "DALI_DEMO_STR_TITLE_SCRIPT_BASED_UI")
#define DALI_DEMO_STR_TITLE_IMAGE_FITTING_SAMPLING dgettext(DALI_DEMO_DOMAIN_LOCAL, "DALI_DEMO_STR_TITLE_IMAGE_FITTING_SAMPLING")
#define DALI_DEMO_STR_TITLE_IMAGE_SCALING dgettext(DALI_DEMO_DOMAIN_LOCAL, "DALI_DEMO_STR_TITLE_IMAGE_SCALING")
#define DALI_DEMO_STR_TITLE_TEXT_LABEL dgettext(DALI_DEMO_DOMAIN_LOCAL, "DALI_DEMO_STR_TITLE_TEXT_LABEL")
#define DALI_DEMO_STR_TITLE_TEXT_LABEL_MULTI_LANGUAGE dgettext(DALI_DEMO_DOMAIN_LOCAL, "DALI_DEMO_STR_TITLE_TEXT_LABEL_MULTI_LANGUAGE")
#define DALI_DEMO_STR_TITLE_EMOJI_TEXT dgettext(DALI_DEMO_DOMAIN_LOCAL, "DALI_DEMO_STR_TITLE_EMOJI_TEXT")
#define DALI_DEMO_STR_TITLE_ANIMATED_SHAPES dgettext(DALI_DEMO_STR_TITLE_ANIMATED_SHAPES)
#define DALI_DEMO_STR_TITLE_PATH_ANIMATION dgettext(DALI_DEMO_STR_TITLE_PATH_ANIMATION)
#define DALI_DEMO_STR_TITLE_NEGOTIATE_SIZE dgettext(DALI_DEMO_STR_TITLE_NEGOTIATE_SIZE)
#define DALI_DEMO_STR_TITLE_BUTTONS dgettext(DALI_DEMO_DOMAIN_LOCAL, "DALI_DEMO_STR_TITLE_BUTTONS")
#define DALI_DEMO_STR_TITLE_LOGGING dgettext(DALI_DEMO_DOMAIN_LOCAL, "DALI_DEMO_STR_TITLE_LOGGING")

#else // !INTERNATIONALIZATION_ENABLED

#define DALI_DEMO_STR_TITLE_BUBBLES                   "Bubbles"
#define DALI_DEMO_STR_TITLE_BLOCKS                    "Blocks"
#define DALI_DEMO_STR_TITLE_CLUSTER                   "Cluster"
#define DALI_DEMO_STR_TITLE_CUBE_TRANSITION           "Cube Effect"
#define DALI_DEMO_STR_TITLE_DISSOLVE_TRANSITION       "Dissolve Effect"
#define DALI_DEMO_STR_TITLE_ITEM_VIEW                 "Item View"
#define DALI_DEMO_STR_TITLE_MAGNIFIER                 "Magnifier"
#define DALI_DEMO_STR_TITLE_MOTION_BLUR               "Motion Blur"
#define DALI_DEMO_STR_TITLE_MOTION_STRETCH            "Motion Stretch"
#define DALI_DEMO_STR_TITLE_PAGE_TURN_VIEW            "Page Turn View"
#define DALI_DEMO_STR_TITLE_RADIAL_MENU               "Radial Menu"
#define DALI_DEMO_STR_TITLE_REFRACTION                "Refract Effect"
#define DALI_DEMO_STR_TITLE_SCROLL_VIEW               "Scroll View"
#define DALI_DEMO_STR_TITLE_LIGHTS_AND_SHADOWS        "Lights and shadows"
#define DALI_DEMO_STR_TITLE_SCRIPT_BASED_UI           "Script Based UI"
#define DALI_DEMO_STR_TITLE_IMAGE_FITTING_SAMPLING    "Image Fitting and Sampling"
#define DALI_DEMO_STR_TITLE_IMAGE_SCALING             "Image Scaling Grid"
#define DALI_DEMO_STR_TITLE_TEXT_LABEL                "Text Label"
#define DALI_DEMO_STR_TITLE_TEXT_LABEL_MULTI_LANGUAGE "Text Scripts"
#define DALI_DEMO_STR_TITLE_EMOJI_TEXT                "Emoji Text"
#define DALI_DEMO_STR_TITLE_ANIMATED_SHAPES           "Animated Shapes"
#define DALI_DEMO_STR_TITLE_PATH_ANIMATION            "Animated Path"
#define DALI_DEMO_STR_TITLE_NEGOTIATE_SIZE            "Negotiate Size"
#define DALI_DEMO_STR_TITLE_BUTTONS                   "Buttons"
#define DALI_DEMO_STR_TITLE_LOGGING                   "Logging"

#endif

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __DALI_DEMO_STRINGS_H__
