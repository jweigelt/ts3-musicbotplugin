/*
* This file is part of Music Bot Plugin. (https://github.com/jweigelt/ts3-musicbotplugin)
* Copyright (C) 2017 Jan Weigelt (jan@janelo.net)
*
* Music Bot Plugin is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.

* Music Bot Plugin is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.

* You should have received a copy of the GNU General Public License
* along with Music Bot Plugin. If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once
#include <iostream>
#include <fstream>
using namespace std;

#define DEBUG_DUMPFILE "C:\\Users\\Jan\\Desktop\\dump.txt"
#define DEBUG_READFILE "C:\\Users\\Jan\\Desktop\\dump.txt"


void BinaryDump(uint8_t* buf) {
	ofstream myfile;
	myfile.open(DEBUG_DUMPFILE);
	myfile << (const char*) buf;
	myfile.close();
}

uint8_t* BinaryRead() {

	ifstream myfile(DEBUG_READFILE, ios::binary | ios::ate);
	myfile.seekg(0, ios::end);
	streampos sz = myfile.tellg();
	myfile.seekg(0);
	uint8_t* buf = (uint8_t*)malloc((size_t)sz);
	myfile.read((char*)buf, sz);
	return buf;

}