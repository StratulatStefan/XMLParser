#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

class XMLFile {
	std::ifstream *file;
public:
	XMLFile(const std::string & title);
	std::vector<std::string> getlines();
	~XMLFile();
};

class Tree {
	uint16_t open;
	uint16_t close;
	std::vector<std::string> data;
	std::vector<Tree*> childs;
public:
	Tree(std::pair<uint16_t, uint16_t> indexes, std::vector<std::string> data);
	static void addNode(Tree &tree, std::pair<uint16_t, uint16_t> indexes, std::vector<std::string> data);
	void display(uint8_t index);
};


class Parser {
	XMLFile *file;
	std::vector<std::string> lines;
	std::map<std::string, std::string> XMLconfiguration;
	std::vector<std::vector<std::string>> elements;
	Tree *xmltree;
	friend class FormatOperations;

	std::pair<uint16_t, uint16_t> findclosetag(std::vector<std::vector<std::string>> elements,
		uint16_t starting_position, std::string tag);
public:
	Parser(const std::string & filename);
	std::map<std::string, std::string> getconfig();
	void getXMLConfiguration();
	void elementSegmentation();
	void buildXMLTree();
	Tree Parser::getTree();
};


class FormatOperations {
public:
	static void displayMap(Parser parser);
	static void displayVectors(Parser parser);
	static std::vector<std::string> split(std::string string, std::string delimiter);
	static std::vector<std::string> substrings(std::vector<std::string> target, uint16_t index);
	static void strip(std::string & target, std::string character);
};

