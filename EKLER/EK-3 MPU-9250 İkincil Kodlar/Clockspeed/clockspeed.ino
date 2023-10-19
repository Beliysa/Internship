uint32_t CPUMhz = 0;
uint32_t APBMhz = 0;
hw_timer_t * timer = NULL;       //Timer read object has 4 values. 
uint64_t timeRead = 0;            //timeRead() function returns uint64_t type. We call timeRead() with our object and return value is our time. 
void setup() {
  Serial.begin(115200);
  timer = timerBegin(0, 80, true);     //prescaler. 80MHz internal timer(1 tick = 12.5ns*80 = 1000ns = 0.001ms).
}

void loop() {
  CPUMhz = getCpuFrequencyMhz();
  APBMhz = getApbFrequency();
  timeRead = timerRead(timer);
  Serial.print("CPUMhz: ");
  Serial.print(CPUMhz);
  Serial.print("       ");
  Serial.print("ABPMhz: ");
  Serial.print(APBMhz);
  Serial.print("       ");
  Serial.print("Timer: ");
  Serial.println(timeRead);
  
}
