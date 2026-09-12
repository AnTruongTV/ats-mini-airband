#include "Common.h"
#include "Themes.h"
#include "Utils.h"
#include "Menu.h"
#include "Draw.h"
#include "Storage.h"
#include "BleMode.h"

static void formatBandEdge(char *buf, size_t len, uint32_t freq)
{
    // FM frequencies are stored in 10 kHz units:
    // 8750 -> 87.50 MHz
    if (currentMode == FM)
    {
        snprintf(
            buf,
            len,
            "%lu.%02lu",
            freq / 100,
            freq % 100
        );
    }

    // AIR display frequency is handled in kHz:
    // 118000 -> 118.000 MHz
    else if (bandIdx == 2)
    {
        snprintf(
            buf,
            len,
            "%lu.%02lu",
            freq / 1000,
            (freq % 1000) / 10
        );
    }

    // AM / SW / SSB: show normal kHz values
    else
    {
        snprintf(buf, len, "%lu", freq);
    }
}


static void drawNewBandScale()
{
    // ============================================================
    // Exact coordinates from the UI mockup
    // ============================================================

    constexpr int LEFT_FREQ_X     = 75;
    constexpr int FREQ_Y          = 103;

    // These are the CENTRES of the two scale-end circles
    constexpr int SCALE_LEFT_X    = 122;
    constexpr int SCALE_RIGHT_X   = 268;
    constexpr int SCALE_Y         = 111;

    constexpr int DOT_START_X     = 126;
    constexpr int DOT_END_X       = 264;

    constexpr int RIGHT_FREQ_X    = 274;

    // ============================================================
    // Current band information
    // ============================================================

    const Band *band = getCurrentBand();

    uint32_t displayFreq = currentFrequency;
    uint32_t displayMin  = band->minimumFreq;
    uint32_t displayMax  = band->maximumFreq;

    // ============================================================
    // AIR / DCV
    // ============================================================

    if (bandIdx == 2)
    {
        uint32_t dcvOffset = 0;

        if (currentDCVIdx == 1)
            dcvOffset = 100000;

        else if (currentDCVIdx == 2)
            dcvOffset = 110000;

        // -----------------------------
        // Frequency shown to the user
        // -----------------------------

        if (
            currentAirSpacing == AIR_833 &&
            (
                currentDCVIdx == 2 ||
                (currentDCVIdx == 1 && currentFrequency >= 18000)
            )
        )
        {
            // Keep scale marker aligned with displayed
            // 8.33 kHz channel designator
            displayFreq = currentAirChannel;
        }
        else
        {
            displayFreq = currentFrequency + dcvOffset;
        }

        // -----------------------------
        // Visible band limits
        // -----------------------------

        displayMin = band->minimumFreq + dcvOffset;
        displayMax = band->maximumFreq + dcvOffset;

        // 110 MHz DCV hardware limit:
        // raw tuner max 27 MHz -> displayed 137 MHz
        if (currentDCVIdx == 2)
            displayMax = 137000;
    }

    // ============================================================
    // SSB BFO correction
    // ============================================================

    else if (isSSB())
    {
        // Scale is in kHz, so keep the fractional BFO only for
        // position calculation.
        int64_t correctedHz =
            (int64_t)currentFrequency * 1000 +
            currentBFO;

        if (correctedHz < 0)
            correctedHz = 0;

        displayFreq = correctedHz / 1000;
    }

    // ============================================================
    // Safety
    // ============================================================

    if (displayMax <= displayMin)
        return;

    uint32_t markerFreq = displayFreq;

    if (markerFreq < displayMin)
        markerFreq = displayMin;

    if (markerFreq > displayMax)
        markerFreq = displayMax;

    // ============================================================
    // Format edge labels
    // ============================================================

    char leftText[16];
    char rightText[16];

    formatBandEdge(leftText, sizeof(leftText), displayMin);
    formatBandEdge(rightText, sizeof(rightText), displayMax);

    spr.setTextColor(TFT_WHITE);

    // Left edge frequency
    spr.setTextDatum(TL_DATUM);
    spr.drawString(leftText, LEFT_FREQ_X, FREQ_Y, 2);

    // Right edge frequency
    spr.setTextDatum(TL_DATUM);
    spr.drawString(rightText, RIGHT_FREQ_X, FREQ_Y, 2);

    // ============================================================
    // Endpoint circles
    // ============================================================

    spr.drawCircle(
        SCALE_LEFT_X,
        SCALE_Y,
        2,
        TFT_WHITE
    );

    spr.drawCircle(
        SCALE_RIGHT_X,
        SCALE_Y,
        2,
        TFT_WHITE
    );

    // ============================================================
    // Dotted scale
    // ============================================================

    for (int x = DOT_START_X; x <= DOT_END_X; x += 3)
    {
        spr.drawPixel(
            x,
            SCALE_Y,
            TFT_WHITE
        );
    }

    // ============================================================
    // Frequency -> scale position
    //
    // Integer math instead of float, so this is lightweight on ESP32.
    // ============================================================

    uint32_t range =
        displayMax - displayMin;

    uint32_t offset =
        markerFreq - displayMin;

    int markerX =
        SCALE_LEFT_X +
        ((uint64_t)offset *
         (SCALE_RIGHT_X - SCALE_LEFT_X)) /
        range;

    // ============================================================
    // Current-frequency marker
    // ============================================================

    spr.fillCircle(
    markerX,
    SCALE_Y,
    2,
    TFT_RED
    );

    // Restore common datum
    spr.setTextDatum(TL_DATUM);
}

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

uint16_t signalColor(int strength)
{
  switch (strength)
  {
    case 1:  return TFT_GREEN;   // S0
    case 2:  return spr.color565(64, 255, 0);    // S1
    case 3:  return spr.color565(128, 255, 0);   // S2
    case 4:  return spr.color565(180, 255, 0);   // S3 yellow-green
    case 5:  return spr.color565(220, 255, 0);   // S4
    case 6:  return spr.color565(255, 255, 0);   // S5
    case 7:  return spr.color565(255, 220, 0);   // S6 lime/yellow
    case 8:  return spr.color565(255, 190, 0);   // S7
    case 9:  return spr.color565(255, 160, 0);   // S8 yellow
    case 10: return spr.color565(255, 128, 0);   // S9 yellow/orange
    case 11: return spr.color565(255, 96, 0);    // S9+10
    case 12: return spr.color565(255, 64, 0);    // S9+20 orange
    case 13: return spr.color565(255, 40, 0);    // S9+30
    case 14: return spr.color565(255, 24, 0);    // S9+40 orange-red
    case 15: return spr.color565(255, 12, 0);    // S9+50
    case 16: return TFT_RED;                     // S9+60
    case 17: return TFT_RED;                     // Peak
    default: return TFT_WHITE;
  }
}

void drawSignalBars(int strength)
{
  constexpr int BAR_COUNT = 16;

  constexpr int START_X = 3;
  constexpr int BASE_Y  = 33;

  constexpr int BAR_W   = 3;
  constexpr int BAR_GAP = 1;

  if (strength < 1)
    strength = 1;

  // Peak = full 16 bars
  int litBars = strength;
  if (litBars > 16)
    litBars = 16;

  static const uint8_t barHeights[BAR_COUNT] =
  {
     4,  8, 12, 16,
    20, 24, 28, 32,
    36, 36, 36, 36,
    36, 36, 36, 36
  };

  for (int i = 0; i < BAR_COUNT; i++)
  {
    int x = START_X + i * (BAR_W + BAR_GAP);
    int h = barHeights[i];
    int y = BASE_Y - h;

    if (i < litBars)
    {
      spr.fillRect(
        x,
        y,
        BAR_W,
        h,
        signalColor(i + 1)
      );
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

const char *batteryShell[] =
{
  "..111111111111111111111111111111111111111111.....",
  ".11111111111111111111111111111111111111111111....",
  "11..........................................11...",
  "11..........................................11...",
  "11..........................................11111",
  "11..........................................11111",
  "11..........................................11111",
  "11..........................................11111",
  "11..........................................11111",
  "11..........................................11111",
  "11..........................................11111",
  "11..........................................11111",
  "11..........................................11111",
  "11..........................................11111",
  "11..........................................11...",
  "11..........................................11...",
  ".11111111111111111111111111111111111111111111....",
  "..111111111111111111111111111111111111111111....."
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

spr.drawFastHLine(0, 38, 72, TFT_WHITE);
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
if (bandIdx == 2)
{
  if (currentAirSpacing == AIR_833)
    spr.drawString("8.33k", 273, 10, 2);
  else
    spr.drawString("25k", 272, 10, 2);
}
else
{
  spr.drawString(getCurrentStep()->desc, 272, 10, 2);
}

// ----- Draw BLT+Wifi -----
// BLE
int8_t bleStatus = getBleStatus();

if (bleStatus)
{
  uint16_t bleColor = bleStatus > 0 ? TFT_BLUE : TFT_RED;
  drawPixelIcon(295, 3, bleIcon, 13, bleColor);
}

int8_t wifiStatus = getWiFiStatus();

if (wifiStatus)
{
  uint16_t wifiColor = wifiStatus > 0 ? TFT_GREEN : TFT_RED;
  drawPixelIcon(303, 6, wifiIcon, 8, wifiColor);
}

// =====================
// FM
// =====================
if (currentMode == FM)
{
  spr.setTextDatum(MR_DATUM);
  spr.drawFloat(currentFrequency / 100.0, 2, 262, 46, 7);

  spr.setTextDatum(MC_DATUM);
  spr.drawString("MHz", 289, 61, 4);
}

// =====================
// AIR
// =====================
else if (bandIdx == 2)
{
  uint32_t displayFreq;

 if (currentDCVIdx == 1 || currentDCVIdx == 2)
{
    const char *dcvText =
        (currentDCVIdx == 1) ? "100" : "110";

    constexpr int DCV_X = 280;
    constexpr int DCV_Y = 26;
    constexpr int DCV_W = 27;
    constexpr int DCV_H = 16;

    // Exact rounded outline from mockup
    spr.drawRoundRect(
        DCV_X,
        DCV_Y,
        DCV_W,
        DCV_H,
        3,
        TFT_WHITE
    );

    spr.setTextColor(TFT_WHITE);
    spr.setTextDatum(MC_DATUM);

    // Font 2
    spr.drawString(
        dcvText,
        293,
        34,
        2
    );
}
  
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
  spr.drawString("MHz", 289, 61, 4);
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

// =====================
// STATION / CHANNEL NAME
// =====================

if (bandIdx != 2)
{
    const char *stationName = getStationName();

    if (stationName && stationName[0] != '\0')
    {
        spr.setTextColor(TFT_WHITE);
        spr.setTextDatum(TC_DATUM);

        // 0xFF means a long EiBi station name
        if ((uint8_t)stationName[0] == 0xFF)
        {
            spr.drawString(
                stationName + 1,
                196,   // center of usable right-side area
                80,
                2
            );
        }
        else
        {
            int font = 4;

            // Fall back to smaller font if name is too wide
            if (spr.textWidth(stationName, font) > 238)
                font = 2;

            spr.drawString(
                stationName,
                196,
                77,
                font
            );
        }
    }
}

drawNewBandScale();
  
spr.setTextColor(TFT_WHITE);
spr.setTextDatum(TL_DATUM);

// ===== SIGNAL INFO =====

char sigText[16];
char snrText[16];
char volText[16];

int strength = getStrength(rssi);

if (strength < 1 || strength > 17)
  strength = 1;

snprintf(snrText, sizeof(snrText), "SNR: %udB", snr);

if (muteOn(MUTE_MAIN, 2))
  snprintf(volText, sizeof(volText), "Vol: Muted");
else
  snprintf(volText, sizeof(volText), "Vol: %u", volume);

spr.setTextColor(TFT_WHITE);
spr.setTextDatum(TL_DATUM);

spr.drawString(snrText, 5, 41, 2);
spr.drawString(volText, 5, 55, 2);
drawSignalBars(strength);
spr.setTextColor(TFT_WHITE);
spr.setTextDatum(TL_DATUM);

char agcAttText[16];
char avcText[16];
char sqlText[16];
char rssiText[20];

// AGC / ATT combined
if (agcIdx == 0)
  snprintf(agcAttText, sizeof(agcAttText), "AGC:ON");
else
  snprintf(agcAttText, sizeof(agcAttText), "ATT:%d", agcNdx);

// AVC
if (currentMode == FM)
  snprintf(avcText, sizeof(avcText), "AVC:n/a");
else
  snprintf(avcText, sizeof(avcText), "AVC:%ddB",
           isSSB() ? SsbAvcIdx : AmAvcIdx);

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
spr.setTextColor(TFT_WHITE);
spr.setTextDatum(TL_DATUM);

spr.drawString(agcAttText, 79, 125, 2);
spr.drawString(avcText,    79, 148, 2);

spr.drawString(sqlText,   160, 125, 2);
spr.drawString(rssiText,  160, 148, 2);

// Saving icon
if (prefsAreWritten())
  drawPixelIcon(258, 122, saveIcon, 12, TFT_RED);

// Voltage
drawBattery(0, 0);

// Draw battery
drawPixelIcon(263, 144, batteryShell, 18, TFT_WHITE);

// Menu
drawSideBar(currentCmd, 0, 73, 0);
}
