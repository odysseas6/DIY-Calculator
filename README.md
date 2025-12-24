# DIY-Calculator
# The REAL Chinese Calculator

An advanced floating-point calculator project for Arduino using a 4x5 matrix keypad and an I2C LCD display. This calculator supports standard arithmetic, chaining operations, modulo, and a unique Power/Root mode selector.

## Features
* **Basic Arithmetic:** Addition (+), Subtraction (-), Multiplication (*), Division (/).
* **Scientific Operations:**
    * **Modulo (`B`):** Calculates the remainder of a division.
    * **Power & Root (`A`):** A submenu allows you to choose between calculating $x^y$ (Power) or $\sqrt[y]{x}$ (Root).
* **Smart Formatting:** Automatically switches between integer display, fixed decimals, and scientific notation based on result size.
* **Error Handling:** Protects against Division by 0, Modulo by 0, and imaginary numbers (even roots of negative numbers).
* **Chain Calculations:** Immediately use the result of the previous calculation as the start of the next one.

## Hardware Requirements
**Microcontroller:** Arduino Uno, Nano, or compatible.
* **Display:** 16x2 LCD with I2C Backpack (Address: `0x27`).
* **Input:** 4x5 Matrix Keypad (Membrane or Mechanical).
* **Connections:** Jumper wires.

## Wiring & Pinout
Based on the code configuration, connect your components as follows:

### Keypad Connection
| Keypad Pin | Arduino Pin | Function |
| :--- | :--- | :--- |
| Row 1 | D2 | Row |
| Row 2 | D3 | Row |
| Row 3 | D4 | Row |
| Row 4 | D5 | Row |
| Col 1 | D6 | Column |
| Col 2 | D7 | Column |
| Col 3 | D8 | Column |
| Col 4 | D9 | Column |
| Col 5 | D10 | Column |

### LCD Connection
| LCD Pin | Arduino Pin |
| :--- | :--- |
| VCC | 5V |
| GND | GND |
| SDA | A4 (Uno) / D18 (Mega) |
| SCL | A5 (Uno) / D19 (Mega) |

## Keypad Map
The code uses a custom 4x5 layout:

| | Col 1 | Col 2 | Col 3 | Col 4 | Col 5 |
| :--- | :---: | :---: | :---: | :---: | :---: |
| **Row 1** | `/` | `*` | `-` | `+` | `=` |
| **Row 2** | `B` | `9` | `6` | `3` | `.` |
| **Row 3** | `A` | `8` | `5` | `2` | `0` |
| **Row 4** | `C` | `7` | `4` | `1` | `D` |

## Usage Guide

### 1. Basic Math
Type the first number, press an operator (`+`, `-`, `*`, `/`), type the second number, and press `=`.

### 2. Modulo (Remainder)
Use the **`B`** key.
* Example: `10` `B` `3` `=`
* Result: `1` (because 10 divided by 3 leaves a remainder of 1).

### 3. Power & Root (The 'A' Key)
The **`A`** key enters a special mode.
1.  Enter your first number (Base).
2.  Press **`A`**.
3.  The screen will show `D+:Root D-:Power`.
4.  **Select Mode:**
    * Press **`D`** to toggle the sign.
    * **Sign: +** means **Root** (calculate $\sqrt[y]{x}$).
    * **Sign: -** means **Power** (calculate $x^y$).
5.  Enter the second number (Exponent or Root degree).
6.  Press `=`.

### 4. Negative Numbers
To make a number negative, type the number first, then press **`D`**.
* Example: To enter `-5`, type `5` then press `D`.

### 5. Clear
Press **`C`** to reset the calculator completely.
