#include "climate_ir_electra.h"
#include "esphome/core/log.h"

namespace esphome::climate_ir_electra
{
    // ESPHOME_LOG_TAG(TAG, "climate.climate_ir_electra");
    static const char *const TAG = "climate.climate_ir_electra";
    void ElectraIrClimate::transmit_state()
    {
        const bool climate_is_off = (this->mode == climate::CLIMATE_MODE_OFF);

        if (climate_is_off)
        {
            this->electraRemoteProtocol.set_power(ElectraRemoteProtocol::POWER::POWER_OFF);
        }
        else
        {
            this->electraRemoteProtocol.set_power(ElectraRemoteProtocol::POWER::POWER_ON);
        }

        ESP_LOGD(TAG, "climate_lg_ir mode code: 0x%02X", this->mode);

        auto signal = this->electraRemoteProtocol.get_signal();
        auto bytes_count = signal.size();

        auto transmit = this->transmitter_->transmit();
        auto *data = transmit.get_data();

        data->set_carrier_frequency(38000);
        data->reserve(2 + (bytes_count * 8u + 2u));

        data->item(this->header_high_, this->header_low_);

        for (size_t i = 0; i < signal.size(); i++)
        {
            for (int j = 7; j >= 0; j--)
            {
                bool cur_bit = (signal[i] >> j) & 1;
                if (cur_bit)
                {
                    data->item(this->bit_high_, this->bit_one_low_);
                }
                else
                {
                    data->item(this->bit_high_, this->bit_zero_low_);
                }
            }
        }

        // might be wrong 3.0 bs 690
        data->mark(this->bit_zero_low_);
        transmit.perform();


        this->publish_state();
    }
}