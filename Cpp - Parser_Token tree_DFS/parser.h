#pragma once

#include <istream>
#include <string>
#include <vector>
#include <variant>

namespace parser {

    //счетчик для инкремента Id
    class UniqueID {
    public:
        static int nextID;
        static int GetNextID() {
            return ++nextID;
        }
        static int Clear() {
            return nextID = 0;
        }
    };

    class Node;

    using Array = std::vector<Node>;
    using NodeVariant = std::variant<std::nullptr_t, std::vector<Node>, std::string>;

    class Node : public std::variant<std::nullptr_t, std::vector<Node>, std::string> {
    public:
        using variant::variant;

        bool IsString() const;

        bool IsNull() const;

        bool IsArray() const;

        const std::string &AsString() const;

        const Array &AsArray() const;

        const NodeVariant &GetValue() const;

        Node SetName(const std::string& name);

        const std::string GetName() const;

        Node SetId(int id);

        const int GetId() const;

    private:
        std::string name_ = "";
        int id_ = 0;
    };

    class Document {
    public:
        explicit Document(Node root);

        const Node &GetRoot() const;

        bool operator==(const Document &rhs) const;

        bool operator!=(const Document &rhs) const;

    private:
        Node root_;
    };

    class ParsingError : public std::runtime_error {
    public:
        using runtime_error::runtime_error;
    };

    // Контекст вывода, хранит ссылку на поток вывода и текущий отступ
    struct PrintContext {
        std::ostream &out;
        int indent_step = 2;
        int indent = 0;

        PrintContext(std::ostream &out)
                : out(out) {}

        PrintContext(std::ostream &out, int indent_step, int indent = 0)
                : out(out), indent_step(indent_step), indent(indent) {}

        //печатает текущий отступ
        void PrintIndent() const;

        // Возвращает новый контекст вывода с увеличенным смещением
        PrintContext Indented() const;
    };

    template<typename T>
    void PrintValue(const T &value, std::ostream &out) { // для печати null_ptr
        out << std::boolalpha << value;
    }

    template<typename T>
    const PrintContext &operator<<(const PrintContext &ctx, const T &val) {
        ctx.out << val;
        return ctx;
    }

    // Перегрузка функции PrintValue для вывода значений null
    void PrintValue(std::nullptr_t, const PrintContext &ctx, [[maybe_unused]] int parent_id);

    // Перегрузка функции PrintValue для вывода значений string
    void PrintValue(const std::string& text, const PrintContext &ctx, [[maybe_unused]] int parent_id);

    // Перегрузка функции PrintValue для вывода значений array
    void PrintValue(const Array &, const PrintContext &ctx, int parent_id);

    void PrintNode(Node const &node, const PrintContext &ctx);

    void Print(const Document &doc, std::ostream &out);

    Document Load(std::istream &input);

    bool operator==(const Node &lhs, const Array &rhs);

} //namespace parser