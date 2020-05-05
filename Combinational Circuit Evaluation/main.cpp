#include<iostream>
#include<fstream>
#include<string>
#include <vector> 
#include <queue>

using namespace std;

/* Not necessarily a gate, could be an input */
class Gate {
	public:
		vector<Gate*> inputs;
		string type;
		int Unknown;
		bool value;
		vector<Gate*> outputs;

		Gate(vector<Gate*> _inputs, string _type) {
			inputs = _inputs;
			type = _type;
			Unknown = _inputs.size();
			value = false;
			for (int i = 0; i < _inputs.size(); i++) { //If no input, then this will not be excuted
				(_inputs[i]->outputs).push_back(this);
			}
		}

		/* Set the output value of a gate, or set the value of the input node */
		void setValue(bool _value) {
			value = _value;
			/* Reduce the unknown degree of its successors */
			for (int i = 0; i < outputs.size(); i++) { //If no output, then this will not be excuted
				outputs[i]->Unknown--;
			}
		}

		/* Calculate the output of a gate */
		void calculate();
};

/* Nodes store all the nodes in the circuit. */
class Nodes_List {
public:
	vector<Gate*> pointers;
	vector<string> names;

	/* Add a node to the circuit */
	void addNode(Gate* gate, string name) {
		pointers.push_back(gate);
		names.push_back(name);
	}

	/* Given the name of the node, find the pointer to it */
	Gate* getPtr(string name) {
		for (int i = 0; i < pointers.size(); i++) {
			if (names[i] == name) {
				return pointers[i];
			}
		}
		cout << "Error: The node being searched doesn't exist in the circuit. Node name:" << name << endl;
		return NULL;
	}

	string getName(Gate* gate) {
		for (int i = 0; i < pointers.size(); i++) {
			if (pointers[i] == gate) {
				return names[i];
			}
		}
		cout << "Error: The Gate pointer being searched doesn't exist in the circuit." << endl;
		return NULL;
	}
};

Nodes_List nodes; //Store all nodes, including inputs, of the circuit
vector<string> circuit_outputs; // Store all the of all the circuit outputs

/* Gates with multiple inputs are calculated recursively. */

bool AND(vector<Gate*> inputs) {
	if (inputs.size() == 1) {
		return inputs[0]->value;
	}
	else {
		Gate* temp = inputs.back();
		inputs.pop_back();
		return temp->value && AND(inputs);
	}
}

bool OR(vector<Gate*> inputs) {
	if (inputs.size() == 1) {
		return inputs[0]->value;
	}
	else {
		Gate* temp = inputs.back();
		inputs.pop_back();
		return temp->value || OR(inputs);
	}
}

bool XOR_inner(bool a, bool b) {
	return (!a && b) || (a && !b);
}
bool XOR(vector<Gate*> inputs) {
	if (inputs.size() == 1) {
		return inputs[0]->value;
	}
	else {
		if (inputs.size() == 2) {
			return XOR_inner(inputs[0]->value, inputs[1]->value);
		}
		else {
			Gate* temp = inputs.back();
			inputs.pop_back();
			return XOR_inner(temp->value, XOR(inputs));
		}
	}
}

bool NXOR_inner(bool a, bool b) {
	return !(!a && b) || (a && !b);
}
bool NXOR(vector<Gate*> inputs) {
	if (inputs.size() == 1) {
		return inputs[0]->value;
	}
	else {
		if (inputs.size() == 2) {
			return NXOR_inner(inputs[0]->value, inputs[1]->value);
		}
		else {
			Gate* temp = inputs.back();
			inputs.pop_back();
			return NXOR_inner(temp->value, NXOR(inputs));
		}
	}
}

/* Calculate the output of a gate */
void Gate::calculate() {
	if (Unknown == 0) {
		/* Calculate the result */
		if (type == "AND") { value = AND(inputs); }
		if (type == "NAND") { value = !AND(inputs); }
		if (type == "OR") { value = OR(inputs); }
		if (type == "NOR") { value = !OR(inputs); }
		if (type == "XOR") { value = XOR(inputs); }
		if (type == "NXOR") { value = NXOR(inputs); }
		if (type == "NOT") { value = !inputs[0]->value; }
		if (type == "BUF") { value = inputs[0]->value; }
	}
	else {
		cout << "Error: Trying to calculate a gate with unknown inputs. Gate name: " << nodes.getName(this) << endl;
	}
}

/* Given a string, transform the string to upper case */

string toUpper(string str1) {
	for (int i = 0; i < str1.size(); i++) {
		if (str1[i] >= 'a' && str1[i] <= 'z') {
			str1[i] -= 32;
		}
	}
	return str1;
}

/* Given a string command, initialize a gate */
void initialGate(string str1) {
	int index = 0;
	string temp;
	string nodename;
	string gatetype;
	vector<Gate*> inputs;

	/* Eliminate spaces */
	if (!str1.empty()) {
		while ((index = str1.find(' ', index)) != string::npos) {
			str1.erase(index, 1);
		}
	}

	index = str1.find("=");
	temp = str1.substr(0, index);
	nodename = temp;
	str1.erase(0, index + 1);

	index = str1.find("(");
	temp = str1.substr(0, index);
	temp = toUpper(temp);
	gatetype = temp;
	str1.erase(0, index + 1);

	while (temp != "") {
		index = str1.find(",") < str1.find(")") ? str1.find(",") : str1.find(")");
		temp = str1.substr(0, index);
		if (temp == "") {
			break;
		}
		else {
			inputs.push_back(nodes.getPtr(temp));
			str1.erase(0, index + 1);
		}
	}

	Gate* newgate = new Gate(inputs,gatetype);
	/* Place the gate into the list if all gates */
	nodes.addNode(newgate, nodename);
}

/* Given a string command, initialize an input or an output */
void initialInOut(string str1) {
	vector<Gate*> blank;
	int index = 0;
	string temp;
	string nodename;
	string gatetype;

	/* Eliminate spaces */
	if (!str1.empty()) {
		while ((index = str1.find(' ', index)) != string::npos) {
			str1.erase(index, 1);
		}
	}

	index = str1.find("(");
	temp = str1.substr(0, index);
	temp = toUpper(temp); //To upper
	gatetype = temp;
	str1.erase(0, index + 1);

	index = str1.find(")");
	temp = str1.substr(0, index);
	nodename = temp;
	str1.erase(0, index + 1);

	if (gatetype == "INPUT") {
		Gate* newgate = new Gate(blank, gatetype);
		/* Place the gate into the list if all gates */
		nodes.addNode(newgate, nodename);
	}
	else {
		circuit_outputs.push_back(nodename); // Set gate as an output. Don't new the gate: it will be declared in the following text!
	}
}

/* Given a string command, set an input value */
void initialValue(string str1) {
	int index = 0;
	string temp;
	string nodename;
	int nodevalue;

	index = str1.find(" ");
	temp = str1.substr(0, index);
	nodename = temp;
	str1.erase(0, index + 1);

	nodevalue = atoi(str1.c_str());

	nodes.getPtr(nodename)->setValue(nodevalue);
}

/* Calculate the outputs of the circuit */
void execute() {
	queue<Gate*> q;
	Gate* current;

	/* Scan all nodes for those nodes with all inputs known */
	for (int i = 0; i < nodes.names.size(); i++) {
		current = nodes.pointers[i];
		if (current->Unknown == 0 && current->type != "INPUT") {
			q.push(current);
		}
	}

	/* Begin topological sorting */
	while (!q.empty()) {
		current = q.front();
		q.pop();
		current->calculate();
		/* Reduce the unknown degree of its successors */
		for (int i = 0; i < current->outputs.size(); i++) { //If no output, then this will not be excuted
			current->outputs[i]->Unknown--;
			if (current->outputs[i]->Unknown == 0) {
				q.push(current->outputs[i]);
			}
		}
	}
}

/* Print the outputs of the circuit */
void printOutput() {
	Gate *res;
	string name;

	/* Scan circuit_output for all the outputs */
	for (int i = 0; i < circuit_outputs.size(); i++) {
		name = circuit_outputs[i];
		res = nodes.getPtr(name);
		/* Begin printing */
		cout << name << " " << res->value << endl;
	}
}

int main(int argc, char *argv[]) {

	ifstream file_bench;
	ifstream file_val;
	string line;
	int index;

	/* Begin reading circuit */
	file_bench.open(argv[1]);
	if (!file_bench) {
		cout << "Error: Failed to open file!" << endl;
	}
	else {
		while (getline(file_bench, line)) {
			if (!line.empty() && line[0] != '#') {
				index = line.find("=");
				if (index == line.npos) {
					initialInOut(line);
				}
				else {
					initialGate(line);
				}
			}
		}
		file_bench.close();
	}

	/* Begin reading values */
	file_val.open(argv[2]);
	if (!file_val) {
		cout << "Error: Failed to open file!" << endl;
	}
	else {
		while (getline(file_val, line)) {
			initialValue(line);
		}
		file_val.close();
	}

	/* Begin calculating */
	execute();
	printOutput();

	/*Test*/

	/* Free memory and close files*/

	for (int i = 0; i < nodes.names.size(); i++) {
		delete nodes.pointers[i];
	}
}