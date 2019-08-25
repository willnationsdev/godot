/*************************************************************************/
/*  spreadsheet.cpp                                                      */
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

#include "spreadsheet.h"

#include "core/os/file_access.h"

List<Spreadsheet::Worksheet>::Element *Spreadsheet::_list_get(List<Worksheet> *p_list, int p_idx) {
	ERR_FAIL_COND_V(!p_list, NULL);
	ERR_FAIL_INDEX_V(p_idx, p_list->size(), NULL);
	List<Worksheet>::Element *E = p_list->front();
	for (int i = 0; E && i < p_idx; E = E->next()) {
	}
	return E;
}

const List<Spreadsheet::Worksheet>::Element *Spreadsheet::_list_get_const(const List<Worksheet> *p_list, int p_idx) const {
	ERR_FAIL_COND_V(!p_list, NULL);
	ERR_FAIL_INDEX_V(p_idx, p_list->size(), NULL);
	const List<Worksheet>::Element *E = p_list->front();
	for (int i = 0; E && i < p_idx; E = E->next()) {
	}
	return E;
}

const String Spreadsheet::_get_delim() const {
	String delim;
	switch (io_delimeter) {
		case SHEET_DELIM_SEMI_COLON:
			delim = ";";
		case SHEET_DELIM_TAB:
			delim = "\t";
		case SHEET_DELIM_COMMA:
		default:
			delim = ",";
	}
	return delim;
}

Spreadsheet::Worksheet *Spreadsheet::get_sheet() {
	Worksheet &sheet = sheets[current_sheet_id];
	return &sheet;
}

const Spreadsheet::Worksheet *Spreadsheet::get_sheet_const() const {
	const Worksheet &sheet = sheets[current_sheet_id];
	return &sheet;
}

String Spreadsheet::_get_key(const Vector<Cell> &p_row) const {
	if (p_row.size())
		return p_row[0].data;
	return "";
}

void Spreadsheet::_update_title_indexes() {
	const Worksheet *sheet = get_sheet_const();
	ERR_FAIL_COND(!sheet);
	ERR_FAIL_COND(sheet->cells.size() <= 0);
	title_indexes.clear();
	for (int i = 0; i < sheet->cells[0].size(); i++) {
		title_indexes[sheet->cells[0][i].data] = i;
	}
}

void Spreadsheet::_update_row_keys() {
	Worksheet *sheet = get_sheet();
	ERR_FAIL_COND(!sheet);
	Cells &cells = sheet->cells;
	row_keys.clear();
	for (int i = 0; i < cells.size(); ++i) {
		Vector<Cell> &cell_row = cells.write[i];
		String key;
		if (get_script_instance() && get_script_instance()->has_method("_get_key")) {
			Array row;
			for (int j = 0; j < cell_row.size(); ++j) {
				row.push_back(cells[i][j].data);
			}
			key = get_script_instance()->call("_get_key", row);
		}
		key = _get_key(cells[i]);

		row_keys[key] = &cell_row;
	}
}

bool Spreadsheet::_set(const StringName &p_name, const Variant &p_value) {
	// "<row>/<col>/data"

	String n = p_name;
	if (n.get_slice_count("/") == 2) {
		Vector<String> sections = n.split("/");
		String row_id = sections[0];
		String col_id = sections[1];

		Vector<Cell> *cell_row;
		if (get_uses_keys()) {
			// "name/<col>/data"

			if (!row_keys.has(row_id))
				return false;
			cell_row = row_keys[row_id];
			ERR_FAIL_COND_V(!cell_row, false);

			int index;
			if (get_uses_titles()) {
				if (!title_indexes.has(col_id))
					return false;
				index = title_indexes[col_id];
			} else {
				index = String::to_int(col_id.c_str());
				ERR_FAIL_INDEX_V(index, cell_row->size(), false);
			}
			cell_row->write[index].data = p_value;
			return true;
		} else {
			// "#/<col>/data"
			if (!row_id.is_numeric())
				return false;
			Worksheet *sheet = get_sheet();
			ERR_FAIL_COND_V(!sheet, false);
			Cells &cells = sheet->cells;
			int row_id_int = String::to_int(row_id.c_str());
			ERR_FAIL_INDEX_V(row_id_int, cells.size(), false);
			cell_row = &cells.write[row_id_int];
		}
		if (!cell_row)
			return false;
	}

	int slash = n.find("/");
	if (slash == -1)
		return false;
	String row_id = "";

	if (get_uses_keys()) {
		if (row_keys.has(p_name)) {
			if (get_uses_titles()) {

			} else {
			}
		}
	}

	return false;
}

bool Spreadsheet::_get(const StringName &p_name, Variant &r_ret) const {
	return false;
}

void Spreadsheet::_get_property_list(List<PropertyInfo> *p_list) const {
}

Array Spreadsheet::_get_sheet_names() const {
	List<StringName> names;
	get_sheet_names(&names);
	Array ret;
	for (List<StringName>::Element *E = names.front(); E; E = E->next()) {
		ret.push_back(E->get());
	}
	return ret;
}

Array Spreadsheet::_sheet_get_cells() const {
	const Worksheet *sheet = get_sheet_const();
	ERR_FAIL_COND_V(!sheet, Array());

	const Cells &cells = sheet->cells;

	Array ret;
	for (int i = 0; i < cells.size(); ++i) {
		for (int j = 0; j < cells.size(); ++j) {
			ret.push_back(cells[i][j].data);
		}
	}
	return ret;
}

void Spreadsheet::set_sheet_index(int p_idx) {
	ERR_FAIL_INDEX_MSG(p_idx, sheets.size(), "Worksheet " + itos(p_idx) + " does not exist.");
	current_sheet_id = p_idx;
}

void Spreadsheet::add_sheet(const StringName &p_name) {
	StringName name = p_name == StringName() ? StringName("Sheet" + itos(sheets.size() + 1)) : p_name;
	Worksheet sheet(p_name);
	sheets.push_back(sheet);
}

void Spreadsheet::insert_sheet(const StringName &p_name, int p_idx) {
	add_sheet(p_name);
	List<Worksheet>::Element *sheet_before = _list_get(&sheets, p_idx);
	ERR_FAIL_COND(!sheet_before);
	sheets.move_before(sheets.back(), sheet_before);
}

void Spreadsheet::move_sheet(int p_idx, int p_destination) {
	List<Worksheet>::Element *sheet_to_move = _list_get(&sheets, p_idx);
	List<Worksheet>::Element *sheet_before = _list_get(&sheets, p_destination);
	ERR_FAIL_COND(!sheet_to_move || !sheet_before);
	sheets.move_before(sheet_to_move, sheet_before);
}

void Spreadsheet::remove_sheet(int p_idx) {
	List<Worksheet>::Element *sheet_to_remove = _list_get(&sheets, p_idx);
	ERR_FAIL_COND(!sheet_to_remove);
	sheets.erase(sheet_to_remove);
}

void Spreadsheet::get_sheet_names(List<StringName> *r_list) const {
	sheet_map.get_key_list(r_list);
}

int Spreadsheet::sheet_get_index() const {
	return current_sheet_id;
}

StringName Spreadsheet::sheet_get_name() const {
	const List<Worksheet>::Element *E = _list_get_const(&sheets, current_sheet_id);
	ERR_FAIL_COND_V(!E, StringName());
	const Worksheet &sheet = E->get();
	return sheet.name;
}

void Spreadsheet::sheet_set_name(const StringName &p_name) {
	List<Worksheet>::Element *sheet_to_rename = _list_get(&sheets, current_sheet_id);
	ERR_FAIL_COND(!sheet_to_rename);
	Worksheet &sheet = sheet_to_rename->get();
	sheet_map.erase(sheet.name);
	sheet.name = p_name;
	sheet_map[sheet.name] = sheet_to_rename;
}

const Spreadsheet::Cells *Spreadsheet::sheet_get_cells() const {
	const Worksheet *sheet = get_sheet_const();
	ERR_FAIL_COND_V(!sheet, NULL);
	return &sheet->cells;
}

void Spreadsheet::sheet_resize(int p_rows, int p_cols) {
	List<Worksheet>::Element *E = _list_get(&sheets, current_sheet_id);
	ERR_FAIL_COND(!E);
	Worksheet &sheet = E->get();
	sheet.cells.resize(p_rows);
	for (int i = 0; i < sheet.cells.size(); ++i) {
		sheet.cells.write[i].resize(p_cols);
	}
}

Variant Spreadsheet::sheet_cell_get_data(int p_row, int p_col) const {
	const Worksheet *sheet = get_sheet_const();
	ERR_FAIL_COND_V(!sheet, Variant());
	ERR_FAIL_INDEX_V_MSG(p_row, sheet->cells.size(), Variant(), "Failed to find row " + itos(p_row) + " in the current worksheet.");
	ERR_FAIL_INDEX_V_MSG(p_col, sheet->cells[p_row].size(), Variant(), "Failed to find column " + itos(p_col) + " in row " + itos(p_row) + " of the current worksheet.");
	return sheet->cells[p_row][p_col].data;
}

void Spreadsheet::sheet_cell_set_data(int p_row, int p_col, Variant p_data) {
	Worksheet *sheet = get_sheet();
	ERR_FAIL_COND(!sheet);
	ERR_FAIL_INDEX_MSG(p_row, sheet->cells.size(), "Failed to find row " + itos(p_row) + " in the current worksheet.");
	ERR_FAIL_INDEX_MSG(p_col, sheet->cells[p_row].size(), "Failed to find column " + itos(p_col) + " in row " + itos(p_row) + " of the current worksheet.");
	sheet->cells.write[p_row].write[p_col].data = p_data;
}

String Spreadsheet::sheet_cell_get_text(int p_row, int p_col) const {
	const Worksheet *sheet = get_sheet_const();
	ERR_FAIL_COND_V(!sheet, Variant());
	ERR_FAIL_INDEX_V_MSG(p_row, sheet->cells.size(), Variant(), "Failed to find row " + itos(p_row) + " in the current worksheet.");
	ERR_FAIL_INDEX_V_MSG(p_col, sheet->cells[p_row].size(), Variant(), "Failed to find column " + itos(p_col) + " in row " + itos(p_row) + " of the current worksheet.");
	return sheet->cells[p_row][p_col].text;
}

bool Spreadsheet::get_uses_titles() const {
	return uses_titles;
}

void Spreadsheet::set_uses_titles(bool p_uses_titles) {
	uses_titles = p_uses_titles;
}

bool Spreadsheet::get_uses_keys() const {
	return uses_keys;
}

void Spreadsheet::set_uses_keys(bool p_uses_keys) {
	uses_keys = p_uses_keys;
}

Ref<Script> Spreadsheet::get_row_script() const {
	return row_script;
}

void Spreadsheet::set_row_script(const Ref<Script> &p_row_script) {
	row_script = p_row_script;
}

Spreadsheet::SheetDelimeter Spreadsheet::get_io_delimeter() const {
	return io_delimeter;
}

void Spreadsheet::set_io_delimeter(SheetDelimeter p_io_delimeter) {
	io_delimeter = p_io_delimeter;
}

Error Spreadsheet::load_csv(const String &p_filepath) {
	Error err;
	FileAccess *fa = FileAccess::open(p_filepath, FileAccess::READ, &err);
	if (err != OK)
		return err;

	String delim = _get_delim();

	Vector<String> line;
	line = fa->get_csv_line(delim);
	if (!line.size())
		return OK;

	Worksheet *sheet = get_sheet();
	ERR_FAIL_COND_V(!sheet, ERR_DOES_NOT_EXIST);
	Cells &cells = sheet->cells;

	// update cells row by line
	for (int i = 0; i < line.size(); ++i) {
		const String &s = line[i];
		title_indexes[s] = i;
		cells.write[0].write[i].text = s;
		cells.write[0].write[i].data = s;
	}

	{
		int i = 1;
		line = fa->get_csv_line(delim);

		while (line.size()) {
			cells.write[i].resize(line.size());

			// update cells row by line
			for (int j = 0; j < line.size(); ++j) {
				const String &s = line[j];
				cells.write[i].write[j].text = s;
				cells.write[i].write[j].data = s;
			}

			line = fa->get_csv_line(delim);
		}
	}

	return OK;
}

Error Spreadsheet::save_csv(const String &p_filepath) {
	return OK;
}

void Spreadsheet::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_uses_titles"), &Spreadsheet::get_uses_titles);
	ClassDB::bind_method(D_METHOD("set_uses_titles", "enable"), &Spreadsheet::set_uses_titles);
	ClassDB::bind_method(D_METHOD("get_uses_keys"), &Spreadsheet::get_uses_keys);
	ClassDB::bind_method(D_METHOD("set_uses_keys", "enable"), &Spreadsheet::set_uses_keys);
	ClassDB::bind_method(D_METHOD("get_row_script"), &Spreadsheet::get_row_script);
	ClassDB::bind_method(D_METHOD("set_row_script", "script"), &Spreadsheet::set_row_script);
	ClassDB::bind_method(D_METHOD("get_io_delimeter"), &Spreadsheet::get_io_delimeter);
	ClassDB::bind_method(D_METHOD("set_io_delimeter", "delim"), &Spreadsheet::set_io_delimeter);

	ClassDB::bind_method(D_METHOD("set_sheet_index", "index"), &Spreadsheet::set_sheet_index);
	ClassDB::bind_method(D_METHOD("add_sheet", "name"), &Spreadsheet::add_sheet);
	ClassDB::bind_method(D_METHOD("insert_sheet", "name", "index"), &Spreadsheet::insert_sheet);
	ClassDB::bind_method(D_METHOD("move_sheet", "index", "destination"), &Spreadsheet::move_sheet);
	ClassDB::bind_method(D_METHOD("remove_sheet", "index"), &Spreadsheet::remove_sheet);
	ClassDB::bind_method(D_METHOD("get_sheet_names"), &Spreadsheet::_get_sheet_names);

	ClassDB::bind_method(D_METHOD("sheet_get_index"), &Spreadsheet::sheet_get_index);
	ClassDB::bind_method(D_METHOD("sheet_get_name"), &Spreadsheet::sheet_get_name);
	ClassDB::bind_method(D_METHOD("sheet_set_name", "name"), &Spreadsheet::sheet_set_name);
	ClassDB::bind_method(D_METHOD("sheet_get_cells"), &Spreadsheet::_sheet_get_cells);
	ClassDB::bind_method(D_METHOD("sheet_resize"), &Spreadsheet::sheet_resize);

	ClassDB::bind_method(D_METHOD("sheet_cell_get_data"), &Spreadsheet::sheet_cell_get_data);
	ClassDB::bind_method(D_METHOD("sheet_cell_set_data"), &Spreadsheet::sheet_cell_set_data);
	ClassDB::bind_method(D_METHOD("sheet_cell_get_text"), &Spreadsheet::sheet_cell_get_text);

	BIND_ENUM_CONSTANT(SHEET_DELIM_COMMA);
	BIND_ENUM_CONSTANT(SHEET_DELIM_SEMI_COLON);
	BIND_ENUM_CONSTANT(SHEET_DELIM_TAB);

	ADD_GROUP("Options", "");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "uses_titles"), "set_uses_titles", "get_uses_titles");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "uses_keys"), "set_uses_keys", "get_uses_keys");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "row_script", PROPERTY_HINT_RESOURCE_TYPE, "Script", PROPERTY_USAGE_DEFAULT, "Script"), "set_row_script", "get_row_script");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "io_delimeter", PROPERTY_HINT_ENUM, "Comma,Semi-Colon,Tab", PROPERTY_USAGE_DEFAULT), "set_io_delimeter", "get_io_delimeter");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "current_sheet"), "set_sheet_index", "sheet_get_index");
	ADD_GROUP("Sheet", "sheet_");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "sheet_name"), "sheet_set_name", "sheet_get_name");
}

Spreadsheet::Spreadsheet() {
	current_sheet_id = 0;
	Worksheet sheet("Sheet1");
	sheets.push_back(sheet);
	uses_titles = true;
	uses_keys = false;
}
