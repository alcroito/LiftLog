#ifndef GENERIC_TREE_NODE_H
#define GENERIC_TREE_NODE_H

#include <QVariant>

class GenericTreeNode {
public:
    explicit GenericTreeNode(const QVariant& theData, GenericTreeNode* parentItem = 0) : parent(parentItem), itemData(theData) {}
    ~GenericTreeNode() { qDeleteAll(children); }
    void appendChild(GenericTreeNode* child) { children.append(child); }
    void prependChild(GenericTreeNode* child) { children.prepend(child); }
    GenericTreeNode* child(int row) { return children.value(row); }
    int childCount() const { return children.count(); }
    int columnCount() const { return 1; }
    QVariant data() { return itemData; }
    void setData(QVariant newData) { itemData = newData; }
    int row() const {
        if (parent)
            return parent->children.indexOf(const_cast<GenericTreeNode*>(this));
        return 0;
    }
    GenericTreeNode* parentItem() { return parent; }
    void deleteChild(int row) { delete children.value(row); children.removeAt(row);}
private:
    GenericTreeNode* parent;
    QList<GenericTreeNode*> children;
    QVariant itemData;
};

#endif // GENERIC_TREE_NODE_H

