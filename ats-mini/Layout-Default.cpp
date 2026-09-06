#include "Common.h"
#include "Themes.h"
#include "Utils.h"
#include "Menu.h"
#include "Draw.h"
#include "Storage.h"

void drawPixelIcon(int x, int y, const char *icon[], int h, uint16_t c)
{
  for (int row = 0; row < h; row++)
  {
    for (int col = 0; icon[row][col] != 0; col++)
    {
      if (icon[row][col] == '1')
        spr.drawPixel(x + col, y + row, c);
    }
  }
}

const char *bleIcon[] =
{
  "...1...",
  "...11..",
  "...1.1.",
  "1..1..1",
  ".1.1.1.",
  "..111..",
  "...1...",
  "..111..",
  ".1.1.1.",
  "1..1..1",
  "...1.1.",
  "...11..",
  "...1..."
};

const char *wifiIcon[] =
{
  "...11111111...",
  "..1........1..",
  ".1..111111..1.",
  "1..1......1..1",
  "..1..1111..1..",
  "....1....1....",
  "......11......",
  "......11......"
};

const char *saveIcon[] =
{
  "...111...",
  "...111...",
  "...111...",
  "...111...",
  "...111...",
  ".1111111.",
  "..11111..",
  "...111...",
  "....1....",
  ".........",
  "1.......1",
  "111111111"
};

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
spr.drawString(timeText ? timeText : "--:--", 22, 10, 2);

// ----- DATE -----
uint16_t year;
uint8_t month, day, weekday;
char dateText[6] = "--/--";

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
spr.drawString(bandModeDesc[currentMode], 133, 10, 2);

// ----- Bandwidth -----
char bwText[12];

if (strcmp(getCurrentBandwidth()->desc, "Auto") == 0)
  sprintf(bwText, "F%s", getCurrentBandwidth()->desc);
else
  sprintf(bwText, "F%sHz", getCurrentBandwidth()->desc);

spr.setTextColor(TFT_WHITE);
spr.setTextDatum(MC_DATUM);
spr.drawString(bwText, 175, 10, 2);

// ----- Band name -----
spr.setTextColor(TFT_WHITE);
spr.setTextDatum(MC_DATUM);
spr.drawString(getCurrentBand()->bandName, 228, 10, 2);

// ----- Step -----
spr.setTextColor(TFT_WHITE);
spr.setTextDatum(MC_DATUM);
spr.drawString(getCurrentStep()->desc, 273, 10, 2);

// ----- Draw BLT+Wifi -----
drawPixelIcon(295, 3, bleIcon, 13, TFT_WHITE);
drawPixelIcon(303, 6, wifiIcon, 8, TFT_WHITE);

spr.setTextColor(TFT_WHITE);

// =====================
// FM
// =====================
if (currentMode == FM)
{
  spr.setTextDatum(MR_DATUM);
  spr.drawFloat(currentFrequency / 100.0, 2, 262, 46, 7);

  spr.setTextDatum(MC_DATUM);
  spr.drawString("MHz", 289, 66, 4);
}

// =====================
// AIR
// =====================
else if (bandIdx == 2)
{
  uint32_t displayFreq;

  if (currentAirSpacing == AIR_833 &&
      (
        currentDCVIdx == 2 ||
        (currentDCVIdx == 1 && currentFrequency >= 18000)
      ))
  {
    displayFreq = currentAirChannel;
  }
  else
  {
    displayFreq = currentFrequency;

    if (currentDCVIdx == 1)
      displayFreq += 100000;
    else if (currentDCVIdx == 2)
      displayFreq += 110000;
  }

  char freqText[12];

  sprintf(
    freqText,
    "%lu.%03lu",
    displayFreq / 1000,
    displayFreq % 1000
  );

  spr.setTextDatum(MR_DATUM);
  spr.drawString(freqText, 262, 46, 7);

  spr.setTextDatum(MC_DATUM);
  spr.drawString("MHz", 289, 66, 4);
}

// =====================
// HF / AM / SSB
// =====================
else
{
  uint32_t freqHz;

  if (isSSB())
    freqHz = currentFrequency * 1000 + currentBFO;
  else
    freqHz = currentFrequency * 1000;

  // Large kHz part
  spr.setTextDatum(MR_DATUM);
  spr.drawNumber(freqHz / 1000, 262, 46, 7);

  // Small .xxx part
  char fracText[8];
  sprintf(fracText, ".%03lu", freqHz % 1000);

  spr.setTextDatum(ML_DATUM);
  spr.drawString(fracText, 267, 61, 4);

  // kHz opposite .xxx
  spr.setTextDatum(MC_DATUM);
  spr.drawString("kHz", 292, 39, 4);
}

spr.setTextColor(TFT_WHITE);
spr.setTextDatum(TL_DATUM);

// ===== SIGNAL INFO =====

char sigText[16];
char snrText[16];
char volText[16];

int strength = getStrength(rssi);

const char *strengthText[] =
{
  "",
  "S0", "S1", "S2", "S3", "S4", "S5", "S6", "S7", "S8",
  "S9", "S9+10", "S9+20", "S9+30", "S9+40", "S9+50",
  "S9+60", "Peak"
};

if (strength < 1 || strength > 17)
  strength = 1;

snprintf(sigText, sizeof(sigText), "SIG:%s", strengthText[strength]);
snprintf(snrText, sizeof(snrText), "SNR:%udB", snr);

if (muteOn(MUTE_MAIN, 2))
  snprintf(volText, sizeof(volText), "Vol:Muted");
else
  snprintf(volText, sizeof(volText), "Vol:%u", volume);
  
spr.setTextColor(TFT_WHITE);
spr.setTextDatum(TL_DATUM);
spr.drawString(sigText, 5, 21, 2);
spr.drawString(snrText, 5, 38, 2);
spr.drawString(volText, 5, 55, 2);

spr.setTextColor(TFT_WHITE);
spr.setTextDatum(TL_DATUM);

char agcText[16];
char attText[16];
char sqlText[20];
char rssiText[20];

// AGC
if (agcIdx == 0)
  snprintf(agcText, sizeof(agcText), "AGC:ON");
else
  snprintf(agcText, sizeof(agcText), "AGC:OFF");

// ATT
if (agcIdx == 0)
  snprintf(attText, sizeof(attText), "ATT:OFF");
else
  snprintf(attText, sizeof(attText), "ATT:%d", agcNdx);

// SQL
uint8_t sqlRaw   = currentSquelch[currentMode];
uint8_t sqlValue = sqlRaw & 0x7F;
bool sqlSNR      = sqlRaw & 0x80;

if (sqlValue == 0)
  snprintf(sqlText, sizeof(sqlText), "SQL:OFF");
else if (sqlSNR)
  snprintf(sqlText, sizeof(sqlText), "SQL:%udB", sqlValue);
else
  snprintf(sqlText, sizeof(sqlText), "SQL:%udBuV", sqlValue);

// RSSI
snprintf(rssiText, sizeof(rssiText), "RSSI:%udBuV", rssi);

// Draw
spr.drawString(agcText,   79, 125, 2);
spr.drawString(attText,   79, 148, 2);
spr.drawString(sqlText,  160, 125, 2);
spr.drawString(rssiText, 160, 148, 2);

// Saving data
if (prefsAreWritten())
  drawPixelIcon(258, 122, saveIcon, 12, TFT_WHITE);

// Voltage
drawBattery(0, 0);
}
