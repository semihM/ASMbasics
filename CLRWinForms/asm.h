#pragma once
#ifndef ASM_ASM

extern "C" void ASMAdjustBrightness(unsigned char* bmpDataScan0,unsigned char* bmpOrg,short val,int imgsizeinbytes);

extern "C" void ASMNegativeIMG(unsigned char* bmpDataScan0, unsigned char* bmpOrg, int imgsizeinbytes,bool checked);

extern "C" void ASMBlurIMG(int height,
						   int width,
						   unsigned char* bmpOrg,
						   unsigned char* bmpDataScan0,
						   unsigned char* hblur, int blurWidth);

extern "C" void ASMGreyscale(unsigned char* scan0, unsigned char* org, int size);

extern "C" void ASMrgb2hsl(float r, float g, float b, unsigned char* hslptr);

extern "C" void ASMColorChangeColorRange(unsigned char* bmp, unsigned char* newColorRGB, unsigned char* ranges, int size);
#endif // !ASM_ASM
