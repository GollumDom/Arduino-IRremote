/*
 * IRremote: IRsendRawDemo - demonstrates sending IR codes with sendRaw
 * An IR LED must be connected to Arduino PWM pin 3.
 * Initially coded 2009 Ken Shirriff http://www.righto.com
 *
 * IRsendRawDemo - added by AnalysIR (via www.AnalysIR.com), 24 August 2015
 *
 * This example shows how to send a RAW signal using the IRremote library.
 * The example signal is actually a 32 bit NEC signal.
 * Remote Control button: LGTV Power On/Off.
 * Hex Value: 0x20DF10EF, 32 bits
 *
 * It is more efficient to use the sendNEC function to send NEC signals.
 * Use of sendRaw here, serves only as an example of using the function.
 *
 */

#include <IRremote.h>

IRsend IrSender;

// On the Zero and others we switch explicitly to SerialUSB
#if defined(ARDUINO_ARCH_SAMD)
#define Serial SerialUSB
#endif

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);

    Serial.begin(115200);
#if defined(__AVR_ATmega32U4__) || defined(SERIAL_USB) || defined(SERIAL_PORT_USBVIRTUAL)
    delay(2000); // To be able to connect Serial monitor after reset and before first printout
#endif
    // Just to know which program is running on my Arduino
    Serial.println(F("START " __FILE__ " from " __DATE__));
    Serial.print(F("Ready to send IR signals at pin "));
    Serial.println(IR_SEND_PIN);
}

/*
 * NEC address=0xFB0C, command=0x18
 *
 * This is data in byte format.
 * The uint8_t/byte elements contain the number of ticks in 50 us
 * The integer format contains the (number of ticks * 50) if generated by IRremote,
 * so the integer format has the same resolution but requires double space.
 */
const uint8_t irSignalP[] PROGMEM
= { 180, 90 /*Start bit*/, 11, 11, 11, 11, 11, 34, 11, 34/*0011 0xC of address LSB first*/, 11, 11, 11, 11, 11, 11, 11, 11/*0000*/,
        11, 34, 11, 34, 11, 11, 11, 34/*1101 0xB*/, 11, 34, 11, 34, 11, 34, 11, 34/*1111*/, 11, 11, 11, 11, 11, 11, 11,
        34/*0001 0x08 of command LSB first*/, 11, 34, 11, 11, 11, 11, 11, 11/*1000 0x01*/, 11, 34, 11, 34, 11, 34, 11,
        11/*1110 Inverted 8 of command*/, 11, 11, 11, 34, 11, 34, 11, 34/*0111 inverted 1 of command*/, 11 /*stop bit*/};

void loop() {
    int khz = 38; // 38kHz carrier frequency for the NEC protocol
    /*
     * Send hand crafted data from RAM
     * The values are NOT multiple of 50, but are taken from the NEC timing definitions
     */
    Serial.println(F("Send NEC 0xFB04, 0x08 with exact timing (integer format)"));
    const uint16_t irSignal[] = { 9000, 4500, 560, 560, 560, 560, 560, 1690, 560, 560, 560, 560, 560, 560, 560, 560, 560, 560,
            560, 1690, 560, 1690, 560, 560, 560, 1690, 560, 1690, 560, 1690, 560, 1690, 560, 1690, 560, 560, 560, 560, 560, 560,
            560, 1690, 560, 560, 560, 560, 560, 560, 560, 560, 560, 1690, 560, 1690, 560, 1690, 560, 560, 560, 1690, 560, 1690, 560,
            1690, 560, 1690, 560 }; // Using exact NEC timing
    IrSender.sendRaw(irSignal, sizeof(irSignal) / sizeof(irSignal[0]), khz); // Note the approach used to automatically calculate the size of the array.

    delay(2000);

    /*
     * Send byte data direct from FLASH
     * Note the approach used to automatically calculate the size of the array.
     */
    Serial.println(F("Send NEC 0xFB0C, 0x18 with tick resolution timing (byte format) "));
    IrSender.sendRaw_P(irSignalP, sizeof(irSignalP) / sizeof(irSignalP[0]), khz);

    delay(2000);

    Serial.println(F("Send NEC 0xFF00, 0x17 generated"));
    IrSender.sendNECStandard(0xFF00, 0x17, 0);

    delay(3000);
}
