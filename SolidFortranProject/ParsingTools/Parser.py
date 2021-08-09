#!/usr/bin/env python3
# -*- coding: utf-8 -*-
__author__ = "MPZinke"

########################################################################################################################
#                                                                                                                      #
#   created by: MPZinke                                                                                                #
#   on 2021.08.07                                                                                                      #
#                                                                                                                      #
#   DESCRIPTION:                                                                                                       #
#   BUGS:                                                                                                              #
#   FUTURE:                                                                                                            #
#                                                                                                                      #
########################################################################################################################


import re;


FILENAME = "../solid.f";


# Variables
PARAM_REGEX = r"[a-zA-Z0-9,_ \t\.]*";  # get parameters from between parentheses
TOKEN_REGEX = r"[a-zA-Z_][a-zA-Z0-9_]*"  # standard token (C, Fortran, ASCII python, etc)
# VARIABLE_ASSIGNMENT_REGEX: Token with a possible array index (in this case, the author only uses i as an index).
# Use case uses declaration context VARIABLE_DECLARATION_REGEX to determine whether it is a function or an array.
VARIABLE_ASSIGNMENT_REGEX = r"      [ \t]*"+TOKEN_REGEX+r"[ \t]*(\([a-zA-Z]?[0-9]*\))?[ \t]*=";
VARIABLE_DECLARATION_REGEX = r"(      dimension |      logical |      double precision )"

# Function & subroutine
## Declarations
SUBROUTINE_REGEX = r"      subroutine[ \t]*"+TOKEN_REGEX+r"[ \t]*\("+PARAM_REGEX+r"\)";
FUNCTION_REGEX = r"      double precision function[ \t]*"+TOKEN_REGEX+r"[ \t]*\("+PARAM_REGEX+r"\)";
SUBROUTINE_OR_FUNCTION_REGEX = r"      (double precision function|subroutine)[ \t]*" \
	+TOKEN_REGEX+r"[ \t]*\("+PARAM_REGEX+r"\)";

## Calls
SUBROUTINE_CALL_REGEX = r"      (  )*call[ \t]+"+TOKEN_REGEX+r"[ \t]*\(";
FUNCTION_CALL = TOKEN_REGEX+r"[ \t]*\("+PARAM_REGEX+r"\)";
FUNCTION_CALL_LINE_START = r"(      [ \t]*"+TOKEN_REGEX+r".*=|     \*[ \t]*[\+\-\*/]*[ \t]*"+TOKEN_REGEX+r")";



# —————————————————————————————————————————————————————  UTILITY ————————————————————————————————————————————————————— #

# Adds amount to each value in a list.
# Takes the lsit to be altered, the amount to increase each value by.
# Returns altered list.
def add(lst: list, amount: int) -> list:
	return [val + amount for val in lst];


# Determine whether a token is a symbol.
# Takes the token string.
def is_keyword(token: str) -> bool:
	return token == "if" or token == "elseif" or token == "while" or token == "do" or token == "read" \
		or token == "write";


# Removes Fortran comments from a line.
# Takes the line to be cleaned.
# Searches for Fortran line comment. If found, it removes it.
# Returns substring of line without trailing comment.
def strip_comment(line: str) -> str:
	if("!***" in line): return line[:line.find("!***")]
	return line;


# SUGAR
# Gets the substring from a string using a list parameter.
# Takes the string to get substring from, list of start and end points.
# Uses standard substring method
def substr(string: str, span: list) -> str:
	return string[span[0]: span[1]];


# —————————————————————————————————————————————————————  CLASSES ————————————————————————————————————————————————————— #

# Used to store data about a function/subroutine call.
class Call:
	def __init__(self, name, line):
		self.block = None;
		self.name = name;
		self.line = line;


	# Gets the block for the call if it exists.
	# Takes the list of blocks to choose from.
	# Iterates blocks, looking for block name
	def assign_block(self, blocks):
		for block in blocks:
			if(block.is_complex and block.name == self.name):
				self.block = block;
				break;  # why waste the clock cycles



# Used to store data about a code block (ComplexBlock inherits from here).
class Block:
	SIMPLE = 0;
	SUBROUTINE = 1;
	FUNCTION = 2;
	# —————————————————————————————————————————————————  CONSTRUCTOR ————————————————————————————————————————————————— #

	def __init__(self, line_number, lines):
		self.is_complex = isinstance(self, ComplexBlock);
		self.line_number = line_number;
		self.lines = lines;
		self.name = "MAIN";  # block is neither function nor subroutine, so it must be MAIN
		self.calls = [];
		self.variables = [];
		self.get_variables();
		if(not self.is_complex): self.get_calls();  # not called here if is_complex so that params are added to vars


	# Assigns code blocks to calls based on name.
	# Takes the blocks list to find the matching block from.
	# Iterates calls of each block and calls the method to find and assign the block to the call.
	def assign_calls_blocks(self, blocks):
		for call in self.calls: call.assign_block(blocks);


	# Finds the function call within the block.
	# Iterates lines. Determines if there is a possible function call within the block. To distinguish 
	def get_calls(self):
		for x in range(len(self.lines)):
			line = strip_comment(self.lines[x]);
			# if is subroutine call
			if(re.match(SUBROUTINE_CALL_REGEX, line)):
				token = line[line.find("call")+4: line.find('(')].strip();
				self.calls.append(Call(token, x));
			# if is function call
			elif(re.match(FUNCTION_CALL_LINE_START, line)):
				matches = re.findall(FUNCTION_CALL, line);
				for x in range(len(matches)): matches[x] = matches[x][:matches[x].find('(')].strip();  # convert token
				self.calls = [Call(token, x) for token in matches if(self.is_not_var(token) and not is_keyword(token))];


	# Gets the variables within a codeblock.
	# Iterates lines. Finds declarations of dimensional & global variables if line specifies any exist.
	# Finds variables being assigned to, if line contains assignments to variables. If the variable is dimensional,
	#  Fortran will have already declared it in function/subroutine header and thus will not have to use context to be
	#  distinguished. Additionally, in Fortran (or at least this program), functions are not assigned on the lefthand.
	def get_variables(self):
		for x in range(len(self.lines)):
			line = strip_comment(self.lines[x]);
			# defined variables at top EG. double precision xsta(3),xsun(3),xmon(3),dxtide(3),xcorsta(3)
			if(re.match(VARIABLE_DECLARATION_REGEX, line)):
				variables = re.findall(TOKEN_REGEX, substr(line, re.match(VARIABLE_DECLARATION_REGEX, line).span()));
				self.variables += [var for var in variables if var not in self.variables] if variables else [];
			# declared variables  EG. scsun=scs/rsta/rsun
			elif(re.match(VARIABLE_ASSIGNMENT_REGEX, line)):
				variable = substr(line, re.search(TOKEN_REGEX, line).span());
				if(variable not in self.variables): self.variables.append(variable);


	# —————————————————————————————————————————————————— CONVERSION —————————————————————————————————————————————————— #

	def __str__(self):
		return ""


	def print_calls(self, recursion=0, signature=False):
		if(signature): self.print_signature(recursion)
		else: print("{}{}".format("|  " * recursion, self.name if self.is_complex else "MAIN"));

		for call in self.calls:
			if(call.block): call.block.print_calls(recursion+1, signature);
			else: print("{}{}".format("|  " * (recursion+1), call.name));


	def print_signature(self, recursion=0):
		print("{}MAIN".format("|  " * recursion))


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
		self.params = [];


	# Get block's name & parameters.
	# Gets the name. Then finds the parameters for the function/subroutine block.
	def get_signature_info(self):
		line = self.lines[0];  # first line
		self.name = substr(line, add(re.match(r"[ \t]*"+TOKEN_REGEX, line[self.offset:]).span(), self.offset)).strip();

		param_string = line[line.find('('): line.find(')')];
		self.params += re.findall(TOKEN_REGEX, param_string);
		self.variables += [param for param in self.params if param not in self.variables];


	def __str__(self):
		return 	"NAME: {}\n".format(self.name) \
				+ "PARAMS: \n{}\n".format("\n\t".join(self.params)) \
				+ "LINES: \n{}\n".format("".join(self.lines));



# Used for storing fortran subroutine block information.
class Subroutine(ComplexBlock):
	def __init__(self, line_number, lines):
		ComplexBlock.__init__(self, line_number, lines, 16);
		self.altered_params = [];  # passed params that are affected in the 	
		self.get_signature_info();
		self.get_calls();
		self.get_changed_params();


	# Finds the memory locations (parameters that affect the arguments put into them outside the subroutine) that are
	#  changed.
	# Looks for lefthand assignment variable that matches the parameter token. Adds token to altered params list.
	def get_changed_params(self):
		for x in range(len(self.lines)):
			line = strip_comment(self.lines[x]);
			match = re.match(VARIABLE_ASSIGNMENT_REGEX, line);
			if(not match): continue;
			# get token from string
			token = substr(line, re.search(TOKEN_REGEX, line).span());
			if(token in self.params and token not in self.altered_params): self.altered_params.append(token);


	def print_signature(self, recursion=0):
		tab = "|  " * recursion;
		args = [tab, self.name, tab, ", ".join(self.params), tab, ", ".join(self.altered_params)];
		print("{}{} [Subroutine]\n{}   Params: {}\n{}   Altered Params: {}".format(*args));



# Used for storing fortran function block information.
class Function(ComplexBlock):
	def __init__(self, line_number, lines):
		ComplexBlock.__init__(self, line_number, lines, 31);
		self.get_signature_info();


	def print_signature(self, recursion=0):
		tab = "|  " * recursion;
		args = [tab, self.name, tab, ", ".join(self.params)];
		print("{}{} [Function]\n{}   Params: {}".format(*args));



# —————————————————————————————————————————————————————  CLASSES ————————————————————————————————————————————————————— #

# Converts lines of file into code blocks for storing block information.
# Takes a list of line strings for the entire program.
# Iterates lines. Checks if line is the start of a new block type (subroutine, function). If it is, it creates a new 
#  block and adds previous code to a new code block (sliding window-esque method).
# Returns list of blocks. 
def parse_code_blocks(lines: list) -> list:
	blocks = []
	# determine subroutines and functions from lines of code
	block_start = 0;
	block_type = Block.SIMPLE;
	block_constructors = [Block, Subroutine, Function];
	for index in range(len(lines)):
		# end of program: store current block contents as a block
		if(index == len(lines)-1):
			blocks.append(block_constructors[block_type](block_start, lines[block_start: index+1]));

		if(re.match(SUBROUTINE_OR_FUNCTION_REGEX, lines[index])):
			blocks.append(block_constructors[block_type](block_start, lines[block_start: index+1]));
			block_start = index;
			block_type = Block.SUBROUTINE if re.match(SUBROUTINE_REGEX, lines[index]) else Block.FUNCTION;

	for block in blocks: block.assign_calls_blocks(blocks);
	return blocks;



# Opens file and reads contents into list of lines.
def get_file_contents(filename):
	with open(filename, "r") as file:
		return file.readlines();


def print_block_calls(blocks, signature=False):
	blocks[0].print_calls(0, signature);


def print_block_names(blocks):
	for block in blocks:
		if(block.is_complex): print(block.name, "Function" if isinstance(block, Function) else "Subroutine");


def print_block_signatures(blocks):
	for block in blocks:
		if(block.is_complex): 
			block.print_signature();


def main():
	lines = get_file_contents(FILENAME);
	blocks = parse_code_blocks(lines);

	print_block_calls(blocks, True);
	print_block_names(blocks);
	print_block_signatures(blocks);


main();
