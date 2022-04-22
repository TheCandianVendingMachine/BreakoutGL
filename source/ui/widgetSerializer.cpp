#include "widgetSerializer.hpp"
#include "widgetManager.hpp"
#include "str.hpp"
#include "widgetTransformEnums.hpp"
#include "widgetTypes.hpp"
#include <robin_hood.h>
#include <vector>
#include <cstdio>
#include <spdlog/spdlog.h>
#include <glm/vec2.hpp>
#include <string>
#include <cctype>
#include <charconv>
#include <queue>
#undef ABSOLUTE

struct serialisedWidget
	{
		fe::str name = 0;

		widgetTypes widgetType = widgetTypes::NONE;
		std::queue<fe::str> parents;
		fe::str immediateParent = 0;

		widgetTransformEnums::anchor anchor = widgetTransformEnums::anchor::MIDDLE;
		widgetTransformEnums::type sizeType = widgetTransformEnums::type::ABSOLUTE;
		widgetTransformEnums::type positionType = widgetTransformEnums::type::ABSOLUTE;

		glm::vec2 size = { 0, 0 };
		glm::vec2 position = { 0, 0 };

		fe::str onMouseEnterEvent = 0;
		fe::str onMouseLeaveEvent = 0;
		fe::str onClickEvent = 0;
		fe::str onClickStartEvent = 0;
		fe::str onClickEndEvent = 0;
		fe::str onDoubleClickEvent = 0;
		fe::str onDrawEvent = 0;

		bool noMouseInteraction = false;

		fe::str texture = 0;
	};

widgetTypes getWidgetTypeFromString(fe::str string)
	{
		switch (string)
			{
				case FE_STR_CONST("panel"):
					return widgetTypes::PANEL;
					break;
				case FE_STR_CONST("button"):
					return widgetTypes::BUTTON;
					break;
				default:
					break;
			}
		return widgetTypes::NONE;
	}

widgetTransformEnums::anchor getAnchorFromString(fe::str string)
	{
		switch (string)
			{
				case FE_STR_CONST("top left"):
					return widgetTransformEnums::anchor::TOP_LEFT;
					break;
				case FE_STR_CONST("top middle"):
					return widgetTransformEnums::anchor::TOP_MIDDLE;
					break;
				case FE_STR_CONST("top right"):
					return widgetTransformEnums::anchor::TOP_RIGHT;
					break;
				case FE_STR_CONST("middle left"):
					return widgetTransformEnums::anchor::MIDDLE_LEFT;
					break;
				case FE_STR_CONST("middle"):
					return widgetTransformEnums::anchor::MIDDLE;
					break;
				case FE_STR_CONST("middle right"):
					return widgetTransformEnums::anchor::MIDDLE_RIGHT;
					break;
				case FE_STR_CONST("bottom left"):
					return widgetTransformEnums::anchor::BOTTOM_LEFT;
					break;
				case FE_STR_CONST("bottom middle"):
					return widgetTransformEnums::anchor::BOTTOM_MIDDLE;
					break;
				case FE_STR_CONST("bottom right"):
					return widgetTransformEnums::anchor::BOTTOM_RIGHT;
					break;
				default:
					break;
			}
		return widgetTransformEnums::anchor::MIDDLE;
	}

widgetTransformEnums::type getTransformTypeFromString(fe::str string)
	{
		switch (string)
			{
				case FE_STR_CONST("absolute"):
					return widgetTransformEnums::type::ABSOLUTE;
					break;
				case FE_STR_CONST("percent"):
					return widgetTransformEnums::type::PERCENT;
					break;
				default:
					break;
			}
		return widgetTransformEnums::type::ABSOLUTE;
	}

void processKey(serialisedWidget &widget, fe::str key, const std::string &data)
	{
		switch (key)
			{
				case FE_STR_CONST("name"):
					widget.name = FE_STR(data.c_str());
					break;
				case FE_STR_CONST("parent"):
					{
						std::string parentPath = data;
						while (!parentPath.empty())
							{
								int breakPoint = parentPath.find_first_of('/');
								std::string parent = parentPath.substr(0, breakPoint);
								widget.parents.push(FE_STR(parent.c_str()));

								if (breakPoint == -1)
									{
										break;
									}
								parentPath.erase(parentPath.begin(), parentPath.begin() + breakPoint + 1);
							}

						if (!widget.parents.empty())
							{
								widget.immediateParent = widget.parents.back();
							}
					}
					break;
				case FE_STR_CONST("type"):
					widget.widgetType = getWidgetTypeFromString(FE_STR(data.c_str()));
					break;
				case FE_STR_CONST("position"):
					{
						int commaPos = data.find_first_of(',', 0);

						std::string px = data.substr(data.find_first_of('(', 0) + 1, commaPos - 1);
						std::string py = data.substr(commaPos + 1, data.find_first_of(')', 0) - commaPos - 1);

						px.erase(std::remove_if(px.begin(), px.end(), [] (char c) { return std::isspace(c); }), px.end());
						py.erase(std::remove_if(py.begin(), py.end(), [] (char c) { return std::isspace(c); }), py.end());

						std::from_chars(px.data(), px.data() + px.size(), widget.position.x);
						std::from_chars(py.data(), py.data() + py.size(), widget.position.y);
					}
					break;
				case FE_STR_CONST("position type"):
					widget.positionType = getTransformTypeFromString(FE_STR(data.c_str()));
					break;
				case FE_STR_CONST("anchor"):
					widget.anchor = getAnchorFromString(FE_STR(data.c_str()));
					break;
				case FE_STR_CONST("size"):
					{
						int commaPos = data.find_first_of(',', 0);

						std::string px = data.substr(data.find_first_of('(', 0) + 1, commaPos - 1);
						std::string py = data.substr(commaPos + 1, data.find_first_of(')', 0) - commaPos - 1);

						px.erase(std::remove_if(px.begin(), px.end(), [] (char c) { return std::isspace(c); }), px.end());
						py.erase(std::remove_if(py.begin(), py.end(), [] (char c) { return std::isspace(c); }), py.end());

						std::from_chars(px.data(), px.data() + px.size(), widget.size.x);
						std::from_chars(py.data(), py.data() + py.size(), widget.size.y);
					}
					break;
				case FE_STR_CONST("size type"):
					widget.sizeType = getTransformTypeFromString(FE_STR(data.c_str()));
					break;
				case FE_STR_CONST("on mouse enter"):
					widget.onMouseEnterEvent = FE_STR(data.c_str());
					break;
				case FE_STR_CONST("on mouse leave"):
					widget.onMouseLeaveEvent = FE_STR(data.c_str());
					break;
				case FE_STR_CONST("on click"):
					widget.onClickEvent = FE_STR(data.c_str());
					break;
				case FE_STR_CONST("on click start"):
					widget.onClickStartEvent = FE_STR(data.c_str());
					break;
				case FE_STR_CONST("on click end"):
					widget.onClickEndEvent = FE_STR(data.c_str());
					break;
				case FE_STR_CONST("on double click"):
					widget.onDoubleClickEvent = FE_STR(data.c_str());
					break;
				case FE_STR_CONST("on draw"):
					widget.onDrawEvent = FE_STR(data.c_str());
					break;
				case FE_STR_CONST("allow mouse interaction"):
					{
						if (data.empty())
							{
								widget.noMouseInteraction = false;
							}
						else
							{
								const char *trueString = "false";
								int index = 0;
								widget.noMouseInteraction = true;
								for (auto c : data)
									{
										if (std::tolower(c) != trueString[index++])
											{
												widget.noMouseInteraction = false;
												break;
											}
									}
							}
					}
					break;
				case FE_STR_CONST("texture"):
					widget.texture = FE_STR(data.c_str());
					break;
				default:
					break;
			}
	}

void widgetSerializer::saveToFile(widgetManager &widgets, const char *file)
	{
		
	}

void widgetSerializer::loadFromFile(widgetManager &widgets, const char *file)
	{
		/*
			First pass: load in CSV and parse first row for all keys. Put into array for ordering
			Second pass: For all subsequent rows, parse each column value and store in a serialised widget. Each row corresponds to one widget
			Third pass: For each serialised widget, create relevant widgets within widgetManager
		*/
		robin_hood::unordered_map<fe::str, serialisedWidget> serialisedWidgets;

		std::FILE *csv = std::fopen(file, "r");
		if (!csv)
			{
				spdlog::error("Failed to open CSV file {}", file);
				return;
			}


		// Step 1: Read all column keys
		std::vector<fe::str> columnKeys;
		std::string currentKey;
		int c;
		while ((c = std::fgetc(csv)) != EOF)
			{
#ifdef _DEBUG
				if (std::ferror(csv) != 0)
					{
						spdlog::error("Error reading CSV file {}", std::ferror(csv));
					}
#endif
				if (c == ',' || c == '\n')
					{
						columnKeys.push_back(FE_STR(currentKey.c_str()));

						currentKey = "";

						if (c == '\n')
							{
								break;
							}
					}
				else if (c <= 127)
					{
						currentKey += c;
					}
			}

		if (c == EOF)
			{
				spdlog::error("EOF reached before first row completed in file {}", file);
				std::fclose(csv);
				return;
			}

		// Step 2: Read all subsequent rows
		serialisedWidget workingWidget;
		currentKey = "";
		int currentKeyIndex = 0;
		bool inQuote = false;

		while ((c = std::fgetc(csv)) != EOF)
			{
#ifdef _DEBUG
				if (std::ferror(csv) != 0)
					{
						spdlog::error("Error reading CSV file {}", std::ferror(csv));
					}
#endif
				if ((!inQuote && c == ',') || c == '\n')
					{
						processKey(workingWidget, columnKeys[currentKeyIndex], currentKey);

						currentKeyIndex += 1;
						if (c == '\n')
							{
								serialisedWidgets[workingWidget.name] = workingWidget;
								workingWidget = serialisedWidget{};
								currentKeyIndex = 0;
							}

						currentKey = "";
					}
				else if (c == '"')
					{
						inQuote = !inQuote;
					}
				else if (c <= 127)
					{
						currentKey += c;
					}
			}

		std::fclose(csv);

		// Step 3: Process all widgets
		robin_hood::unordered_map<fe::str, widgetGraph::node*> processedWidgets;

		widgetGraph &graph = widgets.m_widgetGraph;
		plf::colony<widgetManager::widgetState> &managerWidgets = widgets.m_widgets;

		auto processWidget = [&widgets, &graph, &processedWidgets, &managerWidgets] (serialisedWidget& serialisedWidget) {
			// If we havent processed this widget yet
			if (processedWidgets.find(serialisedWidget.name) == processedWidgets.end())
				{
					// we can guarantee that the parent is processed because of the widget parent queue
					widgetManager::widgetState &newWidget = *managerWidgets.emplace();

					// copy serialised data
					newWidget.widget.noMouseInteraction = serialisedWidget.noMouseInteraction;
					newWidget.widget.onClickEndEvent = serialisedWidget.onClickEndEvent;
					newWidget.widget.onClickEvent = serialisedWidget.onClickEvent;
					newWidget.widget.onClickStartEvent = serialisedWidget.onClickStartEvent;
					newWidget.widget.onDoubleClickEvent = serialisedWidget.onDoubleClickEvent;
					newWidget.widget.onDrawEvent = serialisedWidget.onDrawEvent;
					newWidget.widget.onMouseEnterEvent = serialisedWidget.onMouseEnterEvent;
					newWidget.widget.onMouseLeaveEvent = serialisedWidget.onMouseLeaveEvent;
					newWidget.widget.texture = nineBox("9box.png", 8);
					newWidget.widget.transform.setAnchor(serialisedWidget.anchor);
					newWidget.widget.transform.setPosition(serialisedWidget.position, serialisedWidget.positionType);
					newWidget.widget.transform.setSize(serialisedWidget.size, serialisedWidget.sizeType);
					newWidget.widget.type = serialisedWidget.widgetType;

					widgetGraph::node &widgetNode = graph.addWidget(newWidget.widget, &newWidget);
					if (serialisedWidget.immediateParent != 0 && processedWidgets.find(serialisedWidget.immediateParent) != processedWidgets.end())
						{
							graph.addChild(*processedWidgets.at(serialisedWidget.immediateParent), widgetNode);
						}
					else if (serialisedWidget.immediateParent != 0 && processedWidgets.find(serialisedWidget.immediateParent) == processedWidgets.end())
						{
							spdlog::error("Attempted to process widget with a parent unprocessed!");
						}

					processedWidgets[serialisedWidget.name] = &widgetNode;
				}
		};

		for (auto &widget : serialisedWidgets)
			{
				// We process widgets from the first parent -> the last parent, and finally the working widget
				// If the widget has already been processed, ignore it.
				if (processedWidgets.find(widget.first) == processedWidgets.end())
					{
						while (!widget.second.parents.empty())
							{
								fe::str working = widget.second.parents.front();
								if (serialisedWidgets.find(working) != serialisedWidgets.end()) 
									{
										processWidget(serialisedWidgets.at(working));
									}
								widget.second.parents.pop();
							}

						processWidget(serialisedWidgets.at(widget.first));
					}
			}
	}
