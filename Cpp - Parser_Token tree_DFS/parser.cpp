#include "parser.h"
#include <cctype>
#include <iostream>
#include <string>
#include <vector>
#include <map>

namespace parser {

    bool Node::IsString() const {
        return std::holds_alternative<std::string>(*this);
    }

    bool Node::IsNull() const {
        return std::holds_alternative<std::nullptr_t>(*this);
    }

    bool Node::IsArray() const {
        return std::holds_alternative<std::vector<Node>>(*this);
    }

    const std::string &Node::AsString() const {
        using namespace std::literals;

        if (IsString()) {
            return std::get<std::string>(*this);
        }
        throw std::logic_error("AsString()"s);
    }

    const NodeVariant &Node::GetValue() const {
        return *this;
    }

    const Array &Node::AsArray() const {
        using namespace std::literals;

        if (IsArray()) {
            return std::get<std::vector<Node>>(*this);
        }
        throw std::logic_error("AsArray()"s);
    }

    Node Node::SetName(const std::string& name) {
        name_ = name;
        return *this;
    }

    const std::string Node::GetName() const {
        return name_;
    }

    Node Node::SetId(int id) {
        id_ = id;
        return *this;
    }

    const int Node::GetId() const {
        return id_;
    }

    Document::Document(Node root)
            : root_(move(root)) {
    }

    const Node &Document::GetRoot() const {
        return root_;
    }

    bool Document::operator==(const Document &rhs) const {
        return this->GetRoot() == rhs.GetRoot();
    }

    bool Document::operator!=(const Document &rhs) const {
        return this->GetRoot() != rhs.GetRoot();
    }

    Node LoadNode(std::istream &input);

    Node LoadString(std::istream &input) {
        using namespace std::literals;

        auto it = std::istreambuf_iterator<char>(input);
        auto end = std::istreambuf_iterator<char>();
        std::string s;
        while (true) {
            if (it == end) {
                // Поток закончился до того, как встретили закрывающую кавычку?
                throw ParsingError("String parsing error"s);
            }
            const char ch = *it;
            if (ch == '"') {
                // Встретили закрывающую кавычку
                ++it;
                break;
            } else if (ch == '\\') {
                // Встретили начало escape-последовательности
                ++it;
                if (it == end) {
                    // Поток завершился сразу после символа обратной косой черты
                    throw ParsingError("String parsing error"s);
                }
                const char escaped_char = *(it);
                // Обрабатываем одну из последовательностей: \\, \n, \t, \r, \"
                switch (escaped_char) {
                    case 'n':
                        s.push_back('\n');
                        break;
                    case 't':
                        s.push_back('\t');
                        break;
                    case 'r':
                        s.push_back('\r');
                        break;
                    case '"':
                        s.push_back('"');
                        break;
                    case '\\':
                        s.push_back('\\');
                        break;
                    default:
                        // Встретили неизвестную escape-последовательность
                        throw ParsingError("Unrecognized escape sequence \\"s + escaped_char);
                }
            } else if (ch == '\n' || ch == '\r') {
                // Строковый литерал внутри- значения "строка" не может прерываться символами \r или \n
                throw ParsingError("Unexpected end of line"s);
            } else {
                // Просто считываем очередной символ и помещаем его в результирующую строку
                s.push_back(ch);
            }
            ++it;
        }

        return Node(move(s));
    }

    std::string LoadName(std::istream &input) {
        using namespace std::literals;

        auto it = std::istreambuf_iterator<char>(input);
        auto end = std::istreambuf_iterator<char>();
        std::string s;
        while (true) {
            if (it == end) {
                // Поток закончился до того, как встретили =
                throw ParsingError("Node name parsing error"s);
            }
            const char ch = *it;
            if (ch == ' ' || ch == '=') {
                // Встретили окончание имени
                //++it;
                break;
            } else if (ch == '\\' || ch == '\n' || ch == '\r') {
                // Строковый литерал внутри имени не может прерываться символами \r или \n и содержать escape
                throw ParsingError("Node name parsing error. Unexpected end of line"s);
            } else {
                // Просто считываем очередной символ и помещаем его в результирующую строку
                s.push_back(ch);
            }
            ++it;
        }

        return move(s);
    }

    Node LoadNull(std::istream &input) {
        using namespace std::literals;

        std::string parsed_null;
        if (input.peek() == 'n') {
            for (uint8_t i = 0; i < 4; ++i) {
                parsed_null += static_cast<char>(input.get());
            }
            if (!input) {
                throw ParsingError("Failed to read null from stream"s);
            }
            if (parsed_null == "null"s) {
                return Node(nullptr);
            } else {
                throw ParsingError("Failed to read null from stream"s);
            }
        }
        throw ParsingError("Null expected"s);
    }

    Node LoadArray(std::istream &input) {
        using namespace std::literals;

        std::vector<Node> result;

        char c;
        for (c = '{'; input >> c && c != '}';) {
            if (c != ',') {
                input.putback(c);
            }
            result.push_back(LoadNode(input));
        }
        if ((c == '}' && result.empty()) || (c != '}')) {
            throw ParsingError("List parsing error"s);
        }
        return Node(move(result));
    }

    Node LoadValue(std::istream &input) {
        using namespace std::literals;

        char c;
        input >> c;

        try {
            if (c == '{') {
                return LoadArray(input);
            } else if (c == '"') {
                return LoadString(input);
            } else if (c == 'n') {
                input.putback(c);
                return LoadNull(input);
            } else {
                throw ParsingError("Неверный формат данных"s);
            }
        } catch (...) {
            throw ParsingError("Неверный формат данных"s);
        }
    }

    Node LoadNode(std::istream &input) {
        using namespace std::literals;

        char c;
        input >> c;

        try {
            if (c == '_' || std::isalpha(static_cast<char>(c))) {
                input.putback(c);
                std::string node_name = LoadName(input);
                input >> c;
                if (c != '=') {
                    throw ParsingError("Неверный формат данных"s);
                }
                const int id = UniqueID::GetNextID();
                return LoadValue(input).SetName(node_name).SetId(id);
            } else { //if (c == '{' || c == '}' || c == '"' || std::isdigit(static_cast<char>(c))) {
                throw ParsingError("Неверный формат данных"s);
            }
        } catch (...) {
            throw ParsingError("Неверный формат данных"s);
        }
    }

    Document Load(std::istream &input) {
        UniqueID::Clear();
        return Document{LoadNode(input)};
    }

    void PrintContext::PrintIndent() const {
        for (int i = 0; i < indent; ++i) {
            out.put(' ');
        }
    }

    PrintContext PrintContext::Indented() const {
        return {out, indent_step, indent_step + indent};
    }

    const PrintContext &operator<<(const PrintContext &ctx, const Array &arr) {
        bool first = true;
        ctx << '{';
        for (const auto &e: arr) {
            if (!first) {
                ctx << ' ';
            }
            first = false;
            ctx << e.GetName();
        }
        ctx << '}' << '\n';
        return ctx;
    }


    // Перегрузка функции PrintValue для вывода значений null
    void PrintValue(std::nullptr_t, const PrintContext &ctx, [[maybe_unused]] int parent_id) {
        using namespace std::literals;
        ctx << "null"s << '\n';
    }

    // Перегрузка функции PrintValue для вывода значений string
    void PrintValue(const std::string& text, const PrintContext &ctx, [[maybe_unused]] int parent_id) {
        using namespace std::literals;

        std::map<char, std::string> special_chars{
                {'\"', R"(\")"s},
                {'\r', R"(\r)"s},
                {'\n', R"(\n)"s},
                {'\t', "\t"s},
                {'\\', R"(\\)"s}
        };

        for (char x: text) {
            if (special_chars.count(x)) {
                ctx << special_chars.at(x);
            } else {
                ctx << x;
            }
        }
        ctx << '\n';
    }

    // Перегрузка функции PrintValue для вывода значений array
    void PrintValue(const Array &arr, const PrintContext &ctx, int parent_id) {

        ctx << arr;

        for (const auto &e: arr) {
            ctx.PrintIndent();
            ctx << e.GetId() << ',' << parent_id << ',' << e.GetName() << ',';
            std::visit(
                    [&ctx, &e](const auto &value) { PrintValue(value, ctx.Indented(), e.GetId()); },
                    e.GetValue());
        }
    }

    void PrintNode(Node const &node, const PrintContext &ctx) {
        using namespace std::literals;

        ctx.PrintIndent();
        ctx << node.GetId() << ",0"s << ',' << node.GetName() << ',';
        std::visit(
                [&ctx, &node](const auto &value) {
                    PrintValue(value, ctx.Indented(), node.GetId());
                },
                node.GetValue());
    }

    void Print(const Document &doc, std::ostream &out) {
        PrintContext ctx(out, 2, 0);
        PrintNode(doc.GetRoot(), ctx);
    }

    bool operator==(const Node &lhs, const Array &rhs) {
        return lhs.AsArray() == rhs;
    }

    //инициализация счетчика стартовым значением
    int UniqueID::nextID = 0;

} //namespace parser