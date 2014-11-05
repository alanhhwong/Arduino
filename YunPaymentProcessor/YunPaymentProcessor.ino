#include "Adafruit_Thermal.h"
#include <SoftwareSerial.h>
#include <avr/pgmspace.h>
#include <Mailbox.h>
#include <aJSON.h>

const int PAPER_CHAR_WIDTH = 32;

int printer_RX_Pin = 5;  //green wire
int printer_TX_Pin = 6;  //yellow wire

Adafruit_Thermal printer(printer_RX_Pin, printer_TX_Pin);

void setup() {
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  // Initialize Bridge and Mailbox
  Bridge.begin();
  Mailbox.begin();
  // Initalize Printer
  pinMode(7, OUTPUT); digitalWrite(7, LOW); // To also work w/IoTP printer
  printer.begin();
  digitalWrite(13, LOW);

  // Initialize Serial
  Serial.begin(9600);
  
  // Wait until a Serial Monitor is connected.
  while (!Serial);

  Serial.println("Payment Processor Ready\n");
}

void loop() {
  String message;

  // if there is a message in the Mailbox
  if (Mailbox.messageAvailable())
  {
    // read all the messages present in the queue
    while (Mailbox.messageAvailable())
    {
      Mailbox.readMessage(message);
      Serial.println(message);
      printReceipt(message);
    }
    
    Serial.println("Waiting 5 seconds before checking the Mailbox again");
  }
  
  // wait 5 seconds
  delay(5000);
}

void printReceipt(String payment) {
  //parse products json
  char payment_char_array[payment.length() + 1];
  payment.toCharArray(payment_char_array, payment.length() + 1);
  aJsonObject* paymentObj = aJson.parse(payment_char_array);
  
  aJsonObject* total = aJson.getObjectItem(paymentObj, "total");
  aJsonObject* products = aJson.getObjectItem(paymentObj, "products");

  //Title
  printer.justify('C');
  printer.setSize('M');
  printer.underlineOn();
  printer.boldOn();
  printer.println("Commerce Factory SG");
  printer.boldOff();
  printer.underlineOff();
  printer.setSize('S');
  printer.println("Newton Circus\n237 South Bridge Road, Singapore");
  printer.feed(1);
 
  //Item
  printer.justify('L');
  for (char i = 0; i < aJson.getArraySize(products); i++) {
    aJsonObject* title = aJson.getObjectItem(aJson.getArrayItem(products, i), "t");
    aJsonObject* quantity = aJson.getObjectItem(aJson.getArrayItem(products, i), "q");
    aJsonObject* price = aJson.getObjectItem(aJson.getArrayItem(products, i), "p");
    
    printLineItem(title->valuestring, quantity->valuestring, price->valuestring);
  }

  //Total
  printLineItem("", "", "======");
  printLineItem("TOTAL (SGD)", "", total->valuestring);
  
  //Footer
  printer.feed(1);
  printer.print("********************************\nEmail me: ");
  printer.underlineOn();
  printer.println("alawong@paypal.com");
  printer.underlineOff();
  printer.println("Follow me: @alanhhwong");
  printer.println("********************************");
  printer.feed(2);

}

void printLineItem(String title, String quantity, String cost) {
  printer.print(title);
  if (quantity.length() > 0) {
    quantity = " x" + quantity;
    printer.print(quantity);
  }
  
  for (char i = 0; i < PAPER_CHAR_WIDTH - title.length() - quantity.length() - cost.length(); i++) {
    printer.print(" ");
  }
  printer.println(cost);
}
