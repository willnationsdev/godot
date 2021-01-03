
#include "property_editor.h"

Size2 PropertyEditor::get_minimum_size() const {
	Size2 ms;
	Ref<Font> font = get_theme_font("font", "Tree");
	int font_size = get_theme_font_size("font_size", "Tree");
	ms.height = font->get_height(font_size);

	for (int i = 0; i < get_child_count(); i++) {
		Control *c = Object::cast_to<Control>(get_child(i));
		if (!c) {
			continue;
		}
		if (c->is_set_as_top_level()) {
			continue;
		}
		if (!c->is_visible()) {
			continue;
		}
		if (c == bottom_editor) {
			continue;
		}

		Size2 minsize = c->get_combined_minimum_size();
		ms.width = MAX(ms.width, minsize.width);
		ms.height = MAX(ms.height, minsize.height);
	}

	if (keying) {
		Ref<Texture2D> key = get_theme_icon("Key", "EditorIcons");
		ms.width += key->get_width() + get_theme_constant("hseparator", "Tree");
	}

	if (deletable) {
		Ref<Texture2D> key = get_theme_icon("Close", "EditorIcons");
		ms.width += key->get_width() + get_theme_constant("hseparator", "Tree");
	}

	if (checkable) {
		Ref<Texture2D> check = get_theme_icon("checked", "CheckBox");
		ms.width += check->get_width() + get_theme_constant("hseparation", "CheckBox") + get_theme_constant("hseparator", "Tree");
	}

	if (bottom_editor != nullptr && bottom_editor->is_visible()) {
		ms.height += get_theme_constant("vseparation", "Tree");
		Size2 bems = bottom_editor->get_combined_minimum_size();
		//bems.width += get_constant("item_margin", "Tree");
		ms.height += bems.height;
		ms.width = MAX(ms.width, bems.width);
	}

	return ms;
}

void PropertyEditor::emit_changed(const StringName &p_property, const Variant &p_value, const StringName &p_field, bool p_changing) {
	Variant args[4] = { p_property, p_value, p_field, p_changing };
	const Variant *argptrs[4] = { &args[0], &args[1], &args[2], &args[3] };

	emit_signal("property_changed", (const Variant **)argptrs, 4);
}

void PropertyEditor::_notification(int p_what) {
	if (p_what == NOTIFICATION_SORT_CHILDREN) {
		Size2 size = get_size();
		Rect2 rect;
		Rect2 bottom_rect;

		right_child_rect = Rect2();
		bottom_child_rect = Rect2();

		{
			int child_room = size.width * (1.0 - split_ratio);
			Ref<Font> font = get_theme_font("font", "Tree");
			int font_size = get_theme_font_size("font_size", "Tree");
			int height = font->get_height(font_size);
			bool no_children = true;

			//compute room needed
			for (int i = 0; i < get_child_count(); i++) {
				Control *c = Object::cast_to<Control>(get_child(i));
				if (!c) {
					continue;
				}
				if (c->is_set_as_top_level()) {
					continue;
				}
				if (c == bottom_editor) {
					continue;
				}

				Size2 minsize = c->get_combined_minimum_size();
				child_room = MAX(child_room, minsize.width);
				height = MAX(height, minsize.height);
				no_children = false;
			}

			if (no_children) {
				text_size = size.width;
				rect = Rect2(size.width - 1, 0, 1, height);
			} else {
				text_size = MAX(0, size.width - (child_room + 4 * app_scale));
				if (is_layout_rtl()) {
					rect = Rect2(1, 0, child_room, height);
				} else {
					rect = Rect2(size.width - child_room, 0, child_room, height);
				}
			}

			if (bottom_editor) {
				int m = 0; //get_constant("item_margin", "Tree");

				bottom_rect = Rect2(m, rect.size.height + get_theme_constant("vseparation", "Tree"), size.width - m, bottom_editor->get_combined_minimum_size().height);
			}

			if (keying) {
				Ref<Texture2D> key;

				if (use_keying_next()) {
					key = get_theme_icon("KeyNext", "EditorIcons");
				} else {
					key = get_theme_icon("Key", "EditorIcons");
				}

				rect.size.x -= key->get_width() + get_theme_constant("hseparator", "Tree");
				if (is_layout_rtl()) {
					rect.position.x += key->get_width() + get_theme_constant("hseparator", "Tree");
				}

				if (no_children) {
					text_size -= key->get_width() + 4 * app_scale;
				}
			}

			if (deletable) {
				Ref<Texture2D> close;

				close = get_theme_icon("Close", "EditorIcons");

				rect.size.x -= close->get_width() + get_theme_constant("hseparator", "Tree");

				if (is_layout_rtl()) {
					rect.position.x += close->get_width() + get_theme_constant("hseparator", "Tree");
				}

				if (no_children) {
					text_size -= close->get_width() + 4 * app_scale;
				}
			}
		}

		//set children
		for (int i = 0; i < get_child_count(); i++) {
			Control *c = Object::cast_to<Control>(get_child(i));
			if (!c) {
				continue;
			}
			if (c->is_set_as_top_level()) {
				continue;
			}
			if (c == bottom_editor) {
				continue;
			}

			fit_child_in_rect(c, rect);
			right_child_rect = rect;
		}

		if (bottom_editor) {
			fit_child_in_rect(bottom_editor, bottom_rect);
			bottom_child_rect = bottom_rect;
		}

		update(); //need to redraw text
	}

	if (p_what == NOTIFICATION_DRAW) {
		Ref<Font> font = get_theme_font("font", "Tree");
		int font_size = get_theme_font_size("font_size", "Tree");
		Color dark_color = get_theme_color("dark_color_2", "Editor");
		bool rtl = is_layout_rtl();

		Size2 size = get_size();
		if (bottom_editor) {
			size.height = bottom_editor->get_offset(SIDE_TOP);
		} else if (label_reference) {
			size.height = label_reference->get_size().height;
		}

		if (selected) {
			Ref<StyleBox> sb = get_theme_stylebox("selected", "Tree");
			draw_style_box(sb, Rect2(Vector2(), size));
		}

		if (draw_top_bg && right_child_rect != Rect2()) {
			draw_rect(right_child_rect, dark_color);
		}
		if (bottom_child_rect != Rect2()) {
			draw_rect(bottom_child_rect, dark_color);
		}

		Color color;
		if (draw_red) {
			color = get_theme_color("error_color", "Editor");
		} else {
			color = get_theme_color("property_color", "Editor");
		}
		if (label.find(".") != -1) {
			color.a = 0.5; //this should be un-hacked honestly, as it's used for editor overrides
		}

		int ofs = 0;
		int text_limit = text_size;

		if (checkable) {
			Ref<Texture2D> checkbox;
			if (checked) {
				checkbox = get_theme_icon("GuiChecked", "EditorIcons");
			} else {
				checkbox = get_theme_icon("GuiUnchecked", "EditorIcons");
			}

			Color color2(1, 1, 1);
			if (check_hover) {
				color2.r *= 1.2;
				color2.g *= 1.2;
				color2.b *= 1.2;
			}
			check_rect = Rect2(ofs, ((size.height - checkbox->get_height()) / 2), checkbox->get_width(), checkbox->get_height());
			if (rtl) {
				draw_texture(checkbox, Vector2(size.width - check_rect.position.x - checkbox->get_width(), check_rect.position.y), color2);
			} else {
				draw_texture(checkbox, check_rect.position, color2);
			}
			ofs += get_theme_constant("hseparator", "Tree") + checkbox->get_width() + get_theme_constant("hseparation", "CheckBox");
			text_limit -= ofs;
		} else {
			check_rect = Rect2();
		}

		if (can_revert) {
			Ref<Texture2D> reload_icon = get_theme_icon("ReloadSmall", "EditorIcons");
			text_limit -= reload_icon->get_width() + get_theme_constant("hseparator", "Tree") * 2;
			revert_rect = Rect2(text_limit + get_theme_constant("hseparator", "Tree"), (size.height - reload_icon->get_height()) / 2, reload_icon->get_width(), reload_icon->get_height());

			Color color2(1, 1, 1);
			if (revert_hover) {
				color2.r *= 1.2;
				color2.g *= 1.2;
				color2.b *= 1.2;
			}
			if (rtl) {
				draw_texture(reload_icon, Vector2(size.width - revert_rect.position.x - reload_icon->get_width(), revert_rect.position.y), color2);
			} else {
				draw_texture(reload_icon, revert_rect.position, color2);
			}
		} else {
			revert_rect = Rect2();
		}

		int v_ofs = (size.height - font->get_height(font_size)) / 2;
		if (rtl) {
			draw_string(font, Point2(size.width - ofs - text_limit, v_ofs + font->get_ascent(font_size)), label, HALIGN_RIGHT, text_limit, font_size, color);
		} else {
			draw_string(font, Point2(ofs, v_ofs + font->get_ascent(font_size)), label, HALIGN_LEFT, text_limit, font_size, color);
		}

		if (keying) {
			Ref<Texture2D> key;

			if (use_keying_next()) {
				key = get_theme_icon("KeyNext", "EditorIcons");
			} else {
				key = get_theme_icon("Key", "EditorIcons");
			}

			ofs = size.width - key->get_width() - get_theme_constant("hseparator", "Tree");

			Color color2(1, 1, 1);
			if (keying_hover) {
				color2.r *= 1.2;
				color2.g *= 1.2;
				color2.b *= 1.2;
			}
			keying_rect = Rect2(ofs, ((size.height - key->get_height()) / 2), key->get_width(), key->get_height());
			if (rtl) {
				draw_texture(key, Vector2(size.width - keying_rect.position.x - key->get_width(), keying_rect.position.y), color2);
			} else {
				draw_texture(key, keying_rect.position, color2);
			}

		} else {
			keying_rect = Rect2();
		}

		if (deletable) {
			Ref<Texture2D> close;

			close = get_theme_icon("Close", "EditorIcons");

			ofs = size.width - close->get_width() - get_theme_constant("hseparator", "Tree");

			Color color2(1, 1, 1);
			if (delete_hover) {
				color2.r *= 1.2;
				color2.g *= 1.2;
				color2.b *= 1.2;
			}
			delete_rect = Rect2(ofs, ((size.height - close->get_height()) / 2), close->get_width(), close->get_height());
			if (rtl) {
				draw_texture(close, Vector2(size.width - delete_rect.position.x - close->get_width(), delete_rect.position.y), color2);
			} else {
				draw_texture(close, delete_rect.position, color2);
			}
		} else {
			delete_rect = Rect2();
		}
	}
}

void PropertyEditor::set_label(const String &p_label) {
	label = p_label;
	update();
}

String PropertyEditor::get_label() const {
	return label;
}

Object *PropertyEditor::get_edited_object() {
	return object;
}

StringName PropertyEditor::get_edited_property() {
	return property;
}

void PropertyEditor::update_property() {
	if (get_script_instance()) {
		get_script_instance()->call("update_property");
	}
}

void PropertyEditor::set_read_only(bool p_read_only) {
	read_only = p_read_only;
}

bool PropertyEditor::is_read_only() const {
	return read_only;
}

bool PropertyEditorRevert::may_node_be_in_instance(Node *p_node) {
	Node *edited_scene = EditorNode::get_singleton()->get_edited_scene();

	bool might_be = false;
	Node *node = p_node;

	while (node) {
		if (node == edited_scene) {
			if (node->get_scene_inherited_state().is_valid()) {
				might_be = true;
				break;
			}
			might_be = false;
			break;
		}
		if (node->get_scene_instance_state().is_valid()) {
			might_be = true;
			break;
		}
		node = node->get_owner();
	}

	return might_be; // or might not be
}

bool PropertyEditorRevert::get_instanced_node_original_property(Node *p_node, const StringName &p_prop, Variant &value) {
	Node *node = p_node;
	Node *orig = node;

	Node *edited_scene = EditorNode::get_singleton()->get_edited_scene();

	bool found = false;

	while (node) {
		Ref<SceneState> ss;

		if (node == edited_scene) {
			ss = node->get_scene_inherited_state();

		} else {
			ss = node->get_scene_instance_state();
		}

		if (ss.is_valid()) {
			NodePath np = node->get_path_to(orig);
			int node_idx = ss->find_node_by_path(np);
			if (node_idx >= 0) {
				bool lfound = false;
				Variant lvar;
				lvar = ss->get_property_value(node_idx, p_prop, lfound);
				if (lfound) {
					found = true;
					value = lvar;
				}
			}
		}
		if (node == edited_scene) {
			//just in case
			break;
		}
		node = node->get_owner();
	}

	if (!found && p_node) {
		//if not found, try default class value
		Variant attempt = ClassDB::class_get_default_property_value(p_node->get_class_name(), p_prop);
		if (attempt.get_type() != Variant::NIL) {
			found = true;
			value = attempt;
		}
	}

	return found;
}

bool PropertyEditorRevert::is_node_property_different(Node *p_node, const Variant &p_current, const Variant &p_orig) {
	// this is a pretty difficult function, because a property may not be saved but may have
	// the flag to not save if one or if zero

	//make sure there is an actual state
	{
		Node *node = p_node;
		if (!node) {
			return false;
		}

		Node *edited_scene = EditorNode::get_singleton()->get_edited_scene();
		bool found_state = false;

		while (node) {
			Ref<SceneState> ss;

			if (node == edited_scene) {
				ss = node->get_scene_inherited_state();

			} else {
				ss = node->get_scene_instance_state();
			}

			if (ss.is_valid()) {
				found_state = true;
			}
			if (node == edited_scene) {
				//just in case
				break;
			}
			node = node->get_owner();
		}

		if (!found_state) {
			return false; //pointless to check if we are not comparing against anything.
		}
	}

	if (p_current.get_type() == Variant::FLOAT && p_orig.get_type() == Variant::FLOAT) {
		float a = p_current;
		float b = p_orig;

		return !Math::is_equal_approx(a, b); //this must be done because, as some scenes save as text, there might be a tiny difference in floats due to numerical error
	}

	return bool(Variant::evaluate(Variant::OP_NOT_EQUAL, p_current, p_orig));
}

bool PropertyEditorRevert::can_property_revert(Object *p_object, const StringName &p_property) {
	bool has_revert = false;

	Node *node = Object::cast_to<Node>(p_object);

	if (node && PropertyEditorRevert::may_node_be_in_instance(node)) {
		//check for difference including instantiation
		Variant vorig;
		if (PropertyEditorRevert::get_instanced_node_original_property(node, p_property, vorig)) {
			Variant v = p_object->get(p_property);

			if (PropertyEditorRevert::is_node_property_different(node, v, vorig)) {
				has_revert = true;
			}
		}
	} else {
		//check for difference against default class value instead
		Variant default_value = ClassDB::class_get_default_property_value(p_object->get_class_name(), p_property);
		if (default_value != Variant() && default_value != p_object->get(p_property)) {
			has_revert = true;
		}
	}

	// If the object implements property_can_revert, rely on that completely
	// (i.e. don't then try to revert to default value - the property_get_revert implementation
	// can do that if so desired)
	if (p_object->has_method("property_can_revert")) {
		has_revert = p_object->call("property_can_revert", p_property).operator bool();
	} else {
		if (!has_revert && !p_object->get_script().is_null()) {
			Ref<Script> scr = p_object->get_script();
			if (scr.is_valid()) {
				Variant orig_value;
				if (scr->get_property_default_value(p_property, orig_value)) {
					if (orig_value != p_object->get(p_property)) {
						has_revert = true;
					}
				}
			}
		}
	}

	return has_revert;
}

void PropertyEditor::update_reload_status() {
	if (property == StringName()) {
		return; //no property, so nothing to do
	}

	bool has_reload = PropertyEditorRevert::can_property_revert(object, property);

	if (has_reload != can_revert) {
		can_revert = has_reload;
		update();
	}
}

bool PropertyEditor::use_keying_next() const {
	List<PropertyInfo> plist;
	object->get_property_list(&plist, true);

	for (List<PropertyInfo>::Element *I = plist.front(); I; I = I->next()) {
		PropertyInfo &p = I->get();

		if (p.name == property) {
			return (p.usage & PROPERTY_USAGE_KEYING_INCREMENTS);
		}
	}

	return false;
}

void PropertyEditor::set_checkable(bool p_checkable) {
	checkable = p_checkable;
	update();
	queue_sort();
}

bool PropertyEditor::is_checkable() const {
	return checkable;
}

void PropertyEditor::set_checked(bool p_checked) {
	checked = p_checked;
	update();
}

bool PropertyEditor::is_checked() const {
	return checked;
}

void PropertyEditor::set_draw_red(bool p_draw_red) {
	draw_red = p_draw_red;
	update();
}

void PropertyEditor::set_keying(bool p_keying) {
	keying = p_keying;
	update();
	queue_sort();
}

void PropertyEditor::set_deletable(bool p_deletable) {
	deletable = p_deletable;
	update();
	queue_sort();
}

bool PropertyEditor::is_deletable() const {
	return deletable;
}

bool PropertyEditor::is_keying() const {
	return keying;
}

bool PropertyEditor::is_draw_red() const {
	return draw_red;
}

void PropertyEditor::_focusable_focused(int p_index) {
	if (!selectable) {
		return;
	}
	bool already_selected = selected;
	selected = true;
	selected_focusable = p_index;
	update();
	if (!already_selected && selected) {
		emit_signal("selected", property, selected_focusable);
	}
}

void PropertyEditor::add_focusable(Control *p_control) {
	p_control->connect("focus_entered", callable_mp(this, &PropertyEditor::_focusable_focused), varray(focusables.size()));
	focusables.push_back(p_control);
}

void PropertyEditor::select(int p_focusable) {
	bool already_selected = selected;

	if (p_focusable >= 0) {
		ERR_FAIL_INDEX(p_focusable, focusables.size());
		focusables[p_focusable]->grab_focus();
	} else {
		selected = true;
		update();
	}

	if (!already_selected && selected) {
		emit_signal("selected", property, selected_focusable);
	}
}

void PropertyEditor::deselect() {
	selected = false;
	selected_focusable = -1;
	update();
}

bool PropertyEditor::is_selected() const {
	return selected;
}

void PropertyEditor::_gui_input(const Ref<InputEvent> &p_event) {
	if (property == StringName()) {
		return;
	}

	Ref<InputEventMouse> me = p_event;

	if (me.is_valid()) {
		Vector2 mpos = me->get_position();
		if (is_layout_rtl()) {
			mpos.x = get_size().x - mpos.x;
		}
		bool button_left = me->get_button_mask() & BUTTON_MASK_LEFT;

		bool new_keying_hover = keying_rect.has_point(mpos) && !button_left;
		if (new_keying_hover != keying_hover) {
			keying_hover = new_keying_hover;
			update();
		}

		bool new_delete_hover = delete_rect.has_point(mpos) && !button_left;
		if (new_delete_hover != delete_hover) {
			delete_hover = new_delete_hover;
			update();
		}

		bool new_revert_hover = revert_rect.has_point(mpos) && !button_left;
		if (new_revert_hover != revert_hover) {
			revert_hover = new_revert_hover;
			update();
		}

		bool new_check_hover = check_rect.has_point(mpos) && !button_left;
		if (new_check_hover != check_hover) {
			check_hover = new_check_hover;
			update();
		}
	}

	Ref<InputEventMouseButton> mb = p_event;

	if (mb.is_valid() && mb->is_pressed() && mb->get_button_index() == BUTTON_LEFT) {
		Vector2 mpos = mb->get_position();
		if (is_layout_rtl()) {
			mpos.x = get_size().x - mpos.x;
		}

		if (!selected && selectable) {
			selected = true;
			emit_signal("selected", property, -1);
			update();
		}

		if (keying_rect.has_point(mpos)) {
			emit_signal("property_keyed", property, use_keying_next());

			if (use_keying_next()) {
				if (property == "frame_coords" && (object->is_class("Sprite2D") || object->is_class("Sprite3D"))) {
					Vector2 new_coords = object->get(property);
					new_coords.x++;
					if (new_coords.x >= object->get("hframes").operator int64_t()) {
						new_coords.x = 0;
						new_coords.y++;
					}

					call_deferred("emit_changed", property, new_coords, "", false);
				} else {
					call_deferred("emit_changed", property, object->get(property).operator int64_t() + 1, "", false);
				}

				call_deferred("update_property");
			}
		}
		if (delete_rect.has_point(mpos)) {
			emit_signal("property_deleted", property);
		}

		if (revert_rect.has_point(mpos)) {
			Variant vorig;

			Node *node = Object::cast_to<Node>(object);
			if (node && PropertyEditorRevert::may_node_be_in_instance(node) && PropertyEditorRevert::get_instanced_node_original_property(node, property, vorig)) {
				emit_changed(property, vorig.duplicate(true));
				update_property();
				return;
			}

			if (object->call("property_can_revert", property).operator bool()) {
				Variant rev = object->call("property_get_revert", property);
				emit_changed(property, rev);
				update_property();
				return;
			}

			if (!object->get_script().is_null()) {
				Ref<Script> scr = object->get_script();
				if (scr.is_valid()) {
					Variant orig_value;
					if (scr->get_property_default_value(property, orig_value)) {
						emit_changed(property, orig_value);
						update_property();
						return;
					}
				}
			}

			Variant default_value = ClassDB::class_get_default_property_value(object->get_class_name(), property);
			if (default_value != Variant()) {
				emit_changed(property, default_value);
				update_property();
				return;
			}
		}
		if (check_rect.has_point(mpos)) {
			checked = !checked;
			update();
			emit_signal("property_checked", property, checked);
		}
	}
}

void PropertyEditor::set_label_reference(Control *p_control) {
	label_reference = p_control;
}

void PropertyEditor::set_bottom_editor(Control *p_control) {
	bottom_editor = p_control;
}

Variant PropertyEditor::get_drag_data(const Point2 &p_point) {
	if (property == StringName()) {
		return Variant();
	}

	Dictionary dp;
	dp["type"] = "obj_property";
	dp["object"] = object;
	dp["property"] = property;
	dp["value"] = object->get(property);

	Label *label = memnew(Label);
	label->set_text(property);
	set_drag_preview(label);
	return dp;
}

void PropertyEditor::set_use_folding(bool p_use_folding) {
	use_folding = p_use_folding;
}

bool PropertyEditor::is_using_folding() const {
	return use_folding;
}

void PropertyEditor::expand_all_folding() {
}

void PropertyEditor::collapse_all_folding() {
}

void PropertyEditor::set_selectable(bool p_selectable) {
	selectable = p_selectable;
}

bool PropertyEditor::is_selectable() const {
	return selectable;
}

void PropertyEditor::set_name_split_ratio(float p_ratio) {
	split_ratio = p_ratio;
}

float PropertyEditor::get_name_split_ratio() const {
	return split_ratio;
}

void PropertyEditor::set_app_scale(float p_scale) {
	app_scale = p_scale;
}

float PropertyEditor::get_app_scale() const {
	return app_scale;
}

void PropertyEditor::set_object_and_property(Object *p_object, const StringName &p_property) {
	object = p_object;
	property = p_property;
}

Control *PropertyEditor::make_custom_tooltip(const String &p_text) const {
	tooltip_text = p_text;
	EditorHelpBit *help_bit = memnew(EditorHelpBit);
	//help_bit->add_theme_style_override("panel", get_theme_stylebox("panel", "TooltipPanel"));
	help_bit->get_rich_text()->set_fixed_size_to_width(360 * app_scale);

	String text;
	PackedStringArray slices = p_text.split("::", false);
	if (!slices.is_empty()) {
		String property_name = slices[0].strip_edges();
		text = TTR("Property:") + " [u][b]" + property_name + "[/b][/u]";

		if (slices.size() > 1) {
			String property_doc = slices[1].strip_edges();
			if (property_name != property_doc) {
				text += "\n" + property_doc;
			}
		}
		help_bit->set_text(text);
	}

	return help_bit;
}

String PropertyEditor::get_tooltip_text() const {
	return tooltip_text;
}

void PropertyEditor::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_label", "text"), &PropertyEditor::set_label);
	ClassDB::bind_method(D_METHOD("get_label"), &PropertyEditor::get_label);

	ClassDB::bind_method(D_METHOD("set_read_only", "read_only"), &PropertyEditor::set_read_only);
	ClassDB::bind_method(D_METHOD("is_read_only"), &PropertyEditor::is_read_only);

	ClassDB::bind_method(D_METHOD("set_checkable", "checkable"), &PropertyEditor::set_checkable);
	ClassDB::bind_method(D_METHOD("is_checkable"), &PropertyEditor::is_checkable);

	ClassDB::bind_method(D_METHOD("set_checked", "checked"), &PropertyEditor::set_checked);
	ClassDB::bind_method(D_METHOD("is_checked"), &PropertyEditor::is_checked);

	ClassDB::bind_method(D_METHOD("set_draw_red", "draw_red"), &PropertyEditor::set_draw_red);
	ClassDB::bind_method(D_METHOD("is_draw_red"), &PropertyEditor::is_draw_red);

	ClassDB::bind_method(D_METHOD("set_keying", "keying"), &PropertyEditor::set_keying);
	ClassDB::bind_method(D_METHOD("is_keying"), &PropertyEditor::is_keying);

	ClassDB::bind_method(D_METHOD("set_deletable", "deletable"), &PropertyEditor::set_deletable);
	ClassDB::bind_method(D_METHOD("is_deletable"), &PropertyEditor::is_deletable);

	ClassDB::bind_method(D_METHOD("get_edited_property"), &PropertyEditor::get_edited_property);
	ClassDB::bind_method(D_METHOD("get_edited_object"), &PropertyEditor::get_edited_object);

	ClassDB::bind_method(D_METHOD("_gui_input"), &PropertyEditor::_gui_input);

	ClassDB::bind_method(D_METHOD("get_tooltip_text"), &PropertyEditor::get_tooltip_text);

	ClassDB::bind_method(D_METHOD("add_focusable", "control"), &PropertyEditor::add_focusable);
	ClassDB::bind_method(D_METHOD("set_bottom_editor", "editor"), &PropertyEditor::set_bottom_editor);

	ClassDB::bind_method(D_METHOD("emit_changed", "property", "value", "field", "changing"), &PropertyEditor::emit_changed, DEFVAL(StringName()), DEFVAL(false));

	ADD_PROPERTY(PropertyInfo(Variant::STRING, "label"), "set_label", "get_label");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "read_only"), "set_read_only", "is_read_only");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "checkable"), "set_checkable", "is_checkable");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "checked"), "set_checked", "is_checked");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "draw_red"), "set_draw_red", "is_draw_red");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "keying"), "set_keying", "is_keying");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "deletable"), "set_deletable", "is_deletable");
	ADD_SIGNAL(MethodInfo("property_changed", PropertyInfo(Variant::STRING_NAME, "property"), PropertyInfo(Variant::NIL, "value", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NIL_IS_VARIANT)));
	ADD_SIGNAL(MethodInfo("multiple_properties_changed", PropertyInfo(Variant::PACKED_STRING_ARRAY, "properties"), PropertyInfo(Variant::ARRAY, "value")));
	ADD_SIGNAL(MethodInfo("property_keyed", PropertyInfo(Variant::STRING_NAME, "property")));
	ADD_SIGNAL(MethodInfo("property_deleted", PropertyInfo(Variant::STRING_NAME, "property")));
	ADD_SIGNAL(MethodInfo("property_keyed_with_value", PropertyInfo(Variant::STRING_NAME, "property"), PropertyInfo(Variant::NIL, "value", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NIL_IS_VARIANT)));
	ADD_SIGNAL(MethodInfo("property_checked", PropertyInfo(Variant::STRING_NAME, "property"), PropertyInfo(Variant::STRING, "bool")));
	ADD_SIGNAL(MethodInfo("resource_selected", PropertyInfo(Variant::STRING, "path"), PropertyInfo(Variant::OBJECT, "resource", PROPERTY_HINT_RESOURCE_TYPE, "Resource")));
	ADD_SIGNAL(MethodInfo("object_id_selected", PropertyInfo(Variant::STRING_NAME, "property"), PropertyInfo(Variant::INT, "id")));
	ADD_SIGNAL(MethodInfo("selected", PropertyInfo(Variant::STRING, "path"), PropertyInfo(Variant::INT, "focusable_idx")));

	MethodInfo vm;
	vm.name = "update_property";
	BIND_VMETHOD(vm);
}

PropertyEditor::PropertyEditor() {
	draw_top_bg = true;
	object = nullptr;
	split_ratio = 0.5;
	selectable = true;
	text_size = 0;
	read_only = false;
	checkable = false;
	checked = false;
	draw_red = false;
	keying = false;
	deletable = false;
	keying_hover = false;
	revert_hover = false;
	check_hover = false;
	can_revert = false;
	use_folding = false;
	property_usage = 0;
	selected = false;
	selected_focusable = -1;
	label_reference = nullptr;
	bottom_editor = nullptr;
	delete_hover = false;
}
