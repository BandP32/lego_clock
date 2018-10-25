/*
  Digit Mapping for the LEGO Clock
  - Arrays with more than 8 bits in an array
*/

unsigned int dig_array[10];  // digits 0-9

int count_r = 6;         // number of shift registers
int length_r = 8;        // 8-bit register
unsigned int shift_array[6][8];   // array of the shift registers

unsigned int clock_array[4][11] = {{1,1,1,1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1,1,1,1}};    // array with 4 elements
int array_length = 11;  // binary-digits per element
int array_i = 4;        // clock-digit array elements 

int b_r;    // bit count per register
int i;      // counter array bits
int d=0;      // 4 digit positions (h1 h2 m1 m2)


void setup()
{
  // digits mapping
  dig_array[0] = 0b11111111110;   // Zahl 0 mit den LEDs

 
  // averagingBuffer[i][bufferIndex] = joystickInput;      // insert sth to array[i][position]


  Serial.begin(9600);


  
  // for (int i=0;i < n;i++){
  //  fib_array[i] = fib_old; // insert fib_old at index i
    
  // }
  // Serial.println(clock_array[3]); // 

}

void loop()
{
  for (int r=0; r < count_r; r++){
    

    for (int b=0; b < length_r; b++){
      Serial.println(r);    // register
      Serial.println(b);    // bit pos of current register
      Serial.println(i);    // current bit of the 11-bit digits
      Serial.println(d);    // 11bit digit number 0-3
      Serial.println('-');

      if (d >= array_i){
        
        shift_array[r][b] = pow(2, b) * 0;    // 
      }
      else{
        shift_array[r][b] = pow(2, b) * clock_array[d][i];    // 
      }
      
      Serial.println(shift_array[r][b]);
      Serial.println('-');
      
      i++;    // next bit of the 11bit digit

      

      if (i > 10){    // if all 11 bits are in the register:
        d++;    // next digit
        i=0;    // reset counter 

      
      }
      // delay(3000);
      
    }

  
 
  }
  delay(100000);
  
}
