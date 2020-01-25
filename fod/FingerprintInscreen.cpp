/*
 * Copyright (C) 2019 The LineageOS Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "FingerprintInscreenService"

#include "FingerprintInscreen.h"
#include <hidl/HidlTransportSupport.h>
#include <android-base/logging.h>
#include <fstream>
#include <cmath>
#include <thread>

/* Define FOD size and location */
#define FOD_SIZE 196
#define FOD_POS_X 445
#define FOD_POS_Y 1967

/* Define Dimlayer paths */
#define DIM_HBM "/sys/kernel/oppo_display/dimlayer_hbm"

/* Define FOD_PRESS */
#define FOD_PRESS "/sys/kernel/oppo_display/notify_fppress"

/* Define High brightness mode */
#define HIGH_BRIGHTNESS "/sys/kernel/oppo_display/hbm"

namespace {

template <typename T>
static void set(const std::string& path, const T& value) {
    std::ofstream file(path);
    file << value;
    LOG(INFO) << "wrote path: " << path << ", value: " << value << "\n";
}

template <typename T>
static T get(const std::string& path, const T& def) {
    std::ifstream file(path);
    T result;

    file >> result;
    return file.fail() ? def : result;
}

} // anonymous namespace

namespace vendor {
namespace lineage {
namespace biometrics {
namespace fingerprint {
namespace inscreen {
namespace V1_1 {
namespace implementation {

FingerprintInscreen::FingerprintInscreen():mFingerPressed{false} {
}

Return<int32_t> FingerprintInscreen::getPositionX() {
    return FOD_POS_X;
}

Return<int32_t> FingerprintInscreen::getPositionY() {
    return FOD_POS_Y;
}

Return<int32_t> FingerprintInscreen::getSize() {
    return FOD_SIZE;
}

Return<void> FingerprintInscreen::onStartEnroll() {
    LOG(INFO) << __func__ << " start";
    set(HIGH_BRIGHTNESS, 1);
    set(DIM_HBM, 1);
    return Void();
}

Return<void> FingerprintInscreen::onFinishEnroll() {
    LOG(INFO) << __func__ << " start";
    set(HIGH_BRIGHTNESS, 0);
    set(DIM_HBM, 0);
    return Void();
}

Return<void> FingerprintInscreen::switchHbm(bool enabled) {
    if (enabled) {
        set(HIGH_BRIGHTNESS, 1);
    } else {
        set(HIGH_BRIGHTNESS, 0);
    }
    return Void();
}

Return<void> FingerprintInscreen::onPress() {
    LOG(INFO) << __func__ << " start";
    return Void();
}

Return<void> FingerprintInscreen::onRelease() {
    LOG(INFO) << __func__ << " start";
    mFingerPressed = false;
    set(FOD_PRESS, 0);
    set(DIM_HBM, 0);
    LOG(INFO) << __func__ << " exit";
    return Void();
}

Return<void> FingerprintInscreen::onShowFODView() {
    mFingerPressed = true;
    LOG(INFO) << __func__ << " start";
    set(DIM_HBM, 1);
    std::thread([this]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(23));
        if (mFingerPressed) {
            LOG(INFO) << "SETTING FP PRESS PATH";
            set(FOD_PRESS, 1);
        }
    }).detach();
    LOG(INFO) << __func__ <<" exit";
    return Void();
}

Return<void> FingerprintInscreen::onHideFODView() {
    LOG(INFO) << __func__ << "start";
    set(DIM_HBM, 0);
    set(FOD_PRESS, 0);
    LOG(INFO) << __func__ << "exit";
    return Void();
}

Return<bool> FingerprintInscreen::handleAcquired(int32_t acquiredInfo, int32_t vendorCode) {
    LOG(ERROR) << "acquiredInfo: " << acquiredInfo << ", vendorCode: " << vendorCode << "\n";
    return false;
}

Return<bool> FingerprintInscreen::handleError(int32_t error, int32_t vendorCode) {
    LOG(ERROR) << "error: " << error << ", vendorCode: " << vendorCode << "\n";
    return false;
}

Return<void> FingerprintInscreen::setLongPressEnabled(bool) {
    return Void();
}

Return<int32_t> FingerprintInscreen::getDimAmount(int32_t brightness) {
    LOG(INFO) << __func__ << " start";
    return(int32_t)((brightness > 498) ? (255 * (1.0 - pow(brightness / 2047.0 * 430.0 / 600.0, 0.455))):
            (255 * (1.0 - pow(brightness / 1605.0, 0.455)))); 
}

Return<bool> FingerprintInscreen::shouldBoostBrightness() {
    return false;
}

Return<void> FingerprintInscreen::setCallback(const sp<::vendor::lineage::biometrics::fingerprint::inscreen::V1_0::IFingerprintInscreenCallback>& callback) {
    {
        std::lock_guard<std::mutex> _lock(mCallbackLock);
        mCallback = callback;
    }
    return Void();
}

}  // namespace implementation
}  // namespace V1_1
}  // namespace inscreen
}  // namespace fingerprint
}  // namespace biometrics
}  // namespace lineage
}  // namespace vendor
