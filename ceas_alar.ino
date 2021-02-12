#include <LiquidCrystal.h>
#include <avr/interrupt.h>
#include <avr/io.h>



LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
// Masca pentru primul caracter, fiecare linie de biţi
//reprezintă o linie a caracterului

String inputString = ""; 
boolean stringComplete = false;

volatile int minutes=00; 
volatile int hours=12;
volatile int seconds=8;
volatile int alarmMinutes; 
volatile int alarmHours;
volatile int alarmSeconds;


volatile int buttonStateCounter = 0;
int buttonStateMinutes = 0;
int buttonStateHours = 0;
int buttonStateMode = 0;

void setup() {
  
 analogWrite(6,110);
 Serial.begin(9600); 
  
 inputString.reserve(200);

  
  
 pinMode(19, INPUT_PULLUP);
 pinMode(20, INPUT_PULLUP);
 pinMode(21,INPUT_PULLUP);
 
 pinMode(13,OUTPUT);
 pinMode(10,OUTPUT);
 pinMode(9,OUTPUT);
 attachInterrupt(digitalPinToInterrupt(19), functieUnu,RISING);
 attachInterrupt(digitalPinToInterrupt(20), functieDoi,RISING);
 attachInterrupt(digitalPinToInterrupt(21), functieCount,RISING);
  
 seconds =55;
 alarmMinutes=2;
 alarmHours = 13;
 alarmSeconds = 00;
   
 cli(); // facem disable la întreruperile globale
 TCCR1A = 0; // setăm TCCR1A și B la 0
 TCCR1B = 0;
 lcd.begin(16, 2);
 // setăm registrul cu valoarea căruia vom compara TCNT
 OCR1A = 15624;
 // activăm modul CTC:
 TCCR1B |= (1 << WGM12);
 // divizăm ceasul plăcii cu 1024:
 TCCR1B |= (1 << CS10);
 TCCR1B |= (1 << CS12);
 // facem enable la întreruperea la comparare prin setarea
 //bitului
 // corespunzător din mască
 TIMSK1 |= (1 << OCIE1A);
 // validăm sistemul global de întreruperi
 sei();


}



// Funcţia loop rămâne nefolosită, puteți să o folosiți pentru
//a extinde funcționalitatea
void loop()
{
  
  //buttonStateMode = digitalRead(21);
  
  
    //afisam ora
  lcd.setCursor(1,0);
    if(hours >= 10)
    lcd.print(hours);
  else
    {
      lcd.print("0");
      lcd.print(hours);
      
    }
    lcd.setCursor(3,0);
    lcd.print(":");
    if(minutes >= 10)
    lcd.print(minutes);
  else
    {
      lcd.print("0");
      lcd.print(minutes);
      
    }
    lcd.setCursor(6,0);
    lcd.print(":");
    lcd.setCursor(7,0);
    if(seconds >= 10)
    lcd.print(seconds);
  else
    {
      lcd.print("0");
      lcd.print(seconds);
      
    }
    lcd.print("  C");
    lcd.setCursor(0,0);
    
  
  //afisam alarma
  lcd.setCursor(1,1);
    if(alarmHours >= 10)
    lcd.print(alarmHours);
  else
    {
      lcd.print("0");
      lcd.print(alarmHours);
      
    }
    lcd.setCursor(3,1);
    lcd.print(":");
    if(alarmMinutes >= 10)
    lcd.print(alarmMinutes);
  else
    {
      lcd.print("0");
      lcd.print(alarmMinutes);
      
    }
    lcd.setCursor(6,1);
    lcd.print(":");
    lcd.setCursor(7,1);
    lcd.print("00  A");
    lcd.setCursor(0,0);
    
  if(inputString){
    if(inputString[0] == 'A' ) {
      int auxHours = inputString[2] - '0';
      auxHours = auxHours * 10;
      auxHours = auxHours + (inputString[3] - '0');
      if(auxHours < 24)
        alarmHours = auxHours;
      else
        Serial.println("Ore gresite!!");
      
      int auxMin = inputString[5] - '0';
      auxMin = auxMin * 10;
      auxMin = auxMin + (inputString[6] - '0');
      if(auxMin < 60)
        alarmMinutes=auxMin;
      else
        Serial.println("Minute gresite!");
      
      Serial.println(alarmMinutes);
      
    }
    if(inputString[0] == 'C' ) {
      int auxHours = inputString[2] - '0';
      auxHours = auxHours * 10;
      auxHours = auxHours + (inputString[3] - '0');
      if(auxHours < 24)
        hours = auxHours;
      else
        Serial.println("ORA INTRODUSA GRESIT!");
      
      int auxMin = inputString[5] - '0';
      auxMin = auxMin * 10;
      auxMin = auxMin + (inputString[6] - '0');
      if(auxMin < 60)
        minutes=auxMin;
      else
        Serial.println("Minute gresite!");
      
      //Serial.println(alarmMinutes);
      
    }
  }
  
  
  
  
  
  
    Serial.println(buttonStateCounter);
      



  
}

ISR(TIMER1_COMPA_vect)
{
  int pres=0;
  seconds = seconds + 1;
  if(seconds >= 60)
    {
        seconds =00;
      minutes = minutes + 1;
    }
    
    if(minutes >= 60)
    {
        minutes =00;
      hours = hours + 1;
    }
    
    if(hours >= 24)
    {
        hours = 0;
      
    }
 
    
    if(alarmMinutes >= 60)
    {
        alarmMinutes =00;
      hours = hours + 1;
    }
    
    if(alarmHours >= 24)
    {
        alarmHours = 0;
      
    }
  if((alarmMinutes == minutes) && (alarmHours == hours))  
      pres=1;

  if(seconds >= 15 && pres == 1)
  {
    pres=0;
    digitalWrite(13,LOW);
    digitalWrite(10,LOW);
    digitalWrite(9,LOW);
  }
 if(pres==1){
   if(seconds % 3 == 0)
     {
        digitalWrite(13,HIGH);
        digitalWrite(10,LOW);
        digitalWrite(9,LOW);
     }
   if(seconds % 3 == 1)
     {
        digitalWrite(10,HIGH);
        digitalWrite(13,LOW);
        digitalWrite(9,LOW);
     }
   if(seconds % 3 == 2)
     {
        digitalWrite(9,HIGH);
        digitalWrite(10,LOW);
        digitalWrite(13,LOW);
     }
 }
}


void serialEvent() {
  while (Serial.available()) {
 // luăm byte-ul nou venit:
 // îl citim cu serial.read
        //char inChar =  (char)Serial.read();
        inputString = "";
    inputString = Serial.readStringUntil('\n');
    
    
 //adăugăm în inputString
 // nu adăugăm new line în input string întrucât ne va
 //afișa un caracter în plus pe lcd
    /*if (inChar != '\n')
      inputString += inChar;*/
 // de ceva în
 //legătură cu asta
    /*if (inChar == '\n') {
      stringComplete = true;
    }*/
    }
}

void functieDoi()
{
  if((buttonStateCounter %2) == 0)
        hours = (hours + 1)%24;
    else
        alarmHours = (alarmHours +1)%24;
}


//a doua procedură ISR
void functieUnu()
{
  if((buttonStateCounter %2) ==  0)
      minutes = (minutes + 1)%60;
    else
      alarmMinutes = (alarmMinutes+1)%60;
  
}

void functieCount()
{
      buttonStateCounter = buttonStateCounter +1;
      
}
