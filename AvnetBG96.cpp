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

#include "AT_CellularPower.h"
#include "ATHandler.h"
#include "AvnetBG96.h"
#include "mbed_wait_api.h"
#include "gpio_api.h"
#include "mbed_trace.h"
#include "DigitalOut.h"
#include "QUECTEL_BG96_CellularPower.h"

#define TRACE_GROUP "CELL"

using namespace mbed;

namespace mbed {
class AvnetBG96_CellularPower: public QUECTEL_BG96_CellularPower {
    public:
        AvnetBG96_CellularPower(ATHandler &atHandler): QUECTEL_BG96_CellularPower(atHandler) {}
        ~AvnetBG96_CellularPower() {}

        nsapi_error_t on() {
            nsapi_error_t err = QUECTEL_BG96_CellularPower::on();
            if ((err == NSAPI_ERROR_OK) || (err == NSAPI_ERROR_UNSUPPORTED)) {
                printf("Powerkeying\n");
                DigitalOut powerkey(MBED_CONF_AVNET_BG96_POWER);
                powerkey.write(1);
                wait_ms(250);
                powerkey.write(0);

                // wait for RDY
                _at.lock();
                _at.set_at_timeout(5000);
                _at.set_stop_tag("RDY");
                bool rdy = _at.consume_to_stop_tag();
                _at.restore_at_timeout();
                _at.set_stop_tag(NULL);

                tr_debug("Modem %sready to receive AT commands", rdy?"":"NOT ");
                _at.unlock();

                if (!rdy) {
                    err = NSAPI_ERROR_DEVICE_ERROR;
                }
            }
            return err;
        }

        nsapi_error_t off() {
            _at.lock();
            _at.set_stop_tag("POWERED DOWN");
            
            _at.set_at_timeout(5000);
            // _at.set_at_timeout(65000);
            
            _at.cmd_start("AT+QPOWD");
            _at.cmd_stop();

            bool powered_down = _at.consume_to_stop_tag();
            _at.restore_at_timeout();
            _at.set_stop_tag(NULL);
            if (powered_down) {
                wait_ms(1000);
            }
            _at.unlock();
            // we can now cut vbat.
            return QUECTEL_BG96_CellularPower::off();
        }
};
};

AvnetBG96::AvnetBG96(FileHandle *fh) : QUECTEL_BG96(fh)
{
    DigitalOut reset(MBED_CONF_AVNET_BG96_RESET);
    // start with modem disabled
    reset.write(0);
    wait_ms(250);
    reset.write(1);
}

AvnetBG96::~AvnetBG96()
{
}

AT_CellularPower *AvnetBG96::open_power_impl(ATHandler &at)
{
    return new AvnetBG96_CellularPower(at);
}
