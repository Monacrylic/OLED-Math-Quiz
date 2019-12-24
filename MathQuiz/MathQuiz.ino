/*
 * A simple math quiz game that can be played on an OLED screen.
 * The game uses a potentiometer and a pushbutton for user INPUT.
 * It generates questions in a using a defined format but with random numbers.
 * For circuit diagram and other details visit www.curiousmotor.com
 *
 */


#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define PUSHBUTTON 2 // Pushbutton is connected to pin 2
#define POTENTIOMETER A0 //Potentiometer output is wired to A0
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int score= 0; // Keep track of score
int correctAnswer; //correct solution of the question
String equation; // String to store the question
int x,y,z; //variables for numbers in questions, declared here for global scope
int ansUpperLimit; // upperlimit for potentiometer value
int ansLowerLimit; // lowerlimit for potentiometer value
volatile int selectedAnswer; // answer selected by user
#define LOGO_HEIGHT   64
#define LOGO_WIDTH    128
// Below is the splash screen for the curiousmotor logo- (can be removed)
const unsigned char logo_bmp [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x2e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x2c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3a, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3b, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x74, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0xf4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x17, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x01, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x77, 0x40, 0x00, 0x00, 
  0x00, 0x00, 0x03, 0xbf, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x77, 0x40, 0x00, 0x00, 
  0x00, 0x00, 0x03, 0xbf, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x77, 0x40, 0x00, 0x00, 
  0x00, 0x00, 0x03, 0xbf, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x77, 0xe0, 0x00, 0x00, 
  0x00, 0x00, 0x03, 0xbf, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7d, 0xa0, 0x00, 0x00, 
  0x00, 0x00, 0x02, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xa0, 0x00, 0x00, 
  0x00, 0x00, 0x02, 0xfd, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xa0, 0x00, 0x00, 
  0x00, 0x00, 0x02, 0x7d, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0x20, 0x00, 0x00, 
  0x00, 0x00, 0x0e, 0xff, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xbf, 0xb0, 0x00, 0x00, 
  0x00, 0x00, 0x0e, 0xbe, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0xae, 0xb0, 0x00, 0x00, 
  0x00, 0x00, 0x06, 0xba, 0x28, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x75, 0x30, 0x00, 0x00, 
  0x00, 0x00, 0x07, 0xd5, 0x58, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 0xd5, 0x70, 0x00, 0x00, 
  0x00, 0x00, 0x05, 0xf4, 0x9c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 0xab, 0xe0, 0x00, 0x00, 
  0x00, 0x00, 0x05, 0xfa, 0xd4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x19, 0x0f, 0x60, 0x00, 0x00, 
  0x00, 0x00, 0x02, 0xf9, 0x76, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2f, 0x4f, 0xa0, 0x00, 0x00, 
  0x00, 0x00, 0x0f, 0x6c, 0x33, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6e, 0x1f, 0x58, 0x00, 0x00, 
  0x00, 0x00, 0x07, 0xd6, 0x39, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xca, 0x6e, 0xf0, 0x00, 0x00, 
  0x00, 0x00, 0x04, 0xed, 0xa8, 0x80, 0x00, 0x01, 0x20, 0x00, 0x00, 0x93, 0xd3, 0xb0, 0x00, 0x00, 
  0x00, 0x00, 0x02, 0xb2, 0x64, 0x80, 0x00, 0x09, 0x20, 0x00, 0x00, 0x86, 0x24, 0xa0, 0x00, 0x00, 
  0x00, 0x00, 0x02, 0x78, 0x91, 0x80, 0x00, 0x05, 0xe8, 0x00, 0x00, 0x84, 0x8f, 0x60, 0x00, 0x00, 
  0x00, 0x00, 0x05, 0x7c, 0x19, 0x00, 0x00, 0x04, 0x10, 0x00, 0x00, 0xc8, 0x3f, 0xf0, 0x00, 0x00, 
  0x00, 0x00, 0x03, 0xf1, 0xc1, 0x00, 0x00, 0x14, 0x10, 0x00, 0x00, 0x41, 0xc7, 0xe0, 0x00, 0x00, 
  0x00, 0x00, 0x03, 0xe6, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4c, 0x37, 0x40, 0x00, 0x00, 
  0x00, 0x00, 0x01, 0x50, 0x83, 0x00, 0x00, 0x04, 0x10, 0x00, 0x00, 0x44, 0x0d, 0xc0, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0xbc, 0x03, 0x00, 0x00, 0x05, 0xd0, 0x00, 0x00, 0x40, 0x1f, 0x80, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x7f, 0xc3, 0x00, 0x00, 0x02, 0xb0, 0x00, 0x00, 0x41, 0xff, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x3f, 0x01, 0x00, 0x00, 0x09, 0xa0, 0x00, 0x00, 0x44, 0xfe, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x1c, 0x01, 0x00, 0xe0, 0x01, 0xe0, 0x00, 0x00, 0x40, 0x18, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x0f, 0x41, 0x01, 0x90, 0x00, 0x20, 0x00, 0x00, 0xc3, 0x68, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x08, 0xf9, 0x83, 0x00, 0x00, 0xf3, 0xc0, 0x00, 0x8f, 0x98, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x05, 0xf8, 0xf2, 0x00, 0x1c, 0xf2, 0x60, 0xef, 0x8f, 0xd0, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x02, 0x7e, 0x7a, 0x04, 0x72, 0xf6, 0x31, 0x8f, 0x3f, 0x60, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x01, 0xfc, 0x0a, 0x06, 0x70, 0xf4, 0x31, 0x88, 0x1f, 0x80, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x19, 0xfa, 0x02, 0x70, 0x74, 0x71, 0x4f, 0x8c, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x0f, 0x03, 0x1e, 0xf0, 0x73, 0xd3, 0x20, 0x78, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xf3, 0xd0, 0x70, 0x0f, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x6f, 0x21, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x79, 0xa1, 0x37, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0xf1, 0xa2, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16, 0xf1, 0xfa, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x13, 0xf1, 0xe2, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x13, 0x7f, 0x27, 0xe8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x3c, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


//Function to draw the curiousmotor splash screen
void testdrawbitmap(void) {
  display.clearDisplay();
  display.drawBitmap(
    (display.width()  - LOGO_WIDTH ) / 2,
    (display.height() - LOGO_HEIGHT) / 2,
    logo_bmp, LOGO_WIDTH, LOGO_HEIGHT, 1);
  display.display();
  delay(1000);
} 

void setup() {
  Serial.begin(9600);
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
      Serial.println(F("SSD1306 allocation failed"));
      for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay(); 
  display.display();
  testdrawbitmap();    
  delay(3000);
  display.clearDisplay();
  display.setTextWrap(true);
 
  pinMode(PUSHBUTTON, INPUT_PULLUP); // Declare the pushbutton as input
  newQuestion();
}

void loop() {

displayEquation(equation);
displayAnswerOptions();
display.display();

if(digitalRead(PUSHBUTTON)==LOW){
checkAndProceed();
delay(1000);
}
display.clearDisplay();
}

void displayAnswerOptions(){
display.setTextSize(2);   
 display.setTextColor(SSD1306_WHITE);        // Draw white text
 display.setCursor(0,SCREEN_HEIGHT/2+5); 
 
 selectedAnswer= map(analogRead(POTENTIOMETER), 0, 1023, ansLowerLimit, ansUpperLimit); //Map pot values that range from 0-1023 to upper and lower limit values depending on the question's nswer
 display.println("Ans= " + String(selectedAnswer)); 

}

//Function to display question
void displayEquation(String equation){
display.setTextSize(2);             // Normal 1:1 pixel scale
 display.setTextColor(SSD1306_WHITE);        // Draw white text
 display.setCursor(0,0); 
 display.println(equation);

}

//Function to check user's answer and proceed
void checkAndProceed(void){
display.clearDisplay();
  //if correct then proceed else cut one point.
  if(correctAnswer==selectedAnswer){
  score++;
  
  display.setCursor(0,SCREEN_HEIGHT/2-15);
  display.setTextSize(2); 
  display.println("Correct!");  
  display.setCursor(0,SCREEN_HEIGHT/2);
   display.setTextSize(1); 
  display.println("Score= " + String(score));
  newQuestion();

  }
  else{
  
  display.setCursor(0,SCREEN_HEIGHT/2-15);
  display.setTextSize(1); 
  display.println("-Game Over-");  
  display.setCursor(0,SCREEN_HEIGHT/2);
   display.setTextSize(1); 
  display.println("Score= " + String(score));
  display.println();
  display.println("The correct answer was " + String(correctAnswer));


  }
    display.display();
  delay(1000);
  display.clearDisplay();
}

//function to generate new question
void newQuestion(void){
x=random(0,12);
y=random(0,12);
z=random(0,12);
String equationSet[]={String(x)+"*("+String(y)+"+"+String(z)+") = ?"};
int answerSet[]={x*(y+z)};
equation= equationSet[0];
correctAnswer=answerSet[0];
ansUpperLimit=correctAnswer + random(0,10);
ansLowerLimit=correctAnswer - random(0,10);
}
