ifeq ($(DEBUG),1)
APP_OPTIM := debug
else
APP_OPTIM := release
endif
APP_STL := c++_shared
APP_ABI := $(TARGET)
APP_MODULES := libdali
APP_PLATFORM := android-$(API)
APP_BUILD_SCRIPT := ./Android.mk
