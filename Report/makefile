# This is a makefile. Look up how to use this by running:
# 	$ man make
# to view the manual page on the command make

all: diss.tex
	pdflatex diss.tex &> /dev/null
	pdflatex diss.tex &> /dev/null
	pdflatex diss.tex &> /dev/null

clean:
	rm -f diss.dvi diss.aux diss.bbl diss.log diss.lof diss.lot diss.toc diss.blg diss.pdf diss.out diss_defs.aux

run: all
	evince ./diss.pdf
