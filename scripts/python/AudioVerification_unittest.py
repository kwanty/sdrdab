#!/usr/bin/python3
# -*- coding: utf-8 -*-
#
# Authors
# (c) Szymon Bar
# Krakow, Poland
# 29.05.2017
# szbar@student.agh.edu.pl
#
# (c) Mateusz Ziarko
# Krakow, Poland
# 29.05.2017
# mat.ziarko@gmail.com

import sys
import os
import os.path
import difflib

if len(sys.argv) < 3:
	print('Usage: ./AudioVerification_unittest.py <reference> <output_from_sdrdab-cli>')
	sys.exit(1)
elif len(sys.argv) > 3:
	sys.exit(2)

if not os.path.isfile(sys.argv[1]) or not os.path.isfile(sys.argv[2]):
	print('One file does not exist, maybe both...')
	sys.exit(3)

command1 = 'xxd ' + sys.argv[1] + ' | cut -c 11-49 > ref.txt'
command2 = 'xxd ' + sys.argv[2] + ' | cut -c 11-49 > out.txt'

os.system(command1)
os.system(command2)

fh_m = open('ref.txt')
fh_c = open('out.txt')

data_m = fh_m.read()
data_c = fh_c.read()

fh_m.close()
fh_c.close()


data_m_lines = data_m.splitlines()
data_c_lines = data_c.splitlines()

s = difflib.SequenceMatcher(lambda x: x == " ", data_c_lines, data_m_lines)

print()
for opcode in s.get_opcodes():
	if opcode[0] == 'insert':
		print('Reference lines {0} - {1} are missing in the output file'.format(opcode[3] + 1, opcode[4] + 1))
		print('\tThey should be placed in line no {0} of the output file'.format(opcode[1] + 1))
		print()
	elif opcode[0] == 'equal':
		print('Reference lines {0} - {1} match output lines {2} - {3}'.format(opcode[3] + 1, opcode[4] + 1, opcode[1] + 1, opcode[2] + 1))
		print('\tCalculated offset is {0}'.format(opcode[4] - opcode[2]))
		print()
	elif opcode[0] == 'delete':
		print('Output lines {0} - {1} are extra comparing to the reference file'.format(opcode[1] + 1, opcode[2] + 1))
		print()

print('The measure of similarity is {0:3.5f}%'.format(s.ratio()*100))

# uncomment if you want to have "raw" output
'''
for opcode in s.get_opcodes():
	print ("%6s a[%d:%d] b[%d:%d]" % opcode)
	# 'a' is C++ code, 'b' is Matlab code
'''
