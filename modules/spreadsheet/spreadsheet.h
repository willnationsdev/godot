/*************************************************************************/
/*  spreadsheet.h                                                        */
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

// If `uses_titles` is true, columns are identified by a name. Else by index (rows will all be Arrays).
// If `uses_keys` is true, rows are indexed by a name from the first column value. Else by index. If true, current sheet's rows simulated as properties.
// If `row_script` is non-null, row values are supplied as a script instead of a Dictionary (column names must share a name with the script's properties).
// Add getters/setters for rows/columns.

#ifndef SPREADSHEET_H
#define SPREADSHEET_H

#include "core/math/expression.h"
#include "core/resource.h"
#include "core/script_language.h"
#include "core/vector.h"

class Spreadsheet : public Resource {
	GDCLASS(Spreadsheet, Resource);

public:
	struct Cell {
		Variant data;
		String text;
	};

	enum SheetDelimeter {
		SHEET_DELIM_COMMA,
		SHEET_DELIM_SEMI_COLON,
		SHEET_DELIM_TAB,
	};

	typedef Vector<Vector<Cell> > Cells;

	struct Worksheet {
		Cells cells;
		StringName name;

		Worksheet(const StringName &p_name = StringName()) :
				name(p_name) {}
	};

private:
	List<Worksheet> sheets;
	HashMap<StringName, List<Worksheet>::Element *> sheet_map;
	int current_sheet_id;
	bool uses_titles;
	bool uses_keys;
	Ref<Script> row_script;
	SheetDelimeter io_delimeter;
	Expression expr;
	HashMap<StringName, Vector<Cell> *> row_keys;
	HashMap<StringName, int> title_indexes;

	List<Worksheet>::Element *_list_get(List<Worksheet> *p_list, int p_idx);
	const List<Worksheet>::Element *_list_get_const(const List<Worksheet> *p_list, int p_idx) const;
	const String _get_delim() const;

	Worksheet *get_sheet();
	const Worksheet *get_sheet_const() const;

	String _get_key(const Vector<Cell> &p_row) const;
	void _update_title_indexes();
	void _update_row_keys();

protected:
	static void _bind_methods();
	bool _set(const StringName &p_name, const Variant &p_value);
	bool _get(const StringName &p_name, Variant &r_ret) const;
	void _get_property_list(List<PropertyInfo> *p_list) const;

	Array _get_sheet_names() const;

	Array _sheet_get_cells() const;

public:
	void set_sheet_index(int p_idx);
	void add_sheet(const StringName &p_name = StringName());
	void insert_sheet(const StringName &p_name, int p_idx);
	void move_sheet(int p_idx, int p_destination);
	void remove_sheet(int p_idx);
	void get_sheet_names(List<StringName> *r_list) const;

	int sheet_get_index() const;
	StringName sheet_get_name() const;
	void sheet_set_name(const StringName &p_name);
	const Cells *sheet_get_cells() const;
	void sheet_resize(int p_rows, int p_cols);

	Variant sheet_cell_get_data(int p_row, int p_col) const;
	void sheet_cell_set_data(int p_row, int p_col, Variant p_data);
	String sheet_cell_get_text(int p_row, int p_col) const;

	bool get_uses_titles() const;
	void set_uses_titles(bool p_uses_titles);
	bool get_uses_keys() const;
	void set_uses_keys(bool p_uses_keys);
	Ref<Script> get_row_script() const;
	void set_row_script(const Ref<Script> &p_row_script);
	SheetDelimeter get_io_delimeter() const;
	void set_io_delimeter(SheetDelimeter p_io_delimeter);

	Error load_csv(const String &p_filepath);
	Error save_csv(const String &p_filepath);

	Spreadsheet();
};

VARIANT_ENUM_CAST(Spreadsheet::SheetDelimeter);

#endif // SPREADSHEET_H
