/**
 *  Example demonstrating how to interface an Arduino with the IS3720
 *  over I2C to configure and read DMX/RDM data.
 */
#include <Wire.h>

// IS3720 memory map (16-bit addresses)
#define IS3720_I2C_SLAVE_ADDRESS  22
#define ADDRESS_CHIP_ID           513
#define ADDRESS_DMX_START_ADDRESS 516
#define ADDRESS_DMX_FOOTPRINT     519
#define ADDRESS_UID               521
#define ADDRESS_CATEGORY          527
#define ADDRESS_MANUFACTURER_LBL  529
#define ADDRESS_MODEL_NUM         561
#define ADDRESS_MODEL_LBL         563
#define ADDRESS_SOFT_NUM          595
#define ADDRESS_SOFT_LBL          599
#define ADDRESS_RDM_ONLINE        631

uint8_t data[100];

/**
 * @brief Write one or more bytes to a given IS3720 register address.
 * @param registerAddress  16-bit register address.
 * @param data             Pointer to data buffer to be written.
 * @param length           Number of bytes to write.
 */
void writeRegisters(uint16_t registerAddress, uint8_t* data, uint8_t length) {
  Wire.beginTransmission(IS3720_I2C_SLAVE_ADDRESS);

  // Send the 16-bit register address (MSB first).
  Wire.write((registerAddress >> 8) & 0xFF); // High byte.
  Wire.write(registerAddress & 0xFF);        // Low byte.

  // Send all data bytes in a single transmission.
  for (uint8_t i = 0; i < length; i++) {
    Wire.write(data[i]);
  }

  Wire.endTransmission();
}

/**
 * @brief   Read one or more bytes from a given IS3720 register address.
 * @param   registerAddress: The memory address (16bits) to read.
 * @param   buffer: An array to store the data. 
 * @param   lenght: How many bytes to read. 
 * @retval  Returns true if there was an error while reading. 
 *          Returns false if the reading was Ok. 
 */
bool readRegisters(uint16_t registerAddress, uint8_t* buffer, uint8_t length) {
  Wire.beginTransmission(IS3720_I2C_SLAVE_ADDRESS);
  Wire.write((registerAddress >> 8) & 0xFF);
  Wire.write(registerAddress & 0xFF);
  
  if (Wire.endTransmission(false) != 0) return true; // Returning true means there was some error reading the I2C. 
  if (Wire.requestFrom(IS3720_I2C_SLAVE_ADDRESS, length) != length) return true;

  for (uint8_t i = 0; i < length; i++) {
    buffer[i] = Wire.read();
  }

  return false; // Returning a false means there was no error reading the I2C. 
}

void setup() {
  Wire.begin();           // Initialize the I2C interface.
  Serial.begin(9600);     // Initialize the serial port for debug printing.

  delay(100);
  // Read the register CHIP_ID to detect the chip:
  while (1) {
    readRegisters(ADDRESS_CHIP_ID, data, 1);
    if (data[0] == 152) {
      Serial.println("IS3720 detected!");
      break;
    }
    else {
      Serial.println("IS3720 NOT detected.");
      delay(200);
    }
  }

  // Ensure RDM is disabled before configuration
  data[0] = 0; // A value of 0 disables the RDM
  writeRegisters(ADDRESS_RDM_ONLINE, data, 1);

  // Configure DMX footprint (number of DMX channels used, in our case 3, RGB)
  // This is a important value, tell the RDM controller how many DMX channels your product uses
  data[0] = 0;
  data[1] = 3;
  writeRegisters(ADDRESS_DMX_FOOTPRINT, data, 2);

  // Set product category (e.g. lighting fixture)
  // These values are defined by the RDM standard. Refer to the datasheet for more information.
  data[0] = 1;
  data[1] = 0;
  writeRegisters(ADDRESS_CATEGORY, data, 2);

  // Set Manufacturer Label (null-terminated string)
  // Update this with your specific data
  uint8_t manufacturer_lbl[] = "UNDERWATER LIGHTS COMPANY";
  writeRegisters(ADDRESS_MANUFACTURER_LBL, manufacturer_lbl, sizeof(manufacturer_lbl));

  // Set Model Number (example: 2000)
  // Update this with your specific data
  data[0] = 0x20;
  data[1] = 0x00;
  writeRegisters(ADDRESS_RDM_ONLINE, data, 2);

   // Set Model Label (null-terminated string)
   // Update this with your specific data
  uint8_t model_lbl[] = "Pool Light 2000";
  writeRegisters(ADDRESS_MODEL_LBL, model_lbl, sizeof(model_lbl));

  // Configure the Software Number to 1.25 
  // Update this with your specific data
  data[0] = 0x01;
  data[1] = 0x25;
  writeRegisters(ADDRESS_SOFT_NUM, data, 2);

  // Configure the Software Label
  // Update this with your specific data
  uint8_t software_lbl[] = "Version 1.25";
  writeRegisters(ADDRESS_SOFT_LBL, software_lbl, sizeof(software_lbl));

  // Configure the UID 
  // Check the datasheet for more information about the UID number
  // The UID can only be changed if RDM_ONLINE register is 0. 
  data[0] = 0x7F; data[1] = 0xF7; data[2] = 0x01; data[3] = 0x02; data[4] = 0x03; data[5] = 0x04;
  writeRegisters(ADDRESS_SOFT_NUM, data, 2);

  // Start the RDM:
  data[0] = 1;
  writeRegisters(ADDRESS_RDM_ONLINE, data, 1);

}

void loop() {
  uint8_t dmxData[3];
  static uint16_t dmxStartAddress, oldDmxStartAddress;
  bool error;

  // Read the DMX Start Address
  error = readRegisters(ADDRESS_DMX_START_ADDRESS, data, 2);
  if (error == false) {
    dmxStartAddress = (data[0] << 8);
    dmxStartAddress = dmxStartAddress | data[1];
    if (dmxStartAddress != oldDmxStartAddress) {
      oldDmxStartAddress = dmxStartAddress;
      Serial.print("DMX Start Address changed to: "); Serial.println(dmxStartAddress);
    }
  }

  // Read the DMX Channels:
  readRegisters(dmxStartAddress, dmxData, 3);

  // Control the LEDs using DMX values
  analogWrite(3, dmxData[0]); // Red
  analogWrite(5, dmxData[1]); // Green
  analogWrite(6, dmxData[2]); // Blue
}
