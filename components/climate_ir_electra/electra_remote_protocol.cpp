#include "electra_remote_protocol.h"

ElectraRemoteProtocol::ElectraRemoteProtocol::ElectraRemoteProtocol()
    : temp(TEMPERATURE::TEMP_24), fan_speed(FAN_SPEED::FAN_AUTO), power(POWER::POWER_OFF), swing1(SWING1::SWING_OFF), swing2(SWING2::SWING_OFF), button(BUTTON::BUTTON_POWER), mode(MODE::MODE_COOL), sleep(SLEEP::SLEEP_OFF) {}

ElectraRemoteProtocol::ElectraRemoteProtocol::~ElectraRemoteProtocol() = default;

void ElectraRemoteProtocol::ElectraRemoteProtocol::set_temperature(TEMPERATURE temp) {
    if(temp  > this-> temp) {
        this->button = BUTTON::BUTTON_TEMP_UP;
    } else {
        this->button = BUTTON::BUTTON_TEMP_DOWN;
    }

    this->temp = temp;
}

void ElectraRemoteProtocol::ElectraRemoteProtocol::set_fan_speed(FAN_SPEED speed) {
    this->fan_speed = speed;
    this->button = BUTTON::BUTTON_SPEED;
}

void ElectraRemoteProtocol::ElectraRemoteProtocol::set_power(POWER power) {
    this->power = power;
    this->button = BUTTON::BUTTON_POWER;
}

void ElectraRemoteProtocol::ElectraRemoteProtocol::set_swing1(SWING1 swing1) {
    this->swing1 = swing1;
    this->button = BUTTON::BUTTON_SWING1;
}
void ElectraRemoteProtocol::ElectraRemoteProtocol::set_swing2(SWING2 swing2) {
    this->swing2 = swing2;
    this->button = BUTTON::BUTTON_SWING2;
}
void ElectraRemoteProtocol::ElectraRemoteProtocol::set_mode(MODE mode) {
    this->mode = mode;
    this->button = BUTTON::BUTTON_MODE;
}

void ElectraRemoteProtocol::ElectraRemoteProtocol::set_sleep(SLEEP sleep) {
    this->sleep = sleep;
    this->button = BUTTON::BUTTON_SLEEP;
}

uint8_t ElectraRemoteProtocol::ElectraRemoteProtocol::reverse_Uint8_t(uint8_t byte) {
    uint8_t reversed = 0;
    for (int i = 0; i < 8; ++i) {
        reversed <<= 1;
        reversed |= (byte & 1);
        byte >>= 1;
    }
    return reversed;
}

std::vector<uint8_t> ElectraRemoteProtocol::ElectraRemoteProtocol::get_signal() {
    std::vector<uint8_t> signal_bytes(13);

    // byte 0 address
    signal_bytes[0] = address;

    // byte 1 swing 1 and temp
    // swing1, tmp
    // xxx     yyyyy
    signal_bytes[1] = (static_cast<uint8_t>(swing1) << 5) | (static_cast<uint8_t>(temp));

    // byte 2 hours(which we ignore and set to 00000) and swing 2
    // const swing2
    // 00000 xxx
    // incase we need hours we need to change this

    signal_bytes[2] = (0b00000 << 3) | (static_cast<uint8_t>(swing2));

    // byte 3 minutes(which we ignore and set to 00000)
    // minutes
    // 00000000
    // incase we need minutes we need to change this

    signal_bytes[3] = 0b00000000;

    // byte 4 fullhours of time(which we ignore and set to 00000) and fan speed
    // const fan speed
    // 00000 xxx
    signal_bytes[4] = (0b00000 << 3) | (static_cast<uint8_t>(fan_speed));

    // byte 5 last time i checked it have the stong off/on and is timer on
    // we set it all to 0 for now
    signal_bytes[5] = 0b00000000;

    // byte 6 sleep and mode
    // sleep, mode
    // xxxxx  yyy
    signal_bytes[6] = (static_cast<uint8_t>(sleep) << 3) | (static_cast<uint8_t>(mode));

    // byte 7 last i chagned it changed feel/health mode but we ignore it and set to 00000000
    signal_bytes[7] = 0b00000000;

    // byte 8 const 0b0000000
    signal_bytes[8] = 0b00000000;

    // byte 9 on off in this case, but i saw some other remores that control here is heating and is timer and is health
    // we ignore it and use it as on/off switch
    signal_bytes[9] = static_cast<uint8_t>(power);

    // byte 10 const 0b00000000
    signal_bytes[10] = 0b00000000;

    // byte 11 button
    signal_bytes[11] = static_cast<uint8_t>(button);


    // calculate checksum
    uint8_t checksum = 0;
    for (size_t i = 0; i < 12; ++i) {
        checksum += reverse_Uint8_t(signal_bytes[i]);
    }

    signal_bytes[12] = checksum;

    return signal_bytes;
}