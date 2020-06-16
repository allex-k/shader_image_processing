#pragma once
#include <iostream>
#include <fstream>
#include <string>

#define HEADER_SIZE 54
class BMPfile {
private:
	uint8_t *buffHeader;
	size_t fileSize, imageSize, width, height;
	std::ifstream headerFile;
	std::string destPath;

public:
	BMPfile(std::string headerPath, std::string _destPath, uint32_t _width, uint32_t _height) 
	{
		headerFile = std::ifstream(headerPath, std::ios::in | std::ios::binary);
		if (!headerFile) std::cout << "ERROR with header file: " + headerPath;
		else
		{
			destPath = _destPath;
			width = _width; height = _height;
			imageSize = width * height * 3;
			fileSize = HEADER_SIZE + imageSize;

			buffHeader = new uint8_t[HEADER_SIZE];
			headerFile.read((char *)buffHeader, HEADER_SIZE);
			headerFile.close();
			//get_sizes();

			memcpy(&buffHeader[18], &width, 4);
			memcpy(&buffHeader[22], &height, 4);
			memcpy(&buffHeader[2], &fileSize, 4);
		}
	}

	
	void save_to_file(uint8_t* newPixelsArr) {
		std::ofstream destFile(destPath, std::ios::out | std::ios::binary);
		if (!destFile) std::cout << "ERROR with opening file: " + destPath;

		destFile.write((char *)buffHeader, HEADER_SIZE);
		destFile.write((char *)newPixelsArr, imageSize);
		destFile.seekp(std::ios::beg);
		destFile.close();
	}
	size_t get_width() { return width; }
	size_t get_height() { return height; }
	size_t get_imageSize() { return imageSize; }
	size_t get_fileSize() { return fileSize; }

	~BMPfile() {
		delete[] buffHeader; buffHeader = nullptr;

	}


};
