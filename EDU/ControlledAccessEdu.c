//Librerias
#include <Keypad.h>
#include <LiquidCrystal.h>
#include <Servo.h>

// LiquidCrystal setup (Pantalla)
// lista de numeros enteros(pines para las conexiones de la pantalla)
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
// Instancia de LiquidCrytstal con los pines asignados
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Numpad
// Declaramos que nuestro numpad es de 4x4
const byte ROWS = 4;
const byte COLS = 4;

// Arreglo 2D para los valos del numpad
char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

//Define los pines usados para las columnas y filas del numpad
byte rowPins[ROWS] = { 13, 10, 9, 8 };
byte colPins[COLS] = { 7, 6, 0, 1 };

// Intancias del constomKeypad con todos los valores previos
Keypad customKeypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Motor servo
Servo door_servo;

// Entradas del usuario
char pinArray[5];  // 4 digits + null terminator for safety
byte pinCount = 0;
// Número pin que permite el acceso
const char setPin[5] = "0420";

/*
Ya terminamos de definir todas la variables necesarias para nuestro programa.

A partir de este punto es la definición de pura logica trabajando con las variables establecidas previamente

OJO ---> ES LA DEFINICIÓN, AUN NO ESTAMOS USANDO ESTA FUNCIONES. LAS FUNCIONES SERA UTILIZADAS MAS ADELANTE
DENTRO DEL PROGRAMA PRINCIPAL loop() En la parte inferior del código.
*/

//Set up básico del arduino
void setup() {
  // Inicion de pantalla LCD, declaramos que la pantalla que estamos usando es de 16 columnas con 2 filas
  lcd.begin(16, 2);
  // El motor servo está conectado al pin Analogo 0
  door_servo.attach(A0);
  // El motor servo debe comenzar en 0 grados (sin movimineto inicial)
  door_servo.write(0);
}

// Lógica para validar PIN
bool validatePin() {
  // En la pantalla LCD escribir "Enter pin: "
  lcd.print("Enter pin:");
  // Mientras el numero de digitos del pin sean menores a 4, repetir el codigo dentro de do{}whilel()
  do {
    // Variable qué guarda el ultimo digito de entrada
    char pinInput = customKeypad.getKey();
    // Si se detecta un digito de entrada, alojarlo en el arreglo en la pocisión pinCount(0,1,2,3)
    if (pinInput) {
      pinArray[pinCount] = pinInput;
      // Cada vez que se presione un nuevo digito, el la pantalla LCD escribimos un (*) no el digito por "Seguridad"
      lcd.print('*');
      // Aumnetamos en 1 la variable que nos dice cuantos digitos del pin hemos propocionado
      pinCount++;
    }
  } while (pinCount < 4);

  // Por seguridad en C, el ultimo elemento del areglo debe de ser un null terminator (\0)
  pinArray[4] = '\0';  // Null-terminate safely

  //Lipiamos pantalla y regresamos en cursor de la pantalla a la pocisión (0,0)
  lcd.clear();
  lcd.setCursor(0, 0);

  // Comparamos los arreglos del pin valid vs el pin de entrada para ver si son iguales
  if (memcmp(pinArray, setPin, 4) == 0) {
    // Si son igulaes mostramos un mesaje de accesso en la pantalla
    lcd.print("Access Granted");
    // Limpiamos el historial del pin con la función resetPinInput() definida más abajo
    resetPinInput();
    // Retornamos TRUE para indicar que el pin fue validado correctamente
    return true;
  } else {
    // Si el pin no fue correcto, mostramos mensaje de acceso negado
    lcd.print("Access Denied");
    // Limpiamos el historial del pin con la función resetPinInput() definida más abajo
    resetPinInput();
    // Retornamos FALSE para indicar que el pin no fue validado correctamente
    return false;
  }
}

// Función previamente mencionada para limpiar le historial del pin
void resetPinInput() {
  // Regresamos a 0 digitos registrados
  pinCount = 0;
  // Pequeña pausa
  delay(1500);
  // Limpiamos la pantalla
  lcd.clear();
}

//RFID Modulo de la tarjete de acceso 
/*
Aquí va el código del modulo del RID, por ahora solamente lo esta simulando
NO SE PROCUPEN POR ESTA SECCIÓN AHORITA JAJAJJAJAAJ
*/
void validateCard(){
  lcd.print("Please scan card");
  delay(3000);
  //logic for card scanner here//
  lcd.clear();
  lcd.print("Scanning..");
  delay(3000);
  lcd.clear();
}


// Motor servo
/*Estas dos funciones simplemente utilzan una iteración para decirle cuantos grados mover en motor servo y cada cuando hacerlo
openDoor() va de 0 a 90 y closeDoor de 90 a 0. La funcion cycleDoor() simplemente llama a las dos funciones una despues de la otra.
*/
void openDoor(){
  for (int angle = 0; angle <= 90; angle += 1) {
    door_servo.write(angle);
    delay(15);
  }
}

void closeDoor(){
  for (int angle = 90; angle > 0; angle -= 1){
    door_servo.write(angle);
    delay(15);
  }
}

void cycleDoor(){
  openDoor();
  delay(3000);
  closeDoor();
}

/*
ESTO ES EL PROGRAMA PRINCIPAL 
*/

void loop() {
  // Validamos la tarjeta con la función previamente definida
  validateCard();
  // Una vez validad la tajeta podemos intentar validar el pin
  // Si el pin es válido, abrir y cerrar la puerta con la función cycleDoor()
  if (validatePin()) {
    lcd.print("Opening door");
    cycleDoor();
    //Temp: avita que el programa se repita, no se preocupen por esto ahorita
    while(true);
    // En caso de que el pin no sea válido, mostrar mensaje indicandolo. 
  } else {
    lcd.print("Please Try again");
    delay(2500);
    lcd.clear();
  }
}