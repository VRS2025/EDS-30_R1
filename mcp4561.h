

#define MCP4561_ADDR1 0x2e//0x0101110  // Address of MCP4561 with A0 = GND (0x0101110)46
#define MCP4561_ADDR2 0x2f//0x0101111  // Address of MCP4561 with A0 = VDD (0x0101111)47

#define MIN_DB -70.0  // Minimum dB level
#define MAX_DB 0.0    // Maximum dB level

void MCP4561_WriteWiper(uint8_t i2cAddress, uint8_t wiperValue);
uint8_t MCP4561_ReadWiper(uint8_t i2cAddress);
void SetControlRegister(unsigned char potAddress, unsigned char value);
void MCP4561_Write(unsigned char address, unsigned char reg, unsigned char data);
void SetWiperValue(unsigned char potAddress, unsigned char wiperNumber, unsigned char value);
unsigned char MCP4561_Read(unsigned char address, unsigned char reg);
unsigned char GetWiperValue(unsigned char potAddress, unsigned char wiperNumber);
float ConvertVolumeToDB(unsigned char volume);




