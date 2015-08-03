#ifndef WORKOUTTREENODE_H
#define WORKOUTTREENODE_H

#include <QVariant>

class WorkoutTreeNode {
public:
    explicit WorkoutTreeNode(const QVariant& theData, WorkoutTreeNode* parentItem = 0) : parent(parentItem), itemData(theData) {}
    ~WorkoutTreeNode() { qDeleteAll(children); }
    void appendChild(WorkoutTreeNode* child) { children.append(child); }
    WorkoutTreeNode* child(int row) { return children.value(row); }
    int childCount() const { return children.count(); }
    int columnCount() const { return 1; }
    QVariant data() { return itemData; }
    int row() const {
        if (parent)
            return parent->children.indexOf(const_cast<WorkoutTreeNode*>(this));
        return 0;
    }
    WorkoutTreeNode* parentItem() { return parent; }
private:
    WorkoutTreeNode* parent;
    QList<WorkoutTreeNode*> children;
    QVariant itemData;
};


#endif // WORKOUTTREENODE_H
