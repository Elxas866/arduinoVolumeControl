#define pin_clk 6
#define pin_dt 5
#define pin_sw 4

int clk_state;
int clk_last_state;

unsigned long timeBefore = millis();
unsigned long timeAfter;

void setup() {
  pinMode(pin_clk, INPUT);
  pinMode(pin_dt, INPUT);
  pinMode(pin_sw, INPUT);

  Serial.begin(9600);
}

void loop() {
  clk_state = digitalRead(pin_clk);
  if (clk_state != clk_last_state) {
    if (digitalRead(pin_dt) != clk_state) {
      Serial.println("POSPLUS");
    }else {
      Serial.println("POSMINUS");
    }
  }
  clk_last_state = clk_state;

  timeAfter = millis();
  if (digitalRead(pin_sw) == LOW) {
    if (timeAfter - timeBefore >= 500) {
      Serial.println("clicked");
    }
    timeBefore = millis();
  }
}
