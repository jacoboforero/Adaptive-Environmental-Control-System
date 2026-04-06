#include <Bounce2.h>
#include <DHT.h>
#include <EEPROM.h>
#include <LiquidCrystal.h>
#include <Servo.h>

// Library Manager dependencies:
// - Bounce2
// - DHT sensor library by Adafruit
// - Adafruit Unified Sensor

enum SystemMode { MODE_AUTO, MODE_MANUAL, MODE_ECO };
enum UiState { UI_BROWSE, UI_EDIT };
enum MenuItem {
  MENU_DASHBOARD,
  MENU_SENSORS,
  MENU_MODE,
  MENU_LIGHT_THRESHOLD,
  MENU_AUTO_TEMP,
  MENU_ECO_TEMP,
  MENU_ALARM,
  MENU_LED1,
  MENU_LED2,
  MENU_LED3,
  MENU_FAN,
  MENU_GATE,
  MENU_BUZZER_TEST,
  MENU_COUNT
};
enum JoyDirection { JOY_NEUTRAL, JOY_UP, JOY_DOWN };

struct Settings {
  uint8_t version;
  uint8_t mode;
  uint16_t lightThreshold;
  uint8_t autoTempThreshold;
  uint8_t ecoTempThreshold;
  uint8_t alarmEnabled;
  uint8_t manualLedMask;
  uint8_t manualFanOn;
  uint8_t manualGateOpen;
};

struct OutputState {
  bool leds[3];
  bool fanOn;
  bool gateOpen;
  bool buzzerOn;
};

const uint8_t PIN_LCD_RS = 12;
const uint8_t PIN_LCD_E = 11;
const uint8_t PIN_LCD_D4 = 5;
const uint8_t PIN_LCD_D5 = 4;
const uint8_t PIN_LCD_D6 = 3;
const uint8_t PIN_LCD_D7 = 2;

const uint8_t PIN_SELECT = 6;
const uint8_t PIN_BACK = 7;
const uint8_t PIN_LED_1 = 8;
const uint8_t PIN_LED_2 = 9;
const uint8_t PIN_DHT = 10;
const uint8_t PIN_PIR = 13;

const uint8_t PIN_JOYSTICK_Y = A0;
const uint8_t PIN_LDR = A1;
const uint8_t PIN_SERVO = A2;
const uint8_t PIN_FAN = A3;
const uint8_t PIN_LED_3 = A4;
const uint8_t PIN_BUZZER = A5;

const uint8_t SETTINGS_VERSION = 1;
const int EEPROM_ADDRESS = 0;

const unsigned long LCD_REFRESH_MS = 200UL;
const unsigned long LCD_RECOVERY_REFRESH_MS = 2000UL;
const unsigned long DHT_REFRESH_MS = 2000UL;
const unsigned long MOTION_HOLD_MS = 20000UL;
const unsigned long BUZZER_PULSE_MS = 250UL;
const unsigned long BUZZER_TEST_PULSE_MS = 900UL;
const unsigned long BUZZER_TEST_MESSAGE_MS = 1200UL;
const unsigned long GATE_OPEN_MS = 5000UL;
const unsigned long SERVO_SETTLE_MS = 350UL;

const uint8_t JOYSTICK_CENTER_SAMPLES = 12;
const uint8_t JOYSTICK_READ_SAMPLES = 4;
const uint8_t JOYSTICK_STABLE_READS = 3;
const int JOYSTICK_ACTIVE_OFFSET = 260;
const int JOYSTICK_NEUTRAL_DEADBAND = 120;
const uint16_t LIGHT_THRESHOLD_MIN = 260;
const uint16_t LIGHT_THRESHOLD_MAX = 900;
const uint8_t TEMPERATURE_THRESHOLD_MIN = 15;
const uint8_t TEMPERATURE_THRESHOLD_MAX = 45;
const uint8_t GATE_CLOSED_ANGLE = 10;
const uint8_t GATE_OPEN_ANGLE = 90;

const char *const MENU_LABELS[MENU_COUNT] = {
  "Dashboard",
  "Sensors",
  "Mode",
  "Light Thr",
  "Auto Temp",
  "Eco Temp",
  "Alarm",
  "LED 1",
  "LED 2",
  "LED 3",
  "Fan",
  "Gate",
  "Buzzer Test"
};

LiquidCrystal lcd(PIN_LCD_RS, PIN_LCD_E, PIN_LCD_D4, PIN_LCD_D5, PIN_LCD_D6, PIN_LCD_D7);
DHT dht(PIN_DHT, DHT11);
Servo gateServo;
Bounce2::Button selectButton;
Bounce2::Button backButton;

Settings settings;
Settings editSettings;
OutputState outputs;

UiState uiState = UI_BROWSE;
MenuItem currentMenu = MENU_DASHBOARD;
JoyDirection lastJoyDirection = JOY_NEUTRAL;
int joystickCenter = 512;
uint8_t joystickStableReads = 0;
bool joystickLatched = false;

int lightLevel = 0;
bool pirDetected = false;
bool previousPirDetected = false;
float temperatureC = 0.0f;
float humidityPercent = 0.0f;
bool hasDhtReading = false;
bool dhtLastReadOk = false;

unsigned long lastMotionAtMs = 0;
unsigned long lastDhtReadAtMs = 0;
unsigned long lastDisplayRefreshMs = 0;
unsigned long lastDisplayRecoveryMs = 0;
unsigned long buzzerUntilMs = 0;
unsigned long buzzerTestMessageUntilMs = 0;
unsigned long gateOpenUntilMs = 0;
unsigned long servoDetachAtMs = 0;

bool forceDisplayRefresh = true;
bool buzzerDriveActive = false;
bool servoAttached = false;
bool lastServoTargetOpen = false;
char lastRenderedLine1[17] = "";
char lastRenderedLine2[17] = "";

void setDefaultSettings(Settings &target) {
  target.version = SETTINGS_VERSION;
  target.mode = MODE_AUTO;
  target.lightThreshold = 450;
  target.autoTempThreshold = 28;
  target.ecoTempThreshold = 31;
  target.alarmEnabled = 1;
  target.manualLedMask = 0;
  target.manualFanOn = 0;
  target.manualGateOpen = 0;
}

void sanitizeSettings(Settings &target) {
  if (target.mode > MODE_ECO) {
    target.mode = MODE_AUTO;
  }

  target.lightThreshold = constrain(target.lightThreshold, LIGHT_THRESHOLD_MIN, LIGHT_THRESHOLD_MAX);
  target.autoTempThreshold = constrain(target.autoTempThreshold, TEMPERATURE_THRESHOLD_MIN, TEMPERATURE_THRESHOLD_MAX);
  target.ecoTempThreshold = constrain(target.ecoTempThreshold, TEMPERATURE_THRESHOLD_MIN, TEMPERATURE_THRESHOLD_MAX);
  target.alarmEnabled = target.alarmEnabled ? 1 : 0;
  target.manualLedMask &= 0x07;
  target.manualFanOn = target.manualFanOn ? 1 : 0;
  target.manualGateOpen = target.manualGateOpen ? 1 : 0;

  if (target.ecoTempThreshold < target.autoTempThreshold) {
    target.ecoTempThreshold = target.autoTempThreshold;
  }
}

void saveSettings() {
  settings.version = SETTINGS_VERSION;
  sanitizeSettings(settings);
  EEPROM.put(EEPROM_ADDRESS, settings);
}

void loadSettings() {
  EEPROM.get(EEPROM_ADDRESS, settings);

  if (settings.version != SETTINGS_VERSION) {
    setDefaultSettings(settings);
    saveSettings();
    return;
  }

  sanitizeSettings(settings);
}

void applyStartupDefaults() {
  settings.mode = MODE_AUTO;
  settings.manualFanOn = 0;
  sanitizeSettings(settings);
  saveSettings();
}

SystemMode currentMode() {
  return static_cast<SystemMode>(settings.mode);
}

const char *modeName(SystemMode mode) {
  switch (mode) {
    case MODE_AUTO:
      return "AUTO";
    case MODE_MANUAL:
      return "MAN";
    case MODE_ECO:
      return "ECO";
  }

  return "AUTO";
}

const char *alarmStatusLabel(bool enabled) {
  return enabled ? "Enabled" : "Disabled";
}

const char *onOffLabel(bool enabled) {
  return enabled ? "ON" : "OFF";
}

const char *gateStatusLabel(bool open) {
  return open ? "OPEN" : "CLOSED";
}

bool menuItemEditable(MenuItem item) {
  switch (item) {
    case MENU_MODE:
    case MENU_LIGHT_THRESHOLD:
    case MENU_AUTO_TEMP:
    case MENU_ECO_TEMP:
    case MENU_ALARM:
    case MENU_LED1:
    case MENU_LED2:
    case MENU_LED3:
    case MENU_FAN:
    case MENU_GATE:
      return true;
    case MENU_DASHBOARD:
    case MENU_SENSORS:
    case MENU_BUZZER_TEST:
    case MENU_COUNT:
      return false;
  }

  return false;
}

void setManualLedBit(uint8_t index, bool enabled, Settings &target) {
  if (enabled) {
    target.manualLedMask |= (1 << index);
  } else {
    target.manualLedMask &= ~(1 << index);
  }
}

bool getManualLedBit(uint8_t index, const Settings &target) {
  return (target.manualLedMask & (1 << index)) != 0;
}

void triggerBuzzer(unsigned long now, unsigned long durationMs) {
  unsigned long until = now + durationMs;

  if (until > buzzerUntilMs) {
    buzzerUntilMs = until;
  }
}

void openGateFor(unsigned long now, unsigned long durationMs) {
  unsigned long until = now + durationMs;

  if (until > gateOpenUntilMs) {
    gateOpenUntilMs = until;
  }
}

void beginEdit() {
  editSettings = settings;
  uiState = UI_EDIT;
  forceDisplayRefresh = true;
}

void confirmEdit() {
  settings = editSettings;
  sanitizeSettings(settings);
  saveSettings();
  uiState = UI_BROWSE;
  forceDisplayRefresh = true;
}

void cancelEdit() {
  uiState = UI_BROWSE;
  forceDisplayRefresh = true;
}

void applyEditDelta(int delta) {
  if (delta == 0) {
    return;
  }

  switch (currentMenu) {
    case MENU_MODE: {
      int nextMode = editSettings.mode + delta;

      if (nextMode > MODE_ECO) {
        nextMode = MODE_AUTO;
      } else if (nextMode < MODE_AUTO) {
        nextMode = MODE_ECO;
      }

      editSettings.mode = nextMode;
      break;
    }
    case MENU_LIGHT_THRESHOLD: {
      int nextThreshold = static_cast<int>(editSettings.lightThreshold) + (delta * 10);
      editSettings.lightThreshold = constrain(nextThreshold, LIGHT_THRESHOLD_MIN, LIGHT_THRESHOLD_MAX);
      break;
    }
    case MENU_AUTO_TEMP: {
      int nextThreshold = static_cast<int>(editSettings.autoTempThreshold) + delta;
      editSettings.autoTempThreshold = constrain(nextThreshold, TEMPERATURE_THRESHOLD_MIN, TEMPERATURE_THRESHOLD_MAX);

      if (editSettings.ecoTempThreshold < editSettings.autoTempThreshold) {
        editSettings.ecoTempThreshold = editSettings.autoTempThreshold;
      }
      break;
    }
    case MENU_ECO_TEMP: {
      int nextThreshold = static_cast<int>(editSettings.ecoTempThreshold) + delta;
      editSettings.ecoTempThreshold = constrain(nextThreshold, TEMPERATURE_THRESHOLD_MIN, TEMPERATURE_THRESHOLD_MAX);

      if (editSettings.ecoTempThreshold < editSettings.autoTempThreshold) {
        editSettings.ecoTempThreshold = editSettings.autoTempThreshold;
      }
      break;
    }
    case MENU_ALARM:
      editSettings.alarmEnabled = delta > 0 ? 1 : 0;
      break;
    case MENU_LED1:
      setManualLedBit(0, delta > 0, editSettings);
      break;
    case MENU_LED2:
      setManualLedBit(1, delta > 0, editSettings);
      break;
    case MENU_LED3:
      setManualLedBit(2, delta > 0, editSettings);
      break;
    case MENU_FAN:
      editSettings.manualFanOn = delta > 0 ? 1 : 0;
      break;
    case MENU_GATE:
      editSettings.manualGateOpen = delta > 0 ? 1 : 0;
      break;
    case MENU_DASHBOARD:
    case MENU_SENSORS:
    case MENU_BUZZER_TEST:
    case MENU_COUNT:
      break;
  }

  forceDisplayRefresh = true;
}

JoyDirection readJoystickDirection() {
  long total = 0;

  for (uint8_t i = 0; i < JOYSTICK_READ_SAMPLES; ++i) {
    total += analogRead(PIN_JOYSTICK_Y);
  }

  int value = total / JOYSTICK_READ_SAMPLES;
  int delta = value - joystickCenter;

  if (delta <= -JOYSTICK_ACTIVE_OFFSET) {
    return JOY_UP;
  }

  if (delta >= JOYSTICK_ACTIVE_OFFSET) {
    return JOY_DOWN;
  }

  if (abs(delta) <= JOYSTICK_NEUTRAL_DEADBAND) {
    return JOY_NEUTRAL;
  }

  return JOY_NEUTRAL;
}

void moveMenuSelection(int delta) {
  int nextItem = static_cast<int>(currentMenu) + delta;

  if (nextItem >= MENU_COUNT) {
    nextItem = MENU_DASHBOARD;
  } else if (nextItem < MENU_DASHBOARD) {
    nextItem = MENU_COUNT - 1;
  }

  currentMenu = static_cast<MenuItem>(nextItem);
  forceDisplayRefresh = true;
}

void handleJoystick(unsigned long now) {
  JoyDirection direction = readJoystickDirection();

  if (direction != lastJoyDirection) {
    lastJoyDirection = direction;
    joystickStableReads = 1;
  } else if (joystickStableReads < 255) {
    joystickStableReads++;
  }

  if (direction == JOY_NEUTRAL) {
    joystickLatched = false;
    return;
  }

  if (joystickLatched || joystickStableReads < JOYSTICK_STABLE_READS) {
    return;
  }

  if (uiState == UI_BROWSE) {
    moveMenuSelection(direction == JOY_UP ? -1 : 1);
  } else {
    applyEditDelta(direction == JOY_UP ? 1 : -1);
  }

  joystickLatched = true;
}

void handleSelectPressed(unsigned long now) {
  if (uiState == UI_EDIT) {
    confirmEdit();
    return;
  }

  if (currentMenu == MENU_BUZZER_TEST) {
    triggerBuzzer(now, BUZZER_TEST_PULSE_MS);
    buzzerTestMessageUntilMs = now + BUZZER_TEST_MESSAGE_MS;
    forceDisplayRefresh = true;
    return;
  }

  if (menuItemEditable(currentMenu)) {
    beginEdit();
  }
}

void handleBackPressed() {
  if (uiState == UI_EDIT) {
    cancelEdit();
    return;
  }

  if (currentMenu != MENU_DASHBOARD) {
    currentMenu = MENU_DASHBOARD;
    forceDisplayRefresh = true;
  }
}

void updateButtons(unsigned long now) {
  selectButton.update();
  backButton.update();

  if (selectButton.pressed()) {
    handleSelectPressed(now);
  }

  if (backButton.pressed()) {
    handleBackPressed();
  }
}

void handlePirAlert(unsigned long now) {
  if (currentMode() == MODE_MANUAL || !settings.alarmEnabled) {
    return;
  }

  triggerBuzzer(now, BUZZER_PULSE_MS);
  openGateFor(now, GATE_OPEN_MS);
}

void readSensors(unsigned long now) {
  lightLevel = analogRead(PIN_LDR);

  pirDetected = digitalRead(PIN_PIR) == HIGH;
  if (pirDetected) {
    lastMotionAtMs = now;
  }

  if (pirDetected && !previousPirDetected) {
    handlePirAlert(now);
  }
  previousPirDetected = pirDetected;

  if (lastDhtReadAtMs == 0 || (now - lastDhtReadAtMs) >= DHT_REFRESH_MS) {
    float nextHumidity = dht.readHumidity();
    float nextTemperature = dht.readTemperature();

    lastDhtReadAtMs = now;
    if (!isnan(nextHumidity) && !isnan(nextTemperature)) {
      humidityPercent = nextHumidity;
      temperatureC = nextTemperature;
      hasDhtReading = true;
      dhtLastReadOk = true;
    } else {
      dhtLastReadOk = false;
    }

    forceDisplayRefresh = true;
  }
}

bool hasRecentMotion(unsigned long now) {
  return pirDetected || (lastMotionAtMs != 0 && (now - lastMotionAtMs) <= MOTION_HOLD_MS);
}

uint8_t computeLightingZones(unsigned long now) {
  if (!hasRecentMotion(now) || lightLevel >= settings.lightThreshold) {
    return 0;
  }

  int threshold = static_cast<int>(settings.lightThreshold);
  int zones = 1;

  if (lightLevel < threshold - 120) {
    zones = 2;
  }
  if (lightLevel < threshold - 240) {
    zones = 3;
  }

  if (currentMode() == MODE_ECO && zones > 2) {
    zones = 2;
  }

  return zones;
}

bool fanShouldRun() {
  if (!hasDhtReading) {
    return false;
  }

  uint8_t threshold = currentMode() == MODE_ECO ? settings.ecoTempThreshold : settings.autoTempThreshold;
  return temperatureC >= threshold;
}

void runControlLogic(unsigned long now) {
  outputs.leds[0] = false;
  outputs.leds[1] = false;
  outputs.leds[2] = false;
  outputs.fanOn = false;
  outputs.gateOpen = false;
  outputs.buzzerOn = buzzerUntilMs != 0 && now < buzzerUntilMs;

  if (currentMode() == MODE_MANUAL) {
    outputs.leds[0] = getManualLedBit(0, settings);
    outputs.leds[1] = getManualLedBit(1, settings);
    outputs.leds[2] = getManualLedBit(2, settings);
    outputs.fanOn = settings.manualFanOn;
    outputs.gateOpen = settings.manualGateOpen;
    return;
  }

  uint8_t zones = computeLightingZones(now);
  outputs.leds[0] = zones >= 1;
  outputs.leds[1] = zones >= 2;
  outputs.leds[2] = zones >= 3;
  outputs.fanOn = fanShouldRun();
  outputs.gateOpen = settings.alarmEnabled && gateOpenUntilMs != 0 && now < gateOpenUntilMs;
}

void updateServo(unsigned long now) {
  bool targetOpen = outputs.gateOpen;

  if (!servoAttached || targetOpen != lastServoTargetOpen) {
    if (!servoAttached) {
      gateServo.attach(PIN_SERVO);
      servoAttached = true;
    }

    gateServo.write(targetOpen ? GATE_OPEN_ANGLE : GATE_CLOSED_ANGLE);
    lastServoTargetOpen = targetOpen;
    servoDetachAtMs = now + SERVO_SETTLE_MS;
  }

  if (servoAttached && servoDetachAtMs != 0 && now >= servoDetachAtMs) {
    gateServo.detach();
    servoAttached = false;
    servoDetachAtMs = 0;
  }
}

void applyOutputs(unsigned long now) {
  digitalWrite(PIN_LED_1, outputs.leds[0] ? HIGH : LOW);
  digitalWrite(PIN_LED_2, outputs.leds[1] ? HIGH : LOW);
  digitalWrite(PIN_LED_3, outputs.leds[2] ? HIGH : LOW);
  digitalWrite(PIN_FAN, outputs.fanOn ? HIGH : LOW);

  if (outputs.buzzerOn) {
    if (!buzzerDriveActive) {
      tone(PIN_BUZZER, 2200);
      buzzerDriveActive = true;
    }
  } else if (buzzerDriveActive) {
    noTone(PIN_BUZZER);
    digitalWrite(PIN_BUZZER, LOW);
    buzzerDriveActive = false;
  }

  updateServo(now);
}

uint8_t activeLightCount() {
  uint8_t count = 0;

  for (uint8_t i = 0; i < 3; ++i) {
    if (outputs.leds[i]) {
      ++count;
    }
  }

  return count;
}

void formatTemperatureText(char *buffer, size_t size) {
  if (!hasDhtReading) {
    snprintf(buffer, size, "--");
    return;
  }

  snprintf(buffer, size, "%dC", static_cast<int>(temperatureC + 0.5f));
}

void formatHumidityText(char *buffer, size_t size) {
  if (!hasDhtReading) {
    snprintf(buffer, size, "--");
    return;
  }

  snprintf(buffer, size, "%d%%", static_cast<int>(humidityPercent + 0.5f));
}

const char *dhtStatusLabel() {
  if (dhtLastReadOk) {
    return "OK";
  }

  return hasDhtReading ? "STL" : "ERR";
}

void menuValueLabel(MenuItem item, const Settings &source, char *buffer, size_t size) {
  switch (item) {
    case MENU_MODE:
      snprintf(buffer, size, "%s", modeName(static_cast<SystemMode>(source.mode)));
      break;
    case MENU_LIGHT_THRESHOLD:
      snprintf(buffer, size, "%u", static_cast<unsigned int>(source.lightThreshold));
      break;
    case MENU_AUTO_TEMP:
      snprintf(buffer, size, "%u C", static_cast<unsigned int>(source.autoTempThreshold));
      break;
    case MENU_ECO_TEMP:
      snprintf(buffer, size, "%u C", static_cast<unsigned int>(source.ecoTempThreshold));
      break;
    case MENU_ALARM:
      snprintf(buffer, size, "%s", alarmStatusLabel(source.alarmEnabled));
      break;
    case MENU_LED1:
      snprintf(buffer, size, "%s", onOffLabel(getManualLedBit(0, source)));
      break;
    case MENU_LED2:
      snprintf(buffer, size, "%s", onOffLabel(getManualLedBit(1, source)));
      break;
    case MENU_LED3:
      snprintf(buffer, size, "%s", onOffLabel(getManualLedBit(2, source)));
      break;
    case MENU_FAN:
      snprintf(buffer, size, "%s", onOffLabel(source.manualFanOn));
      break;
    case MENU_GATE:
      snprintf(buffer, size, "%s", gateStatusLabel(source.manualGateOpen));
      break;
    case MENU_BUZZER_TEST: {
      unsigned long now = millis();
      if (now < buzzerTestMessageUntilMs) {
        snprintf(buffer, size, "Test signal sent");
      } else {
        snprintf(buffer, size, "Press Select");
      }
      break;
    }
    case MENU_DASHBOARD:
    case MENU_SENSORS:
    case MENU_COUNT:
      buffer[0] = '\0';
      break;
  }
}

void buildDashboardLines(char *line1, size_t line1Size, char *line2, size_t line2Size) {
  char tempText[6];
  formatTemperatureText(tempText, sizeof(tempText));

  snprintf(line1, line1Size, "%s L%d M%d %s", modeName(currentMode()), activeLightCount(), pirDetected ? 1 : 0, dhtStatusLabel());
  snprintf(line2, line2Size, "T:%-3s F:%d G:%d", tempText, outputs.fanOn ? 1 : 0, outputs.gateOpen ? 1 : 0);
}

void buildSensorLines(char *line1, size_t line1Size, char *line2, size_t line2Size) {
  char tempText[6];
  char humidityText[6];
  formatTemperatureText(tempText, sizeof(tempText));
  formatHumidityText(humidityText, sizeof(humidityText));

  snprintf(line1, line1Size, "L:%4d PIR:%d", lightLevel, pirDetected ? 1 : 0);

  if (hasDhtReading) {
    snprintf(line2, line2Size, "T:%s H:%s%s", tempText, humidityText, dhtLastReadOk ? "" : "*");
  } else {
    snprintf(line2, line2Size, "DHT read error");
  }
}

void buildMenuLines(char *line1, size_t line1Size, char *line2, size_t line2Size) {
  const Settings &source = uiState == UI_EDIT ? editSettings : settings;
  snprintf(line1, line1Size, "%c%s", uiState == UI_EDIT ? '*' : '>', MENU_LABELS[currentMenu]);

  if (currentMenu == MENU_DASHBOARD) {
    buildDashboardLines(line1, line1Size, line2, line2Size);
    return;
  }

  if (currentMenu == MENU_SENSORS) {
    buildSensorLines(line1, line1Size, line2, line2Size);
    return;
  }

  menuValueLabel(currentMenu, source, line2, line2Size);
}

void fillDisplayLine(char *destination, const char *source) {
  memset(destination, ' ', 16);

  size_t length = strlen(source);
  if (length > 16) {
    length = 16;
  }

  memcpy(destination, source, length);
  destination[16] = '\0';
}

void renderDisplay(unsigned long now) {
  if (!forceDisplayRefresh && (now - lastDisplayRefreshMs) < LCD_REFRESH_MS) {
    return;
  }

  char rawLine1[17];
  char rawLine2[17];
  char paddedLine1[17];
  char paddedLine2[17];

  rawLine1[0] = '\0';
  rawLine2[0] = '\0';

  buildMenuLines(rawLine1, sizeof(rawLine1), rawLine2, sizeof(rawLine2));
  fillDisplayLine(paddedLine1, rawLine1);
  fillDisplayLine(paddedLine2, rawLine2);
  bool needsRecoveryRefresh = forceDisplayRefresh || lastDisplayRecoveryMs == 0 || (now - lastDisplayRecoveryMs) >= LCD_RECOVERY_REFRESH_MS;

  if (needsRecoveryRefresh || strncmp(paddedLine1, lastRenderedLine1, 16) != 0) {
    lcd.setCursor(0, 0);
    lcd.print(paddedLine1);
    strncpy(lastRenderedLine1, paddedLine1, sizeof(lastRenderedLine1));
  }

  if (needsRecoveryRefresh || strncmp(paddedLine2, lastRenderedLine2, 16) != 0) {
    lcd.setCursor(0, 1);
    lcd.print(paddedLine2);
    strncpy(lastRenderedLine2, paddedLine2, sizeof(lastRenderedLine2));
  }

  lastDisplayRefreshMs = now;
  if (needsRecoveryRefresh) {
    lastDisplayRecoveryMs = now;
  }
  forceDisplayRefresh = false;
}

void configureButtons() {
  selectButton.attach(PIN_SELECT, INPUT);
  selectButton.interval(25);
  selectButton.setPressedState(HIGH);

  backButton.attach(PIN_BACK, INPUT);
  backButton.interval(25);
  backButton.setPressedState(HIGH);
}

void calibrateJoystick() {
  long total = 0;

  for (uint8_t i = 0; i < JOYSTICK_CENTER_SAMPLES; ++i) {
    total += analogRead(PIN_JOYSTICK_Y);
    delay(5);
  }

  joystickCenter = total / JOYSTICK_CENTER_SAMPLES;

  if (joystickCenter < 150 || joystickCenter > 900) {
    joystickCenter = 512;
  }
}

void configureOutputs() {
  pinMode(PIN_LED_1, OUTPUT);
  pinMode(PIN_LED_2, OUTPUT);
  pinMode(PIN_LED_3, OUTPUT);
  pinMode(PIN_FAN, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_PIR, INPUT);

  digitalWrite(PIN_LED_1, LOW);
  digitalWrite(PIN_LED_2, LOW);
  digitalWrite(PIN_LED_3, LOW);
  digitalWrite(PIN_FAN, LOW);
  digitalWrite(PIN_BUZZER, LOW);
}

void setup() {
  lcd.begin(16, 2);
  lcd.clear();

  configureButtons();
  configureOutputs();
  calibrateJoystick();

  gateServo.attach(PIN_SERVO);
  gateServo.write(GATE_CLOSED_ANGLE);
  servoAttached = true;
  servoDetachAtMs = millis() + SERVO_SETTLE_MS;
  lastServoTargetOpen = false;

  dht.begin();
  loadSettings();
  applyStartupDefaults();

  forceDisplayRefresh = true;
}

void loop() {
  unsigned long now = millis();

  updateButtons(now);
  handleJoystick(now);
  readSensors(now);
  runControlLogic(now);
  applyOutputs(now);
  renderDisplay(now);
}
