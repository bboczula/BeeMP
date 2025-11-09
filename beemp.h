#pragma once

#include <string>
#include <fstream>
#include <vector>
#include <Windows.h>

std::vector<unsigned char> rawBuffer;
std::vector<unsigned char> output;
std::vector<unsigned char> outputBmp;

unsigned int LoadFileToMemory(const std::string& fileName, bool isBinary)
{
      // Check if file exists
	// This requires C++17, mayble let's try to keep this simple
      //if (!std::filesystem::exists(fileInfo.fullName))
      //{
      //      return ErrorCode::NonExistingFile;
      //}

      // Clear the buffer, since it might have something in it
      rawBuffer.clear();

      //load and decode
      std::ifstream file(fileName.c_str(), std::ios::in | std::ios::ate | std::ios::binary);

      // Figure out the file size
      // - this type is an implementation-defined signed integral type used to represent the number of
      //   characters transered in an I/O opration or the size of I/O buffer. It is usead as a ssinged counterpart of the std:size_T
      std::streamsize size = 0;

      // - seekg sets the position of the next character to be extracted from the input stream
      // - it returns the istream object
      // - first parameter is the offset value, relative to the second parameter
      // - second parameter can take three values: beginning, current or end of the current stream
      // - this seems to set the next character to read as the last character in the file
      if (file.seekg(0, std::ios::end).good())
      {
            // tellg() returns the position of the currenct character in the input stream
            // the return type is the streampos
            // effectively this returns the position of the last character
            size = file.tellg();
      }

      // this seems to set the next character to read as the first character
      if (file.seekg(0, std::ios::beg).good())
      {
            // tellg() function returns the position of the first character
            // you calculate the size of the file by substracting position of the last character from the first character
            size -= file.tellg();
      }

      //read contents of the file into the vector
      if (size > 0)
      {
            rawBuffer.resize((size_t)size);
            file.read((char*)(&rawBuffer[0]), size);
            if (!isBinary)
            {
                  // Needed for stringstream to work properly.
                  rawBuffer.push_back('\0');
            }
      }
      else
      {
            rawBuffer.clear();
      }

      return 0;
}

unsigned int StoreBufferToFile(const std::string& fileName, const std::vector<unsigned char>& buffer, bool isBinary)
{
      std::ofstream file;
      if (isBinary)
      {
            file.open(fileName.c_str(), std::ios::out | std::ios::binary);
      }
      else
      {
            file.open(fileName.c_str(), std::ios::out);
      }
      if (!file.is_open())
      {
            return 1; // could not open file for writing
      }
      file.write((const char*)buffer.data(), buffer.size());
      file.close();
      return 0;
}

void PrintInfoHeader(BITMAPINFOHEADER infoHeader)
{
      std::cout << "Width: " << infoHeader.biWidth << std::endl;
      std::cout << "Height: " << infoHeader.biHeight << std::endl;
      std::cout << "Planes: " << infoHeader.biPlanes << std::endl;
      std::cout << "BitCount: " << infoHeader.biBitCount << std::endl;
      std::cout << "Compression: " << infoHeader.biCompression << std::endl;
      std::cout << "SizeImage: " << infoHeader.biSizeImage << std::endl;
      std::cout << "XPelsPerMeter: " << infoHeader.biXPelsPerMeter << std::endl;
      std::cout << "YPelsPerMeter: " << infoHeader.biYPelsPerMeter << std::endl;
      std::cout << "ClrUsed: " << infoHeader.biClrUsed << std::endl;
      std::cout << "ClrImportant: " << infoHeader.biClrImportant << std::endl;
}

int CalculatePadding(DWORD lineSize)
{
      auto padding = (lineSize % 4);
      return(padding ? 4 - padding : 0);
}

unsigned int ReadBmpData()
{
      if (rawBuffer.size() < 54)
      {
            return 1; // file too small to be a valid BMP
      }

      // Check the BMP signature
      if (rawBuffer[0] != 'B' || rawBuffer[1] != 'M')
      {
            return 2; // not a valid BMP file
      }

      BITMAPFILEHEADER fileHeader;
      BITMAPINFOHEADER infoHeader;

      // The buffer could be null, if you didn't read anything there
      memcpy(&fileHeader, rawBuffer.data(), sizeof(BITMAPFILEHEADER));
      memcpy(&infoHeader, rawBuffer.data() + sizeof(BITMAPFILEHEADER), sizeof(BITMAPINFOHEADER));

      PrintInfoHeader(infoHeader);

      DWORD pixelDataOffset = fileHeader.bfOffBits;
      DWORD bitsPerPixel = infoHeader.biBitCount;
      DWORD height = infoHeader.biHeight;
      DWORD width = infoHeader.biWidth;

      // The bits representing the bitmap pixels are packed in rows, also known
      // as strides or scan lines. The size of each row is rounded up to a multiple
      // of 4 bytes (a 32-bit DWORD) by padding. For images with height above 1,
      // multiple padded rows are stored consecutively, forming a PixelArray

      // Calculate proper byte line width
      // ERROR: this 8 is magic number, works only for 24-bit
      const int MAGIC = bitsPerPixel / 8;
      const DWORD byteLineSize = width * MAGIC;
      auto padding = CalculatePadding(byteLineSize);
      const DWORD fileLineSize = byteLineSize + padding;

      // The tricky part is, the image is upside down, it has optional padding for every line, and each pixel is in BGR, not RGB
      //std::vector<BYTE> output;
      output.resize(height * byteLineSize);
      for (DWORD i = 0; i < height; i++)
      {
            memcpy(output.data() + (height - 1 - i) * byteLineSize, rawBuffer.data() + pixelDataOffset + i * fileLineSize, byteLineSize);
      }

      // BMPs are usually stored "bottom-up"

      // BMP seems to be in BGR format, not RGB, I need to revert it
      for (DWORD i = 0; i < height; i++)
      {
            for (DWORD j = 0; j < width * 3; j += 3)
            {
                  auto temp = output[(i * byteLineSize) + j];
                  output[(i * byteLineSize) + j] = output[(i * byteLineSize) + j + 2];
                  output[(i * byteLineSize) + j + 2] = temp;
            }
      }

      return 0;
}

unsigned int EncodeBmpData(const std::vector<unsigned char>& imageData, DWORD width, DWORD height, DWORD bitsPerPixel)
{
      if (bitsPerPixel != 24)
      {
            return 1; // only 24-bit supported for now
      }
      DWORD byteLineSize = width * (bitsPerPixel / 8);
      auto padding = CalculatePadding(byteLineSize);
      DWORD fileLineSize = byteLineSize + padding;
      DWORD pixelDataSize = fileLineSize * height;
      BITMAPFILEHEADER fileHeader = { 0 };
      BITMAPINFOHEADER infoHeader = { 0 };
      fileHeader.bfType = 0x4D42; // 'BM'
      fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
      fileHeader.bfSize = fileHeader.bfOffBits + pixelDataSize;
      infoHeader.biSize = sizeof(BITMAPINFOHEADER);
      infoHeader.biWidth = width;
      infoHeader.biHeight = height;
      infoHeader.biPlanes = 1;
      infoHeader.biBitCount = bitsPerPixel;
      infoHeader.biCompression = BI_RGB;
      infoHeader.biSizeImage = pixelDataSize;
      infoHeader.biXPelsPerMeter = 0;
      infoHeader.biYPelsPerMeter = 0;
      infoHeader.biClrUsed = 0;
      infoHeader.biClrImportant = 0;
      outputBmp.clear();
      outputBmp.resize(fileHeader.bfSize);
      // Copy headers
      memcpy(outputBmp.data(), &fileHeader, sizeof(BITMAPFILEHEADER));
      memcpy(outputBmp.data() + sizeof(BITMAPFILEHEADER), &infoHeader, sizeof(BITMAPINFOHEADER));
      // Copy pixel data
      for (DWORD i = 0; i < height; i++)
      {
            // Copy pixel data line
            memcpy(outputBmp.data() + fileHeader.bfOffBits + (i * fileLineSize),
                   imageData.data() + ((height - 1 - i) * byteLineSize),
                   byteLineSize);
            // Add padding
            for (DWORD p = 0; p < padding; p++)
            {
                  outputBmp[fileHeader.bfOffBits + (i * fileLineSize) + byteLineSize + p] = 0;
            }
      }
      return 0;
}