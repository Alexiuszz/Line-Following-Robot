#define enB 3
#define enA 9
#define IN1 7
#define IN2 6
#define IN3 5
#define IN4 4

#define calLED 10
#define calButton 11

#define followButton 12

#define IR1 A7
#define IR2 A6
#define IR3 A5
#define IR4 A4
#define IR5 A3
#define IR6 A2
#define IR7 A1
#define IR8 A0

#include <Adafruit_NeoPixel.h>

const int baseSpeed = 120;
const uint8_t maxSpeed = 180;
int aSpeed = 0;
int bSpeed = 0;
int prevPos = 0;
int pos = 0;
//const int offPos = 1000;
const int offPos = 2300;

//const int weight [8] = { -4, -3, -2, -1, 1, 2, 3, 4};
const int weight [8] = { -12, -7, -3, -1, 1, 3, 7, 12};

int irArray [8];

int highIr [8];
//int lowIr [8] = {1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000};
int lowIr [8] = {2300, 2300, 2300, 2300, 2300, 2300, 2300, 2300};

int highBlack = 10;
int thresh = 20;

bool caliState = false;

bool followState = false;

Adafruit_NeoPixel pixels(1, calLED, NEO_GRB + NEO_KHZ800);

void setup() {
  // put your setup code here, to run once:
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(IN1, OUTPUT);
  pinMode(IN1, OUTPUT);

  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(enA, INPUT);
  pinMode(enB, INPUT);

  pinMode(IR1, INPUT);
  pinMode(IR2, INPUT);
  pinMode(IR3, INPUT);
  pinMode(IR4, INPUT);
  pinMode(IR5, INPUT);
  pinMode(IR6, INPUT);
  pinMode(IR7, INPUT);
  pinMode(IR8, INPUT);

  pinMode(calButton, INPUT);
  pinMode(followButton, INPUT);

  pixels.begin();
  pixels.clear(); // Set all pixel colors to 'off'


  //  Serial.println("Place to calibrate Black");
  //  delay(3000);
  //  calibrate(false);
  //
  //  delay(100);
  //  Serial.println("Place to calibrate White");
  //  delay(3000);
  //  calibrate(true);
  //
  //  Serial.println("Done");
  //  delay(1000);
}

void loop() {
  int cali = digitalRead(calButton);
  if (cali == 1) {
    Serial.println("Place to calibrate Black");
    delay(3000);
    calibrate(false);

    delay(500);
    Serial.println("Place to calibrate White");
    delay(3000);
    calibrate(true);

    Serial.println("Done");
    delay(1000);

    caliState = true;
  }

  if (caliState && followState  == false) {
    int follow = digitalRead(followButton);
    if (follow == 1) {
      followState = true;
    }
  }
  else if (caliState && followState) {
    // put your main code here, to run repeatedly:
    // put your main code here, to run repeatedly:
    irArray[0] = GetState(analogRead(IR1), highIr[0], lowIr[0]);
    irArray[1] = GetState(analogRead(IR2), highIr[1], lowIr[1]);
    irArray[2] = GetState(analogRead(IR3), highIr[2], lowIr[2]);
    irArray[3] = GetState(analogRead(IR4), highIr[3], lowIr[3]);
    irArray[4] = GetState(analogRead(IR5), highIr[4], lowIr[4]);
    irArray[5] = GetState(analogRead(IR6), highIr[5], lowIr[5]);
    irArray[6] = GetState(analogRead(IR7), highIr[6], lowIr[6]);
    irArray[7] = GetState(analogRead(IR8), highIr[7], lowIr[7]);

    //    for (int i = 0; i < 8; i++) {
    //      Serial.print(irArray[i]);
    //      Serial.print("   ");
    //    }
    //    Serial.println();

    pos = GetPosition();
    //    if (pos > offPos || pos < -offPos){
    if (pos > -5000) {
      prevPos = pos;
      //      drive(0, 0);
    } else {
      if (prevPos > 0) {
        pos = offPos;
      }
      else pos = -offPos;
    }
    startFollow();
  }
  else
    return;

  //    Serial.println(pos);

}


void startFollow() {
  if (pos == offPos || pos == -offPos)
    sharpTurn();
  else {
    setSpeeds();
    drive(aSpeed, bSpeed);

  }
}
void setSpeeds() {
  int rPos = map(pos, -1500, 1500, -70, 70);
  //  Serial.println(rPos);
  aSpeed = baseSpeed - rPos;
  bSpeed = baseSpeed + rPos;
  if (aSpeed > maxSpeed) {
    aSpeed = maxSpeed;
  }
  if (bSpeed > maxSpeed) {
    bSpeed = maxSpeed;
  }

  //  if (getPositive(aSpeed - baseSpeed) < 2) {
  //    aSpeed = maxSpeed, bSpeed = maxSpeed;
  //  }
  if (aSpeed < 20) {
    aSpeed = 0;
  }
  if (bSpeed < 20) {
    bSpeed = 0;
  }
}

int getPositive(int num) {
  if (num > 0)
    return num;
  else return -1 * num;
}
void drive(int speedA, int speedB) {
  //  Serial.print(speedA);
  //  Serial.print("   ");
  //  Serial.print(speedB);
  //  Serial.println();

  analogWrite(enA, speedA);
  analogWrite(enB, speedB);
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
}

void sharpTurn() {
  if (pos == -offPos) {
    analogWrite(enA, 100);
    analogWrite(enB, 80);
    digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
  }
  else {
    analogWrite(enA, 80);
    analogWrite(enB, 100);
    digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  }
}

void calibrate(bool white) {
  int curr = 0;
  if (!white) {
    for (int i = 0; i < 3000; i++) {
      pixels.setPixelColor(0, pixels.Color(0, 0 , i % 50));
      pixels.show();
      curr = analogRead(IR1);
      highIr[0] = CheckMax(highIr[0], curr);
      curr = analogRead(IR2);
      highIr[1] = CheckMax(highIr[1], curr);
      curr = analogRead(IR3);
      highIr[2] = CheckMax(highIr[2], curr);
      curr = analogRead(IR4);
      highIr[3] = CheckMax(highIr[3], curr);
      curr = analogRead(IR5);
      highIr[4] = CheckMax(highIr[4], curr);
      curr = analogRead(IR6);
      highIr[5] = CheckMax(highIr[5], curr);
      curr = analogRead(IR7);
      highIr[6] = CheckMax(highIr[6], curr);
      curr = analogRead(IR8);
      highIr[7] = CheckMax(highIr[7], curr);
    }

  } else {
    curr = 0;
    for (int i = 0; i < 3000; i++) {
      pixels.setPixelColor(0, pixels.Color(i % 50 , i % 50 , i % 50));
      pixels.show();
      curr = analogRead(IR1);
      lowIr[0] = CheckMin(lowIr[0], curr);
      curr = analogRead(IR2);
      lowIr[1] = CheckMin(lowIr[1], curr);
      curr = analogRead(IR3);
      lowIr[2] = CheckMin(lowIr[2], curr);
      curr = analogRead(IR4);
      lowIr[3] = CheckMin(lowIr[3], curr);
      curr = analogRead(IR5);
      lowIr[4] = CheckMin(lowIr[4], curr);
      curr = analogRead(IR6);
      lowIr[5] = CheckMin(lowIr[5], curr);
      curr = analogRead(IR7);
      lowIr[6] = CheckMin(lowIr[6], curr);
      curr = analogRead(IR8);
      lowIr[7] = CheckMin(lowIr[7], curr);
    }
  }
  pixels.setPixelColor(0, pixels.Color(0, 0 , 0));
  pixels.show();
}

int CheckMax(int temp, int curr) {
  if (temp < curr) {
    return curr;
  }
  return temp;
}

int CheckMin(int temp, int curr) {
  if (temp > curr) {
    return curr;
  }
  return temp;
}

int GetState(int value, int high, int low) {
  return map(value, low, high, 0, 100);
}

int GetPosition() {
  if (getPositive(irArray[3]) + getPositive(irArray[4]) > 18) {
    if (irArray[3] - irArray[4] < 15 && irArray[4] - irArray[3] > 15)
      //      Serial.println("middle");
      return irArray[3] + irArray[4];
  } else {
    for (int i = 0; i < 8; i++) {
      //find first black
      if (irArray[i] > thresh) {
        //if black is not last ir
        if (i != 7) {
          //    check if first
          if (i == 0) {
            //          Serial.println("First is high");
            //      check if consecutive black
            if (irArray[i + 1] > highBlack) {
              //check for 4 consec black
              if (irArray[i + 2] > thresh && irArray[i + 3] > thresh) {
                //    sharp turn needed
                return  (halfCalc(i)) * (2);
              }
              //check if 3 consec or less, reduced turn since its most likely a curve
              else if (irArray[i + 2] > thresh) {
                return  halfCalc(i) * 3 / 5;
              } else {
                return  halfCalc(i) * 4 / 5;
              }
            }
            //   if no consecutive black
            else {
              //incase there are  reasonable jump right side like for a fork
              if (linearCalc() < 0 || linearCalc() > 350) { //250
                //no
                return  -1 * offPos;
              }
              else {
                if (linearCalc() > 0 )
                  return  linearCalc();
              }
            }
          }
          else {
            if (i + 1 < 8 && irArray[i + 1] > thresh) {
              //check for 4 consec black
              if (i + 3 < 8 && irArray[i + 2] > thresh && irArray[i + 3] > thresh) {
                // if ir2, middle 4 ir,choose one half
                if (i == 2) {
                  //check side with higher pos
                  if (irArray[i] + irArray[i + 1] < irArray[i + 2] + irArray[i + 3] )
                    return halfCalc(i) * -2;
                  else
                    return halfCalc(i) * 2;
                }
                else
                  return halfCalc(i);
              }
              else {
                //check if 3 consec, reduced turn since its most likely a curve
                if (i + 2 < 8 && irArray[i + 2] > 70) {
                  return linearCalc() * 3 / 5;
                } else {
                  return linearCalc() * 4 / 5;
                }
              }

            }
            else
              return linearCalc();
          }
        }
        else {
          return offPos;
        }
      }
    }
    return -5000;
  }
}

int linearCalc() {
  int newPos = 0;
  for (int i = 0; i < 8; i++) {
    newPos += weight[i] * irArray[i];
  }
  return newPos;
}


int halfCalc(int firstHigh) {
  int newPos = 0;
  if (firstHigh < 2) {
    for (int i = 0; i < 4; i++) {
      newPos += weight[i] * irArray[i];
    }
  }
  else if (firstHigh > 5) {
    for (int i = 5; i < 8; i++) {
      newPos += weight[i] * irArray[i];
    }
  }
  return newPos;
}

//float GetPosition() {
//  float temp = 0.0;
//  for (int i = 0; i < 8; i++) {
//    //    find first high
//    if (irArray[i] == HIGH) {
//      if (i != 7) {
//        // check if next is high since atleast 2 must be high except extremes
//        if (irArray[i + 1] == HIGH) {
//          //   if third consecutive high
//          if (i + 2 != 7) {
//            if (irArray[i + 2] == HIGH && irArray[i + 4] == HIGH) {
//              //if 3 straight high then sharp turn needed
//              if (i < 3) { //left
//                return temp = 0.0;
//              }
//              else { //right
//                return temp = 100;
//              }
//            } else {
//              // assign temp non-extreme value
//              return temp = (i + 1) * 12.5;
//            }
//          }
//          // only last two are High  assign temp non-extreme
//          else {
//            return temp = (i + 1) * 12.5;
//          }
//        }
//        //Since only one High
//        else {
//          //   if its the first
//          if (i == 0) {
//            //            wait and check for a jump
//            bool jump = false;
//            for (int j = i + 1; j < 8; j++) {
//              if (irArray[j] == HIGH) {
//                //if jump found break sub search
//                i = j - 1;
//                jump = true;
//                break;
//              }
//            }
//            //and jump main search
//            if (jump) {
//              continue;
//            }
//            else {
//              //if no jump extreme left
//              return temp = 0;
//            }
//          }
//          //  else {  if not first don't change position an error/bug
//          return temp = pos;
//        }
//      }
//      else { // extreme right
//        return temp = 100;
//      }
//    }
//  }
//}


//void MoveStraight() {
//  currSpeed = 150;
//  analogWrite(enA, 150);
//  analogWrite(enB, 150);
//  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
//  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
//}

//void Stop() {
//  for (currSpeed; currSpeed >= 0; currSpeed--)  {
//    analogWrite(enA, currSpeed);
//    analogWrite(enB, currSpeed);
//    digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
//    digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
//  }
//}
//
//
//void TurnSlow (int right) {
//  currSpeed = 100;
//  if (right) {
//    analogWrite(enA, 100);
//    analogWrite(enB, 80);
//  }
//  else {
//    analogWrite(enA, 80);
//    analogWrite(enB, 100);
//  }
//
//  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
//  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
//}
//
//void Turn (int right) {
//  currSpeed = 150;
//  if (right) {
//    analogWrite(enA, 150);
//    analogWrite(enB, 0);
//  }
//  else {
//    analogWrite(enA, 0);
//    analogWrite(enB, 150);
//  }
//
//  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
//  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
//}
