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
spr.drawFastVLine(270, 119, 18, TFT_WHITE);
spr.drawFastHLine(254, 136, 66, TFT_WHITE);

// 7 boxes - adjusted to your new mockup
// Time
spr.drawRect(2,   2, 40, 16, TFT_WHITE);
// Date
spr.drawRect(43,  2, 44, 16, TFT_WHITE);
// BFO
spr.drawRect(88,  2, 29, 16, TFT_WHITE);
// Mode
spr.drawRect(118, 2, 29, 16, TFT_WHITE);
// Bandwidth
spr.drawRect(148, 2, 54, 16, TFT_WHITE);
// Band
spr.drawRect(203, 2, 49, 16, TFT_WHITE);
// Step
spr.drawRect(253, 2, 40, 16, TFT_WHITE);
spr.setTextDatum(MC_DATUM);

// ----- TIME -----
spr.setTextColor(TFT_WHITE);
const char *timeText = clockGet();
spr.drawString(timeText ? timeText : "12:26", 22, 10, 2);

// ----- DATE -----
uint16_t year;
uint8_t month, day, weekday;
char dateText[6] = "07/05";

if (clockGetDate(&year, &month, &day, &weekday))
  sprintf(dateText, "%02u/%02u", day, month);

spr.drawString(dateText, 65, 10, 2);

// ----- BFO -----
bool bfoOn =
  (currentMode == USB && getCurrentBand()->usbCal != 0) ||
  (currentMode == LSB && getCurrentBand()->lsbCal != 0);

uint16_t c = bfoOn ? TFT_GREEN : TFT_DARKGREY;

spr.drawRect(88, 2, 29, 16, c);
spr.setTextColor(c);
spr.drawString("BFO", 103, 10, 2);

// ----- MODE -----
spr.setTextColor(TFT_WHITE);
spr.setTextDatum(MC_DATUM);
spr.drawString(bandModeDesc[currentMode], 132, 10, 2);
}
