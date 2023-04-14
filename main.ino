#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define SS_PIN 53
#define RST_PIN 5

LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo s1;
Servo s2;

bool s1is180 = false;
bool s2is180 = false;


int button_wait_time = 20000;
int button1Pin = 2;
int button2Pin = 3;
bool button1Pressed = false;
bool button2Pressed = false;

class Card {
  public:
    String uid;
    int credits;
};

Card authorized_cards[] = {
  {"03 AF AF AA", 10},
  {"73 34 78 A5", 5}
};

int num_authorized_cards = sizeof(authorized_cards) / sizeof(authorized_cards[0]);

MFRC522 mfrc522(SS_PIN, RST_PIN);



void printStringsOnLCD(String str1, String str2) {
  lcd.clear(); // clear the LCD screen

  // Print the first string on the first line of the LCD
  lcd.setCursor(0, 0);
  lcd.print(str1);

  // Print the second string on the second line of the LCD
  lcd.setCursor(0, 1);
  lcd.print(str2);
}

void setup() {
  Serial.begin(9600);
  SPI.begin();
  Wire.begin();
  lcd.begin();
  lcd.backlight();
  
  mfrc522.PCD_Init();
  Serial.println("Scan your ID to begin purchase");
  printStringsOnLCD("Scan your ID to", "Begin purchase");  
  Serial.println();

  pinMode(button1Pin, INPUT);
  pinMode(button2Pin, INPUT);
  s1.attach(9);
  s2.attach(8);
  //s1
  if (s1.read() > 90) {
    s1.write(180);
    s1is180=true;
  }
  else if (s1.read() < 90) {
    s1.write(0);
  }
  else {
    s1.write(0);
  }
//s2
if (s2.read() > 90) {
    s2.write(180);
    s2is180=true;
  }
  else if (s2.read() < 90) {
    s2.write(0);
  }
  else {
    s2.write(0);
  }
  
}

void loop() {
  unsigned long currentMillisForLcd = millis();
  const unsigned long intervalLcd = 500;
  delay(500);

  printStringsOnLCD("SCAN YOUR ID TO", "BEGIN PURCHASE"); 
  Serial.println("Scan ID to begin");

  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  Serial.print("UID tag :");
  String content = "";
  byte letter;
  
  for (byte i = 0; i < mfrc522.uid.size; i++) {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }

  Serial.println();
  content.toUpperCase();
  boolean is_authorized = false;
  int credits = 0;
  
  for (int i = 0; i < num_authorized_cards; i++) {
    if (content.substring(1) == authorized_cards[i].uid) {
      is_authorized = true;
      credits = authorized_cards[i].credits;
      break;
    }
  }
  
  if (is_authorized) {
    Serial.print("Welcome. Your available credit is: ");
    Serial.print(credits);
    Serial.println(" credits");
    Serial.println("Please select your item...");
    printStringsOnLCD("CREDITS: " + String(credits) + "TAKA", "SELECT YOUR ITEM");
    
    int button_timer_start = millis();
    int button_pressed = 0;
   
    while (button_pressed == 0) {
      int button_timer_current = millis();
      if (button_timer_current-button_timer_start>=button_wait_time){
        break;
      }
     if (digitalRead(button1Pin) == HIGH ) {
        button_pressed=1;
        button1Pressed=true;
      }
      else if ( digitalRead(button2Pin) == HIGH) {
      button_pressed=1;
        button2Pressed=true;
      }
    }

       if (button_pressed==0) {
     Serial.println("Timeout!");
     printStringsOnLCD("TIMEOUT", "");
    }
  //s1-button1
  if (button1Pressed) {
  
   if (s1is180) {
      s1.write(0);
      s1is180=false;
    } else {
      s1.write(180);
      s1is180=true;
    }
    Serial.println("Thank you for your purchase");
    printStringsOnLCD("THANK YOU FOR", "YOUR PURCHASE");
    button1Pressed=false;
    delay(1000);
  }
   //s2-button1
  if (button2Pressed) {
  
   if (s2is180) {
      s2.write(0);
      s2is180=false;
    } else {
      s2.write(180);
      s2is180=true;
    }
    Serial.println("Thank you for your purchase");
    printStringsOnLCD("THANK YOU FOR", "YOUR PURCHASE");
    button2Pressed=false;
    delay(1000);
  }

  
  }
  
    
   
  else   
  {
    Serial.println("Access denied");
    printStringsOnLCD("ID NOT", " RECOGNIZED");
    delay(1000);
  }
}

