#include "NodeFactory.h"

#include "CuteNode.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>


namespace
{
    const auto kDefaultNodeDir = QStringLiteral("templates");
    const auto kNodeFileName   = QStringLiteral("node_desc.json");
}


NodeFactory* NodeFactory::_instance = nullptr;

NodeFactory::NodeFactory()
{
    QDir nodeDir = QDir::current();
    nodeDir.cd(kDefaultNodeDir);
    collectAllNodeTypes(nodeDir);
}

NodeFactory::NodeFactory(const NodeFactory& other) { _instance = other._instance; }
NodeFactory& NodeFactory::operator=(const NodeFactory& other)
{
    if (this != &other)
        _instance = other._instance;

    return *this;
}

NodeFactory& NodeFactory::getInstance()
{
    static NodeFactory actualInstance;
    _instance = &actualInstance;

    return *_instance;
}

CuteNode* NodeFactory::createNode(const QString& name)
{
    auto node = new CuteNode;
    node->read(_templatesByName[name]);
    return node;
}

void NodeFactory::collectAllNodeTypes(const QDir& nodeTypeDir)
{
    const auto nodeDirs = QDir{nodeTypeDir}.entryList();
    for (const auto& dir: nodeDirs)
    {
        QDir baseDir{nodeTypeDir};
        baseDir.cd(dir);

        QFile nodeDescFile(baseDir.absoluteFilePath(kNodeFileName));
        if (!nodeDescFile.exists())
            continue;

        if (!nodeDescFile.open(QIODevice::ReadOnly))
            continue;

        const auto content = nodeDescFile.readAll();
        QJsonParseError parseError;
        const auto document = QJsonDocument::fromJson(content, &parseError);
        if (parseError.error != QJsonParseError::NoError)
            continue;

        auto json = document.object();

        // This patches the path to the icon, so that it is relative to the root dir of the project.
        QFileInfo iconFile(json["icon"].toString());
        if (!iconFile.exists() && iconFile.isRelative())
        {
            json["icon"] = QDir::toNativeSeparators(baseDir.path() + '/' + json["icon"].toString());
        }

        _templatesByName[json["name"].toString()] = json;
    }
}

const QJsonObject& NodeFactory::getNodeDescByName(const QString& name) const
{
    const auto nodeDescIter = _templatesByName.find(name);
    if (nodeDescIter == end(_templatesByName))
        qDebug() << "Node Description for Node " << name << " not found!";

    return nodeDescIter->second;
}

QStringList NodeFactory::getNodeTypeNames() const
{
    QStringList nodeNames;
    nodeNames.reserve(_templatesByName.size());

    Q_FOREACH (const auto& node, _templatesByName)
    {
        nodeNames.append(node.first);
    }

    return nodeNames;
}
