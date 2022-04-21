#include "widgetGraph.hpp"
#include <algorithm>
#include <spdlog/spdlog.h>

void widgetGraph::removeFromParent(node &parent, node &child)
	{
		parent.children.erase(std::remove(parent.children.begin(), parent.children.end(), &child), parent.children.end());
		if (parent.children.empty())
			{
				m_leafs.push_back(&parent);
			}
	}

widgetGraph::node &widgetGraph::addWidget(widget* widget)
	{
		node &n = *m_nodes.emplace();
		n.widget = widget;
		m_roots.push_back(&n);
		m_leafs.push_back(&n);
		return n;
	}

void widgetGraph::removeWidget(node &widgetNode)
	{
		if (widgetNode.parent)
			{
				removeFromParent(*widgetNode.parent, widgetNode);
			}
		else
			{
				m_roots.erase(std::remove(m_roots.begin(), m_roots.end(), &widgetNode), m_roots.end());
			}

		if (widgetNode.children.empty())
			{
				m_leafs.erase(std::remove(m_leafs.begin(), m_leafs.end(), &widgetNode), m_leafs.end());
			}
		else
			{
				m_roots.insert(m_roots.end(), widgetNode.children.begin(), widgetNode.children.end());
				for (auto &child : widgetNode.children)
					{
						child->parent = nullptr;
					}
			}

		m_nodes.erase(std::remove_if(m_nodes.begin(), m_nodes.end(), [&widgetNode](node &test){
			return &widgetNode == &test;
		}), m_nodes.end());
	}

void widgetGraph::addChild(node &parent, node &child)
	{
		if (child.parent)
			{
				spdlog::warn("Attempting to add child to parent where child already has parent!");
				return;
			}

		if (std::find(parent.children.begin(), parent.children.end(), &child) != parent.children.end())
			{
				spdlog::warn("Attempting to add child to parent who already has child!");
				return;
			}

		m_roots.erase(std::remove(m_roots.begin(), m_roots.end(), &child), m_roots.end());
		m_leafs.erase(std::remove(m_leafs.begin(), m_leafs.end(), &parent), m_leafs.end());

		parent.children.push_back(&child);
		child.parent = &parent;
	}
