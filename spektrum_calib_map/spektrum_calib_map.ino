#include <CPPM.h>

#define UPDATE_TIMEOUT_SEC 10

const int ppmPin = 8;

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

void cppm_cycle(void) {
  if (CPPM.synchronized()) {
    //    // good for DX8-R615X
    //    int aile = (CPPM.read(CPPM_AILE) - 1500*2) / 8 * 125 / 128; // aile -100% .. +100%
    //    int elev = (CPPM.read(CPPM_ELEV) - 1500*2) / 8 * 125 / 128; // elevator -100% .. +100%
    //    int thro = (CPPM.read(CPPM_THRO) - 1500*2) / 8 * 125 / 128; // throttle -100% .. +100%
    //    int rudd = (CPPM.read(CPPM_RUDD) - 1500*2) / 8 * 125 / 128; // rudder -100% .. +100%
    //    int gear = (CPPM.read(CPPM_GEAR) - 1500*2) / 8 * 125 / 128; // gear -100% .. +100%
    //    int aux1 = (CPPM.read(CPPM_AUX1) - 1500*2) / 8 * 125 / 128; // flap -100% .. +100%

    int aile = CPPM.read_us(CPPM_AILE) - 1500; // aileron
    int elev = CPPM.read_us(CPPM_ELEV) - 1500; // elevator
    int thro = CPPM.read_us(CPPM_THRO) - 1500; // throttle
    int rudd = CPPM.read_us(CPPM_RUDD) - 1500; // rudder
    int gear = CPPM.read_us(CPPM_GEAR) - 1500; // gear
    int aux1 = CPPM.read_us(CPPM_AUX1) - 1500; // flap

    /*Serial.print(aile); Serial.print(", ");
      Serial.print(elev); Serial.print(", ");
      Serial.print(thro); Serial.print(", ");
      Serial.print(rudd); Serial.print(", ");
      Serial.print(gear); Serial.print(", ");
      Serial.print(aux1); Serial.print("\n");
      Serial.flush();*/

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

  else {
    // if not synchronized, do something...
  }
}

void setup() {
  Serial.begin(115200);

  Serial.println(F("When program begins, move both sticks to their maximum positions in every direction."));
  Serial.println(F("Program will automatically end once sufficient data is gathered."));
  delay(5000);
  Serial.println(F("Program starting...begin moving control sticks."));
  delay(1000);

  CPPM.begin();

  while ((millis() - updateLast) < updateTimeout) {
    cppm_cycle();
    delay(100);
  }

  Serial.println();
  Serial.println(F("Data capture complete. Computing averages and create mapped output parameters."));
  Serial.println();

  int aileMid = (aileMin + aileMax) / 2;
  int elevMid = (elevMin + elevMax) / 2;
  int throMid = (throMin + throMax) / 2;
  int ruddMid = (ruddMin + ruddMax) / 2;
  int gearMid = (gearMin + gearMax) / 2;
  int aux1Mid = (aux1Min + aux1Max) / 2;

  Serial.println(F("            Min - Mid - Max"));
  Serial.print(F("Aileron:  "));
  Serial.print(F("Elevator: "));
  Serial.print(F("Throttle: "));
  Serial.print(F("Rudder:   "));
  Serial.print(F("Gear:     "));
  Serial.print(F("Aux1:     "));
}

void loop() {
}
