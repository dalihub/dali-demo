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

/**
 * @file text-label-example.cpp
 * @brief Basic usage of TextLabel control
 */

// INTERNAL INCLUDES
#include "vertical-layout.h"

// EXTERNAL INCLUDES
#include <dali-toolkit/dali-toolkit.h>
#include <dali/public-api/text-abstraction/text-abstraction.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
  struct Language
  {
    std::string languageName;
    std::string languageRomanName;
    std::string text;
  };

  const Language LANGUAGES[] = {
    {
      "العَرَبِيةُ",
      "(Arabic)",
      "لإعادة ترتيب الشاشات، يجب تغيير نوع العرض إلى شبكة قابلة للتخصيص."
    },
    {
      "অসমীয়া লিপি",
      "(Assamese)",
      "পৃষ্ঠাসমূহ পুনঃব্যৱস্থিত কৰিবলৈ, আপুনি দৰ্শনৰ প্ৰকাৰ এটা অনুকূলনযোগ্য গ্ৰীডলৈ পৰিৱৰ্তন কৰাটো আৱশ্যক৷"
    },
    {
      "বাংলা",
      "(Bengali)",
      "তথ্য লোড করতে অক্ষম৷ পুনরায় চেষ্টা করতে ট্যাপ করুন৷"
    },
    {
      "English",
      "(English)",
      "A Quick Brown Fox Jumps Over The Lazy Dog"
    },
    {
      "Español",
      "(Spanish)",
      "No se puede añadir más aplicaciones. Se alcanzó la cantidad máxima."
    },
    {
      "فارسی",
      "(Persian)",
      "برای مرتب کردن مجدد صفحه‌ها، باید نوع نمایش را به یک نمای شبکه‌ای قابل تنظیم تغییر دهید."
    },
    {
      "Français",
      "(French)",
      "La nouvelle page sera composée des Dynamic Box S Planner, Tâches et S Memo"
    },
    {
      "ગુજરાતી લિપિ",
      "(Gujarati)",
      "પૃષ્ઠોને ફરીથી ગોઠવવા માટે, તમારે દૃશ્ય પ્રકારને કસ્ટમાઇઝેશન યોગ્ય ગ્રિડ પર બદલવાની જરૂર છે."
    },
    {
      "हिन्दी",
      "(Hindi)",
      "पेज पुनः व्यवस्थित करने के लिए, आपको दृश्य प्रकार को अनुकूलित करने योग्य ग्रिड में बदलना होगा।"
    },
    {
      "Bahasa Indonesia",
      "(Indonesian)",
      "Tidak dapat menambahkan aplikasi lagi. Jumlah maksimum aplikasi tercapai."
    },
    {
      "ខេមរភាសា",
      "(Cambodian)",
      "ដើម្បី​រៀបចំ​ទំព័រ​ឡើងវិញ អ្នក​ត្រូវ​ប្ដូរ​ប្រភេទ​បង្ហាញ​ទៅ​ក្រឡាចត្រង្គ​ដែល​អាច​ប្ដូរ​តាមបំណង។"
    },
    {
      "ಕನ್ನಡ ",
      "(Kannada)",
      "ಪುಟಗಳನ್ನು ಮರುವ್ಯವಸ್ಥಿತವಾಗಿ ಇರಿಸಲು, ನೀವು ವೀಕ್ಷಣೆ ವಿಧವನ್ನು ಗ್ರಾಹಕೀಕರಿಸಬಲ್ಲ ಗ್ರಿಡ್‌ಗೆ ಬದಲಾಯಿಸುವುದು ಅಗತ್ಯವಿದೆ."
    },
    {
      "한국어",
      "(Korean)",
      "페이지 순서를 변경하려면 보기 방식을 격자 보기(직접 설정)로 변경하세요."
    },
    {
      "ພາສາລາວ",
      "(Lao)",
      "ເພື່ອຈັດລຽງໜ້າຄືນ, ທ່ານຈໍາເປັນຕ້ອງປ່ຽນຊະນິດຂອງມຸມມອງໄປຫາຕາຕະລາງທີ່ກຳນົດເອງໄດ້."
    },
    {
      "മലയാളം",
      "(Malayalam)",
      "പേജുകൾ പുനഃക്രമീകരിക്കുന്നതിന്, ഇഷ്ടാനുസൃതമാക്കാവുന്ന ഗ്രിഡിലേക്ക് കാഴ്ചയുടെ തരം നിങ്ങൾ മാറ്റേണ്ടതുണ്ട്."
    },
    {
      "मराठी",
      "(Marathi)",
      "पृष्‍ठांची पुनर्रचना करा, आपण सानुकूलित करण्‍यायोग्‍य ग्रिडमध्‍ये व्‍ह्यू प्रकार बदलणे गरजेचे आहे."
    },
    {
      "Bahasa Melayu",
      "(Malay)",
      "Tidak boleh menambah aplikasi lagi. Bilangan maksimum aplikasi dicapai."
    },
    {
      "မြန်မာဘာသာ",
      "(Burmese)",
      "စာမ်က္ႏွာမ်ား ျပန္စီစဥ္ရန္ အျမင္ပံုစံကို စိတ္ၾကိဳက္လုပ္ႏိုင္ေသာ ဂရစ္ တစ္ခုသို႔ ေျပာင္းဖို႔လိုသည္။"
    },
    {
      "नेपाली",
      "(Nepali)",
      "थप अनुप्रयोगहरू थप्न सकिएन। अनुप्रयोगहरूको अधिकतम संख्या पुग्यो।"
    },
    {
      "ଓଡ଼ିଆ",
      "(Oriya)",
      "ପରବର୍ତ୍ତୀ ପୃଷ୍ଠା ଦେଖିବା ପାଇଁ ଦୁଇ ଆଙ୍ଗୁଠିରେ ସ୍ୱାଇପ୍ କରନ୍ତୁ।"
    },
    {
      "ਗੁਰਮੁਖੀ",
      "(Punjabi)",
      "ਇਹ ਫੋਲਡਰ ਅਤੇ ਇਸ ਵਿੱਚ ਸ਼ਾਮਲ ਸਾਰੇ ਆਈਟਮਾਂ ਨੂੰ ਮਿਟਾ ਦੇਵੇਗਾ।"
    },
    {
      "Português",
      "(Portuguese)",
      "Para reorganizar páginas, é necessário alterar o tipo de exibição para uma grade personalizável."
    },
    {
      "සිංහල",
      "(Sinhala)",
      "පිටු නැවත පෙළ ගැස්වීම සඳහා ඔබ විසින් දසුන් ප්‍රවර්ගය අභිමතීකෘත්‍ය ජාලයකට වෙනස් කළ යුතුය."
    },
    {
      "தமிழ்",
      "(Tamil)",
      "பக்கங்களை மறுஒழுங்குபடுத்தவென காட்சி வகையை தனிப்பயனாக்கத்தக்க கட்டமைப்பிற்கு மாற்ற வேண்டியுள்ளது."
    },
    {
      "తెలుగు",
      "(Telugu)",
      "మరిన్ని అప్లికేషన్‌లను జోడించడం సాధ్యం కాలేదు. గరిష్ట అప్లికేషన్‌ల సంఖ్యను చేరుకున్నాయి."
    },
    {
      "ภาษาไทย",
      "(Thai)",
      "คุณต้องเปลี่ยนชนิดการแสดง เป็นตารางที่กำหนดเองได้ เพื่อจัดหน้าใหม่"
    },
    {
      "Tagalog",
      "(Tagalog)",
      "Hindi makapagdagdag ng extra na linya dahil sa paghihigpit sa espasyo"
    },
    {
      "Türkçe",
      "(Turkish)",
      "Sayfaları yeniden düzenlemek için özelleştirilebilir kılavuzun görünüm tipini değiştirmeniz gerekir."
    },
    {
      "اُردُو‎",
      "(Urdu)",
      "صفحات کو دوبارہ ترتیب دینے کے لئے، آپ کو منظر کی قسم کو ایک حسب ضرورت گرڈ میں تبدیل کرنا ہوگا۔"
    },
    {
      "tiếng Việt",
      "(Vietnamese)",
      "Để sắp xếp lại các trang, bạn cần thay đổi kiểu xem sang dạng lưới tùy chỉnh."
    },
    {
      "漢語",
      "(Chinese)",
      "若要重新排列页面，您需要将视图类型更改为可自定义网格。"
    },
  };

  const unsigned int NUMBER_OF_LANGUAGES = 31u;
} // namespace

/**
 * @brief The main class of the demo.
 */
class TextLabelMultiLanguageExample : public ConnectionTracker
{
public:

  TextLabelMultiLanguageExample( Application& application )
  : mApplication( application ),
    mLastPoint( 0.f )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &TextLabelMultiLanguageExample::Create );
  }

  ~TextLabelMultiLanguageExample()
  {
    // Nothing to do here.
  }

  /**
   * One-time setup in response to Application InitSignal.
   */
  void Create( Application& application )
  {
    Stage stage = Stage::GetCurrent();

    TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
    fontClient.SetDpi( 96, 96 );

    mLayout = VerticalLayout::New();
    mLayout.SetParentOrigin( ParentOrigin::CENTER );

    stage.Add( mLayout );

    for( unsigned int index = 0u; index < NUMBER_OF_LANGUAGES; ++index )
    {
      const Language& language = LANGUAGES[index];

      TextLabel label = TextLabel::New();
      label.SetParentOrigin( ParentOrigin::CENTER );

      label.SetProperty( TextLabel::PROPERTY_MULTI_LINE, true );

      const std::string text = language.languageName + " " + language.languageRomanName + " " + language.text;

      label.SetProperty( TextLabel::PROPERTY_TEXT, text );
      mLayout.AddLabel( label );

      mLayout.TouchedSignal().Connect( this, &TextLabelMultiLanguageExample::OnTouchEvent );
    }

    const Vector2& size = Stage::GetCurrent().GetSize();
    const float height = mLayout.GetHeightForWidth( size.width );
    mLayout.SetSize( Size( size.width, height ) );
  }

  bool OnTouchEvent( Actor actor, const TouchEvent& event )
  {
    if( 1u == event.GetPointCount() )
    {
      const TouchPoint::State state = event.GetPoint(0u).state;
      if( TouchPoint::Down == state )
      {
        mLastPoint = event.GetPoint( 0 ).local.y;
        mAnimation = Animation::New( 0.25f );
        mAnimation.FinishedSignal().Connect( this, &TextLabelMultiLanguageExample::AnimationCompleted );
      }
      else if( TouchPoint::Motion == state )
      {
        if( mAnimation )
        {
          const float localPoint = event.GetPoint( 0 ).local.y;
          mAnimation.MoveBy( mLayout, Vector3( 0.f, localPoint - mLastPoint, 0.f ), AlphaFunctions::Linear );
          mAnimation.Play();
          mLastPoint = localPoint;
        }
      }
    }

    return true;
  }

  void AnimationCompleted(Animation& source)
  {
    //mAnimation.Reset();
  }

private:

  Application&   mApplication;
  VerticalLayout mLayout;
  Animation      mAnimation;
  float          mLastPoint;
};

void RunTest( Application& application )
{
  TextLabelMultiLanguageExample test( application );

  application.MainLoop();
}

/** Entry point for Linux & Tizen applications */
int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  RunTest( application );

  return 0;
}
