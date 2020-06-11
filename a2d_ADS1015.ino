/*
 * ADS1015 12 bit analog to digital
 * 
 * use library:
 * https://github.com/adafruit/Adafruit_ADS1X15

 * see https://learn.adafruit.com/adafruit-4-channel-adc-breakouts
 * smaller board, same pinout
 * VDD - 2 to 5 VDC
 * GND - ground
 * SCL - i2c pins (on wemos: SDA => D2, SCL => D1)
 * SDA - ic2 pins
 * ADDR - sets the i2c address
 *   0x48 (1001000) ADR -> GND
 *   0x49 (1001001) ADR -> VDD
 *   0x4A (1001010) ADR -> SDA
 *   0x4B (1001011) ADR -> SCL
 * ALRT - set when comparator is triggered.  normally HIGH, goes LOW when triggered.
 *   hookup to input pin to set an interrupt
 * A0 - analog input
 * A1 - analog input
 * A2 - analog input
 * A3 - analog input
 * 
 * it can measure 4 single ended inputs (difference between
 * input and ground)
 * or it can measure 2 differential inputs (difference between 
 * A0&A1 or A2&A3)
 * NOTE: max input voltage for any input is VDD
 * 
 * 
 * Single Ended Conversion:
 * uint16_t readADC_SingleEnded(uint8_t channel);  
 *     Perform a single-ended analog to digital conversion on the specified channel.
 *
 *
 *
 * Differential Conversion:
 * int16_t readADC_Differential_0_1(void);  
 *     Perform a differential analog to digital conversion on the voltage between channels 0 and 1.
 * 
 * 
 * 
 * comparator operation:
 * compare an input voltage to a threshold and set the ALRT pin
 * if the threshold is exceeded. (or read the conversion result
 * over i2c)
 * void startComparator_SingleEnded(uint8_t channel, int16_t threshold);  
 *     Set the threshold and channel for comparator operation.
 * int16_t getLastConversionResults();
 *     Get the last conversion result and clear the comparator.
 */


#include <Wire.h>
#include <Adafruit_ADS1015.h>

// default uses 0x48 for the address (tie ADR to GND)
// Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */
Adafruit_ADS1015 ads;     /* Use this for the 12-bit version */


bool isComparator = false;
bool isSingle = true;
bool isDifferential = false;

double gain;


void setup(void) 
{
  // start serial port
  Serial.begin(115200);
  Serial.print(F("\n\n ADS1015 test\n\n"));
  
  Serial.println("Single-ended readings from AIN0 with >3.0V comparator");
  Serial.println("ADC Range: +/- 6.144V (1 bit = 3mV/ADS1015, 0.1875mV/ADS1115)");
  Serial.println("Comparator Threshold: 1000 (3.000V)");

  
  // The ADC input range (or gain) can be changed via the following
  // functions, but be careful never to exceed VDD +0.3V max, or to
  // exceed the upper and lower limits if you adjust the input range!
  // Setting these values incorrectly may destroy your ADC!
  //                                                                ADS1015  ADS1115
  //                                                                -------  -------
  ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
  // ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
  // ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
  // ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
  // ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
  // ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV

  gain = 0.003;
//  gain = 0.001;
  
  ads.begin();

  if (isComparator) {
    // Setup comparator on A0, threshold is 1000 (or 3.0V)
    ads.startComparator_SingleEnded(0, 1000);
  }
}

void loop(void) 
{
  if (isComparator) {
    // Comparator will only de-assert after a read
    int16_t adc0 = ads.getLastConversionResults();
    double volts = gain * adc0;
    Serial.printf("AIN0: %d, volts: %f\n", adc0, volts);
  }

  if (isSingle) {
    int16_t adc0 = ads.readADC_SingleEnded(0);
    double volts = gain * adc0;
    Serial.printf("AIN0: %d, volts: %f\n", adc0, volts);
  }

  if (isDifferential) {
    int16_t adc01 = ads.readADC_Differential_0_1();
    double volts = gain * adc01;
    Serial.printf("AIN01: %d, volts: %f\n", adc01, volts);
  }
  
  delay(100);
}
