/*************************************************************************/
/*  property_editor.h                                                    */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2021 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2021 Godot Engine contributors (cf. AUTHORS.md).   */
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

#ifndef PROPERTY_EDITOR_H
#define PROPERTY_EDITOR_H

#include "scene/gui/container.h"

class PropertyEditor : public Container {
	GDCLASS(PropertyEditor, Container);

private:
	String label;
	int text_size;
	friend class Inspector;
	Object *object;
	StringName property;

	int property_usage;

	bool read_only;
	bool checkable;
	bool checked;
	bool draw_red;
	bool keying;
	bool deletable;

	Rect2 right_child_rect;
	Rect2 bottom_child_rect;

	Rect2 keying_rect;
	bool keying_hover = false;
	Rect2 revert_rect;
	bool revert_hover = false;
	Rect2 check_rect;
	bool check_hover = false;
	Rect2 delete_rect;
	bool delete_hover = false;

	bool can_revert;

	bool use_folding;
	bool draw_top_bg;

	bool _is_property_different(const Variant &p_current, const Variant &p_orig);
	bool _get_instanced_node_original_property(const StringName &p_prop, Variant &value);
	void _focusable_focused(int p_index);

	bool selectable;
	bool selected;
	int selected_focusable;

	float split_ratio;
	float app_scale; // NEW

	Vector<Control *> focusables;
	Control *label_reference;
	Control *bottom_editor;

	mutable String tooltip_text;

	// NEW
	Ref<Texture> icon_key;
	Ref<Texture> icon_close;
	Ref<Texture> icon_checked;

protected:
	void _notification(int p_what);
	static void _bind_methods();

	void _gui_input(const Ref<InputEvent> &p_event);

public:
	void emit_changed(const StringName &p_property, const Variant &p_value, const StringName &p_field = StringName(), bool p_changing = false);

	virtual Size2 get_minimum_size() const override;

	void set_label(const String &p_label);
	String get_label() const;

	void set_read_only(bool p_read_only);
	bool is_read_only() const;

	Object *get_edited_object();
	StringName get_edited_property();

	virtual void update_property();
	void update_reload_status();

	virtual bool use_keying_next() const;

	void set_checkable(bool p_checkable);
	bool is_checkable() const;

	void set_checked(bool p_checked);
	bool is_checked() const;

	void set_draw_red(bool p_draw_red);
	bool is_draw_red() const;

	void set_keying(bool p_keying);
	bool is_keying() const;

	void set_deletable(bool p_enable);
	bool is_deletable() const;
	void add_focusable(Control *p_control);
	void select(int p_focusable = -1);
	void deselect();
	bool is_selected() const;

	void set_label_reference(Control *p_control);
	void set_bottom_editor(Control *p_control);

	void set_use_folding(bool p_use_folding);
	bool is_using_folding() const;

	virtual void expand_all_folding();
	virtual void collapse_all_folding();

	virtual Variant get_drag_data(const Point2 &p_point) override;

	void set_selectable(bool p_selectable);
	bool is_selectable() const;

	void set_name_split_ratio(float p_ratio);
	float get_name_split_ratio() const;

	void set_app_scale(float p_ratio);
	float get_app_scale() const;

	void set_object_and_property(Object *p_object, const StringName &p_property);
	virtual Control *make_custom_tooltip(const String &p_text) const override;

	String get_tooltip_text() const;

	void set_draw_top_bg(bool p_draw) { draw_top_bg = p_draw; }

	bool can_revert_to_default() const { return can_revert; }

	PropertyEditor();
};

#endif // PROPERTY_EDITOR_H
