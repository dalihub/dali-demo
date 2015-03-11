#ifndef __DALI_DEMO_EMOJI_STRINGS_H__
#define __DALI_DEMO_EMOJI_STRINGS_H__

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

// EXTERNAL INCLUDES
#include <string>

namespace EmojiStrings
{
  struct Emoji
  {
    uint32_t mUnicode;
    std::string mUTF8;
    std::string mDescription;
  };

  const Emoji EMOJIS[] = {

    { 0x1F601,"\xF0\x9F\x98\x81","grinning face with smiling eyes" },
    { 0x1F602,"\xF0\x9F\x98\x82","face with tears of joy" },
    //{ 0x1F603,"\xF0\x9F\x98\x83","smiling face with open mouth" },
    //{ 0x1F604,"\xF0\x9F\x98\x84","smiling face with open mouth and smiling eyes" },
    { 0x1F605,"\xF0\x9F\x98\x85","smiling face with open mouth and cold sweat" },
    //{ 0x1F606,"\xF0\x9F\x98\x86","smiling face with open mouth and tightly-closed eyes" },
    { 0x1F609,"\xF0\x9F\x98\x89","winking face" },
    { 0x1F60A,"\xF0\x9F\x98\x8A","smiling face with smiling eyes" },
    { 0x1F60B,"\xF0\x9F\x98\x8B","face savouring delicious food" },
    { 0x1F60C,"\xF0\x9F\x98\x8C","relieved face" },
    { 0x1F60D,"\xF0\x9F\x98\x8D","smiling face with heart-shaped eyes" },
    { 0x1F60F,"\xF0\x9F\x98\x8F","smirking face" },
    //{ 0x1F612,"\xF0\x9F\x98\x92","unamused face" },
    //{ 0x1F613,"\xF0\x9F\x98\x93","face with cold sweat" },
    //{ 0x1F614,"\xF0\x9F\x98\x94","pensive face" },
    //{ 0x1F616,"\xF0\x9F\x98\x96","confounded face" },
    { 0x1F618,"\xF0\x9F\x98\x98","face throwing a kiss" },
    { 0x1F61A,"\xF0\x9F\x98\x9A","kissing face with closed eyes" },
    //{ 0x1F61C,"\xF0\x9F\x98\x9C","face with stuck-out tongue and winking eye" },
    //{ 0x1F61D,"\xF0\x9F\x98\x9D","face with stuck-out tongue and tightly-closed eyes" },
    //{ 0x1F61E,"\xF0\x9F\x98\x9E","disappointed face" },
    { 0x1F620,"\xF0\x9F\x98\xA0","angry face" },

    /*
    { "POUTING FACE",
    { "CRYING FACE",
    { "PERSEVERING FACE",
    { "FACE WITH LOOK OF TRIUMPH",
    { "DISAPPOINTED BUT RELIEVED FACE",
    { "FEARFUL FACE",
    { "WEARY FACE",
    { "SLEEPY FACE",
    { "TIRED FACE",
    { "LOUDLY CRYING FACE",
    { "FACE WITH OPEN MOUTH AND COLD SWEAT",
    { "FACE SCREAMING IN FEAR",
    { "ASTONISHED FACE",
    { "FLUSHED FACE",
    { "DIZZY FACE",
    { "FACE WITH MEDICAL MASK",
    { "GRINNING CAT FACE WITH SMILING EYES",
    { "CAT FACE WITH TEARS OF JOY",
    { "SMILING CAT FACE WITH OPEN MOUTH",
    { "SMILING CAT FACE WITH HEART-SHAPED EYES",
    { "CAT FACE WITH WRY SMILE",
    { "KISSING CAT FACE WITH CLOSED EYES",
    { "POUTING CAT FACE",
    { "CRYING CAT FACE",
    { "WEARY CAT FACE",
    { "FACE WITH NO GOOD GESTURE",
    { "FACE WITH OK GESTURE",
    { "PERSON BOWING DEEPLY",
    { "SEE-NO-EVIL MONKEY",
    { "HEAR-NO-EVIL MONKEY",
    { "SPEAK-NO-EVIL MONKEY",
    { "HAPPY PERSON RAISING ONE HAND",
    { "PERSON RAISING BOTH HANDS IN CELEBRATION",
    { "PERSON FROWNING",
    { "PERSON WITH POUTING FACE",
    { "PERSON WITH FOLDED HANDS",
  */

  };

  const unsigned int NUMBER_OF_EMOJIS = 12;

} // EmojiStrings

#endif // __DALI_DEMO_EMOJI_STRINGS_H__
