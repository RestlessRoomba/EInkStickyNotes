
#include "tool.h"


namespace {
Tool activeTool = Tool::Pen;
}

void changeTool(Tool tool)
{
	activeTool = tool;
}

Tool currentTool()
{
	return activeTool;
}
