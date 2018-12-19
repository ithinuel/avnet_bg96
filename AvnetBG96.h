/*
 * Copyright (c) 2018, Arm Limited and affiliates.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef STMOD_CELLULAR_H_
#define STMOD_CELLULAR_H_

#include "QUECTEL_BG96.h"

namespace mbed {

class AvnetBG96 : public QUECTEL_BG96 {
public:
    AvnetBG96(FileHandle *fh);
    virtual ~AvnetBG96();

    virtual AT_CellularPower *open_power_impl(ATHandler &at);
};

} // namespace mbed
#endif // STMOD_CELLULAR_H_
