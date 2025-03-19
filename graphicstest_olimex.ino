#include "Board_Pinout.h"
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include "Wire.h"
#include "NS2009.h"
#include <ESP32CAN.h>
#include <CAN_config.h>
CAN_device_t CAN_cfg;


#define RELAY1 32
#define RELAY2 33
#define BOXSIZE 40

bool blueStatus = true;
bool addr40A = false;
bool addr40B = false;

NS2009 TS(false, true);
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

void resetLeftButoon() {
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(3);
  tft.fillRoundRect(20, 200, 100, 100, 8, ILI9341_RED);
  tft.drawRoundRect(20, 200, 100, 100, 8, ILI9341_WHITE);
  tft.setCursor(45, 240);
  tft.println("ON");
}

void pressedLeftButoon() {
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(3);
  tft.fillRoundRect(20, 200, 100, 100, 8, ILI9341_GREEN);
  tft.drawRoundRect(20, 200, 100, 100, 8, ILI9341_WHITE);
  tft.setCursor(45, 240);
  tft.println("ON");
}

void resetRightButoon() {
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(3);
  tft.fillRoundRect(120, 200, 100, 100, 8, ILI9341_RED);
  tft.drawRoundRect(120, 200, 100, 100, 8, ILI9341_WHITE);
  tft.setCursor(130, 240);
  tft.println(" OFF");
}

void pressedRightButoon() {
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(3);
  tft.fillRoundRect(120, 200, 100, 100, 8, ILI9341_GREEN);
  tft.drawRoundRect(120, 200, 100, 100, 8, ILI9341_WHITE);
  tft.setCursor(130, 240);
  tft.println(" OFF");
}

void pressedAddressLeftButton() {
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(3);
  tft.fillRoundRect(20, 100, 100, 100, 8, ILI9341_DARKGREEN);
  tft.drawRoundRect(20, 100, 100, 100, 8, ILI9341_WHITE);
  tft.setCursor(45, 140);
  tft.println("40A");
}

void pressedAddressRightButton() {
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(3);
  tft.fillRoundRect(120, 100, 100, 100, 8, ILI9341_DARKGREEN);
  tft.drawRoundRect(120, 100, 100, 100, 8, ILI9341_WHITE);
  tft.setCursor(130, 140);
  tft.println(" 40B");
}

void resetAddressLeftButton() {
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(3);
  tft.fillRoundRect(20, 100, 100, 100, 8, ILI9341_BLUE);
  tft.drawRoundRect(20, 100, 100, 100, 8, ILI9341_WHITE);
  tft.setCursor(45, 140);
  tft.println("40A");
}

void resetAddressRightButton() {
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(3);
  tft.fillRoundRect(120, 100, 100, 100, 8, ILI9341_BLUE);
  tft.drawRoundRect(120, 100, 100, 100, 8, ILI9341_WHITE);
  tft.setCursor(130, 140);
  tft.println(" 40B");
}

void background() {
  tft.fillRoundRect(10, 90, 220, 220, 8, ILI9341_OLIVE);
  tft.drawRoundRect(10, 90, 220, 220, 8, ILI9341_WHITE);
}

void resetBothRedButtons() {
  resetLeftButoon();
  resetRightButoon();
  digitalWrite(RELAY1, LOW);
  digitalWrite(RELAY2, LOW);
}

void resetBothBlueButtons() {
  resetAddressLeftButton();
  resetAddressRightButton();
}

void resetAllButtons() {
  resetBothRedButtons();
  resetBothBlueButtons();
}

void statusPanelStartup() {
  tft.setCursor(40, 25);
  tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(4);
  tft.println("Welcome");
  tft.setCursor(10, 60);
  tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(1);
  tft.println(" Touch the Address and Status value");
}

void statusPanel(int messages) {
  tft.fillRect(0, 0, 240, 85, ILI9341_BLACK);
  tft.setCursor(5, 25);
  tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(2);
  tft.print("Last Message Sent->\n\n ");
  switch (messages) {
    case 1:
      tft.print("Address - 40A ON");
      sendCanMessage(1);
      break;

    case 2:
      tft.print("Address - 40B ON");
      sendCanMessage(2);
      break;

    case 3:
      tft.print("Address - 40A OFF");
      sendCanMessage(3);
      break;

    case 4:
      tft.print("Address - 40B OFF");
      sendCanMessage(4);
      break;

    default:
      printf("Invalid");
      break;
  }
}

void clearStatus() {
  tft.fillRect(0, 0, 240, 85, ILI9341_BLACK);
}

void homeScreen() {
  tft.fillScreen(ILI9341_BLACK);
  statusPanelStartup();
  background();
  resetAllButtons();
  pressedAddressLeftButton();
  pressedRightButoon();
}

void sendCanMessage(int Input) {
  uint8_t Output_Buff[8];
  CAN_frame_t rx_frame;
  switch (Input) {
    case 1:
    rx_frame.MsgID = 0x40A;
      Output_Buff[0] = 0x01;
      Output_Buff[1] = 0x01;
      Output_Buff[2] = 0x01;
      Output_Buff[3] = 0x01;
      Output_Buff[4] = 0x01;
      Output_Buff[5] = 0x01;
      Output_Buff[6] = 0x01;
      Output_Buff[7] = 0x01;
      break;

    case 2:
    rx_frame.MsgID = 0x40B;
      Output_Buff[0] = 0x01;
      Output_Buff[1] = 0x01;
      Output_Buff[2] = 0x01;
      Output_Buff[3] = 0x01;
      Output_Buff[4] = 0x01;
      Output_Buff[5] = 0x01;
      Output_Buff[6] = 0x01;
      Output_Buff[7] = 0x01;
      break;

    case 3:
    rx_frame.MsgID = 0x40A;
      Output_Buff[0] = 0x00;
      Output_Buff[1] = 0x00;
      Output_Buff[2] = 0x00;
      Output_Buff[3] = 0x00;
      Output_Buff[4] = 0x00;
      Output_Buff[5] = 0x00;
      Output_Buff[6] = 0x00;
      Output_Buff[7] = 0x00;
      break;

    case 4:
    rx_frame.MsgID = 0x40B;
      Output_Buff[0] = 0x00;
      Output_Buff[1] = 0x00;
      Output_Buff[2] = 0x00;
      Output_Buff[3] = 0x00;
      Output_Buff[4] = 0x00;
      Output_Buff[5] = 0x00;
      Output_Buff[6] = 0x00;
      Output_Buff[7] = 0x00;
      break;
  }
  rx_frame.FIR.B.FF = CAN_frame_std;
  rx_frame.FIR.B.DLC = 8;
  //strncpy ((char*)rx_frame.data.u8, (char*)Output_Buff, 8);  // message length max 8 chars
  for (int i = 0; i < 8; i++) {
    rx_frame.data.u8[i] = Output_Buff[i];
  }
  ESP32Can.CANWriteFrame(&rx_frame);
  printf("BB-Message sent through CAN!\n");
  printf("Message ID = %d; DLC = %d\n", rx_frame.MsgID, rx_frame.FIR.B.DLC);
  printf("Sent data: %s\n\n\n", Output_Buff);
}

void setup() {
  Serial.begin(115200);
  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);

  delay(1000);
  //CAN Initialize
  CAN_cfg.speed = CAN_SPEED_1000KBPS;
  CAN_cfg.tx_pin_id = GPIO_NUM_5;
  CAN_cfg.rx_pin_id = GPIO_NUM_35;
  CAN_cfg.rx_queue = xQueueCreate(10, sizeof(CAN_frame_t));
  //initialize CAN Module
  ESP32Can.CANInit();

  tft.begin();

#if (defined ARDUINO_ESP32_WROVER_KIT) || (defined ARDUINO_ESP32_DEV)
  Wire.begin(18, 23);
#else
  Wire.begin();
#endif

  pinMode(TFT_DC, OUTPUT);
  delay(1000);
  //testFillScreen();

  homeScreen();
}
void loop(void) {
  bool answer = Get_on_off();
  if (answer) {
    //Serial.println("ON");
    if (blueStatus) {
      digitalWrite(RELAY1, HIGH);
      Serial.println("Address - 40A send ON");
      statusPanel(1);
    } else {
      digitalWrite(RELAY2, HIGH);
      Serial.println("Address - 40B send ON");
      statusPanel(2);
    }
  } else {
    //Serial.println("OFF");
    if (blueStatus) {
      digitalWrite(RELAY1, LOW);
      Serial.println("Address - 40A send OFF");
      statusPanel(3);
    } else {
      digitalWrite(RELAY2, LOW);
      Serial.println("Address - 40B send OFF");
      statusPanel(4);
    }
  }
}

bool Get_on_off(void) {
  while (1) {
    TS.ScanBlocking();
    Serial.printf("Screen touched!\n\r");
    if ((TS.Y > 200) && (TS.Y < 300)) {
      if ((TS.X > 20) && (TS.X < 220)) {
        //touched OFF button
        if (TS.X > 120) {
          pressedRightButoon();
          resetLeftButoon();
          if (blueStatus)
            addr40A = false;
          else
            addr40B = false;
          return false;
        }
        //touched ON button
        else {
          pressedLeftButoon();
          resetRightButoon();
          if (blueStatus)
            addr40A = true;
          else
            addr40B = true;
          return true;
        }
      }
    } else if ((TS.Y > 100) && (TS.Y < 180)) {
      if ((TS.X > 20) && (TS.X < 220)) {
        if (TS.X > 120) {
          //pressed 40B address
          pressedAddressRightButton();
          resetAddressLeftButton();
          if (addr40B) {
            pressedLeftButoon();
            resetRightButoon();
          } else {
            pressedRightButoon();
            resetLeftButoon();
          }
          blueStatus = false;
          //clearStatus();
          continue;
        } else {
          //pressed 40A address
          pressedAddressLeftButton();
          resetAddressRightButton();
          if (addr40A) {
            pressedLeftButoon();
            resetRightButoon();
          } else {
            pressedRightButoon();
            resetLeftButoon();
          }
          blueStatus = true;
          //clearStatus();
          continue;
        }
      }
    }
    while (TS.CheckTouched())
      ;
    Serial.printf("Screen released!\n\r");
  }
}

unsigned long testFillScreen() {
  unsigned long start = micros();
  tft.fillScreen(ILI9341_RED);
  yield();
  delay(150);
  tft.fillScreen(ILI9341_GREEN);
  yield();
  delay(150);
  tft.fillScreen(ILI9341_BLUE);
  yield();
  delay(150);
  tft.fillScreen(ILI9341_YELLOW);
  yield();
  delay(150);
  return micros() - start;
}
