#!/usr/bin/env python

import os, subprocess
import matplotlib.pyplot as plot
import numpy

def make(filename, use_bruteforce):
	subprocess.call("make clean", shell=True)

	options = ""
	options += "PMAP=0 "
	options += "GUI=0 "
	options += "VERBOSE=0 "
	if use_bruteforce:
		options += "BRUTEFORCE_TRIANGLE_INTERSECTION=1 "
	subprocess.call(options + " make -j4", shell=True)

	subprocess.call("mv raytracer " + filename, shell=True)

def run(filename, dim):
	dim = str(dim)
	command = filename + " -w " + dim + " -h " + dim + " -i ./data/scenes/dragon.scene"
	command = "/usr/bin/time -f \"%e\" " + command
	p = subprocess.Popen(command, stderr=subprocess.PIPE, shell=True)
	return float(p.communicate()[1])

def main():
	bf = "./bf"
	kd = "./kd"

#	make(bf, True)
	make(kd, False)

	a = []
	b = []
	r = xrange(1,250)
	for i in r:
		print(i)
#		a.append(run(bf, i))
		b.append(run(kd, i))

#	bf_plot, = plot.plot(r,a)
	kd_plot, = plot.plot(r,b)
#	plot.legend([bf_plot, kd_plot], ["bruteforce", "k-d tree"])
	plot.ylabel("time (s)")
	plot.xlabel("resolution n x n")
	plot.show()

	subprocess.call("rm " + bf + " " + kd, shell=True)

if __name__ == "__main__":
	main();
