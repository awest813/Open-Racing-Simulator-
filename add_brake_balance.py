#!/usr/bin/env python3
import re

with open('src/drivers/human/human.cpp', 'r') as f:
    content = f.read()

# Find the line after ABS/ASR message
pattern = r'(\s+const int bufsize = sizeof\(car->_msgCmd\[0\]\);\s+snprintf\(car->_msgCmd\[0\], bufsize, "%s %s", \(HCtx\[idx\]->ParamAbs \? "ABS" : ""\), \(HCtx\[idx\]->ParamAsr \? "ASR" : ""\)\);\s+memcpy\(car->_msgColorCmd, color, sizeof\(car->_msgColorCmd\)\);\s+)'

match = re.search(pattern, content, re.DOTALL)
if match:
    # Insert brake balance code after the message line
    insert_point = match.end()
    before = content[:insert_point]
    after = content[insert_point:]
    
    brake_balance_code = '''
	// Brake balance adjustment
	if (((cmd[CMD_BRAKE_BALANCE_FWD].type == GFCTRL_TYPE_KEYBOARD) && keyInfo[cmd[CMD_BRAKE_BALANCE_FWD].val].edgeUp) ||
		((cmd[CMD_BRAKE_BALANCE_FWD].type == GFCTRL_TYPE_SKEYBOARD) && skeyInfo[cmd[CMD_BRAKE_BALANCE_FWD].val].edgeUp) ||
		((cmd[CMD_BRAKE_BALANCE_FWD].type == GFCTRL_TYPE_JOY_BUT) && joyInfo->edgeup[cmd[CMD_BRAKE_BALANCE_FWD].val])) {
		HCtx[idx]->brakeBalance += 0.01f;
		if (HCtx[idx]->brakeBalance > 0.7f) HCtx[idx]->brakeBalance = 0.7f;
		snprintf(car->_msgCmd[2], bufsize, "Brake Bias: %.0f%%", HCtx[idx]->brakeBalance * 100.0f);
	}

	if (((cmd[CMD_BRAKE_BALANCE_REAR].type == GFCTRL_TYPE_KEYBOARD) && keyInfo[cmd[CMD_BRAKE_BALANCE_REAR].val].edgeUp) ||
		((cmd[CMD_BRAKE_BALANCE_REAR].type == GFCTRL_TYPE_SKEYBOARD) && skeyInfo[cmd[CMD_BRAKE_BALANCE_REAR].val].edgeUp) ||
		((cmd[CMD_BRAKE_BALANCE_REAR].type == GFCTRL_TYPE_JOY_BUT) && joyInfo->edgeup[cmd[CMD_BRAKE_BALANCE_REAR].val])) {
		HCtx[idx]->brakeBalance -= 0.01f;
		if (HCtx[idx]->brakeBalance 