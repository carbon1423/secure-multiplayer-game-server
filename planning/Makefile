# Makefile for compiling LaTeX documents in the planning/ folder

TEXFILES := $(wildcard *.tex)
PDFFILES := $(TEXFILES:.tex=.pdf)

LATEX = pdflatex
FLAGS = -interaction=nonstopmode

.PHONY: all clean

# Compile all .tex files into .pdf
all: $(PDFFILES)

%.pdf: %.tex
	$(LATEX) $(FLAGS) $<

# Remove all auxiliary files
clean:
	rm -f *.aux *.log *.out *.toc *.lof *.lot *.nav *.snm *.fls *.fdb_latexmk *.synctex.gz
