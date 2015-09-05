/*

  // Materials
  Arduino Mega 2560 Rev3
  ITEAD PN532 NFC Module
  Arduino Ethernet Shield R3
  Cat5e Ethernet Patch Cable (15 Feet) - RJ45 Computer Networking Cord
  50 PCS White/Red/Blue/Yellow LED Electronics 5mm - 10 units 
  Veewon 3pcs Set Flexible 30cm Multicolored 40 Pin Male to Male,Male to Female,Female to Female Breadboard Jumper Wires Ribbon Cable (30cm)

  // LED STATUS
  40 true Blue
  41 false Red
  
  modified Thursday 23 July 2015
  modified Thursday 31 July 2015
  by Tuva Ergun
  
*/

#include <SPI.h> 
#include <HttpClient.h>
#include <Ethernet.h>
#include <EthernetClient.h>

#include <PN532_SPI.h>
#include <PN532.h>
#include <NfcAdapter.h>

#define DEBUG   // debug modunu açmak için define aktif edilmeli

  
  // Internet
  byte mac[] = { 0xFF, 0xAD, 0xBE, 0xEF, 0xFE, 0xEF };

  char server[] = "arduino.tuva.me";  
  const int kNetworkTimeout = 30*700;
  const int kNetworkDelay = 800;

  ///////////////////////
  String inString = "";
  String stringTwo = "";    // string to hold input
  String stringUrl = "/ping/EF3136F282CCC00C960F0F20F620BA15/";    // string to hold input
  String readString = "", readString1 = "";
  int x=0;
  char lf=10;
  ///////////////////////
 
  // NDEF nfc
  PN532_SPI pn532spi(SPI, 46);
  NfcAdapter nfc = NfcAdapter(pn532spi);
  

void setup()
{
 
  #ifdef DEBUG
    Serial.begin(115200); // sadece serial debug da gosterilir
    Serial.println("------[ BOOT START ]-----");
  #endif

  pinMode(40, OUTPUT);
  pinMode(41, OUTPUT);

  while (Ethernet.begin(mac) != 1){
    #ifdef DEBUG
      Serial.println("-> Error getting IP address via DHCP, trying again..."); // sadece serial debug da gosterilir
    #endif
    Ethernet.begin(mac);
    delay(500);
  }

  delay(2000);

  #ifdef DEBUG
    Serial.println("======[          TUVA  ]=====");
    Serial.println("------[ anfc  v0.3 ]-----");
    Serial.println("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n");
    Serial.print("IP Address        : ");
    Serial.println(Ethernet.localIP());
    Serial.print("Subnet Mask       : ");
    Serial.println(Ethernet.subnetMask());
    Serial.print("Default Gateway IP: ");
    Serial.println(Ethernet.gatewayIP());
    Serial.print("DNS Server IP     : ");
    Serial.println(Ethernet.dnsServerIP());
    Serial.println("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n");
  #endif
  
}


void loop(){
  
  nfc.begin();
  
  if (nfc.tagPresent()){
      
    NfcTag tag = nfc.read();
    
    stringTwo = tag.getUidString();
    stringTwo.replace(" ", ":");
    // D0:1E:75:11
    
    #ifdef DEBUG
      Serial.print("-> UID: ");Serial.println(tag.getUidString()); // sadece serial debug da gosterilir
      Serial.print("-> UID Format: ");Serial.println(stringTwo); // sadece serial debug da gosterilir
    #endif
    
    if (stringTwo){
      ethernetGET();  
    }
    
    // kart okuma başarılı ise bi sure işlemlerın tamamlanması için beklıyor.
    delay(200);
                
  } // nfc read döngusu

delay(200);

}

void ethernetGET(){

      inString = stringUrl + stringTwo;

      // ethernet baslıyor

      int err =0;
      
      EthernetClient c;
      HttpClient http(c);
          
      String newData;
      char buffer[80];
      newData=inString;         
      newData.toCharArray(buffer, 80); 

      #ifdef DEBUG
        Serial.println(buffer);
      #endif
        
      err = http.get(server, buffer);
      if (err == 0)
      {
        #ifdef DEBUG
        Serial.println("Started Request ok");
        #endif
    
        err = http.responseStatusCode();
        if (err >= 0)
        {
          #ifdef DEBUG
            Serial.print("Got status code: ");
            Serial.println(err);
          #endif
    
          // Usually you'd check that the response code is 200 or a
          // similar "success" code (200-299) before carrying on,
          // but we'll print out whatever response we get
    
          err = http.skipResponseHeaders();
          if (err >= 0)
          {
            int bodyLen = http.contentLength();
            
             #ifdef DEBUG
              Serial.print("Content length is: ");
              Serial.println(bodyLen);
              Serial.println();
              Serial.println("Body returned follows:");
             #endif
          
            // Now we've got to the body, so we can print it out
            unsigned long timeoutStart = millis();
            char c;
            // Whilst we haven't timed out & haven't reached the end of the body
            while ( (http.connected() || http.available()) &&
                   ((millis() - timeoutStart) < kNetworkTimeout) )
            {
                if (http.available())
                {
                    c = http.read();
                    
                    if (c != '\n') { 
                      readString += c;
                    }
                   
                    bodyLen--;
                    // We read something, reset the timeout counter
                    timeoutStart = millis();
                }
                else
                {
                    // We haven't got any data, so let's pause to allow some to
                    // arrive
                    delay(kNetworkDelay);
                }
            }

          // gelen cevaplar
               if (readString != "") {

                  #ifdef DEBUG
                    Serial.println("- - - - - - - - - - - - - - [ GELEN CEVAP ] - - - - - - - - - - - - - - - - - - - -");
                    Serial.println();  
                    Serial.println();
                    Serial.print("Current data row : " );
                    Serial.println(readString); 
                    Serial.println(); 
                    Serial.println();
                  #endif

                  readString.replace('[', ' ');
                  
                   if (readString.indexOf("true") > 0) {
                      #ifdef DEBUG
                        Serial.println("- - [ True deger ] - - -");  
                      #endif
                      digitalWrite(40, 225);
                      delay(800);
                      digitalWrite(40, 0); 
                    }
                    if (readString.indexOf("false") > 0) {
                      #ifdef DEBUG
                        Serial.println("- - [ False deger ] - - -");  
                      #endif
                      digitalWrite(41, 225);
                      delay(800);
                      digitalWrite(41, 0); 
                    }

                  #ifdef DEBUG
                    Serial.println(); 
                    Serial.println(); 
                    Serial.println("- - - - - - - - - - - - - - [ GELEN CEVAP ] - - - - - - - - - - - - - - - - - - - -");
                  #endif
                  
                  
                }
      
                readString1 = "";
                readString  = "";
          //gelen cevaplar

            
          }
          else
          {
            #ifdef DEBUG
              Serial.print("Failed to skip response headers: ");
              Serial.println(err);
            #endif
          }
        }
        else
        {    
          #ifdef DEBUG
            Serial.print("Getting response failed: ");
            Serial.println(err);
          #endif
        }
      }
      else
      {
        #ifdef DEBUG
          Serial.print("Connect failed: ");
          Serial.println(err);
        #endif
      }
      http.stop();
      
      delay(100);

    // Connect closed

    inString = "";
  
}



