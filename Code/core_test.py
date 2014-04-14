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
	options += "REL=1 "
	if use_bruteforce:
		options += "BRUTEFORCE_TRIANGLE_INTERSECTION=1 "
	subprocess.call(options + " make -j4", shell=True)

	subprocess.call("mv raytracer " + filename, shell=True)

def run(filename, dim, cores, num_samples = 10):
	dim   = str(dim)
	cores = str(cores)
	command = "/usr/bin/time -f \"%e\" "
	command += filename
	command += " -w " + dim + " -h " + dim 
	command += " -i ./data/scenes/dragon.scene"
	command += " -t " + cores
	command += " --samples 40"
	acc = 0.0

	for i in xrange(num_samples):
		p = subprocess.Popen(command, stderr=subprocess.PIPE, shell=True)
		acc += float(p.communicate()[1])
	return acc / num_samples

def main():
	cores = "./cores"

	make(cores, False)

	a = []
	num_threads = 4
	r = xrange(1,num_threads + 1)
	for i in r:
		print(i)
		a.append(run(cores, num_threads, i))

	m = max(a)
	a = [i / m for i in a]
	
	kd_plot, = plot.plot(r, a)
	plot.ylabel("time (s)")
	plot.xlabel("number of threads")
	plot.show()

	subprocess.call("rm " + cores, shell=True)

if __name__ == "__main__":
	main();
