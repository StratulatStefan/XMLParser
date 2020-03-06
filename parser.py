class Tree:
    def __init__(self,indexes,data):
        self.open = indexes[0]
        self.close = indexes[1]
        self.data = data
        self.childs = []

    @staticmethod
    def addNode(tree,indexes,data):
        last = None
        if len(tree.childs) > 0 :
            last = tree.childs[-1]
        else:
            tree.childs.append(Tree(indexes,data))
        if last:
            if last.close == indexes[0] - 1:
                tree.childs.append(Tree(indexes,data))
                return
            else:
                Tree.addNode(tree.childs[-1],indexes,data)

    def display(self, index):
        if self:
            print("\t" * index, end = "")
            print(self.data[0].strip('<').strip('>'), end = "")
            if len(self.data) > 1:
                print(" : " + self.data[1])
            else:
                print()
            if self.childs:
                for child in self.childs:
                    child.display(index + 1)
        else:
            return


class File:
    def __init__(self,file_name):
        self.file = open(file_name,'r')

    def getlines(self):
        return self.file.read().split('\n')

    def __del__(self):
        self.file.close()


class Parser:
    def __init__(self,file_name):
        self.file = File(file_name)
        self.lines = self.file.getlines()
        self.XMLconfiguration = {}
        self.elements = []
        self.xmltree = None

    def findclosetag(self,elements,starting_position, tag):
        for index, element in enumerate(elements[starting_position:]):
            for subelement in element:
                if index > 0:
                    if "<" + tag + ">" in subelement:
                        return False
                if "</" + tag + ">" in subelement:
                    return True, starting_position, index + starting_position
        return False

    def getXMLconfiguration(self):
        configuration = self.lines[0].split(' ')
        configuration[-1] = configuration[-1][:-2]
        for element in configuration[1:]:
            elements = element.split("=")
            self.XMLconfiguration.update({elements[0] : elements[1]})
        self.lines = self.lines[1:]


    def elementSegmentation(self):
        for index, line in enumerate(self.lines):
            first_tag = line[:line.find('>') + 1].strip()
            tag = line[line.find('>') + 1:].split("<")
            if len(tag) > 1:
                tag[-1] = "<" + tag[-1]
                self.elements.append([first_tag,tag[0],tag[1]])
            elif len(first_tag) > 1:
                self.elements.append([first_tag])

    def buildXMLTree(self):
        for index, element in enumerate(self.elements):
            if len(element) == 1:
                attribute = element[0].strip('<').strip('>')
                if "/" not in attribute:
                    validitate =  self.findclosetag(self.elements,index,attribute)
                    if validitate[1] == 0:
                        self.xmltree = Tree(validitate[1:],element)
                    else:
                        Tree.addNode(self.xmltree,validitate[1:],element)
            elif len(element) > 1:
                attribute = element[0].strip('<').strip('>')
                validitate = self.findclosetag(self.elements,index,attribute)
                Tree.addNode(self.xmltree,validitate[1:],element)


if __name__ == "__main__":
    parser = Parser("licenta.xml")
    parser.getXMLconfiguration()
    parser.elementSegmentation()
    parser.buildXMLTree()
    print(parser.XMLconfiguration)
    parser.xmltree.display(0)