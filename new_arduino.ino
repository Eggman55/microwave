// Microwave_control.ino
// Currently does not multi-task (does not use timers and interrupts),
// i.e., pauses program for pulse/purging

// contains pinouts

//	Source 	Cable color		R200 pin	R200 block
//	A		    Blue/White		3			    2
//	B		    Blue			    5,9			  2
//	D		    Orange/White	10			  1
//	E		    Orange			  13			  1

#define PIN_PRECA       5
#define PIN_PRECB       4
#define PIN_PRECD       7
#define PIN_PRECE       2
#define PIN_LID_DOWN    6
#define PIN_ERR_LED     7
#define PIN_uWave_STAT_LED 11
#define PIN_uWave_RELAY    12

// ALD RECIPE - copy from template and modify recipe parameters, then include here
// all time values in ms

unsigned long uWave_WAIT = 50000;
unsigned long uWave_PULSE = 30000;
int Num_Cycles = 200;
bool running = false;
String inputString = "";

bool debug = true;


// program variables
int serialByte = 0;



// setup function
void setup() {

	// digital I/O setup (output)
	pinMode(PIN_uWave_RELAY, OUTPUT);
	pinMode(PIN_uWave_STAT_LED, OUTPUT);

	// uWave relay initially closed
	digitalWrite(PIN_uWave_RELAY, LOW);
	digitalWrite(PIN_uWave_STAT_LED, LOW);

	// for debugging purposes currently
	Serial.begin(9600);

  Serial.print("Pulse time: ");
  Serial.println(uWave_PULSE);
  Serial.print("Wait Time: ");
  Serial.println(uWave_WAIT);
  Serial.print("Number of cycles: ");
  Serial.println(Num_Cycles);

  Serial.println("DEBUG Mode.");
  Serial.println("Send 1 to start sequence.");
}

// main program loop (infinite)
void loop() {
  checkSerial();

  if (running) {
    for (int n = 1; n <= Num_Cycles; n++) {
      Serial.print("Pulsing ");
      Serial.println(n);
      pulseuWave(uWave_WAIT, uWave_PULSE);

      checkSerial(); // allow stop
      if (!running) {
        Serial.println("Stopped early.");
        break;
      }
    }
    running = false;
    Serial.println("Done.");

}


	// only run program and use generator if process lid is down
	if (!debug) {

    // detect precursor signal RISING edge, and pulse microwaves if detected
    for (int n = 1; n <= Num_Cycles; n++) {
      Serial.print("Pulsing ");
      Serial.println(n);
      pulseuWave(uWave_WAIT, uWave_PULSE);
    }
    debug = true;
    Serial.println("Done.");

	} 

} // end main program loop


// Functions

//reads serial monitor
void checkSerial() {
  while (Serial.available() > 0) {
    char c = Serial.read();
    if (c == '\n') {
      checkCommand(inputString);
      inputString = "";
    } else {
      inputString += c;
    }
  }
}


//checks input
void checkCommand(String cmd) {
  cmd.trim();

  if (cmd == "1") {
    running = true;
    Serial.println("Start command received.");
    return;
  }

  if (cmd == "0") {
    running = false;
    Serial.println("Stop command received.");
    digitalWrite(PIN_uWave_RELAY, LOW);     // close uWave relay, generator ON
		digitalWrite(PIN_uWave_STAT_LED, LOW);
    running = false;
    return;
  }

  if (cmd == "2") {
    Serial.println("microwave on.")
    digitalWrite(PIN_uWave_RELAY, HIGH);     
		digitalWrite(PIN_uWave_STAT_LED, HIGH);
    return;
  }

  if (cmd == "3") {
     Serial.println("microwave off.")
    digitalWrite(PIN_uWave_RELAY, LOW);     
		digitalWrite(PIN_uWave_STAT_LED, LOW);
    return;
  }

  if (cmd.startsWith("Pulse=,")) {
    uWave_PULSE = cmd.substring(7).toInt();
    Serial.print("Pulse set to ");
    Serial.println(uWave_PULSE);
    return;
  }

  if (cmd.startsWith("Delay=,")) {
    uWave_WAIT = cmd.substring(7).toInt();
    Serial.print("Delay set to ");
    Serial.println(uWave_WAIT);
    return;
  }

  if (cmd.startsWith("Cycles=,")) {
    Num_Cycles = cmd.substring(8).toInt();
    Serial.print("Cycles set to ");
    Serial.println(Num_Cycles);
    return;
  }

  Serial.print("Unknown command: ");
  Serial.println(cmd);
}

// Controls microwave relay
void pulseuWave(unsigned long uWave_wait, unsigned long uWave_pulse) {
		delay(uWave_wait);					      // wait for precursor to purge
    Serial.println("Generator on.");
    digitalWrite(PIN_uWave_RELAY, HIGH);     // close uWave relay, generator ON
		digitalWrite(PIN_uWave_STAT_LED, HIGH);	// indicate uWave lamp status

		delay(uWave_pulse);					      // wait for UV pulse to finish
		digitalWrite(PIN_uWave_RELAY, LOW);		// open uWave relay, generator OFF
		digitalWrite(PIN_uWave_STAT_LED, LOW);	// indicate uWave lamp status
    Serial.println("Generator off.");
    Serial.println();
}
