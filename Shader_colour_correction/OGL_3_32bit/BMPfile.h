#pragma once
#include <iostream>
#include <fstream>
#include <string>
const unsigned int HEADER_SIZE = 54;
class BMPfile {
private:
	char* buffHeader;
	char* pixelsArr;
	unsigned int fileSize, imageSize, width, height;
	std::ifstream srcFile;
	std::ofstream destFile;

	std::ios::pos_type get_file_size(std::ifstream* fileptr) {
		fileptr->seekg(0, std::ios::end);
		std::ios::pos_type filesize = fileptr->tellg();
		fileptr->seekg(0, std::ios::beg);
		return filesize;
	}
	void get_sizes() {
		memcpy(&width, &buffHeader[18], 4);    memcpy(&height, &buffHeader[22], 4);
	}


public:
	BMPfile(std::string srcPath, std::string destPath) {
		srcFile = std::ifstream(srcPath, std::ios::in | std::ios::binary);
		if (!srcFile) {
			std::cout << "ERROR with opening file: " + srcPath;
		}
		else {
			fileSize = get_file_size(&srcFile);
			imageSize = fileSize - HEADER_SIZE;

			buffHeader = new char[HEADER_SIZE];
			//pixelsArr = new byte[imageSize];

			srcFile.read((char *)buffHeader, HEADER_SIZE);
			//srcFile.read((char *)pixelsArrByte, imageSize);

			get_sizes();
			srcFile.close();
		}
		destFile=std::ofstream(destPath, std::ios::out | std::ios::binary);
		if (!destFile) {
			std::cout << "ERROR with opening file: " + destPath;
		}

	}
	void save_to_file(char* newPixelsArr) {
		destFile.write((char *)buffHeader, HEADER_SIZE);
		destFile.write((char *)newPixelsArr, imageSize);
		destFile.seekp(std::ios::beg);

	}
	BMPfile() {
		destFile.close();
		delete[] buffHeader; buffHeader = nullptr;
		delete[] pixelsArr; pixelsArr = nullptr;

	}
	

};