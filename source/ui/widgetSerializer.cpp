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
		std::string name;

		widgetTypes widgetType = widgetTypes::NONE;
		std::queue<std::string> parents;
		std::string immediateParent;

		widgetTransformEnums::anchor anchor = widgetTransformEnums::anchor::MIDDLE;
		widgetTransformEnums::type sizeType = widgetTransformEnums::type::ABSOLUTE;
		widgetTransformEnums::type positionType = widgetTransformEnums::type::ABSOLUTE;

		glm::vec2 size = { 0, 0 };
		glm::vec2 position = { 0, 0 };

		std::string onMouseEnterEvent;
		std::string onMouseLeaveEvent;
		std::string onClickEvent;
		std::string onClickStartEvent;
		std::string onClickEndEvent;
		std::string onDoubleClickEvent;
		std::string onDrawEvent;

		bool noMouseInteraction = false;

		std::string texture;
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
					widget.name = data;
					break;
				case FE_STR_CONST("parent"):
					{
						std::string parentPath = data;
						while (!parentPath.empty())
							{
								int breakPoint = parentPath.find_first_of('/');
								std::string parent = parentPath.substr(0, breakPoint);
								widget.parents.push(parent);

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
					widget.onMouseEnterEvent = data;
					break;
				case FE_STR_CONST("on mouse leave"):
					widget.onMouseLeaveEvent = data;
					break;
				case FE_STR_CONST("on click"):
					widget.onClickEvent = data;
					break;
				case FE_STR_CONST("on click start"):
					widget.onClickStartEvent = data;
					break;
				case FE_STR_CONST("on click end"):
					widget.onClickEndEvent = data;
					break;
				case FE_STR_CONST("on double click"):
					widget.onDoubleClickEvent = data;
					break;
				case FE_STR_CONST("on draw"):
					widget.onDrawEvent = data;
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
					widget.texture = data;
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
		robin_hood::unordered_map<std::string, serialisedWidget> serialisedWidgets;

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
		robin_hood::unordered_map<std::string, widgetGraph::node*> processedWidgets;

		widgetGraph &graph = widgets.m_widgetGraph;
		plf::colony<widgetManager::widgetState> &managerWidgets = widgets.m_widgets;

		auto processWidget = [&widgets, &graph, &processedWidgets, &managerWidgets] (serialisedWidget& serialisedWidget) {
			// If we havent processed this widget yet
			if (processedWidgets.find(serialisedWidget.name) == processedWidgets.end())
				{
					// we can guarantee that the parent is processed because of the widget parent queue
					widgetManager::widgetState &newWidget = *managerWidgets.emplace();

					// copy serialised data
					newWidget.widget.noMouseInteraction =	serialisedWidget.noMouseInteraction;
					newWidget.widget.onClickEndEvent =		FE_STR(serialisedWidget.onClickEndEvent.c_str());
					newWidget.widget.onClickEvent =			FE_STR(serialisedWidget.onClickEvent.c_str());
					newWidget.widget.onClickStartEvent =	FE_STR(serialisedWidget.onClickStartEvent.c_str());
					newWidget.widget.onDoubleClickEvent =	FE_STR(serialisedWidget.onDoubleClickEvent.c_str());
					newWidget.widget.onDrawEvent =			FE_STR(serialisedWidget.onDrawEvent.c_str());
					newWidget.widget.onMouseEnterEvent =	FE_STR(serialisedWidget.onMouseEnterEvent.c_str());
					newWidget.widget.onMouseLeaveEvent =	FE_STR(serialisedWidget.onMouseLeaveEvent.c_str());
					newWidget.widget.texture = nineBox("9box.png", 8);
					newWidget.widget.transform.setAnchor(serialisedWidget.anchor);
					newWidget.widget.transform.setPosition(serialisedWidget.position, serialisedWidget.positionType);
					newWidget.widget.transform.setSize(serialisedWidget.size, serialisedWidget.sizeType);
					newWidget.widget.type = serialisedWidget.widgetType;

					widgetGraph::node &widgetNode = graph.addWidget(newWidget.widget, &newWidget);
					if (serialisedWidget.immediateParent != "" && processedWidgets.find(serialisedWidget.immediateParent) != processedWidgets.end())
						{
							graph.addChild(*processedWidgets.at(serialisedWidget.immediateParent), widgetNode);
						}
					else if (serialisedWidget.immediateParent != "" && processedWidgets.find(serialisedWidget.immediateParent) == processedWidgets.end())
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
								const std::string &working = widget.second.parents.front();
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
