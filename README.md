# bioinf-layout

### To compile project
```
cd src/
cmake CMakeList.txt
make
```

### Run
```
./layout <overlaps> <reads> <output>
```

### Test example with lambda genome

```
./layout ../test-data/lambda_overlaps.paf ../test-data/lambda_reads.fasta ../test-data/lambda_unitig.fasta
```
