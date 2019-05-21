# Dali defines
LOCAL_CFLAGS += -DDALI_DEFAULT_FONT_CACHE_DIR="\"/data/data/com.sec.dali_demo/cache/\""
LOCAL_CFLAGS += -DDALI_USER_FONT_CACHE_DIR="\"/data/data/com.sec.dali_demo/cache/\""
LOCAL_CFLAGS += -DDALI_SHADERBIN_DIR="\"/data/data/com.sec.dali_demo/cache/\""
LOCAL_CFLAGS += -DDALI_DEFAULT_THEME_DIR="\"/data/data/com.sec.dali_demo/cache/\""
LOCAL_CFLAGS += -DFONT_PRELOADED_PATH="\"/data/data/com.sec.dali_demo/cache/\""
LOCAL_CFLAGS += -DFONT_DOWNLOADED_PATH="\"/data/data/com.sec.dali_demo/cache/\""
LOCAL_CFLAGS += -DFONT_APPLICATION_PATH="\"/data/data/com.sec.dali_demo/cache/\""
LOCAL_CFLAGS += -DFONT_CONFIGURATION_FILE="\"/data/data/com.sec.dali_demo/cache/fonts.conf\""
LOCAL_CFLAGS += -DNON_POWER_OF_TWO_TEXTURES

LOCAL_CFLAGS += -DDALI_DATA_READ_ONLY_DIR="\"assets/\""
LOCAL_CFLAGS += -DDALI_IMAGE_DIR="\"assets/toolkit/images/\""
LOCAL_CFLAGS += -DDALI_SOUND_DIR="\"assets/toolkit/sounds/\""
LOCAL_CFLAGS += -DDALI_STYLE_DIR="\"assets/toolkit/styles/\""
LOCAL_CFLAGS += -DDALI_STYLE_IMAGE_DIR="\"assets/toolkit/styles/images/\""
LOCAL_CFLAGS += -DAPPLICATION_RESOURCE_PATH="\"assets/resources\""

LOCAL_CFLAGS += -DDALI_GLES_VERSION=20
LOCAL_CFLAGS += -DNETWORK_LOGGING_ENABLED

# Warnings, optimisation and debug flags
# --------------------------------------
LOCAL_CFLAGS += -w -Wall
LOCAL_CFLAGS += -g
ifeq ($(APP_OPTIM),debug)
#LOCAL_CFLAGS += -DDALI_GL_ERROR_CHECK                             # Extra GL error checking / reporting in Dali renderer
LOCAL_CFLAGS += -DDEBUG_ENABLED                                   # Generic flag to enable additional debug in code (toolkit, adaptor, dali, and also libxml)
LOCAL_CFLAGS += -O0
ifeq ($(BUILD_NATIVE_TRACE), 1)
LOCAL_CFLAGS += -DBUILD_NATIVE_TRACE
$(info Native Android systrace enabled)
endif
else
LOCAL_CFLAGS += -Os
endif

ifeq ($(DALI_LOGGING), 1)
LOCAL_CFLAGS += -DENABLE_DALI_LOGGING
endif

# C++ language features
# ---------------------
LOCAL_CPPFLAGS += -fexceptions
LOCAL_CPPFLAGS += -frtti
