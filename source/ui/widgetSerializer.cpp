#include "widgetSerializer.hpp"
#include "widgetManager.hpp"
#include "str.hpp"
#include "widgetTransformEnums.hpp"
#include "widgetTypes.hpp"
#include "font/fontSizeType.hpp"
#include "text.hpp"
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

        widgetTransformEnums::anchor anchor = widgetTransformEnums::anchor::MIDDLE;
        widgetTransformEnums::type sizeType = widgetTransformEnums::type::ABSOLUTE;
        widgetTransformEnums::type positionType = widgetTransformEnums::type::ABSOLUTE;

        glm::vec2 size = { 0, 0 };
        glm::vec2 position = { 0, 0 };

        std::string fontID;
        std::string text;
        glm::vec2 textPosition = { 0, 0 };
        widgetTransformEnums::anchor textAnchor = widgetTransformEnums::anchor::MIDDLE;
        widgetTransformEnums::type textPositionType = widgetTransformEnums::type::ABSOLUTE;
        float fontSize = 48.f;
        fontSizeType fontSizeType = fontSizeType::PIXEL;

        std::string onMouseEnterEvent;
        std::string onMouseLeaveEvent;
        std::string onClickEvent;
        std::string onClickStartEvent;
        std::string onClickEndEvent;
        std::string onDoubleClickEvent;
        std::string onDrawEvent;

        bool noMouseInteraction = false;

        std::string texture;

        std::queue<std::string> parents;
        std::string immediateParent;
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

std::string getStringFromWidgetType(widgetTypes type)
    {
        switch (type)
            {
                case widgetTypes::PANEL:
                    return "panel";
                case widgetTypes::BUTTON:
                    return "button";
                default:
                    break;
            }
        return "";
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

std::string getStringFromAnchor(widgetTransformEnums::anchor anchor)
    {
        switch (anchor)
            {
                case widgetTransformEnums::anchor::TOP_LEFT:
                    return "top left";
                case widgetTransformEnums::anchor::TOP_MIDDLE:
                    return "top middle";
                case widgetTransformEnums::anchor::TOP_RIGHT:
                    return "top right";
                case widgetTransformEnums::anchor::MIDDLE_LEFT:
                    return "middle left";
                case widgetTransformEnums::anchor::MIDDLE:
                    return "middle";
                case widgetTransformEnums::anchor::MIDDLE_RIGHT:
                    return "middle right";
                case widgetTransformEnums::anchor::BOTTOM_LEFT:
                    return "bottom left";
                case widgetTransformEnums::anchor::BOTTOM_MIDDLE:
                    return "bottom middle";
                case widgetTransformEnums::anchor::BOTTOM_RIGHT:
                    return "bottom right";
                default:
                    break;
            }
        return "middle";
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

std::string getStringFromTransformType(widgetTransformEnums::type type)
    {
        switch (type)
            {
                case widgetTransformEnums::type::ABSOLUTE:
                    return "absolute";
                case widgetTransformEnums::type::PERCENT:
                    return "percent";
                default:
                    break;
            }
        return "absolute";
    }

fontSizeType getFontSizeTypeFromString(fe::str string)
    {
        switch (string)
            {
                case FE_STR_CONST("px"):
                    return fontSizeType::PIXEL;
                    break;
                case FE_STR_CONST("pt"):
                    return fontSizeType::POINT;
                    break;
                default:
                    break;
            }
        return fontSizeType::PIXEL;
    }

std::string getStringFromFontSizeType(fontSizeType type)
    {
        switch (type)
            {
                case fontSizeType::PIXEL:
                    return "px";
                    break;
                case fontSizeType::POINT:
                    return "pt";
                    break;
                default:
                    break;
            }
        return "px";
    }

glm::vec2 getVectorFromString(const std::string &str)
    {
        int commaPos = str.find_first_of(',', 0);

        std::string px = str.substr(str.find_first_of('(', 0) + 1, commaPos - 1);
        std::string py = str.substr(commaPos + 1, str.find_first_of(')', 0) - commaPos - 1);

        px.erase(std::remove_if(px.begin(), px.end(), [] (char c) { return std::isspace(c); }), px.end());
        py.erase(std::remove_if(py.begin(), py.end(), [] (char c) { return std::isspace(c); }), py.end());

        glm::vec2 returnVector = {};

        std::from_chars(px.data(), px.data() + px.size(), returnVector.x);
        std::from_chars(py.data(), py.data() + py.size(), returnVector.y);

        return returnVector;
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
                    widget.position = getVectorFromString(data);
                    break;
                case FE_STR_CONST("position type"):
                    widget.positionType = getTransformTypeFromString(FE_STR(data.c_str()));
                    break;
                case FE_STR_CONST("anchor"):
                    widget.anchor = getAnchorFromString(FE_STR(data.c_str()));
                    break;
                case FE_STR_CONST("size"):
                    widget.size = getVectorFromString(data);
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
                case FE_STR_CONST("font"):
                    widget.fontID = data;
                    break;
                case FE_STR_CONST("text"):
                    widget.text = data;
                    break;
                case FE_STR_CONST("textSize"):
                    std::from_chars(data.data(), data.data() + data.size(), widget.fontSize);
                    break;
                case FE_STR_CONST("textSizeType"):
                    widget.fontSizeType = getFontSizeTypeFromString(FE_STR(data.c_str()));
                    break;
                case FE_STR_CONST("textPosition"):
                    widget.textPosition = getVectorFromString(data);
                    break;
                case FE_STR_CONST("textPositionType"):
                    widget.textPositionType = getTransformTypeFromString(FE_STR(data.c_str()));
                    break;
                case FE_STR_CONST("textPositionAnchor"):
                    widget.textAnchor = getAnchorFromString(FE_STR(data.c_str()));
                    break;
                default:
                    break;
            }
    }

void widgetSerializer::saveToFile(widgetManager &widgets, const char *file)
    {
        /*
            Perform BFS on every root node. For each node, serialise to a widget. For each serialised widget, write to disk
            Step 1: Get all widgets serialised
            Step 2: Print to disk
        */
        std::vector<serialisedWidget> serialisedWidgets;
        for (auto &root : widgets.m_widgetGraph.roots)
            {
                std::queue<std::string> currentParentPath;
                currentParentPath.push("");

                std::queue<const widgetGraph::node*> toProcess;
                toProcess.push(root);

                while (!toProcess.empty())
                    {
                        const widgetGraph::node *working = toProcess.front();
                        toProcess.pop();

                        std::string parentPath = currentParentPath.front();
                        currentParentPath.pop();

                        for (auto &child : working->children)
                            {
                                if (!parentPath.empty())
                                    {
                                        currentParentPath.push(fmt::format("{}/{}", parentPath, working->widget->name));
                                    }
                                else
                                    {
                                        currentParentPath.push(fmt::format("{}", working->widget->name));
                                    }
                                toProcess.push(child);
                            }

                        serialisedWidget serialisedWidget;

                        serialisedWidget.name =                 working->widget->name;
                        serialisedWidget.widgetType =           working->widget->type;
                        serialisedWidget.anchor =               working->widget->transform.anchor;
                        serialisedWidget.sizeType =             working->widget->transform.sizeType;
                        serialisedWidget.positionType =         working->widget->transform.positionType;
                        serialisedWidget.size =                 working->widget->transform.getStoredSize();
                        serialisedWidget.position =             working->widget->transform.getStoredPosition();
                        serialisedWidget.onMouseEnterEvent =    working->widget->onMouseEnterEvent;
                        serialisedWidget.onMouseLeaveEvent =    working->widget->onMouseLeaveEvent;
                        serialisedWidget.onClickEvent =         working->widget->onClickEvent;
                        serialisedWidget.onClickStartEvent =    working->widget->onClickStartEvent;
                        serialisedWidget.onClickEndEvent =      working->widget->onClickEndEvent;
                        serialisedWidget.onDoubleClickEvent =   working->widget->onDoubleClickEvent;
                        serialisedWidget.onDrawEvent =          working->widget->onDrawEvent;
                        serialisedWidget.noMouseInteraction =   working->widget->noMouseInteraction;
                        serialisedWidget.texture =              "";
                        serialisedWidget.text =                 working->widget->text.string;
                        serialisedWidget.fontID =               working->widget->text.m_font.id();
                        serialisedWidget.fontSize =             working->widget->text.m_font.font().size;
                        serialisedWidget.fontSizeType =         working->widget->text.m_font.font().sizeType;
                        serialisedWidget.textAnchor =           working->widget->textTransform.anchor;
                        serialisedWidget.textPosition =         working->widget->textTransform.getStoredPosition();
                        serialisedWidget.textPositionType =     working->widget->textTransform.positionType;


                        // Combine parents into string
                        serialisedWidget.immediateParent = parentPath; // bastardising immediate parent to use the whole path: its easier to construct this way

                        serialisedWidgets.push_back(std::move(serialisedWidget));
                    }
            }

        std::FILE *csv = std::fopen(file, "w");
        if (!csv)
            {
                spdlog::error("Failed to open CSV file {}", file);
                return;
            }

        std::fputs("name,parent,type,position,position type,anchor,size,size type,on mouse enter,on mouse leave,on click,on click start,on click end,on double click,on draw,allow mouse interaction,texture,font,text,textSize,textSizeType,textPosition,textPositionType,textPositionAnchor\n", csv);
        for (auto &widget : serialisedWidgets)
            {
                std::fprintf(csv, "%s,%s,%s,\"(%f, %f)\",%s,%s,\"(%f, %f)\",%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%f,%s,(%f, %f),%s,%s\n",
                             widget.name.c_str(),
                             widget.immediateParent.c_str(),
                             getStringFromWidgetType(widget.widgetType).c_str(),
                             widget.position.x, widget.position.y,
                             getStringFromTransformType(widget.positionType).c_str(),
                             getStringFromAnchor(widget.anchor).c_str(),
                             widget.size.x, widget.size.y,
                             getStringFromTransformType(widget.sizeType).c_str(),
                             widget.onMouseEnterEvent.c_str(),
                             widget.onMouseLeaveEvent.c_str(),
                             widget.onClickEvent.c_str(),
                             widget.onClickStartEvent.c_str(),
                             widget.onClickEndEvent.c_str(),
                             widget.onDoubleClickEvent.c_str(),
                             widget.onDrawEvent.c_str(),
                             widget.noMouseInteraction ? "TRUE" : "FALSE",
                             widget.texture.c_str(),
                             widget.fontID,
                             widget.text.c_str(),
                             widget.fontSize,
                             getStringFromFontSizeType(widget.fontSizeType).c_str(),
                             widget.textPosition.x, widget.textPosition.y,
                             getStringFromTransformType(widget.textPositionType).c_str(),
                             getStringFromAnchor(widget.textAnchor).c_str()
                );
            }

        std::fclose(csv);
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
                    newWidget.widget.name =                     serialisedWidget.name;
                    newWidget.widget.noMouseInteraction =       serialisedWidget.noMouseInteraction;
                    newWidget.widget.type =                     serialisedWidget.widgetType;
                    newWidget.widget.texture =                  nineBox("9box.png", 8);

                    // events
                    newWidget.widget.onClickEvent =             serialisedWidget.onClickEvent;
                    newWidget.widget.onClickEndEvent =          serialisedWidget.onClickEndEvent;
                    newWidget.widget.onClickStartEvent =        serialisedWidget.onClickStartEvent;
                    newWidget.widget.onDoubleClickEvent =       serialisedWidget.onDoubleClickEvent;
                    newWidget.widget.onDrawEvent =              serialisedWidget.onDrawEvent;
                    newWidget.widget.onMouseEnterEvent =        serialisedWidget.onMouseEnterEvent;
                    newWidget.widget.onMouseLeaveEvent =        serialisedWidget.onMouseLeaveEvent;

                    // transform
                    newWidget.widget.transform.setAnchor(serialisedWidget.anchor);
                    newWidget.widget.transform.setPosition(serialisedWidget.position, serialisedWidget.positionType);
                    newWidget.widget.transform.setSize(serialisedWidget.size, serialisedWidget.sizeType);

                    // text
                    if (!serialisedWidget.fontID.empty()) 
                        {
                            newWidget.widget.text = text(serialisedWidget.text.c_str(), widgets.m_fonts.get(serialisedWidget.fontID.c_str()));
                            newWidget.widget.textTransform.setAnchor(serialisedWidget.anchor);
                            newWidget.widget.textTransform.setPosition(serialisedWidget.textPosition, serialisedWidget.textPositionType);
                            newWidget.widget.hasText = true;
                        }

                    // done

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
