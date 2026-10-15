#include <SPI.h>          // Library for SPI Communication 
#include <mcp2515.h>      // Library for CAN Communication (https://github.com/autowp/arduino-mcp2515/)

// -------- CAN Frame --------
struct can_frame canMsg;

// -------- MCP2515 on CS Pin 10 --------
MCP2515 mcp2515(10);

// -------- Serial Buffer --------
String inputData = "";

void setup()
{
  Serial.begin(9600);          // UART from STM32
  while (!Serial);

  SPI.begin();                 // Begins SPI communication

  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ);  // Sets CAN at 500KBPS, 8MHz
  mcp2515.setNormalMode();

  // Serial.println("CAN + UART Bridge Ready");
}

void loop()
{
  // -------- Receive from STM32 UART --------
  if (Serial.available()) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      if (inputData.length() > 0) {
        // Serial.print("Received from STM32: ");
        // Serial.println(inputData);

        // Convert string to integer value
        int value = inputData.toInt();   // e.g., "1" -> 1

        // -------- Prepare CAN Frame --------
        canMsg.can_id  = 0x100;        // Example CAN ID
        canMsg.can_dlc = 1;            // Sending 1 byte only
        canMsg.data[0] = (byte)value;  // Numeric value (0x01, 0x02, 0x03)

        // -------- Send via CAN --------
        if (mcp2515.sendMessage(&canMsg) == MCP2515::ERROR_OK) {
          // Serial.print("Sent over CAN (numeric): ");
          Serial.println(value);
        } else {
          // Serial.println("CAN send fail");
        }
      }
      inputData = "";  // clear buffer
    } else {
      inputData += c;  // accumulate serial data
    }
  }
}
