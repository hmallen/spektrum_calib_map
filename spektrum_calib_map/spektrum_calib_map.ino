/*
   To Do:
   - Save calibration values to (and load from EEPROM)
*/

#include <CPPM.h>
#include <EEPROM.h>

#define UPDATE_TIMEOUT_SEC 10

#define AILE_EEPROM 0
#define ELEV_EEPROM 6
#define RUDD_EEPROM 12
#define THRO_EEPROM 18
#define GEAR_EEPROM 24
#define AUX1_EEPROM 30

const int ppmPin = 8;

int aile, elev, thro, rudd, gear, aux1;
int aileMap, elevMap, throMap, gearMap, aux1Map;

int aileMin = 9999;
int aileMax = -9999;
int elevMin = 9999;
int elevMax = -9999;
int throMin = 9999;
int throMax = -9999;
int ruddMin = 9999;
int ruddMax = -9999;
int gearMin = 9999;
int gearMax = -9999;
int aux1Min = 9999;
int aux1Max = -9999;

unsigned long updateLast;
const unsigned long updateTimeout = UPDATE_TIMEOUT_SEC * 1000;
bool calibrationComplete = false;

void cppm_cycle(void) {
  if (CPPM.synchronized()) {
    //    // good for DX8-R615X
    //    aile = (CPPM.read(CPPM_AILE) - 1500*2) / 8 * 125 / 128; // aile -100% .. +100%
    //    elev = (CPPM.read(CPPM_ELEV) - 1500*2) / 8 * 125 / 128; // elevator -100% .. +100%
    //    thro = (CPPM.read(CPPM_THRO) - 1500*2) / 8 * 125 / 128; // throttle -100% .. +100%
    //    rudd = (CPPM.read(CPPM_RUDD) - 1500*2) / 8 * 125 / 128; // rudder -100% .. +100%
    //    gear = (CPPM.read(CPPM_GEAR) - 1500*2) / 8 * 125 / 128; // gear -100% .. +100%
    //    aux1 = (CPPM.read(CPPM_AUX1) - 1500*2) / 8 * 125 / 128; // flap -100% .. +100%

    aile = CPPM.read_us(CPPM_AILE) - 1500; // aileron
    elev = CPPM.read_us(CPPM_ELEV) - 1500; // elevator
    thro = CPPM.read_us(CPPM_THRO) - 1500; // throttle
    rudd = CPPM.read_us(CPPM_RUDD) - 1500; // rudder
    gear = CPPM.read_us(CPPM_GEAR) - 1500; // gear
    aux1 = CPPM.read_us(CPPM_AUX1) - 1500; // flap

    /*Serial.print(aile); Serial.print(", ");
      Serial.print(elev); Serial.print(", ");
      Serial.print(thro); Serial.print(", ");
      Serial.print(rudd); Serial.print(", ");
      Serial.print(gear); Serial.print(", ");
      Serial.print(aux1); Serial.print("\n");
      Serial.flush();*/

    if (!calibrationComplete) {
      bool updatePerformed = false;

      /*aileMin = min(aile, aileMin);
        aileMax = max(aile, aileMax);
        elevMin = min(elev, elevMin);
        elevMax = max(elev, elevMax);
        throMin = min(thro, throMin);
        throMax = max(thro, throMax);
        ruddMin = min(rudd, ruddMin);
        ruddMax = max(rudd, ruddMax);
        gearMin = min(gear, gearMin);
        gearMax = max(gear, gearMax);
        aux1Min = min(aux1, aux1Min);
        aux1Max = max(aux1, aux1Max);*/

      // Aileron
      if (aile < aileMin) {
        aileMin = aile;
        updatePerformed = true;
      }
      else if (aile > aileMax) {
        aileMax = aile;
        updatePerformed = true;
      }
      // Elevator
      if (elev < elevMin) {
        elevMin = elev;
        updatePerformed = true;
      }
      else if (elev > elevMax) {
        elevMax = elev;
        updatePerformed = true;
      }
      // Throttle
      if (thro < throMin) {
        throMin = thro;
        updatePerformed = true;
      }
      else if (thro > throMax) {
        throMax = thro;
        updatePerformed = true;
      }
      // Rudder
      if (rudd < ruddMin) {
        ruddMin = rudd;
        updatePerformed = true;
      }
      else if (rudd > ruddMax) {
        ruddMax = rudd;
        updatePerformed = true;
      }
      // Gear
      if (gear < gearMin) {
        gearMin = gear;
        updatePerformed = true;
      }
      else if (gear > gearMax) {
        gearMax = gear;
        updatePerformed = true;
      }
      // Aux1
      if (aux1 < aux1Min) {
        aux1Min = aux1;
        updatePerformed = true;
      }
      else if (aux1 > aux1Max) {
        aux1Max = aux1;
        updatePerformed = true;
      }

      if (updatePerformed) {
        updateLast = millis();

        Serial.print(F("Aileron:  ")); Serial.print(aileMin); Serial.print(F(" | ")); Serial.println(aileMax);
        Serial.print(F("Elevator: ")); Serial.print(elevMin); Serial.print(F(" | ")); Serial.println(elevMax);
        Serial.print(F("Throttle: ")); Serial.print(throMin); Serial.print(F(" | ")); Serial.println(throMax);
        Serial.print(F("Rudder:   ")); Serial.print(ruddMin); Serial.print(F(" | ")); Serial.println(ruddMax);
        Serial.print(F("Gear:     ")); Serial.print(gearMin); Serial.print(F(" | ")); Serial.println(gearMax);
        Serial.print(F("Aux1:     ")); Serial.print(aux1Min); Serial.print(F(" | ")); Serial.println(aux1Max);
        Serial.flush();
      }
    }
  }

  else {
    // if not synchronized, do something...
  }
}

void setup() {
  Serial.begin(115200);

  Serial.println(F("When program begins, move sticks and aux switches/knobs to maxima in every direction."));
  Serial.println(F("Press any key to begin."));
  //delay(5000);
  while (!Serial.available()) {
    delay(1);
  }
  while (Serial.available()) {
    char c = Serial.read();
    delay(5);
  }
  Serial.println(F("Begin moving control sticks."));
  delay(1000);

  CPPM.begin();

  updateLast = millis();

  while ((millis() - updateLast) < updateTimeout) {
    cppm_cycle();
    delay(100);
  }

  Serial.println();
  Serial.println(F("Data capture complete."));
  Serial.println(F("Computing averages."));
  Serial.println();

  int aileMid = (aileMin + aileMax) / 2;
  int elevMid = (elevMin + elevMax) / 2;
  int throMid = (throMin + throMax) / 2;
  int ruddMid = (ruddMin + ruddMax) / 2;
  int gearMid = (gearMin + gearMax) / 2;
  int aux1Mid = (aux1Min + aux1Max) / 2;

  calibrationComplete = true;

  Serial.println(F("            Min - Mid - Max"));
  // Aileron
  Serial.print(F("Aileron:  "));
  Serial.print(aileMin); Serial.print(F(" - "));
  Serial.print(aileMid); Serial.print(F(" - "));
  Serial.println(aileMax);
  // Elevator
  Serial.print(F("Elevator: "));
  Serial.print(elevMin); Serial.print(F(" - "));
  Serial.print(elevMid); Serial.print(F(" - "));
  Serial.print(elevMax);
  // Throttle
  Serial.print(F("Throttle: "));
  Serial.print(throMin); Serial.print(F(" - "));
  Serial.print(throMid); Serial.print(F(" - "));
  Serial.println(throMax);
  // Rudder
  Serial.print(F("Rudder:   "));
  Serial.print(ruddMin); Serial.print(F(" - "));
  Serial.print(ruddMid); Serial.print(F(" - "));
  Serial.println(ruddMax);
  // Gear
  Serial.print(F("Gear:     "));
  Serial.print(gearMin); Serial.print(F(" - "));
  Serial.print(gearMid); Serial.print(F(" - "));
  Serial.println(gearMax);
  // Aux1
  Serial.print(F("Aux1:     "));
  Serial.print(aux1Min); Serial.print(F(" - "));
  Serial.print(aux1Mid); Serial.print(F(" - "));
  Serial.println(aux1Max);

  //Serial.println(F("Generating mapped output table."));

  //

  Serial.println(F("Save values to EEPROM? (y/N)"));

  while (!Serial.available()) {
    delay(1);
  }
  if (Serial.read() == 'y') {
    // Save to EEPROM
    Serial.print(F("Saving to EEPROM..."));
    EEPROM.put(AILE_EEPROM, aileMin);
    EEPROM.put((AILE_EEPROM + 2), aileMid);
    EEPROM.put((AILE_EEPROM + 4), aileMax);
    EEPROM.put(ELEV_EEPROM, elevMin);
    EEPROM.put((ELEV_EEPROM + 2), elevMid);
    EEPROM.put((ELEV_EEPROM + 4), elevMid);
    EEPROM.put(RUDD_EEPROM, ruddMin);
    EEPROM.put((RUDD_EEPROM + 2), ruddMid);
    EEPROM.put((RUDD_EEPROM + 4), ruddMax);
    EEPROM.put(THRO_EEPROM, throMin);
    EEPROM.put((THRO_EEPROM + 2), throMid);
    EEPROM.put((THRO_EEPROM + 4), throMax);
    EEPROM.put(GEAR_EEPROM, gearMin);
    EEPROM.put((GEAR_EEPROM + 2), gearMid);
    EEPROM.put((GEAR_EEPROM + 4), gearMax);
    EEPROM.put(AUX1_EEPROM, aux1Min);
    EEPROM.put((AUX1_EEPROM + 2), aux1Mid);
    EEPROM.put((AUX1_EEPROM + 4), aux1Max);
    Serial.println(F("complete."));
  }

  Serial.println(F("Calibration complete."));
  Serial.println();
  delay(1000);
}

void loop() {
  cppm_cycle();
  mapSticks();

  Serial.print(aileMap); Serial.print(F(" / "));
  Serial.print(elevMap); Serial.print(F(" / "));
  Serial.print(ruddMap); Serial.print(F(" / "));
  Serial.print(throMap); Serial.print(F(" / "));
  Serial.print(gearMap); Serial.print(F(" / "));
  Serial.println(aux1Map);

  delay(100);
}

void mapSticks() {
  aileMap = map(aile, aileMin, aileMax, -100, 100);
  elevMap = map(elev, elevMin, elevMax, -100, 100);
  ruddMap = map(rudd, ruddMin, ruddMax, -100, 100);
  gearMap = map(gear, gearMin, gearMax, -100, 100);
  aux1Map = map(aux1, aux1Min, aux1Max, -100, 100);
}
