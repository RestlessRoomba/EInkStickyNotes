#pragma once

// Tools that can be selected by changeTool().
enum class Tool {
    Pen,
    Text,
    Line,
    Rectangle,
    Circle
};

void changeTool(Tool tool);
Tool currentTool();