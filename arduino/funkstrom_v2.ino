

#define SIGNAL_PIN 2
#define POWER_PIN  4

////////////////////////////////////////////////////////////////////////
// DATAGRAM

#define DATAGRAM_SIZE          13
#define DATAGRAM_SYS_OFFSET    0
#define DATAGRAM_UC_OFFSET     5
#define DATAGRAM_SW_OFF        10
#define DATAGRAM_SW_ON         11
#define DATAGRAM_STOPBIT       12

////////////////////////////////////////////////////////////////////////
// DATAGRAM OPTIONS

#define VAL_VSS   0b10001000
#define VAL_VCC   0b11101110
#define VAL_FLW   0b10001110

////////////////////////////////////////////////////////////////////////
// System Code

#define SYS_1 0b00000001
#define SYS_2 0b00000010
#define SYS_3 0b00000100
#define SYS_4 0b00001000
#define SYS_5 0b00010000

////////////////////////////////////////////////////////////////////////
// Unit Code

#define UC_A  0b00000001
#define UC_B  0b00000010
#define UC_C  0b00000100
#define UC_D  0b00001000
#define UC_E  0b00010000


void send_datagram( const char* datagram ) {
  
  for(char amount = 0; amount < 3; amount++ ) {
    
      for(char i = 0; i < DATAGRAM_SIZE; i++){
         char _byte = datagram[i];
       
         for(char _bit=7; _bit >= 0; _bit--) {
             digitalWrite(SIGNAL_PIN, ((_byte >> _bit) & 0x01) );
             delayMicroseconds(320);
         } 
      }
    
  }
}

void switch_socket(byte syscode, byte address, bool on_off ) {

    char datagram[DATAGRAM_SIZE];
   
    for(char _bit=0; _bit < 5; _bit++){
        datagram[DATAGRAM_SYS_OFFSET + _bit] = (syscode & (1<<_bit))? VAL_VSS : VAL_FLW;
    }
   
    for(char _bit=0; _bit < 5; _bit++){
        datagram[DATAGRAM_UC_OFFSET + _bit] = (address & (1<<_bit))? VAL_VSS : VAL_FLW;
    }
   
    if(on_off){
        datagram[DATAGRAM_SW_OFF] = VAL_VSS;
        datagram[DATAGRAM_SW_ON]  = VAL_FLW;
    } else {
        datagram[DATAGRAM_SW_OFF] = VAL_FLW;
        datagram[DATAGRAM_SW_ON]  = VAL_VSS; 
    }

    datagram[DATAGRAM_STOPBIT] = 0b10000000;
    
    send_datagram(datagram);
}

void socket_on(byte syscode, byte address) {
   switch_socket(syscode, address, true);
}

void socket_off(byte syscode, byte address) {
   switch_socket(syscode, address, false);
}


void setup() {
    pinMode(SIGNAL_PIN, OUTPUT); 
    pinMode(POWER_PIN, INPUT); 
}

char uc[] = {UC_A, UC_B, UC_C};

void loop() {
  
  /*
    Switch on and off all power sockets defined in uc
  */
  bool doit = digitalRead(POWER_PIN);
  
  if( doit ) {

    byte syscode = SYS_1 | SYS_2 | SYS_3 | SYS_4 | SYS_5;
    
    for(char lulz = 0; lulz < 2; lulz++ ) {
        bool on_off = (bool) lulz;
      
        for(char steckdose = 0; steckdose < 3; steckdose++) {
            switch_socket( syscode, uc[steckdose], on_off);
            delay(600);
        }
        
    }
  }

}
