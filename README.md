# SolidEarthFlexing
_created by: MPZinke_ <br/>
C++ translated version of solid.f by Dennis Milbert https://geodesyworld.github.io/SOFTS/solid.htm

## About

---

## Use

### Compiling

```bash
make
```

### Running

```bash
./SolidEarthFlexing
```
Or with input file
```bash
./SolidEarthFlexing < input.txt	
```

### Testing
```bash
git checkout Testing
make test
./Testing/SolidEarthFlexing < ./Testing/TestInput.txt
```

## Details

### `solid.f`

#### Function Calls

```txt
MAIN
|  geoxyz
|  civmjd
|  mjdciv
|  setjd0
|  sunxyz
|  |  utc2ttt
|  |  |  utc2tai
|  |  |  tai2tt
|  |  getghar
|  |  rot3
|  moonxyz
|  |  utc2ttt
|  |  |  utc2tai
|  |  |  tai2tt
|  |  rot1
|  |  getghar
|  |  rot3
|  detide
|  |  utc2ttt
|  |  |  utc2tai
|  |  |  tai2tt
|  |  sprod
|  |  sprod
|  |  zero_vec8
|  |  st1idiu
|  |  |  enorm8
|  |  |  enorm8
|  |  |  enorm8
|  |  st1isem
|  |  |  enorm8
|  |  |  enorm8
|  |  |  enorm8
|  |  st1l1
|  |  |  enorm8
|  |  |  enorm8
|  |  |  enorm8
|  |  step2diu
|  |  step2lon
|  rge
|  mjdciv

```
