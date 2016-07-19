#include <map>

/// forwards
class CuteNode;
class QDir;
class QJsonObject;
class QString;
class QStringList;
///

/// Instantiates all types of nodes that have been specified as a template json.
/// All node descriptions from the templates will be stored in the class during its initialization.
/// This way there is no further disk access needed every time a node is created.
class NodeFactory
{
    static NodeFactory* _instance;

    NodeFactory();
    ~NodeFactory() = default;

    NodeFactory(const NodeFactory& other);
    NodeFactory& operator=(const NodeFactory& other);

public:
    static NodeFactory& getInstance();

    // ownership is transferred to caller
    CuteNode* createNode(const QString& name);

    const QJsonObject& getNodeDescByName(const QString& name) const;
    QStringList getNodeTypeNames() const;

private:
    void collectAllNodeTypes(const QDir& nodeTypeDir);

private:
    std::map<QString, QJsonObject> _templatesByName;
};
