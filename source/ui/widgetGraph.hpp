// widgetGraph.hpp
// All widgets are defined in a graph where the base widget is a product of all parent widgets
#pragma once
#include <vector>
#include <plf_colony.h>

struct widget;
class widgetGraph
	{
		public:
			struct node
				{
					private:
						std::vector<node*> children;
						node *parent = nullptr;
						widget *widget = nullptr;

						friend class widgetGraph;
				};

		private:
			plf::colony<node> m_nodes;
			std::vector<node*> m_leafs;
			std::vector<node*> m_roots;

			void removeFromParent(node &parent, node &child);

		public:
			const std::vector<node*> &roots = m_roots;

			node &addWidget(widget *widget);
			void removeWidget(node& widgetNode);

			void addChild(node &parent, node &child);

	};
