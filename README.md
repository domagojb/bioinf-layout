# bioinf-layout

### To compile project
```
cmake CMakeList.txt
make
```

### Run
```
./layout <overlaps> <reads> <output>
```
* overlaps - path to overlap file in **PAF** format
* reads - path to reads file in **FASTA** format
* output - file to output unitigs to in **FASTA** format

### Test example with lambda genome

```
./layout test-data/lambda_overlaps.paf test-data/lambda_reads.fasta test-data/lambda_unitig.fasta
```
