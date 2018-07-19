#include <IRremote.h>   
#include <SPI.h>
#include <SD.h>

#define PanasonicAddress  0x4004

File myFile;
String ctype , cdata , len ;
char d[20];
int clen , type;
long codeData ;

#define RECV_PIN = 9;
#define mode = 2;
#define RED 6
#define GREEN 7
#define BLUE 8

IRrecv irrecv(RECV_PIN);
IRsend irsend;

decode_results results;

String string;

int modecnt = 0;


void setup()
{
  Serial.begin(9600);
  irrecv.enableIRIn(); 
  pinMode(mode, INPUT);
  digitalWrite(mode,HIGH);

  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);

  digitalWrite(RED,HIGH);
  digitalWrite(GREEN,HIGH);
  digitalWrite(BLUE,HIGH);
 //  Serial.begin(9600);
  while (!Serial) {
    ; 
  }


//  Serial.print("Initializing SD card...");

  if (!SD.begin(10)) {
  //  Serial.println("initialization failed!");
    return;
  }
//  Serial.println("initialization done.");
}


int codeType = -1; 
unsigned long codeValue; 
unsigned int rawCodes[RAWBUF];
int codeLen; 
int toggle = 0; 


void storeCode(decode_results *results) {
  codeType = results->decode_type;
  //int count = results->rawlen;
  if (codeType == UNKNOWN) {
    digitalWrite(RED,LOW);
    digitalWrite(GREEN,HIGH);
    digitalWrite(BLUE,HIGH);
    delay(1000);
    digitalWrite(RED,HIGH);
    digitalWrite(GREEN,HIGH);
    digitalWrite(BLUE,HIGH);   
Serial.println("Received unknown code, saving as raw");
    codeLen = results->rawlen - 1;

    for (int i = 1; i <= codeLen; i++) {
      if (i % 2) {
        rawCodes[i - 1] = results->rawbuf[i]*USECPERTICK - MARK_EXCESS;
     //   Serial.print(" m");
      } 
      else {
        rawCodes[i - 1] = results->rawbuf[i]*USECPERTICK + MARK_EXCESS;
    //    Serial.print(" s");
      }
    //  Serial.print(rawCodes[i - 1], DEC);
    }
 //   Serial.println("");
  }
  else {

   
    if (codeType == NEC) {
      Serial.print("Received NEC: ");
      if (results->value == REPEAT) {
    //    Serial.println("repeat");
        return;
      }
    } 
    else if (codeType == SONY) {
   //   Serial.print("Received SONY: ");
    } 
    else if (codeType == PANASONIC) {
      Serial.print("Received PANASONIC: ");
    }
    else if (codeType == JVC) {
    //  Serial.print("Received JVC: ");
    }
    else if (codeType == RC5) {
      Serial.print("Received RC5: ");
    } 
    else if (codeType == RC6) {
      Serial.print("Received RC6: ");
    } 
    else {
   //   Serial.print("Unexpected codeType ");
   //   Serial.print(codeType, DEC);
   //   Serial.println("");
    }

 
 //   Serial.print(codeLen);
  //  Serial.print("   ");
    Serial.println(results->value, HEX);
    codeValue = results->value;
    codeLen = results->bits;


     if (SD.exists(string))
            {
              SD.remove(string);
              Serial.println("Already Exists!!!!!!!!!   Removing...........");
            }
     
     myFile = SD.open(string, FILE_WRITE);

  if (myFile) {
    Serial.println("Writing data...");
    myFile.println(codeType);
    myFile.println(codeValue,HEX);
    myFile.println(codeLen);
    myFile.close();
    digitalWrite(RED,HIGH);
    digitalWrite(GREEN,LOW);
    digitalWrite(BLUE,HIGH);
    delay(500);
    digitalWrite(RED,HIGH);
    digitalWrite(GREEN,HIGH);
    digitalWrite(BLUE,HIGH);
   
    Serial.println("done.");
  } else {
    Serial.println("error opening file");
    digitalWrite(RED,LOW);
    digitalWrite(GREEN,HIGH);
    digitalWrite(BLUE,HIGH);
    delay(1000);
    digitalWrite(RED,HIGH);
    digitalWrite(GREEN,HIGH);
    digitalWrite(BLUE,HIGH);
  }
    
   myFile = SD.open(string);
  if (myFile) {
   // Serial.println("data.txt:");

    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    myFile.close();
  } else {
    
    Serial.println("error opening file");
  }
  
  
  }
}



void loop() {
      SD.begin(10);
      if(digitalRead(mode) == LOW)
      {
        //string="";
        delay(200);
        modecnt++;
        if(modecnt == 2)
          modecnt = 0;
        
      }
     // Serial.println(modecnt);
     
       
          if(modecnt == 0)
          {
            ////////////////////////Testing Mode/////////////////////////
              while (Serial.available())
              { 
              delay(10); 
              char c = Serial.read(); 
              string += c; 
              }  
              if (string.length() > 0)
              {
            //    Serial.println(string);
            //      Serial.println("...........In Testing Mode...........");
                  getData(string);
                  
                 //////////////////////////////////////////////////////////////////////////////

                 //////////////////////////////////////////////////////////////////////////////  
                  sendCode(type,codeData,clen);
//                  sendCode(ctype,cdata,len);
                  string="";
              }
          }
         else if(modecnt == 1)
          {
            irrecv.enableIRIn();
              /*  digitalWrite(RED,LOW);
                digitalWrite(GREEN,HIGH);
                digitalWrite(BLUE,HIGH);
                delay(1000);
                digitalWrite(RED,HIGH);
                digitalWrite(GREEN,HIGH);
                digitalWrite(BLUE,HIGH);
              */
            ///////////////////////Training Mode///////////////////
              while (Serial.available())
              { 
              delay(10); 
              char c = Serial.read(); 
              string += c; 
              }  
              if (string.length() > 0)
              {
                //irrecv.enableIRIn();
                 Serial.println(string); 
                 digitalWrite(RED,LOW);
                 digitalWrite(GREEN,LOW);
                 digitalWrite(BLUE,HIGH);
               //  Serial.println("...........In Training Mode...........");
                 delay(300);
                 
                 if (irrecv.decode(&results)) 
                 {
                   // Serial.println("....In Receiver....");
                    storeCode(&results);
                    irrecv.resume(); // resume receiver
                    string="";
                 }
                  
              }
              //string="";
          }
}

void getData(String Data)
{
  myFile = SD.open(Data);
                  if (myFile) {
                   // Serial.println("data.txt:");
                   
                    while (myFile.available()) {
                      
                      ctype = myFile.readStringUntil('\n');
                      cdata = myFile.readStringUntil('\n');
                      len = myFile.readStringUntil('\n');
                      clen = len.toInt();
                      type = ctype.toInt();
                      //codeData = cdata.toInt();
                      cdata.toCharArray(d,clen);
                      codeData = strtol(d,NULL,16);
               //       Serial.println(ctype);
               //       Serial.println(codeData,HEX);
               //       Serial.println(clen);
                          
               /*       digitalWrite(RED,HIGH);
                      digitalWrite(GREEN,HIGH);
                      digitalWrite(BLUE,LOW);
                      delay(500);
                      digitalWrite(RED,HIGH);
                      digitalWrite(GREEN,HIGH);
                      digitalWrite(BLUE,HIGH);

                */
                     // delay(1000);
                    }
                    myFile.close();
                  } else {
                      type = 0 ;
                      codeData = 0 ;
                      clen = 0 ;
                      digitalWrite(RED,LOW);
                      digitalWrite(GREEN,HIGH);
                      digitalWrite(BLUE,HIGH);
                      delay(1000);
                      digitalWrite(RED,HIGH);
                      digitalWrite(GREEN,HIGH);
                      digitalWrite(BLUE,HIGH);
                     // Serial.println("error opening data.txt");
                  }
}

void sendCode(int codeType,long codeValue, int codeLen) 
{
      //codeValue = (codeValue,HEX);
      Serial.println("...............Send Funciton...............");
      Serial.println(codeType);
      Serial.println(codeValue);
      Serial.println(codeLen);

 // codeLen = clen.toInt();
 // cdata.toCharArray(d,len);
 // codeValue = strtol(d,NULL,16);

  if (codeType == 3) {//NEC
    
      irsend.sendNEC(codeValue, codeLen);
    digitalWrite(RED,HIGH);
    digitalWrite(GREEN,HIGH);
    digitalWrite(BLUE,LOW);
    delay(300);
    digitalWrite(RED,HIGH);
    digitalWrite(GREEN,HIGH);
    digitalWrite(BLUE,HIGH);
 //     Serial.print("Sent NEC ");
  //    Serial.println(codeValue, HEX);
    
  } 
  else if (codeType == 4) {//SONY
    irsend.sendSony(codeValue, codeLen);
    digitalWrite(RED,HIGH);
    digitalWrite(GREEN,HIGH);
    digitalWrite(BLUE,LOW);
    delay(300);
    digitalWrite(RED,HIGH);
    digitalWrite(GREEN,HIGH);
    digitalWrite(BLUE,HIGH);
  //  Serial.print("Sent Sony ");
  //  Serial.println(codeValue, HEX);
  } 
   if (codeType == 5) {//PANASONIC
   //     Serial.print("Sent Panasonic");

    //for(int i = 0 ; i < 3 ; i++ )
    {
      irsend.sendPanasonic(PanasonicAddress,codeValue);
    digitalWrite(RED,HIGH);
    digitalWrite(GREEN,HIGH);
    digitalWrite(BLUE,LOW);
    delay(300);
    digitalWrite(RED,HIGH);
    digitalWrite(GREEN,HIGH);
    digitalWrite(BLUE,HIGH);
    }
   /* Serial.println(codeValue, HEX);
    digitalWrite(RED,HIGH);
    digitalWrite(GREEN,HIGH);
    digitalWrite(BLUE,LOW);
    delay(1000);
    digitalWrite(RED,HIGH);
    digitalWrite(GREEN,HIGH);
    digitalWrite(BLUE,HIGH);
    */

  }
  else if (codeType == 6) { // JVC
    irsend.sendJVC(codeValue, codeLen, false);
    digitalWrite(RED,HIGH);
    digitalWrite(GREEN,HIGH);
    digitalWrite(BLUE,LOW);
    delay(300);
    digitalWrite(RED,HIGH);
    digitalWrite(GREEN,HIGH);
    digitalWrite(BLUE,HIGH);
  //  Serial.print("Sent JVC");
  //  Serial.println(codeValue, HEX);
  }


  else if (codeType == 7) { // SAMSUNG
    irsend.sendSAMSUNG(codeValue, codeLen);
    digitalWrite(RED,HIGH);
    digitalWrite(GREEN,HIGH);
    digitalWrite(BLUE,LOW);
    delay(300);
    digitalWrite(RED,HIGH);
    digitalWrite(GREEN,HIGH);
    digitalWrite(BLUE,HIGH);
  //  Serial.print("Sent JVC");
  //  Serial.println(codeValue, HEX);
  }

    else if (codeType == 8) { // Whynter
    irsend.sendWhynter(codeValue, codeLen);
    digitalWrite(RED,HIGH);
    digitalWrite(GREEN,HIGH);
    digitalWrite(BLUE,LOW);
    delay(300);
    digitalWrite(RED,HIGH);
    digitalWrite(GREEN,HIGH);
    digitalWrite(BLUE,HIGH);
  //  Serial.print("Sent JVC");
  //  Serial.println(codeValue, HEX);
  }

      else if (codeType == 9) { // AiwaRCT501
    irsend.sendAiwaRCT501(codeValue);
    digitalWrite(RED,HIGH);
    digitalWrite(GREEN,HIGH);
    digitalWrite(BLUE,LOW);
    delay(300);
    digitalWrite(RED,HIGH);
    digitalWrite(GREEN,HIGH);
    digitalWrite(BLUE,HIGH);
  //  Serial.print("Sent JVC");
  //  Serial.println(codeValue, HEX);
  }

      else if (codeType == 10) { // LG
    irsend.sendLG(codeValue , codeLen);
    digitalWrite(RED,HIGH);
    digitalWrite(GREEN,HIGH);
    digitalWrite(BLUE,LOW);
    delay(300);
    digitalWrite(RED,HIGH);
    digitalWrite(GREEN,HIGH);
    digitalWrite(BLUE,HIGH);
  //  Serial.print("Sent JVC");
  //  Serial.println(codeValue, HEX);
  }


        else if (codeType == 11) { // Sanyo
  //  irsend.sendLG(codeValue , codeLen);
    digitalWrite(RED,HIGH);
    digitalWrite(GREEN,HIGH);
    digitalWrite(BLUE,LOW);
    delay(300);
    digitalWrite(RED,HIGH);
    digitalWrite(GREEN,HIGH);
    digitalWrite(BLUE,HIGH);
  //  Serial.print("Sent JVC");
  //  Serial.println(codeValue, HEX);
  }

          else if (codeType == 13) { // DISH
    irsend.sendDISH(codeValue , codeLen);
    digitalWrite(RED,HIGH);
    digitalWrite(GREEN,HIGH);
    digitalWrite(BLUE,LOW);
    delay(300);
    digitalWrite(RED,HIGH);
    digitalWrite(GREEN,HIGH);
    digitalWrite(BLUE,HIGH);
  //  Serial.print("Sent JVC");
  //  Serial.println(codeValue, HEX);
  }


    else if (codeType == 14) { // Sharp
    irsend.sendSharpRaw(codeValue , codeLen);
    digitalWrite(RED,HIGH);
    digitalWrite(GREEN,HIGH);
    digitalWrite(BLUE,LOW);
    delay(300);
    digitalWrite(RED,HIGH);
    digitalWrite(GREEN,HIGH);
    digitalWrite(BLUE,HIGH);
  //  Serial.print("Sent JVC");
  //  Serial.println(codeValue, HEX);
  }


  else if (codeType == 15) { // Denon
    irsend.sendDenon(codeValue , codeLen);
    digitalWrite(RED,HIGH);
    digitalWrite(GREEN,HIGH);
    digitalWrite(BLUE,LOW);
    delay(300);
    digitalWrite(RED,HIGH);
    digitalWrite(GREEN,HIGH);
    digitalWrite(BLUE,HIGH);
  //  Serial.print("Sent JVC");
  //  Serial.println(codeValue, HEX);
  }



    else if (codeType == 17) { // LegoPowerFunctions
    irsend.sendLegoPowerFunctions(codeValue , false);
    digitalWrite(RED,HIGH);
    digitalWrite(GREEN,HIGH);
    digitalWrite(BLUE,LOW);
    delay(300);
    digitalWrite(RED,HIGH);
    digitalWrite(GREEN,HIGH);
    digitalWrite(BLUE,HIGH);
  //  Serial.print("Sent JVC");
  //  Serial.println(codeValue, HEX);
  }

  
  else if (codeType == 1 || codeType == 2) { // RC5 OR RC6
   
    // Add the toggle bit into the code to send
    codeValue = codeValue & ~(1 << (codeLen - 1));
    codeValue = codeValue | (toggle << (codeLen - 1));
    if (codeType == 1) {
    //  Serial.print("Sent RC5 ");
   //   Serial.println(codeValue, HEX);
      irsend.sendRC5(codeValue, codeLen);
    digitalWrite(RED,HIGH);
    digitalWrite(GREEN,HIGH);
    digitalWrite(BLUE,LOW);
    delay(300);
    digitalWrite(RED,HIGH);
    digitalWrite(GREEN,HIGH);
    digitalWrite(BLUE,HIGH);
    } 
    else {
      irsend.sendRC6(codeValue, codeLen);
    digitalWrite(RED,HIGH);
    digitalWrite(GREEN,HIGH);
    digitalWrite(BLUE,LOW);
    delay(300);
    digitalWrite(RED,HIGH);
    digitalWrite(GREEN,HIGH);
    digitalWrite(BLUE,HIGH);
    //  Serial.print("Sent RC6 ");
    //  Serial.println(codeValue, HEX);
    }
  } 
  else if (codeType == -1 /* i.e. raw */) {
    // Assume 38 KHz
//    digitalWrite(RED,HIGH);
    digitalWrite(RED,LOW);
    digitalWrite(GREEN,HIGH);
    digitalWrite(BLUE,HIGH);
    delay(300);
    digitalWrite(RED,HIGH);
    digitalWrite(GREEN,HIGH);
    digitalWrite(BLUE,HIGH);
  //  irsend.sendRaw(rawCodes, codeLen, 38);
 //   Serial.println("Sent raw");
  }

  //Serial.println(codeValue, HEX);

  Serial.println(codeValue, HEX);
//  type = 0 ;
//  codeData = 0 ;
//  clen = 0 ;
}
