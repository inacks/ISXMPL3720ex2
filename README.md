# Arduino Example for the IS3720 I2C DMX+RDM Receiver Chip

## Objective
Demonstrate how to remotely configure the DMX Start Address and control an LED via DMX using the **IS3720 I2C DMX+RDM Receiver** with **Arduino**.

## Required Material
- **IS3720**
- RS-485 transceiver (e.g., SN75176A or THVD1500)
- **Arduino UNO**  
- **Enttec** or other DMX+RDM controller

## Setup
1. Download the **ISXMPL3720ex2** code from this repository.
2. Configure your RDM data (company name, product name, etc.).
3. Set the DMX Start Address via RDM, and start sending DMX data.

## How it works
The program continuously checks the assigned DMX Start Address set by the RDM controller. It then reads the corresponding DMX channel data and maps it to the LEDs.

## Other
- More information: [https://inacks.com/is3725](https://inacks.com/is3725)  
