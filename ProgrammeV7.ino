
  #include <Ticker.h>
  
  const int BParcade[7] ={2,3,4,5,6,7,8}; //pins associé aux boutons arcade dans l'ordre suivant: interupteur,ampoule,fil,pile,mode junior,mode pro ,mode genie

  //pin sur lequelle on envoie une PWM pour faire varrier la tension moyenne
  #define pile 9

  //pin qui vont simulé la perte de continuité durant la panne
  #define PanneInteru 10
  #define PanneAmpoule 11
  #define PanneFil 12


  #define pileIN 14 //permet de savoir si les piles sont en place
  #define InterruON 15 //permet de savoir si l'interupteur est en ON ou OFF


  const int ledBP[7] ={16,17,18,19,20,21,22};//pin des led lié au bouton d'arcade
  
  #define ampoule 26 //vrais ampoule qu'on éteind a chaque panne ou si la pile est retiré ou si l'interrupteur est sur off

  //variable qui sert au switch case
  int etat = 0;


  //variables qui servent aux pannes aléatoires
  int randNumber;
  int randNumber2;

  int compteur =0;  //compteur qui sert a la remise a zero
  Ticker tickeridle; //declaration du ticker


//declarations nececaire au DFPlayer

/***************************************************
DFPlayer - A Mini MP3 Player For Arduino
 <https://www.dfrobot.com/product-1121.html>
 
 ***************************************************
 This example shows the basic function of library for DFPlayer.
 
 Created 2016-12-07
 By [Angelo qiao](Angelo.qiao@dfrobot.com)
 
 GNU Lesser General Public License.
 See <http://www.gnu.org/licenses/> for details.
 All above must be included in any redistribution
 ****************************************************/

/***********Notice and Trouble shooting***************
 1.Connection and Diagram can be found here
 <https://www.dfrobot.com/wiki/index.php/DFPlayer_Mini_SKU:DFR0299#Connection_Diagram>
 2.This code is tested on Arduino Uno, Leonardo, Mega boards.
 ****************************************************/

#include "Arduino.h"
#include "DFRobotDFPlayerMini.h"
#if (defined(ARDUINO_AVR_UNO) || defined(ESP8266))   // Using a soft serial port
#include <SoftwareSerial.h>
SoftwareSerial softSerial(/*rx =*/1, /*tx =*/0); //utilise les pin 1 et 0 comme rx et tx en UART
#define FPSerial softSerial
#else
#define FPSerial Serial1
#endif
DFRobotDFPlayerMini myDFPlayer;



void setup() {
  
  Serial.begin(9600);
  

  // declaration des bouton arcade et leds qui leur sont associer
  for (int i=0;i<7;i++)
  {
    pinMode(BParcade[i],INPUT_PULLUP);
    pinMode(ledBP[i],OUTPUT);
  } 
  pinMode(PanneInteru,OUTPUT);
  pinMode(PanneAmpoule,OUTPUT);
  pinMode(PanneFil,OUTPUT);

  pinMode(ampoule,OUTPUT);//vrai ampoule
  pinMode(pileIN,INPUT_PULLUP);
  pinMode(InterruON,INPUT_PULLUP);

  randomSeed(analogRead(0));//base la generation aléatoire sur le pin A0

  tickeridle.attach(1, idle);// toutes les 1s appelle la fonction idle


//parti DFplayer
  #if (defined ESP32)
  FPSerial.begin(9600, SERIAL_8N1, /*rx =*/D3, /*tx =*/D2);
#else
  FPSerial.begin(9600);
#endif

  Serial.begin(115200);

  if (!myDFPlayer.begin(FPSerial, /*isACK = */true, /*doReset = */true)) {  //Use serial to communicate with mp3.

    while(true){
      delay(0); // Code to compatible with ESP8266 watch dog.
    }
  }

  myDFPlayer.play(5);  //son d'initialisation
  myDFPlayer.volume(1);  //regle le volume de 0 à 30
  //En utilisant myDFPlayer.play(x) la valeur x correspond à un des fichier mp3 enregistré sur la carte microSD
  //les mp3 actuelement sur la carte correspondent à dans l'ordre  1:son d'activation ; 2: son de trompette defaite ; 3:son de felicitation ; 4:son d'erreur ; 5:son d'initialisation;

//continuité
  // pinMode(enabuzz,INPUT_PULLUP);
  // pinMode(buzz,OUTPUT);


}


// fonction appelé via ticker
void idle()
{
  compteur++;
}



void loop() {
 
  switch (etat) {
  case 0://etat de depart qui demande de choisir un des 3 modes de difficultés
    
  // remet a zero toutes les pannes
  digitalWrite(PanneAmpoule, HIGH);
  digitalWrite(PanneFil, HIGH);
  digitalWrite(PanneInteru, HIGH);
  
  analogWrite(pile,255);//met la tension de la pile à 3.3V 

  //allume toutes les leds arcade
  for (int i=0;i<7;i++)
  {
    digitalWrite(ledBP[i], HIGH);
  }

 
    
  Serial.println("Choisi un mode de difficulté");
    
         
        if(digitalRead(BParcade[4])==LOW)
        {
          compteur = 0;
          etat=1;
          myDFPlayer.play(1);// Son de selection de difficulté
        }
        
       
        if(digitalRead(BParcade[5])==LOW)
        {
          randNumber = random(0,4);//génere un nombre aléatoire de 0 à 3 qui sont les numéro de pin des bouton de validation
          compteur = 0;
          etat=2;
          myDFPlayer.play(1);
        }
      
        
        if(digitalRead(BParcade[6])==LOW)
        {
          randNumber = random(0,4);//génere un nombre aléatoire de 0 à 3
          randNumber2 = random(0,4);//génere un autre nombre aléatoire de 0 à 3
          
          while(randNumber2 == randNumber)//re-gener le deuxieme nombre si il est egale au premier
          {
            randNumber2 = random(0,4);
          }
          compteur = 0;
          etat=3;
          myDFPlayer.play(1);
        }
        
 delay(50);
 
    break;


    
  case 1://mode junior qui ne test que la pile
        
    Serial.println("La pile a un probleme");
    analogWrite(pile,80); //met la tension de la pile à 1,6/
          
    for (int i=0;i<4;i++) //chaque itération verifie un bouton à la suite
    {
      if(digitalRead(BParcade[i])==LOW)// si le bouton que l'on verifie est pressé
      {
        if(i==3)//si ce bouton est celui des pile
        {
         Serial.println("Bien joué");
         myDFPlayer.play(3);// Son de felicitation
         digitalWrite(ledBP[i], HIGH);// laisse le bouton allumé quelques secondes
         delay(2000);
         digitalWrite(ledBP[i], LOW);
          etat=0;
        }
        else
        {
          Serial.println("mauvaise reponse");
          myDFPlayer.play(4);
          digitalWrite(ledBP[i], HIGH);//allume le bouton le temps de l'appuie
          delay(200);
          digitalWrite(ledBP[i], LOW);
        }
      }  
    }

    if(digitalRead(BParcade[5])==LOW ) //permet de changer le mode de difficulté
    {
      etat=2;
      myDFPlayer.play(1);
    }
    else if(digitalRead(BParcade[6])==LOW)
    {
      etat=3;
      myDFPlayer.play(1);
    }
      
      //allume la led du mode actuel et eteind les autres
      for (int i=0;i<4;i++)
      {
        digitalWrite(ledBP[i], LOW);
      }
      digitalWrite(ledBP[4], HIGH);
      digitalWrite(ledBP[5], LOW);
      digitalWrite(ledBP[6], LOW);

      delay(50);  


    break;



    
    case 2://mode pro qui simule une panne aléatoire

    Serial.println("Il y a une panne");

    //panne qui survient en fonction du nombre aléatoire généré
    if(randNumber==0)
    {
     
     digitalWrite(PanneInteru,LOW);
    }
    else if (randNumber==1)
    {
      
      digitalWrite(PanneAmpoule,LOW);
    }
    else if (randNumber==2)
    { 
     
      digitalWrite(PanneFil,LOW);
    }

    else if (randNumber==3)
    { 
     
      analogWrite(pile,80);
    }

    
    for (int i=0;i<4;i++) //chaque itération verifie un bouton à la suite
    {
      if(digitalRead(BParcade[i])==LOW)// si le bouton que l'on verifie est pressé
      {
        if(i==randNumber)//si ce bouton est celui liés à la panne aléatoire
        {
         Serial.println("Bien joué");
         myDFPlayer.play(3);
         digitalWrite(ledBP[i], HIGH);
         delay(2000);
         digitalWrite(ledBP[i], LOW);
         etat=0;
        }
        else
        {
          Serial.println("mauvaise reponse");
          myDFPlayer.play(4);
          digitalWrite(ledBP[i], HIGH);
          delay(200);
          digitalWrite(ledBP[i], LOW);
        }
      }  
    }

    //permet de changer le mode de difficulté
    if(digitalRead(BParcade[4])==LOW )
    {
      etat=1;
      myDFPlayer.play(1);
    }
    else if(digitalRead(BParcade[6])==LOW)
    {
      etat=3;
      myDFPlayer.play(1);
    }
      delay(50);

      //allume la led du mode actuel et eteind les autres
      for (int i=0;i<5;i++)
      {
        digitalWrite(ledBP[i], LOW);
      }
      digitalWrite(ledBP[5], HIGH);
      digitalWrite(ledBP[6], LOW);

      delay(50);
    break;





    case 3: //mode genie qui simule deux pannes aléatoire
        if (randNumber==0 || randNumber2 == 0)//panne de l'interrupteur qui survient en fonction du nombre aléatoire généré
    {
     digitalWrite(PanneInteru,LOW);
    }

    if (randNumber==1 || randNumber2 == 1)//panne de l'ampoule qui survient en fonction du nombre aléatoire généré
    {
      digitalWrite(PanneAmpoule,LOW);
    }

    if (randNumber==2 || randNumber2 == 2)//panne du fil qui survient en fonction du nombre aléatoire généré
    {
      digitalWrite(PanneFil,LOW);
    }

    if (randNumber==3 || randNumber2 == 3)//panne du fil qui survient en fonction du nombre aléatoire généré
    {
      analogWrite(pile,80);
    }





    for (int i=0;i<4;i++) //chaque itération verifie un bouton a la suite
    {
      if(digitalRead(BParcade[i])==LOW)// si le bouton que l'on verifie est pressé
      {
        if(i==randNumber)//si ce bouton est celui liés a la panne aléatoire
        {
         Serial.println("tu a trouver une panne sur 2");
         digitalWrite(ledBP[i], HIGH);
         myDFPlayer.play(1);
         delay(500);
         etat=5;
        }
        else if (i==randNumber2)
        {
        Serial.println("tu a trouver une panne sur 2");
         digitalWrite(ledBP[i], HIGH);
         myDFPlayer.play(1);
         delay(500);
         etat=4;
        }

        else
        {
          Serial.println("mauvaise reponse");
          myDFPlayer.play(4);
          digitalWrite(ledBP[i], HIGH);
          delay(200);
          digitalWrite(ledBP[i], LOW);
        }
      }  
    }



     if(digitalRead(BParcade[4])==LOW ) //permet de change le mode de dificulté
    {
      etat=1;
      myDFPlayer.play(1);
    }
    else if(digitalRead(BParcade[5])==LOW)
    {
      etat=2;
      myDFPlayer.play(1);
    }

    //allume la led du mode actuel et eteind les autres
      for (int i=0;i<6;i++)
      {
        digitalWrite(ledBP[i], LOW);
      }
    digitalWrite(ledBP[6], HIGH);

    delay(50);

        break;





  case 4:// sous-etat du mode genie qui attend la deuxieme bonne reponse

    if (randNumber==0 || randNumber2 == 0)//panne de l'interrupteur qui survient en fonction du nombre aléatoire généré
    {
     digitalWrite(PanneInteru,LOW);
    }
    if (randNumber==1 || randNumber2 == 1)//panne de l'ampoule qui survient en fonction du nombre aléatoire généré
    {
      digitalWrite(PanneAmpoule,LOW);
    }
    if (randNumber==2 || randNumber2 == 2)//panne du fil qui survient en fonction du nombre aléatoire généré
    {
      digitalWrite(PanneFil,LOW);
    }
    if (randNumber==3 || randNumber2 == 3)//panne du fil qui survient en fonction du nombre aléatoire généré
    {
      analogWrite(pile,80);
    }


    
    for (int i=0;i<4;i++) //chaque itération verifie un bouton a la suite
    {
      if(digitalRead(BParcade[i])==LOW)// si le bouton que l'on verifie est pressé
      {
        if(i==randNumber)//si ce bouton est celui liés a la panne aléatoire
        {
         Serial.println("Bien joué");
         myDFPlayer.play(3);
         digitalWrite(ledBP[i], HIGH);
         delay(2000);
          digitalWrite(ledBP[i], LOW);
          etat=0;
        }
        else
        {
          Serial.println("mauvaise reponse");
          myDFPlayer.play(4);
          digitalWrite(ledBP[i], HIGH);
          delay(500);
          digitalWrite(ledBP[i], LOW);
        }
      }  
    }


     if(digitalRead(BParcade[4])==LOW ) //permet de change le mode de dificulté
    {
      etat=1;
      myDFPlayer.play(1);
    }
    else if(digitalRead(BParcade[5])==LOW)
    {
      etat=2;
      myDFPlayer.play(1);
    }

    //allume la led du mode actuel et eteind les autres

    digitalWrite(ledBP[randNumber2], HIGH);

    delay(50);

        break;



  case 5:// sous-etat du mode genie qui attend la deuxieme bonne reponse

    if (randNumber==0 || randNumber2 == 0)//panne de l'interrupteur qui survient en fonction du nombre aléatoire généré
    {
     digitalWrite(PanneInteru,LOW);
    }
    if (randNumber==1 || randNumber2 == 1)//panne de l'ampoule qui survient en fonction du nombre aléatoire généré
    {
      digitalWrite(PanneAmpoule,LOW);
    }
    if (randNumber==2 || randNumber2 == 2)//panne du fil qui survient en fonction du nombre aléatoire généré
    {
      digitalWrite(PanneFil,LOW);
    }
    if (randNumber==3 || randNumber2 == 3)//panne du fil qui survient en fonction du nombre aléatoire généré
    {
      analogWrite(pile,80);
    }


    
    for (int i=0;i<4;i++) //chaque itération verifie un bouton a la suite
    {
      if(digitalRead(BParcade[i])==LOW)// si le bouton que l'on verifie est pressé
      {
        if (i==randNumber2)
        {
        Serial.println("Bien joué");
        myDFPlayer.play(3);
        digitalWrite(ledBP[i], HIGH);
        delay(2000);
        digitalWrite(ledBP[i], LOW);
        etat=0;
        }
        else
        {
          Serial.println("mauvaise reponse");
          myDFPlayer.play(4);
          digitalWrite(ledBP[i], HIGH);
          delay(500);
          digitalWrite(ledBP[i], LOW);
        }
      }  
    }

     if(digitalRead(BParcade[4])==LOW ) //permet de change le mode de dificulté
    {
      etat=1;
      myDFPlayer.play(1);
    }
    else if(digitalRead(BParcade[5])==LOW)
    {
      etat=2;
      myDFPlayer.play(1);
    }

    //allume la led du mode actuel et eteind les autres
 
    digitalWrite(ledBP[randNumber], HIGH);

    delay(50);

        break;

      
    default:
    etat=0;
    myDFPlayer.play(2);

  }
  //fin du case


//si on appuie sur un bouton d'erreur le timer est remis à 0
  if(digitalRead(BParcade[0])==LOW || digitalRead(BParcade[1])==LOW || digitalRead(BParcade[2])==LOW ||digitalRead(BParcade[3])==LOW)
  {
    compteur = 0;
  }


  // si le compteur atteind les 300s (5 min) d'inactivité on est renvoyé à l'etat de départ
  if (compteur == 300 )
  {
    Serial.println("Temps ecoulé");
    etat=0;
    compteur = 0;
    
  }

  //allume l'ampoule si on est dans l'etat de selection de difficulté ET que les piles sont en place ET que l'interupteur est sur ON
  if(etat==0 && digitalRead(pileIN) ==LOW && digitalRead(InterruON) ==HIGH )
  {
    digitalWrite(ampoule,HIGH);
  }
  else
  {
    digitalWrite(ampoule,LOW);
  }

//testeur de continuité
  // if(digitalRead(enabuzz)==LOW)
  // {
  //   analogWrite(buzz,240);
  //   Serial.println("continuité");
  // }
  // else{
  //   analogWrite(buzz,0);
  // }
  
}