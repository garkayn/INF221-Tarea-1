# Mini-informe Tarea 1

## Descripción

Este directorio contiene mi mini-informe en **LaTeX** para INF-221. Las tablas
y afirmaciones se basan en los CSV generados por los programas de `code/`, y
las figuras se incluyen desde los directorios `data/plots` correspondientes.

## Compilación

```bash
pdflatex report.tex
bibtex report
pdflatex report.tex
pdflatex report.tex
```

También se puede usar `latexmk -pdf report.tex` si la instalación está
configurada para ejecutar BibTeX. Debe regenerarse primero la evidencia con
`make benchmark` desde `../code`. En Linux, el flujo completo es:

```bash
cd ../code
python3 -m pip install -r requirements.txt
make benchmark
cd ../report
latexmk -pdf report.tex
```

```bash
.
├── author.tex
├── preamble.tex
├── README.md
├── references.bib
├── report.pdf
├── report.tex
└── sections
    ├── conclusions.tex
    ├── experiment_intro.tex
    └── experiment_results.tex

2 directories, 9 files

```

## Entrega

La entrega se realiza vía **aula.usm.cl** en formato `.zip`.
