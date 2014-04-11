#!/usr/bin/env python
import matplotlib.pyplot as plt

def main():
	fp = open("out.txt")
	lines = [line.strip() for line in fp]
	#Read in the resolution
	resolution = lines[0].split()
	x = int(resolution[0])
	y = int(resolution[1])
	#Read in the thread times
	data = {}
	for i in lines[1:]:
		thread, time = [float(i) for i in i.split()]
		thread = int(thread)
		try:
			data[int(thread)].append(time)
		except KeyError:
			data[int(thread)] = [time]

	d = []
	keys = data.keys()
	for i in keys:
		d.append(sum(data[i]) / len(data[i]))

	plt.plot(d)
	plt.show()

if __name__ == "__main__":
	main()
