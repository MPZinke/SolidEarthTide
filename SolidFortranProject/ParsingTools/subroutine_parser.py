

class Subroutine:
	def __init__(self, name, start, parameters):
		self.name = name
		self.start = start
		self.end = 0
		self.parameters = parameters
		self.changed_params = set()
		self.line = self.getlines()

	def getlines(self):
		lines = []
		with open("../solid.f") as file:
			for x, line in enumerate(file):
				if(self.start <= x):
					if("      return" in line): 
						self.end = x
						return lines
					lines.append(line)



def get_lines_from_file():
	with open("../solid.f", "r") as file:
		return file.readlines()


def get_subroutines(lines):
	subroutines = []
	for x, line in enumerate(lines):
		if("      subroutine " in line):
			subroutine_name = line[line.index("subroutine ")+11: line.index("(")]
			subroutines.append(Subroutine(subroutine_name, x, line[line.index("(")+1 : line.index(")")].split(",")))
	return subroutines


def parameter_for_line_if_line_changes_parameter(line, parameters):
	for param in parameters:
		if( '=' in line and param + '=' in line.lstrip()[:line.index('=')+1]):
			return param


def determine_if_parameter_is_changed_in_subroutine(subroutine):
	with open("../solid.f", "r") as file:
		for x, line in enumerate(file):
			if(subroutine.start <= x):
				altered_param = parameter_for_line_if_line_changes_parameter(line, subroutine.parameters)
				if altered_param: subroutine.changed_params.add(altered_param)
			if(subroutine.end < x): 
				return subroutine


def print_param_altering_subroutines(subroutines):
	for sub in subroutines:
		if not sub.changed_params: continue
		print(sub.name, '('+", ".join(sub.parameters)+')')
		print("ALTERED:", ', '.join(sub.changed_params))
		print()


def print_all_subroutines(subroutines):
	for sub in subroutines:
		print(sub.start)
		print(sub.name, '('+", ".join(sub.parameters)+')')
		print()


def main():
	lines = get_lines_from_file()
	subroutines = get_subroutines(lines)
	sorted_subroutines = [determine_if_parameter_is_changed_in_subroutine(subroutine) for subroutine in subroutines]
	print("——————— SUBROUTINES ———————")
	print_all_subroutines(subroutines)
	print("———————— CHANGES ————————")
	print_param_altering_subroutines(subroutines)


if __name__ == '__main__':
	main()