void setup() {
  // Inicialitza la comunicació sèrie
  Serial.begin(9600);

  // Inicialitza el generador de números aleatoris amb una llavor basada en el pin analògic A0
  randomSeed(analogRead(A0));
}

void loop() {
  // Genera un número aleatori entre 0 i 100
  int numeroAleatori = random(0, 101);

  // Envia el número pel port sèrie
  //Serial.print("Número aleatori: ");
  Serial.println(numeroAleatori);

  // Espera un segon abans de repetir
  delay(1000);
}