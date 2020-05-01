LOCAL_PATH := $(call my-dir)

ifneq ($(filter RMX1921,$(TARGET_DEVICE)),)
include $(call all-makefiles-under,$(LOCAL_PATH))

include $(CLEAR_VARS)
endif

