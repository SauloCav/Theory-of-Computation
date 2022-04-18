#include <iostream>
#include <string.h>

struct STATE_DESC;
struct STATE_RULE;
struct TAPE_DESC;

enum STATE_CONTROL
{
	NORMAL = 0,START = 1, END = 2
};

struct STATE_RULE
{	
	STATE_CONTROL rule_type;
	int switch_on_char;
	int move_tape_dir; 
	int write_new_char;
	int new_state_id;
	STATE_DESC* parent_state_desc;

	 void print_step(){
		std::cout << "ON:" << switch_on_char << "->" << write_new_char << ",MOVE:" << move_tape_dir << " GOTO:"<< new_state_id << std::endl;
	}
};

struct STATE_DESC
{
	int state_id;
	STATE_RULE* state_rules;
	unsigned int state_rules_count;

	void destroy_state_rules() {
		delete state_rules;
		state_rules = NULL;
		state_rules_count = 0;
	}
};

struct TAPE_DESC
{
	unsigned int tape_size;
	int* tape_cells;
	unsigned int current_head_position;
	unsigned int max_reached_tape_position;
	void create_tape() {
		if (tape_size == 0) {
			tape_size = 128;
		}
		current_head_position = 0;
		tape_cells = new int[tape_size];
		if (tape_cells == nullptr) { throw; }
		for (size_t i = 0; i < tape_size; i++)
		{
			tape_cells[i] = -1;
		}
		max_reached_tape_position = 1;
	}

	void destroy_tape() {
		delete tape_cells;
		tape_cells = NULL;
	}

	void move_tape(int _move_by) {
		current_head_position += _move_by;
		if (current_head_position < 0) {
			current_head_position = 0;
			throw;

		}
		else if (current_head_position > tape_size - 1) {
			current_head_position = tape_size - 1;
			throw;
		}
		if (max_reached_tape_position < current_head_position) {
			max_reached_tape_position = current_head_position;
		}
	}

	void print_tape() {
		for (size_t i = 0; i < max_reached_tape_position+1 ; i++)
		{
			if (max_reached_tape_position + 1 > tape_size) {
				break;
			}
			if (tape_cells[i] == -1) {
				std::cout << "#";
			}
			else {
				std::cout << tape_cells[i];
			}
			
		}
		std::cout <<	std::endl;
	}
};

STATE_DESC* states;
TAPE_DESC tape;

const char* turing_add_one_program = "tape_size,32\n"
"start_head_pos,3\n"
"start_rule,0\n"
"tape_conf,1,1,2,3\n"
"rule_set,0,0,1,-1,1,1\n" 
"rule_set,0,1,0,-1,0,0\n"
"rule_set,0,-1,1,0,2,-1\n"
"rule_set,1,0,0,-1,0,1\n"
"rule_set,1,1,1,-1,0,1\n"
"rule_set,1,-1,-1,1,2,0\n";

int main()
{
	if (turing_add_one_program == nullptr) {
		throw;
	}
	const char* start_needle = NULL;
	const char* end_needle = NULL;

	int read_tape_size = 1024;
	start_needle = strstr(turing_add_one_program, "tape_size,");
	if (start_needle == nullptr) {
		read_tape_size = 1024;
	}
	else {
		start_needle += strlen("tape_size,");
		end_needle = strstr(start_needle, "\n");
		std::string tape_size = "";
		tape_size.append(start_needle, end_needle);
		read_tape_size = atoi(tape_size.c_str());
	}

	start_needle = NULL;
	end_needle = NULL;
	int read_start_head_pos = 0;
	start_needle = strstr(turing_add_one_program, "start_head_pos,");
	if (start_needle == nullptr) {
		read_start_head_pos = 0;
	}
	else {
		start_needle += strlen("start_head_pos,");
		end_needle = strstr(start_needle, "\n");
		std::string start_head_pos = "";
		start_head_pos.append(start_needle, end_needle);
		read_start_head_pos = atoi(start_head_pos.c_str());
	}

	start_needle = NULL;
	end_needle = NULL;
	int read_start_rule = -1;
	start_needle = strstr(turing_add_one_program, "start_rule,");
	if (start_needle == nullptr) {
		read_start_rule = -1;
	}
	else {
		start_needle += strlen("start_rule,");
		end_needle = strstr(start_needle, "\n");
		std::string start_rule = "";
		start_rule.append(start_needle, end_needle);
		read_start_rule = atoi(start_rule.c_str());
	}

	tape.tape_size = read_tape_size;
	tape.create_tape();
	tape.current_head_position = read_start_head_pos;

	if (tape.current_head_position + 1 > tape.max_reached_tape_position) {
		tape.max_reached_tape_position = tape.current_head_position + 1;
	}

	start_needle = strstr(turing_add_one_program, "tape_conf,");
	const char* start_needle_file = start_needle;
	const char* end_needle_line = strstr(start_needle, "\n");
	std::string tpcong_line = "";
	tpcong_line.append(start_needle, end_needle_line);
	start_needle = strstr(tpcong_line.c_str(), "tape_conf,");
	end_needle = NULL;
	bool was_char_pos = false;
	std::string tmp_string = "";
	int* selected_char = NULL;
	while (start_needle_file != nullptr)
	{
		if (!was_char_pos) {
			start_needle += strlen("tape_conf,");
			was_char_pos = true;
			end_needle = strstr(start_needle, ",");
		
			tmp_string = "";
			tmp_string.append(start_needle, end_needle);
			selected_char = new int();

			*selected_char = atoi(tmp_string.c_str()); 
			start_needle += tmp_string.size() + strlen(",");
			
		}
		else if(was_char_pos && selected_char != nullptr){
			if (start_needle == nullptr || strcmp(start_needle, "\n") == 0) {

				start_needle = strstr(end_needle_line, "tape_conf,");
				if (start_needle == nullptr) { break; }
				end_needle_line = strstr(start_needle, "\n");
				tpcong_line = "";
				tpcong_line.append(start_needle, end_needle_line);
				start_needle = strstr(tpcong_line.c_str(), "tape_conf,");
				was_char_pos = false;
				tmp_string = "";
				selected_char = NULL;
				end_needle = NULL;
				continue;
			}
			else {
				tmp_string = "";
				end_needle = strstr(start_needle, ",");
				if (end_needle == nullptr) {
					tmp_string.append(start_needle);
					tape.tape_cells[atoi(tmp_string.c_str())] = *selected_char; 

					start_needle = strstr(end_needle_line, "tape_conf,");
					if (start_needle == nullptr) { break; }
					end_needle_line = strstr(start_needle, "\n");
					tpcong_line = "";
					tpcong_line.append(start_needle, end_needle_line);
					start_needle = strstr(tpcong_line.c_str(), "tape_conf,");
					was_char_pos = false;
					tmp_string = "";
					selected_char = NULL;
					end_needle = NULL;
					continue;
				}
				else {
					tmp_string = "";
					tmp_string.append(start_needle,end_needle);
					tape.tape_cells[atoi(tmp_string.c_str())] = *selected_char;
					start_needle += tmp_string.size() + strlen(",");
				}
			}
		}
		else {

			throw;
		}
	}

	const char* rule_set_counter = strstr(turing_add_one_program, "rule_set,");
	
	int tileset_array_size = 0;
	while (true) {
		rule_set_counter = strstr(rule_set_counter, "rule_set,");
		if (rule_set_counter != 0) {
			rule_set_counter += strlen("rule_set,"); 
			tileset_array_size++;
		}
		else {
			break;
		}
	}

	int* found_id_count = new int[tileset_array_size];
	std::string* found_rule_sets = new std::string[tileset_array_size];
	for (size_t i = 0; i < tileset_array_size; i++)
	{
		found_id_count[i] = 0;
	}

	rule_set_counter = strstr(turing_add_one_program, "rule_set,");
	const char* rule_set_id_seperator = NULL;
	const char* rule_set_id_line = NULL;
	int std_counter = 0;
	std::string tmp_id = "";
	int tileset_array_size_id = 0;
	while (true) {
		rule_set_counter = strstr(rule_set_counter, "rule_set,");
		if (rule_set_counter != 0) {
			rule_set_counter += strlen("rule_set,");
			rule_set_id_seperator = strstr(rule_set_counter, ",");
			rule_set_id_seperator++;
			found_rule_sets[std_counter] = "";
			rule_set_id_line  = strstr(rule_set_id_seperator, "\n");
			found_rule_sets[std_counter].append(rule_set_counter, rule_set_id_line);
			tmp_id = "";
			tmp_id.append(rule_set_counter, rule_set_id_seperator);
			found_id_count[atoi(tmp_id.c_str())]++;
			tileset_array_size_id++;
			std_counter++;
		}
		else {
			break;
		}
	}

	int rs_u_id_counter = 0;
	for (size_t i = 0; i < tileset_array_size; i++)
	{
		if (found_id_count[i] > 0) {
			rs_u_id_counter++;
		}
	}
	if (rs_u_id_counter <= 0) {
		throw;
	}

	states = new STATE_DESC[rs_u_id_counter];
	if (states == nullptr) {
		throw;
	}
	STATE_RULE rule;

	std::string tmp_str = "";
	int st_id_counter = 0;

	for (size_t i = 0; i < rs_u_id_counter; i++)
	{

		states[i].state_id = i;
		states[i].state_rules_count = found_id_count[i];
		if (found_id_count[i] <= 0) { continue; }
		states[i].state_rules = new STATE_RULE[states[i].state_rules_count];

		st_id_counter = 0;
		for (size_t j = 0; j <tileset_array_size; j++)
		{
			const char* start_rule_needle = found_rule_sets[j].c_str();
			const char* end_rule_needle = NULL;
			if (start_rule_needle == nullptr) { throw; }

			end_rule_needle = strstr(start_rule_needle, ",");
			tmp_str = "";
			tmp_str.append(start_rule_needle, end_rule_needle);

			if (atoi(tmp_str.c_str()) == states[i].state_id) {

				start_rule_needle = end_rule_needle + 1;
				if (start_rule_needle == nullptr) { throw; }
				end_rule_needle = strstr(start_rule_needle, ",");
				if (end_rule_needle == nullptr) { throw; }
				tmp_id = "";
				tmp_id.append(start_rule_needle, end_rule_needle);
				states[i].state_rules[st_id_counter].switch_on_char = atoi(tmp_id.c_str());

				start_rule_needle = end_rule_needle + 1;
				if (start_rule_needle == nullptr) { throw; }
				end_rule_needle = strstr(start_rule_needle, ",");
				if (end_rule_needle == nullptr) { throw; }
				tmp_id = "";
				tmp_id.append(start_rule_needle, end_rule_needle);
				states[i].state_rules[st_id_counter].write_new_char = atoi(tmp_id.c_str());

				start_rule_needle = end_rule_needle + 1;
				if (start_rule_needle == nullptr) { throw; }
				end_rule_needle = strstr(start_rule_needle, ",");
				if (end_rule_needle == nullptr) { throw; }
				tmp_id = "";
				tmp_id.append(start_rule_needle, end_rule_needle);
				states[i].state_rules[st_id_counter].move_tape_dir = atoi(tmp_id.c_str());

				start_rule_needle = end_rule_needle + 1;
				if (start_rule_needle == nullptr) { throw; }
				end_rule_needle = strstr(start_rule_needle, ",");
				if (end_rule_needle == nullptr) { throw; }
				tmp_id = "";
				tmp_id.append(start_rule_needle, end_rule_needle);
				switch (atoi(tmp_id.c_str()))
				{
				case 0:
					states[i].state_rules[st_id_counter].rule_type = STATE_CONTROL::NORMAL;
					break;
				case 1:
					states[i].state_rules[st_id_counter].rule_type = STATE_CONTROL::START;
					break;
				case 2:
					states[i].state_rules[st_id_counter].rule_type = STATE_CONTROL::END;
					break;
				default:
					states[i].state_rules[st_id_counter].rule_type = STATE_CONTROL::NORMAL;
					break;
				}

				start_rule_needle = end_rule_needle + 1;
				if (start_rule_needle == nullptr) { throw; }
				tmp_id = "";
				tmp_id.append(start_rule_needle);
				states[i].state_rules[st_id_counter].new_state_id = atoi(tmp_id.c_str());

					st_id_counter++; 
			}
			else {
				continue;
			}

			int k = 0;
		}



		int j = 0;
	}


	unsigned int current_state_pos = 0;
	
	current_state_pos = read_start_rule;
	
	STATE_CONTROL last_rule_state_flag = STATE_CONTROL::START;
	
	tape.print_tape();

	while (last_rule_state_flag != STATE_CONTROL::END )
	{
		for (size_t i = 0; i < states[current_state_pos].state_rules_count; i++)
		{
			if (states[current_state_pos].state_rules[i].switch_on_char == tape.tape_cells[tape.current_head_position]) {
				states[current_state_pos].state_rules[i].print_step();
				tape.tape_cells[tape.current_head_position] = states[current_state_pos].state_rules[i].write_new_char;
				tape.move_tape(states[current_state_pos].state_rules[i].move_tape_dir);
				last_rule_state_flag = states[current_state_pos].state_rules[i].rule_type;
				tape.print_tape();
				current_state_pos = states[current_state_pos].state_rules[i].new_state_id; 
				break; 
			}
			
		}
	}

	for (size_t i = 0; i < rs_u_id_counter; i++)
	{
		states[i].destroy_state_rules();
	}
	delete[] states;
	states = NULL;
	tape.destroy_tape();
	delete[] found_id_count;

	char a = 0;
	std::cin >> a;
    return 0;
}
