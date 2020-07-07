#include "Arduhdlc.h"
#include "defined_commands.h"
#define MAX_HDLC_FRAME_LENGTH 32

/* Function to send out byte/char */
void send_character(uint8_t data);

/* Function to handle a valid HDLC frame */
void hdlc_command_router(const uint8_t *data, uint16_t length);

/* Initialize Arduhdlc library with three parameters.
1. Character send function, to send out HDLC frame one byte at a time.
2. HDLC frame handler function for received frame.
3. Length of the longest frame used, to allocate buffer in memory */
Arduhdlc hdlc(&send_character, &hdlc_command_router, MAX_HDLC_FRAME_LENGTH);

/* Function to send out one 8bit character */
void send_character(uint8_t data) {
  Serial.print("### send_character");
  //  Serial1.print((char)data);
}

/*
* Command router/dispatcher
* Calls the right command, passing data to it.
*/
void hdlc_command_router(const uint8_t *framebuffer, uint16_t length) {
    enum serial_commands command = static_cast<serial_commands>(framebuffer[0]);
    switch(command)
    {
        case COMMAND_ERROR: Serial.print("__COMAND ERROR__");  break;        
        /** case COMMAND_SET_SERVO_POSITION:     command_set_servo_position(framebuffer, framelength); break;
        case COMMAND_TOGGLE_LED:             command_toggle_led(framebuffer, framelength); break;
        case COMMAND_READ_SIGNATURE:         command_read_signature(framebuffer, framelength); break;
        case COMMAND_ECHO_DATA:              command_echo_data(framebuffer, framelength); break;*/
        case COMMAND_CUSTOM:     command_custom(framebuffer, length);  break;
        default:
          //command_default();
        break;
    }    
}

// L298N driver
#define enA 10
#define in1 6
#define in2 7

// second motor
#define enB 11
#define in3 4
#define in4 5

void setup() {
    // communication with the host computer
    Serial.begin(9600);  

    // communication with the BT module on serial1
    // using pin 18 TX, pin 19 RX
    Serial1.begin(9600);

    // L298N
    pinMode(enA, OUTPUT);
    pinMode(in1, OUTPUT);
    pinMode(in2, OUTPUT);

    pinMode(enB, OUTPUT);
    pinMode(in3, OUTPUT);
    pinMode(in4, OUTPUT);

    // Set initial rotation direction
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);

    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);

    //analogWrite(enA, 255);
    //analogWrite(enB, 50);
    
}

void loop() { }

void serialEvent1() {
    while (Serial1.available()) {
        // get the new byte:
        int inByte = Serial1.read();
        char inChar = (char)inByte;
        
        // Pass all incoming data to hdlc char receiver
        hdlc.charReceiver(inChar);
    }
}

void command_custom(const uint8_t *data, uint16_t length) {    
    analogWrite(enA, data[1]);
}
