#ifndef AVLTREEWORDLIST_H
#define AVLTREEWORDLIST_H

#include <iostream>
#include <algorithm>
#include <memory>
#include "BaseStructs.h"

class AVLTreeWordlist
{
private:
    std::unique_ptr<Node> root_;

    std::unique_ptr<Node> rotate(std::unique_ptr<Node> a, RotateType type)
    {
        std::unique_ptr<Node> b;

        switch (type)
        {
            case RotateType::LEFT:
            {
                b = std::move(a->right_);
                a->right_ = std::move(b->left_);
                b->left_ = std::move(a);
                break;
            }
            case RotateType::RIGHT:
            {
                b = std::move(a->left_);
                a->left_ = std::move(b->right_);
                b->right_ = std::move(a);
                break;
            }
        }

        b->left_->updateHeight();
        b->updateHeight();
        return b;
    }

    Node* findMinValueNode(Node* root) const
    {
        Node* current = root;
        while (current->left_)
            current = current->left_.get();
        return current;
    }

    std::unique_ptr<Node> insertNode(std::unique_ptr<Node> node, 
                                     const std::string& key, 
                                     const std::wstring& translation)
    {
        if (!node) 
            return std::make_unique<Node>(key, translation);

        if (key < node->key_)
        {
            node->left_ = insertNode(std::move(node->left_), key, translation);
        }
        else if (key > node->key_)
        {
            node->right_ = insertNode(std::move(node->right_), key, translation);
        }
        else
        {
            node->translations_.insert(translation);
            return node;
        }

        node->updateHeight();
        int balance = node->getBalanceFactor();

        if (balance > 1)
        {
            if (key > node->left_->key_)
            {
                node->left_ = rotate(std::move(node->left_), RotateType::LEFT);
            }
            return rotate(std::move(node), RotateType::RIGHT);
        }
        else if (balance < -1)
        {
            if (key < node->right_->key_)
            {
                node->right_ = rotate(std::move(node->right_), RotateType::RIGHT);
            }
            return rotate(std::move(node), RotateType::LEFT);
        }

        return node;
    }

    std::unique_ptr<Node> deleteNode(std::unique_ptr<Node> node, const std::string& key)
    {
        if (!node) return nullptr;

        if (key < node->key_)
        {
            node->left_ = deleteNode(std::move(node->left_), key);
        }
        else if (key > node->key_)
        {
            node->right_ = deleteNode(std::move(node->right_), key);
        }
        else
        {
            if (!node->left_ || !node->right_)
            {
                return node->left_ ? std::move(node->left_) : std::move(node->right_);
            }
            else
            {
                Node* temp = findMinValueNode(node->right_.get());
                node->key_ = temp->key_;
                node->translations_ = temp->translations_;
                node->right_ = deleteNode(std::move(node->right_), temp->key_);
            }
        }

        if (!node) return nullptr;

        node->updateHeight();
        int balance = node->getBalanceFactor();

        if (balance > 1)
        {
            if (node->left_->getBalanceFactor() < 0)
            {
                node->left_ = rotate(std::move(node->left_), RotateType::LEFT);
            }
            return rotate(std::move(node), RotateType::RIGHT);
        }
        else if (balance < -1)
        {
            if (node->right_->getBalanceFactor() > 0)
            {
                node->right_ = rotate(std::move(node->right_), RotateType::RIGHT);
            }
            return rotate(std::move(node), RotateType::LEFT);
        }

        return node;
    }

    Node* searchNode(Node* root, const std::string& key) const
    {
        if (!root) return nullptr;
        if (key == root->key_) return root;
        if (key < root->key_) return root->left_ ? searchNode(root->left_.get(), key) : nullptr;
        return root->right_ ? searchNode(root->right_.get(), key) : nullptr;
    }

    void printAsTree(std::wostream& out, const Node* node, int depth = 0, bool isLeft = false) const
    {
        if (!node) return;

        printAsTree(out, node->right_.get(), depth + 1, false);

        if (depth > 0)
        {
            std::string prefix = isLeft ? "\t\\------ " : "\t/------ ";
            std::wstring wprefix = converter.from_bytes(prefix);
            out << converter.from_bytes(std::string(2 * (depth - 1), '\t')) << wprefix;
        }
        node->print(out);

        printAsTree(out, node->left_.get(), depth + 1, true);
    }

public:
    AVLTreeWordlist() = default;
    ~AVLTreeWordlist() = default;

    // Автоматическое удаление дерева через unique_ptr
    void insert(const std::string& key, const std::wstring& translation) noexcept
    {
        try
        {
            root_ = insertNode(std::move(root_), key, translation);
        }
        catch (const std::exception& ex)
        {
            std::cerr << ex.what() << std::endl;
        }
    }

    void remove(const std::string& key) noexcept
    {
        try
        {
            root_ = deleteNode(std::move(root_), key);
        }
        catch (const std::exception& ex)
        {
            std::cerr << ex.what() << std::endl;
        }
    }

    void search(const std::string& key) const noexcept
    {
        try
        {
            Node* node = searchNode(root_.get(), key);
            if (!node) 
                throw std::invalid_argument(" \"" + key + "\" is unknown word");
            node->print(std::wcout);
        }
        catch (const std::exception& ex)
        {
            std::cerr << ex.what() << std::endl;
        }
    }

    void printAsTree() const noexcept
    {
        printAsTree(std::wcout, root_.get());
    }
};

#endif // AVLTREEWORDLIST_H