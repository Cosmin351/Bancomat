#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <EEPROM.h>
#include <string.h>
#include <Keypad.h>
#include "Adafruit_TCS34725.h"


const byte RAN =4;
const byte COL =4;

char keys[RAN][COL]={
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte PINran[RAN]={13,12,11,10};
byte PINcol[COL]={9,14,15,16};
Keypad customKeypad = Keypad(makeKeymap(keys), PINran, PINcol, RAN, COL); 

#define RFIDRPIN 5
#define RFIDSSPIN 53

int Senzorobj = 2;

int MotorCARD = 3;
int MCARD1 = 4;
int MCARD2 = 5;

int MotorBANI = 8;
int MBANI1 = 7;
int MBANI2 = 6;

int MotorCUTIE = 32;
int MCUTIE1 = 30;
int MCUTIE2 = 28;

int Motor1LEU = 22;
int M1LEU1 = 24;
int M1LEU2 = 26;

int loop1=0;


byte citirecard[4];
int citireok;
String tagID="";

MFRC522 mfrc522(RFIDSSPIN, RFIDRPIN);
LiquidCrystal_I2C lcd(0x27, 20, 4);
Adafruit_TCS34725 tcs = Adafruit_TCS34725(700, 1);


struct date{
  char id[10];
  char nume[20];
  char prenume[20];
  int pin;
  int suma;
};
struct datebani{
  int b1;
  int b5;
  int b10;
  int pozitiecasa;
};
datebani bd;
date persoana;
void setup(){
 int adresabani=0;
adresabani+=sizeof(int);
for(int i=0;i<3;i++){
    adresabani+=sizeof(date);
}
EEPROM.get(adresabani,bd);
Serial.begin(9600);
SPI.begin();
mfrc522.PCD_Init();
tcs.begin();
lcd.init();
lcd.backlight();
lcd.clear();
pinMode(Senzorobj, INPUT);
pinMode(MotorCARD,OUTPUT);
pinMode(MotorBANI,OUTPUT);
pinMode(MCARD1,OUTPUT);
pinMode(MCARD2,OUTPUT);
pinMode(MBANI1,OUTPUT);
pinMode(MBANI2,OUTPUT);
pinMode(MotorCUTIE,OUTPUT);
pinMode(Motor1LEU,OUTPUT);
pinMode(MCUTIE1,OUTPUT);
pinMode(MCUTIE2,OUTPUT);
pinMode(M1LEU1,OUTPUT);
pinMode(M1LEU2,OUTPUT);

digitalWrite(MCARD1,LOW);
digitalWrite(MCARD2,LOW);
digitalWrite(MBANI1,LOW);
digitalWrite(MBANI2,LOW);
digitalWrite(MCUTIE1,LOW);
digitalWrite(MCUTIE2,LOW);
digitalWrite(M1LEU1,LOW);
digitalWrite(M1LEU2,LOW);

}


void loop() {
  delay(500);
  if(loop1==0){
    loop1=1;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Introdu cardul.");
  }
  int valsenzor=digitalRead(Senzorobj);
  Serial.begin(9600);
  if(valsenzor==0){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Asteapta...");
    analogWrite(MotorBANI,0);
    analogWrite(MotorCARD,250);
    digitalWrite(MCARD1,HIGH);
    digitalWrite(MCARD2,LOW);
    delay(3000);
    digitalWrite(MCARD1,LOW);
    digitalWrite(MCARD2,LOW);
    digitalWrite(MBANI1,LOW);
    digitalWrite(MBANI2,LOW);
    int id=0;
    while(!id){
      id=getID();
    }
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Verificare card.");
    lcd.setCursor(0, 1);
    lcd.print("Asteapta...");
    delay(1500);
    lcd.clear();
    lcd.setCursor(0, 0);
    int ok=verificareid();
    if(ok==0){
      lcd.print("Card neinregistrat.");
      analogWrite(MotorCARD,250);
      digitalWrite(MCARD1,LOW);
      digitalWrite(MCARD2,HIGH);
      delay(3000);
      digitalWrite(MCARD1,LOW);
      digitalWrite(MCARD2,LOW);
      digitalWrite(MBANI1,LOW);
      digitalWrite(MBANI2,LOW);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Introdu cardul.");
    }
    else{
      lcd.print("PIN:");
      int raspuns=verificarepin();
      if(raspuns){
        char rsp='x';
        int ok=0;
        while(!ok){
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("A.Detalii cont.");
          lcd.setCursor(0,1);
          lcd.print("B.Retragere bani.");
          lcd.setCursor(0, 2);
          lcd.print("C.Depunere bani.");
          lcd.setCursor(0, 3);
          lcd.print("D.Iesire");
          while(rsp!='A'&&rsp!='B'&&rsp!='C'&&rsp!='D'&&rsp!='*'&& rsp!='#'){
            delay(100);
            rsp=customKeypad.getKey();
           
          }
          ok=afisareoptiuni(rsp);
          rsp='x';
        }
        
      }
      else{
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("CARD BLOCAT");
      digitalWrite(MCARD1,HIGH);
      digitalWrite(MCARD2,LOW);
      delay(3000);
      digitalWrite(MCARD1,LOW);
      digitalWrite(MCARD2,LOW);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Introdu cardul.");
      }
    }
  }
}

int getID(){
 if(!mfrc522.PICC_IsNewCardPresent()){
 return 0;
 }
 if(!mfrc522.PICC_ReadCardSerial()){
   return 0;
 }
  tagID = "";
  for ( uint8_t i = 0; i < 4; i++)
  {
    tagID.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  tagID.toUpperCase();
  mfrc522.PICC_HaltA();
  return 1;
}

int verificareid(){
    int persoane=0,adresa=0;
    char tagIDx[10];
    tagID.toCharArray(tagIDx, tagID.length()+1);
    EEPROM.get(adresa,persoane);
    adresa+=sizeof(int);
    for(int i=0;i<persoane;i++){
    EEPROM.get(adresa,persoana);
    if(strcmp(persoana.id,tagIDx)==0){
      return 1;
    }
    adresa+=sizeof(date);
    }
  return 0;
}

int verificarepin(){
    char key,pinx[10]="abcd";
    int contor=0,incercari=0,pinz;
    while(incercari<3)
    {key=customKeypad.getKey();
    if((key>='0'&&key<='9')&&(contor<4)){
      lcd.print('*');
      pinx[contor]=key;
      //Serial.println(pinx);
      contor++;
    }
    if(key=='#'&&contor>0){
      lcd.setCursor(3+contor,0);
      lcd.print(" ");
      lcd.setCursor(3+contor,0);
      contor--;
    }
    if(key=='*'&&contor==4)
      {pinz=atoi(pinx);
      //Serial.println(pinz);
        if(pinz==persoana.pin)
          return 1;
        else{
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("PIN GRESIT");
          delay(2000);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("PIN:");
          contor=0;
          incercari++;
        }
      }
    }
    return 0;
}


int afisareoptiuni(char raspuns){
  char rspt='x';
  switch(raspuns){
          case'A':{
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print(persoana.nume);
            lcd.setCursor(0,1);
            lcd.print(persoana.prenume);
            lcd.setCursor(0,2);
            lcd.print("Sold curent:");
            lcd.print(persoana.suma);
            lcd.print(" RON");
            lcd.setCursor(0,3);
            lcd.print("D.Iesire.");
            while(rspt!='D'){
              rspt=customKeypad.getKey();
            }
          };return 0;
          case'B':{
            int contorret=0;
            char sum[4]="";
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Introdu suma:");
            while(rspt!='*'){
              rspt=customKeypad.getKey();
              if((rspt>='0'&&rspt<='9')&&(contorret<4)){
                lcd.print(rspt);
                sum[contorret]=rspt;
                contorret++;
              }
              if(rspt=='#'){
                lcd.setCursor(13+contorret-1, 0);
                lcd.print(" ");
                lcd.setCursor(13+contorret-1, 0);
                contorret--;
              }
            }
            if(contorret!=0){
              if(contorret<4)
                sum[contorret]='\0';
              int intsum=atoi(sum);
              //Serial.println(sum);
              //Serial.println(intsum);
              if(intsum>persoana.suma){
                lcd.clear();
                lcd.setCursor(0,0);
                lcd.print("Sold insuficient.");
                delay(2000);
              }
              else{
                int rspb=0;
                rspb=Retragerebani(intsum);
                if(rspb==5){
                lcd.clear();
                lcd.setCursor(0,0);
                lcd.print("Bancomatul nu are suficienti bani.");
                delay(2500);
                }
                else{
                lcd.clear();
                lcd.setCursor(0,0);
                lcd.print("Ridica banii.");
                delay(2500);
                }
              }
            }
          };return 0;
          case 'C':{
            int contordep=0;
            char sum[4]="";
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Introdu suma:");
            while(rspt!='*'){
              rspt=customKeypad.getKey();
              if((rspt>='0'&&rspt<='9')&&(contordep<4)){
                lcd.print(rspt);
                sum[contordep]=rspt;
                contordep++;
              }
              if(rspt=='#'){
                lcd.setCursor(13+contordep-1, 0);
                lcd.print(" ");
                lcd.setCursor(13+contordep-1, 0);
                contordep--;
              }
            }
            if(contorret!=0){
              if(contordep<4)
                sum[contordep]='\0';
              int intsum=atoi(sum);
              int rspb=0;
              rspb=Depunereabani(intsum);
            }
          };return 0;
          case 'D':{
            int flag=0;
            lcd.clear();
            lcd.setCursor(0, 0);
            analogWrite(MotorCARD,250);
            digitalWrite(MCARD1,HIGH);
            digitalWrite(MCARD2,LOW);
            delay(3000);
            digitalWrite(MCARD1,LOW);
            digitalWrite(MCARD2,LOW);
            lcd.print("Ridica cardul.");
            while(!flag){
              flag=digitalread(Senzorobj);
            }
          };return 1;
        default:break;
        }
}

int Retragerebani(int sbani){
  int bc1=0,bc5=0,bc10=0,i=0;
  if(sbani>=10){
    bc10=sbani/10;
    sbani=sbani%10;
  }
  if(sbani>=5){
    bc5=sbani/5;
    sbani=sbani%5;
  }
  bc1=sbani;
  Serial.println("======"); // 0-gramada 1-10lei 2-5lei 3-1lei
  Serial.println(bc5);
  Serial.println("======");
  if((bc10>bd.b10)||(bc1>bd.b1)||(bc5>bd.b5))
    return 5;
  if(bc10!=0){
    Schimbarepozitie(&bd.pozitiecasa,1);
    for(i=0;i<bc10;i++){
      analogWrite(Motor1LEU,250);
      digitalWrite(M1LEU1,HIGH);
      digitalWrite(M1LEU2,LOW);
      delay(2000);
      digitalWrite(M1LEU1,LOW);
      digitalWrite(M1LEU2,LOW);
    }
  }
  if(bc5!=0){
    Schimbarepozitie(&bd.pozitiecasa,2);
    for(i=0;i<bc5;i++){
      analogWrite(Motor1LEU,250);
      digitalWrite(M1LEU1,HIGH);
      digitalWrite(M1LEU2,LOW);
      delay(2000);
      digitalWrite(M1LEU1,LOW);
      digitalWrite(M1LEU2,LOW);
    }
  }
  if(bc1!=0){
    Schimbarepozitie(&bd.pozitiecasa,3);
    for(i=0;i<bc1;i++){
      analogWrite(Motor1LEU,250);
      digitalWrite(M1LEU1,HIGH);
      digitalWrite(M1LEU2,LOW);
      delay(2000);
      digitalWrite(M1LEU1,LOW);
      digitalWrite(M1LEU2,LOW);
    }
  }
  return 1;
}

void Schimbarepozitie(int* poz,int pozdorita){
  switch(pozdorita){
    //Cuite bani gramada
    case 0:{
        switch(*poz){
          case 1:{
            analogWrite(MotorCUTIE,250);
            digitalWrite(MCUTIE1,LOW);
            digitalWrite(MCUTIE2,HIGH);
            delay(2000);
            digitalWrite(MCUTIE1,LOW);
            digitalWrite(MCUTIE2,LOW);
            *poz=0;
          };break;
          case 2:{
            analogWrite(MotorCUTIE,250);
            digitalWrite(MCUTIE1,LOW);
            digitalWrite(MCUTIE2,HIGH);
            delay(4000);
            digitalWrite(MCUTIE1,LOW);
            digitalWrite(MCUTIE2,LOW);
            *poz=0;
          };break;
          case 3:{
            analogWrite(MotorCUTIE,250);
            digitalWrite(MCUTIE1,LOW);
            digitalWrite(MCUTIE2,HIGH);
            delay(6000);
            digitalWrite(MCUTIE1,LOW);
            digitalWrite(MCUTIE2,LOW);
            *poz=0;
          }
          default:break;
        }
    };break;
    //Cutie 10 lei
    case 1:{
      switch(*poz){
          case 0:{
            analogWrite(MotorCUTIE,250);
            digitalWrite(MCUTIE1,HIGH);
            digitalWrite(MCUTIE2,LOW);
            delay(2000);
            digitalWrite(MCUTIE1,LOW);
            digitalWrite(MCUTIE2,LOW);
            *poz=1;
          };break;
          case 2:{
            analogWrite(MotorCUTIE,250);
            digitalWrite(MCUTIE1,LOW);
            digitalWrite(MCUTIE2,HIGH);
            delay(2000);
            digitalWrite(MCUTIE1,LOW);
            digitalWrite(MCUTIE2,LOW);
            *poz=1;
          };break;
          case 3:{
            analogWrite(MotorCUTIE,250);
            digitalWrite(MCUTIE1,LOW);
            digitalWrite(MCUTIE2,HIGH);
            delay(4000);
            digitalWrite(MCUTIE1,LOW);
            digitalWrite(MCUTIE2,LOW);
            *poz=1;
          }
          default:break;
        }
    };break;
    //Cutie 5 lei
    case 2:{
      switch(*poz){
          case 0:{
            analogWrite(MotorCUTIE,250);
            digitalWrite(MCUTIE1,HIGH);
            digitalWrite(MCUTIE2,LOW);
            delay(4000);
            digitalWrite(MCUTIE1,LOW);
            digitalWrite(MCUTIE2,LOW);
            *poz=2;
          };break;
          case 1:{
            analogWrite(MotorCUTIE,250);
            digitalWrite(MCUTIE1,HIGH);
            digitalWrite(MCUTIE2,LOW);
            delay(2000);
            digitalWrite(MCUTIE1,LOW);
            digitalWrite(MCUTIE2,LOW);
            *poz=2;
          };break;
          case 3:{
            analogWrite(MotorCUTIE,250);
            digitalWrite(MCUTIE1,LOW);
            digitalWrite(MCUTIE2,HIGH);
            delay(2000);
            digitalWrite(MCUTIE1,LOW);
            digitalWrite(MCUTIE2,LOW);
            *poz=2;
          }
          default:break;
        }
    };break;
    //Cutie 1 leu
    case 3:{
      switch(*poz){
          case 0:{
            analogWrite(MotorCUTIE,250);
            digitalWrite(MCUTIE1,HIGH);
            digitalWrite(MCUTIE2,LOW);
            delay(6000);
          };break;
          case 1:{
            analogWrite(MotorCUTIE,250);
            digitalWrite(MCUTIE1,HIGH);
            digitalWrite(MCUTIE2,LOW);
            delay(4000);
          };break;
          case 2:{
            analogWrite(MotorCUTIE,250);
            digitalWrite(MCUTIE1,HIGH);
            digitalWrite(MCUTIE2,LOW);
            delay(2000);
          }
          default:break;
        }
        digitalWrite(MCUTIE1,LOW);
        digitalWrite(MCUTIE2,LOW);
        *poz=3;
     };break;
    default:break;
  }
}

void Depunerebani(int sbani){
lcd.clear();
lcd.setCursor(0,0);
lcd.print("Suma ramasa:");
lcd.print(sbani);
lcd.print(" RON");
uint16_t r, g, b, c;
struct bnt{
  int max1;
  int min1;
  int max2;
  int min2;
};
bnt bmaxmin[8]={
  {303,308,314,318},
  {327,332,337,342},
  {3,0},
  {0,0},
  {0,0}
}
int b1min=10,b1max=50,b5min=100,b5max=150,b10min=250,b10max=300;
int b20min=255,b20max=250,b50min=0,b50max=0,b100min=0,b100max=0,b200max=0,b200min=0,b500max=0,b500min=0;
int ok=0,v[8]={1,5,10,20,50,100,200,500},detect;;
while(sbani>0){ 
  detect=digitalRead(Senzorobj);
  if(!detect){
    analogWrite(MBANI,250);
    digitalWrite(MBANI1,HIGH);
    digitalWrite(MBANI2,LOW);
    delay(1000);
    digitalWrite(MBANI1,LOW);
    digitalWrite(MBANI2,LOW);
    for(int i=0;i<25;i++){
    tcs.getRawData(&r, &g, &b, &c);
      for(int j=0;j<8;j++){
        if(c>=x&&c<=y){
          digitalWrite(MBANI1,HIGH);
          digitalWrite(MBANI2,LOW);
          delay(1000);
          digitalWrite(MBANI1,LOW);
          digitalWrite(MBANI2,LOW);
          sbani-=v[j];
        }
      }
    if(c>=b1min&&c<=b1max){
      digitalWrite(MBANI1,HIGH);
      digitalWrite(MBANI2,LOW);
      delay(1000);
      digitalWrite(MBANI1,LOW);
      digitalWrite(MBANI2,LOW);
      sbani-=1;
    }
    else if(c>=b5min&&c<=b5max){
      digitalWrite(MBANI1,HIGH);
      digitalWrite(MBANI2,LOW);
      delay(1000);
      digitalWrite(MBANI1,LOW);
      digitalWrite(MBANI2,LOW);
      sbani-=5;
    }
    else if(c>=b10min&&c<=b10max){
      digitalWrite(MBANI1,HIGH);
      digitalWrite(MBANI2,LOW);
      delay(1000);
      digitalWrite(MBANI1,LOW);
      digitalWrite(MBANI2,LOW);
      sbani-=10;
    }
    delay(50);
    }
  }
}



}

