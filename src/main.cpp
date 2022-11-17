/* The packages used to implement this system are listed below. */
#include <Arduino.h>
#include <Keypad.h>
#include <LiquidCrystal.h>


/* The constant variables such as pin numbers are listed below, along with their description. */
// The maximum length of the password; can be easily modified.
#define MAX_LENGTH 10

// The pins of the button and the keypad.
#define BUTTON_PIN 2
#define ROW_PIN_A 13
#define ROW_PIN_B 12
#define ROW_PIN_C 3
#define ROW_PIN_D 4
#define COLUMN_PIN_1 5
#define COLUMN_PIN_2 6
#define COLUMN_PIN_3 7
#define LCD_CONTROLL_1 A0
#define LCD_CONTROLL_2 A1
#define LCD_DATA_4 11
#define LCD_DATA_5 10
#define LCD_DATA_6 9
#define LCD_DATA_7 8


void DisplayPassword();

/* The global variables are defined here.*/
// The hardcoded correct password.
const char CORRECT_PASSWORD[] = {'1', '2', '3', '4', '5', '6', '0', '\0'};

// Configuration of the keypad.
const byte Rows = 4;
const byte Columns = 3;
char HexaKeyers[Rows][Columns] = {
	{'1', '2', '3'},
	{'4', '5', '6'},
	{'7', '8', '9'},
	{'*', '0', '#'}
};
byte RowPins[Rows] = {ROW_PIN_A, ROW_PIN_B, ROW_PIN_C, ROW_PIN_D};
byte ColumnPins[Columns] = {COLUMN_PIN_1, COLUMN_PIN_2, COLUMN_PIN_3};
Keypad PasswordKeypad = Keypad
(
	makeKeymap(HexaKeyers), 
	RowPins, ColumnPins, 
	Rows, Columns
);

// Configuration of the LCD.
LiquidCrystal PasswordLCD = LiquidCrystal
(
	LCD_CONTROLL_1, LCD_CONTROLL_2, 
	LCD_DATA_4, LCD_DATA_5, LCD_DATA_6, LCD_DATA_7
);

// The values associated with password are listed below.
// The +1 is because of the NULL terminator.
char Password[MAX_LENGTH + 1] = "\0";
byte PasswordSize = 0;
volatile bool PasswordShow;
volatile bool PasswordChanged;

// Messages that are meant to be sent to the display are listed below.
const char SuccessMessage[] = "Access is granted";
const char FailureMessage[] = "Wrong password";
const char WelcomeMessage[] = "Enter password";

/* The functions are listed below along with their implementation. */
// Adds one character to the end of the password string; invoked when the user
// pushes a number key on the keypad.
void AddCharacter(const char character)
{
	if (PasswordSize < MAX_LENGTH) {
		Password[PasswordSize] = character;
		Password[PasswordSize + 1] = '\0';
		PasswordSize++;
	}
}

// Deletes the last character; invoked when the user presses '#' on the keypad.
void DeleteLastCharacter()
{
	if (PasswordSize > 0) {
		Password[PasswordSize - 1] = '\0';
		PasswordSize--;
	}
}

// Toggles the visiablity of the password; invoked when the user pushes the button.
void TogglePasswordShow()
{
	PasswordShow = !PasswordShow;
	PasswordChanged = true;
}

// This function decides, depeneding on whether the correct password is entered,
// to whether print the success message or the failure one.
void PrintLoginMessage(const bool isCorrect)
{
	PasswordLCD.clear();
	if (isCorrect) PasswordLCD.print(SuccessMessage);
	else PasswordLCD.print(FailureMessage);

	// Reseting the password.
	memset(Password, 0, sizeof(Password));
	PasswordSize = 0;
	delay(1000);
	PasswordLCD.clear();
	PasswordLCD.print(WelcomeMessage);
}

// This functions compares the entered password with the correct password
// and returns true if they match; otherwise, it returns false.
bool SubmitPassword()
{
	if (strcmp(CORRECT_PASSWORD, Password) == 0) return true;
	else return false;
}

// This function sends the content of the password to the LCD.
// If PasswordShow is false, it only sends '*' character.
void DisplayPassword()
{
	PasswordLCD.clear();
	if (PasswordSize == 0) {
		return;
	}
	if (PasswordShow == true) {
		PasswordLCD.print(Password);
	}
	else {
		char StarString[PasswordSize];
		for (int i = 0; i < PasswordSize; i++) {
			StarString[i] = '*';
		}
		StarString[PasswordSize] = '\0';
		PasswordLCD.print(StarString);
	}
	delay(10);
}

void setup() {
	Serial.begin(9600);
	pinMode(BUTTON_PIN, INPUT);
	attachInterrupt
	(
		digitalPinToInterrupt(BUTTON_PIN),
		TogglePasswordShow,
		RISING
	);
	PasswordShow = false;
	PasswordChanged = false;
	PasswordLCD.begin(16, 2);
	PasswordLCD.print(WelcomeMessage);
	delay(10);
}

void loop() {
	if (PasswordChanged) {
		DisplayPassword();
		PasswordChanged = false;
	}
	char ReadCharacter = PasswordKeypad.getKey();
	if (ReadCharacter) {
		if (ReadCharacter != '#' && ReadCharacter != '*') {
			AddCharacter(ReadCharacter);
			DisplayPassword();
		}
		else if (ReadCharacter == '#') {
			DeleteLastCharacter();
			DisplayPassword();
		}
		else if (ReadCharacter == '*') {
			bool IsPasswordCorrect = SubmitPassword();
			PrintLoginMessage(IsPasswordCorrect);
		}
		delay(10);
	}
}