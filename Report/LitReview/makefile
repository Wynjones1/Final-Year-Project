all:main.tex
	rubber -d main.tex
	rubber --clean main.tex
	mv main.pdf out.pdf
	pdftotext out.pdf
	cat *.tex > tmp.tex
	wc tmp.tex
	rm tmp.tex

run: all
	nohup evince out.pdf &> /dev/null &

clean:
	rubber -d --clean main.tex
	rm -f out.pdf
