#include "Common.h"
#include "Themes.h"
#include "Utils.h"
#include "Menu.h"
#include "Draw.h"

void drawLayoutDefault(const char *statusLine1, const char *statusLine2)
{
  #if 0
  // Draw preferences write request icon
  drawSaveIndicator(SAVE_OFFSET_X, SAVE_OFFSET_Y);

  // Draw BLE icon
  drawBleIndicator(BLE_OFFSET_X, BLE_OFFSET_Y);

  // Draw battery indicator & voltage
  bool has_voltage = drawBattery(BATT_OFFSET_X, BATT_OFFSET_Y);

  // Draw WiFi icon
  drawWiFiIndicator(has_voltage ? WIFI_OFFSET_X : BATT_OFFSET_X - 13, WIFI_OFFSET_Y);

  // Set font we are going to use
  spr.setFreeFont(&Orbitron_Light_24);

  // Draw band and mode
  drawBandAndMode(
    getCurrentBand()->bandName,
    bandModeDesc[currentMode],
    BAND_OFFSET_X, BAND_OFFSET_Y
  );

  if(switchThemeEditor())
  {
    spr.setTextDatum(TR_DATUM);
    spr.setTextColor(TH.text_warn);
    spr.drawString(TH.name, 319, BATT_OFFSET_Y + 17, 2);
  }

  // Draw frequency, units, and optionally highlight a digit
  drawFrequency(
    currentFrequency,
    FREQ_OFFSET_X, FREQ_OFFSET_Y,
    FUNIT_OFFSET_X, FUNIT_OFFSET_Y,
    currentCmd == CMD_FREQ ? getFreqInputPos() + (pushAndRotate ? 0x80 : 0) : 100
  );

  // Show station or channel name, if present
  if(*getStationName() == 0xFF)
    drawLongStationName(getStationName() + 1, MENU_OFFSET_X + 1 + 76 + MENU_DELTA_X + 2, RDS_OFFSET_Y);
  else if(*getStationName())
    drawStationName(getStationName(), RDS_OFFSET_X, RDS_OFFSET_Y);

  // Draw left-side menu/info bar
  // @@@ FIXME: Frequency display (above) intersects the side bar!
  drawSideBar(currentCmd, MENU_OFFSET_X, MENU_OFFSET_Y, MENU_DELTA_X);

  // Draw S-meter
  drawSMeter(getStrength(rssi), METER_OFFSET_X, METER_OFFSET_Y);

  // Indicate FM pilot detection (stereo indicator)
  drawStereoIndicator(METER_OFFSET_X, METER_OFFSET_Y, (currentMode==FM) && rx.getCurrentPilot());

  if(currentCmd == CMD_SCAN)
  {
    drawScanGraphs(isSSB()? (currentFrequency + currentBFO/1000) : currentFrequency);
  }
  else if(!drawWiFiStatus(statusLine1, statusLine2, STATUS_OFFSET_X, STATUS_OFFSET_Y))
  {
    // Show radio text if present, else show frequency scale
    if(*getRadioText() || *getProgramInfo())
      drawRadioText(STATUS_OFFSET_Y, STATUS_OFFSET_Y + 25);
    else
      drawScale(isSSB()? (currentFrequency + currentBFO/1000) : currentFrequency);
  }
  #endif
// Dark grey test background
spr.fillSprite(TFT_BLACK);

// Main divider lines
spr.drawRect(0, 0, 320, 170, TFT_WHITE);
spr.drawFastHLine(0, 19, 320, TFT_WHITE);
spr.drawFastVLine(72, 19, 151, TFT_WHITE);
spr.drawFastHLine(0, 72, 320, TFT_WHITE);
spr.drawFastHLine(72, 102, 248, TFT_WHITE);
spr.drawFastHLine(72, 119, 248, TFT_WHITE);
spr.drawFastVLine(254, 119, 51, TFT_WHITE);

// 7 boxes - adjusted to your new mockup
spr.drawRect(1,   1, 40, 17, TFT_WHITE);  // time
spr.drawRect(42,  1, 45, 17, TFT_WHITE);  // date
spr.drawRect(88,  1, 31, 17, TFT_WHITE);  // BFO
spr.drawRect(120, 1, 33, 17, TFT_WHITE);  // mode
spr.drawRect(154, 1, 57, 17, TFT_WHITE);  // bandwidth
spr.drawRect(212, 1, 43, 17, TFT_WHITE);  // band
spr.drawRect(256, 1, 42, 17, TFT_WHITE);  // step

spr.setTextDatum(MC_DATUM);

// ----- TIME -----
spr.setTextColor(TFT_WHITE, TFT_BLACK);
const char *timeText = clockGet();
spr.drawString(timeText ? timeText : "--:--", 21, 10, 2);

// ----- DATE -----
uint16_t year;
uint8_t month, day, weekday;
char dateText[6] = "--/--";

if (clockGetDate(&year, &month, &day, &weekday))
  sprintf(dateText, "%02u/%02u", day, month);

spr.drawString(dateText, 64, 10, 2);

// ----- BFO -----
bool bfoOn =
  (currentMode == USB && getCurrentBand()->usbCal != 0) ||
  (currentMode == LSB && getCurrentBand()->lsbCal != 0);

uint16_t bfoColor = bfoOn ? TFT_GREEN : TFT_DARKGREY;

spr.drawRect(88, 1, 31, 17, bfoColor);
spr.setTextColor(bfoColor, TFT_BLACK);
spr.drawString("BFO", 103, 10, 2);

// ----- MODE -----
spr.setTextColor(TFT_WHITE, TFT_BLACK);
spr.drawString(getModeShort(), 136, 10, 2);

// ----- TEMP PLACEHOLDERS FOR NOW -----
spr.drawString("F6.0kHz", 182, 10, 2);
spr.drawString("160M",    233, 10, 2);
spr.drawString("8.33k",   277, 10, 2);
}
