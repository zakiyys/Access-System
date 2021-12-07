#include <SPI.h>
#include <Ethernet.h>
#include <Wiegand.h>

EthernetClient client;
int count;
long nokartudes;
String nokartu;

char server[] = "192.168.0.200";
char dataStr[12];
String hasildata;
char c;
int x = 0;
int pinrelay = 4, pinreset = 5;



void setup() {

  Ethernet.init(10);
  Serial.begin(9600);
  pinMode(pinrelay, OUTPUT);
  //pinMode(pinreset, OUTPUT);
  digitalWrite(pinreset, HIGH);
  digitalWrite(pinrelay, LOW);
  delay(100);
  pinMode(pinreset, OUTPUT);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // start the Ethernet connection:
  Serial.println("Initialize Ethernet with DHCP:");

  //ETHERNET MODULE INITIAL
  SPI.begin();        // Init SPI bus
  uint8_t mac[6] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05}; //MAC = 000102030405
  IPAddress mip(192, 168, 0, 199);                      //IP = 192.168.1.123
  IPAddress mdns(8, 8, 8, 8);                           //DNS = 8.8.8.8
  IPAddress mgate(192, 168, 0, 1);                      //GATEWAY = 192.168.1.5
  IPAddress msubnet(255, 255, 255, 0);                  //SUBNET = 255.255.255.0
  Ethernet.begin(mac, mip, mdns, mgate , msubnet);      //CONNECT USING ABOVE
  Serial.println("Succesful define connection");
  // END OF ETHERNET

  // give the Ethernet shield a second to initialize:
  delay(1000);

  WIEGAND wg;
  wg.begin();
  Serial.println("Baca Kartu");
  for (count = 0; count < 5; count++) {
    if (wg.available())
    {
      nokartudes = wg.getCode();
      nokartu = String(nokartudes, HEX);
      //nokartu = nokartudes;
      delay(500);
      Serial.println(" ");
      Serial.print("No Kartu : ");
      Serial.println(nokartu);
      Serial.print("No Kartu2 : ");
      Serial.println(wg.getCode(), HEX);
      Serial.print("Desimal  : ");
      Serial.println(wg.getCode());
      Serial.print("Type     : W");
      Serial.println(wg.getWiegandType());
      String queryString = String("?uid=") + String(nokartu);
      delay(1500);

      if (client.connect(server, 80)) {

        Serial.println("connected");
        Serial.println(queryString);
        client.println("GET /access-app/modul/data" + queryString + "&status=1" + " HTTP/1.1");
        client.println("Host: 192.168.0.200");
        client.println("Connection: close");
        Serial.println("Selesai Kirim HTTP GET");
        client.println(); // end HTTP header
      }
      else {
        Serial.println("connection failed");
        delay(500);
        digitalWrite(pinreset, LOW);
      }

      count = 6;

    }
    else {
      count = 0;
    }
  }
  Serial.println("Selesai Baca Kartu");



}




void loop() {
  while (client.connected() || client.available())
  {
    c = client.read(); //read first character
    while (c != '<') { //while < character is not coming yet, keep reading character
      c = client.read();
    }
    c = client.read(); //read the '<' character, but not storing in array
    while (c != '>') { //while > character is not coming yet,
      dataStr[x] = c; //store character in array
      c = client.read(); //read next character
      x++; //incrementing index array 
    }
    delay(500);

    bacadata();
    //akses();//print the character that has been captured by array at Serial Monitor
  }
  client.stop();  //stop connection

}



void bacadata() {
  for (x = 0; x < 12; x++) {
    Serial.print(dataStr[x]);
    hasildata = String(dataStr[7]);
  }
  Serial.println(""); Serial.print("hasil : "); Serial.print(hasildata);
  delay(2500);
  akses();
  Serial.println(""); Serial.print("Selesai");
  //digitalWrite(pinreset, LOW);
}

void akses() {
  if (hasildata == "1") {
    digitalWrite(pinrelay, HIGH);
    delay(5000);
    digitalWrite(pinrelay, LOW);
    //digitalWrite(pinreset, HIGH);
    Serial.print("pinrelay : "); Serial.print(pinrelay);
    digitalWrite(pinreset, LOW);
  }
  else {
    digitalWrite(pinrelay, LOW);
    //digitalWrite(pinreset, HIGH);
    digitalWrite(pinreset, LOW);
  }
}
