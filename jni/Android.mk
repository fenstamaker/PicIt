LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := PicItNativeSobel
LOCAL_CPPFLAGS += -fexceptions -O3
LOCAL_LDLIBS := -llog
### Add all source file names to be included in lib separated by a whitespace
LOCAL_SRC_FILES := PicItNativeSobel.cpp ImageClassifier.h ImageClassifier.cpp SobelEdgeDetection.h SobelEdgeDetection.cpp

include $(BUILD_SHARED_LIBRARY)
