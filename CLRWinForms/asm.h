#pragma once
#ifndef ASM_ASM

extern "C" void ASMAdjustBrightness(unsigned char* bmpDataScan0,unsigned char* bmpOrg,short val,int imgsizeinbytes);
extern "C" void ASMNegativeIMG(unsigned char* bmpDataScan0, unsigned char* bmpOrg, int imgsizeinbytes,bool checked);
#endif // !ASM_ASM
