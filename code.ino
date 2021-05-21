// Jeu de cibles de Cowboys avec fusil a flechettes Nerf
/* On utilise un code légérement différent pour chaque cible, car le random sort tjs les mêmes chiffres :D
 * Pour info il faut appuyer 10 sec sur les boutons car il y a une pause 433MHz
 */

#include <Wire.h>
#include <VirtualWire.h>

// on initialise les variables:
int GameOver = 1; // si = 1 -> fin du jeu
int Temps_Debout = 0; // temps que le cowboy reste debout (diminue a chaque tour de loop) il est définit à 2000 + loin
int Temps_Hasard = 0; // temps d'attente avant de monter la cible (défini de facon aléatoire) il est définit + loin entre 10 et 15sec

int Led_GameOver = 4; // pin utilisée pour allumer la led du panneau de ctrl "GameOver"
int Led_433Mhz = 5; // pin utilisée pour allumer la led du panneau de ctrl "Attente msg 433Mhz"
int Bouton_Go_Stop = 6; // pin utilisée pour le bouton "Go/Stop"
int Bouton_Test = 7; // pin utilisée pour le bouton "Test Monter/Descendre"
int Moteur = 8; // pin utilisée pour les moteurs (HIGH: relais collé->cible monte, Relais au repos: la cible descend)
int Micro = 10; // pin utilisée pour le module mic (pin 10)
int Led_Pistolet = 13; // pin utilisée pour allumer la led du 6 coups

const char* MSG_DEBUT = "DEB"; // msg de début de jeu
const char* MSG_GAME_OVER = "GOV"; // msg de game over

void setup() {
  Serial.begin(57600);
  vw_setup(2000); // Initialisation de la bibliothèque VirtualWire
  //vw_set_tx_pin(12); // fonction qui definit la broche utilisée pour transmettre les données, par défaut il s'agit de la broche D12
  //vw_set_rx_pin(11); // fonction qui definit la broche utilisée pour recevoir les données, par défaut il s'agit de la broche D11
  vw_rx_start(); // On peut maintenant recevoir des messages
  Serial.println("Initialisation..");

  pinMode(Moteur, OUTPUT);// on met la pin du relais des moteurs en mode sortie
  pinMode(Led_Pistolet, OUTPUT);// on met la pin led pistolet en mode sortie
  pinMode(Led_433Mhz, OUTPUT);// on met la pin led 433Mhz en mode sortie
  pinMode(Led_GameOver, OUTPUT);// on met la pin led GameOver en mode sortie
  pinMode(Micro, INPUT_PULLUP);// on met la pin du micro en mode lecture AVEC la résistance pull-up activée
  pinMode(Bouton_Go_Stop, INPUT_PULLUP);// on met la pin du bouton Go/Stop en mode lecture AVEC la résistance pull-up activée
  pinMode(Bouton_Test, INPUT_PULLUP);// on met la pin du bouton test en mode lecture AVEC la résistance pull-up activée

  digitalWrite(Moteur,LOW); // on met la sortie moteur à 0
  digitalWrite(Led_GameOver,HIGH); // on allume la led game over tant que $gameover=1
  digitalWrite(Led_433Mhz,HIGH); // on allume la led d'attente msg 433Mhz

  digitalWrite(Led_Pistolet,HIGH); // on test la led du pistolet
  delay(200);
  digitalWrite(Led_Pistolet,LOW);
  delay(50);
  digitalWrite(Led_Pistolet,HIGH);
  delay(50);
  digitalWrite(Led_Pistolet,LOW);
  Serial.println("Test de la led pistolet: BANG!!");

  Serial.println("Début de la loop, on est en attente du message 433Mhz: 'debut de jeu'");
  Serial.println(" ");
}

void loop() {
  Serial.println(" ");
  byte message[VW_MAX_MESSAGE_LEN];
  byte taille_message = VW_MAX_MESSAGE_LEN; // la constante VW_MAX_MESSAGE_LEN est fournie par la lib VirtualWire

  // Attente d'appui sur les boutons ou de la reception du message "début de partie"
  while (GameOver == 1){ // while active jusqu'a ce qu'on appuie sur le bouton "début de jeu" ou qu'on recoive le msg "début de jeu" en 433Mhz
    Temps_Debout = 2000; // Changer cette valeur pour changer le temps que reste le cowboy debout (diminue de 50 a chaque tour de loop)
    digitalWrite(Led_433Mhz,HIGH); // on allume la led d'attente msg 433Mhz si ce n'est pas deja fait..
    if(digitalRead(Bouton_Test) == LOW) { // si bouton "Test Monter/Descendre" est appuyé
      // On va coller le relais pour faire monter la cible
      Serial.println("Le bouton 'Test Monter/Descendre' est appuyé -> on fait monter la cible");
      digitalWrite(Moteur,HIGH);
      digitalWrite(Led_433Mhz,LOW); // on eteint la led d'attente msg 433Mhz
      delay(1200); // On attend que la cible soit en haut avant d'écouter les bruits..
      for(int i = 1; i < 1000; i++) { // durant boucle de 1000 on test si du bruit est détecté par le micro
        Serial.println("Test de détection de bruit sur le micro");
        Serial.print("i: ");
        Serial.println(i);
        if (digitalRead(Micro) == LOW) {
          Serial.println("Bruit détecté -> descente de la cible");
          break;
        }
      }
      // On redescend la cible (soit à la fin de la boucle de 1000, soit avant s'il y a eu un bruit grace au break)
      digitalWrite(Moteur,LOW);
      Serial.println("Descente de la cible");
      Serial.println("Fin du test");
      digitalWrite(Led_433Mhz,HIGH); // on rallume la led d'attente msg 433Mhz
      Serial.println(" ");
    } // fin de si bouton "Test" est appuyé
    
    if(digitalRead(Bouton_Go_Stop) == LOW) { // si bouton "Emission: Go" est appuyé -> on émet le message "début de jeu"
      digitalWrite(Moteur,LOW); // on descend la cible
      digitalWrite(Led_GameOver,LOW); // on eteint la led game over
      digitalWrite(Led_433Mhz,LOW); // on eteint la led d'attente msg 433Mhz
      vw_send((byte*) MSG_DEBUT, strlen(MSG_DEBUT) + 1); // On envoie le message debut
      vw_wait_tx(); // On attend la fin de l'envoi
      Serial.println("Message de début de jeu: ENVOYE!");
      GameOver = 0; // on break la while
      Serial.println("- NOUVELLE PARTIE ----------------------");
    }else{ // sinon (si pas de pression sur les boutons: on attend un msg de debut de jeu durant 10sec
      //vw_wait_rx(); // attente sans timeout
      vw_wait_rx_max(10000); // timeout de 10sec
      if (vw_get_message(message, &taille_message)) {
        if (strcmp((char*) message, MSG_DEBUT) == 0) { // si on recoit le msg on break la while, sinon on réinterroge les 2 boutons..
          Serial.println("Message de début de jeu: RECU!");
          GameOver = 0; // on break la while
          digitalWrite(Led_GameOver,LOW); // on eteint la led game over
          digitalWrite(Led_433Mhz,LOW); // on eteint la led d'attente msg 433Mhz
          Serial.println("- NOUVELLE PARTIE ----------------------");
          // pour confirmer le début du jeu: tous les cowboys se lèvent (sauf si deja debout..) puis redescendent
          Serial.println("Pour confirmer le debut du jeu: on fait monter la cible, puis la redescend");
          digitalWrite(Moteur,HIGH);
          delay(4000); // On attend un instant afin que tous les cowboys se lèvent
          digitalWrite(Moteur,LOW);
        }
      }
    } // fin de si bouton "Go/Stop" est appuyé
  } // fin de while

  
  //-------------------DEBUT DU JEU------------------------
  // attente reception msg de GameOver avec un timeout aléatoire
  vw_wait_rx_max(10000); // timeout de 10sec
  if (vw_get_message(message, &taille_message)) {
    if (strcmp((char*) message, MSG_GAME_OVER) == 0) { // si on recoit le msg GameOver -> $GameOver = 1 -> fin du jeu
      Serial.println("Message de fin de jeu: RECU! -> Game Over!");
      GameOver = 1;
      digitalWrite(Led_GameOver,HIGH); // on allume la led game over
      digitalWrite(Led_433Mhz,HIGH); // on allume la led d'attente msg 433Mhz
      Serial.println("Pour confirmer la fin du jeu: on fait monter la cible");
      digitalWrite(Moteur,HIGH);
    }
  }


  // la cible remonte au bout d'un temps défini au hasard (+10sec de boucle d'attente du msg gameover)
  Temps_Hasard = random(5, 12); // temps entre 0 et 4 sec pour la cible 1
  //Temps_Hasard = random(8, 14); // temps entre 1 et 5 sec pour la cible 2
  //Temps_Hasard = random(8, 12); // temps entre 2 et 4 sec pour la cible 3
  Temps_Hasard = (Temps_Hasard * 1000); //on le met en milisec
  Serial.println("On attend entre 5 et 14 sec...");
  delay(Temps_Hasard);

  if(GameOver == 0){ // afin que la cible ne monte pas si on est game over..
    Serial.println("- NOUVELLE BOUCLE ----------------------");
    Serial.println(" ");
    Serial.println("On monte la cible");
    digitalWrite(Moteur,HIGH);
    delay(1200); // On attend que la cible soit en haut avant d'écouter les bruits..
  
    // Durant le boucle on test si du bruit est détecté par le micro
    for(int i = 1; i < Temps_Debout; i++) { // nb de boucle définit dans Temps_Debout, que l'on diminue à chaque cycle de void loop..
      Serial.println("Test de détection de bruit sur le micro");
      Serial.print("i: ");
      Serial.println(i);
      Serial.println(" ");
      if(digitalRead(Micro) == LOW) {
        Serial.println("Bruit détecté -> descente de la cible");
        digitalWrite(Moteur,LOW);
        break;
      }
      if(i == (Temps_Debout-1)){ // si $i atteint $Temps_Debout c'est qu'on a pas toucher le cowboy -> game over!
        Serial.println("Temps_Debout dépassé, c'est qu'on s'est fait tirer dessus -> Message de fin de jeu: ENVOYE!");
        digitalWrite(Led_GameOver,HIGH); // on allume la led game over
        Serial.println("Carte son non trouvée -> Pas pu jouer le 'BANG'..."); // ### JOUER UN SON "BANG" SUR LA CARTE SON!! ###
        digitalWrite(Led_Pistolet,HIGH); // on allume la led du pistolet
        delay(200);
        digitalWrite(Led_Pistolet,LOW);
        delay(50);
        digitalWrite(Led_Pistolet,HIGH);
        delay(50);
        digitalWrite(Led_Pistolet,LOW);
        Serial.println("Le pistolet a fait: BANG!!");
        vw_send((byte*) MSG_GAME_OVER, strlen(MSG_GAME_OVER) + 1); // On envoie le message gameover
        vw_wait_tx(); // On attend la fin de l'envoi
        delay(10000); // pause de 10 sec avant le renvoi d'un msg 433MHz afin augmenter les chances que les autres cibles le captent
        vw_send((byte*) MSG_GAME_OVER, strlen(MSG_GAME_OVER) + 1); // On renvoie le msg gameover (si une cible est en attente d'un son sur le micro, il ne sera pas recu)
        vw_wait_tx(); // On attend la fin de l'envoi
        delay(10000); // pause de 10 sec avant le renvoi d'un msg 433MHz afin augmenter les chances que les autres cibles le captent
        vw_send((byte*) MSG_GAME_OVER, strlen(MSG_GAME_OVER) + 1); // On renvoie le msg gameover (si une cible est en attente d'un son sur le micro, il ne sera pas recu)
        vw_wait_tx(); // On attend la fin de l'envoi
        GameOver = 1;
        digitalWrite(Led_433Mhz,HIGH); // on allume la led d'attente msg 433Mhz
      }
    }
    Temps_Debout = (Temps_Debout - 50);
    if(Temps_Debout < 200){
      Temps_Debout = 200;
    }
  } // fin de si game over = 0

  Serial.println("Fin boucle..");
  Serial.println(" ");
  delay(10); // pause CPU
}
