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

            switch (this->mode)
            {
            case climate::CLIMATE_MODE_COOL:
                this->electraRemoteProtocol.set_mode(ElectraRemoteProtocol::MODE::MODE_COOL);
                break;

            case climate::CLIMATE_MODE_HEAT:
                this->electraRemoteProtocol.set_mode(ElectraRemoteProtocol::MODE::MODE_HEAT);
                break;
            case climate::CLIMATE_MODE_AUTO:
                this->electraRemoteProtocol.set_mode(ElectraRemoteProtocol::MODE::MODE_AUTO);
                break;
            case climate::CLIMATE_MODE_DRY:
                this->electraRemoteProtocol.set_mode(ElectraRemoteProtocol::MODE::MODE_DRY);
                break;
            case climate::CLIMATE_MODE_FAN_ONLY:
                this->electraRemoteProtocol.set_mode(ElectraRemoteProtocol::MODE::MODE_FAN);
                break;
            default:
                break;
            }
        }

        if (this->fan_mode.has_value())
        {
            switch (this->fan_mode.value())
            {
            case climate::CLIMATE_FAN_AUTO:
                this->electraRemoteProtocol.set_fan_speed(ElectraRemoteProtocol::FAN_SPEED::FAN_AUTO);
                break;

            case climate::CLIMATE_FAN_LOW:
                this->electraRemoteProtocol.set_fan_speed(ElectraRemoteProtocol::FAN_SPEED::FAN_LOW);
                break;

            case climate::CLIMATE_FAN_MEDIUM:
                this->electraRemoteProtocol.set_fan_speed(ElectraRemoteProtocol::FAN_SPEED::FAN_MEDIUM);
                break;

            case climate::CLIMATE_FAN_HIGH:
                this->electraRemoteProtocol.set_fan_speed(ElectraRemoteProtocol::FAN_SPEED::FAN_HIGH);
                break;
            default:
                break;
            }
        }

        switch (this->swing_mode)
        {
        case climate::CLIMATE_SWING_OFF:
            this->electraRemoteProtocol.set_swing1(ElectraRemoteProtocol::SWING1::SWING_OFF);
            this->electraRemoteProtocol.set_swing2(ElectraRemoteProtocol::SWING2::SWING_OFF);
            break;

        case climate::CLIMATE_SWING_VERTICAL:
            this->electraRemoteProtocol.set_swing1(ElectraRemoteProtocol::SWING1::SWING_ON);
            this->electraRemoteProtocol.set_swing2(ElectraRemoteProtocol::SWING2::SWING_OFF);
            break;

        case climate::CLIMATE_SWING_HORIZONTAL:
            this->electraRemoteProtocol.set_swing1(ElectraRemoteProtocol::SWING1::SWING_OFF);
            this->electraRemoteProtocol.set_swing2(ElectraRemoteProtocol::SWING2::SWING_ON);
            break;

        case climate::CLIMATE_SWING_BOTH:
            this->electraRemoteProtocol.set_swing1(ElectraRemoteProtocol::SWING1::SWING_ON);
            this->electraRemoteProtocol.set_swing2(ElectraRemoteProtocol::SWING2::SWING_ON);
            break;

        default:
            break;
        }

        uint8_t temp = static_cast<uint8_t>(roundf(clamp<float>(this->target_temperature, TEMP_MIN, TEMP_MAX)));

        ElectraRemoteProtocol::TEMPERATURE temp_enum;

        switch (temp)
        {
        case 16:
            temp_enum = ElectraRemoteProtocol::TEMPERATURE::TEMP_16;
            break;
        case 17:
            temp_enum = ElectraRemoteProtocol::TEMPERATURE::TEMP_17;
            break;
        case 18:
            temp_enum = ElectraRemoteProtocol::TEMPERATURE::TEMP_18;
            break;
        case 19:
            temp_enum = ElectraRemoteProtocol::TEMPERATURE::TEMP_19;
            break;
        case 20:
            temp_enum = ElectraRemoteProtocol::TEMPERATURE::TEMP_20;
            break;
        case 21:
            temp_enum = ElectraRemoteProtocol::TEMPERATURE::TEMP_21;
            break;
        case 22:
            temp_enum = ElectraRemoteProtocol::TEMPERATURE::TEMP_22;
            break;
        case 23:
            temp_enum = ElectraRemoteProtocol::TEMPERATURE::TEMP_23;
            break;
        case 24:
            temp_enum = ElectraRemoteProtocol::TEMPERATURE::TEMP_24;
            break;
        case 25:
            temp_enum = ElectraRemoteProtocol::TEMPERATURE::TEMP_25;
            break;
        case 26:
            temp_enum = ElectraRemoteProtocol::TEMPERATURE::TEMP_26;
            break;
        case 27:
            temp_enum = ElectraRemoteProtocol::TEMPERATURE::TEMP_27;
            break;
        case 28:
            temp_enum = ElectraRemoteProtocol::TEMPERATURE::TEMP_28;
            break;
        case 29:
            temp_enum = ElectraRemoteProtocol::TEMPERATURE::TEMP_29;
            break;
        case 30:
            temp_enum = ElectraRemoteProtocol::TEMPERATURE::TEMP_30;
            break;
        case 31:
            temp_enum = ElectraRemoteProtocol::TEMPERATURE::TEMP_31;
            break;
        case 32:
            temp_enum = ElectraRemoteProtocol::TEMPERATURE::TEMP_32;
            break;
        default:
            temp_enum = ElectraRemoteProtocol::TEMPERATURE::TEMP_24;
            break;
        }

        this->electraRemoteProtocol.set_temperature(temp_enum);

        auto signal = this->electraRemoteProtocol.get_signal();
        auto bytes_count = signal.size();

        std::string payload_str = "";
        for (size_t i = 0; i < bytes_count; i++)
        {
            for (int j = 7; j >= 0; j--)
            {
                payload_str += ((signal[i] >> j) & 1) ? "1" : "0";
            }
            payload_str += " "; // Space between each byte
        }
        ESP_LOGD(TAG, "Sending Electra IR State:");
        ESP_LOGD(TAG, "  Power: %s", climate_is_off ? "OFF" : "ON");
        ESP_LOGD(TAG, "  Mode: %d", this->mode);
        ESP_LOGD(TAG, "  Temp: %d C", temp);
        if (this->fan_mode.has_value())
        {
            ESP_LOGD(TAG, "  Fan: %d", this->fan_mode.value());
        }
        ESP_LOGD(TAG, "  Swing: %d", this->swing_mode);
        ESP_LOGD(TAG, "  Payload Code: %s", payload_str.c_str());
        ESP_LOGD(TAG, "  Payload Size: %zu bytes", bytes_count);

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

        data->mark(this->bit_zero_low_);
        transmit.perform();

        this->publish_state();
    }
}