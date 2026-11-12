
# Arduino Tic-Tac-Toe Game 🎮

## About the Project
This project is a two-player Tic-Tac-Toe game developed using Arduino Uno, a 4x4 Matrix Keypad and a 16x2 I2C LCD.

Two players take turns selecting positions using the keypad. Player X and Player O compete to complete a horizontal, vertical or diagonal line.

## Components Used
- Arduino Uno
- 4x4 Matrix Keypad
- 16x2 I2C LCD
- Jumper Wires
- USB Cable
- Tinkercad Circuits

## Features
- Two-player game (X and O).
- Keypad-based input.
- LCD display for game status.
- Automatic winner detection.
- Draw detection.
- Occupied box detection.
- Reset game using key 0.
- Serial Monitor output.

## Pin Connections

### Keypad to Arduino
| Keypad | Arduino |
|---|---|
| Row 1 | D2 |
| Row 2 | D3 |
| Row 3 | D4 |
| Row 4 | D5 |
| Column 1 | D6 |
| Column 2 | D7 |
| Column 3 | D8 |
| Column 4 | D9 |

### LCD to Arduino
| LCD | Arduino |
|---|---|
| GND | GND |
| VCC | 5V |
| SDA | A4 |
| SCL | A5 |

## Keypad Mapping

1 | 2 | 3
--|---|--
4 | 5 | 6
7 | 8 | 9

Press 0 to reset the game.

## Working
1. Arduino initializes the keypad and LCD.
2. Player X starts the game.
3. Players enter positions using keys 1–9.
4. Arduino updates the board.
5. The program checks winning combinations.
6. The LCD displays the winner or draw result.

## Programming Language
Arduino C/C++

## Libraries Used
- Wire.h
- Adafruit_LiquidCrystal.h
- Keypad.h

## Applications
- Arduino learning projects.
- Embedded system demonstration.
- Keypad and LCD interfacing practice.

## Future Scope
- Add buzzer sound effects.
- Add score tracking.
- Introduce single-player mode.
- Add Bluetooth connectivity.

## Author
Simran

## Conclusion
This project demonstrates the use of Arduino Uno, keypad interfacing, LCD communication and embedded programming to develop an interactive two-player game.
