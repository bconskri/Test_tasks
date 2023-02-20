#include "parser.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>

using namespace parser;
using namespace std::literals;

Document LoadParseFile(const std::string &s) {
    std::istringstream strm(s);
    return parser::Load(strm);
}

std::string Print(const Node &node) {
    std::ostringstream out;
    parser::Print(Document{node}, out);
    return out.str();
}

void TestNull() {
    Node null_node;
    assert(null_node.IsNull());
    assert(!null_node.IsString());
    assert(!null_node.IsArray());

    Node null_node1{nullptr};
    assert(null_node1.IsNull());

    assert(Print(null_node) == "0,0,,null\n"s);
    assert(null_node == null_node1);
    assert(!(null_node != null_node1));

    const Node node = LoadParseFile("a = null"s).GetRoot();
    assert(node.IsNull());
    assert(node == null_node);
    // Пробелы, табуляции и символы перевода строки между токенами файла игнорируются
    assert(LoadParseFile("a = \t\r\n\n\r null \t\r\n\n\r "s).GetRoot() == null_node);
}

void TestStrings() {
    Node str_node{"Hello, \'everybody\'"s};
    assert(str_node.IsString());
    assert(str_node.AsString() == "Hello, \'everybody\'"s);

    assert(Print(str_node) == "0,0,,Hello, \'everybody\'\n"s);

    // Пробелы, табуляции и символы перевода строки между токенами файла игнорируются
    auto a = LoadParseFile("a = \t\r\n\n\r \"Hello\" \t\r\n\n\r ");
    assert(LoadParseFile("a = \t\r\n\n\r \"Hello\" \t\r\n\n\r ").GetRoot() == Node{"Hello"s});
}

void TestArray() {
    Node arr_node{Array{Node{"1"s}.SetName("x"s), Node{"0"s}.SetName("y"s), Node{"0"s}.SetName("z"s)}};
    arr_node.SetName("test");
    assert(arr_node.IsArray());
    const Array &arr = arr_node.AsArray();
    assert(arr.size() == 3);
    assert(arr.at(0).AsString() == "1"s);
    assert(arr.at(1).AsString() == "0"s);
    assert(arr.at(2).AsString() == "0"s);

    assert(LoadParseFile("test = {x=\"1\" y=\"0\" z=\"0\"}"s).GetRoot() == arr_node);
    assert(!LoadParseFile("test = {a = {x=\"1\" y=\"0\" z=\"0\"}}"s).GetRoot().IsNull());

    // Пробелы, табуляции и символы перевода строки между токенами файла игнорируются
    assert(LoadParseFile("test =  \r \n {x=\"1\" \r\n\t y=\"0\" \n \n  \t\t z=\"0\"}\n"s).GetRoot()
           == arr_node);
}

void MustFailToLoad(const std::string &s) {
    try {
        LoadParseFile(s);
        std::cerr << "ParsingError exception is expected on '"sv << s << "'"sv << std::endl;
        assert(false);
    } catch (const parser::ParsingError &) {
        // ok
    } catch (const std::exception &e) {
        std::cerr << "exception thrown: "sv << e.what() << std::endl;
        assert(false);
    } catch (...) {
        std::cerr << "Unexpected error"sv << std::endl;
        assert(false);
    }
}

void TestErrorHandling() {
    MustFailToLoad("["s);
    MustFailToLoad("]"s);

    MustFailToLoad("{"s);
    MustFailToLoad("}"s);
    MustFailToLoad("="s);

    MustFailToLoad("1a = \"1\""s); //имя начинается с цифры
    MustFailToLoad("a bc = \"1\""s); //в имени пробел

    MustFailToLoad("a = \"hello"s);  // незакрытая кавычка
    MustFailToLoad("a = {\"hello\""s);  // незакрытая }
    MustFailToLoad("a = [\"hello\"]"s);  // [] вместо {}
    MustFailToLoad("a = {}"s);  // пустой список не должен проходить //todo уточнить необходимость этого условия
    MustFailToLoad("a = {null}"s);  // нет имени узла в списке
    MustFailToLoad("a = {е = null b}"s);  // имя узла без значения
    MustFailToLoad("a = {е = null b=}"s);  // имя узла без значения
    MustFailToLoad("a = {е = null b=1}"s);  // значение узла не список и не строка
    MustFailToLoad("a = {{b = \"1\"}}"s);  // второй вложенный список без имени узла

    MustFailToLoad("nul"s);
}

void TestCase() {
    const std::string t_case = R"(shape = {
                    type = "tetrahedron"
                    vertices = {
                    point = { x = "1" y = "0" z = "0" }
                    point = { x = "0" y = "1" z = "0" }
                    point = { x = "0" y = "0" z = "1" }
                    point = { x = "1" y = "1" z = "1" }
                    }
                    color = { r = "0xFF" g = "0x00" b = "0x80" alpha = "0x80" }
                    })";

    auto root = LoadParseFile(t_case).GetRoot();

    const std::string t_out = R"(1,0,shape,{type vertices color}
  2,1,type,tetrahedron
  3,1,vertices,{point point point point}
    4,3,point,{x y z}
      5,4,x,1
      6,4,y,0
      7,4,z,0
    8,3,point,{x y z}
      9,8,x,0
      10,8,y,1
      11,8,z,0
    12,3,point,{x y z}
      13,12,x,0
      14,12,y,0
      15,12,z,1
    16,3,point,{x y z}
      17,16,x,1
      18,16,y,1
      19,16,z,1
  20,1,color,{r g b alpha}
    21,20,r,0xFF
    22,20,g,0x00
    23,20,b,0x80
    24,20,alpha,0x80
)"s;

    std::ostringstream s;
    PrintContext ctx(s, 2, 0);
    PrintNode(root, ctx);
    std::string ss = s.str();
    assert(s.str() == t_out);
}

void TestParser() {
    using namespace std::literals;

    TestNull();
    TestStrings();
    TestArray();
    TestErrorHandling();

    TestCase();

    std::cout << "Test passed"s << std::endl;
}

int main(int argc, char **argv) {

    //тест класса
    TestParser();

    if (argc != 3) {
        return -1;
    }

    std::ifstream inFile(argv[1]);
    if (inFile) {
        parser::Document doc = parser::Load(inFile);
        std::fstream outFile(argv[2], std::ios::out);
        if (outFile) {
            Print(doc, outFile);
            return 0;
        }
    }
    return -1;
}