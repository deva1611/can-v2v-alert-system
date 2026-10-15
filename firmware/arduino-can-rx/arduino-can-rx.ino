#include <SPI.h>
#include <mcp2515.h>

// -------- CAN Frame --------
struct can_frame canMsg;

// -------- MCP2515 on CS Pin 10 --------
MCP2515 mcp2515(10);

void setup() {
  Serial.begin(9600);           // Output to STM32/PC
  while (!Serial);

  SPI.begin();                  // Begin SPI communication

  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ);  // CAN speed
  mcp2515.setNormalMode();

  // Serial.println("CAN Receiver Ready...");
}

void loop() {
  if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) {
    // Example: printing received frame
    // Serial.print("ID: ");
    // Serial.print(canMsg.can_id, HEX);
    // Serial.print("  Data: ");

    for (int i = 0; i < canMsg.can_dlc; i++) {
      Serial.print(canMsg.data[i], DEC);   // Print numeric value
      Serial.print(" ");
    }
    Serial.println();
  }
}
