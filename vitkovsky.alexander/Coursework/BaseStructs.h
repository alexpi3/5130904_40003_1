#ifndef BASESTRUCTS_H
#define BASESTRUCTS_H

#include <string>
#include <locale>
#include <codecvt>
#include <cwctype>
#include <iostream>
#include <set>
#include <memory>

std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

class Node
{
public:
    std::string key_;
    std::set<std::wstring> translations_;
    std::unique_ptr<Node> left_;
    std::unique_ptr<Node> right_;
    int height_;

    Node(std::string key, std::wstring translation) :
        key_(key),
        height_(1)
    {
        translations_.insert(translation);
    }

    // Автоматическое удаление дочерних узлов через unique_ptr
    ~Node() = default;

    Node(const Node& other) = delete;
    Node(Node&& other) = delete;
    Node& operator=(const Node& other) = delete;
    Node& operator=(Node&& other) = delete;

    void updateHeight()
    {
        int leftHeight = left_ ? left_->height_ : 0;
        int rightHeight = right_ ? right_->height_ : 0;
        height_ = std::max(leftHeight, rightHeight) + 1;
    }

    int getBalanceFactor() const
    {
        int leftHeight = left_ ? left_->height_ : 0;
        int rightHeight = right_ ? right_->height_ : 0;
        return leftHeight - rightHeight;
    }

    void print(std::wostream& out) const
    {
        std::wstring wkey = converter.from_bytes(key_);
        out << wkey << L": ";

        bool first = true;
        for (const auto& translation : translations_) {
            if (!first) out << L", ";
            out << translation;
            first = false;
        }
        out << std::endl;
    }
};

enum class RotateType
{
    LEFT,
    RIGHT
};

#endif // BASESTRUCTS_H