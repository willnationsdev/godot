/*************************************************************************/
/*  markdown_preview.h                                                   */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2019 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2019 Godot Engine contributors (cf. AUTHORS.md)    */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/

#include "modules/regex/regex.h"
#include "scene/gui/rich_text_label.h"
#include "scene/gui/text_edit.h"

#ifndef MARKDOWN_PREVIEW_H
#define MARKDOWN_PREVIEW_H

// Reference: https://github.com/progsource/maddy/tree/master/include/maddy

class MDLineParser {
	virtual void parse(String &r_line) = 0;
	virtual ~MDLineParser() {}
};

// Has to be used before the `EmphasizedParser`.
class MDStrongParser : public MDLineParser {
public:
	void parse(String &r_line) override {
		Ref<RegEx> reg1 = memnew(RegEx);
		ERR_FAIL_COND(reg1->compile("(?!.*`.*|.*<code>.*)\\*\\*(?!.*`.*|.*<\\/code>.*)([^\\*\\*]*)\\*\\*(?!.*`.*|.*<\\/code>.*)") == OK);
		Ref<RegEx> reg2 = memnew(RegEx);
		ERR_FAIL_COND(reg2->compile("(?!.*`.*|.*<code>.*)__(?!.*`.*|.*<\\/code>.*)([^__]*)__(?!.*`.*|.*<\\/code>.*)") == OK);

		String rep = "<strong>$1</strong>";
		reg1->sub(r_line, rep, true);
		reg2->sub(r_line, rep, true);
	}
};

class MarkdownPreview : public RichTextLabel {

	GDCLASS(MarkdownPreview, RichTextLabel);

private:
	TextEdit *editor = NULL;

	void _update();

protected:
	static void _bind_methods();

public:
	void set_editor(TextEdit *p_editor);
	MarkdownPreview();
};

#endif // MARKDOWN_PREVIEW_H
