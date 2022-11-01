#include <JrkG2.h>

#include "src\CrsfSerial.h"
#include "Configuration.h"


// Initializing JRK G2 controllers per device ID
// S1
JrkG2I2C jrk4(17);

// S2
JrkG2I2C jrk1(11);

// L1
JrkG2I2C jrk3(15);

// L2
JrkG2I2C jrk2(13);


// Initializing ExpressLRS UART
CrsfSerial crsf(Serial1, 250000);

/*
 * Read values from the controllers and publish them onto the serial connection
 */
void read_and_send_values()
{
  // S1
  uint16_t S1_p = jrk4.getScaledFeedback();

  // S2
  uint16_t S2_p = jrk1.getScaledFeedback();

  // L1
  uint16_t L1_p = jrk3.getScaledFeedback();

  // L2
  uint16_t L2_p = jrk2.getScaledFeedback();

  Serial.print("S1: ");
  Serial.println(S1_p);

  Serial.print("S2: ");
  Serial.println(S2_p);

  Serial.print("L1: ");
  Serial.println(L1_p);

  Serial.print("L2: ");
  Serial.println(L2_p);
}

/*
 * Callback triggered whenever new values appear on the UART 
 */
void packetChannels()
{
  int la_lb = 300;
  int la_ub = 3000;
    
  int ch1_0 = crsf.getChannel(1);
  int ch2_0 = crsf.getChannel(2);
  int ch3_0 = crsf.getChannel(3);
  int ch4_0 = crsf.getChannel(4);

  /*
   * Mixing channels done in TX for the sake of simplicity
   */

  // Mapping values to limits
  int S1_f = map(
    ch3_0, 
    CHANNEL_3_LOW_EP, 
    CHANNEL_3_HIGH_EP, 
    S1_LOWER_BOUND, 
    S1_UPPER_BOUND);
    
  int S2_f = map(
    ch4_0, 
    CHANNEL_4_LOW_EP, 
    CHANNEL_4_HIGH_EP, 
    S2_LOWER_BOUND, 
    S2_UPPER_BOUND);
    
  int L1_f = map(
    ch1_0, 
    CHANNEL_1_LOW_EP, 
    CHANNEL_1_HIGH_EP, 
    L1_LOWER_BOUND, 
    L1_UPPER_BOUND);
     
  int L2_f = map(
    ch2_0, 
    CHANNEL_2_LOW_EP, 
    CHANNEL_2_HIGH_EP, 
    L2_LOWER_BOUND, 
    L2_UPPER_BOUND);

  // Setting targets for individual linear actuator
  // L1
  jrk3.setTarget(L1_f);

  // L2 
  jrk2.setTarget(L2_f);

  // S1
  jrk4.setTarget(S1_f);

  // S2
  jrk1.setTarget(S2_f);

  // Collect position values and read them out to the Jetson
  read_and_send_values();
}


/*
 * Conditions to be satisfied in the case of a connectivity error being detected
 */
void crsfLinkDown() {
    // Position actuators in safe position
    // L1
    jrk3.setTarget(L1_FAILSAFE_POSITION);

    // L2 
    jrk2.setTarget(L2_FAILSAFE_POSITION);

    // S1
    jrk4.setTarget(S1_FAILSAFE_POSITION);

    // S2
    jrk1.setTarget(S2_FAILSAFE_POSITION);
}


void setup()
{
    // Start UART connection to Jetson
    Serial.begin(57600);

    // Start I2C connection to JRK G2 controllers
    Wire.begin();

    // Attaching callbacks
    crsf.onPacketChannels = &packetChannels;
    crsf.onLinkDown = &crsfLinkDown;

}


void loop()
{
    // Must call CrsfSerial.loop() in loop() to process data
    crsf.loop();
}
