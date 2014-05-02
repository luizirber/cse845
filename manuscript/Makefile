all: full_article.tex
	latex full_article
	bibtex full_article
	latex full_article
	latex full_article
	dvipdf full_article

clean:
	-rm full_article.{bbl,blg,dvi,log,out,aux}
	-rm Adaptive*
