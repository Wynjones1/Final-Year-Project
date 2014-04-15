#!/usr/bin/env python
from matplotlib import pyplot
from random     import uniform
from math import pi

def est(n):
	x = []
	y = []
	m = 0.0
	for i in xrange(0, n):
		x.append(uniform(-1, 1))
		y.append(uniform(-1, 1))
		if(x[-1] ** 2 + y[-1] ** 2 <= 1.0):
			m += 1
	return 4 * m / n

def plot():
	x = []
	y = []
	n = 1000
	m = 0.0
	for i in xrange(0, n):
		x.append(uniform(-1, 1))
		y.append(uniform(-1, 1))

	pyplot.ylim([-1,1])
	pyplot.xlim([-1,1])
	pyplot.axis("equal")
	pyplot.scatter(x, y, marker='+')
	c0 = pyplot.Circle([0,0],radius=1, alpha = 0.2)
	pyplot.gcf().gca().add_artist(c0)
	pyplot.savefig("circle.png")

def conv():
	x = []
	y = []
	n = 4000
	for i in xrange(20, n):
		x.append(i)
		y.append(est(i))

	pyplot.plot(x,y)
	pyplot.savefig("converge.png")

def main():
	#plot()
	conv()
		

if __name__ == "__main__":
	main()
