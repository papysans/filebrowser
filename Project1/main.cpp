#define _CRT_SECURE_NO_WARNINGS
using namespace std;

#include <iostream>
#include <string>
#include <vector>

struct Node
{
    string name;
    string type;
    int size;
    string extension;
    string lastModified;
    vector<Node *> children;
    Node *parent;
};

class FileBrowser
{
private:
    Node *root;

public:
    FileBrowser()
    {
        root = new Node();
        root->name = "Root";
        root->type = "Folder";
        root->size = 0;
        root->extension = "";
        root->lastModified = "";
    }

    int getMaxDepth(Node *node)
    {
        if (node == nullptr)
        {
            return 0;
        }

        int maxDepth = 0;
        for (Node *child : node->children)
        {
            int childDepth = getMaxDepth(child);
            if (childDepth > maxDepth)
            {
                maxDepth = childDepth;
            }
        }

        return maxDepth + 1;
    }

    void insertNode(Node *parent, Node *child)
    {
        parent->children.push_back(child);
        child->parent = parent;
    }

    void deleteNode(Node *node)
    {
        if (node == nullptr)
        {
            return;
        }

        for (Node *child : node->children)
        {
            deleteNode(child);
        }

        delete node;
    }

    void printNode(Node *node)
    {
        if (node == nullptr)
        {
            return;
        }

        for (Node *child : node->children)
        {
            cout << child->name << " (" << child->type << ")" << endl;
        }
    }

    void printAllNode(Node *node, int level)
    {
        if (node == nullptr)
        {
            return;
        }

        for (int i = 0; i < level; i++)
        {
            cout << "  ";
        }

        cout << node->name << " (" << node->type << ")" << endl;

        for (Node *child : node->children)
        {
            printAllNode(child, level + 1);
        }
    }

    string getCurrentPath(Node *node)
    {
        if (node == root)
        {
            return node->name;
        }
        else
        {
            return getCurrentPath(node->parent) + "/" + node->name;
        }
    }

    Node *searchNode(Node *node, const string &name)
    {
        if (node == nullptr)
        {
            return nullptr;
        }

        if (node->name == name)
        {
            return node;
        }

        for (Node *child : node->children)
        {
            Node *result = searchNode(child, name);
            if (result != nullptr)
            {
                return result;
            }
        }
        return nullptr;
    }

    void moveNode(Node *sourceNode, Node *destNode)
    {
        // 从源节点的父节点的子节点列表中移除源节点
        auto &sourceSiblings = sourceNode->parent->children;
        sourceSiblings.erase(remove(sourceSiblings.begin(), sourceSiblings.end(), sourceNode), sourceSiblings.end());

        // 将源节点添加到目标节点的子节点列表中
        destNode->children.push_back(sourceNode);

        // 更新源节点的父节点
        sourceNode->parent = destNode;
    }

    void copyNode(Node *sourceNode, Node *destNode)
    {
        // 创建新节点并复制源节点的数据
        Node *newNode = new Node(*sourceNode);
        newNode->children.clear(); // 清除复制过来的子节点指针

        // 将新节点添加到目标节点的子节点列表中
        destNode->children.push_back(newNode);

        // 更新新节点的父节点
        newNode->parent = destNode;

        // 递归复制源节点的所有子节点
        for (Node *child : sourceNode->children)
        {
            copyNode(child, newNode);
        }
    }

    // void printFileContent(Node *node)
    // {
    //     if (node->type != "File")
    //     {
    //         cout << "Node is not a file." << endl;
    //         return;
    //     }

    //     // 假设文件内容存储在节点的一个名为content的成员变量中
    //     cout << node->content << endl;
    // }

    void printNodeDetails(Node *node)
    {
        cout << "Name: " << node->name << endl;
        cout << "Type: " << node->type << endl;
        cout << "Size: " << node->size << endl;
        cout << "Extension: " << node->extension << endl;
        cout << "Last Modified: " << node->lastModified << endl;
    }
    Node *getRoot()
    {
        return root;
    }
};

int main()
{
    FileBrowser fileBrowser;

    Node *root = fileBrowser.getRoot();
    int depth = fileBrowser.getMaxDepth(root);
    Node *downloads = new Node();
    downloads->name = "Downloads";
    downloads->type = "Folder";
    downloads->size = 0;
    downloads->extension = "";
    downloads->lastModified = "";
    fileBrowser.insertNode(root, downloads);

    Node *studyMaterials = new Node();
    studyMaterials->name = "Study Materials";
    studyMaterials->type = "Folder";
    studyMaterials->size = 0;
    studyMaterials->extension = "";
    studyMaterials->lastModified = "";
    fileBrowser.insertNode(downloads, studyMaterials);

    Node *file1 = new Node();
    file1->name = "File1";
    file1->type = "File";
    file1->size = 100;
    file1->extension = ".txt";
    file1->lastModified = "2022-01-01";
    fileBrowser.insertNode(studyMaterials, file1);

    Node *file2 = new Node();
    file2->name = "File2";
    file2->type = "File";
    file2->size = 200;
    file2->extension = ".pdf";
    file2->lastModified = "2022-01-02";
    fileBrowser.insertNode(studyMaterials, file2);

    Node *currentNode = root;

    while (true)
    {
        string command;
        cout << fileBrowser.getCurrentPath(currentNode) << " > ";
        getline(cin, command);

        if (command == "exit")
        {
            break;
        }
        else if (command == "clear")
        {
            #ifdef _WIN32
            system("cls");
            #else
            system("clear");
            #endif
        }
        else if (command == "ls")
        {
            fileBrowser.printNode(currentNode);
        }
        else if (command == "lsall")
        {
            fileBrowser.printAllNode(currentNode, depth);
        }
        else if (command.substr(0, 2) == "cd")
        {
            if (command.substr(3) == "..")
            {
                if (currentNode->parent != nullptr)
                {
                    currentNode = currentNode->parent;
                }
                else
                {
                    cout << "Already at root folder." << endl;
                }
            }
            else
            {
                string folderName = command.substr(3);
                Node *folder = fileBrowser.searchNode(currentNode, folderName);
                if (folder != nullptr && folder->type == "Folder")
                {
                    currentNode = folder;
                }
                else
                {
                    cout << "Folder not found." << endl;
                }
            }
        }
        else if (command.substr(0, 5) == "touch")
        {
            string fileName = command.substr(6);
            Node *file = new Node();
            file->name = fileName;
            file->type = "File";
            file->size = 0;
            file->extension = "";
            file->lastModified = "";
            fileBrowser.insertNode(currentNode, file);
        }
        else if (command.substr(0, 5) == "mkdir")
        {
            string folderName = command.substr(6);
            Node *folder = new Node();
            folder->name = folderName;
            folder->type = "Folder";
            folder->size = 0;
            folder->extension = "";
            folder->lastModified = "";
            fileBrowser.insertNode(currentNode, folder);
        }
        else if (command.substr(0, 2) == "rm")
        {
            string nodeName = command.substr(3);
            Node *node = fileBrowser.searchNode(currentNode, nodeName);
            if (node != nullptr)
            {
                fileBrowser.deleteNode(node);
            }
            else
            {
                cout << "File or folder not found." << endl;
            }
        }

        else if (command.substr(0, 2) == "mv")
        {
            string sourceName = command.substr(3, command.find(' ', 3) - 3);
            string destName = command.substr(command.find(' ', 3) + 1);
            Node *sourceNode = fileBrowser.searchNode(currentNode, sourceName);
            Node *destNode = fileBrowser.searchNode(currentNode, destName);
            if (sourceNode != nullptr && destNode != nullptr && destNode->type == "Folder")
            {
                fileBrowser.moveNode(sourceNode, destNode);
            }
            else
            {
                cout << "Invalid source or destination." << endl;
            }
        }
        else if (command.substr(0, 2) == "cp")
        {
            string sourceName = command.substr(3, command.find(' ', 3) - 3);
            string destName = command.substr(command.find(' ', 3) + 1);
            Node *sourceNode = fileBrowser.searchNode(currentNode, sourceName);
            Node *destNode = fileBrowser.searchNode(currentNode, destName);
            if (sourceNode != nullptr && destNode != nullptr && destNode->type == "Folder")
            {
                fileBrowser.copyNode(sourceNode, destNode);
            }
            else
            {
                cout << "Invalid source or destination." << endl;
            }
        }
        else if (command.substr(0, 4) == "find")
        {
            string nodeName = command.substr(5);
            Node *node = fileBrowser.searchNode(currentNode, nodeName);
            if (node != nullptr)
            {
                cout << "Found " << nodeName << " at " << fileBrowser.getCurrentPath(node) << endl;
            }
            else
            {
                cout << nodeName << " not found." << endl;
            }
        }
        else if (command.substr(0, 6) == "rename")
        {
            string oldName = command.substr(7, command.find(' ', 7) - 7);
            string newName = command.substr(command.find(' ', 7) + 1);
            Node *node = fileBrowser.searchNode(currentNode, oldName);
            if (node != nullptr)
            {
                node->name = newName;
            }
            else
            {
                cout << "File or folder not found." << endl;
            }
        }
        else if (command.substr(0, 4) == "stat")
        {
            string nodeName = command.substr(5);
            Node *node = fileBrowser.searchNode(currentNode, nodeName);
            if (node != nullptr)
            {
                fileBrowser.printNodeDetails(node);
            }
            else
            {
                cout << "File or folder not found." << endl;
            }
        }

        else
        {
            cout << "Invalid command." << endl;
        }
    }
}
