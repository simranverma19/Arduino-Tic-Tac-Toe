// ===== Two-player Tic-Tac-Toe: Arduino Uno + 4x4 keypad + 16x2 I2C LCD (Tinkercad) =====
#include <Wire.h>
#include <Adafruit_LiquidCrystal.h>
#include <Keypad.h>

Adafruit_LiquidCrystal lcd(0);

// ---------- Keypad (pins matched to your wiring screenshot) ----------
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {2, 3, 4, 5};
byte colPins[COLS] = {6, 7, 8, 9};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// ---------- Game state ----------
char board[3][3];                 // ' ' = empty, otherwise 'X' or 'O'
char currentPlayer;
enum Phase { PLAYING, WON, DRAW };
Phase phase;
char winner;
byte winFrame;
unsigned long nextFrameAt = 0;
unsigned long restoreAt = 0;

// ---------- LCD: whole screen changes together ----------
void writeLine(byte row, const char* text) {
  lcd.setCursor(0, row);
  byte n = 0;
  while (text[n] != '\0' && n < 16) { lcd.print(text[n]); n++; }
  while (n < 16) { lcd.print(' '); n++; }
}

void showScreen(const char* line0, const char* line1) {
  lcd.noDisplay();            // hide while writing
  writeLine(0, line0);
  writeLine(1, line1);
  lcd.display();              // new screen appears all at once
}

// Board text for LCD line 1, e.g. "XO3|4X6|789"
void boardLine(char* out) {
  byte p = 0;
  for (byte r = 0; r < 3; r++) {
    for (byte c = 0; c < 3; c++)
      out[p++] = (board[r][c] == ' ') ? (char)('1' + r * 3 + c) : board[r][c];
    if (r < 2) out[p++] = '|';
  }
  out[p] = '\0';
}

void showTurn() {
  char top[17], bot[17];
  boardLine(top);
  snprintf(bot, sizeof(bot), "Turn: Player %c", currentPlayer);
  showScreen(top, bot);
}

// ---------- Serial Monitor: grid with row and column labels ----------
void printBoardSerial() {
  Serial.println();
  Serial.println(F("      C1  C2  C3"));
  for (byte r = 0; r < 3; r++) {
    Serial.print(F("  R"));
    Serial.print(r + 1);
    Serial.print(F("  "));
    for (byte c = 0; c < 3; c++) {
      Serial.print(' ');
      Serial.print(board[r][c] == ' ' ? (char)('1' + r * 3 + c) : board[r][c]);
      Serial.print(' ');
      if (c < 2) Serial.print('|');
    }
    Serial.println();
    if (r < 2) Serial.println(F("      ---+---+---"));
  }
  Serial.println();
}

// ---------- 'X' / 'O' = winner, 'D' = draw, 0 = keep playing ----------
char checkResult() {
  for (byte i = 0; i < 3; i++) {
    if (board[i][0] != ' ' && board[i][0] == board[i][1] && board[i][1] == board[i][2]) return board[i][0];
    if (board[0][i] != ' ' && board[0][i] == board[1][i] && board[1][i] == board[2][i]) return board[0][i];
  }
  if (board[1][1] != ' ') {
    if (board[0][0] == board[1][1] && board[1][1] == board[2][2]) return board[1][1];
    if (board[0][2] == board[1][1] && board[1][1] == board[2][0]) return board[1][1];
  }
  for (byte r = 0; r < 3; r++)
    for (byte c = 0; c < 3; c++)
      if (board[r][c] == ' ') return 0;
  return 'D';
}

// ---------- Winner screens, cycled without blocking ----------
void showWinFrame() {
  char a[17], top[17];
  snprintf(a, sizeof(a), "Player %c Wins!", winner);
  if (winFrame == 0) {
    showScreen(a, "Winner Winner");
  } else if (winFrame == 1) {
    showScreen("Chicken Dinner!", a);
  } else {
    boardLine(top);
    showScreen(top, "0 = play again");
  }
}

void resetGame() {
  for (byte r = 0; r < 3; r++)
    for (byte c = 0; c < 3; c++)
      board[r][c] = ' ';
  currentPlayer = 'X';
  phase = PLAYING;
  restoreAt = 0;
  showTurn();
  printBoardSerial();
  Serial.println(F("Player X starts. Press 1-9."));
}

void setup() {
  Serial.begin(115200);
  keypad.setDebounceTime(5);
  lcd.begin(16, 2);
  lcd.setBacklight(1);
  Wire.setClock(400000);      // faster I2C = faster LCD updates

  showScreen("Welcome to", "Tic Tac Toe Game");
  delay(1000);
  showScreen("Press 1-9: play", "Press 0: reset");
  delay(1000);

  resetGame();
}

void loop() {
  unsigned long now = millis();

  // Return to the turn screen after "Box taken!"
  if (restoreAt != 0 && now >= restoreAt) {
    restoreAt = 0;
    if (phase == PLAYING) showTurn();
  }

  // Cycle winner screens
  if (phase == WON && now >= nextFrameAt) {
    showWinFrame();
    winFrame = (winFrame + 1) % 3;
    nextFrameAt = millis() + 1500;
  }

  char key = keypad.getKey();
  if (key == NO_KEY) return;

  if (key == '0') { resetGame(); return; }   // reset at any time
  if (phase != PLAYING) return;              // wait for reset
  if (key < '1' || key > '9') return;        // ignore A-D, *, #

  byte n = key - '1';                        // 0..8
  byte r = n / 3;                            // row    (key 5 -> row 1 = R2)
  byte c = n % 3;                            // column (key 5 -> col 1 = C2)

  if (board[r][c] != ' ') {                  // box already taken
    char top[17];
    boardLine(top);
    showScreen(top, "Box taken! Retry");
    restoreAt = millis() + 800;
    return;
  }

  restoreAt = 0;
  board[r][c] = currentPlayer;

  Serial.print(F("Player "));
  Serial.print(currentPlayer);
  Serial.print(F(" -> key "));
  Serial.print(key);
  Serial.print(F(" (R"));
  Serial.print(r + 1);
  Serial.print(F(", C"));
  Serial.print(c + 1);
  Serial.println(')');
  printBoardSerial();

  char result = checkResult();
  if (result == 'X' || result == 'O') {
    phase = WON;
    winner = result;
    winFrame = 0;
    showWinFrame();
    winFrame = 1;
    nextFrameAt = millis() + 1500;
    Serial.print(F("Player "));
    Serial.print(winner);
    Serial.println(F(" winner winner chicken dinner!"));
  } else if (result == 'D') {
    phase = DRAW;
    char top[17];
    boardLine(top);
    showScreen(top, "Draw! 0=reset");
    Serial.println(F("It's a draw!"));
  } else {
    currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
    showTurn();
  }
}
