MODULE_NAME = dali
SRC_PATH := ../../../..
NDK_PROJECT_PATH = null
NDK_OUT = ./obj
NDK_LIBS_OUT = ./lib
NDK_APPLICATION_MK = ./Application.mk
include $(ANDROID_NDK)/build/core/build-local.mk

.PHONY: headers
headers:
	mkdir -p $(SRC_PATH)/dali-env/android/include/dali/public-api
	mkdir -p $(SRC_PATH)/dali-env/android/include/dali/devel-api
	mkdir -p $(SRC_PATH)/dali-env/android/include/dali/integration-api/adaptors
	mkdir -p $(SRC_PATH)/dali-env/android/include/dali-toolkit/public-api
	mkdir -p $(SRC_PATH)/dali-env/android/include/dali-toolkit/devel-api
	(cd $(SRC_PATH)/dali-core/dali/public-api ; find . -name "*.h" | xargs cp --parents -t  ../../../dali-env/android/include/dali/public-api )
	(cd $(SRC_PATH)/dali-core/dali/devel-api ; find . -name "*.h" | xargs cp --parents -t  ../../../dali-env/android/include/dali/devel-api )
	(cd $(SRC_PATH)/dali-core/dali/integration-api ; find . -name "*.h" | xargs cp --parents -t ../../../dali-env/android/include/dali/integration-api )
	cp  $(SRC_PATH)/dali-adaptor/dali/dali.h $(SRC_PATH)/dali-env/android/include/dali/
	(cd $(SRC_PATH)/dali-adaptor/dali/public-api ; find . -name "*.h" | xargs cp --parents -t ../../../dali-env/android/include/dali/public-api )
	(cd $(SRC_PATH)/dali-adaptor/dali/devel-api ; find . -name "*.h" | xargs cp --parents -t ../../../dali-env/android/include/dali/devel-api )
	(cd $(SRC_PATH)/dali-adaptor/dali/integration-api ; find . -name "*.h" | xargs cp --parents -t ../../../dali-env/android/include/dali/integration-api/adaptors )
	cp  $(SRC_PATH)/dali-toolkit/dali-toolkit/dali-toolkit.h $(SRC_PATH)/dali-env/android/include/dali-toolkit/
	(cd $(SRC_PATH)/dali-toolkit/dali-toolkit/public-api ; find . -name "*.h" | xargs cp --parents -t ../../../dali-env/android/include/dali-toolkit/public-api )
	(cd $(SRC_PATH)/dali-toolkit/dali-toolkit/devel-api ; find . -name "*.h" | xargs cp --parents -t ../../../dali-env/android/include/dali-toolkit/devel-api )


