/*
 * Project: The REAL Chinese Calculator
 * Hardware: 
 * - Arduino (Uno/Nano)
 * - 16x2 I2C LCD Display (Address 0x27)
 * - 4x5 Matrix Keypad
 *
 * Description:
 * A floating-point calculator capable of basic arithmetic (+, -, *, /),
 * modulo operations ('B'), and advanced power/root functions ('A').
 * Includes error handling for division by zero and invalid roots.
 */

#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ==========================================
// Hardware Configuration
// ==========================================

// LCD Setup (Common address is 0x27, alternative is 0x3F)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Keypad Configuration
const byte ROWS = 4;
const byte COLS = 5;

// Keypad Mapping
char keys[ROWS][COLS] =
{
  {'/','*','-','+','='},
  {'B','9','6','3','.'},
  {'A','8','5','2','0'},
  {'C','7','4','1','D'},
};

// Pin Definitions
byte rowPins[ROWS] = {2, 3, 4, 5};
byte colPins[COLS] = {6, 7, 8, 9, 10};

// Initialize Keypad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// ==========================================
// Global Variables
// ==========================================

// Calculation Data
String input = "";          // Buffer for current number entry
float firstNum = 0;         // Left operand
float secondNum = 0;        // Right operand
char op = 0;                // Current operator char

// State Flags
bool waitingForSecond = false;  // True if we are entering the second number
bool resultDisplayed = false;   // True if the screen is currently showing a result

// Special Mode Variables (Key 'A')
bool aSignPositive = true;      // Logic toggle: True = Root, False = Power
bool aWaitingForMode = false;   // True if waiting for user to select Power/Root mode

// ==========================================
// Setup
// ==========================================
void setup()
{
  // Initialize LCD with backlight
  lcd.init();
  lcd.backlight();
  
  // Display Welcome Message
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("The REAL Chinese");
  lcd.setCursor(0, 1);
  lcd.print("Calculator!");
  delay(2000);
  
  // Clear screen for operation
  lcd.clear();
  lcd.setCursor(0, 0);
  delay(2000); // Additional delay to ensure clean start
  lcd.clear();
}

// ==========================================
// Main Loop
// ==========================================
void loop()
{
  char key = keypad.getKey();

  if (key)
  {
    // --------------------------------------
    // Scenario 1: Reset on new digit
    // --------------------------------------
    // If a result is currently displayed and the user types a number,
    // assume a brand new calculation is starting.
    if (resultDisplayed && (key >= '0' && key <= '9'))
    {
      lcd.clear();
      resultDisplayed = false;
      input = "";
      firstNum = 0;
      op = 0;
      waitingForSecond = false;
    }
    
    // --------------------------------------
    // Scenario 2: Chain calculation
    // --------------------------------------
    // If a result is displayed and the user hits an operator,
    // keep the result as 'firstNum' and continue.
    if (resultDisplayed && (key == '+' || key == '-' || key == '*' || key == '/' || key == 'B' || key == 'A'))
    {
      resultDisplayed = false;
      waitingForSecond = false;
    }

    // --------------------------------------
    // Input Handling: Digits and Decimals
    // --------------------------------------
    if ((key >= '0' && key <= '9') || key == '.')
    {
      // If we were in the middle of selecting Root vs Power ('A' mode),
      // typing a number finalizes the selection and moves to input.
      if (aWaitingForMode)
      {
        aWaitingForMode = false;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(firstNum);
        lcd.print(" ");
        lcd.print(aSignPositive ? "rt" : "^"); // Display symbol based on selection
        lcd.setCursor(0, 1);
        waitingForSecond = true;
      }
      
      // Validation: Prevent multiple decimal points
      if (key == '.' && input.indexOf('.') != -1) return;
      
      // Validation: Prevent buffer overflow (LCD limit)
      if (input.length() >= 15) return;

      input += key;
      
      // UI: Clear the second line if we are just starting the second number
      if (waitingForSecond && input.length() == 1 && !aWaitingForMode)
      {
        lcd.setCursor(0, 1);
        lcd.print("                ");
        lcd.setCursor(0, 1);
      }
      
      lcd.print(key);
      resultDisplayed = false;
    }
    // --------------------------------------
    // Operator Handling (+, -, *, /, B, A)
    // --------------------------------------
    else if (key == '+' || key == '-' || key == '*' || key == '/' || key == 'B' || key == 'A')
    {
      if (input != "")
      {
        // Calculate previous operation if one exists (chaining)
        if (waitingForSecond)
        {
          calculateResult();
        }
        
        firstNum = input.toFloat();
        op = key;
        input = "";
        
        // Special Case: Key 'A' (Root / Power Mode Selection)
        if (key == 'A')
        {
          lcd.clear();
          lcd.print("D+:Root D-:Power");
          lcd.setCursor(0, 1);
          lcd.print("Sign: +");
          aSignPositive = true; // Default to Root
          aWaitingForMode = true;
          return; // Stop here, wait for 'D' or digit input
        }
        
        // Standard Operator Display
        lcd.setCursor(0, 0);
        lcd.print("                "); // Clear Line 1
        lcd.setCursor(0, 0);
        lcd.print(firstNum);
        lcd.print(" ");
        
        if (key == 'B') lcd.print("%"); // Display % for Modulo
        else lcd.print(key);

        waitingForSecond = true;
        resultDisplayed = false;
      }
    }
    // --------------------------------------
    // Execute Calculation (=)
    // --------------------------------------
    else if (key == '=')
    {
      // If stuck in 'A' mode selection, finalize it before calculating
      if (aWaitingForMode)
      {
        aWaitingForMode = false;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(firstNum);
        lcd.print(" ");
        lcd.print(aSignPositive ? "rt" : "^");
        lcd.setCursor(0, 1);
        waitingForSecond = true;
        return;
      }
      
      if (input != "" && waitingForSecond)
      {
        calculateResult();
      }
    }
    // --------------------------------------
    // Clear (C)
    // --------------------------------------
    else if (key == 'C')
    {
      resetCalc();
    }
    // --------------------------------------
    // Sign Toggle / Mode Toggle (D)
    // --------------------------------------
    else if (key == 'D')
    {
      // Context 1: Toggling between Root and Power inside 'A' mode
      if (aWaitingForMode)
      {
        aSignPositive = !aSignPositive;
        lcd.setCursor(0, 1);
        lcd.print("                ");
        lcd.setCursor(0, 1);
        lcd.print(aSignPositive ? "Sign: +" : "Sign: -");
      }
      // Context 2: Toggling positive/negative on a standard number input
      else if (input != "" && input != "0")
      {
        if (input.charAt(0) == '-')
        {
          input = input.substring(1); // Remove negative
        }
        else
        {
          input = "-" + input;        // Add negative
        }
        
        // Redraw current line
        if (waitingForSecond)
        {
          lcd.setCursor(0, 1);
          lcd.print("                ");
          lcd.setCursor(0, 1);
        }
        else
        {
          lcd.setCursor(0, 0);
          lcd.print("                ");
          lcd.setCursor(0, 0);
        }
        lcd.print(input);
      }
    }
  }
}

// ==========================================
// Calculation Logic
// ==========================================
void calculateResult()
{
  secondNum = input.toFloat();
  float result = 0;
  bool error = false;

  switch (op)
  {
    case '+': result = firstNum + secondNum; break;
    case '-': result = firstNum - secondNum; break;
    case '*': result = firstNum * secondNum; break;
    
    // Division
    case '/': 
      if (secondNum != 0) 
        result = firstNum / secondNum;
      else
      {
        displayError("Error: Div by 0");
        error = true;
      }
      break;

    // Modulo
    case 'B': 
      if (secondNum != 0)
      {
        result = fmod(firstNum, secondNum); // Floating point modulo
      }
      else
      {
        displayError("Error: Mod by 0");
        error = true;
      }
      break;

    // Power / Root
    case 'A': 
      if (aSignPositive)
      {
        // Logic: Nth Root
        // Formula: firstNum ^ (1/secondNum)
        if (secondNum == 0)
        {
          displayError("Error: 0th root");
          error = true;
        }
        else if (firstNum < 0 && fmod(secondNum, 2) == 0)
        {
          // Complex number protection (Even root of negative number)
          lcd.clear();
          lcd.print("Error: Even root");
          lcd.setCursor(0, 1);
          lcd.print("of negative");
          delay(2000);
          resetCalc();
          error = true;
        }
        else
        {
          result = pow(firstNum, 1.0 / secondNum);
        }
      }
      else
      {
        // Logic: Power
        // Formula: firstNum ^ secondNum
        result = pow(firstNum, secondNum);
      }
      break;
  }

  // Display Result if no errors occurred
  if (!error)
  {
    // Re-draw the full equation on Line 1
    lcd.setCursor(0, 0);
    lcd.print("                ");
    lcd.setCursor(0, 0);
    lcd.print(firstNum);
    
    if (op == 'B') lcd.print("%");
    else if (op == 'A') lcd.print(aSignPositive ? "rt" : "^");
    else lcd.print(op);
    
    lcd.print(secondNum);

    // Display Result on Line 2
    lcd.setCursor(0, 1);
    lcd.print("                ");
    lcd.setCursor(0, 1);
    lcd.print("= ");

    // Smart Formatting:
    // 1. If whole number, print as Integer
    if (result == (int)result)
    {
      lcd.print((int)result);
    }
    // 2. If very large or very small, use scientific notation (default float print)
    else if (abs(result) >= 1000000 || (abs(result) < 0.001 && result != 0))
    {
      lcd.print(result);
    }
    // 3. Medium numbers: 2 decimal places
    else if (abs(result) >= 1000)
    {
      lcd.print(result, 2);
    }
    // 4. Small numbers: 4 decimal places
    else
    {
      lcd.print(result, 4);
    }

    // Prepare state for chaining operations
    input = String(result);
    firstNum = result;
    waitingForSecond = false;
    resultDisplayed = true;
  }
}

// ==========================================
// Helper Functions
// ==========================================

// Display error message and reset
void displayError(const char* msg) {
    lcd.clear();
    lcd.print(msg);
    delay(2000);
    resetCalc();
}

// Full Reset of Calculator State
void resetCalc()
{
  lcd.clear();
  input = "";
  firstNum = 0;
  secondNum = 0;
  op = 0;
  waitingForSecond = false;
  resultDisplayed = false;
  aSignPositive = true; // Reset 'A' mode to Root
  aWaitingForMode = false;
}
