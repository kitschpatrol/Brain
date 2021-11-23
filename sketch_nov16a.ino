#include <Brain.h>

Brain brain(Mindflex);

void setup() 
{
  /* Begin the hardware serial 
  */
  Serial.begin(9600);
}

void loop() 
{   
  /* @brief: Print out data
  *
  * The .readCSV() function returns a char* containing the most recent brain data, in the format:
  * "signal strength, attention, meditation, delta, theta, low alpha, high alpha, low beta, high beta, low gamma, high gamma" 
  */

  if (brain.update())
  {
    Mindflex.println(brain.readCSV());
  }
}