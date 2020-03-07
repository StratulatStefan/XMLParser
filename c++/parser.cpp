#include "parser.h"


XMLFile::XMLFile(const std::string & title) {
	file = new std::ifstream(title);
}

std::vector<std::string> XMLFile::getlines() {
	std::vector<std::string> lines{};
	std::string line;
	while (std::getline(*this->file, line)) {
		lines.push_back(line);
	}
	return lines;
}

XMLFile::~XMLFile() {
	if (file != nullptr) {
		delete file;
	}
}

/* --------------------------------------------------------------------------------------- */

Tree::Tree(std::pair<uint16_t, uint16_t> indexes, std::vector<std::string> data) {
	this->open = indexes.first;
	this->close = indexes.second;
	this->data = data;
	this->childs = {};
}

void Tree::addNode(Tree &tree, std::pair<uint16_t, uint16_t> indexes, std::vector<std::string> data) {
	Tree *last = nullptr;
	if (tree.childs.size() > 0) {
		last = tree.childs.back();
	}
	else {
		tree.childs.push_back(new Tree(indexes, data));
	}
	if (last != nullptr) {
		if (last->close == indexes.first - 1) {
			tree.childs.push_back(new Tree(indexes, data));
			return;
		}
		else {
			Tree::addNode(*tree.childs.back(), indexes, data);
		}
	}
}

void Tree::display(uint8_t index) {
	if (this != nullptr) {
		std::cout << std::string(index, '\t');
		std::cout << this->data.front();
		if (this->data.size() > 1) {
			std::cout << " : " << this->data[1] << std::endl;
		}
		else {
			std::cout << std::endl;
		}
	}
	else {
		return;
	}
}

/* --------------------------------------------------------------------------------------- */

void FormatOperations::displayMap(Parser parser){
	for (const auto & element : parser.XMLconfiguration) {
		std::cout << element.first << " : " << element.second << std::endl;
	}
	std::cout << std::endl;
}

std::vector<std::string> FormatOperations::split(std::string string, std::string delimiter) {
	std::vector<std::string> tokens = {};
	size_t pos = 0;
	std::string token;
	while ((pos = string.find(delimiter)) != std::string::npos) {
		token = string.substr(0, pos);
		//std::cout << token << std::endl;
		tokens.push_back(token);
		string.erase(0, pos + delimiter.length());
	}
	if (string.length() > 0) {
		tokens.push_back(string);
	}
	return tokens;
}

std::vector<std::string> FormatOperations::substrings(std::vector<std::string> target, uint16_t index) {
	std::vector<std::string> substr = {};
	for (std::vector<std::string>::iterator it = target.begin() + index; it != target.end(); it++) {
		substr.push_back(*it);
	}
	return substr;
}

void FormatOperations::strip(std::string & target, std::string character) {
	int index_bracket = target.find(character);
	target.erase(index_bracket, character.length());
}

void FormatOperations::displayVectors(Parser parser) {
	for (const auto & element : parser.elements) {
		for (const auto & subelement : element) {
			std::cout << subelement;
		}
		std::cout << std::endl;
	}
}

/* --------------------------------------------------------------------------------------- */

std::pair<uint16_t, uint16_t> Parser::findclosetag(std::vector<std::vector<std::string>> elements,
	uint16_t starting_position, std::string tag) {
	for (std::vector<std::vector<std::string>>::iterator element = elements.begin() + starting_position; element != elements.end(); element++) {
		for (const auto & subelement : (*element)) {
			if (element - elements.begin() > 0) {
				std::string substring = std::string("<" + tag + ">");
				if (subelement.find(substring) != std::string::npos) {
					return std::pair<uint16_t, uint16_t>{-1, -1};
				}
			}
			std::string substring = std::string("</" + tag + ">");
			if (subelement.find(substring) != std::string::npos) {
				return std::pair<uint16_t, uint16_t>{starting_position, element - elements.begin() + starting_position};
			}
		}

	}
	return std::pair<uint16_t, uint16_t>{-1, -1};
}

Parser::Parser(const std::string & filename) {
	this->file = new XMLFile(filename);
	this->lines = this->file->getlines();
	this->XMLconfiguration = {};
	this->elements = {};
}

std::map<std::string, std::string> Parser::getconfig() {
		return this->XMLconfiguration;
	}

void Parser::getXMLConfiguration() {
	std::vector<std::string> configuration = FormatOperations::split(this->lines[0], std::string(" "));
	for (const auto & element : configuration) {
		if (element != configuration.front()) {
			std::vector<std::string> elements = FormatOperations::split(element, std::string("="));
			this->XMLconfiguration.insert(std::pair<std::string, std::string>{elements[0], elements[1]});
		}
	}
	this->lines = FormatOperations::substrings(this->lines, 1);
}

void Parser::elementSegmentation() {
	for (std::vector<std::string>::iterator line = this->lines.begin(); line != this->lines.end(); line++) {
		unsigned short int bracket_position = (*line).find(std::string(">")) + 1;
		std::string line_copy(*line);
		std::string first_tag = line_copy.erase(bracket_position, line_copy.length() - bracket_position);
		std::string tag = (*line).erase(0, bracket_position);
		std::vector<std::string> tags = FormatOperations::split(tag, std::string("<"));
		if (tags.size() > 1) {
			std::string aux = "<" + tags.back();
			tags.pop_back();
			tags.push_back(aux);
			this->elements.push_back(std::vector<std::string>{ first_tag,tags[0],tags[1] });
		}
		else if (first_tag.length() > 1) {
			this->elements.push_back(std::vector<std::string>{first_tag});
		}
	}
}

void Parser::buildXMLTree() {
	for (std::vector<std::vector<std::string>>::iterator line = this->elements.begin(); line != this->elements.end(); line++) {
		if ((*line).size() == 1) {
			std::string attribute((*line)[0]);
			FormatOperations::strip(attribute, std::string("<"));
			FormatOperations::strip(attribute, std::string(">"));
			if (attribute.find(std::string("/")) > attribute.length()) {
				std::pair<uint16_t, uint16_t> validitate = this->findclosetag(this->elements, (uint16_t)(line - this->elements.begin()), attribute);
				if (validitate.first == 0) {
					this->xmltree = new Tree(validitate, (*line));
				}
				else {
					Tree::addNode(*this->xmltree, validitate, *line);
				}
			}
		}
		else if ((*line).size() > 1) {
			std::string attribute((*line)[0]);
			FormatOperations::strip(attribute, std::string("<"));
			FormatOperations::strip(attribute, std::string(">"));
			std::pair<uint16_t, uint16_t> validitate = this->findclosetag(this->elements, (uint16_t)(line - this->elements.begin()), attribute);
			Tree::addNode(*this->xmltree, validitate, *line);
		}
	}
}

Tree Parser::getTree() {
	return *this->xmltree;
}

/* --------------------------------------------------------------------------------------- */


int main() {
	Parser parser("licenta.xml");
	parser.getXMLConfiguration();
	std::map<std::string, std::string> xmlconfiguration = parser.getconfig();
	FormatOperations::displayMap(parser);
	parser.elementSegmentation();
	parser.buildXMLTree();
	parser.getTree().display(0);
	system("pause");
	return 0;
}