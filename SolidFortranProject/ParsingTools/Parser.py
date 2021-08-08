



import re;


FILENAME = "../solid.f";


# Variables
TOKEN_REGEX = r"[a-zA-Z_][a-zA-Z0-9_]*"
VARIABLE_DECLARE_OR_ASSIGNMENT_REGEX = r"      [ \t]*"+TOKEN_REGEX+r"[ \t]*(\([a-zA-Z]?[0-9]*\))?[ \t]*="
# VARIABLE_DECLARE_OR_ASSIGNMENT_REGEX = r"      [ \t]*"+TOKEN_REGEX+r"[ \t]*=";

PARAM_REGEX = r"[a-zA-Z0-9 \t,_]*";

# Function & subroutine
## Declarations
SUBROUTINE_REGEX = r"      subroutine[ \t]*"+TOKEN_REGEX+r"[ \t]*\("+PARAM_REGEX+"\)";
FUNCTION_REGEX = r"      double precision function[ \t]*"+TOKEN_REGEX+r"[ \t]*\("+PARAM_REGEX+r"\)";

SUBROUTINE_FUNCTION_REGEX = r"      (double precision function|subroutine)[ \t]*"+TOKEN_REGEX+r"[ \t]*\("+PARAM_REGEX+r"\)";
## Calls
SUBROUTINE_CALL_REGEX = r"      (  )*call[ \t]+"+TOKEN_REGEX+r"[ \t]*\(";
POSSIBLE_FUNCTION_CALL = TOKEN_REGEX+r"[ \t]*\(";
FUNCTION_CALL_LINE_START = r"(      [ \t]*"+TOKEN_REGEX+r".*=|     \*[ \t]*[\+\-\*/]*[ \t]*"+TOKEN_REGEX+r")";



# —————————————————————————————————————————————————————  UTILITY ————————————————————————————————————————————————————— #

def is_symbol(token):
	return token == "if" or token == "elseif" or token == "while" or token == "do" or token == "read" \
		or token == "write";


# —————————————————————————————————————————————————————  CLASSES ————————————————————————————————————————————————————— #

class Call:
	def __init__(self, name, line):
		self.block = None;
		self.name = name;
		self.line = line;


	def assign_block(self, blocks):
		for block in blocks:
			if(block.is_complex and block.name == self.name):
				self.block = block;
				break;  # why waste the clock cycles



class Block:
	SIMPLE = 0;
	SUBROUTINE = 1;
	FUNCTION = 2;
	# —————————————————————————————————————————————————  CONSTRUCTOR ————————————————————————————————————————————————— #

	def __init__(self, line_number, lines):
		self.is_complex = isinstance(self, ComplexBlock);
		self.line_number = line_number;
		self.lines = lines;
		self.calls = [];
		self.variables = [];
		self.get_variables();
		self.get_calls();


	def assign_calls_blocks(self, blocks):
		for call in self.calls:
			call.assign_block(blocks);


	def get_calls(self):
		for x in range(len(self.lines)):
			line = self.lines[x];
			# if is subroutine call
			if(re.match(SUBROUTINE_CALL_REGEX, line)):
				name = line[line.find("call")+4: line.find('(')].strip();
				self.calls.append(Call(name, x));
			# if is function call
			elif(re.match(FUNCTION_CALL_LINE_START, line)):
				matches = re.findall(POSSIBLE_FUNCTION_CALL, line);
				for x in range(len(matches)): matches[x] = matches[x][:len(matches[x])-1].strip();  # convert to name
				self.calls += [Call(name, x) for name in matches if(self.is_not_var(name) and not is_symbol(name))];


	def get_variables(self):
		for line in self.lines:
			# defined variables at top EG. double precision xsta(3),xsun(3),xmon(3),dxtide(3),xcorsta(3)
			if(line[:23] == "      double precision "):
				declared_vars = re.findall(TOKEN_REGEX, line[23:])
				self.variables += [var for var in declared_vars if var not in self.variables] if declared_vars else [];
			# declared variables  EG. scsun=scs/rsta/rsun
			elif(re.match(VARIABLE_DECLARE_OR_ASSIGNMENT_REGEX, line)):
				variable = line[:line.find('=')].strip();
				if(variable not in self.variables): self.variables.append(variable);


	# —————————————————————————————————————————————————— CONVERSION —————————————————————————————————————————————————— #

	def __str__(self):
		return ""


	def print_calls(self, recurrsion=0):
		print("{}{}".format('|  ' * (recurrsion), self.name if self.is_complex else "MAIN"));
		for call in self.calls: 
			if(call.block): call.block.print_calls(recurrsion+1);


	def print_variables(self):
		for variable in self.variables: print(variable);


	# —————————————————————————————————————————————————— CONVERSION —————————————————————————————————————————————————— #

	def is_not_var(self, value):
		return value not in self.variables;



class ComplexBlock(Block):

	# —————————————————————————————————————————————————  CONSTRUCTOR ————————————————————————————————————————————————— #

	def __init__(self, line_number, lines, offset):
		Block.__init__(self, line_number, lines);
		self.offset = offset;
		self.name = None;
		self.params = None;


	# Get block's name & parameters
	def get_signature_info(self):
		first_line = self.lines[0];
		name_span = re.match(r"[ \t]*[a-zA-Z0-9_]+", first_line[self.offset:]).span();
		self.name = first_line[name_span[0]+self.offset: name_span[1]+self.offset].strip();

		param_string = first_line[first_line.find('('): first_line.find(')')];
		param_search = re.findall(r"[a-zA-Z0-9_]+", param_string);
		self.params = param_search if param_search else [];
		self.variables += [param for param in self.params if param not in self.variables];


	def get_changed_params(self):
		for line in lines:
			if(re.match()): pass


	def __str__(self):
		return 	"NAME: {}\n".format(self.name) \
				+ "PARAMS: \n{}\n".format("\n\t".join(self.params)) \
				+ "LINES: \n{}\n".format("".join(self.lines));



class Subroutine(ComplexBlock):
	def __init__(self, line_number, lines):
		ComplexBlock.__init__(self, line_number, lines, 16);
		self.get_signature_info();



class Function(ComplexBlock):
	def __init__(self, line_number, lines):
		ComplexBlock.__init__(self, line_number, lines, 31);
		self.get_signature_info();


# —————————————————————————————————————————————————————  CLASSES ————————————————————————————————————————————————————— #

# Converts lines of file into code blocks for storing block information
def parse_code_blocks(lines):
	blocks = []
	# determine subroutines and functions from lines of code
	block_start = 0;
	block_type = Block.SIMPLE;
	block_constructors = [Block, Subroutine, Function];
	for index in range(len(lines)):
		if(index == len(lines)-1):
			blocks.append(block_constructors[block_type](block_start, lines[block_start: index+1]));

		if(re.match(SUBROUTINE_FUNCTION_REGEX, lines[index])):
			blocks.append(block_constructors[block_type](block_start, lines[block_start: index+1]));
			block_start = index;
			block_type = Block.SUBROUTINE if re.match(SUBROUTINE_REGEX, lines[index]) else Block.FUNCTION;

	for block in blocks: block.assign_calls_blocks(blocks);
	return blocks;



# Opens file and reads contents into list of lines.
def get_file_contents(filename):
	with open(filename, "r") as file:
		return file.readlines();


def print_block_calls(blocks):
	blocks[0].print_calls();


def print_block_names(blocks):
	for block in blocks:
		if(block.is_complex): print(block.name, "Function" if isinstance(block, Function) else "Subroutine");


def print_block_signatures(blocks):
	for block in blocks:
		if(block.is_complex): 
			block_type = "Function" if isinstance(block, Function) else "Subroutine"
			info = "{}\n\tType: {}\n\tParams: {}".format(block.name, block_type, ", ".join(block.params));
			print(info);


def main():
	lines = get_file_contents(FILENAME);
	blocks = parse_code_blocks(lines);

	block = blocks[1];
	print(block.name if(block.is_complex) else "Main", end="\n------------------\n")
	# print("--------- VARS ---------");
	# block.print_variables();
	# print("--------- CALLS ---------");
	# block.print_calls();
	print_block_calls(blocks);
	# print_block_names(blocks);
	print_block_signatures(blocks);


main();
