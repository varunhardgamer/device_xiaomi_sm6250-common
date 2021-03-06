/*
 * Copyright (C) 2020 The LineageOS Project
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

#define LOG_TAG "android.hardware.power@1.3-service.xiaomi_sm6250"

#include <linux/input.h>

#include "Power.h"

constexpr int kWakeupModeOff = 4;
constexpr int kWakeupModeOn = 5;

namespace android {
namespace hardware {
namespace power {
namespace V1_3 {
namespace implementation {

// Methods from V1_0::IPower follow.
Return<void> Power::setInteractive(bool) {
    return Void();
}

Return<void> Power::powerHint(PowerHint_1_0, int32_t) {
    return Void();
}

int open_ts_input() {
    int fd = -1;
    DIR *dir = opendir("/dev/input");

    if (dir != NULL) {
        struct dirent *ent;

        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_type == DT_CHR) {
                char absolute_path[PATH_MAX] = {0};
                char name[80] = {0};

                strcpy(absolute_path, "/dev/input/");
                strcat(absolute_path, ent->d_name);

                fd = open(absolute_path, O_RDWR);
                if (ioctl(fd, EVIOCGNAME(sizeof(name) - 1), &name) > 0) {
                    if (strcmp(name, "fts_ts") == 0 ||
                            strcmp(name, "NVTCapacitiveTouchScreen") == 0)
                        break;
                }

                close(fd);
                fd = -1;
            }
        }

        closedir(dir);
    }

    return fd;
}

Return<void> Power::setFeature(Feature feature, bool activate) {
    switch (feature) {
        case Feature::POWER_FEATURE_DOUBLE_TAP_TO_WAKE: {
	int fd = open_ts_input();
            if (fd == -1) {
                ALOGW("DT2W won't work because no supported touchscreen input devices were found");
                return Void();
            }
            struct input_event ev;
            ev.type = EV_SYN;
            ev.code = SYN_CONFIG;
            ev.value = activate ? kWakeupModeOn : kWakeupModeOff;
            write(fd, &ev, sizeof(ev));
            close(fd);
            } break;
        default:
            break;
    }
    return Void();
}

Return<void> Power::getPlatformLowPowerStats(getPlatformLowPowerStats_cb _hidl_cb) {
    _hidl_cb({}, Status::SUCCESS);
    return Void();
}

// Methods from V1_1::IPower follow.
Return<void> Power::getSubsystemLowPowerStats(getSubsystemLowPowerStats_cb _hidl_cb) {
    _hidl_cb({}, Status::SUCCESS);
    return Void();
}

Return<void> Power::powerHintAsync(PowerHint_1_0, int32_t) {
    return Void();
}

// Methods from V1_2::IPower follow.
Return<void> Power::powerHintAsync_1_2(PowerHint_1_2, int32_t) {
    return Void();
}

// Methods from V1_3::IPower follow.
Return<void> Power::powerHintAsync_1_3(PowerHint_1_3, int32_t) {
    return Void();
}

}  // namespace implementation
}  // namespace V1_3
}  // namespace power
}  // namespace hardware
}  // namespace android
